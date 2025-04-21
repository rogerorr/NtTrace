/*
NAME
  GetModuleBase.cpp

DESCRIPTION
  Helper for stack walking.

AUTHOR
  Roger Orr mailto:rogero@howzatt.co.uk
  Bug reports, comments, and suggestions are always welcome.

COPYRIGHT
  Copyright (C) 2003 under the MIT license:

  "Permission is hereby granted, free of charge, to any person obtaining a
  copy of this software and associated documentation files (the "Software"),
  to deal in the Software without restriction, including without limitation
  the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
  IN THE SOFTWARE."
*/

#include "GetModuleBase.h"

#include <windows.h>
#pragma warning(push)
#pragma warning(disable : 4091) // 'typedef ': ignored on left of '' when no
                                // variable is declared
#include <dbghelp.h>
#pragma warning(pop)
#include <psapi.h> // GetModuleFileNameEx
#include <string>
#include <vector>

#pragma comment(lib, "psapi")

static char const szRCSID[] =
    "$Id: GetModuleBase.cpp 2676 2025-04-21 17:00:58Z roger $";

namespace {
//////////////////////////////////////////////////////////
// fix for problem with resource leak in symsrv
void fixSymSrv() {
  static bool loaded = false;
  if (!loaded) {
    HMODULE const hSymSrv = ::GetModuleHandle("SymSrv");
    if (hSymSrv != nullptr) {
      ::LoadLibrary("SymSrv");
      loaded = true;
    }
  }
}
} // namespace

/////////////////////////////////////////////////////////////////////////////////////
/// GetModuleBase: try to get information about a module being loaded
///
/// This function is called by the stack walker to load symbols for a new
/// address. NOTES: This function is needed because we are using FALSE for the
/// last parameter of SymInitialize(), which prevents ALL modules being
/// automatically loaded and so dramatically improves the speed of the stack
/// walk We check whether the address is valid first because some invalid
/// addresses cause access violations inside DbgHelp.dll
///
DWORD64 CALLBACK GetModuleBase(HANDLE hProcess, DWORD64 dwAddress) {
  DWORD64 baseAddress = 0;

  MEMORY_BASIC_INFORMATION mbInfo;
  if (::VirtualQueryEx(hProcess, (PVOID)dwAddress, &mbInfo, sizeof(mbInfo)) &&
      ((mbInfo.State & MEM_FREE) == 0)) {
    // It is already in the symbol engine?
    IMAGEHLP_MODULE64 stIHM = {sizeof(IMAGEHLP_MODULE64)};

    if (::SymGetModuleInfo64(hProcess, dwAddress, &stIHM)) {
      baseAddress = stIHM.BaseOfImage;
      // ATLTRACE("showGetModuleBase got addr from SymGetModuleInfo = %x\n",
      // baseAddress);
    } else {
      baseAddress = (DWORD64)mbInfo.AllocationBase;
      HMODULE const hmod = (HMODULE)mbInfo.AllocationBase;

      char szFileName[MAX_PATH] = "";
      DWORD const dwNameLen = GetModuleFileNameWrapper(
          hProcess, hmod, szFileName, sizeof szFileName / sizeof szFileName[0]);

      if (0 != dwNameLen) {
        bool bPathSet(false);
        std::vector<char> searchpath(1024);
        if (::SymGetSearchPath(hProcess, &searchpath[0], 1024)) {
          // symbol files often stored with binary image
          char const *delim = strrchr(szFileName, '\\');
          if (delim) {
            std::string fullpath(szFileName, delim - szFileName);
            fullpath += ";";
            fullpath += &searchpath[0];
            ::SymSetSearchPath(
                hProcess, const_cast<char *>(
                              fullpath.c_str())); // Some versions of DbgHelp.h
                                                  // not const-correct
            bPathSet = true;
          }
        }
        // We do not need to pass a file handle - trapNtCalls reveals that
        // DbgHelp simply opens the file if we don't provide a handle or
        // duplicates the handle if we do.
        if (!::SymLoadModule64(hProcess, nullptr, szFileName, nullptr,
                               baseAddress, 0)) {
          // ATLTRACE("SymLoadModule, failed for %s\n", szFileName);
        }
        fixSymSrv();
        if (bPathSet) {
          ::SymSetSearchPath(hProcess, const_cast<char *>(&searchpath[0]));
        }
      } else {
        // ATLTRACE("Module not found at %X\n", baseAddress);
      }

      // ATLTRACE("GetModuleBase got addr from VirtualQueryEx = %x\n",
      // baseAddress);
    }
  }

  return baseAddress;
}

/**
 * Get module file name, correcting for a couple of common issues.
 *
 * @param hProcess the process to query
 * @param hMod the module to query
 * @param szBuff the output filename buffer
 * @param bufLen the size of the output buffer
 * @returns the length of the string copied to the buffer, or zero on failure,
 * in which case call GetLastError for the underlying error code.
 */
DWORD GetModuleFileNameWrapper(HANDLE hProcess, HMODULE hMod, char *szBuff,
                               DWORD bufLen) {
  DWORD ret = ::GetModuleFileNameEx(hProcess, hMod, szBuff, bufLen);
  if (ret == 0 && hMod == nullptr) {
    DWORD lastError = GetLastError();
    if (lastError == ERROR_PARTIAL_COPY || lastError == ERROR_INVALID_HANDLE) {
      // Use alternate API to get exe name in 64-bit windows.
      using pfnQueryFullProcessImageName = BOOL(WINAPI *)(
          /*__in*/ HANDLE hProcess,
          /*__in*/ DWORD dwFlags,
          /*__out*/ LPCSTR lpExeName,
          /*__inout*/ PDWORD lpdwSize);

      static pfnQueryFullProcessImageName pQueryFullProcessImageName =
          (pfnQueryFullProcessImageName)GetProcAddress(
              GetModuleHandle("KERNEL32"), "QueryFullProcessImageNameA");

      if (pQueryFullProcessImageName &&
          pQueryFullProcessImageName(hProcess, 0, szBuff, &bufLen)) {
        ret = bufLen;
      } else {
        // Windows 2003 only has this one: which returns paths in device form
        using pfnGetProcessImageFileName = DWORD(WINAPI *)(
            IN HANDLE hProcess, OUT LPTSTR lpImageFileName, IN DWORD nSize);

        static pfnGetProcessImageFileName pGetProcessImageFileName =
            (pfnGetProcessImageFileName)GetProcAddress(
                GetModuleHandle("psapi"), "GetProcessImageFileNameA");
        if (pGetProcessImageFileName) {
          ret = pGetProcessImageFileName(hProcess, szBuff, bufLen);
        }
      }
    }
    if (ret == 0) {
      SetLastError(lastError);
    }
  }
  if (ret != 0) {
    if (memcmp(szBuff, "\\??\\", 4) == 0) {
      szBuff[1] = '\\'; // For some reason the wrong UNC header is returned
    }
  }
  return ret;
}
