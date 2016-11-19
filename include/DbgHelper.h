#ifndef DBGHELPER_H_
#define DBGHELPER_H_

/**
@file
    Helper for wrapping calls to the Microsoft DbgHelp.dll.

    @author Roger Orr <rogero@howzatt.demon.co.uk>

    Copyright &copy; 2003.
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

    $Revision: 1657 $
*/

// $Id: DbgHelper.h 1657 2016-11-18 20:45:19Z Roger $

#include <windows.h>

#include "SymTagEnum.h"

#if(WINVER < 0x0500)
// Check for original VC6 SDK
#error "Windows 2000 or higher platform SDK required"
#endif

#pragma warning(push)
#pragma warning(disable: 4091) // 'typedef ': ignored on left of '' when no variable is declared
#include <DbgHelp.h>
#pragma warning(pop)

#pragma comment( lib, "dbgHelp.lib" )

#include <iosfwd>

#ifdef SSRVOPT_TRACE // identifies newer DbgHelp.h (!)
#define DBGHELP_6_1_APIS
#endif

#ifdef DBGHELP_6_1_APIS

/** Helper for streaming SymTagEnum values */
std::ostream& operator<<( std::ostream& os, enum SymTagEnum const value );

#endif // DBGHELP_6_1_APIS

namespace or2
{

/** Class to assist with DbgHelp API */
class DbgHelper
{
public:
   /** Construct a helper object. */
   DbgHelper();

   /** Destroy and clean up. */
   virtual ~DbgHelper();

   /** Initalise the object to access the specified process. */
   BOOL Initialise( HANDLE hProcess );

   /** Clean up data structures for the current process. */
   BOOL Cleanup();

   /** Get module information for the input address. */
   BOOL GetModuleInfo64( DWORD64 dwAddr, PIMAGEHLP_MODULE64 ModuleInfo  ) const;

   /** Load the module base information for the input address. */
   DWORD64 GetModuleBase64( DWORD64 dwAddr ) const;

   /** Load information for a module. */
   BOOL LoadModule64( HANDLE hFile, PCSTR ImageName, PCSTR ModuleName, DWORD64 BaseOfDll, DWORD SizeOfDll ) const;

   /** Unload information for a module */
   BOOL UnloadModule64( DWORD64 BaseOfDll );

   /** Get line number information for the input address. */
   BOOL GetLineFromAddr64( DWORD64 dwAddr, PDWORD pdwDisplacement, PIMAGEHLP_LINE64 Line ) const;

   /** Get the process currently being debugged. */
   HANDLE GetProcess() const;

#ifndef DBGHELP_6_1_APIS

   /** Get symbolic information for the input address. */
   BOOL GetSymFromAddr64( DWORD dwAddr, PDWORD64 pdwDisplacement, PIMAGEHLP_SYMBOL64 Symbol ) const;

#else

// Version 6.1 APIs

   /** Get symbol for specific address. */
   BOOL SymFromAddr( DWORD64 Address, PDWORD64 Displacement, PSYMBOL_INFO Symbol ) const;

   /** Get symbol for a name */
   BOOL SymFromName( PCSTR Name, PSYMBOL_INFO Symbol ) const;

   /** Get type info from a type ID. */
   BOOL GetTypeInfo( DWORD64 ModBase, ULONG TypeId, IMAGEHLP_SYMBOL_TYPE_INFO GetType, PVOID pInfo ) const;

   /** Get type info from a name. */
   BOOL GetTypeFromName( ULONG64 BaseOfDll, PCTSTR Name, PSYMBOL_INFO Symbol ) const;

   /** The SetContext function sets context information used by the SymEnumSymbols function. */
   BOOL SetContext( PIMAGEHLP_STACK_FRAME StackFrame, PIMAGEHLP_CONTEXT Context ) const;

   /** The EnumSymbols function enumerates all symbols in a process. */
   BOOL EnumSymbols( ULONG64 BaseOfDll, PCSTR Mask, PSYM_ENUMERATESYMBOLS_CALLBACK EnumSymbolsCallback, PVOID UserContext ) const;

   /** The EnumTypes function enumerates all types in a process. */
   BOOL EnumTypes( ULONG64 BaseOfDll, PSYM_ENUMERATESYMBOLS_CALLBACK EnumSymbolsCallback, PVOID UserContext ) const;

   /** Test whether EnumSymbols API is available. */
   BOOL IsEnumSymbolsAvailable() const;

   /** Write a mini-dump */
   BOOL WriteMiniDump( DWORD processId, HANDLE hFile, MINIDUMP_TYPE DumpType,
       PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam, PMINIDUMP_CALLBACK_INFORMATION CallbackParam );

#endif // DBGHELP_6_1_APIS

private:
   // suppress copy and assign
   DbgHelper( DbgHelper const & ); 
   DbgHelper& operator=( DbgHelper const & );

   HANDLE m_hProcess; // Current process being debugged

#ifdef DBGHELP_6_1_APIS
   // Helper function for resolving addresses dynamically
   static FARPROC GetProc(char const * name);
#endif // DBGHELP_6_1_APIS
};

/** Helper struct to zero initialise POD classes */
template <typename T>
struct DbgInit : public T
{
   /** Constructor sets the base class to zeroes */
   DbgInit()
   {
      ::memset( static_cast<void*>(this), 0, sizeof( T ) );
      this->SizeOfStruct = sizeof( T );
   }
};

}

// inline implementation
#include "DbgHelper.inl"

#endif // DBGHELPER_H_
