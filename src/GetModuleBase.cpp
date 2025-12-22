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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#pragma warning(push)
#pragma warning(disable : 4091) // 'typedef ': ignored on left of '' when no
                                // variable is declared
#include <dbghelp.h>
#pragma warning(pop)
#include <psapi.h> // GetModuleFileNameEx
#include <string>
#include <vector>

#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "psapi")

static char const szRCSID[] =
    "$Id: GetModuleBase.cpp 3012 2025-12-22 08:38:30Z roger $";

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
    IMAGEHLP_MODULE64 stIHM{};
    stIHM.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);

    if (::SymGetModuleInfo64(hProcess, dwAddress, &stIHM)) {
      baseAddress = stIHM.BaseOfImage;
      // ATLTRACE("showGetModuleBase got addr from SymGetModuleInfo = %x\n",
      // baseAddress);
    } else {
      baseAddress = reinterpret_cast<DWORD64>(mbInfo.AllocationBase);
      const auto hmod = static_cast<HMODULE>(mbInfo.AllocationBase);

      const auto filename = GetModuleFileNameWrapper(hProcess, hmod);

      if (!filename.empty()) {
        bool bPathSet(false);
        std::vector<char> searchpath(1024);
        if (::SymGetSearchPath(hProcess, &searchpath[0], 1024)) {
          // symbol files often stored with binary image
          const auto index = filename.find_last_of('\\');
          if (index != std::string::npos) {
            std::string fullpath(filename.substr(0, index));
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
        if (!::SymLoadModule64(hProcess, nullptr, filename.c_str(), nullptr,
                               baseAddress, 0)) {
          // ATLTRACE("SymLoadModule, failed for %s\n", filename.c_str());
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
 * @returns the filename, which is empty on failure,
 * in which case call GetLastError for the underlying error code.
 */
std::string GetModuleFileNameWrapper(HANDLE hProcess, HMODULE hMod) {
  DWORD bufLen = MAX_PATH;
  std::string result(bufLen, '\0');
  DWORD ret = ::GetModuleFileNameEx(hProcess, hMod, &result[0], bufLen);
  while (ret == bufLen) {
    // File name truncation
    bufLen *= 2;
    result.resize(bufLen);
    ret = ::GetModuleFileNameEx(hProcess, hMod, &result[0], bufLen);
  }
  if (ret == 0 && hMod == nullptr) {
    const DWORD lastError = GetLastError();
    if (lastError == ERROR_PARTIAL_COPY || lastError == ERROR_INVALID_HANDLE) {
      if (QueryFullProcessImageName(hProcess, 0, &result[0], &bufLen)) {
        ret = bufLen;
      }
    }
    if (ret == 0) {
      SetLastError(lastError);
    }
  }
  if (ret != 0) {
    if (result.compare(0, 4, R"(\??\)") == 0) {
      result[1] = '\\'; // For some reason the wrong UNC header is returned
    }
  }
  result.resize(ret);
  return result;
}
