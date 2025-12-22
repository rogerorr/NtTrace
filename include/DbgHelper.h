#ifndef DBGHELPER_H_
#define DBGHELPER_H_

/**
@file
  Helper for wrapping calls to the Microsoft DbgHelp.dll.

  @author Roger Orr mailto:rogero@howzatt.co.uk
  Bug reports, comments, and suggestions are always welcome..

  Copyright &copy; 2003-2021 under the MIT license:

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

  $Revision: 3020 $
*/

// $Id: DbgHelper.h 3020 2025-12-22 17:45:15Z roger $

#include <cvconst.h> //DIA SDK
#include <windows.h>

#if (WINVER < 0x0500)
// Check for original VC6 SDK
#error "Windows 2000 or higher platform SDK required"
#endif

#pragma warning(push)
#pragma warning(disable : 4091) // 'typedef ': ignored on left of '' when no
                                // variable is declared
#include <DbgHelp.h>
#pragma warning(pop)

#pragma comment(lib, "dbgHelp.lib")

#include <iosfwd>

#ifdef SSRVOPT_TRACE // identifies newer DbgHelp.h (!)
#define DBGHELP_6_1_APIS
#endif

#ifdef INLINE_FRAME_CONTEXT_INIT // identifies addition of Inline frame handling
#define DBGHELP_6_2_APIS
#endif

#ifdef DBGHELP_6_1_APIS

/** Helper for streaming SymTagEnum values */
std::ostream &operator<<(std::ostream &os, enum SymTagEnum const value);

#endif // DBGHELP_6_1_APIS

namespace or2 {

/** Class to assist with DbgHelp API */
class DbgHelper {
public:
  /** Construct a helper object. */
  DbgHelper();

  /** Do not copy */
  DbgHelper(DbgHelper const &) = delete;

  /** Do not assign */
  DbgHelper &operator=(DbgHelper const &) = delete;

  /** Destroy and clean up. */
  virtual ~DbgHelper();

  /** Initalise the object to access the specified process. */
  BOOL Initialise(HANDLE hProcess);

  /** Clean up data structures for the current process. */
  BOOL Cleanup();

  /** Get module information for the input address. */
  BOOL GetModuleInfo64(DWORD64 dwAddr, PIMAGEHLP_MODULE64 ModuleInfo) const;

  /** Load the module base information for the input address. */
  DWORD64 GetModuleBase64(DWORD64 dwAddr) const;

  /** Load information for a module. */
  BOOL LoadModule64(HANDLE hFile, PCSTR ImageName, PCSTR ModuleName,
                    DWORD64 BaseOfDll, DWORD SizeOfDll) const;

  /** Unload information for a module */
  BOOL UnloadModule64(DWORD64 BaseOfDll);

  /** Get line number information for the input address. */
  BOOL GetLineFromAddr64(DWORD64 dwAddr, PDWORD pdwDisplacement,
                         PIMAGEHLP_LINE64 Line) const;

  /** Get the process currently being debugged. */
  HANDLE GetProcess() const;

#ifndef DBGHELP_6_1_APIS

  /** Get symbolic information for the input address. */
  BOOL GetSymFromAddr64(DWORD64 dwAddr, PDWORD64 pdwDisplacement,
                        PIMAGEHLP_SYMBOL64 Symbol) const;

#else

  // Version 6.1 APIs

  /** Get symbol for specific address. */
  BOOL SymFromAddr(DWORD64 Address, PDWORD64 Displacement,
                   PSYMBOL_INFO Symbol) const;

  /** Get symbol for a name */
  BOOL SymFromName(PCSTR Name, PSYMBOL_INFO Symbol) const;

  /** Get type info from a type ID. */
  BOOL GetTypeInfo(DWORD64 ModBase, ULONG TypeId,
                   IMAGEHLP_SYMBOL_TYPE_INFO GetType, PVOID pInfo) const;

  /** Get type info from a name. */
  BOOL GetTypeFromName(ULONG64 BaseOfDll, PCTSTR Name,
                       PSYMBOL_INFO Symbol) const;

  /** The SetContext function sets context information used by the
   * SymEnumSymbols function. */
  BOOL SetContext(PIMAGEHLP_STACK_FRAME StackFrame,
                  PIMAGEHLP_CONTEXT Context) const;

  /** The EnumSymbols function enumerates all symbols in a process. */
  BOOL EnumSymbols(ULONG64 BaseOfDll, PCSTR Mask,
                   PSYM_ENUMERATESYMBOLS_CALLBACK EnumSymbolsCallback,
                   PVOID UserContext) const;

  /** The EnumTypes function enumerates all types in a process. */
  BOOL EnumTypes(ULONG64 BaseOfDll,
                 PSYM_ENUMERATESYMBOLS_CALLBACK EnumSymbolsCallback,
                 PVOID UserContext) const;

  /** Test whether EnumSymbols API is available. */
  BOOL IsEnumSymbolsAvailable() const;

  /** Write a mini-dump */
  BOOL WriteMiniDump(DWORD processId, HANDLE hFile, MINIDUMP_TYPE DumpType,
                     PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
                     PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
                     PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

#endif // DBGHELP_6_1_APIS

#ifdef DBGHELP_6_2_APIS
  /** Number of inline frames at the given address */
  DWORD AddrIncludeInlineTrace(DWORD64 address) const;

  /** Get inline context */
  BOOL QueryInlineTrace(DWORD64 StartAddress, DWORD StartContext,
                        DWORD64 StartRetAddress, DWORD64 CurAddress,
                        LPDWORD CurContext, LPDWORD CurFrameIndex) const;

  /** Get Symbol from inline context */
  BOOL FromInlineContext(DWORD64 Address, DWORD InlineContext,
                         PDWORD64 pDisplacement, PSYMBOL_INFO Symbol) const;

  /** Get Line from inline context */
  BOOL GetLineFromInlineContext(DWORD64 Address, DWORD InlineContext,
                                DWORD64 ModuleBaseAddress, PDWORD pDisplacement,
                                PIMAGEHLP_LINE64 Line64) const;

  /** Sets the local scope to the symbol that matches the specified address and
   * inline context. */
  BOOL SetScopeFromInlineContext(DWORD64 Address, DWORD InlineContext) const;

  /** The EnumSymbolsEx function enumerates all symbols in a process. */
  BOOL EnumSymbolsEx(ULONG64 BaseOfDll, PCSTR Mask,
                     PSYM_ENUMERATESYMBOLS_CALLBACK EnumSymbolsCallback,
                     PVOID UserContext, DWORD Options) const;

#endif // DBGHELP_6_2_APIS

private:
  HANDLE m_hProcess{}; // Current process being debugged

#ifdef DBGHELP_6_1_APIS
  // Helper function for resolving addresses dynamically
  static FARPROC GetProc(char const *name);
#endif // DBGHELP_6_1_APIS
};

/** Helper struct to zero initialise POD classes */
template <typename T> struct DbgInit : public T {
  /** Constructor sets the base class to zeroes */
  DbgInit() {
    ::memset(static_cast<void *>(this), 0, sizeof(T));
    this->SizeOfStruct = sizeof(T);
  }
};

} // namespace or2

// inline implementation
#include "DbgHelper.inl"

#endif // DBGHELPER_H_
