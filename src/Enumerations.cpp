/*
NAME
    Enuerations.cpp

DESCRIPTION
    Class to define NT native enumerations for use by ShowData

COPYRIGHT
    Copyright (C) 2011 by Roger Orr <rogero@howzatt.demon.co.uk>

    This software is distributed in the hope that it will be useful, but
    without WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    Permission is granted to anyone to make or distribute verbatim
    copies of this software provided that the copyright notice and
    this permission notice are preserved, and that the distributor
    grants the recipient permission for further distribution as permitted
    by this notice.

    Comments and suggestions are always welcome.
    Please report bugs to rogero@howzatt.demon.co.uk.
*/

static char const szRCSID[] = "$Id: Enumerations.cpp 1405 2013-11-12 23:57:17Z Roger $";

#include "Enumerations.h"

namespace Enumerations
{
#define DEF(X) { (unsigned long)X, #X }

typedef enum _ALPC_MESSAGE_INFORMATION_CLASS {
  AlpcMessageSidInformation,
  AlpcMessageTokenModifiedIdInformation,
  MaxAlpcMessageInfoClass,
} ALPC_MESSAGE_INFORMATION_CLASS;

EnumMap ALPC_MESSAGE_INFORMATION_CLASS_MAP[] = 
{
  DEF(AlpcMessageSidInformation),
  DEF(AlpcMessageTokenModifiedIdInformation),
  DEF(MaxAlpcMessageInfoClass),
  {0, 0}
};


typedef enum ALPC_PORT_INFORMATION_CLASS {
  AlpcBasicInformation,
  AlpcPortInformation,
  AlpcAssociateCompletionPortInformation,
  AlpcConnectedSIDInformation,
  AlpcServerInformation,
  AlpcMessageZoneInformation,
  AlpcRegisterCompletionListInformation,
  AlpcUnregisterCompletionListInformation,
  AlpcAdjustCompletionListConcurrencyCountInformation,
  AlpcRegisterCallback,
  AlpcDisableCompletionList,
} ALPC_PORT_INFORMATION_CLASS;

EnumMap ALPC_PORT_INFORMATION_CLASS_MAP[] = 
{
  DEF(AlpcBasicInformation),
  DEF(AlpcPortInformation),
  DEF(AlpcAssociateCompletionPortInformation),
  DEF(AlpcConnectedSIDInformation),
  DEF(AlpcServerInformation),
  DEF(AlpcMessageZoneInformation),
  DEF(AlpcRegisterCompletionListInformation),
  DEF(AlpcUnregisterCompletionListInformation),
  DEF(AlpcAdjustCompletionListConcurrencyCountInformation),
  DEF(AlpcRegisterCallback),
  DEF(AlpcDisableCompletionList),
  {0, 0}
};


typedef enum _ATOM_INFORMATION_CLASS {
  AtomBasicInformation,
  AtomTableInformation,
} ATOM_INFORMATION_CLASS;

EnumMap ATOM_INFORMATION_CLASS_MAP[] = 
{
  DEF(AtomBasicInformation),
  DEF(AtomTableInformation),
  {0, 0}
};


typedef enum _AUDIT_EVENT_TYPE {
  AuditEventObjectAccess,
  AuditEventDirectoryServiceAccess,
} AUDIT_EVENT_TYPE;

EnumMap AUDIT_EVENT_TYPE_MAP[] = 
{
  DEF(AuditEventObjectAccess),
  DEF(AuditEventDirectoryServiceAccess),
  {0, 0}
};


typedef enum _DEBUGOBJECTINFOCLASS {
  DebugObjectKillOnExit = 1,
} DEBUGOBJECTINFOCLASS;

EnumMap DEBUGOBJECTINFOCLASS_MAP[] =
{
  DEF(DebugObjectKillOnExit),
  {0, 0}
};


typedef enum _DEBUG_CONTROL_CODE {
  DebugGetTraceInformation = 1,
  DebugSetInternalBreakpoint,
  DebugSetSpecialCall,
  DebugClearSpecialCalls,
  DebugQuerySpecialCalls,
  DebugDbgBreakPoint,
  DebugDbgLoadSymbols,
  DebugSysReadPhysicalMemory = 10,
  DebugSysReadIoSpace = 14,
  DebugSysWriteIoSpace = 15,
} DEBUG_CONTROL_CODE;

EnumMap DEBUG_CONTROL_CODE_MAP[] =
{
  DEF(DebugGetTraceInformation),
  DEF(DebugSetInternalBreakpoint),
  DEF(DebugSetSpecialCall),
  DEF(DebugClearSpecialCalls),
  DEF(DebugQuerySpecialCalls),
  DEF(DebugDbgBreakPoint),
  DEF(DebugDbgLoadSymbols),
  DEF(DebugSysReadPhysicalMemory),
  DEF(DebugSysReadIoSpace),
  DEF(DebugSysWriteIoSpace),
  {0, 0}
};


typedef enum _DEVICE_POWER_STATE { 
  PowerDeviceUnspecified,
  PowerDeviceD0,
  PowerDeviceD1,
  PowerDeviceD2,
  PowerDeviceD3,
} DEVICE_POWER_STATE;

EnumMap DEVICE_POWER_STATE_MAP[] = {
  DEF(PowerDeviceUnspecified),
  DEF(PowerDeviceD0),
  DEF(PowerDeviceD1),
  DEF(PowerDeviceD2),
  DEF(PowerDeviceD3),
  {0, 0}
};


typedef enum _ENLISTMENT_INFORMATION_CLASS {
  EnlistmentBasicInformation,
  EnlistmentRecoveryInformation,
  EnlistmentCrmInformation,
} ENLISTMENT_INFORMATION_CLASS;

EnumMap ENLISTMENT_INFORMATION_CLASS_MAP[] = 
{
  DEF(EnlistmentBasicInformation),
  DEF(EnlistmentRecoveryInformation),
  DEF(EnlistmentCrmInformation),
  {0, 0}
};


typedef enum _EVENT_INFORMATION_CLASS {
  EventBasicInformation,
} EVENT_INFORMATION_CLASS;

EnumMap EVENT_INFORMATION_CLASS_MAP[] = 
{
  DEF(EventBasicInformation),
  {0, 0}
};


typedef enum _EVENT_TYPE {
  NotificationEvent,
  SynchronizationEvent,
} EVENT_TYPE;

EnumMap EVENT_TYPE_MAP[] =
{
  DEF(NotificationEvent),
  DEF(SynchronizationEvent),
  {0, 0}
};


typedef enum _FILE_INFORMATION_CLASS {
  FileDirectoryInformation=1,
  FileFullDirectoryInformation,
  FileBothDirectoryInformation,
  FileBasicInformation,
  FileStandardInformation,
  FileInternalInformation,
  FileEaInformation,
  FileAccessInformation,
  FileNameInformation,
  FileRenameInformation,
  FileLinkInformation,
  FileNamesInformation,
  FileDispositionInformation,
  FilePositionInformation,
  FileFullEaInformation,
  FileModeInformation,
  FileAlignmentInformation,
  FileAllInformation,
  FileAllocationInformation,
  FileEndOfFileInformation,
  FileAlternateNameInformation,
  FileStreamInformation,
  FilePipeInformation,
  FilePipeLocalInformation,
  FilePipeRemoteInformation,
  FileMailslotQueryInformation,
  FileMailslotSetInformation,
  FileCompressionInformation,
  FileObjectIdInformation,
  FileCompletionInformation,
  FileMoveClusterInformation,
  FileQuotaInformation,
  FileReparsePointInformation,
  FileNetworkOpenInformation,
  FileAttributeTagInformation,
  FileTrackingInformation,
  FileIdBothDirectoryInformation,
  FileIdFullDirectoryInformation,
  FileValidDataLengthInformation,
  FileShortNameInformation,
  FileIoCompletionNotificationInformation,
  FileIoStatusBlockRangeInformation,
  FileIoPriorityHintInformation,
  FileSfioReserveInformation,
  FileSfioVolumeInformation,
  FileHardLinkInformation,
  FileProcessIdsUsingFileInformation,
  FileNormalizedNameInformation,
  FileNetworkPhysicalNameInformation,
  FileIdGlobalTxDirectoryInformation,
  FileIsRemoteDeviceInformation,
  FileAttributeCacheInformation,
  FileNumaNodeInformation,
  FileStandardLinkInformation,
  FileRemoteProtocolInformation,
} FILE_INFORMATION_CLASS;

EnumMap FILE_INFORMATION_CLASS_MAP[] = 
{
  DEF(FileDirectoryInformation),
  DEF(FileFullDirectoryInformation),
  DEF(FileBothDirectoryInformation),
  DEF(FileBasicInformation),
  DEF(FileStandardInformation),
  DEF(FileInternalInformation),
  DEF(FileEaInformation),
  DEF(FileAccessInformation),
  DEF(FileNameInformation),
  DEF(FileRenameInformation),
  DEF(FileLinkInformation),
  DEF(FileNamesInformation),
  DEF(FileDispositionInformation),
  DEF(FilePositionInformation),
  DEF(FileFullEaInformation),
  DEF(FileModeInformation),
  DEF(FileAlignmentInformation),
  DEF(FileAllInformation),
  DEF(FileAllocationInformation),
  DEF(FileEndOfFileInformation),
  DEF(FileAlternateNameInformation),
  DEF(FileStreamInformation),
  DEF(FilePipeInformation),
  DEF(FilePipeLocalInformation),
  DEF(FilePipeRemoteInformation),
  DEF(FileMailslotQueryInformation),
  DEF(FileMailslotSetInformation),
  DEF(FileCompressionInformation),
  DEF(FileObjectIdInformation),
  DEF(FileCompletionInformation),
  DEF(FileMoveClusterInformation),
  DEF(FileQuotaInformation),
  DEF(FileReparsePointInformation),
  DEF(FileNetworkOpenInformation),
  DEF(FileAttributeTagInformation),
  DEF(FileTrackingInformation),
  DEF(FileIdBothDirectoryInformation),
  DEF(FileIdFullDirectoryInformation),
  DEF(FileValidDataLengthInformation),
  DEF(FileShortNameInformation),
  DEF(FileIoCompletionNotificationInformation),
  DEF(FileIoStatusBlockRangeInformation),
  DEF(FileIoPriorityHintInformation),
  DEF(FileSfioReserveInformation),
  DEF(FileSfioVolumeInformation),
  DEF(FileHardLinkInformation),
  DEF(FileProcessIdsUsingFileInformation),
  DEF(FileNormalizedNameInformation),
  DEF(FileNetworkPhysicalNameInformation),
  DEF(FileIdGlobalTxDirectoryInformation),
  DEF(FileIsRemoteDeviceInformation),
  DEF(FileAttributeCacheInformation),
  DEF(FileNumaNodeInformation),
  DEF(FileStandardLinkInformation),
  DEF(FileRemoteProtocolInformation),
  {0, 0}
};


typedef enum _FS_INFORMATION_CLASS {
  FsVolumeInformation = 1,
  FsLabelInformation,
  FsSizeInformation,
  FsDeviceInformation,
  FsAttributeInformation,
  FsControlInformation,
  FsFullSizeInformation,
  FsObjectIdInformation,
  FsDriverPathInformation,
  FsVolumeFlagsInformation,
  FsSectorSizeInformation,
} FS_INFORMATION_CLASS;

EnumMap FS_INFORMATION_CLASS_MAP[] = 
{
  DEF(FsVolumeInformation),
  DEF(FsLabelInformation),
  DEF(FsSizeInformation),
  DEF(FsDeviceInformation),
  DEF(FsAttributeInformation),
  DEF(FsControlInformation),
  DEF(FsFullSizeInformation),
  DEF(FsObjectIdInformation),
  DEF(FsDriverPathInformation),
  DEF(FsVolumeFlagsInformation),
  DEF(FsSectorSizeInformation),
  {0, 0}
};


typedef enum _HARDERROR_RESPONSE_OPTION {
  OptionAbortRetryIgnore,
  OptionOk,
  OptionOkCancel,
  OptionRetryCancel,
  OptionYesNo,
  OptionYesNoCancel,
  OptionShutdownSystem,
} HARDERROR_RESPONSE_OPTION;

EnumMap HARDERROR_RESPONSE_OPTION_MAP[] = {
  DEF(OptionAbortRetryIgnore),
  DEF(OptionOk),
  DEF(OptionOkCancel),
  DEF(OptionRetryCancel),
  DEF(OptionYesNo),
  DEF(OptionYesNoCancel),
  DEF(OptionShutdownSystem),
  {0, 0}
};


typedef enum _IO_SESSION_STATE {
  IoSessionStateCreated = 1,
  IoSessionStateInitialized,
  IoSessionStateConnected,
  IoSessionStateDisconnected,
  IoSessionStateDisconnectedLoggedOn,
  IoSessionStateLoggedOn,
  IoSessionStateLoggedOff,
  IoSessionStateTerminated,
} IO_SESSION_STATE;

EnumMap IO_SESSION_STATE_MAP[] =
{
  DEF(IoSessionStateCreated),
  DEF(IoSessionStateInitialized),
  DEF(IoSessionStateConnected),
  DEF(IoSessionStateDisconnected),
  DEF(IoSessionStateDisconnectedLoggedOn),
  DEF(IoSessionStateLoggedOn),
  DEF(IoSessionStateLoggedOff),
  DEF(IoSessionStateTerminated),
  {0, 0}
};

typedef enum _JOB_INFORMATION_CLASS {
  JobObjectBasicAccountingInformation = 1,
  JobObjectBasicLimitInformation,
  JobObjectBasicProcessIdList,
  JobObjectBasicUIRestrictions,
  JobObjectSecurityLimitInformation,
  JobObjectEndOfJobTimeInformation,
  JobObjectAssociateCompletionPortInformation,
  JobObjectBasicAndIoAccountingInformation,
  JobObjectExtendedLimitInformation,
  JobObjectJobSetInformation,
  JobObjectGroupInformation,
  JobObjectNotificationLimitInformation,
  JobObjectLimitViolationInformation,
  JobObjectGroupInformationEx,
  JobObjectCpuRateControlInformation,
} JOB_INFORMATION_CLASS;

EnumMap JOB_INFORMATION_CLASS_MAP[] = 
{
  DEF(JobObjectBasicAccountingInformation),
  DEF(JobObjectBasicLimitInformation),
  DEF(JobObjectBasicProcessIdList),
  DEF(JobObjectBasicUIRestrictions),
  DEF(JobObjectSecurityLimitInformation),
  DEF(JobObjectEndOfJobTimeInformation),
  DEF(JobObjectAssociateCompletionPortInformation),
  DEF(JobObjectBasicAndIoAccountingInformation),
  DEF(JobObjectExtendedLimitInformation),
  DEF(JobObjectJobSetInformation),
  DEF(JobObjectGroupInformation),
  DEF(JobObjectNotificationLimitInformation),
  DEF(JobObjectLimitViolationInformation),
  DEF(JobObjectGroupInformationEx),
  DEF(JobObjectCpuRateControlInformation),
  {0, 0}
};


typedef enum _KEY_INFORMATION_CLASS {
  KeyBasicInformation,
  KeyNodeInformation,
  KeyFullInformation,
  KeyNameInformation,
  KeyCachedInformation,
  KeyFlagsInformation,
  KeyVirtualizationInformation,
  KeyHandleTagsInformation,
} KEY_INFORMATION_CLASS;

EnumMap KEY_INFORMATION_CLASS_MAP[] = 
{
  DEF(KeyBasicInformation),
  DEF(KeyNodeInformation),
  DEF(KeyFullInformation),
  DEF(KeyNameInformation),
  DEF(KeyCachedInformation),
  DEF(KeyFlagsInformation),
  DEF(KeyVirtualizationInformation),
  DEF(KeyHandleTagsInformation),
  {0, 0}
};


typedef enum _KEY_SET_INFORMATION_CLASS {
  KeyWriteTimeInformation,
  KeyWow64FlagsInformation,
  KeyControlFlagsInformation,
  KeySetVirtualizationInformation,
  KeySetDebugInformation,
  KeySetHandleTagsInformation,
} KEY_SET_INFORMATION_CLASS;

EnumMap KEY_SET_INFORMATION_CLASS_MAP[] = 
{
  DEF(KeyWriteTimeInformation),
  DEF(KeyWow64FlagsInformation),
  DEF(KeyControlFlagsInformation),
  DEF(KeySetVirtualizationInformation),
  DEF(KeySetDebugInformation),
  DEF(KeySetHandleTagsInformation),
  {0, 0}
};


typedef enum _KEY_VALUE_INFORMATION_CLASS {
  KeyValueBasicInformation,
  KeyValueFullInformation,
  KeyValuePartialInformation,
  KeyValueFullInformationAlign64,
  KeyValuePartialInformationAlign64,
} KEY_VALUE_INFORMATION_CLASS;

EnumMap KEY_VALUE_INFORMATION_CLASS_MAP[] = 
{
  DEF(KeyValueBasicInformation),
  DEF(KeyValueFullInformation),
  DEF(KeyValuePartialInformation),
  DEF(KeyValueFullInformationAlign64),
  DEF(KeyValuePartialInformationAlign64),
  {0, 0}
};


typedef enum _KPROFILE_SOURCE {
  ProfileTime,
  ProfileAlignmentFixup,
  ProfileTotalIssues,
  ProfilePipelineDry,
  ProfileLoadInstructions,
  ProfilePipelineFrozen,
  ProfileBranchInstructions,
  ProfileTotalNonissues,
  ProfileDcacheMisses,
  ProfileIcacheMisses,
  ProfileCacheMisses,
  ProfileBranchMispredictions,
  ProfileStoreInstructions,
  ProfileFpInstructions,
  ProfileIntegerInstructions,
  Profile2Issue,
  Profile3Issue,
  Profile4Issue,
  ProfileSpecialInstructions,
  ProfileTotalCycles,
  ProfileIcacheIssues,
  ProfileDcacheAccesses,
  ProfileMemoryBarrierCycles,
  ProfileLoadLinkedIssues,
} KPROFILE_SOURCE;

EnumMap KPROFILE_SOURCE_MAP[] = 
{
  DEF(ProfileTime),
  DEF(ProfileAlignmentFixup),
  DEF(ProfileTotalIssues),
  DEF(ProfilePipelineDry),
  DEF(ProfileLoadInstructions),
  DEF(ProfilePipelineFrozen),
  DEF(ProfileBranchInstructions),
  DEF(ProfileTotalNonissues),
  DEF(ProfileDcacheMisses),
  DEF(ProfileIcacheMisses),
  DEF(ProfileCacheMisses),
  DEF(ProfileBranchMispredictions),
  DEF(ProfileStoreInstructions),
  DEF(ProfileFpInstructions),
  DEF(ProfileIntegerInstructions),
  DEF(Profile2Issue),
  DEF(Profile3Issue),
  DEF(Profile4Issue),
  DEF(ProfileSpecialInstructions),
  DEF(ProfileTotalCycles),
  DEF(ProfileIcacheIssues),
  DEF(ProfileDcacheAccesses),
  DEF(ProfileMemoryBarrierCycles),
  DEF(ProfileLoadLinkedIssues),
  {0, 0}
};


typedef enum _KTMOBJECT_TYPE {
  KTMOBJECT_TRANSACTION,
  KTMOBJECT_TRANSACTION_MANAGER,
  KTMOBJECT_RESOURCE_MANAGER,
  KTMOBJECT_ENLISTMENT,
} KTMOBJECT_TYPE;

EnumMap KTMOBJECT_TYPE_MAP[] = 
{
  DEF(KTMOBJECT_TRANSACTION),
  DEF(KTMOBJECT_TRANSACTION_MANAGER),
  DEF(KTMOBJECT_RESOURCE_MANAGER),
  DEF(KTMOBJECT_ENLISTMENT),
  {0, 0}
};


/** Type of LPC messages */
typedef enum _LPC_TYPE {
  LPC_INIT, // (Not in kdexts.dll)
  LPC_REQUEST,
  LPC_REPLY,
  LPC_DATAGRAM,
  LPC_LOST_REPLY,
  LPC_PORT_CLOSED,
  LPC_CLIENT_DIED,
  LPC_EXCEPTION,
  LPC_DEBUG_EVENT,
  LPC_ERROR_EVENT,
  LPC_CONNECTION_REQUEST,
  LPC_CONNECTION_REPLY,
  LPC_CANCELED,
  LPC_UNREGISTER_PROCESS,
} LPC_TYPE;

EnumMap LPC_TYPE_MAP[] = 
{
  DEF(LPC_INIT),
  DEF(LPC_REQUEST),
  DEF(LPC_REPLY),
  DEF(LPC_DATAGRAM),
  DEF(LPC_LOST_REPLY),
  DEF(LPC_PORT_CLOSED),
  DEF(LPC_CLIENT_DIED),
  DEF(LPC_EXCEPTION),
  DEF(LPC_DEBUG_EVENT),
  DEF(LPC_ERROR_EVENT),
  DEF(LPC_CONNECTION_REQUEST),
  DEF(LPC_CONNECTION_REPLY),
  DEF(LPC_CANCELED),
  DEF(LPC_UNREGISTER_PROCESS),
  {0, 0}
};


typedef enum _MEMORY_INFORMATION_CLASS {
  MemoryBasicInformation,
} MEMORY_INFORMATION_CLASS;

EnumMap MEMORY_INFORMATION_CLASS_MAP[] = 
{
  DEF(MemoryBasicInformation),
  {0, 0}
};


typedef enum _MUTANT_INFORMATION_CLASS {
  MutantBasicInformation,
} MUTANT_INFORMATION_CLASS;

EnumMap MUTANT_INFORMATION_CLASS_MAP[] = 
{
  DEF(MutantBasicInformation),
  {0, 0}
};

typedef enum _NOTIFICATION_MASK {
  TRANSACTION_NOTIFY_PREPREPARE = 0x00000001,
  TRANSACTION_NOTIFY_PREPARE = 0x00000002,
  TRANSACTION_NOTIFY_COMMIT = 0x00000004,
  TRANSACTION_NOTIFY_ROLLBACK = 0x00000008,
  TRANSACTION_NOTIFY_PREPREPARE_COMPLETE = 0x00000010,
  TRANSACTION_NOTIFY_PREPARE_COMPLETE = 0x00000020,
  TRANSACTION_NOTIFY_COMMIT_COMPLETE = 0x00000040,
  TRANSACTION_NOTIFY_ROLLBACK_COMPLETE = 0x00000080,
  TRANSACTION_NOTIFY_RECOVER = 0x00000100,
  TRANSACTION_NOTIFY_SINGLE_PHASE_COMMIT = 0x00000200,
  TRANSACTION_NOTIFY_DELEGATE_COMMIT = 0x00000400,
  TRANSACTION_NOTIFY_RECOVER_QUERY = 0x00000800,
  TRANSACTION_NOTIFY_ENLIST_PREPREPARE = 0x00001000,
  TRANSACTION_NOTIFY_LAST_RECOVER = 0x00002000,
  TRANSACTION_NOTIFY_INDOUBT = 0x00004000,
  TRANSACTION_NOTIFY_TM_ONLINE = 0x02000000,
  TRANSACTION_NOTIFY_REQUEST_OUTCOME = 0x20000000,
  TRANSACTION_NOTIFY_COMMIT_FINALIZE = 0x40000000,
} NOTIFICATION_MASK;

EnumMap NOTIFICATION_MASK_MAP[] = 
{
  DEF(TRANSACTION_NOTIFY_PREPREPARE),
  DEF(TRANSACTION_NOTIFY_PREPARE),
  DEF(TRANSACTION_NOTIFY_COMMIT),
  DEF(TRANSACTION_NOTIFY_ROLLBACK),
  DEF(TRANSACTION_NOTIFY_PREPREPARE_COMPLETE),
  DEF(TRANSACTION_NOTIFY_PREPARE_COMPLETE),
  DEF(TRANSACTION_NOTIFY_COMMIT_COMPLETE),
  DEF(TRANSACTION_NOTIFY_ROLLBACK_COMPLETE),
  DEF(TRANSACTION_NOTIFY_RECOVER),
  DEF(TRANSACTION_NOTIFY_SINGLE_PHASE_COMMIT),
  DEF(TRANSACTION_NOTIFY_DELEGATE_COMMIT),
  DEF(TRANSACTION_NOTIFY_RECOVER_QUERY),
  DEF(TRANSACTION_NOTIFY_ENLIST_PREPREPARE),
  DEF(TRANSACTION_NOTIFY_LAST_RECOVER),
  DEF(TRANSACTION_NOTIFY_INDOUBT),
  DEF(TRANSACTION_NOTIFY_TM_ONLINE),
  DEF(TRANSACTION_NOTIFY_REQUEST_OUTCOME),
  DEF(TRANSACTION_NOTIFY_COMMIT_FINALIZE),
  {0, 0}
};


typedef enum _OBJECT_INFORMATION_CLASS {
  ObjectBasicInformation,
  ObjectNameInformation,
  ObjectTypeInformation,
} OBJECT_INFORMATION_CLASS;

EnumMap OBJECT_INFORMATION_CLASS_MAP[] = 
{
  DEF(ObjectBasicInformation),
  DEF(ObjectNameInformation),
  DEF(ObjectTypeInformation),
  {0, 0}
};


typedef enum _PORT_INFORMATION_CLASS {
  PortBasicInformation,
} PORT_INFORMATION_CLASS;

EnumMap PORT_INFORMATION_CLASS_MAP[] =
{
  DEF(PortBasicInformation),
  {0, 0}
};


typedef enum _POWER_ACTION {
  PowerActionNone,
  PowerActionReserved,
  PowerActionSleep,
  PowerActionHibernate,
  PowerActionShutdown,
  PowerActionShutdownReset,
  PowerActionShutdownOff,
  PowerActionWarmEject,
} POWER_ACTION;

EnumMap POWER_ACTION_MAP[] =
{
  DEF(PowerActionNone),
  DEF(PowerActionReserved),
  DEF(PowerActionSleep),
  DEF(PowerActionHibernate),
  DEF(PowerActionShutdown),
  DEF(PowerActionShutdownReset),
  DEF(PowerActionShutdownOff),
  DEF(PowerActionWarmEject),
  {0, 0}
};


typedef enum _POWER_INFORMATION_LEVEL {
  SystemPowerPolicyAc,
  SystemPowerPolicyDc,
  VerifySystemPolicyAc,
  VerifySystemPolicyDc,
  SystemPowerCapabilities,
  SystemBatteryState,
  SystemPowerStateHandler,
  ProcessorStateHandler,
  SystemPowerPolicyCurrent,
  AdministratorPowerPolicy,
  SystemReserveHiberFile,
  ProcessorInformation,
  SystemPowerInformation,
  ProcessorStateHandler2,
  LastWakeTime,
  LastSleepTime,
  SystemExecutionState,
  SystemPowerStateNotifyHandler,
  ProcessorPowerPolicyAc,
  ProcessorPowerPolicyDc,
  VerifyProcessorPowerPolicyAc,
  VerifyProcessorPowerPolicyDc,
  ProcessorPowerPolicyCurrent,
} POWER_INFORMATION_LEVEL;

EnumMap POWER_INFORMATION_LEVEL_MAP[] =
{
  DEF(SystemPowerPolicyAc),
  DEF(SystemPowerPolicyDc),
  DEF(VerifySystemPolicyAc),
  DEF(VerifySystemPolicyDc),
  DEF(SystemPowerCapabilities),
  DEF(SystemBatteryState),
  DEF(SystemPowerStateHandler),
  DEF(ProcessorStateHandler),
  DEF(SystemPowerPolicyCurrent),
  DEF(AdministratorPowerPolicy),
  DEF(SystemReserveHiberFile),
  DEF(ProcessorInformation),
  DEF(SystemPowerInformation),
  DEF(ProcessorStateHandler2),
  DEF(LastWakeTime),
  DEF(LastSleepTime),
  DEF(SystemExecutionState),
  DEF(SystemPowerStateNotifyHandler),
  DEF(ProcessorPowerPolicyAc),
  DEF(ProcessorPowerPolicyDc),
  DEF(VerifyProcessorPowerPolicyAc),
  DEF(VerifyProcessorPowerPolicyDc),
  DEF(ProcessorPowerPolicyCurrent),
  {0, 0}
};


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
  ProcessIoPortHandlers,
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
} PROCESSINFOCLASS;

