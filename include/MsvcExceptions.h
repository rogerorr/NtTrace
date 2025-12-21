#ifndef MSVCEXCEPTIONS_H_
#define MSVCEXCEPTIONS_H_

/**@file

  Data for Microsoft Visual Studio exceptions

  @author Roger Orr mailto:rogero@howzatt.co.uk
  Bug reports, comments, and suggestions are always welcome.

  Copyright &copy; 2004 under the MIT license:

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

  $Revision: 2998 $
*/

// $Id: MsvcExceptions.h 2998 2025-12-21 15:54:22Z roger $

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

/* Value of ExceptionCode for an MSVC C++ exception ("msc") */
static DWORD const MSVC_EXCEPTION = 0xe06d7363;

/* Value of ExceptionCode for an MSVC notification exception (used for
 * 'SetThreadName') */
static DWORD const MSVC_NOTIFICATION = 0x406d1388;

/* Value of ExceptionCode for an MS CLR exception ("COM") */
static DWORD const CLR_EXCEPTION = 0xe0434f4d;

/* Value of ExceptionCode for an MS CLR 4 exception ("CCR") */
static DWORD const CLR_EXCEPTION_V4 = 0xe0434352;

/* Value of ExceptionCode for an MS CLR notification */
static DWORD const CLR_NOTIFICATION = 0x04242420;

/** Value of ExceptionInformation[0] for an MSVC C++ exception */
static DWORD const MSVC_MAGIC_NUMBER1 =
    0x19930520; // ' Name taken from EHSUP.INC (!)

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
struct MsvcThrow {
  DWORD unknown1;     ///< Use is not known
  DWORD pDestructor;  ///< Offset to destructor, or null
  DWORD unknown2;     ///< Use is not known
  DWORD pClassHeader; ///< Offset to CTA2PAD (MsvcClassHeader)
};

/** Class header for a thrown object in MSVC */
struct MsvcClassHeader {
  DWORD cInfo;   ///< Count of Info structures
  DWORD Info[1]; ///< Actually variable size structure (MsvcClassInfo)
};

/** Information pointed to by the class header for a thrown object in MSVC */
struct MsvcClassInfo {
  DWORD unknown1;  ///< Flags? seen 0 and 4
  DWORD pTypeInfo; ///< Offset to type information (const type_info *)
  DWORD unknown2;  ///< Usually zero
  DWORD unknown3;  ///< Usually 0xffffffff, seen 4
  DWORD unknown4;  ///< Only important if unknown3 != 0xffffffff
  DWORD size;      ///< Size of object
  DWORD copyCtor;  ///< Offset to copy constructor
};

/*
 * Data structures used to implement catch handling and exception unwinding in
 * MSVC
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
struct MsvcFrameHandler {
  DWORD magic;                   ///< magic number (MSVC_MAGIC_NUMBER1)
  DWORD cUnwindEntry;            ///< count of unwind entries
  MsvcUnwindEntry *pUnwindEntry; ///< unwind entries (indexed by 'current level'
                                 ///< in exception record)
  DWORD cTryEntry;               ///< count of try entries
  MsvcTryEntry *pTryEntry; ///< try entries (search in order by current level)
};

/** Unwind entry in MSVC */
struct MsvcUnwindEntry {
  DWORD prev;           ///< previous level for unwinding multiple variables
  PVOID unwindFunction; ///< function to unwind current variable
};

/** Try entry in MSVC */
struct MsvcTryEntry {
  DWORD startIndex;            ///< Level at start of try block
  DWORD endIndex;              ///< Level at end of try block
  DWORD catchIndex;            ///< Level after catch statement(s)
  DWORD nCatchEntry;           ///< Number of catch statements
  MsvcCatchEntry *pCatchEntry; ///< Pointer to catch entries
};

/** Data for an individual catch handler in MSVC */
struct MsvcCatchEntry {
  DWORD flags;        ///< flags describing sort of catch
  DWORD rtti;         ///< pointer to RTTI information
  DWORD offset;       ///< stack offset (?)
  PVOID catchHandler; ///< start of catch block
};

#endif // MSVCEXCEPTIONS_H_