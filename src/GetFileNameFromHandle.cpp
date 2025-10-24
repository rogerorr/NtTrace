/*
NAME
  GetFileNameFromHandle

DESCRIPTION
  Helper function to try and get a meaningful file name from an opened file
  handle

AUTHOR
  Roger Orr mailto:rogero@howzatt.co.uk
  Bug reports, comments, and suggestions are always welcome.

COPYRIGHT
  Original code � 2010 Microsoft Corporation. All rights reserved.
  From http://msdn.microsoft.com/en-us/library/aa366789%28v=vs.85%29.aspx

  Changes copyright (C) 2011 by Roger Orr mailto:rogero@howzatt.co.uk

  This software is distributed in the hope that it will be useful, but
  without WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  Permission is granted to anyone to make or distribute verbatim
  copies of this software provided that the copyright notice and
  this permission notice are preserved, and that the distributor
  grants the recipient permission for further distribution as permitted
  by this notice.

IMPLEMENTATION NOTES
  There are various forms of 'raw' file names:
    \Device\HarddiskVolume1\temp\xx.txt
    \Device\LanmanRedirector\or2-freda\drivec\temp\xx.txt
    \Device\Mup\or2-freda\drivec\temp\xx.txt
    \Device\WinDfs\P:000000000001703e\or2-freda\drivec\temp\xx.txt

  and of 'dos device names' for drive letters:
    \Device\HarddiskVolume1
    \Device\LanmanRedirector\;Z:000000000001703e\or2-freda\drivec
    \Device\WinDfs\P:000000000001703e

  Network files may be accessed via a UNC path rather than a drive letter.

*/

static char const szRCSID[] =
    "$Id: GetFileNameFromHandle.cpp 2880 2025-10-24 22:03:14Z roger $";

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>

#include <psapi.h>
#include <string.h>
#include <tchar.h>

#pragma comment(lib, "mpr")
#pragma comment(lib, "psapi")

#define BUFSIZE 512

namespace {
static char const lanman[] = "\\Device\\LanmanRedirector";
static char const mup[] = "\\Device\\Mup";
static char const dfs[] = "\\Device\\WinDfs";
static char const dfsUnc[] = "\\Device\\WinDfs\\Root";

/* Check for net use using the provided uncPrefix and target device name and
 * drive name */
bool resolveNetUse(std::string &filename, char const *uncPrefix, size_t uncLen,
                   TCHAR *driveName) {
  bool result(false);
  if ((_tcsnicmp(filename.c_str(), uncPrefix, uncLen) == 0)) {
    char shareBuff[MAX_PATH + 1];
    DWORD shareLen = MAX_PATH;
    if (WNetGetConnection(driveName, shareBuff, &shareLen) == 0) {
      char const *shareName = shareBuff + 1;
      shareLen = static_cast<DWORD>(_tcslen(shareName));
      if (_tcsnicmp(filename.c_str() + uncLen, shareName, shareLen) == 0) {
        if (filename[uncLen + shareLen] == '\\') {
          filename.replace(0, uncLen + shareLen, driveName);
          result = true;
        }
      }
    }
  }
  return result;
}

/* Check for unresolved UNC prefix and replace with a backslash designator */
bool resolveUnc(std::string &filename, char const *uncPrefix, UINT uncLen) {
  bool result = _tcsnicmp(filename.c_str(), uncPrefix, uncLen) == 0;
  if (result) {
    filename.replace(0, uncLen, "\\");
  }
  return result;
}
} // namespace

std::string GetFileNameFromHandle(HANDLE hFile) {
  std::string result;

  // Get the file size.
  DWORD dwFileSizeHi = 0;
  DWORD dwFileSizeLo = GetFileSize(hFile, &dwFileSizeHi);

  if (dwFileSizeLo == INVALID_FILE_SIZE && GetLastError() != NO_ERROR) {
    SetLastError(ERROR_FILE_INVALID);
    return result;
  }

  // Create a file mapping object.
  HANDLE hFileMap =
      CreateFileMapping(hFile, nullptr, PAGE_READONLY, 0, 1, nullptr);

  if (hFileMap) {
    // Create a file mapping to get the file name.
    void *pMem = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 1);

    if (pMem) {
      TCHAR pszFilename[MAX_PATH + 1];
      pszFilename[0] = '\0';
      if (GetMappedFileName(GetCurrentProcess(), pMem, pszFilename, MAX_PATH)) {
        result = pszFilename;
        // Translate path with device name to drive letters.
        TCHAR szTemp[BUFSIZE];
        szTemp[0] = '\0';
        szTemp[1] = '\0';
        TCHAR *pDrives = szTemp;

        if (GetLogicalDriveStrings(BUFSIZE - 1, pDrives)) {
          TCHAR szName[MAX_PATH + 1];
          TCHAR szDrive[3] = TEXT(" :");
          BOOL bFound = FALSE;

          do {
            // Copy the drive letter to the template string
            *szDrive = *pDrives;

            // Look up each device name
            if (QueryDosDevice(szDrive, szName, MAX_PATH)) {
              size_t uNameLen = _tcslen(szName);

              if (uNameLen < MAX_PATH) {
                if (_tcsnicmp(szName, lanman, sizeof(lanman) - 1) == 0) {
                  if (resolveNetUse(result, lanman, sizeof(lanman) - 1,
                                    szDrive) ||
                      resolveNetUse(result, mup, sizeof(mup) - 1, szDrive)) {
                    bFound = true;
                  }
                } else if (_tcsnicmp(szName, dfs, sizeof(dfs) - 1) == 0) {
                  if (resolveNetUse(result, szName, uNameLen, szDrive)) {
                    bFound = true;
                  }
                } else {
                  if ((_tcsnicmp(pszFilename, szName, uNameLen) == 0) &&
                      (result[uNameLen] == _T('\\'))) {
                    result.replace(0, uNameLen, szDrive);
                    bFound = true;
                  }
                }
              }
            }

            // Go to the next NULL character.
            while (*pDrives++)
              ;
          } while (!bFound && *pDrives); // end of string
          if (!bFound) {
            resolveUnc(result, mup, sizeof(mup) - 1) ||
                resolveUnc(result, lanman, sizeof(lanman) - 1) ||
                resolveUnc(result, dfsUnc, sizeof(dfsUnc) - 1);
          }
        }
      }
      UnmapViewOfFile(pMem);
    }

    CloseHandle(hFileMap);
  }
  return result;
}