EnumMap PROCESSINFOCLASS_MAP[] = 
{
  DEF(ProcessBasicInformation),
  DEF(ProcessQuotaLimits),
  DEF(ProcessIoCounters),
  DEF(ProcessVmCounters),
  DEF(ProcessTimes),
  DEF(ProcessBasePriority),
  DEF(ProcessRaisePriority),
  DEF(ProcessDebugPort),
  DEF(ProcessExceptionPort),
  DEF(ProcessAccessToken),
  DEF(ProcessLdtInformation),
  DEF(ProcessLdtSize),
  DEF(ProcessDefaultHardErrorMode),
  DEF(ProcessIoPortHandlers),
  DEF(ProcessPooledUsageAndLimits),
  DEF(ProcessWorkingSetWatch),
  DEF(ProcessUserModeIOPL),
  DEF(ProcessEnableAlignmentFaultFixup),
  DEF(ProcessPriorityClass),
  DEF(ProcessWx86Information),
  DEF(ProcessHandleCount),
  DEF(ProcessAffinityMask),
  DEF(ProcessPriorityBoost),
  DEF(ProcessDeviceMap),
  DEF(ProcessSessionInformation),
  DEF(ProcessForegroundInformation),
  DEF(ProcessWow64Information),
  DEF(ProcessImageFileName),
  DEF(ProcessLUIDDeviceMapsEnabled),
  DEF(ProcessBreakOnTermination),
  DEF(ProcessDebugObjectHandle),
  DEF(ProcessDebugFlags),
  DEF(ProcessHandleTracing),
  DEF(ProcessIoPriority),
  DEF(ProcessExecuteFlags),
  DEF(ProcessTlsInformation),
  DEF(ProcessCookie),
  DEF(ProcessImageInformation),
  DEF(ProcessCycleTime),
  DEF(ProcessPagePriority),
  DEF(ProcessInstrumentationCallback),
  DEF(ProcessThreadStackAllocation),
  DEF(ProcessWorkingSetWatchEx),
  DEF(ProcessImageFileNameWin32),
  DEF(ProcessImageFileMapping),
  DEF(ProcessAffinityUpdateMode),
  DEF(ProcessMemoryAllocationMode),
  DEF(ProcessGroupInformation),
  DEF(ProcessTokenVirtualizationEnabled),
  DEF(ProcessConsoleHostProcess),
  DEF(ProcessWindowInformation),
  {0, 0}
};


