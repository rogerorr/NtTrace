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

    $Revision: 2342 $
*/

// $Id: ProcessInfo.h 2342 2022-12-27 13:05:52Z roger $

#include "NtDllStruct.h"

// Forward references
struct _PEB;
typedef _PEB *PPEB;
struct _PEB_LDR_DATA;
typedef _PEB_LDR_DATA *PPEB_LDR_DATA;

// From ntddk.h
typedef enum _PROCESSINFOCLASS {
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
} PROCESSINFOCLASS;

/**
 * Thread priority
 */
typedef LONG KPRIORITY;

/**
 * Basic Process Information
 *
 * NtQueryInformationProcess using ProcessBasicInfo
 */
typedef struct _PROCESS_BASIC_INFORMATION {
  NTSTATUS ExitStatus;
  PPEB PebBaseAddress;
  ULONG_PTR AffinityMask;
  KPRIORITY BasePriority;
  ULONG_PTR UniqueProcessId;
  ULONG_PTR InheritedFromUniqueProcessId;
} PROCESS_BASIC_INFORMATION;

/** Pointer to PROCESS_BASIC_INFORMATION */
typedef PROCESS_BASIC_INFORMATION *PPROCESS_BASIC_INFORMATION;

// From
// http://cvs.sourceforge.net/viewcvs.py/mingw/w32api/include/ddk/ntapi.h?view=markup
typedef struct _RTL_USER_PROCESS_PARAMETERS {
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
} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;

/** typedef for the call to NtQueryInformationProcess */
typedef
    // NTOSAPI
    NTSTATUS NTAPI
    NtQueryInformationProcess(
        IN HANDLE ProcessHandle, ///< Handle to the process
        IN PROCESSINFOCLASS
            ProcessInformationClass,  ///< Class of information requested
        OUT PVOID ProcessInformation, ///< Buffer for returned information
        IN ULONG
            ProcessInformationLength, ///< Length of ProcessInformation buffer
        OUT PULONG ReturnLength OPTIONAL); ///< Returned length

/** typedef for the call to NtSetInformationProcess */
typedef NTSTATUS NTAPI NtSetInformationProcess(
    IN HANDLE ProcessHandle, ///< Handle to the process
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
typedef struct _PEB {
  BYTE Reserved1[2]; /*  00 */        ///< Reserved
  BYTE BeingDebugged; /*  02 */       ///< TRUE when process is being debugged
  BYTE Reserved2[1]; /*  03 */        ///< Reserved
  PVOID __pad4; /*  04 */             ///< Padding
  HMODULE ImageBaseAddress; /*  08 */ ///< Image base address
  PPEB_LDR_DATA LdrData; /*  0c */    ///< Loader data
  RTL_USER_PROCESS_PARAMETERS *ProcessParameters;
  /*  10 */                      ///< Process parameters
  PVOID __pad_14; /*  14 */      ///< Padding
  HANDLE ProcessHeap; /*  18 */  ///< Handle of the default process heap
  BYTE __pad_1c[236]; /*  1c */  ///< Padding
  PVOID Reserved3[51]; /* 108 */ ///< Reserved
  ULONG SessionId; /* 1d4 */     ///< The current session Id
} PEB, *PPEB;

/** Get access to PEB for current process */
#ifdef _M_X64
inline _PEB *getCurrentPeb() {
  _PEB *(*GetCurrentPEB)();
  GetCurrentPEB = (_PEB * (*)())
      GetProcAddress(GetModuleHandle("ntdll.dll"), "RtlGetCurrentPeb");
  return GetCurrentPEB();
}
#else
#pragma warning(push)
#pragma warning(disable : 4035) // no return value
inline _PEB *getCurrentPeb() {
  // 0x30 = offset of PEB in TIB
  _asm mov eax, dword ptr fs : [0x30]
}
#pragma warning(pop)
#endif // _M_X64

#endif // ProcessInfo_H_