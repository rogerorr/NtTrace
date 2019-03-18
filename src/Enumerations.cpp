/*
NAME
    Enuerations.cpp

DESCRIPTION
    Class to define NT native enumerations for use by ShowData

NOTES
    Many of the values are obtainable from ole32.dll, for example using:
    sed -n "s/.*enum /udt /p" Enumerations.cpp | symexplorer ole32.dll

COPYRIGHT
    Copyright (C) 2011, 2019 by Roger Orr <rogero@howzatt.demon.co.uk>

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

static char const szRCSID[] = "$Id: Enumerations.cpp 1816 2019-03-18 22:43:29Z Roger $";

#include "Enumerations.h"

namespace Enumerations
{
#define DEF(X) { (unsigned long)X, #X }

typedef enum _ALPC_MESSAGE_INFORMATION_CLASS {
  AlpcMessageSidInformation = 0,
  AlpcMessageTokenModifiedIdInformation = 1,
  AlpcMessageDirectStatusInformation = 2,
  AlpcMessageHandleInformation = 3,
  MaxAlpcMessageInfoClass,
} ALPC_MESSAGE_INFORMATION_CLASS;

EnumMap ALPC_MESSAGE_INFORMATION_CLASS_MAP[] = 
{
  DEF(AlpcMessageSidInformation),
  DEF(AlpcMessageTokenModifiedIdInformation),
  DEF(AlpcMessageDirectStatusInformation),
  DEF(AlpcMessageHandleInformation),
  DEF(MaxAlpcMessageInfoClass),
  {0, 0}
};


typedef enum _ALPC_PORT_INFORMATION_CLASS {
  AlpcBasicInformation = 0,
  AlpcPortInformation = 1,
  AlpcAssociateCompletionPortInformation = 2,
  AlpcConnectedSIDInformation = 3,
  AlpcServerInformation = 4,
  AlpcMessageZoneInformation = 5,
  AlpcRegisterCompletionListInformation = 6,
  AlpcUnregisterCompletionListInformation = 7,
  AlpcAdjustCompletionListConcurrencyCountInformation = 8,
  AlpcRegisterCallbackInformation = 9,
  AlpcCompletionListRundownInformation = 10,
  AlpcWaitForPortReferences = 11,
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
  DEF(AlpcRegisterCallbackInformation),
  DEF(AlpcCompletionListRundownInformation),
  DEF(AlpcWaitForPortReferences),
  {0, 0}
};


typedef enum _ATOM_INFORMATION_CLASS {
  AtomBasicInformation = 0,
  AtomTableInformation = 1,
} ATOM_INFORMATION_CLASS;

EnumMap ATOM_INFORMATION_CLASS_MAP[] = 
{
  DEF(AtomBasicInformation),
  DEF(AtomTableInformation),
  {0, 0}
};


typedef enum _AUDIT_EVENT_TYPE {
  AuditEventObjectAccess = 0,
  AuditEventDirectoryServiceAccess = 1,
} AUDIT_EVENT_TYPE;

EnumMap AUDIT_EVENT_TYPE_MAP[] = 
{
  DEF(AuditEventObjectAccess),
  DEF(AuditEventDirectoryServiceAccess),
  {0, 0}
};


typedef enum _DEBUGOBJECTINFOCLASS { // (not found in pdb files)
  DebugObjectKillOnExit = 1,
} DEBUGOBJECTINFOCLASS;

EnumMap DEBUGOBJECTINFOCLASS_MAP[] =
{
  DEF(DebugObjectKillOnExit),
  {0, 0}
};


typedef enum _DEVICE_POWER_STATE { 
  PowerDeviceUnspecified = 0,
  PowerDeviceD0 = 1,
  PowerDeviceD1 = 2,
  PowerDeviceD2 = 3,
  PowerDeviceD3 = 4,
  PowerDeviceMaximum = 5,
} DEVICE_POWER_STATE;

EnumMap DEVICE_POWER_STATE_MAP[] = {
  DEF(PowerDeviceUnspecified),
  DEF(PowerDeviceD0),
  DEF(PowerDeviceD1),
  DEF(PowerDeviceD2),
  DEF(PowerDeviceD3),
  DEF(PowerDeviceMaximum),
  {0, 0}
};

typedef enum _DIRECTORY_NOTIFY_INFORMATION_CLASS {
  DirectoryNotifyInformation = 1,
  DirectoryNotifyExtendedInformation = 2,
} DIRECTORY_NOTIFY_INFORMATION_CLASS;

EnumMap DIRECTORY_NOTIFY_INFORMATION_CLASS_MAP[] = 
{
  DEF(DirectoryNotifyInformation),
  DEF(DirectoryNotifyExtendedInformation),
  {0, 0}
};


typedef enum _ENLISTMENT_INFORMATION_CLASS {
  EnlistmentBasicInformation = 0,
  EnlistmentRecoveryInformation = 1,
  EnlistmentCrmInformation = 2,
} ENLISTMENT_INFORMATION_CLASS;

EnumMap ENLISTMENT_INFORMATION_CLASS_MAP[] = 
{
  DEF(EnlistmentBasicInformation),
  DEF(EnlistmentRecoveryInformation),
  DEF(EnlistmentCrmInformation),
  {0, 0}
};


typedef enum _EVENT_INFORMATION_CLASS {
  EventBasicInformation = 0,
} EVENT_INFORMATION_CLASS;

EnumMap EVENT_INFORMATION_CLASS_MAP[] = 
{
  DEF(EventBasicInformation),
  {0, 0}
};


typedef enum _EVENT_TYPE {
  NotificationEvent = 0,
  SynchronizationEvent = 1,
} EVENT_TYPE;

EnumMap EVENT_TYPE_MAP[] =
{
  DEF(NotificationEvent),
  DEF(SynchronizationEvent),
  {0, 0}
};


typedef enum _FILE_INFORMATION_CLASS {
  FileDirectoryInformation = 1,
  FileFullDirectoryInformation = 2,
  FileBothDirectoryInformation = 3,
  FileBasicInformation = 4,
  FileStandardInformation = 5,
  FileInternalInformation = 6,
  FileEaInformation = 7,
  FileAccessInformation = 8,
  FileNameInformation = 9,
  FileRenameInformation = 10,
  FileLinkInformation = 11,
  FileNamesInformation = 12,
  FileDispositionInformation = 13,
  FilePositionInformation = 14,
  FileFullEaInformation = 15,
  FileModeInformation = 16,
  FileAlignmentInformation = 17,
  FileAllInformation = 18,
  FileAllocationInformation = 19,
  FileEndOfFileInformation = 20,
  FileAlternateNameInformation = 21,
  FileStreamInformation = 22,
  FilePipeInformation = 23,
  FilePipeLocalInformation = 24,
  FilePipeRemoteInformation = 25,
  FileMailslotQueryInformation = 26,
  FileMailslotSetInformation = 27,
  FileCompressionInformation = 28,
  FileObjectIdInformation = 29,
  FileCompletionInformation = 30,
  FileMoveClusterInformation = 31,
  FileQuotaInformation = 32,
  FileReparsePointInformation = 33,
  FileNetworkOpenInformation = 34,
  FileAttributeTagInformation = 35,
  FileTrackingInformation = 36,
  FileIdBothDirectoryInformation = 37,
  FileIdFullDirectoryInformation = 38,
  FileValidDataLengthInformation = 39,
  FileShortNameInformation = 40,
  FileIoCompletionNotificationInformation = 41,
  FileIoStatusBlockRangeInformation = 42,
  FileIoPriorityHintInformation = 43,
  FileSfioReserveInformation = 44,
  FileSfioVolumeInformation = 45,
  FileHardLinkInformation = 46,
  FileProcessIdsUsingFileInformation = 47,
  FileNormalizedNameInformation = 48,
  FileNetworkPhysicalNameInformation = 49,
  FileIdGlobalTxDirectoryInformation = 50,
  FileIsRemoteDeviceInformation = 51,
  FileAttributeCacheInformation = 52, // FileUnusedInformation in ole32.pdb
  FileNumaNodeInformation = 53,
  FileStandardLinkInformation = 54,
  FileRemoteProtocolInformation = 55,
  FileRenameInformationBypassAccessCheck = 56,
  FileLinkInformationBypassAccessCheck = 57,
  FileVolumeNameInformation = 58,
  FileIdInformation = 59,
  FileIdExtdDirectoryInformation = 60,
  FileReplaceCompletionInformation = 61,
  FileHardLinkFullIdInformation = 62,
  FileIdExtdBothDirectoryInformation = 63,
  FileDispositionInformationEx = 64,
  FileRenameInformationEx = 65,
  FileRenameInformationExBypassAccessCheck = 66,
  FileDesiredStorageClassInformation = 67,
  FileStatInformation = 68,
  FileMemoryPartitionInformation = 69,
  FileStatLxInformation = 70,
  FileCaseSensitiveInformation = 71,
  FileLinkInformationEx = 72,
  FileLinkInformationExBypassAccessCheck = 73,
  FileStorageReserveIdInformation = 74,
  FileCaseSensitiveInformationForceAccessCheck = 75,
  FileMaximumInformation = 76,
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
  DEF(FileRenameInformationBypassAccessCheck),
  DEF(FileLinkInformationBypassAccessCheck),
  DEF(FileVolumeNameInformation),
  DEF(FileIdInformation),
  DEF(FileIdExtdDirectoryInformation),
  DEF(FileReplaceCompletionInformation),
  DEF(FileHardLinkFullIdInformation),
  DEF(FileIdExtdBothDirectoryInformation),
  DEF(FileDispositionInformationEx),
  DEF(FileRenameInformationEx),
  DEF(FileRenameInformationExBypassAccessCheck),
  DEF(FileDesiredStorageClassInformation),
  DEF(FileStatInformation),
  DEF(FileMemoryPartitionInformation),
  DEF(FileStatLxInformation),
  DEF(FileCaseSensitiveInformation),
  DEF(FileLinkInformationEx),
  DEF(FileLinkInformationExBypassAccessCheck),
  DEF(FileStorageReserveIdInformation),
  DEF(FileCaseSensitiveInformationForceAccessCheck),
  DEF(FileMaximumInformation),
  {0, 0}
};


typedef enum _FSINFOCLASS {
  FsVolumeInformation = 1,
  FsLabelInformation = 2,
  FsSizeInformation = 3,
  FsDeviceInformation = 4,
  FsAttributeInformation = 5,
  FsControlInformation = 6,
  FsFullSizeInformation = 7,
  FsObjectIdInformation = 8,
  FsDriverPathInformation = 9,
  FsVolumeFlagsInformation = 10,
  FsSectorSizeInformation = 11,
  FsDataCopyInformation = 12,
  FsMetadataSizeInformation = 13,
  FsFullSizeInformationEx = 14,
  FileFsMaximumInformation = 15,
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
  DEF(FsDataCopyInformation),
  DEF(FsMetadataSizeInformation),
  DEF(FsFullSizeInformationEx),
  DEF(FileFsMaximumInformation),
  {0, 0}
};


typedef enum _HARDERROR_RESPONSE_OPTION {
  OptionAbortRetryIgnore = 0,
  OptionOk = 1,
  OptionOkCancel = 2,
  OptionRetryCancel = 3,
  OptionYesNo = 4,
  OptionYesNoCancel = 5,
  OptionShutdownSystem = 6,
  OptionOkNoWait = 7,
  OptionCancelTryContinue = 8,
} HARDERROR_RESPONSE_OPTION;

EnumMap HARDERROR_RESPONSE_OPTION_MAP[] = {
  DEF(OptionAbortRetryIgnore),
  DEF(OptionOk),
  DEF(OptionOkCancel),
  DEF(OptionRetryCancel),
  DEF(OptionYesNo),
  DEF(OptionYesNoCancel),
  DEF(OptionShutdownSystem),
  DEF(OptionOkNoWait),
  DEF(OptionCancelTryContinue),
  {0, 0}
};


typedef enum _IO_SESSION_STATE {
  IoSessionStateCreated = 1,
  IoSessionStateInitialized = 2,
  IoSessionStateConnected = 3,
  IoSessionStateDisconnected = 4,
  IoSessionStateDisconnectedLoggedOn = 5,
  IoSessionStateLoggedOn = 6,
  IoSessionStateLoggedOff = 7,
  IoSessionStateTerminated = 8,
  IoSessionStateMax = 9,
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

typedef enum _JOBOBJECTINFOCLASS {
  JobObjectBasicAccountingInformation = 1,
  JobObjectBasicLimitInformation = 2,
  JobObjectBasicProcessIdList = 3,
  JobObjectBasicUIRestrictions = 4,
  JobObjectSecurityLimitInformation = 5,
  JobObjectEndOfJobTimeInformation = 6,
  JobObjectAssociateCompletionPortInformation = 7,
  JobObjectBasicAndIoAccountingInformation = 8,
  JobObjectExtendedLimitInformation = 9,
  JobObjectJobSetInformation = 10,
  JobObjectGroupInformation = 11,
  JobObjectNotificationLimitInformation = 12,
  JobObjectLimitViolationInformation = 13,
  JobObjectGroupInformationEx = 14,
  JobObjectCpuRateControlInformation = 15,
  JobObjectCompletionFilter = 16,
  JobObjectCompletionCounter = 17,
  JobObjectFreezeInformation = 18,
  JobObjectExtendedAccountingInformation = 19,
  JobObjectWakeInformation = 20,
  JobObjectBackgroundInformation = 21,
  JobObjectSchedulingRankBiasInformation = 22,
  JobObjectTimerVirtualizationInformation = 23,
  JobObjectCycleTimeNotification = 24,
  JobObjectClearEvent = 25,
  JobObjectInterferenceInformation = 26,
  JobObjectClearPeakJobMemoryUsed = 27,
  JobObjectMemoryUsageInformation = 28,
  JobObjectSharedCommit = 29,
  JobObjectContainerId = 30,
  JobObjectIoRateControlInformation = 31,
  JobObjectNetRateControlInformation = 32,
  JobObjectNotificationLimitInformation2 = 33,
  JobObjectLimitViolationInformation2 = 34,
  JobObjectCreateSilo = 35,
  JobObjectSiloBasicInformation = 36,
  JobObjectSiloRootDirectory = 37,
  JobObjectServerSiloBasicInformation = 38,
  JobObjectServerSiloUserSharedData = 39,
  JobObjectServerSiloInitialize = 40,
  JobObjectServerSiloRunningState = 41,
  JobObjectIoAttribution = 42,
  JobObjectMemoryPartitionInformation = 43,
  JobObjectContainerTelemetryId = 44,
  JobObjectSiloSystemRoot = 45,
  JobObjectEnergyTrackingState = 46,
  JobObjectThreadImpersonationInformation = 47,
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
  DEF(JobObjectCompletionFilter),
  DEF(JobObjectCompletionCounter),
  DEF(JobObjectFreezeInformation),
  DEF(JobObjectExtendedAccountingInformation),
  DEF(JobObjectWakeInformation),
  DEF(JobObjectBackgroundInformation),
  DEF(JobObjectSchedulingRankBiasInformation),
  DEF(JobObjectTimerVirtualizationInformation),
  DEF(JobObjectCycleTimeNotification),
  DEF(JobObjectClearEvent),
  DEF(JobObjectInterferenceInformation),
  DEF(JobObjectClearPeakJobMemoryUsed),
  DEF(JobObjectMemoryUsageInformation),
  DEF(JobObjectSharedCommit),
  DEF(JobObjectContainerId),
  DEF(JobObjectIoRateControlInformation),
  DEF(JobObjectNetRateControlInformation),
  DEF(JobObjectNotificationLimitInformation2),
  DEF(JobObjectLimitViolationInformation2),
  DEF(JobObjectCreateSilo),
  DEF(JobObjectSiloBasicInformation),
  DEF(JobObjectSiloRootDirectory),
  DEF(JobObjectServerSiloBasicInformation),
  DEF(JobObjectServerSiloUserSharedData),
  DEF(JobObjectServerSiloInitialize),
  DEF(JobObjectServerSiloRunningState),
  DEF(JobObjectIoAttribution),
  DEF(JobObjectMemoryPartitionInformation),
  DEF(JobObjectContainerTelemetryId),
  DEF(JobObjectSiloSystemRoot),
  DEF(JobObjectEnergyTrackingState),
  DEF(JobObjectThreadImpersonationInformation),
  {0, 0}
};


typedef enum _KEY_INFORMATION_CLASS {
  KeyBasicInformation = 0,
  KeyNodeInformation = 1,
  KeyFullInformation = 2,
  KeyNameInformation = 3,
  KeyCachedInformation = 4,
  KeyFlagsInformation = 5,
  KeyVirtualizationInformation = 6,
  KeyHandleTagsInformation = 7,
  KeyTrustInformation = 8,
  KeyLayerInformation = 9,
  MaxKeyInfoClass = 10,
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
  DEF(KeyTrustInformation),
  DEF(KeyLayerInformation),
  DEF(MaxKeyInfoClass),
  {0, 0}
};


typedef enum _KEY_SET_INFORMATION_CLASS {
  KeyWriteTimeInformation = 0,
  KeyWow64FlagsInformation = 1,
  KeyControlFlagsInformation = 2,
  KeySetVirtualizationInformation = 3,
  KeySetDebugInformation = 4,
  KeySetHandleTagsInformation = 5,
  KeySetLayerInformation = 6,
  MaxKeySetInfoClass = 7,
} KEY_SET_INFORMATION_CLASS;

EnumMap KEY_SET_INFORMATION_CLASS_MAP[] = 
{
  DEF(KeyWriteTimeInformation),
  DEF(KeyWow64FlagsInformation),
  DEF(KeyControlFlagsInformation),
  DEF(KeySetVirtualizationInformation),
  DEF(KeySetDebugInformation),
  DEF(KeySetHandleTagsInformation),
  DEF(KeySetLayerInformation),
  DEF(MaxKeySetInfoClass),
  {0, 0}
};


typedef enum _KEY_VALUE_INFORMATION_CLASS {
  KeyValueBasicInformation = 0,
  KeyValueFullInformation = 1,
  KeyValuePartialInformation = 2,
  KeyValueFullInformationAlign64 = 3,
  KeyValuePartialInformationAlign64 = 4,
  KeyValueLayerInformation = 5,
  MaxKeyValueInfoClass = 6,
} KEY_VALUE_INFORMATION_CLASS;

EnumMap KEY_VALUE_INFORMATION_CLASS_MAP[] = 
{
  DEF(KeyValueBasicInformation),
  DEF(KeyValueFullInformation),
  DEF(KeyValuePartialInformation),
  DEF(KeyValueFullInformationAlign64),
  DEF(KeyValuePartialInformationAlign64),
  DEF(KeyValueLayerInformation),
  DEF(MaxKeyValueInfoClass),
  {0, 0}
};


typedef enum _KPROFILE_SOURCE {
  ProfileTime = 0,
  ProfileAlignmentFixup = 1,
  ProfileTotalIssues = 2,
  ProfilePipelineDry = 3,
  ProfileLoadInstructions = 4,
  ProfilePipelineFrozen = 5,
  ProfileBranchInstructions = 6,
  ProfileTotalNonissues = 7,
  ProfileDcacheMisses = 8,
  ProfileIcacheMisses = 9,
  ProfileCacheMisses = 10,
  ProfileBranchMispredictions = 11,
  ProfileStoreInstructions = 12,
  ProfileFpInstructions = 13,
  ProfileIntegerInstructions = 14,
  Profile2Issue = 15,
  Profile3Issue = 16,
  Profile4Issue = 17,
  ProfileSpecialInstructions = 18,
  ProfileTotalCycles = 19,
  ProfileIcacheIssues = 20,
  ProfileDcacheAccesses = 21,
  ProfileMemoryBarrierCycles = 22,
  ProfileLoadLinkedIssues = 23,
  ProfileMaximum = 24,
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
  DEF(ProfileMaximum),
  {0, 0}
};


typedef enum _KTMOBJECT_TYPE {
  KTMOBJECT_TRANSACTION = 0,
  KTMOBJECT_TRANSACTION_MANAGER = 1,
  KTMOBJECT_RESOURCE_MANAGER = 2,
  KTMOBJECT_ENLISTMENT = 3,
  KTMOBJECT_INVALID = 4,
} KTMOBJECT_TYPE;

EnumMap KTMOBJECT_TYPE_MAP[] = 
{
  DEF(KTMOBJECT_TRANSACTION),
  DEF(KTMOBJECT_TRANSACTION_MANAGER),
  DEF(KTMOBJECT_RESOURCE_MANAGER),
  DEF(KTMOBJECT_ENLISTMENT),
  DEF(KTMOBJECT_INVALID),
  {0, 0}
};


/** Type of LPC messages */
typedef enum _LPC_TYPE { // (not found in pdb files)
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
  MemoryBasicInformation = 0,
  MemoryWorkingSetInformation = 1,
  MemoryMappedFilenameInformation = 2,
  MemoryRegionInformation = 3,
  MemoryWorkingSetExInformation = 4,
  MemorySharedCommitInformation = 5,
  MemoryImageInformation = 6,
  MemoryRegionInformationEx = 7,
  MemoryPrivilegedBasicInformation = 8,
  MemoryEnclaveImageInformation = 9,
  MemoryBasicInformationCapped = 10,
} MEMORY_INFORMATION_CLASS;