typedef enum _RESOURCEMANAGER_INFORMATION_CLASS {
  ResourceManagerBasicInformation        = 0,
  ResourceManagerCompletionInformation   = 1,
} RESOURCEMANAGER_INFORMATION_CLASS;

EnumMap RESOURCEMANAGER_INFORMATION_CLASS_MAP[] = 
{
  DEF(ResourceManagerBasicInformation),
  DEF(ResourceManagerCompletionInformation),
  {0, 0}
};


typedef enum _SECTION_INFORMATION_CLASS {
  SectionBasicInformation,
  SectionImageInformation,
} SECTION_INFORMATION_CLASS;

EnumMap SECTION_INFORMATION_CLASS_MAP[] = 
{
  DEF(SectionBasicInformation),
  DEF(SectionImageInformation),
  {0, 0}
};


typedef enum _SECURITY_INFORMATION {
  OWNER_SECURITY_INFORMATION = 0x00000001,
  GROUP_SECURITY_INFORMATION = 0x00000002,
  DACL_SECURITY_INFORMATION = 0x00000004,
  SACL_SECURITY_INFORMATION = 0x00000008,
  LABEL_SECURITY_INFORMATION = 0x00000010,
  UNPROTECTED_SACL_SECURITY_INFORMATION = 0x10000000,
  UNPROTECTED_DACL_SECURITY_INFORMATION = 0x20000000,
  PROTECTED_SACL_SECURITY_INFORMATION = 0x40000000,
  PROTECTED_DACL_SECURITY_INFORMATION = 0x80000000,
} SECURITY_INFORMATION;

