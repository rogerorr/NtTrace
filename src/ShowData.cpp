/*
NAME
  ShowData.cpp

DESCRIPTION
  Class to handle showing data from another process

AUTHOR
  Roger Orr mailto:rogero@howzatt.co.uk
  Bug reports, comments, and suggestions are always welcome.

COPYRIGHT
  Copyright (C) 2002, 2015 under the MIT license:

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

static char const szRCSID[] =
    "$Id: ShowData.cpp 2728 2025-04-24 23:54:43Z roger $";

#include "ShowData.h"

#include <DbgHelp.h>
#include <windows.h>

#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <typeinfo>
#include <vector>

// or2 includes
#include "../include/MsvcExceptions.h"
#include "../include/ProcessInfo.h"
#include "../include/ReadPartialMemory.h"

namespace {
/** Read an object of type 'T' at remoteAddress in the specified process */
template <typename T>
BOOL readHelper(HANDLE hProcess, LPCVOID remoteAddress, T &theValue) {
  return ReadProcessMemory(hProcess, remoteAddress, &theValue, sizeof(T),
                           nullptr);
}

/** Read an object of type 'T' at remoteAddress in the specified process */
template <typename T>
BOOL readHelper(HANDLE hProcess, ULONG_PTR remoteAddress, T &theValue) {
  return ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(remoteAddress),
                           &theValue, sizeof(T), nullptr);
}

bool isWow(HANDLE hProcess);

size_t Utf16ToMbs(char *mb_str, size_t mb_size, const wchar_t *wc_str,
                  size_t wc_len) {
  return WideCharToMultiByte(CP_UTF8, 0, wc_str, static_cast<int>(wc_len),
                             mb_str, static_cast<int>(mb_size), 0, nullptr);
}

} // namespace