EnumMap MEMORY_INFORMATION_CLASS_MAP[] = 
{
  DEF(MemoryBasicInformation),
  DEF(MemoryWorkingSetInformation),
  DEF(MemoryMappedFilenameInformation),
  DEF(MemoryRegionInformation),
  DEF(MemoryWorkingSetExInformation),
  DEF(MemorySharedCommitInformation),
  DEF(MemoryImageInformation),
  DEF(MemoryRegionInformationEx),
  DEF(MemoryPrivilegedBasicInformation),
  DEF(MemoryEnclaveImageInformation),
  DEF(MemoryBasicInformationCapped),
  {0, 0}
};

typedef enum _PARTITION_INFORMATION_CLASS {
  SystemMemoryPartitionInformation = 0,
  SystemMemoryPartitionMoveMemory = 1,
  SystemMemoryPartitionAddPagefile = 2,
  SystemMemoryPartitionCombineMemory = 3,
  SystemMemoryPartitionInitialAddMemory = 4,
  SystemMemoryPartitionGetMemoryEvents = 5,
  SystemMemoryPartitionMax = 6,
} MEMORY_PARTITION_INFORMATION_CLASS;

EnumMap MEMORY_PARTITION_INFORMATION_CLASS_MAP[] = 
{
  DEF(SystemMemoryPartitionInformation),
  DEF(SystemMemoryPartitionMoveMemory),
  DEF(SystemMemoryPartitionAddPagefile),
  DEF(SystemMemoryPartitionCombineMemory),
  DEF(SystemMemoryPartitionInitialAddMemory),
  DEF(SystemMemoryPartitionGetMemoryEvents),
  DEF(SystemMemoryPartitionMax),
  {0, 0}
};