EnumMap SECURITY_INFORMATION_MAP[] = 
{
  DEF(OWNER_SECURITY_INFORMATION),
  DEF(GROUP_SECURITY_INFORMATION),
  DEF(DACL_SECURITY_INFORMATION),
  DEF(SACL_SECURITY_INFORMATION),
  DEF(LABEL_SECURITY_INFORMATION),
  DEF(UNPROTECTED_SACL_SECURITY_INFORMATION),
  DEF(UNPROTECTED_DACL_SECURITY_INFORMATION),
  DEF(PROTECTED_SACL_SECURITY_INFORMATION),
  DEF(PROTECTED_DACL_SECURITY_INFORMATION),
  {0, 0}
};


typedef enum _SECTION_INHERIT {
  ViewShare=1,
  ViewUnmap,
} SECTION_INHERIT;

EnumMap SECTION_INHERIT_MAP[] = 
{
  DEF(ViewShare),
  DEF(ViewUnmap),
  {0, 0}
};


typedef enum _SEMAPHORE_INFORMATION_CLASS {
  SemaphoreBasicInformation,
} SEMAPHORE_INFORMATION_CLASS;

EnumMap SEMAPHORE_INFORMATION_CLASS_MAP[] = 
{
  DEF(SemaphoreBasicInformation),
  {0, 0}
};


