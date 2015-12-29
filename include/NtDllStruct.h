#ifndef NtDllStruct_H_
#define NtDllStruct_H_

/**@file

    Header for NtDll Structures - defines some data structures used by the native API.

    @author Roger Orr <rogero@howzatt.demon.co.uk>

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
    Please report bugs to rogero@howzatt.demon.co.uk.

    $Revision: 1405 $
*/

// $Id: NtDllStruct.h 1405 2013-11-12 23:57:17Z Roger $

// For MSVC 5.00/6.00 basetsd.h (changes with platform SDK)
#ifndef _W64
typedef UINT_PTR ULONG_PTR;
typedef ULONG SIZE_T;
#endif

// Nt services data structures

/** Status codes returned from the Native API */
typedef LONG NTSTATUS;

/** Testing for success */
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)

/** Unicode String used in Native API */
typedef struct _UNICODE_STRING {
    USHORT Length;         ///< Length of string in bytes
    USHORT MaximumLength;  ///< Maximum length of Buffer
    PWSTR  Buffer;         ///< Pointer to Unicode string
} UNICODE_STRING, *PUNICODE_STRING;

/** Object Attributes used in Native API */
typedef struct _OBJECT_ATTRIBUTES {
    ULONG Length;                   ///< Length of the structure
    HANDLE RootDirectory;           ///< Handle to the root object directory or NULL
    PUNICODE_STRING ObjectName;     ///< Name of the object
    ULONG Attributes;               ///< Attributes for the object
    PVOID SecurityDescriptor;       ///< Security descriptor, or NULL
    PVOID SecurityQualityOfService; ///< Quality of service
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

/** Client ID used in Native API */
typedef struct _CLIENT_ID {
    HANDLE UniqueProcess; ///< Process ID
    HANDLE UniqueThread;  ///< Thread ID
} CLIENT_ID, *PCLIENT_ID;

/** I/O status block used in Native API */
typedef struct _IO_STATUS_BLOCK {
    union {
        NTSTATUS Status; ///< NT status value
        PVOID Pointer;   ///< Pointer
    };

    ULONG_PTR Information; ///< Additional information
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

/** Basic File Information used in Native API */
typedef struct _FILE_BASIC_INFORMATION {                    
    LARGE_INTEGER CreationTime;    ///< Time of creation
    LARGE_INTEGER LastAccessTime;  ///< Time of last access
    LARGE_INTEGER LastWriteTime;   ///< Time of last write
    LARGE_INTEGER ChangeTime;      ///< Time of last change
    ULONG FileAttributes;          ///< File attributes
} FILE_BASIC_INFORMATION, *PFILE_BASIC_INFORMATION;         

/** Network Open Information used in Native API */
typedef struct _FILE_NETWORK_OPEN_INFORMATION {                 
    LARGE_INTEGER CreationTime;    ///< Time of creation
    LARGE_INTEGER LastAccessTime;  ///< Time of last access
    LARGE_INTEGER LastWriteTime;   ///< Time of last write
    LARGE_INTEGER ChangeTime;      ///< Time of last change
    LARGE_INTEGER AllocationSize;  ///< Size of file allocation
    LARGE_INTEGER EndOfFile;       ///< End of file position
    ULONG FileAttributes;          ///< File attributes
} FILE_NETWORK_OPEN_INFORMATION, *PFILE_NETWORK_OPEN_INFORMATION;   

/** Object returned from NtQueryDirectoryObject */
typedef struct _OBJECT_DIRECTORY_INFORMATION {
    UNICODE_STRING Name;           ///< Name of directory object
    UNICODE_STRING TypeName;       ///< Type of directory object, eg "Section"
} OBJECT_DIRECTORY_INFORMATION, *POBJECT_DIRECTORY_INFORMATION;

/** LPC message structure */                                                           
typedef struct _LPC_MESSAGE {
    USHORT          DataLength;     ///< Data length
    USHORT          Length;         ///< Length of message, including this header
    USHORT          MessageType;    ///< LPC_TYPE
    USHORT          DataInfoOffset; ///< Offset to data information
    CLIENT_ID       ClientId;       ///< Client ID
    ULONG_PTR       MessageId;      ///< Message identifier (incrementing counter)
    ULONG_PTR       CallbackId;     ///< Callback identifier
} LPC_MESSAGE, *PLPC_MESSAGE;

#endif // NtDllStruct_H_