typedef enum _MUTANT_INFORMATION_CLASS {
  MutantBasicInformation = 0,
  MutantOwnerInformation = 1,
} MUTANT_INFORMATION_CLASS;

EnumMap MUTANT_INFORMATION_CLASS_MAP[] = 
{
  DEF(MutantBasicInformation),
  DEF(MutantOwnerInformation),
  {0, 0}
};

typedef enum _NOTIFICATION_MASK { // (not found in pdb files)
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
  ObjectBasicInformation = 0,
  ObjectNameInformation = 1,
  ObjectTypeInformation = 2,
  ObjectTypesInformation = 3,
  ObjectHandleFlagInformation = 4,
  ObjectSessionInformation = 5,
  ObjectSessionObjectInformation = 6,
  MaxObjectInfoClass = 7,
} OBJECT_INFORMATION_CLASS;

EnumMap OBJECT_INFORMATION_CLASS_MAP[] = 
{
  DEF(ObjectBasicInformation),
  DEF(ObjectNameInformation),
  DEF(ObjectTypeInformation),
  DEF(ObjectHandleFlagInformation),
  DEF(ObjectSessionInformation),
  DEF(ObjectSessionObjectInformation),
  {0, 0}
};


typedef enum _PORT_INFORMATION_CLASS {
  PortBasicInformation = 0,
  PortDumpInformation = 1,
} PORT_INFORMATION_CLASS;

EnumMap PORT_INFORMATION_CLASS_MAP[] =
{
  DEF(PortBasicInformation),
  DEF(PortDumpInformation),
  {0, 0}
};


enum POWER_ACTION {
  PowerActionNone = 0,
  PowerActionReserved = 1,
  PowerActionSleep = 2,
  PowerActionHibernate = 3,
  PowerActionShutdown = 4,
  PowerActionShutdownReset = 5,
  PowerActionShutdownOff = 6,
  PowerActionWarmEject = 7,
  PowerActionDisplayOff = 8,
};

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
  DEF(PowerActionDisplayOff),
  {0, 0}
};