typedef enum _SHUTDOWN_ACTION {
  ShutdownNoReboot,
  ShutdownReboot,
  ShutdownPowerOff,
} SHUTDOWN_ACTION;

EnumMap SHUTDOWN_ACTION_MAP[] = 
{
  DEF(ShutdownNoReboot),
  DEF(ShutdownReboot),
  DEF(ShutdownPowerOff),
  {0, 0}
};


typedef enum _SYSTEM_POWER_STATE {
  PowerSystemUnspecified,
  PowerSystemWorking,
  PowerSystemSleeping1,
  PowerSystemSleeping2,
  PowerSystemSleeping3,
  PowerSystemHibernate,
  PowerSystemShutdown,
} SYSTEM_POWER_STATE;

EnumMap SYSTEM_POWER_STATE_MAP[] =
{
  DEF(PowerSystemUnspecified),
  DEF(PowerSystemWorking),
  DEF(PowerSystemSleeping1),
  DEF(PowerSystemSleeping2),
  DEF(PowerSystemSleeping3),
  DEF(PowerSystemHibernate),
  DEF(PowerSystemShutdown),
  {0, 0}
};


typedef enum _SYSTEM_INFORMATION_CLASS {
  SystemBasicInformation,
  SystemProcessorInformation,
  SystemPerformanceInformation,
  SystemTimeOfDayInformation,
  SystemPathInformation,
  SystemProcessInformation,
  SystemCallCountInformation,
  SystemDeviceInformation,
  SystemProcessorPerformanceInformation,
  SystemFlagsInformation,
  SystemCallTimeInformation,
  SystemModuleInformation,
  SystemLocksInformation,
  SystemStackTraceInformation,
  SystemPagedPoolInformation,
  SystemNonPagedPoolInformation,
  SystemHandleInformation,
  SystemObjectInformation,
  SystemPageFileInformation,
  SystemVdmInstemulInformation,
  SystemVdmBopInformation,
  SystemFileCacheInformation,
  SystemPoolTagInformation,
  SystemInterruptInformation,
  SystemDpcBehaviorInformation,
  SystemFullMemoryInformation,
  SystemLoadGdiDriverInformation,
  SystemUnloadGdiDriverInformation,
  SystemTimeAdjustmentInformation,
  SystemSummaryMemoryInformation,
  SystemMirrorMemoryInformation,
  SystemPerformanceTraceInformation,
  SystemCrashDumpInformation,
  SystemExceptionInformation,
  SystemCrashDumpStateInformation,
  SystemKernelDebuggerInformation,
  SystemContextSwitchInformation,
  SystemRegistryQuotaInformation,
  SystemExtendServiceTableInformation,
  SystemPrioritySeperation,
  SystemVerifierAddDriverInformation,
  SystemVerifierRemoveDriverInformation,
  SystemProcessorIdleInformation,
  SystemLegacyDriverInformation,
  SystemCurrentTimeZoneInformation,
  SystemLookasideInformation,
  SystemTimeSlipNotification,
  SystemSessionCreate,
  SystemSessionDetach,
  SystemSessionInformation,
  SystemRangeStartInformation,
  SystemVerifierInformation,
  SystemVerifierThunkExtend,
  SystemSessionProcessInformation,
  SystemLoadGdiDriverInSystemSpace,
  SystemNumaProcessorMap,
  SystemPrefetcherInformation,
  SystemExtendedProcessInformation,
  SystemRecommendedSharedDataAlignment,
  SystemComPlusPackage,
  SystemNumaAvailableMemory,
  SystemProcessorPowerInformation,
  SystemEmulationBasicInformation,
  SystemEmulationProcessorInformation,
  SystemExtendedHandleInformation,
  SystemLostDelayedWriteInformation,
  SystemBigPoolInformation,
  SystemSessionPoolTagInformation,
  SystemSessionMappedViewInformation,
  SystemHotpatchInformation,
  SystemObjectSecurityMode,
  SystemWatchdogTimerHandler,
  SystemWatchdogTimerInformation,
  SystemLogicalProcessorInformation,
  SystemWow64SharedInformationObsolete,
  SystemRegisterFirmwareTableInformationHandler,
  SystemFirmwareTableInformation,
  SystemModuleInformationEx,
  SystemVerifierTriageInformation,
  SystemSuperfetchInformation,
  SystemMemoryListInformation,
  SystemFileCacheInformationEx,
  SystemThreadPriorityClientIdInformation,
  SystemProcessorIdleCycleTimeInformation,
  SystemVerifierCancellationInformation,
  SystemProcessorPowerInformationEx,
  SystemRefTraceInformation,
  SystemSpecialPoolInformation,
  SystemProcessIdInformation,
  SystemErrorPortInformation,
  SystemBootEnvironmentInformation,
  SystemHypervisorInformation,
  SystemVerifierInformationEx,
  SystemTimeZoneInformation,
  SystemImageFileExecutionOptionsInformation,
  SystemCoverageInformation,
  SystemPrefetchPathInformation,
  SystemVerifierFaultsInformation,
  SystemSystemPartitionInformation,
  SystemSystemDiskInformation,
  SystemProcessorPerformanceDistribution,
  SystemNumaProximityNodeInformation,
  SystemDynamicTimeZoneInformation,
  SystemCodeIntegrityInformation,
  SystemProcessorMicrocodeUpdateInformation,
  SystemProcessorBrandString,
  SystemVirtualAddressInformation,
} SYSTEM_INFORMATION_CLASS;

