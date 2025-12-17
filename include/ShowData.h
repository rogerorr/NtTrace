#ifndef SHOWDATA_H_
#define SHOWDATA_H_

/**@file

  Functions to handle showing data from another process

  @author Roger Orr mailto:rogero@howzatt.co.uk
  Bug reports, comments, and suggestions are always welcome.

  Copyright &copy; 2002, 2013 under the MIT license:

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

  $Revision: 2968 $
*/

// $Id: ShowData.h 2968 2025-12-17 11:24:42Z roger $

#include <ostream>
#include <windows.h>

// or2 includes
#include "../include/NtDllStruct.h" // For Nt native data types
#include "../include/ProcessInfo.h"

/** namespace for functions showing data from another process */
namespace showData {
/** define an enumerator value for an enumeration */
void defineEnumerator(std::string const &enumeration,
                      std::string const &enumerator, unsigned long value);

/** show a DWORD from the debuggee */
void showDword(std::ostream &os, ULONG_PTR value);

/** show a BOOL from the debuggee */
void showBoolean(std::ostream &os, BOOLEAN value);

/** Show an enumeration name, if available */
void showEnum(std::ostream &os, ULONG_PTR value,
              std::string const &enumeration);

/** show a handle from the debuggee */
void showHandle(std::ostream &os, HANDLE handle);

/** Show an mask enumeration name, if available */
void showMask(std::ostream &os, ULONG_PTR value,
              std::string const &enumeration);

/** show an HRESULT from the debuggee */
void showWinError(std::ostream &os, HRESULT hResult);

/** show an image name from the debuggee (in ANSI or Unicode) */
bool showName(std::ostream &os, HANDLE hProcess, LPCVOID lpImageName,
              bool bUnicode);

/**
 * Show a string from the debuggee (in ANSI or Unicode)
 * @return true if ends with a newline, false if not
 */
bool showString(std::ostream &os, HANDLE hProcess, LPCVOID lpString,
                bool bUnicode, WORD nStringLength, bool extend = false);

/** Show the command line from the target process */
void showCommandLine(std::ostream &os, HANDLE hProcess);

/** show Object Attrributes from the debuggee */
void showObjectAttributes(std::ostream &os, HANDLE hProcess,
                          POBJECT_ATTRIBUTES pObjectAttributes);

/** show an Unicode string from the debuggee */
void showUnicodeString(std::ostream &os, HANDLE hProcess,
                       PUNICODE_STRING pTargetUnicodeString);

/** show a generic pointer from the debuggee */
void showPointer(std::ostream &os, HANDLE hProcess, ULONG_PTR argVal);

/** show a pointer to handle from the debuggee */
void showPHandle(std::ostream &os, HANDLE hProcess, ULONG_PTR argVal);

/** show a pointer to BYTE from the debuggee */
void showPByte(std::ostream &os, HANDLE hProcess, ULONG_PTR argVal);

/** show a pointer to USHORT from the debuggee */
void showPUshort(std::ostream &os, HANDLE hProcess, ULONG_PTR argVal);

/** show a pointer to ULONG from the debuggee */
void showPUlong(std::ostream &os, HANDLE hProcess, ULONG_PTR argVal);

/** show an access mask from the debuggee */
void showAccessMask(std::ostream &os, HANDLE hProcess, ACCESS_MASK argVal,
                    const std::string &maskName);

/** show a client ID from the debuggee */
void showPClientId(std::ostream &os, HANDLE hProcess, PCLIENT_ID pClientId);

/** show an OP status block from the debuggee */
void showPIoStatus(std::ostream &os, HANDLE hProcess,
                   PIO_STATUS_BLOCK pIoStatusBlock);

/** show a large integer from the debuggee */
void showPLargeInteger(std::ostream &os, HANDLE hProcess,
                       PLARGE_INTEGER pLargeInteger);

/** Display an LPC message */
void showPLpcMessage(std::ostream &os, HANDLE hProcess,
                     PLPC_MESSAGE pLpcMessage);

/** show file attributes from the debuggee */
void showFileAttributes(std::ostream &os, ULONG argVal);

/** show file basic information from the debuggee */
void showPFileBasicInfo(std::ostream &os, HANDLE hProcess,
                        PFILE_BASIC_INFORMATION pFileBasicInfo);

/** show network open information from the debuggee */
void showPFileNetworkInfo(std::ostream &os, HANDLE hProcess,
                          PFILE_NETWORK_OPEN_INFORMATION pFileNetworkInfo);

/** show user process parameters from the debuggee */
void showUserProcessParams(std::ostream &os, HANDLE hProcess,
                           PRTL_USER_PROCESS_PARAMETERS pUserProcessParams);

/** Convert msvc throw information into a type name */
void showThrowType(std::ostream &os, HANDLE hProcess, ULONG_PTR throwInfo,
                   ULONG_PTR base);

/** Streaming helper for showCommandLine */
struct CommandLine {
  const HANDLE hProcess_;
  CommandLine(HANDLE hProcess) : hProcess_(hProcess) {}
  operator std::string() const;
};

inline std::ostream &operator<<(std::ostream &os,
                                const CommandLine &commandLine) {
  showCommandLine(os, commandLine.hProcess_);
  return os;
}
} // namespace showData

#endif // SHOWDATA_H_