enum POWER_INFORMATION_LEVEL {
  SystemPowerPolicyAc = 0,
  SystemPowerPolicyDc = 1,
  VerifySystemPolicyAc = 2,
  VerifySystemPolicyDc = 3,
  SystemPowerCapabilities = 4,
  SystemBatteryState = 5,
  SystemPowerStateHandler = 6,
  ProcessorStateHandler = 7,
  SystemPowerPolicyCurrent = 8,
  AdministratorPowerPolicy = 9,
  SystemReserveHiberFile = 10,
  ProcessorInformation = 11,
  SystemPowerInformation = 12,
  ProcessorStateHandler2 = 13,
  LastWakeTime = 14,
  LastSleepTime = 15,
  SystemExecutionState = 16,
  SystemPowerStateNotifyHandler = 17,
  ProcessorPowerPolicyAc = 18,
  ProcessorPowerPolicyDc = 19,
  VerifyProcessorPowerPolicyAc = 20,
  VerifyProcessorPowerPolicyDc = 21,
  ProcessorPowerPolicyCurrent = 22,
  SystemPowerStateLogging = 23,
  SystemPowerLoggingEntry = 24,
  SetPowerSettingValue = 25,
  NotifyUserPowerSetting = 26,
  PowerInformationLevelUnused0 = 27,
  SystemMonitorHiberBootPowerOff = 28,
  SystemVideoState = 29,
  TraceApplicationPowerMessage = 30,
  TraceApplicationPowerMessageEnd = 31,
  ProcessorPerfStates = 32,
  ProcessorIdleStates = 33,
  ProcessorCap = 34,
  SystemWakeSource = 35,
  SystemHiberFileInformation = 36,
  TraceServicePowerMessage = 37,
  ProcessorLoad = 38,
  PowerShutdownNotification = 39,
  MonitorCapabilities = 40,
  SessionPowerInit = 41,
  SessionDisplayState = 42,
  PowerRequestCreate = 43,
  PowerRequestAction = 44,
  GetPowerRequestList = 45,
  ProcessorInformationEx = 46,
  NotifyUserModeLegacyPowerEvent = 47,
  GroupPark = 48,
  ProcessorIdleDomains = 49,
  WakeTimerList = 50,
  SystemHiberFileSize = 51,
  ProcessorIdleStatesHv = 52,
  ProcessorPerfStatesHv = 53,
  ProcessorPerfCapHv = 54,
  ProcessorSetIdle = 55,
  LogicalProcessorIdling = 56,
  UserPresence = 57,
  PowerSettingNotificationName = 58,
  GetPowerSettingValue = 59,
  IdleResiliency = 60,
  SessionRITState = 61,
  SessionConnectNotification = 62,
  SessionPowerCleanup = 63,
  SessionLockState = 64,
  SystemHiberbootState = 65,
  PlatformInformation = 66,
  PdcInvocation = 67,
  MonitorInvocation = 68,
  FirmwareTableInformationRegistered = 69,
  SetShutdownSelectedTime = 70,
  SuspendResumeInvocation = 71,
  PlmPowerRequestCreate = 72,
  ScreenOff = 73,
  CsDeviceNotification = 74,
  PlatformRole = 75,
  LastResumePerformance = 76,
  DisplayBurst = 77,
  ExitLatencySamplingPercentage = 78,
  RegisterSpmPowerSettings = 79,
  PlatformIdleStates = 80,
  ProcessorIdleVeto = 81,
  PlatformIdleVeto = 82,
  SystemBatteryStatePrecise = 83,
  ThermalEvent = 84,
  PowerRequestActionInternal = 85,
  BatteryDeviceState = 86,
  PowerInformationInternal = 87,
  ThermalStandby = 88,
  SystemHiberFileType = 89,
  PhysicalPowerButtonPress = 90,
  QueryPotentialDripsConstraint = 91,
  EnergyTrackerCreate = 92,
  EnergyTrackerQuery = 93,
  UpdateBlackBoxRecorder = 94,
  PowerInformationLevelMaximum = 95,
};

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
  DEF(SystemPowerStateLogging),
  DEF(SystemPowerLoggingEntry),
  DEF(SetPowerSettingValue),
  DEF(NotifyUserPowerSetting),
  DEF(PowerInformationLevelUnused0),
  DEF(SystemMonitorHiberBootPowerOff),
  DEF(SystemVideoState),
  DEF(TraceApplicationPowerMessage),
  DEF(TraceApplicationPowerMessageEnd),
  DEF(ProcessorPerfStates),
  DEF(ProcessorIdleStates),
  DEF(ProcessorCap),
  DEF(SystemWakeSource),
  DEF(SystemHiberFileInformation),
  DEF(TraceServicePowerMessage),
  DEF(ProcessorLoad),
  DEF(PowerShutdownNotification),
  DEF(MonitorCapabilities),
  DEF(SessionPowerInit),
  DEF(SessionDisplayState),
  DEF(PowerRequestCreate),
  DEF(PowerRequestAction),
  DEF(GetPowerRequestList),
  DEF(ProcessorInformationEx),
  DEF(NotifyUserModeLegacyPowerEvent),
  DEF(GroupPark),
  DEF(ProcessorIdleDomains),
  DEF(WakeTimerList),
  DEF(SystemHiberFileSize),
  DEF(ProcessorIdleStatesHv),
  DEF(ProcessorPerfStatesHv),
  DEF(ProcessorPerfCapHv),
  DEF(ProcessorSetIdle),
  DEF(LogicalProcessorIdling),
  DEF(UserPresence),
  DEF(PowerSettingNotificationName),
  DEF(GetPowerSettingValue),
  DEF(IdleResiliency),
  DEF(SessionRITState),
  DEF(SessionConnectNotification),
  DEF(SessionPowerCleanup),
  DEF(SessionLockState),
  DEF(SystemHiberbootState),
  DEF(PlatformInformation),
  DEF(PdcInvocation),
  DEF(MonitorInvocation),
  DEF(FirmwareTableInformationRegistered),
  DEF(SetShutdownSelectedTime),
  DEF(SuspendResumeInvocation),
  DEF(PlmPowerRequestCreate),
  DEF(ScreenOff),
  DEF(CsDeviceNotification),
  DEF(PlatformRole),
  DEF(LastResumePerformance),
  DEF(DisplayBurst),
  DEF(ExitLatencySamplingPercentage),
  DEF(RegisterSpmPowerSettings),
  DEF(PlatformIdleStates),
  DEF(ProcessorIdleVeto),
  DEF(PlatformIdleVeto),
  DEF(SystemBatteryStatePrecise),
  DEF(ThermalEvent),
  DEF(PowerRequestActionInternal),
  DEF(BatteryDeviceState),
  DEF(PowerInformationInternal),
  DEF(ThermalStandby),
  DEF(SystemHiberFileType),
  DEF(PhysicalPowerButtonPress),
  DEF(QueryPotentialDripsConstraint),
  DEF(EnergyTrackerCreate),
  DEF(EnergyTrackerQuery),
  DEF(UpdateBlackBoxRecorder),
  {0, 0}
};