EnumMap SYSTEM_INFORMATION_CLASS_MAP[] = 
{
  DEF(SystemBasicInformation),
  DEF(SystemProcessorInformation),
  DEF(SystemPerformanceInformation),
  DEF(SystemTimeOfDayInformation),
  DEF(SystemPathInformation),
  DEF(SystemProcessInformation),
  DEF(SystemCallCountInformation),
  DEF(SystemDeviceInformation),
  DEF(SystemProcessorPerformanceInformation),
  DEF(SystemFlagsInformation),
  DEF(SystemCallTimeInformation),
  DEF(SystemModuleInformation),
  DEF(SystemLocksInformation),
  DEF(SystemStackTraceInformation),
  DEF(SystemPagedPoolInformation),
  DEF(SystemNonPagedPoolInformation),
  DEF(SystemHandleInformation),
  DEF(SystemObjectInformation),
  DEF(SystemPageFileInformation),
  DEF(SystemVdmInstemulInformation),
  DEF(SystemVdmBopInformation),
  DEF(SystemFileCacheInformation),
  DEF(SystemPoolTagInformation),
  DEF(SystemInterruptInformation),
  DEF(SystemDpcBehaviorInformation),
  DEF(SystemFullMemoryInformation),
  DEF(SystemLoadGdiDriverInformation),
  DEF(SystemUnloadGdiDriverInformation),
  DEF(SystemTimeAdjustmentInformation),
  DEF(SystemSummaryMemoryInformation),
  DEF(SystemMirrorMemoryInformation),
  DEF(SystemPerformanceTraceInformation),
  DEF(SystemCrashDumpInformation),
  DEF(SystemExceptionInformation),
  DEF(SystemCrashDumpStateInformation),
  DEF(SystemKernelDebuggerInformation),
  DEF(SystemContextSwitchInformation),
  DEF(SystemRegistryQuotaInformation),
  DEF(SystemExtendServiceTableInformation),
  DEF(SystemPrioritySeperation),
  DEF(SystemVerifierAddDriverInformation),
  DEF(SystemVerifierRemoveDriverInformation),
  DEF(SystemProcessorIdleInformation),
  DEF(SystemLegacyDriverInformation),
  DEF(SystemCurrentTimeZoneInformation),
  DEF(SystemLookasideInformation),
  DEF(SystemTimeSlipNotification),
  DEF(SystemSessionCreate),
  DEF(SystemSessionDetach),
  DEF(SystemSessionInformation),
  DEF(SystemRangeStartInformation),
  DEF(SystemVerifierInformation),
  DEF(SystemVerifierThunkExtend),
  DEF(SystemSessionProcessInformation),
  DEF(SystemLoadGdiDriverInSystemSpace),
  DEF(SystemNumaProcessorMap),
  DEF(SystemPrefetcherInformation),
  DEF(SystemExtendedProcessInformation),
  DEF(SystemRecommendedSharedDataAlignment),
  DEF(SystemComPlusPackage),
  DEF(SystemNumaAvailableMemory),
  DEF(SystemProcessorPowerInformation),
  DEF(SystemEmulationBasicInformation),
  DEF(SystemEmulationProcessorInformation),
  DEF(SystemExtendedHandleInformation),
  DEF(SystemLostDelayedWriteInformation),
  DEF(SystemBigPoolInformation),
  DEF(SystemSessionPoolTagInformation),
  DEF(SystemSessionMappedViewInformation),
  DEF(SystemHotpatchInformation),
  DEF(SystemObjectSecurityMode),
  DEF(SystemWatchdogTimerHandler),
  DEF(SystemWatchdogTimerInformation),
  DEF(SystemLogicalProcessorInformation),
  DEF(SystemWow64SharedInformationObsolete),
  DEF(SystemRegisterFirmwareTableInformationHandler),
  DEF(SystemFirmwareTableInformation),
  DEF(SystemModuleInformationEx),
  DEF(SystemVerifierTriageInformation),
  DEF(SystemSuperfetchInformation),
  DEF(SystemMemoryListInformation),
  DEF(SystemFileCacheInformationEx),
  DEF(SystemThreadPriorityClientIdInformation),
  DEF(SystemProcessorIdleCycleTimeInformation),
  DEF(SystemVerifierCancellationInformation),
  DEF(SystemProcessorPowerInformationEx),
  DEF(SystemRefTraceInformation),
  DEF(SystemSpecialPoolInformation),
  DEF(SystemProcessIdInformation),
  DEF(SystemErrorPortInformation),
  DEF(SystemBootEnvironmentInformation),
  DEF(SystemHypervisorInformation),
  DEF(SystemVerifierInformationEx),
  DEF(SystemTimeZoneInformation),
  DEF(SystemImageFileExecutionOptionsInformation),
  DEF(SystemCoverageInformation),
  DEF(SystemPrefetchPathInformation),
  DEF(SystemVerifierFaultsInformation),
  DEF(SystemSystemPartitionInformation),
  DEF(SystemSystemDiskInformation),
  DEF(SystemProcessorPerformanceDistribution),
  DEF(SystemNumaProximityNodeInformation),
  DEF(SystemDynamicTimeZoneInformation),
  DEF(SystemCodeIntegrityInformation),
  DEF(SystemProcessorMicrocodeUpdateInformation),
  DEF(SystemProcessorBrandString),
  DEF(SystemVirtualAddressInformation),
  {0, 0}
};


