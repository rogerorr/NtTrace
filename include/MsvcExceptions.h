#ifndef MSVCEXCEPTIONS_H_
#define MSVCEXCEPTIONS_H_

/**@file

    Data for Microsoft Visual Studio exceptions

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

// $Id: MsvcExceptions.h 1405 2013-11-12 23:57:17Z Roger $

/* Value of ExceptionCode for an MSVC C++ exception ("msc") */
static DWORD const MSVC_EXCEPTION = 0xe06d7363;

/* Value of ExceptionCode for an MSVC notification exception (used for 'SetThreadName') */
static DWORD const MSVC_NOTIFICATION = 0x406d1388;

/* Value of ExceptionCode for an MS CLR exception ("COM") */
static DWORD const CLR_EXCEPTION = 0xe0434f4d;

/* Value of ExceptionCode for an MS CLR 4 exception ("CCR") */
static DWORD const CLR_EXCEPTION_V4 = 0xe0434352;

/* Value of ExceptionCode for an MS CLR notification */
static DWORD const CLR_NOTIFICATION = 0x04242420;

/** Value of ExceptionInformation[0] for an MSVC C++ exception */
static DWORD const MSVC_MAGIC_NUMBER1 = 0x19930520; // ' Name taken from EHSUP.INC (!)

/*
 * Data passed about with a thrown object in MSVC.
 */

// Forward references
struct MsvcClassHeader;
struct MsvcClassInfo;

/**
  * Data associated with a thrown object in MSVC
  *
  * ExceptionInformation[2] points to one of these, unless it is a no-arg throw
  * when the pointer is null.
  */
struct MsvcThrow
{
    DWORD unknown1;                ///< Use is not known
    DWORD pDestructor;             ///< Offset to destructor, or null
    DWORD unknown2;                ///< Use is not known
    DWORD pClassHeader;            ///< Offset to CTA2PAD (MsvcClassHeader)
};

/** Class header for a thrown object in MSVC */
struct MsvcClassHeader
{
    DWORD cInfo;            ///< Count of Info structures
    DWORD Info[1];          ///< Actually variable size structure (MsvcClassInfo)
};

/** Information pointed to by the class header for a thrown object in MSVC */
struct MsvcClassInfo
{
    DWORD unknown1;             ///< Flags? seen 0 and 4
    DWORD pTypeInfo;            ///< Offset to type information (const type_info *)
    DWORD unknown2;             ///< Usually zero
    DWORD unknown3;             ///< Usually 0xffffffff, seen 4
    DWORD unknown4;             ///< Only important if unknown3 != 0xffffffff
    DWORD size;                 ///< Size of object
    DWORD copyCtor;             ///< Offset to copy constructor
};

/*
 * Data structures used to implement catch handling and exception unwinding in MSVC
 */

// Forward references
struct MsvcUnwindEntry;
struct MsvcTryEntry;
struct MsvcCatchEntry;

/**
  * Top level catch handler in MSVC 
  *
  * The compiler-generated Win32 exception handler starts with a mov into eax of 
  * one of these structures.
  */
struct MsvcFrameHandler
{
    DWORD magic;                   ///< magic number (MSVC_MAGIC_NUMBER1)
    DWORD cUnwindEntry;            ///< count of unwind entries
    MsvcUnwindEntry *pUnwindEntry; ///< unwind entries (indexed by 'current level' in exception record)
    DWORD cTryEntry;               ///< count of try entries
    MsvcTryEntry *pTryEntry;       ///< try entries (search in order by current level)
};

/** Unwind entry in MSVC */
struct MsvcUnwindEntry
{
    DWORD prev;           ///< previous level for unwinding multiple variables
    PVOID unwindFunction; ///< function to unwind current variable
};

/** Try entry in MSVC */
struct MsvcTryEntry
{
    DWORD startIndex;   ///< Level at start of try block
    DWORD endIndex;     ///< Level at end of try block
    DWORD catchIndex;   ///< Level after catch statement(s)
    DWORD nCatchEntry;  ///< Number of catch statements
    MsvcCatchEntry *pCatchEntry;  ///< Pointer to catch entries
};

/** Data for an individual catch handler in MSVC */
struct MsvcCatchEntry
{
    DWORD flags;        ///< flags describing sort of catch
    DWORD rtti;         ///< pointer to RTTI information
    DWORD offset;       ///< stack offset (?)
    PVOID catchHandler; ///< start of catch block
};

#endif // MSVCEXCEPTIONS_H_