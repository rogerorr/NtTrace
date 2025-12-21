#ifndef ProcessHelper_H_
#define ProcessHelper_H_

/**
@file
  Helper functions for process management.

  @author Roger Orr mailto:rogero@howzatt.co.uk
  Bug reports, comments, and suggestions are always welcome.

  Copyright &copy; 2003 under the MIT license:

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

  $Revision: 2996 $
*/

// $Id: ProcessHelper.h 2996 2025-12-21 14:39:04Z roger $

// clang-format off
#include <windows.h> // for CreateProcess
// clang-format on
#include <algorithm> // for transform
#include <cctype>    // for tolower
#include <iostream>
#include <psapi.h> // for EnumProcesses
#include <string>
#include <vector>

#include "Options.h"
#include "ReadInt.h"

namespace or2 {

//////////////////////////////////////////////////////////////////////////
/** Wrapper for create process.
 *
 * @return
 * - 0 on success
 * - non-zero on failure, and GetLastError() can be used
 */

inline int CreateProcessHelper(
    Options::const_iterator it,  ///< first string is command
    Options::const_iterator end, ///< rest of iteration are arguments
    DWORD create_options = 0,    ///< options for CreateProcess
    PROCESS_INFORMATION *pProcessInformation =
        nullptr) ///< returned process information on success
{
  std::string executable = *it;

  // Search for possible executable matching the program name
  char szFullName[MAX_PATH];
  if (0 != SearchPath(nullptr, executable.c_str(), ".exe", sizeof(szFullName),
                      szFullName, nullptr))
    executable = szFullName;

  std::string cmdLine;
  for (; it != end; ++it) {
    std::string curr(*it);

    if (cmdLine.length())
      cmdLine += " ";

    if (curr.find(' ') != std::string::npos) {
      cmdLine += '"';
      cmdLine += curr;
      cmdLine += '"';
    } else {
      cmdLine += curr;
    }
  }

  STARTUPINFO startupInfo = {sizeof(startupInfo)};
  startupInfo.dwFlags = STARTF_USESHOWWINDOW;
  startupInfo.wShowWindow = SW_SHOWNORMAL; // Assist GUI programs
  PROCESS_INFORMATION ProcessInformation;

  if (!CreateProcess(
          const_cast<char *>(executable.c_str()), // name of executable module
          const_cast<char *>(cmdLine.c_str()),    // command line string
          nullptr,                                // SD
          nullptr,                                // SD
          true,                                   // handle inheritance option
          create_options,                         // creation flags
          nullptr,                                // new environment block
          nullptr,                                // current directory name
          &startupInfo,                           // startup information
          &ProcessInformation                     // process information
          )) {
    return 1;
  }

  if (pProcessInformation)
    *pProcessInformation = ProcessInformation;
  else {
    CloseHandle(ProcessInformation.hProcess);
    CloseHandle(ProcessInformation.hThread);
  }

  return 0;
}

//////////////////////////////////////////////////////////////////////////
/** Wrapper for create process as user.
 *
 * @return
 * - 0 on success
 * - non-zero on failure, and GetLastError() can be used
 */

inline int CreateProcessAsUserHelper(
    HANDLE hToken,               ///< security token to use for target process
    Options::const_iterator it,  ///< first string is command
    Options::const_iterator end, ///< rest of iteration are arguments
    DWORD create_options = 0,    ///< options for CreateProcess
    PROCESS_INFORMATION *pProcessInformation =
        nullptr) ///< returned process information on success
{
  std::string executable = *it;

  // Search for possible executable matching the program name
  char szFullName[MAX_PATH];
  if (0 != SearchPath(nullptr, executable.c_str(), ".exe", sizeof(szFullName),
                      szFullName, nullptr))
    executable = szFullName;

  std::string cmdLine;
  for (; it != end; ++it) {
    std::string curr(*it);

    if (cmdLine.length())
      cmdLine += " ";

    if (curr.find(' ') != std::string::npos) {
      cmdLine += '"';
      cmdLine += curr;
      cmdLine += '"';
    } else {
      cmdLine += curr;
    }
  }

  STARTUPINFO startupInfo = {sizeof(startupInfo)};
  startupInfo.dwFlags = STARTF_USESHOWWINDOW;
  startupInfo.wShowWindow = SW_SHOWNORMAL; // Assist GUI programs
  PROCESS_INFORMATION ProcessInformation;

  if (!CreateProcessAsUser(
          hToken,
          const_cast<char *>(executable.c_str()), // name of executable module
          const_cast<char *>(cmdLine.c_str()),    // command line string
          nullptr,                                // SD
          nullptr,                                // SD
          true,                                   // handle inheritance option
          create_options,                         // creation flags
          nullptr,                                // new environment block
          nullptr,                                // current directory name
          &startupInfo,                           // startup information
          &ProcessInformation                     // process information
          )) {
    return 1;
  }

  if (pProcessInformation)
    *pProcessInformation = ProcessInformation;
  else {
    CloseHandle(ProcessInformation.hProcess);
    CloseHandle(ProcessInformation.hThread);
  }

  return 0;
}

//////////////////////////////////////////////////////////
/** Returns all process IDs matching 'pattern'.
 *
 * @return a vector of all matching process IDs
 */
inline std::vector<DWORD> FindProcesses(
    const char *pattern = nullptr ///< pattern to match for process name
) {
  std::vector<DWORD> ret;

  int pid(0);

  if ((pattern != nullptr) && (readInt(pattern, pid))) {
    ret.push_back(static_cast<DWORD>(pid));
    return ret;
  }

  DWORD aProcesses[1024];
  DWORD cbNeeded;

  if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
    return ret;

  // Calculate how many process identifiers were returned.
  DWORD const cProcesses = cbNeeded / sizeof(DWORD);

  std::string lowerPattern(pattern == nullptr ? "" : pattern);
  // avoid C4244 warning from tolower
  struct lcase {
    char operator()(char ch) { return static_cast<char>(tolower(ch)); }
  } lcase;

  std::transform(lowerPattern.begin(), lowerPattern.end(), lowerPattern.begin(),
                 lcase);

  for (unsigned int iProcess = 0; iProcess < cProcesses; iProcess++) {
    DWORD const processID(aProcesses[iProcess]);

    // Open the process to query the main module name

    if (HANDLE hProcess = OpenProcess(
            PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID)) {
      char szModName[MAX_PATH];

      // Get the full path to the module's file.

      if (GetModuleBaseName(hProcess, nullptr, szModName, sizeof(szModName))
#ifdef GetProcessImageFileName
          || GetProcessImageFileName(hProcess, szModName, sizeof(szModName))
#endif // GetProcessImageFileName
      ) {
        std::string lower(szModName);
        std::transform(lower.begin(), lower.end(), lower.begin(), lcase);
        if ((lowerPattern.length() == 0) ||
            (lower.find(lowerPattern) != std::string::npos)) {
          // Return the process identifier.
          ret.push_back(processID);
        }
      }

      CloseHandle(hProcess);
    }
  }

  return ret;
}

} // namespace or2

#endif // ProcessHelper_H_