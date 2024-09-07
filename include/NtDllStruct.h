#ifndef NtDllStruct_H_
#define NtDllStruct_H_

/**@file

    Header for NtDll Structures - defines some data structures used by the
   native API.

    @author Roger Orr <rogero@howzatt.co.uk>

    Copyright &copy; 2004.
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

    $Revision: 2459 $
*/

// $Id: NtDllStruct.h 2459 2024-09-07 17:46:05Z roger $

// For MSVC 5.00/6.00 basetsd.h (changes with platform SDK)
#ifndef _W64
using ULONG_PTR = UINT_PTR;
using SIZE_T = ULONG;
#endif

// Nt services data structures

/** Status codes returned from the Native API */
using NTSTATUS = LONG;

/** Testing for success */
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)

/** Unicode String used in Native API */
struct UNICODE_STRING {
  USHORT Length;        ///< Length of string in bytes
  USHORT MaximumLength; ///< Maximum length of Buffer
  PWSTR Buffer;         ///< Pointer to Unicode string
};
using PUNICODE_STRING = UNICODE_STRING *;

/** Object Attributes used in Native API */
struct OBJECT_ATTRIBUTES {
  ULONG Length;               ///< Length of the structure
  HANDLE RootDirectory;       ///< Handle to the root object directory or NULL
  PUNICODE_STRING ObjectName; ///< Name of the object
  ULONG Attributes;           ///< Attributes for the object
  PVOID SecurityDescriptor;   ///< Security descriptor, or NULL
  PVOID SecurityQualityOfService; ///< Quality of service
};
using POBJECT_ATTRIBUTES = OBJECT_ATTRIBUTES *;

/** Client ID used in Native API */
struct CLIENT_ID {
  HANDLE UniqueProcess; ///< Process ID
  HANDLE UniqueThread;  ///< Thread ID
};
using PCLIENT_ID = CLIENT_ID *;

/** I/O status block used in Native API */
struct IO_STATUS_BLOCK {
  union {
    NTSTATUS Status; ///< NT status value
    PVOID Pointer;   ///< Pointer
  };

  ULONG_PTR Information; ///< Additional information
};
using PIO_STATUS_BLOCK = IO_STATUS_BLOCK *;

/** Basic File Information used in Native API */
struct FILE_BASIC_INFORMATION {
  LARGE_INTEGER CreationTime;   ///< Time of creation
  LARGE_INTEGER LastAccessTime; ///< Time of last access
  LARGE_INTEGER LastWriteTime;  ///< Time of last write
  LARGE_INTEGER ChangeTime;     ///< Time of last change
  ULONG FileAttributes;         ///< File attributes
};
using PFILE_BASIC_INFORMATION = FILE_BASIC_INFORMATION *;

/** Network Open Information used in Native API */
struct FILE_NETWORK_OPEN_INFORMATION {
  LARGE_INTEGER CreationTime;   ///< Time of creation
  LARGE_INTEGER LastAccessTime; ///< Time of last access
  LARGE_INTEGER LastWriteTime;  ///< Time of last write
  LARGE_INTEGER ChangeTime;     ///< Time of last change
  LARGE_INTEGER AllocationSize; ///< Size of file allocation
  LARGE_INTEGER EndOfFile;      ///< End of file position
  ULONG FileAttributes;         ///< File attributes
};
using PFILE_NETWORK_OPEN_INFORMATION = FILE_NETWORK_OPEN_INFORMATION *;

/** Object returned from NtQueryDirectoryObject */
struct OBJECT_DIRECTORY_INFORMATION {
  UNICODE_STRING Name;     ///< Name of directory object
  UNICODE_STRING TypeName; ///< Type of directory object, eg "Section"
};
using POBJECT_DIRECTORY_INFORMATION = OBJECT_DIRECTORY_INFORMATION *;

/** LPC message structure */
struct LPC_MESSAGE {
  USHORT DataLength;     ///< Data length
  USHORT Length;         ///< Length of message, including this header
  USHORT MessageType;    ///< LPC_TYPE
  USHORT DataInfoOffset; ///< Offset to data information
  CLIENT_ID ClientId;    ///< Client ID
  ULONG_PTR MessageId;   ///< Message identifier (incrementing counter)
  ULONG_PTR CallbackId;  ///< Callback identifier
};
using PLPC_MESSAGE = LPC_MESSAGE *;

#endif // NtDllStruct_H_