typedef enum _THREADINFOCLASS {
  ThreadBasicInformation,
  ThreadTimes,
  ThreadPriority,
  ThreadBasePriority,
  ThreadAffinityMask,
  ThreadImpersonationToken,
  ThreadDescriptorTableEntry,
  ThreadEnableAlignmentFaultFixup,
  ThreadEventPair_Reusable,
  ThreadQuerySetWin32StartAddress,
  ThreadZeroTlsCell,
  ThreadPerformanceCount,
  ThreadAmILastThread,
  ThreadIdealProcessor,
  ThreadPriorityBoost,
  ThreadSetTlsArrayAddress,
  ThreadIsIoPending,
  ThreadHideFromDebugger,
  ThreadBreakOnTermination,
  ThreadSwitchLegacyState,
  ThreadIsTerminated,
  ThreadLastSystemCall,
  ThreadIoPriority,
  ThreadCycleTime,
  ThreadPagePriority,
  ThreadActualBasePriority,
  ThreadTebInformation,
  ThreadCSwitchMon,
  ThreadCSwitchPmu,
  ThreadWow64Context,
  ThreadGroupInformation,
  ThreadUmsInformation,
  ThreadCounterProfiling,
  ThreadIdealProcessorEx,
} THREADINFOCLASS;

EnumMap THREADINFOCLASS_MAP[] = 
{
  DEF(ThreadBasicInformation),
  DEF(ThreadTimes),
  DEF(ThreadPriority),
  DEF(ThreadBasePriority),
  DEF(ThreadAffinityMask),
  DEF(ThreadImpersonationToken),
  DEF(ThreadDescriptorTableEntry),
  DEF(ThreadEnableAlignmentFaultFixup),
  DEF(ThreadEventPair_Reusable),
  DEF(ThreadQuerySetWin32StartAddress),
  DEF(ThreadZeroTlsCell),
  DEF(ThreadPerformanceCount),
  DEF(ThreadAmILastThread),
  DEF(ThreadIdealProcessor),
  DEF(ThreadPriorityBoost),
  DEF(ThreadSetTlsArrayAddress),
  DEF(ThreadIsIoPending),
  DEF(ThreadHideFromDebugger),
  DEF(ThreadBreakOnTermination),
  DEF(ThreadSwitchLegacyState),
  DEF(ThreadIsTerminated),
  DEF(ThreadLastSystemCall),
  DEF(ThreadIoPriority),
  DEF(ThreadCycleTime),
  DEF(ThreadPagePriority),
  DEF(ThreadActualBasePriority),
  DEF(ThreadTebInformation),
  DEF(ThreadCSwitchMon),
  DEF(ThreadCSwitchPmu),
  DEF(ThreadWow64Context),
  DEF(ThreadGroupInformation),
  DEF(ThreadUmsInformation),
  DEF(ThreadCounterProfiling),
  DEF(ThreadIdealProcessorEx),
  {0, 0}
};


typedef enum _TIMER_INFORMATION_CLASS {
  TimerBasicInformation,
} TIMER_INFORMATION_CLASS;

EnumMap TIMER_INFORMATION_CLASS_MAP[] = 
{
  DEF(TimerBasicInformation),
  {0, 0}
};


typedef enum _TIMER_TYPE {
  NotificationTimer,
  SynchronizationTimer,
} TIMER_TYPE;

EnumMap TIMER_TYPE_MAP[] = 
{
  DEF(NotificationTimer),
  DEF(SynchronizationTimer),
  {0, 0}
};


typedef enum _TOKEN_INFORMATION_CLASS {
  TokenUser = 1,
  TokenGroups,
  TokenPrivileges,
  TokenOwner,
  TokenPrimaryGroup,
  TokenDefaultDacl,
  TokenSource,
  TokenType,
  TokenImpersonationLevel,
  TokenStatistics,
  TokenRestrictedSids,
  TokenSessionId,
  TokenGroupsAndPrivileges,
  TokenSessionReference,
  TokenSandBoxInert,
  TokenAuditPolicy,
  TokenOrigin,
  TokenElevationType,
  TokenLinkedToken,
  TokenElevation,
  TokenHasRestrictions,
  TokenAccessInformation,
  TokenVirtualizationAllowed,
  TokenVirtualizationEnabled,
  TokenIntegrityLevel,
  TokenUIAccess,
  TokenMandatoryPolicy,
  TokenLogonSid,
  TokenIsAppContainer,
  TokenCapabilities,
  TokenAppContainerSid,
  TokenAppContainerNumber,
  TokenUserClaimAttributes,
  TokenDeviceClaimAttributes,
  TokenRestrictedUserClaimAttributes,
  TokenRestrictedDeviceClaimAttributes,
  TokenDeviceGroups,
  TokenRestrictedDeviceGroups,
  TokenSecurityAttributes,
  TokenIsRestricted,
} TOKEN_INFORMATION_CLASS;

