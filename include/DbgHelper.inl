#ifndef DBGHELPER_INL_
#define DBGHELPER_INL_

/*
    Provide helper functions for DbgHelp DLL.

    @author Roger Orr <rogero@howzatt.co.uk>

    Copyright &copy; 2003, 2021.
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

    $Revision: 2513 $
*/

// $Id: DbgHelper.inl 2513 2025-01-18 16:01:37Z roger $

#include <iostream>

#ifdef DBGHELP_6_1_APIS

/** Helper for streaming SymTagEnum values */
inline std::ostream &operator<<(std::ostream &os, enum SymTagEnum const value) {
#define DEF(X)                                                                 \
  { X, #X }

  static struct {
    int value;
    char const *name;
  } enumValues[] = {DEF(SymTagNull),
                    DEF(SymTagExe),
                    DEF(SymTagCompiland),
                    DEF(SymTagCompilandDetails),
                    DEF(SymTagCompilandEnv),
                    DEF(SymTagFunction),
                    DEF(SymTagBlock),
                    DEF(SymTagData),
                    DEF(SymTagAnnotation),
                    DEF(SymTagLabel),
                    DEF(SymTagPublicSymbol),
                    DEF(SymTagUDT),
                    DEF(SymTagEnum),
                    DEF(SymTagFunctionType),
                    DEF(SymTagPointerType),
                    DEF(SymTagArrayType),
                    DEF(SymTagBaseType),
                    DEF(SymTagTypedef),
                    DEF(SymTagBaseClass),
                    DEF(SymTagFriend),
                    DEF(SymTagFunctionArgType),
                    DEF(SymTagFuncDebugStart),
                    DEF(SymTagFuncDebugEnd),
                    DEF(SymTagUsingNamespace),
                    DEF(SymTagVTableShape),
                    DEF(SymTagVTable),
                    DEF(SymTagCustom),
                    DEF(SymTagThunk),
                    DEF(SymTagCustomType),
                    DEF(SymTagManagedType),
                    DEF(SymTagDimension),
                    DEF(SymTagCallSite),
                    DEF(SymTagInlineSite),
                    DEF(SymTagBaseInterface),
                    DEF(SymTagVectorType),
                    DEF(SymTagMatrixType),
                    DEF(SymTagHLSLType),
#if (_MSC_VER >= 1900)
                    DEF(SymTagCaller),
                    DEF(SymTagCallee),
                    DEF(SymTagExport),
                    DEF(SymTagHeapAllocationSite),
                    DEF(SymTagCoffGroup),
#endif // _MSC_VER
                    DEF(SymTagMax),
                    {0, nullptr}};

#undef DEF

  for (int idx = 0; enumValues[idx].name != nullptr; ++idx) {
    if (enumValues[idx].value == value) {
      return os << enumValues[idx].name;
    }
  }

  return os << "unknown (" << static_cast<int>(value) << ")";
}

#endif // DBGHELP_6_1_APIS

namespace or2 {

/** Constructor */
inline DbgHelper::DbgHelper() {}

/** Destructor */
inline DbgHelper::~DbgHelper() { Cleanup(); }

/** Initialise (only called once) */
inline BOOL DbgHelper::Initialise(HANDLE hProcess) {
  BOOL bRet = ::SymInitialize(hProcess, nullptr, false);
  if (bRet) {
    m_hProcess = hProcess;
  }
  return bRet;
}

/** Clean up */
inline BOOL DbgHelper::Cleanup() {
  BOOL bRet = false;
  if (m_hProcess) {
    bRet = ::SymCleanup(m_hProcess);
    m_hProcess = nullptr;
  }

  return bRet;
}

inline BOOL DbgHelper::GetModuleInfo64(DWORD64 dwAddr,
                                       PIMAGEHLP_MODULE64 ModuleInfo) const {
  return ::SymGetModuleInfo64(m_hProcess, dwAddr, ModuleInfo);
}

inline DWORD64 DbgHelper::GetModuleBase64(DWORD64 dwAddr) const {
  return ::SymGetModuleBase64(m_hProcess, dwAddr);
}

inline BOOL DbgHelper::LoadModule64(HANDLE hFile, PCSTR ImageName,
                                    PCSTR ModuleName, DWORD64 BaseOfDll,
                                    DWORD SizeOfDll) const {
  // Use const_cast to support older dbghelp.h header files which aren't as
  // const correct.
  return ::SymLoadModule64(m_hProcess, hFile, const_cast<PSTR>(ImageName),
                           const_cast<PSTR>(ModuleName), BaseOfDll,
                           SizeOfDll) != 0;
}

inline BOOL DbgHelper::UnloadModule64(DWORD64 BaseOfDll) {
  return SymUnloadModule64(m_hProcess, BaseOfDll);
}

inline BOOL DbgHelper::GetLineFromAddr64(DWORD64 dwAddr, PDWORD pdwDisplacement,
                                         PIMAGEHLP_LINE64 Line) const {
  return ::SymGetLineFromAddr64(m_hProcess, dwAddr, pdwDisplacement, Line);
}

inline HANDLE DbgHelper::GetProcess() const { return m_hProcess; }

#ifndef DBGHELP_6_1_APIS

inline BOOL DbgHelper::GetSymFromAddr64(DWORD64 dwAddr,
                                        PDWORD64 pdwDisplacement,
                                        PIMAGEHLP_SYMBOL64 Symbol) const {
  return ::SymGetSymFromAddr64(m_hProcess, dwAddr, pdwDisplacement, Symbol);
}

#else

// Version 6.1 APIs

inline FARPROC DbgHelper::GetProc(char const *name) {
  MEMORY_BASIC_INFORMATION mbi;
  if (VirtualQuery(&::SymInitialize, &mbi, sizeof(mbi))) {
    return ::GetProcAddress((HMODULE)mbi.AllocationBase, name);
  }
  return nullptr;
}

#define DYN_LOAD(SYMBOL)                                                       \
  static SYMBOL *pfn##SYMBOL = (SYMBOL *)GetProc(#SYMBOL);

/** Get symbol for specific address */
inline BOOL DbgHelper::SymFromAddr(DWORD64 Address, PDWORD64 Displacement,
                                   PSYMBOL_INFO Symbol) const {
  using SymFromAddr =
      BOOL WINAPI(IN HANDLE hProcess, IN DWORD64 Address,
                  OUT PDWORD64 Displacement, IN OUT PSYMBOL_INFO Symbol);

  DYN_LOAD(SymFromAddr);

  BOOL bRet = false;

  if (pfnSymFromAddr) {
    bRet = pfnSymFromAddr(m_hProcess, Address, Displacement, Symbol);
    // Displacement is incorrect for some PDB files (linker optimization?)
    if (bRet && Displacement && Symbol && Symbol->Address) {
      *Displacement = Address - Symbol->Address;
    }
  }

  return bRet;
}

/** Get symbol for name */
inline BOOL DbgHelper::SymFromName(PCSTR Name, PSYMBOL_INFO Symbol) const {
  using SymFromName = BOOL WINAPI(IN HANDLE hProcess, IN PCSTR Name,
                                  IN OUT PSYMBOL_INFO Symbol);

  DYN_LOAD(SymFromName);

  BOOL bRet = false;

  if (pfnSymFromName) {
    bRet = pfnSymFromName(m_hProcess, Name, Symbol);
  }

  return bRet;
}

/** Get type info from a type ID */
inline BOOL DbgHelper::GetTypeInfo(DWORD64 ModBase, ULONG TypeId,
                                   IMAGEHLP_SYMBOL_TYPE_INFO GetType,
                                   PVOID pInfo) const {
  using SymGetTypeInfo =
      BOOL WINAPI(IN HANDLE hProcess, IN DWORD64 ModBase, IN ULONG TypeId,
                  IN IMAGEHLP_SYMBOL_TYPE_INFO GetType, OUT PVOID pInfo);

  DYN_LOAD(SymGetTypeInfo);

  BOOL bRet = false;

  if (pfnSymGetTypeInfo) {
    bRet = pfnSymGetTypeInfo(m_hProcess, ModBase, TypeId, GetType, pInfo);
  }

  return bRet;
}

/** Get type info from a name. */
inline BOOL DbgHelper::GetTypeFromName(DWORD64 ModBase, PCTSTR Name,
                                       PSYMBOL_INFO Symbol) const {
  using SymGetTypeFromName =
      BOOL WINAPI(IN HANDLE hProcess, IN DWORD64 ModBase, IN PCTSTR Name,
                  IN PSYMBOL_INFO Symbol);

  DYN_LOAD(SymGetTypeFromName);

  BOOL bRet = false;

  if (pfnSymGetTypeFromName) {
    bRet = pfnSymGetTypeFromName(m_hProcess, ModBase, Name, Symbol);
  }

  return bRet;
}

/** The SetContext function sets context information used by the SymEnumSymbols
 * function. */
inline BOOL DbgHelper::SetContext(PIMAGEHLP_STACK_FRAME StackFrame,
                                  PIMAGEHLP_CONTEXT Context) const {
  using SymSetContext =
      BOOL WINAPI(HANDLE hProcess, PIMAGEHLP_STACK_FRAME StackFrame,
                  PIMAGEHLP_CONTEXT Context);

  DYN_LOAD(SymSetContext);

  BOOL bRet = false;

  if (pfnSymSetContext) {
    bRet = pfnSymSetContext(m_hProcess, StackFrame, Context);
  }

  return bRet;
}

/** The EnumSymbols function enumerates all symbols in a process. */
inline BOOL
DbgHelper::EnumSymbols(ULONG64 BaseOfDll, PCSTR Mask,
                       PSYM_ENUMERATESYMBOLS_CALLBACK EnumSymbolsCallback,
                       PVOID UserContext) const {
  using SymEnumSymbols = BOOL WINAPI(
      HANDLE hProcess, ULONG64 BaseOfDll, PCSTR Mask,
      PSYM_ENUMERATESYMBOLS_CALLBACK EnumSymbolsCallback, PVOID UserContext);

  DYN_LOAD(SymEnumSymbols);

  BOOL bRet = false;

  if (pfnSymEnumSymbols) {
    bRet = pfnSymEnumSymbols(m_hProcess, BaseOfDll, Mask, EnumSymbolsCallback,
                             UserContext);
  }

  return bRet;
}

/** The EnumTypes function enumerates all types in a process. */
inline BOOL
DbgHelper::EnumTypes(ULONG64 BaseOfDll,
                     PSYM_ENUMERATESYMBOLS_CALLBACK EnumSymbolsCallback,
                     PVOID UserContext) const {
  using SymEnumTypes = BOOL WINAPI(
      HANDLE hProcess, ULONG64 BaseOfDll,
      PSYM_ENUMERATESYMBOLS_CALLBACK EnumSymbolsCallback, PVOID UserContext);

  DYN_LOAD(SymEnumTypes);

  BOOL bRet = false;

  if (pfnSymEnumTypes) {
    bRet = pfnSymEnumTypes(m_hProcess, BaseOfDll, EnumSymbolsCallback,
                           UserContext);
  }

  return bRet;
}

/* Write a mini-dump */
inline BOOL
DbgHelper::WriteMiniDump(DWORD processId, HANDLE hFile, MINIDUMP_TYPE DumpType,
                         PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
                         PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
                         PMINIDUMP_CALLBACK_INFORMATION CallbackParam) {
  using MiniDumpWriteDump = BOOL WINAPI(
      IN HANDLE hProcess, IN DWORD ProcessId, IN HANDLE hFile,
      IN MINIDUMP_TYPE DumpType,
      IN CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
      OPTIONAL IN CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
      OPTIONAL IN CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam OPTIONAL);

  DYN_LOAD(MiniDumpWriteDump);

  BOOL bRet = FALSE;

  if (pfnMiniDumpWriteDump) {
    bRet = pfnMiniDumpWriteDump(m_hProcess, processId, hFile, DumpType,
                                ExceptionParam, UserStreamParam, CallbackParam);
  }
  return bRet;
}

/** Test whether EnumSymbols API is available */
inline BOOL DbgHelper::IsEnumSymbolsAvailable() const {
  // Run a test which won't produce any callbacks
  return EnumSymbols(0, nullptr, nullptr, nullptr);
}

#endif // DBGHELP_6_1_APIS

#ifdef DBGHELP_6_2_APIS
/** Number of inline frames at the given address */
inline DWORD DbgHelper::AddrIncludeInlineTrace(DWORD64 address) const {
  using SymAddrIncludeInlineTrace =
      DWORD WINAPI(HANDLE hProcess, DWORD64 Address);

  DYN_LOAD(SymAddrIncludeInlineTrace);

  DWORD result = 0;
  if (pfnSymAddrIncludeInlineTrace) {
    result = pfnSymAddrIncludeInlineTrace(m_hProcess, address);
  }
  return result;
}

/** Get inline context */
inline BOOL DbgHelper::QueryInlineTrace(DWORD64 StartAddress,
                                        DWORD StartContext,
                                        DWORD64 StartRetAddress,
                                        DWORD64 CurAddress, LPDWORD CurContext,
                                        LPDWORD CurFrameIndex) const {
  using SymQueryInlineTrace =
      BOOL WINAPI(HANDLE hProcess, DWORD64 StartAddress, DWORD StartContext,
                  DWORD64 StartRetAddress, DWORD64 CurAddress,
                  LPDWORD CurContext, LPDWORD CurFrameIndex);

  DYN_LOAD(SymQueryInlineTrace);

  return pfnSymQueryInlineTrace &&
         pfnSymQueryInlineTrace(m_hProcess, StartAddress, StartContext,
                                StartRetAddress, CurAddress, CurContext,
                                CurFrameIndex);
}

/** Get Symbol from inline context */
inline BOOL DbgHelper::FromInlineContext(DWORD64 Address, DWORD InlineContext,
                                         PDWORD64 pDisplacement,
                                         PSYMBOL_INFO Symbol) const {
  using SymFromInlineContext =
      BOOL WINAPI(HANDLE hProcess, DWORD64 Address, DWORD InlineContext,
                  PDWORD64 pDisplacement, PSYMBOL_INFO Symbol);

  DYN_LOAD(SymFromInlineContext);

  return pfnSymFromInlineContext &&
         pfnSymFromInlineContext(m_hProcess, Address, InlineContext,
                                 pDisplacement, Symbol);
}

/** Get Line from inline context */
inline BOOL DbgHelper::GetLineFromInlineContext(DWORD64 Address,
                                                DWORD InlineContext,
                                                DWORD64 ModuleBaseAddress,
                                                PDWORD pDisplacement,
                                                PIMAGEHLP_LINE64 Line64) const {
  using SymGetLineFromInlineContext = BOOL WINAPI(
      HANDLE hProcess, DWORD64 Address, DWORD InlineContext,
      DWORD64 ModuleBaseAddress, PDWORD pDisplacement, PIMAGEHLP_LINE64 Line64);

  DYN_LOAD(SymGetLineFromInlineContext);

  return pfnSymGetLineFromInlineContext &&
         pfnSymGetLineFromInlineContext(m_hProcess, Address, InlineContext,
                                        ModuleBaseAddress, pDisplacement,
                                        Line64);
}

/** Enumerate inline symbols at a given address and inline context. */
inline BOOL
DbgHelper::EnumSymbolsEx(ULONG64 BaseOfDll, PCSTR Mask,
                         PSYM_ENUMERATESYMBOLS_CALLBACK EnumSymbolsCallback,
                         PVOID UserContext, DWORD Options) const {
  using SymEnumSymbolsEx =
      BOOL WINAPI(HANDLE hProcess, ULONG64 BaseOfDll, PCSTR Mask,
                  PSYM_ENUMERATESYMBOLS_CALLBACK EnumSymbolsCallback,
                  PVOID UserContext, DWORD Options);

  DYN_LOAD(SymEnumSymbolsEx);

  return pfnSymEnumSymbolsEx &&
         pfnSymEnumSymbolsEx(m_hProcess, BaseOfDll, Mask, EnumSymbolsCallback,
                             UserContext, Options);
}

inline BOOL DbgHelper::SetScopeFromInlineContext(DWORD64 Address,
                                                 DWORD InlineContext) const {
  using SymSetScopeFromInlineContext =
      BOOL WINAPI(HANDLE hProcess, DWORD64 Address, DWORD InlineContext);

  DYN_LOAD(SymSetScopeFromInlineContext);

  return pfnSymSetScopeFromInlineContext &&
         pfnSymSetScopeFromInlineContext(m_hProcess, Address, InlineContext);
}

#endif // DBGHELP_6_2_APIS

#undef DYN_LOAD

} // namespace or2

#endif // DBGHELPER_INL_