typedef enum _PROCESSINFOCLASS {
  ProcessBasicInformation = 0,
  ProcessQuotaLimits = 1,
  ProcessIoCounters = 2,
  ProcessVmCounters = 3,
  ProcessTimes = 4,
  ProcessBasePriority = 5,
  ProcessRaisePriority = 6,
  ProcessDebugPort = 7,
  ProcessExceptionPort = 8,
  ProcessAccessToken = 9,
  ProcessLdtInformation = 10,
  ProcessLdtSize = 11,
  ProcessDefaultHardErrorMode = 12,
  ProcessIoPortHandlers = 13,
  ProcessPooledUsageAndLimits = 14,
  ProcessWorkingSetWatch = 15,
  ProcessUserModeIOPL = 16,
  ProcessEnableAlignmentFaultFixup = 17,
  ProcessPriorityClass = 18,
  ProcessWx86Information = 19,
  ProcessHandleCount = 20,
  ProcessAffinityMask = 21,
  ProcessPriorityBoost = 22,
  ProcessDeviceMap = 23,
  ProcessSessionInformation = 24,
  ProcessForegroundInformation = 25,
  ProcessWow64Information = 26,
  ProcessImageFileName = 27,
  ProcessLUIDDeviceMapsEnabled = 28,
  ProcessBreakOnTermination = 29,
  ProcessDebugObjectHandle = 30,
  ProcessDebugFlags = 31,
  ProcessHandleTracing = 32,
  ProcessIoPriority = 33,
  ProcessExecuteFlags = 34,
  ProcessTlsInformation = 35,
  ProcessCookie = 36,
  ProcessImageInformation = 37,
  ProcessCycleTime = 38,
  ProcessPagePriority = 39,
  ProcessInstrumentationCallback = 40,
  ProcessThreadStackAllocation = 41,
  ProcessWorkingSetWatchEx = 42,
  ProcessImageFileNameWin32 = 43,
  ProcessImageFileMapping = 44,
  ProcessAffinityUpdateMode = 45,
  ProcessMemoryAllocationMode = 46,
  ProcessGroupInformation = 47,
  ProcessTokenVirtualizationEnabled = 48,
  ProcessOwnerInformation = 49,
  ProcessWindowInformation = 50,
  ProcessHandleInformation = 51,
  ProcessMitigationPolicy = 52,
  ProcessDynamicFunctionTableInformation = 53,
  ProcessHandleCheckingMode = 54,
  ProcessKeepAliveCount = 55,
  ProcessRevokeFileHandles = 56,
  ProcessWorkingSetControl = 57,
  ProcessHandleTable = 58,
  ProcessCheckStackExtentsMode = 59,
  ProcessCommandLineInformation = 60,
  ProcessProtectionInformation = 61,
  ProcessMemoryExhaustion = 62,
  ProcessFaultInformation = 63,
  ProcessTelemetryIdInformation = 64,
  ProcessCommitReleaseInformation = 65,
  ProcessDefaultCpuSetsInformation = 66,
  ProcessAllowedCpuSetsInformation = 67,
  ProcessReserved1Information = 66,
  ProcessReserved2Information = 67,
  ProcessSubsystemProcess = 68,
  ProcessJobMemoryInformation = 69,
  ProcessInPrivate = 70,
  ProcessRaiseUMExceptionOnInvalidHandleClose = 71,
  ProcessIumChallengeResponse = 72,
  ProcessChildProcessInformation = 73,
  ProcessHighGraphicsPriorityInformation = 74,
  ProcessSubsystemInformation = 75,
  ProcessEnergyValues = 76,
  ProcessPowerThrottlingState = 77,
  ProcessReserved3Information = 78,
  ProcessWin32kSyscallFilterInformation = 79,
  ProcessDisableSystemAllowedCpuSets = 80,
  ProcessWakeInformation = 81,
  ProcessEnergyTrackingState = 82,
  ProcessManageWritesToExecutableMemory = 83,
  ProcessCaptureTrustletLiveDump = 84,
  ProcessTelemetryCoverage = 85,
  ProcessEnclaveInformation = 86,
  ProcessEnableReadWriteVmLogging = 87,
  ProcessUptimeInformation = 88,
  ProcessImageSection = 89,
  ProcessDebugAuthInformation = 90,
  ProcessSystemResourceManagement = 91,
  ProcessSequenceNumber = 92,
  ProcessLoaderDetour = 93,
  ProcessSecurityDomainInformation = 94,
  ProcessCombineSecurityDomainsInformation = 95,
  ProcessEnableLogging = 96,
  ProcessLeapSecondInformation = 97,
  MaxProcessInfoClass = 98,
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
  DEF(ProcessOwnerInformation),
  DEF(ProcessWindowInformation),
  DEF(ProcessHandleInformation),
  DEF(ProcessMitigationPolicy),
  DEF(ProcessDynamicFunctionTableInformation),
  DEF(ProcessHandleCheckingMode),
  DEF(ProcessKeepAliveCount),
  DEF(ProcessRevokeFileHandles),
  DEF(ProcessWorkingSetControl),
  DEF(ProcessHandleTable),
  DEF(ProcessCheckStackExtentsMode),
  DEF(ProcessCommandLineInformation),
  DEF(ProcessProtectionInformation),
  DEF(ProcessMemoryExhaustion),
  DEF(ProcessFaultInformation),
  DEF(ProcessTelemetryIdInformation),
  DEF(ProcessCommitReleaseInformation),
  DEF(ProcessDefaultCpuSetsInformation),
  DEF(ProcessAllowedCpuSetsInformation),
  DEF(ProcessReserved1Information),
  DEF(ProcessReserved2Information),
  DEF(ProcessSubsystemProcess),
  DEF(ProcessJobMemoryInformation),
  DEF(ProcessInPrivate),
  DEF(ProcessRaiseUMExceptionOnInvalidHandleClose),
  DEF(ProcessIumChallengeResponse),
  DEF(ProcessChildProcessInformation),
  DEF(ProcessHighGraphicsPriorityInformation),
  DEF(ProcessSubsystemInformation),
  DEF(ProcessEnergyValues),
  DEF(ProcessPowerThrottlingState),
  DEF(ProcessReserved3Information),
  DEF(ProcessWin32kSyscallFilterInformation),
  DEF(ProcessDisableSystemAllowedCpuSets),
  DEF(ProcessWakeInformation),
  DEF(ProcessEnergyTrackingState),
  DEF(ProcessManageWritesToExecutableMemory),
  DEF(ProcessCaptureTrustletLiveDump),
  DEF(ProcessTelemetryCoverage),
  DEF(ProcessEnclaveInformation),
  DEF(ProcessEnableReadWriteVmLogging),
  DEF(ProcessUptimeInformation),
  DEF(ProcessImageSection),
  DEF(ProcessDebugAuthInformation),
  DEF(ProcessSystemResourceManagement),
  DEF(ProcessSequenceNumber),
  DEF(ProcessLoaderDetour),
  DEF(ProcessSecurityDomainInformation),
  DEF(ProcessCombineSecurityDomainsInformation),
  DEF(ProcessEnableLogging),
  DEF(ProcessLeapSecondInformation),
  {0, 0}
};


typedef enum _RESOURCEMANAGER_INFORMATION_CLASS {
  ResourceManagerBasicInformation = 0,
  ResourceManagerCompletionInformation = 1,
} RESOURCEMANAGER_INFORMATION_CLASS;

EnumMap RESOURCEMANAGER_INFORMATION_CLASS_MAP[] = 
{
  DEF(ResourceManagerBasicInformation),
  DEF(ResourceManagerCompletionInformation),
  {0, 0}
};


typedef enum _SECTION_INFORMATION_CLASS {
  SectionBasicInformation = 0,
  SectionImageInformation = 1,
  SectionRelocationInformation = 2,
  SectionOriginalBaseInformation = 3,
  SectionInternalImageInformation = 4,
} SECTION_INFORMATION_CLASS;

EnumMap SECTION_INFORMATION_CLASS_MAP[] = 
{
  DEF(SectionBasicInformation),
  DEF(SectionImageInformation),
  DEF(SectionRelocationInformation),
  DEF(SectionOriginalBaseInformation),
  DEF(SectionInternalImageInformation),
  {0, 0}
};


typedef enum _SECURITY_INFORMATION { // (not found in pdb files)
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
  ViewUnmap = 2,
} SECTION_INHERIT;

EnumMap SECTION_INHERIT_MAP[] = 
{
  DEF(ViewShare),
  DEF(ViewUnmap),
  {0, 0}
};


typedef enum _SEMAPHORE_INFORMATION_CLASS {
  SemaphoreBasicInformation = 0,
} SEMAPHORE_INFORMATION_CLASS;

EnumMap SEMAPHORE_INFORMATION_CLASS_MAP[] = 
{
  DEF(SemaphoreBasicInformation),
  {0, 0}
};


typedef enum _SHUTDOWN_ACTION {
  ShutdownNoReboot = 0,
  ShutdownReboot = 1,
  ShutdownPowerOff = 2,
} SHUTDOWN_ACTION;

EnumMap SHUTDOWN_ACTION_MAP[] = 
{
  DEF(ShutdownNoReboot),
  DEF(ShutdownReboot),
  DEF(ShutdownPowerOff),
  {0, 0}
};


typedef enum _SYSDBG_COMMAND {
  SysDbgQueryModuleInformation = 0,
  SysDbgQueryTraceInformation = 1,
  SysDbgSetTracepoint = 2,
  SysDbgSetSpecialCall = 3,
  SysDbgClearSpecialCalls = 4,
  SysDbgQuerySpecialCalls = 5,
  SysDbgBreakPoint = 6,
  SysDbgQueryVersion = 7,
  SysDbgReadVirtual = 8,
  SysDbgWriteVirtual = 9,
  SysDbgReadPhysical = 10,
  SysDbgWritePhysical = 11,
  SysDbgReadControlSpace = 12,
  SysDbgWriteControlSpace = 13,
  SysDbgReadIoSpace = 14,
  SysDbgWriteIoSpace = 15,
  SysDbgReadMsr = 16,
  SysDbgWriteMsr = 17,
  SysDbgReadBusData = 18,
  SysDbgWriteBusData = 19,
  SysDbgCheckLowMemory = 20,
  SysDbgEnableKernelDebugger = 21,
  SysDbgDisableKernelDebugger = 22,
  SysDbgGetAutoKdEnable = 23,
  SysDbgSetAutoKdEnable = 24,
  SysDbgGetPrintBufferSize = 25,
  SysDbgSetPrintBufferSize = 26,
  SysDbgGetKdUmExceptionEnable = 27,
  SysDbgSetKdUmExceptionEnable = 28,
  SysDbgGetTriageDump = 29,
  SysDbgGetKdBlockEnable = 30,
  SysDbgSetKdBlockEnable = 31,
  SysDbgRegisterForUmBreakInfo = 32,
  SysDbgGetUmBreakPid = 33,
  SysDbgClearUmBreakPid = 34,
  SysDbgGetUmAttachPid = 35,
  SysDbgClearUmAttachPid = 36,
  SysDbgGetLiveKernelDump = 37,
} SYSDBG_COMMAND;

EnumMap SYSDBG_COMMAND_MAP[] =
{
  DEF(SysDbgQueryModuleInformation),
  DEF(SysDbgQueryTraceInformation),
  DEF(SysDbgSetTracepoint),
  DEF(SysDbgSetSpecialCall),
  DEF(SysDbgClearSpecialCalls),
  DEF(SysDbgQuerySpecialCalls),
  DEF(SysDbgBreakPoint),
  DEF(SysDbgQueryVersion),
  DEF(SysDbgReadVirtual),
  DEF(SysDbgWriteVirtual),
  DEF(SysDbgReadPhysical),
  DEF(SysDbgWritePhysical),
  DEF(SysDbgReadControlSpace),
  DEF(SysDbgWriteControlSpace),
  DEF(SysDbgReadIoSpace),
  DEF(SysDbgWriteIoSpace),
  DEF(SysDbgReadMsr),
  DEF(SysDbgWriteMsr),
  DEF(SysDbgReadBusData),
  DEF(SysDbgWriteBusData),
  DEF(SysDbgCheckLowMemory),
  DEF(SysDbgEnableKernelDebugger),
  DEF(SysDbgDisableKernelDebugger),
  DEF(SysDbgGetAutoKdEnable),
  DEF(SysDbgSetAutoKdEnable),
  DEF(SysDbgGetPrintBufferSize),
  DEF(SysDbgSetPrintBufferSize),
  DEF(SysDbgGetKdUmExceptionEnable),
  DEF(SysDbgSetKdUmExceptionEnable),
  DEF(SysDbgGetTriageDump),
  DEF(SysDbgGetKdBlockEnable),
  DEF(SysDbgSetKdBlockEnable),
  DEF(SysDbgRegisterForUmBreakInfo),
  DEF(SysDbgGetUmBreakPid),
  DEF(SysDbgClearUmBreakPid),
  DEF(SysDbgGetUmAttachPid),
  DEF(SysDbgClearUmAttachPid),
  DEF(SysDbgGetLiveKernelDump),
  {0, 0}
};