EnumMap TOKEN_INFORMATION_CLASS_MAP[] = 
{
  DEF(TokenUser),
  DEF(TokenGroups),
  DEF(TokenPrivileges),
  DEF(TokenOwner),
  DEF(TokenPrimaryGroup),
  DEF(TokenDefaultDacl),
  DEF(TokenSource),
  DEF(TokenType),
  DEF(TokenImpersonationLevel),
  DEF(TokenStatistics),
  DEF(TokenRestrictedSids),
  DEF(TokenSessionId),
  DEF(TokenGroupsAndPrivileges),
  DEF(TokenSessionReference),
  DEF(TokenSandBoxInert),
  DEF(TokenAuditPolicy),
  DEF(TokenOrigin),
  DEF(TokenElevationType),
  DEF(TokenLinkedToken),
  DEF(TokenElevation),
  DEF(TokenHasRestrictions),
  DEF(TokenAccessInformation),
  DEF(TokenVirtualizationAllowed),
  DEF(TokenVirtualizationEnabled),
  DEF(TokenIntegrityLevel),
  DEF(TokenUIAccess),
  DEF(TokenMandatoryPolicy),
  DEF(TokenLogonSid),
  DEF(TokenIsAppContainer),
  DEF(TokenCapabilities),
  DEF(TokenAppContainerSid),
  DEF(TokenAppContainerNumber),
  DEF(TokenUserClaimAttributes),
  DEF(TokenDeviceClaimAttributes),
  DEF(TokenRestrictedUserClaimAttributes),
  DEF(TokenRestrictedDeviceClaimAttributes),
  DEF(TokenDeviceGroups),
  DEF(TokenRestrictedDeviceGroups),
  DEF(TokenSecurityAttributes),
  DEF(TokenIsRestricted),
  {0, 0}
};


typedef enum tagTOKEN_TYPE {
  TokenPrimary = 1,
  TokenImpersonation,
} TOKEN_TYPE;

EnumMap TOKEN_TYPE_MAP[] =
{
  {0, 0}
};


typedef enum _TRANSACTION_INFORMATION_CLASS {
  TransactionBasicInformation,
  TransactionPropertiesInformation,
  TransactionEnlistmentInformation,
  TransactionSuperiorEnlistmentInformation,
} TRANSACTION_INFORMATION_CLASS;

EnumMap TRANSACTION_INFORMATION_CLASS_MAP[] = 
{
  DEF(TransactionBasicInformation),
  DEF(TransactionPropertiesInformation),
  DEF(TransactionEnlistmentInformation),
  DEF(TransactionSuperiorEnlistmentInformation),
  {0, 0}
};


typedef enum _TRANSACTIONMANAGER_INFORMATION_CLASS {
  TransactionManagerBasicInformation,
  TransactionManagerLogInformation,
  TransactionManagerLogPathInformation,
  TransactionManagerRecoveryInformation = 4,
} TRANSACTIONMANAGER_INFORMATION_CLASS;

EnumMap TRANSACTIONMANAGER_INFORMATION_CLASS_MAP[] = 
{
  DEF(TransactionManagerBasicInformation),
  DEF(TransactionManagerLogInformation),
  DEF(TransactionManagerLogPathInformation),
  DEF(TransactionManagerRecoveryInformation),
  {0, 0}
};


typedef enum _WAIT_TYPE {
  WaitAll,
  WaitAny,
} WAIT_TYPE;

EnumMap WAIT_TYPE_MAP[] =
{
  DEF(WaitAll),
  DEF(WaitAny),
  {0, 0}
};


typedef enum _WORKERFACTORYINFOCLASS {
  WorkerFactoryTimeout,
  WorkerFactoryRetryTimeout,
  WorkerFactoryIdleTimeout,
  WorkerFactoryBindingCount,
  WorkerFactoryThreadMinimum,
  WorkerFactoryThreadMaximum,
  WorkerFactoryPaused,
  WorkerFactoryBasicInformation,
  WorkerFactoryAdjustThreadGoal,
  WorkerFactoryCallbackType,
  WorkerFactoryStackInformation,
} WORKERFACTORYINFOCLASS;

EnumMap WORKERFACTORYINFOCLASS_MAP[] = 
{
  DEF(WorkerFactoryTimeout),
  DEF(WorkerFactoryRetryTimeout),
  DEF(WorkerFactoryIdleTimeout),
  DEF(WorkerFactoryBindingCount),
  DEF(WorkerFactoryThreadMinimum),
  DEF(WorkerFactoryThreadMaximum),
  DEF(WorkerFactoryPaused),
  DEF(WorkerFactoryBasicInformation),
  DEF(WorkerFactoryAdjustThreadGoal),
  DEF(WorkerFactoryCallbackType),
  DEF(WorkerFactoryStackInformation),
  {0, 0}
};


#undef DEF

////////////////////////////////////////////////////////////////////

#define DEF(X) {#X, X ## _MAP}

/** Provide a mapping from an enumeration name to a zero-terminated array of EnumMap entries */
struct AllEnum allEnums[] =
{
   DEF(ALPC_MESSAGE_INFORMATION_CLASS),
   DEF(ALPC_PORT_INFORMATION_CLASS),
   DEF(ATOM_INFORMATION_CLASS),
   DEF(AUDIT_EVENT_TYPE),
   DEF(DEBUGOBJECTINFOCLASS),
   DEF(DEBUG_CONTROL_CODE),
   DEF(DEVICE_POWER_STATE),
   DEF(ENLISTMENT_INFORMATION_CLASS),
   DEF(EVENT_INFORMATION_CLASS),
   DEF(EVENT_TYPE),
   DEF(FILE_INFORMATION_CLASS),
   DEF(FS_INFORMATION_CLASS),
   DEF(HARDERROR_RESPONSE_OPTION),
   DEF(IO_SESSION_STATE),
   DEF(JOB_INFORMATION_CLASS),
   DEF(KEY_INFORMATION_CLASS),
   DEF(KEY_SET_INFORMATION_CLASS),
   DEF(KEY_VALUE_INFORMATION_CLASS),
   DEF(KPROFILE_SOURCE),
   DEF(KTMOBJECT_TYPE),
   DEF(LPC_TYPE),
   DEF(MEMORY_INFORMATION_CLASS),
   DEF(MUTANT_INFORMATION_CLASS),
   DEF(NOTIFICATION_MASK),
   DEF(OBJECT_INFORMATION_CLASS),
   DEF(PORT_INFORMATION_CLASS),
   DEF(POWER_ACTION),
   DEF(POWER_INFORMATION_LEVEL),
   DEF(PROCESSINFOCLASS),
   DEF(RESOURCEMANAGER_INFORMATION_CLASS),
   DEF(SECTION_INFORMATION_CLASS),
   DEF(SECTION_INHERIT),
   DEF(SECURITY_INFORMATION),
   DEF(SEMAPHORE_INFORMATION_CLASS),
   DEF(SHUTDOWN_ACTION),
   DEF(SYSTEM_POWER_STATE),
   DEF(SYSTEM_INFORMATION_CLASS),
   DEF(THREADINFOCLASS),
   DEF(TIMER_INFORMATION_CLASS),
   DEF(TIMER_TYPE),
   DEF(TOKEN_INFORMATION_CLASS),
   DEF(TOKEN_TYPE),
   DEF(TRANSACTION_INFORMATION_CLASS),
   DEF(TRANSACTIONMANAGER_INFORMATION_CLASS),
   DEF(WAIT_TYPE),
   DEF(WORKERFACTORYINFOCLASS),
   {0, 0}
};

#undef DEF

} // namespace

