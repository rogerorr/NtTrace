#ifndef ProcessInfo_H_
#define ProcessInfo_H_

/**@file

  Header for Process information structures.

  @author Roger Orr mailto:rogero@howzatt.co.uk
  Bug reports, comments, and suggestions are always welcome.

  Copyright &copy; 2004,2011 under the MIT license:

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

  $Revision: 2675 $
*/

// $Id: ProcessInfo.h 2675 2025-04-21 16:52:13Z roger $

#include "NtDllStruct.h"

// Forward references
struct PEB;
using PPEB = PEB *;
struct PEB_LDR_DATA;
using PPEB_LDR_DATA = PEB_LDR_DATA *;

// From ntddk.h
enum PROCESSINFOCLASS {
  ProcessBasicInformation,
  ProcessQuotaLimits,
  ProcessIoCounters,
  ProcessVmCounters,
  ProcessTimes,
  ProcessBasePriority,
  ProcessRaisePriority,
  ProcessDebugPort,
  ProcessExceptionPort,
  ProcessAccessToken,
  ProcessLdtInformation,
  ProcessLdtSize,
  ProcessDefaultHardErrorMode,
  ProcessIoPortHandlers, ///< Note: this is kernel mode only
  ProcessPooledUsageAndLimits,
  ProcessWorkingSetWatch,
  ProcessUserModeIOPL,
  ProcessEnableAlignmentFaultFixup,
  ProcessPriorityClass,
  ProcessWx86Information,
  ProcessHandleCount,
  ProcessAffinityMask,
  ProcessPriorityBoost,
  ProcessDeviceMap,
  ProcessSessionInformation,
  ProcessForegroundInformation,
  ProcessWow64Information,
  ProcessImageFileName,
  ProcessLUIDDeviceMapsEnabled,
  ProcessBreakOnTermination,
  ProcessDebugObjectHandle,
  ProcessDebugFlags,
  ProcessHandleTracing,
  // Some more recent items
  ProcessIoPriority,
  ProcessExecuteFlags,
  ProcessTlsInformation,
  ProcessCookie,
  ProcessImageInformation,
  ProcessCycleTime,
  ProcessPagePriority,
  ProcessInstrumentationCallback,
  ProcessThreadStackAllocation,
  ProcessWorkingSetWatchEx,
  ProcessImageFileNameWin32,
  ProcessImageFileMapping,
  ProcessAffinityUpdateMode,
  ProcessMemoryAllocationMode,
  ProcessGroupInformation,
  ProcessTokenVirtualizationEnabled,
  ProcessConsoleHostProcess,
  ProcessWindowInformation,
  MaxProcessInfoClass ///< MaxProcessInfoClass should always be the last enum
};

/**
 * Thread priority
 */
using KPRIORITY = LONG;

/**
 * Basic Process Information
 *
 * NtQueryInformationProcess using ProcessBasicInfo
 */
struct PROCESS_BASIC_INFORMATION {
  NTSTATUS ExitStatus;
  PPEB PebBaseAddress;
  ULONG_PTR AffinityMask;
  KPRIORITY BasePriority;
  ULONG_PTR UniqueProcessId;
  ULONG_PTR InheritedFromUniqueProcessId;
};

/** Pointer to PROCESS_BASIC_INFORMATION */
using PPROCESS_BASIC_INFORMATION = PROCESS_BASIC_INFORMATION *;

// From
// http://cvs.sourceforge.net/viewcvs.py/mingw/w32api/include/ddk/ntapi.h?view=markup
struct RTL_USER_PROCESS_PARAMETERS {
  ULONG AllocationSize;
  ULONG Size;
  ULONG Flags;
  ULONG DebugFlags;
  HANDLE hConsole;
  ULONG ProcessGroup;
  HANDLE hStdInput;
  HANDLE hStdOutput;
  HANDLE hStdError;
  UNICODE_STRING CurrentDirectoryName;
  HANDLE CurrentDirectoryHandle;
  UNICODE_STRING DllPath;
  UNICODE_STRING ImagePathName;
  UNICODE_STRING CommandLine;
  PWSTR Environment;
  ULONG dwX;
  ULONG dwY;
  ULONG dwXSize;
  ULONG dwYSize;
  ULONG dwXCountChars;
  ULONG dwYCountChars;
  ULONG dwFillAttribute;
  ULONG dwFlags;
  ULONG wShowWindow;
  UNICODE_STRING WindowTitle;
  UNICODE_STRING DesktopInfo;
  UNICODE_STRING ShellInfo;
  UNICODE_STRING RuntimeInfo;
};
using PRTL_USER_PROCESS_PARAMETERS = RTL_USER_PROCESS_PARAMETERS *;

/** typedef for the call to NtQueryInformationProcess */
using NtQueryInformationProcess =
    // NTOSAPI
    NTSTATUS
    NTAPI(IN HANDLE ProcessHandle, ///< Handle to the process
          IN PROCESSINFOCLASS
              ProcessInformationClass,  ///< Class of information requested
          OUT PVOID ProcessInformation, ///< Buffer for returned information
          IN ULONG
              ProcessInformationLength, ///< Length of ProcessInformation buffer
          OUT PULONG ReturnLength OPTIONAL); ///< Returned length

/** typedef for the call to NtSetInformationProcess */
using NtSetInformationProcess = NTSTATUS
NTAPI(IN HANDLE ProcessHandle, ///< Handle to the process
      IN PROCESSINFOCLASS
          ProcessInformationClass,          ///< Class of information requested
      IN PVOID ProcessInformation OPTIONAL, ///< Buffer for returned information
      IN ULONG Length); ///< Length of ProcessInformation buffer

/**
 * Process Environment Block
 *
 * From
 * http://cvs.sourceforge.net/viewcvs.py/darwine/wine/include/winternl.h?rev=1.1.1.2
 * Modified for X64
 */
struct PEB {
  BYTE Reserved1[2]; /*  00 */        ///< Reserved
  BYTE BeingDebugged; /*  02 */       ///< TRUE when process is being debugged
  BYTE Reserved2[1]; /*  03 */        ///< Reserved
  PVOID __pad1;                       ///< Padding
  HMODULE ImageBaseAddress; /*  08 */ ///< Image base address
  PPEB_LDR_DATA LdrData; /*  0c */    ///< Loader data
  RTL_USER_PROCESS_PARAMETERS *ProcessParameters;
  /*  10 */                     ///< Process parameters
  PVOID __pad_2;                ///< Padding
  HANDLE ProcessHeap; /*  18 */ ///< Handle of the default process heap
  PVOID __pad3[14];             ///< Padding
  BYTE ___pad4[20];             ///< Padding
  ULONG GlobalFlag; /* 68 */    ///< Global Flags
  PVOID __pad5[38];             ///< Reserved
  BYTE ___pad6[208];            ///< Padding
  ULONG SessionId; /* 1d4 */    ///< The current session Id
};
using PPEB = PEB *;

/** Get access to PEB for current process */
#ifdef _M_X64
inline PEB *getCurrentPeb() {
  PEB *(*GetCurrentPEB)();
  GetCurrentPEB = (PEB * (*)())
      GetProcAddress(GetModuleHandle("ntdll.dll"), "RtlGetCurrentPeb");
  return GetCurrentPEB();
}
#else
#pragma warning(push)
#pragma warning(disable : 4035) // no return value
inline PEB *getCurrentPeb() {
  // 0x30 = offset of PEB in TIB
  _asm mov eax, dword ptr fs : [0x30]
}
#pragma warning(pop)
#endif // _M_X64

#endif // ProcessInfo_H_