typedef enum _SYSTEM_POWER_STATE {
  PowerSystemUnspecified = 0,
  PowerSystemWorking = 1,
  PowerSystemSleeping1 = 2,
  PowerSystemSleeping2 = 3,
  PowerSystemSleeping3 = 4,
  PowerSystemHibernate = 5,
  PowerSystemShutdown = 6,
  PowerSystemMaximum = 7,
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
  SystemBasicInformation = 0,
  SystemProcessorInformation = 1,
  SystemPerformanceInformation = 2,
  SystemTimeOfDayInformation = 3,
  SystemPathInformation = 4,
  SystemProcessInformation = 5,
  SystemCallCountInformation = 6,
  SystemDeviceInformation = 7,
  SystemProcessorPerformanceInformation = 8,
  SystemFlagsInformation = 9,
  SystemCallTimeInformation = 10,
  SystemModuleInformation = 11,
  SystemLocksInformation = 12,
  SystemStackTraceInformation = 13,
  SystemPagedPoolInformation = 14,
  SystemNonPagedPoolInformation = 15,
  SystemHandleInformation = 16,
  SystemObjectInformation = 17,
  SystemPageFileInformation = 18,
  SystemVdmInstemulInformation = 19,
  SystemVdmBopInformation = 20,
  SystemFileCacheInformation = 21,
  SystemPoolTagInformation = 22,
  SystemInterruptInformation = 23,
  SystemDpcBehaviorInformation = 24,
  SystemFullMemoryInformation = 25,
  SystemLoadGdiDriverInformation = 26,
  SystemUnloadGdiDriverInformation = 27,
  SystemTimeAdjustmentInformation = 28,
  SystemSummaryMemoryInformation = 29,
  SystemMirrorMemoryInformation = 30,
  SystemPerformanceTraceInformation = 31,
  SystemCrashDumpInformation = 32, // SystemObsolete0 in ole32.pdb
  SystemExceptionInformation = 33,
  SystemCrashDumpStateInformation = 34,
  SystemKernelDebuggerInformation = 35,
  SystemContextSwitchInformation = 36,
  SystemRegistryQuotaInformation = 37,
  SystemExtendServiceTableInformation = 38,
  SystemPrioritySeperation = 39,
  SystemVerifierAddDriverInformation = 40,
  SystemVerifierRemoveDriverInformation = 41,
  SystemProcessorIdleInformation = 42,
  SystemLegacyDriverInformation = 43,
  SystemCurrentTimeZoneInformation = 44,
  SystemLookasideInformation = 45,
  SystemTimeSlipNotification = 46,
  SystemSessionCreate = 47,
  SystemSessionDetach = 48,
  SystemSessionInformation = 49,
  SystemRangeStartInformation = 50,
  SystemVerifierInformation = 51,
  SystemVerifierThunkExtend = 52,
  SystemSessionProcessInformation = 53,
  SystemLoadGdiDriverInSystemSpace = 54,
  SystemNumaProcessorMap = 55,
  SystemPrefetcherInformation = 56,
  SystemExtendedProcessInformation = 57,
  SystemRecommendedSharedDataAlignment = 58,
  SystemComPlusPackage = 59,
  SystemNumaAvailableMemory = 60,
  SystemProcessorPowerInformation = 61,
  SystemEmulationBasicInformation = 62,
  SystemEmulationProcessorInformation = 63,
  SystemExtendedHandleInformation = 64,
  SystemLostDelayedWriteInformation = 65,
  SystemBigPoolInformation = 66,
  SystemSessionPoolTagInformation = 67,
  SystemSessionMappedViewInformation = 68,
  SystemHotpatchInformation = 69,
  SystemObjectSecurityMode = 70,
  SystemWatchdogTimerHandler = 71,
  SystemWatchdogTimerInformation = 72,
  SystemLogicalProcessorInformation = 73,
  SystemWow64SharedInformationObsolete = 74,
  SystemRegisterFirmwareTableInformationHandler = 75,
  SystemFirmwareTableInformation = 76,
  SystemModuleInformationEx = 77,
  SystemVerifierTriageInformation = 78,
  SystemSuperfetchInformation = 79,
  SystemMemoryListInformation = 80,
  SystemFileCacheInformationEx = 81,
  SystemThreadPriorityClientIdInformation = 82,
  SystemProcessorIdleCycleTimeInformation = 83,
  SystemVerifierCancellationInformation = 84,
  SystemProcessorPowerInformationEx = 85,
  SystemRefTraceInformation = 86,
  SystemSpecialPoolInformation = 87,
  SystemProcessIdInformation = 88,
  SystemErrorPortInformation = 89,
  SystemBootEnvironmentInformation = 90,
  SystemHypervisorInformation = 91,
  SystemVerifierInformationEx = 92,
  SystemTimeZoneInformation = 93,
  SystemImageFileExecutionOptionsInformation = 94,
  SystemCoverageInformation = 95,
  SystemPrefetchPatchInformation = 96,
  SystemVerifierFaultsInformation = 97,
  SystemSystemPartitionInformation = 98,
  SystemSystemDiskInformation = 99,
  SystemProcessorPerformanceDistribution = 100,
  SystemNumaProximityNodeInformation = 101,
  SystemDynamicTimeZoneInformation = 102,
  SystemCodeIntegrityInformation = 103,
  SystemProcessorMicrocodeUpdateInformation = 104,
  SystemProcessorBrandString = 105,
  SystemVirtualAddressInformation = 106,
  SystemLogicalProcessorAndGroupInformation = 107,
  SystemProcessorCycleTimeInformation = 108,
  SystemStoreInformation = 109,
  SystemRegistryAppendString = 110,
  SystemAitSamplingValue = 111,
  SystemVhdBootInformation = 112,
  SystemCpuQuotaInformation = 113,
  SystemNativeBasicInformation = 114,
  SystemErrorPortTimeouts = 115,
  SystemLowPriorityIoInformation = 116,
  SystemBootEntropyInformation = 117,
  SystemVerifierCountersInformation = 118,
  SystemPagedPoolInformationEx = 119,
  SystemSystemPtesInformationEx = 120,
  SystemNodeDistanceInformation = 121,
  SystemAcpiAuditInformation = 122,
  SystemBasicPerformanceInformation = 123,
  SystemQueryPerformanceCounterInformation = 124,
  SystemSessionBigPoolInformation = 125,
  SystemBootGraphicsInformation = 126,
  SystemScrubPhysicalMemoryInformation = 127,
  SystemBadPageInformation = 128,
  SystemProcessorProfileControlArea = 129,
  SystemCombinePhysicalMemoryInformation = 130,
  SystemEntropyInterruptTimingInformation = 131,
  SystemConsoleInformation = 132,
  SystemPlatformBinaryInformation = 133,
  SystemPolicyInformation = 134,
  SystemHypervisorProcessorCountInformation = 135,
  SystemDeviceDataInformation = 136,
  SystemDeviceDataEnumerationInformation = 137,
  SystemMemoryTopologyInformation = 138,
  SystemMemoryChannelInformation = 139,
  SystemBootLogoInformation = 140,
  SystemProcessorPerformanceInformationEx = 141,
  SystemCriticalProcessErrorLogInformation = 142,
  SystemSecureBootPolicyInformation = 143,
  SystemPageFileInformationEx = 144,
  SystemSecureBootInformation = 145,
  SystemEntropyInterruptTimingRawInformation = 146,
  SystemPortableWorkspaceEfiLauncherInformation = 147,
  SystemFullProcessInformation = 148,
  SystemKernelDebuggerInformationEx = 149,
  SystemBootMetadataInformation = 150,
  SystemSoftRebootInformation = 151,
  SystemElamCertificateInformation = 152,
  SystemOfflineDumpConfigInformation = 153,
  SystemProcessorFeaturesInformation = 154,
  SystemRegistryReconciliationInformation = 155,
  SystemEdidInformation = 156,
  SystemManufacturingInformation = 157,
  SystemEnergyEstimationConfigInformation = 158,
  SystemHypervisorDetailInformation = 159,
  SystemProcessorCycleStatsInformation = 160,
  SystemVmGenerationCountInformation = 161,
  SystemTrustedPlatformModuleInformation = 162,
  SystemKernelDebuggerFlags = 163,
  SystemCodeIntegrityPolicyInformation = 164,
  SystemIsolatedUserModeInformation = 165,
  SystemHardwareSecurityTestInterfaceResultsInformation = 166,
  SystemSingleModuleInformation = 167,
  SystemAllowedCpuSetsInformation = 168,
  SystemVsmProtectionInformation = 169,
  SystemInterruptCpuSetsInformation = 170,
  SystemSecureBootPolicyFullInformation = 171,
  SystemCodeIntegrityPolicyFullInformation = 172,
  SystemAffinitizedInterruptProcessorInformation = 173,
  SystemRootSiloInformation = 174,
  SystemCpuSetInformation = 175,
  SystemCpuSetTagInformation = 176,
  SystemWin32WerStartCallout = 177,
  SystemSecureKernelProfileInformation = 178,
  SystemCodeIntegrityPlatformManifestInformation = 179,
  SystemInterruptSteeringInformation = 180,
  SystemSupportedProcessorArchitectures = 181,
  SystemMemoryUsageInformation = 182,
  SystemCodeIntegrityCertificateInformation = 183,
  SystemPhysicalMemoryInformation = 184,
  SystemControlFlowTransition = 185,
  SystemKernelDebuggingAllowed = 186,
  SystemActivityModerationExeState = 187,
  SystemActivityModerationUserSettings = 188,
  SystemCodeIntegrityPoliciesFullInformation = 189,
  SystemCodeIntegrityUnlockInformation = 190,
  SystemIntegrityQuotaInformation = 191,
  SystemFlushInformation = 192,
  SystemProcessorIdleMaskInformation = 193,
  SystemSecureDumpEncryptionInformation = 194,
  SystemWriteConstraintInformation = 195,
  SystemKernelVaShadowInformation = 196,
  SystemHypervisorSharedPageInformation = 197,
  SystemFirmwareBootPerformanceInformation = 198,
  SystemCodeIntegrityVerificationInformation = 199,
  SystemFirmwarePartitionInformation = 200,
  SystemSpeculationControlInformation = 201,
  SystemDmaGuardPolicyInformation = 202,
  SystemEnclaveLaunchControlInformation = 203,
  SystemWorkloadAllowedCpuSetsInformation = 204,
  SystemCodeIntegrityUnlockModeInformation = 205,
  SystemLeapSecondInformation = 206,
  SystemFlags2Information = 207,
  MaxSystemInfoClass = 208,
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
  DEF(SystemPrefetchPatchInformation),
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
  DEF(SystemLogicalProcessorAndGroupInformation),
  DEF(SystemProcessorCycleTimeInformation),
  DEF(SystemStoreInformation),
  DEF(SystemRegistryAppendString),
  DEF(SystemAitSamplingValue),
  DEF(SystemVhdBootInformation),
  DEF(SystemCpuQuotaInformation),
  DEF(SystemNativeBasicInformation),
  DEF(SystemErrorPortTimeouts),
  DEF(SystemLowPriorityIoInformation),
  DEF(SystemBootEntropyInformation),
  DEF(SystemVerifierCountersInformation),
  DEF(SystemPagedPoolInformationEx),
  DEF(SystemSystemPtesInformationEx),
  DEF(SystemNodeDistanceInformation),
  DEF(SystemAcpiAuditInformation),
  DEF(SystemBasicPerformanceInformation),
  DEF(SystemQueryPerformanceCounterInformation),
  DEF(SystemSessionBigPoolInformation),
  DEF(SystemBootGraphicsInformation),
  DEF(SystemScrubPhysicalMemoryInformation),
  DEF(SystemBadPageInformation),
  DEF(SystemProcessorProfileControlArea),
  DEF(SystemCombinePhysicalMemoryInformation),
  DEF(SystemEntropyInterruptTimingInformation),
  DEF(SystemConsoleInformation),
  DEF(SystemPlatformBinaryInformation),
  DEF(SystemPolicyInformation),
  DEF(SystemHypervisorProcessorCountInformation),
  DEF(SystemDeviceDataInformation),
  DEF(SystemDeviceDataEnumerationInformation),
  DEF(SystemMemoryTopologyInformation),
  DEF(SystemMemoryChannelInformation),
  DEF(SystemBootLogoInformation),
  DEF(SystemProcessorPerformanceInformationEx),
  DEF(SystemCriticalProcessErrorLogInformation),
  DEF(SystemSecureBootPolicyInformation),
  DEF(SystemPageFileInformationEx),
  DEF(SystemSecureBootInformation),
  DEF(SystemEntropyInterruptTimingRawInformation),
  DEF(SystemPortableWorkspaceEfiLauncherInformation),
  DEF(SystemFullProcessInformation),
  DEF(SystemKernelDebuggerInformationEx),
  DEF(SystemBootMetadataInformation),
  DEF(SystemSoftRebootInformation),
  DEF(SystemElamCertificateInformation),
  DEF(SystemOfflineDumpConfigInformation),
  DEF(SystemProcessorFeaturesInformation),
  DEF(SystemRegistryReconciliationInformation),
  DEF(SystemEdidInformation),
  DEF(SystemManufacturingInformation),
  DEF(SystemEnergyEstimationConfigInformation),
  DEF(SystemHypervisorDetailInformation),
  DEF(SystemProcessorCycleStatsInformation),
  DEF(SystemVmGenerationCountInformation),
  DEF(SystemTrustedPlatformModuleInformation),
  DEF(SystemKernelDebuggerFlags),
  DEF(SystemCodeIntegrityPolicyInformation),
  DEF(SystemIsolatedUserModeInformation),
  DEF(SystemHardwareSecurityTestInterfaceResultsInformation),
  DEF(SystemSingleModuleInformation),
  DEF(SystemAllowedCpuSetsInformation),
  DEF(SystemVsmProtectionInformation),
  DEF(SystemInterruptCpuSetsInformation),
  DEF(SystemSecureBootPolicyFullInformation),
  DEF(SystemCodeIntegrityPolicyFullInformation),
  DEF(SystemAffinitizedInterruptProcessorInformation),
  DEF(SystemRootSiloInformation),
  DEF(SystemCpuSetInformation),
  DEF(SystemCpuSetTagInformation),
  DEF(SystemWin32WerStartCallout),
  DEF(SystemSecureKernelProfileInformation),
  DEF(SystemCodeIntegrityPlatformManifestInformation),
  DEF(SystemInterruptSteeringInformation),
  DEF(SystemSupportedProcessorArchitectures),
  DEF(SystemMemoryUsageInformation),
  DEF(SystemCodeIntegrityCertificateInformation),
  DEF(SystemPhysicalMemoryInformation),
  DEF(SystemControlFlowTransition),
  DEF(SystemKernelDebuggingAllowed),
  DEF(SystemActivityModerationExeState),
  DEF(SystemActivityModerationUserSettings),
  DEF(SystemCodeIntegrityPoliciesFullInformation),
  DEF(SystemCodeIntegrityUnlockInformation),
  DEF(SystemIntegrityQuotaInformation),
  DEF(SystemFlushInformation),
  DEF(SystemProcessorIdleMaskInformation),
  DEF(SystemSecureDumpEncryptionInformation),
  DEF(SystemWriteConstraintInformation),
  DEF(SystemKernelVaShadowInformation),
  DEF(SystemHypervisorSharedPageInformation),
  DEF(SystemFirmwareBootPerformanceInformation),
  DEF(SystemCodeIntegrityVerificationInformation),
  DEF(SystemFirmwarePartitionInformation),
  DEF(SystemSpeculationControlInformation),
  DEF(SystemDmaGuardPolicyInformation),
  DEF(SystemEnclaveLaunchControlInformation),
  DEF(SystemWorkloadAllowedCpuSetsInformation),
  DEF(SystemCodeIntegrityUnlockModeInformation),
  DEF(SystemLeapSecondInformation),
  DEF(SystemFlags2Information),
  {0, 0}
};


