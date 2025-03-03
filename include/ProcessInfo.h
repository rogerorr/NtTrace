#ifndef ProcessInfo_H_
#define ProcessInfo_H_

/**@file

    Header for Process information structures.

    @author Roger Orr <rogero@howzatt.co.uk>

    Copyright &copy; 2004,2011.
    This software is distributed in the hope that it will be useful, but
    without WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    Permission is granted to anyone to make or distribute verbatim
    copies of this software provided that the copyright notice and
    this permission notice are preserved, and that the distributor
    grants the recipient permission for further distribution as permitted
    by this notice.

    Comments and suggestions are always welcome.
    Please report bugs to rogero@howzatt.co.uk.

    $Revision: 2604 $
*/

// $Id: ProcessInfo.h 2604 2025-03-03 12:48:04Z roger $

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