namespace showData {

//////////////////////////////////////////////////////////////////////////
// Data for enumerations
using Enumerators = std::vector<std::pair<std::string, unsigned long>>;
using EnumMap = std::map<std::string, Enumerators>;
EnumMap enumMap;

//////////////////////////////////////////////////////////////////////////
/** define an enumerator value for an enumeration */
void defineEnumerator(std::string const &enumeration, std::string const &enumerator, unsigned long value)
{
  auto &entry = enumMap[enumeration];
  entry.push_back(std::make_pair(enumerator, value));
}

//////////////////////////////////////////////////////////////////////////
/** Stream a large integer to an output stream.
 */
std::ostream &operator<<(std::ostream &os, LARGE_INTEGER const &largeInteger) {
  double d = static_cast<double>(largeInteger.QuadPart);
  return os << d;
}

/** Stream a large unsigned integer to an output stream.
 */
std::ostream &operator<<(std::ostream &os,
                         ULARGE_INTEGER const &ulargeInteger) {
  double d = static_cast<double>(ulargeInteger.QuadPart);
  return os << d;
}

/**
 * Stream a file time to an output stream
 */
std::ostream &operator<<(std::ostream &os, FILETIME const &fileTime) {
  static char const *const month[] = {"???", "Jan", "Feb", "Mar", "Apr",
                                      "May", "Jun", "Jul", "Aug", "Sep",
                                      "Oct", "Nov", "Dec"};

  static ULONGLONG const OneYear = (ULONGLONG)365 * 86400 * 1000000000 / 100;

  ULARGE_INTEGER ulFileTime;
  ulFileTime.LowPart = fileTime.dwLowDateTime;
  ulFileTime.HighPart = fileTime.dwHighDateTime;

  SYSTEMTIME systemTime;
  if (FileTimeToSystemTime(&fileTime, &systemTime)) {
    SYSTEMTIME now;
    GetSystemTime(&now);
    FILETIME nowTime;
    SystemTimeToFileTime(&now, &nowTime);
    ULARGE_INTEGER ulTime;
    ulTime.LowPart = nowTime.dwLowDateTime;
    ulTime.HighPart = nowTime.dwHighDateTime;

    // write "mon dd "
    os << month[systemTime.wMonth] << " " << std::setw(2) << systemTime.wDay
       << " ";
    if ((ulFileTime.QuadPart + OneYear) < ulTime.QuadPart) {
      // write full year
      os << std::setw(5) << systemTime.wYear;
    } else {
      // write hh:mm
      os << std::setw(2) << std::setfill('0') << systemTime.wHour << ":"
         << std::setw(2) << systemTime.wMinute << std::setfill(' ');
    }
  } else {
    os << ulFileTime;
  }
  return os;
}

//////////////////////////////////////////////////////////////////////////
void showDword(std::ostream &os, ULONG_PTR value) {
  LONG_PTR iValue(value);
  if ((iValue < 10) && (iValue > -10))
    os << iValue;
  else if (value < 0x10000)
    os << "0x" << std::hex << value << std::dec;
  else if ((value & 0xFFFFFFFF) == value) {
    os << "0x" << std::hex << std::setfill('0') << std::setw(8) << value
       << std::setfill(' ') << std::dec;
  } else {
    os << "0x" << std::hex << std::setfill('0') << std::setw(16) << value
       << std::setfill(' ') << std::dec;
  }
}

//////////////////////////////////////////////////////////////////////////
void showBoolean(std::ostream &os, BOOLEAN value) {
  os << (value ? "true" : "false");
}

//////////////////////////////////////////////////////////////////////////
// Show an enumeration name, if available
void showEnum(std::ostream &os, ULONG_PTR value, std::string const &enumName) {
  showDword(os, value);
  const auto it = enumMap.find(enumName);
  if (it != enumMap.end()) {
    for (const auto& enumerator : it->second) {
      if (enumerator.second == value) {
        os << " [" << enumerator.first << ']';
        break;
      }
    }
  }
}

//////////////////////////////////////////////////////////////////////////
// Show an mask enumeration name, if available
void showMask(std::ostream &os, ULONG_PTR value, std::string const &enumName) {
  showDword(os, value);
  const auto it = enumMap.find(enumName);
  std::string delim = " [";
  if (it != enumMap.end()) {
    for (const auto& enumerator : it->second) {
      if ((value & enumerator.second) == enumerator.second) {
        os << delim << enumerator.first;
        value -= enumerator.second;
        delim = "|";
        break;
      }
    }
    if (delim == "|") {
      // anything left over?
      if (value)
        os << delim << value;
      os << ']';
    }
  }
}

//////////////////////////////////////////////////////////////////////////
/** show a handle from the debuggee */
void showHandle(std::ostream &os, HANDLE handle) {
  showDword(os, reinterpret_cast<ULONG_PTR>(handle));
}

//////////////////////////////////////////////////////////////////////////
// Show a module name from the debuggee
bool showName(std::ostream &os, HANDLE hProcess, LPCVOID lpImageName,
              bool bUnicode) {
  void *lpString = nullptr;

  if (lpImageName == nullptr)
    return false;

  if (!readHelper(hProcess, lpImageName, lpString)) {
    os << "null";
  } else if (lpString == nullptr) {
    return false;
  } else {
    showString(os, hProcess, lpString, bUnicode, MAX_PATH);
  }
  return true;
}

//////////////////////////////////////////////////////////////////////////
// Show a NUL terminated string from the debuggee
bool showString(std::ostream &os, HANDLE hProcess, LPCVOID lpString,
                bool bUnicode, WORD nStringLength) {
  bool newline(false);

  if (nStringLength == 0) {
  } else if (bUnicode) {
    std::vector<wchar_t> chVector(nStringLength + 1);
    or2::ReadPartialProcessMemory(hProcess, lpString, &chVector[0], 1,
                                  nStringLength * sizeof(wchar_t));
    size_t const mbLen = Utf16ToMbs(nullptr, 0, &chVector[0], nStringLength);
    if (mbLen == 0) {
      for (int i = 0; i != nStringLength + 1; ++i) {
        os << "char " << i << " is " << std::hex << (unsigned int)chVector[i]
           << std::dec << '\n';
      }
      os << "invalid string: " << GetLastError();
    } else {
      std::vector<char> mbStr(mbLen + 1);
      Utf16ToMbs(&mbStr[0], mbLen, &chVector[0], nStringLength);
      os << &mbStr[0];
      if (mbLen > 0 && mbStr[mbLen - 1] == '\n') {
        newline = true;
      }
    }
  } else {
    std::vector<char> chVector(nStringLength + 1);
    or2::ReadPartialProcessMemory(hProcess, lpString, &chVector[0], 1,
                                  nStringLength);
    os << &chVector[0];
    if (chVector[nStringLength - 1] == '\0') {
      nStringLength--;
    }
    if (nStringLength > 0 && chVector[nStringLength - 1] == '\n') {
      newline = true;
    }
  }
  return newline;
}

//////////////////////////////////////////////////////////////////////////
void showCommandLine(std::ostream &os, HANDLE hProcess) {
  static NtQueryInformationProcess *pfnNtQueryInformationProcess =
      (NtQueryInformationProcess *)::GetProcAddress(
          ::GetModuleHandle("NTDLL"), "NtQueryInformationProcess");

  if (pfnNtQueryInformationProcess == nullptr) {
    return;
  }

  PROCESS_BASIC_INFORMATION ProcessInformation = {0};
  pfnNtQueryInformationProcess(hProcess, ProcessBasicInformation,
                               &ProcessInformation, sizeof(ProcessInformation),
                               nullptr);

  if (ProcessInformation.PebBaseAddress == nullptr) {
    return;
  }

  PEB peb = {0};
  if (!ReadProcessMemory(hProcess, ProcessInformation.PebBaseAddress, &peb,
                         sizeof(peb), nullptr)) {
    return;
  }

  showUnicodeString(os, hProcess, &peb.ProcessParameters->CommandLine);
}

//////////////////////////////////////////////////////////////////////////
void showObjectAttributes(std::ostream &os, HANDLE hProcess,
                          POBJECT_ATTRIBUTES pObjectAttributes) {
  OBJECT_ATTRIBUTES objectAttributes = {0};

  (void)readHelper(hProcess, pObjectAttributes, objectAttributes);

  if (objectAttributes.RootDirectory) {
    showHandle(os, objectAttributes.RootDirectory);
    os << ':';
  }
  showUnicodeString(os, hProcess, objectAttributes.ObjectName);
}

//////////////////////////////////////////////////////////////////////////
void showUnicodeString(std::ostream &os, HANDLE hProcess,
                       PUNICODE_STRING pTargetUnicodeString) {
  if (pTargetUnicodeString == nullptr)
    os << "null";
  else {
    UNICODE_STRING unicodeString = {0};
    (void)readHelper(hProcess, pTargetUnicodeString, unicodeString);

    os << '"';
    showString(os, hProcess, unicodeString.Buffer, true,
               unicodeString.Length / sizeof(wchar_t));
    os << '"';
  }
}

//////////////////////////////////////////////////////////////////////////
void showPointer(std::ostream &os, HANDLE /*hProcess*/, ULONG_PTR argVal) {
  if (argVal == 0)
    os << "null";
  else {
    os << "0x" << std::hex << argVal << std::dec;
  }
}

void showPointer(std::ostream &os, HANDLE hProcess, const void *argVal) {
  return showPointer(os, hProcess, reinterpret_cast<ULONG_PTR>(argVal));
}

//////////////////////////////////////////////////////////////////////////
void showPHandle(std::ostream &os, HANDLE hProcess, ULONG_PTR argVal) {
  showPointer(os, hProcess, argVal);
  if (argVal) {
    HANDLE handle{};
    (void)readHelper(hProcess, argVal, handle);

    os << " [";
    showHandle(os, handle);
    os << "]";
  }
}

//////////////////////////////////////////////////////////////////////////
void showPByte(std::ostream &os, HANDLE hProcess, ULONG_PTR argVal) {
  showPointer(os, hProcess, argVal);
  if (argVal) {
    BYTE value = 0;
    (void)readHelper(hProcess, argVal, value);

    os << " [";
    showDword(os, value);
    os << "]";
  }
}

//////////////////////////////////////////////////////////////////////////
void showPUshort(std::ostream &os, HANDLE hProcess, ULONG_PTR argVal) {
  showPointer(os, hProcess, argVal);
  if (argVal) {
    USHORT value = 0;
    (void)readHelper(hProcess, argVal, value);

    os << " [";
    showDword(os, value);
    os << "]";
  }
}

//////////////////////////////////////////////////////////////////////////
void showPUlong(std::ostream &os, HANDLE hProcess, ULONG_PTR argVal) {
  showPointer(os, hProcess, argVal);
  if (argVal) {
    ULONG value = 0;
    (void)readHelper(hProcess, argVal, value);

    os << " [";
    showDword(os, value);
    os << "]";
  }
}

//////////////////////////////////////////////////////////////////////////
void showAccessMask(std::ostream &os, HANDLE /*hProcess*/, ACCESS_MASK argVal,
                    const std::string &maskName) {
  // Deal with easy case first!
  if (argVal == 0) {
    os << "0";
    return;
  }

  std::ostringstream mask;

#define ADD_MASK(X)                                                            \
  if ((argVal & X) == X) {                                                     \
    mask << "|" #X;                                                            \
    argVal &= ~X;                                                              \
  }

  if (maskName == "DIRECTORY_ACCESS_MASK") {
    ADD_MASK(FILE_LIST_DIRECTORY);
    ADD_MASK(FILE_ADD_FILE);
    ADD_MASK(FILE_ADD_SUBDIRECTORY);
    ADD_MASK(FILE_READ_EA);
    ADD_MASK(FILE_WRITE_EA);
    ADD_MASK(FILE_TRAVERSE);
    ADD_MASK(FILE_DELETE_CHILD);
  } else if (maskName == "EVENT_ACCESS_MASK") {
    ADD_MASK(EVENT_ALL_ACCESS);
    ADD_MASK(MUTANT_QUERY_STATE);
    ADD_MASK(EVENT_MODIFY_STATE);
  } else if (maskName == "FILE_ACCESS_MASK") {
    // File system combined masks
    ADD_MASK(FILE_ALL_ACCESS);
    ADD_MASK(FILE_GENERIC_READ);
    ADD_MASK(FILE_GENERIC_WRITE);
    ADD_MASK(FILE_GENERIC_EXECUTE);
    ADD_MASK(FILE_READ_DATA);
    ADD_MASK(FILE_WRITE_DATA);
    ADD_MASK(FILE_APPEND_DATA);
    ADD_MASK(FILE_READ_EA);
    ADD_MASK(FILE_WRITE_EA);
    ADD_MASK(FILE_EXECUTE);
    ADD_MASK(FILE_DELETE_CHILD);
    ADD_MASK(FILE_READ_ATTRIBUTES);
    ADD_MASK(FILE_WRITE_ATTRIBUTES);
  } else if (maskName == "JOB_ACCESS_MASK") {
    ADD_MASK(JOB_OBJECT_ALL_ACCESS);
    ADD_MASK(JOB_OBJECT_ASSIGN_PROCESS);
    ADD_MASK(JOB_OBJECT_SET_ATTRIBUTES);
    ADD_MASK(JOB_OBJECT_QUERY);
    ADD_MASK(JOB_OBJECT_TERMINATE);
    ADD_MASK(JOB_OBJECT_SET_SECURITY_ATTRIBUTES);
    ADD_MASK(JOB_OBJECT_IMPERSONATE);
  } else if (maskName == "KEY_ACCESS_MASK") {
    // Registry combined masks
    ADD_MASK(KEY_ALL_ACCESS);
    ADD_MASK(KEY_READ);
    ADD_MASK(KEY_WRITE);
    ADD_MASK(KEY_EXECUTE);
    ADD_MASK(KEY_QUERY_VALUE);
    ADD_MASK(KEY_SET_VALUE);
    ADD_MASK(KEY_CREATE_SUB_KEY);
    ADD_MASK(KEY_ENUMERATE_SUB_KEYS);
    ADD_MASK(KEY_CREATE_LINK);
    ADD_MASK(KEY_NOTIFY);
    ADD_MASK(KEY_WOW64_32KEY);
    ADD_MASK(KEY_WOW64_64KEY);
    ADD_MASK(KEY_WOW64_RES);
  } else if (maskName == "MUTANT_ACCESS_MASK") {
    ADD_MASK(MUTANT_ALL_ACCESS);
    ADD_MASK(MUTANT_QUERY_STATE);
  } else if (maskName == "PROCESS_ACCESS_MASK") {
    ADD_MASK(PROCESS_ALL_ACCESS);
    ADD_MASK(PROCESS_TERMINATE);
    ADD_MASK(PROCESS_CREATE_THREAD);
    ADD_MASK(PROCESS_SET_SESSIONID);
    ADD_MASK(PROCESS_VM_OPERATION);
    ADD_MASK(PROCESS_VM_READ);
    ADD_MASK(PROCESS_VM_WRITE);
    ADD_MASK(PROCESS_DUP_HANDLE);
    ADD_MASK(PROCESS_CREATE_PROCESS);
    ADD_MASK(PROCESS_SET_QUOTA);
    ADD_MASK(PROCESS_SET_INFORMATION);
    ADD_MASK(PROCESS_QUERY_INFORMATION);
    ADD_MASK(PROCESS_SUSPEND_RESUME);
    ADD_MASK(PROCESS_QUERY_LIMITED_INFORMATION);
    ADD_MASK(PROCESS_SET_LIMITED_INFORMATION);
  } else if (maskName == "SECTION_ACCESS_MASK") {
    ADD_MASK(SECTION_ALL_ACCESS);
    ADD_MASK(SECTION_EXTEND_SIZE);
    ADD_MASK(SECTION_MAP_EXECUTE);
    ADD_MASK(SECTION_MAP_READ);
    ADD_MASK(SECTION_MAP_WRITE);
    ADD_MASK(SECTION_QUERY);
  } else if (maskName == "SEMAPHORE_ACCESS_MASK") {
    ADD_MASK(SEMAPHORE_ALL_ACCESS);
    ADD_MASK(SEMAPHORE_MODIFY_STATE);
    ADD_MASK(MUTANT_QUERY_STATE);
  } else if (maskName == "THREAD_ACCESS_MASK") {
    ADD_MASK(THREAD_ALL_ACCESS);
    ADD_MASK(THREAD_TERMINATE);
    ADD_MASK(THREAD_SUSPEND_RESUME);
    ADD_MASK(THREAD_GET_CONTEXT);
    ADD_MASK(THREAD_SET_CONTEXT);
    ADD_MASK(THREAD_QUERY_INFORMATION);
    ADD_MASK(THREAD_SET_INFORMATION);
    ADD_MASK(THREAD_SET_THREAD_TOKEN);
    ADD_MASK(THREAD_IMPERSONATE);
    ADD_MASK(THREAD_DIRECT_IMPERSONATION);
    ADD_MASK(THREAD_SET_LIMITED_INFORMATION);
    ADD_MASK(THREAD_QUERY_LIMITED_INFORMATION);
    ADD_MASK(THREAD_RESUME);
  } else if (maskName == "TIMER_ACCESS_MASK") {
    ADD_MASK(TIMER_ALL_ACCESS);
    ADD_MASK(TIMER_QUERY_STATE);
    ADD_MASK(TIMER_MODIFY_STATE);
  } else if (maskName == "TOKEN_ACCESS_MASK") {
    ADD_MASK(TOKEN_ALL_ACCESS);
    ADD_MASK(TOKEN_READ);
    ADD_MASK(TOKEN_WRITE);
    ADD_MASK(TOKEN_EXECUTE);
    ADD_MASK(TOKEN_ASSIGN_PRIMARY);
    ADD_MASK(TOKEN_DUPLICATE);
    ADD_MASK(TOKEN_IMPERSONATE);
    ADD_MASK(TOKEN_QUERY);
    ADD_MASK(TOKEN_QUERY_SOURCE);
    ADD_MASK(TOKEN_ADJUST_PRIVILEGES);
    ADD_MASK(TOKEN_ADJUST_GROUPS);
    ADD_MASK(TOKEN_ADJUST_DEFAULT);
    ADD_MASK(TOKEN_ADJUST_SESSIONID);
  }

  //  The following are masks for the predefined standard access types

  ADD_MASK(STANDARD_RIGHTS_ALL);
  ADD_MASK(STANDARD_RIGHTS_REQUIRED);
  ADD_MASK(DELETE);
  ADD_MASK(READ_CONTROL);
  ADD_MASK(WRITE_DAC);
  ADD_MASK(WRITE_OWNER);
  ADD_MASK(SYNCHRONIZE);

  // AccessSystemAcl access type
  ADD_MASK(ACCESS_SYSTEM_SECURITY);

  // MaximumAllowed access type
  ADD_MASK(MAXIMUM_ALLOWED);

  //  These are the generic rights.
  ADD_MASK(GENERIC_READ);
  ADD_MASK(GENERIC_WRITE);
  ADD_MASK(GENERIC_EXECUTE);
  ADD_MASK(GENERIC_ALL);

#undef ADD_MASK

  // Specific rights
  if (argVal) {
    mask << "|0x" << std::hex << argVal;
  }

  os << mask.str().substr(1);
}

//////////////////////////////////////////////////////////////////////////
void showPClientId(std::ostream &os, HANDLE hProcess, PCLIENT_ID pClientId) {
  showPointer(os, hProcess, pClientId);
  if (pClientId) {
    CLIENT_ID clientId = {nullptr};
    (void)readHelper(hProcess, pClientId, clientId);

    os << " [";
    showHandle(os, clientId.UniqueProcess);
    os << "/";
    showHandle(os, clientId.UniqueThread);
    os << "]";
  }
}

//////////////////////////////////////////////////////////////////////////
void showPIoStatus(std::ostream &os, HANDLE hProcess,
                   PIO_STATUS_BLOCK pIoStatusBlock) {
  showPointer(os, hProcess, pIoStatusBlock);
  if (pIoStatusBlock) {
    IO_STATUS_BLOCK IoStatusBlock = {0};
    readHelper(hProcess, pIoStatusBlock, IoStatusBlock);

    os << " [";
    showDword(os, IoStatusBlock.Status);
    os << "/";
    showDword(os, static_cast<ULONG_PTR>(IoStatusBlock.Information));
    os << "]";
  }
}

//////////////////////////////////////////////////////////////////////////
void showPLargeInteger(std::ostream &os, HANDLE hProcess,
                       PLARGE_INTEGER pLargeInteger) {
  showPointer(os, hProcess, pLargeInteger);
  if (pLargeInteger) {
    LARGE_INTEGER largeInteger = {0};
    readHelper(hProcess, pLargeInteger, largeInteger);

    os << " [";
    os << largeInteger;
    os << "]";
  }
}

//////////////////////////////////////////////////////////////////////////
// Display an LPC message
void showPLpcMessage(std::ostream &os, HANDLE hProcess,
                     PLPC_MESSAGE pLpcMessage) {
  showPointer(os, hProcess, pLpcMessage);
  if (pLpcMessage) {
    LPC_MESSAGE message;
    readHelper(hProcess, pLpcMessage, message);

    os << " [";
    showEnum(os, message.MessageType, "LPC_TYPE");
    os << " (" << message.DataLength << "b)";
    os << "]";
  }
}

//////////////////////////////////////////////////////////////////////////
void showFileAttributes(std::ostream &os, ULONG argVal) {
  // Deal with easy case first!
  if (argVal == 0) {
    os << "0";
    return;
  }

  std::ostringstream mask;

#define ADD_MASK(X)                                                            \
  if (argVal & FILE_ATTRIBUTE_##X) {                                           \
    mask << "|" #X;                                                            \
    argVal &= ~FILE_ATTRIBUTE_##X;                                             \
  }

  //  The following are masks for the predefined standard access types

  ADD_MASK(READONLY);
  ADD_MASK(HIDDEN);
  ADD_MASK(SYSTEM);
  ADD_MASK(DIRECTORY);
  ADD_MASK(ARCHIVE);
  ADD_MASK(ENCRYPTED);
  ADD_MASK(NORMAL);
  ADD_MASK(TEMPORARY);
  ADD_MASK(SPARSE_FILE);
  ADD_MASK(REPARSE_POINT);
  ADD_MASK(COMPRESSED);
  ADD_MASK(OFFLINE);
  ADD_MASK(NOT_CONTENT_INDEXED);

#undef ADD_MASK

  if (argVal) {
    mask << "|0x" << std::hex << argVal;
  }

  os << mask.str().substr(1);
}

//////////////////////////////////////////////////////////////////////////
// note: file times are not returned by the commonest Api,
// NtQueryInformationFile ...
void showPFileBasicInfo(std::ostream &os, HANDLE hProcess,
                        PFILE_BASIC_INFORMATION pFileBasicInfo) {
  showPointer(os, hProcess, pFileBasicInfo);
  if (pFileBasicInfo) {
    FILE_BASIC_INFORMATION fileBasicInfo = {0};
    readHelper(hProcess, pFileBasicInfo, fileBasicInfo);

    os << " [";
    showFileAttributes(os, fileBasicInfo.FileAttributes);
    os << "]";
  }
}

//////////////////////////////////////////////////////////////////////////
void showPFileNetworkInfo(std::ostream &os, HANDLE hProcess,
                          PFILE_NETWORK_OPEN_INFORMATION pFileNetworkInfo) {
  showPointer(os, hProcess, pFileNetworkInfo);
  if (pFileNetworkInfo) {
    FILE_NETWORK_OPEN_INFORMATION fileNetworkInfo = {0};
    readHelper(hProcess, pFileNetworkInfo, fileNetworkInfo);

    os << " [";
    FILETIME fileTime;
    fileTime.dwLowDateTime = fileNetworkInfo.LastWriteTime.LowPart;
    fileTime.dwHighDateTime = fileNetworkInfo.LastWriteTime.HighPart;
    os << fileTime;
    os << " ";
    os << fileNetworkInfo.EndOfFile;
    os << " ";
    showFileAttributes(os, fileNetworkInfo.FileAttributes);
    os << "]";
  }
}

//////////////////////////////////////////////////////////////////////////
void showUserProcessParams(std::ostream &os, HANDLE hProcess,
                           PRTL_USER_PROCESS_PARAMETERS pUserProcessParams) {
  showPointer(os, hProcess, pUserProcessParams);
  if (pUserProcessParams) {
    os << " [";
    showUnicodeString(os, hProcess, &pUserProcessParams->ImagePathName);
    os << "]";
  }
}

//////////////////////////////////////////////////////////////////////////
// Convert windows NT error into a text string, if possible.
void showWinError(std::ostream &os, HRESULT hResult) {
  char *pszMsg = nullptr;
  HMODULE hmod = nullptr;

  DWORD hmodFlags(hmod ? FORMAT_MESSAGE_FROM_HMODULE : 0);
  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS | hmodFlags,
                hmod, hResult, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR)&pszMsg, 0, nullptr);

  if (pszMsg != nullptr) {
    size_t nLen = strlen(pszMsg);
    if (nLen > 1 && pszMsg[nLen - 1] == '\n') {
      pszMsg[nLen - 1] = 0;
      if (pszMsg[nLen - 2] == '\r') {
        pszMsg[nLen - 2] = 0;
      }
    }

    if (hResult < 0)
      os << " [0x" << std::hex << hResult << std::dec;
    else
      os << " [" << hResult;
    os << " '" << pszMsg << "']";
    ::LocalFree(pszMsg);
  }
}