typedef enum _THREADINFOCLASS {
  ThreadBasicInformation = 0,
  ThreadTimes = 1,
  ThreadPriority = 2,
  ThreadBasePriority = 3,
  ThreadAffinityMask = 4,
  ThreadImpersonationToken = 5,
  ThreadDescriptorTableEntry = 6,
  ThreadEnableAlignmentFaultFixup = 7,
  ThreadEventPair_Reusable = 8,
  ThreadQuerySetWin32StartAddress = 9,
  ThreadZeroTlsCell = 10,
  ThreadPerformanceCount = 11,
  ThreadAmILastThread = 12,
  ThreadIdealProcessor = 13,
  ThreadPriorityBoost = 14,
  ThreadSetTlsArrayAddress = 15,
  ThreadIsIoPending = 16,
  ThreadHideFromDebugger = 17,
  ThreadBreakOnTermination = 18,
  ThreadSwitchLegacyState = 19,
  ThreadIsTerminated = 20,
  ThreadLastSystemCall = 21,
  ThreadIoPriority = 22,
  ThreadCycleTime = 23,
  ThreadPagePriority = 24,
  ThreadActualBasePriority = 25,
  ThreadTebInformation = 26,
  ThreadCSwitchMon = 27,
  ThreadCSwitchPmu = 28,
  ThreadWow64Context = 29,
  ThreadGroupInformation = 30,
  ThreadUmsInformation = 31,
  ThreadCounterProfiling = 32,
  ThreadIdealProcessorEx = 33,
  ThreadCpuAccountingInformation = 34,
  ThreadSuspendCount = 35,
  ThreadHeterogeneousCpuPolicy = 36,
  ThreadContainerId = 37,
  ThreadNameInformation = 38,
  ThreadSelectedCpuSets = 39,
  ThreadSystemThreadInformation = 40,
  ThreadActualGroupAffinity = 41,
  ThreadDynamicCodePolicyInfo = 42,
  ThreadExplicitCaseSensitivity = 43,
  ThreadWorkOnBehalfTicket = 44,
  ThreadSubsystemInformation = 45,
  ThreadDbgkWerReportActive = 46,
  ThreadAttachContainer = 47,
  ThreadManageWritesToExecutableMemory = 48,
  ThreadPowerThrottlingState = 49,
  ThreadWorkloadClass = 50,
  MaxThreadInfoClass = 51,
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
  DEF(ThreadCpuAccountingInformation),
  DEF(ThreadSuspendCount),
  DEF(ThreadHeterogeneousCpuPolicy),
  DEF(ThreadContainerId),
  DEF(ThreadNameInformation),
  DEF(ThreadSelectedCpuSets),
  DEF(ThreadSystemThreadInformation),
  DEF(ThreadActualGroupAffinity),
  DEF(ThreadDynamicCodePolicyInfo),
  DEF(ThreadExplicitCaseSensitivity),
  DEF(ThreadWorkOnBehalfTicket),
  DEF(ThreadSubsystemInformation),
  DEF(ThreadDbgkWerReportActive),
  DEF(ThreadAttachContainer),
  DEF(ThreadManageWritesToExecutableMemory),
  DEF(ThreadPowerThrottlingState),
  DEF(ThreadWorkloadClass),
  {0, 0}
};


typedef enum _TIMER_INFORMATION_CLASS {
  TimerBasicInformation = 0,
} TIMER_INFORMATION_CLASS;

EnumMap TIMER_INFORMATION_CLASS_MAP[] = 
{
  DEF(TimerBasicInformation),
  {0, 0}
};


typedef enum _TIMER_TYPE {
  NotificationTimer = 0,
  SynchronizationTimer = 1,
} TIMER_TYPE;

EnumMap TIMER_TYPE_MAP[] = 
{
  DEF(NotificationTimer),
  DEF(SynchronizationTimer),
  {0, 0}
};


typedef enum _TOKEN_INFORMATION_CLASS {
  TokenUser = 1,
  TokenGroups = 2,
  TokenPrivileges = 3,
  TokenOwner = 4,
  TokenPrimaryGroup = 5,
  TokenDefaultDacl = 6,
  TokenSource = 7,
  TokenType = 8,
  TokenImpersonationLevel = 9,
  TokenStatistics = 10,
  TokenRestrictedSids = 11,
  TokenSessionId = 12,
  TokenGroupsAndPrivileges = 13,
  TokenSessionReference = 14,
  TokenSandBoxInert = 15,
  TokenAuditPolicy = 16,
  TokenOrigin = 17,
  TokenElevationType = 18,
  TokenLinkedToken = 19,
  TokenElevation = 20,
  TokenHasRestrictions = 21,
  TokenAccessInformation = 22,
  TokenVirtualizationAllowed = 23,
  TokenVirtualizationEnabled = 24,
  TokenIntegrityLevel = 25,
  TokenUIAccess = 26,
  TokenMandatoryPolicy = 27,
  TokenLogonSid = 28,
  TokenIsAppContainer = 29,
  TokenCapabilities = 30,
  TokenAppContainerSid = 31,
  TokenAppContainerNumber = 32,
  TokenUserClaimAttributes = 33,
  TokenDeviceClaimAttributes = 34,
  TokenRestrictedUserClaimAttributes = 35,
  TokenRestrictedDeviceClaimAttributes = 36,
  TokenDeviceGroups = 37,
  TokenRestrictedDeviceGroups = 38,
  TokenSecurityAttributes = 39,
  TokenIsRestricted = 40,
  TokenProcessTrustLevel = 41,
  TokenPrivateNameSpace = 42,
  TokenSingletonAttributes = 43,
  TokenBnoIsolation = 44,
  TokenChildProcessFlags = 45,
  TokenIsLessPrivilegedAppContainer = 46,
  MaxTokenInfoClass = 47,
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
  DEF(TokenProcessTrustLevel),
  DEF(TokenPrivateNameSpace),
  DEF(TokenSingletonAttributes),
  DEF(TokenBnoIsolation),
  DEF(TokenChildProcessFlags),
  DEF(TokenIsLessPrivilegedAppContainer),
  {0, 0}
};


typedef enum _TOKEN_TYPE {
  TokenPrimary = 1,
  TokenImpersonation = 2,
} TOKEN_TYPE;

EnumMap TOKEN_TYPE_MAP[] =
{
  {0, 0}
};


typedef enum _TRANSACTION_INFORMATION_CLASS {
  TransactionBasicInformation = 0,
  TransactionPropertiesInformation = 1,
  TransactionEnlistmentInformation = 2,
  TransactionSuperiorEnlistmentInformation = 3,
  TransactionBindInformation = 4,
  TransactionDTCPrivateInformation = 5,
} TRANSACTION_INFORMATION_CLASS;

EnumMap TRANSACTION_INFORMATION_CLASS_MAP[] = 
{
  DEF(TransactionBasicInformation),
  DEF(TransactionPropertiesInformation),
  DEF(TransactionEnlistmentInformation),
  DEF(TransactionSuperiorEnlistmentInformation),
  DEF(TransactionBindInformation),
  DEF(TransactionDTCPrivateInformation),
  {0, 0}
};


typedef enum _TRANSACTIONMANAGER_INFORMATION_CLASS {
  TransactionManagerBasicInformation = 0,
  TransactionManagerLogInformation = 1,
  TransactionManagerLogPathInformation = 2,
  TransactionManagerOnlineProbeInformation = 3,
  TransactionManagerRecoveryInformation = 4,
  TransactionManagerOldestTransactionInformation = 5,
} TRANSACTIONMANAGER_INFORMATION_CLASS;

EnumMap TRANSACTIONMANAGER_INFORMATION_CLASS_MAP[] = 
{
  DEF(TransactionManagerBasicInformation),
  DEF(TransactionManagerLogInformation),
  DEF(TransactionManagerLogPathInformation),
  DEF(TransactionManagerOnlineProbeInformation),
  DEF(TransactionManagerRecoveryInformation),
  DEF(TransactionManagerOldestTransactionInformation),
  {0, 0}
};


typedef enum _WAIT_TYPE {
  WaitAll = 0,
  WaitAny = 1,
  WaitNotification = 2,
  WaitDequeue = 3,
} WAIT_TYPE;

EnumMap WAIT_TYPE_MAP[] =
{
  DEF(WaitAll),
  DEF(WaitAny),
  DEF(WaitNotification),
  DEF(WaitDequeue),
  {0, 0}
};


typedef enum _WORKERFACTORYINFOCLASS {
  WorkerFactoryTimeout = 0,
  WorkerFactoryRetryTimeout = 1,
  WorkerFactoryIdleTimeout = 2,
  WorkerFactoryBindingCount = 3,
  WorkerFactoryThreadMinimum = 4,
  WorkerFactoryThreadMaximum = 5,
  WorkerFactoryPaused = 6,
  WorkerFactoryBasicInformation = 7,
  WorkerFactoryAdjustThreadGoal = 8,
  WorkerFactoryCallbackType = 9,
  WorkerFactoryStackInformation = 10,
  WorkerFactoryThreadBasePriority = 11,
  WorkerFactoryTimeoutWaiters = 12,
  WorkerFactoryFlags = 13,
  WorkerFactoryThreadSoftMaximum = 14,
  MaxWorkerFactoryInfoClass = 15,
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
  DEF(WorkerFactoryThreadBasePriority),
  DEF(WorkerFactoryTimeoutWaiters),
  DEF(WorkerFactoryFlags),
  DEF(WorkerFactoryThreadSoftMaximum),
  DEF(MaxWorkerFactoryInfoClass),
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
   DEF(DEVICE_POWER_STATE),
   DEF(DIRECTORY_NOTIFY_INFORMATION_CLASS),
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
   DEF(MEMORY_PARTITION_INFORMATION_CLASS),
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
   DEF(SYSDBG_COMMAND),
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