//////////////////////////////////////////////////////////////////////////
// Convert msvc throw information into a type name
void showThrowType(std::ostream &os, HANDLE hProcess, ULONG_PTR throwInfo,
                   ULONG_PTR base) {
  MsvcThrow msvcThrow = {0};
  MsvcClassHeader msvcClassHeader = {0};
  MsvcClassInfo msvcClassInfo = {0};
  BYTE type_info[sizeof(std::type_info) + 256] = "";
  DWORD offset = 0;

#ifdef _M_X64
  // process 32-bit type_info in 64-bit debugger
  if (((throwInfo >> 32) == 0) && isWow(hProcess)) {
    offset = sizeof(PVOID);
  }
#endif // _M_X64

  if (!readHelper(hProcess, (PVOID)throwInfo, msvcThrow) ||
      !readHelper(hProcess, (PVOID)(base + msvcThrow.pClassHeader),
                  msvcClassHeader) ||
      !readHelper(hProcess, (PVOID)(base + msvcClassHeader.Info[0]),
                  msvcClassInfo) ||
      !or2::ReadPartialProcessMemory(
          hProcess, (PVOID)(base + msvcClassInfo.pTypeInfo), type_info + offset,
          sizeof(std::type_info), sizeof(type_info))) {
    return;
  }

  // We now have a copy of the type_info in the target process (which may be
  // us!) Ensure we clear the internal m_Data member to avoid trying to read
  // pointers which aren't ours...
  memset(type_info + sizeof(PVOID), 0, sizeof(PVOID));

  const std::type_info *pType_info = (const std::type_info *)type_info;
  const char *decorated_name = pType_info->raw_name();

  char buffer[1024] = "";
  if ((decorated_name[0] == '.') &&
      UnDecorateSymbolName(decorated_name + 1, buffer, sizeof(buffer),
                           UNDNAME_32_BIT_DECODE | UNDNAME_NO_ARGUMENTS)) {
    os << " type: " << buffer;
  } else {
    os << " raw type: " << decorated_name;
  }
}

} // namespace showData

namespace {
bool isWow(HANDLE hProcess) {
  using IsWow64Process =
      BOOL WINAPI(_In_ HANDLE hProcess, _Out_ PBOOL Wow64Process);

  static IsWow64Process *pfnIsWow64Process = (IsWow64Process *)::GetProcAddress(
      ::GetModuleHandle("Kernel32"), "IsWow64Process");
  BOOL result(false);
  if (pfnIsWow64Process)
    pfnIsWow64Process(hProcess, &result);
  return result != 0;
}

} // namespace