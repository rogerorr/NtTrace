/*
NAME
  SymbolEngine

DESCRIPTION
  Additional symbol engine functionality

AUTHOR
  Roger Orr mailto:rogero@howzatt.co.uk
  Bug reports, comments, and suggestions are always welcome.

COPYRIGHT
  Copyright (C) 2003,2021 under the MIT license:

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
*/

static char const szRCSID[] =
    "$Id: SymbolEngine.cpp 3010 2025-12-21 18:00:47Z roger $";

#ifdef _MSC_VER
#pragma warning(disable : 4511 4512) // copy constructor/assignment operator
                                     // could not be generated
#endif                               // _MSC_VER

#include "SymbolEngine.h"

// clang-format off
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
// clang-format on
#include <comutil.h>
#include <psapi.h>
#pragma comment(lib, "oleaut32.lib")
#pragma comment(lib, "comsupp.lib")

// stl
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <typeinfo>

#include "../include/MsvcExceptions.h"
#include "../include/ReadPartialMemory.h"
#include "../include/StrFromWchar.h"

#include "GetModuleBase.h"

#pragma comment(lib, "psapi")

// helper function
namespace or2 {
namespace {
// fix for problem with resource leak in symsrv
void fixSymSrv();

// Show function/SEH parameters
template <typename WORDSIZE>
void addParams(std::ostream &os, WORDSIZE *pParams, size_t maxParams) {
  for (size_t i = 0; i < maxParams; ++i) {
    WORDSIZE const param = pParams[i];
    if ((-10 < (long)param) && ((long)param < 10))
      os << " " << (long)param;
    else
      os << " " << (PVOID)(ULONG_PTR)param;
  }
}

struct RegInfo {
  RegInfo(std::string name, DWORD64 value)
      : name(std::move(name)), value(value) {}
  std::string name;
  DWORD64 value;
};

RegInfo getRegInfo(ULONG reg, const CONTEXT &context);

#ifdef DBGHELP_6_1_APIS

struct EngineCallBack {
  EngineCallBack(SymbolEngine const &eng, SymbolEngine::EnumLocalCallBack &cb)
      : eng_(eng), cb_(cb) {}

  static BOOL CALLBACK enumSymbolsProc(PSYMBOL_INFO pSymInfo,
                                       ULONG /*SymbolSize*/,
                                       PVOID UserContext) {
    EngineCallBack &thisCb = *(EngineCallBack *)UserContext;

    return thisCb.cb_(thisCb.eng_, pSymInfo);
  }

  SymbolEngine const &eng_;
  SymbolEngine::EnumLocalCallBack &cb_;
};

struct VariableCallBack : public SymbolEngine::EnumLocalCallBack {
  VariableCallBack(std::ostream &opf, DWORD64 frameOffset,
                   const CONTEXT &context)
      : opf(opf), frameOffset(frameOffset), context(context) {}

  bool operator()(SymbolEngine const &eng, PSYMBOL_INFO pSymInfo) override {
    std::string name(pSymInfo->Name, pSymInfo->NameLen);
    eng.decorateName(name, pSymInfo->ModBase, pSymInfo->TypeIndex);
    if (!(pSymInfo->Flags & SYMFLAG_LOCAL)) {
      // Ignore anything not a local variable
    } else if (pSymInfo->Flags & SYMFLAG_NULL) {
      // Ignore 'NULL' objects
    } else if ((pSymInfo->Flags & SYMFLAG_REGREL) ||
               (pSymInfo->Flags & SYMFLAG_FRAMEREL)) {
      opf << "  " << name;
      showValue(name, eng, pSymInfo);
      if (pSymInfo->Flags & SYMFLAG_PARAMETER) {
        opf << " (parameter)";
      }
      opf << std::endl;
    } else if (pSymInfo->Flags & SYMFLAG_REGISTER) {
      opf << "  " << name;
      const RegInfo reg_info = getRegInfo(pSymInfo->Register, context);
      if (reg_info.name.empty()) {
        opf << " (register '" << pSymInfo->Register << "\')";
      } else {
        opf << " (" << reg_info.name << ") = 0x" << std::hex << reg_info.value
            << std::dec;
      }
      if (pSymInfo->Flags & SYMFLAG_PARAMETER) {
        opf << " (parameter)";
      }
      opf << std::endl;
    } else {
      opf << "  " << name << " Flags: " << std::hex << pSymInfo->Flags
          << std::dec << std::endl;
    }

    return true;
  }

  // Show the value of a register-relative value
  void showValue(const std::string &name, SymbolEngine const &eng,
                 PSYMBOL_INFO pSymInfo) {
    const RegInfo reg_info = (pSymInfo->Flags & SYMFLAG_REGREL)
                                 ? getRegInfo(pSymInfo->Register, context)
                                 : RegInfo("frame", frameOffset);
    if (reg_info.name.empty()) {
      opf << " [register '" << pSymInfo->Register << "']";
    } else {
      opf << " [" << reg_info.name;
      if (pSymInfo->Address > 0x7fffffff)
        opf << "-" << std::hex << -(int)pSymInfo->Address << std::dec;
      else
        opf << "+" << std::hex << (int)pSymInfo->Address << std::dec;
      opf << "]";

      if (pSymInfo->Size == sizeof(char)) {
        unsigned char data;
        eng.ReadMemory((PVOID)(reg_info.value + pSymInfo->Address), &data,
                       sizeof(data));
        if (isprint(data))
          opf << " = '" << data << '\'';
        else
          opf << " = " << (int)data;
      } else if (pSymInfo->Size == sizeof(short)) {
        unsigned short data;
        eng.ReadMemory((PVOID)(reg_info.value + pSymInfo->Address), &data,
                       sizeof(data));
        opf << " = " << data;
      } else if ((pSymInfo->Size == sizeof(int)) || (pSymInfo->Size == 0)) {
        unsigned int data;
        eng.ReadMemory((PVOID)(reg_info.value + pSymInfo->Address), &data,
                       sizeof(data));
        opf << " = 0x" << std::hex << data << std::dec;
      } else if ((pSymInfo->Size == 8) && (name.compare(0, 6, "double") == 0)) {
        double data;
        eng.ReadMemory((PVOID)(reg_info.value + pSymInfo->Address), &data,
                       sizeof(data));
        opf << " = " << data;
      } else if (pSymInfo->Size == 8) {
        LONGLONG data;
        eng.ReadMemory((PVOID)(reg_info.value + pSymInfo->Address), &data,
                       sizeof(data));
        opf << " = 0x" << std::hex << data << std::dec;
      }
    }
  }

  std::ostream &opf;
  DWORD64 frameOffset;
  const CONTEXT &context;
};

#endif // DBGHELP_6_1_APIS

void showVariablesAt(std::ostream &os, DWORD64 codeOffset, DWORD64 frameOffset,
                     const CONTEXT &context, SymbolEngine const &eng);
void showInlineVariablesAt(std::ostream &os, DWORD64 codeOffset,
                           DWORD64 frameOffset, const CONTEXT &context,
                           DWORD inline_context, SymbolEngine const &eng);

//////////////////////////////////////////////////////////
// Helper function: getBaseType maps PDB type + length to C++ name
std::string getBaseType(DWORD baseType, ULONG64 length);

#ifdef _M_X64
// Helper function to delay load Wow64GetThreadContext or emulate on W2K3
BOOL getWow64ThreadContext(HANDLE hProcess, HANDLE hThread,
                           CONTEXT const &context, WOW64_CONTEXT *pWowContext);
#endif // _M_X64

// Wrapper for WideCharToMultiByte
size_t Utf16ToMbs(char *mb_str, size_t mb_size, const wchar_t *wc_str,
                  size_t wc_len) {
  return WideCharToMultiByte(CP_UTF8, 0, wc_str, static_cast<int>(wc_len),
                             mb_str, static_cast<int>(mb_size), nullptr,
                             nullptr);
}

} // namespace

/////////////////////////////////////////////////////////////////////////////////////
/** Implementation class */
struct SymbolEngine::Impl {
  std::map<DWORD64, std::string> addressMap;
  std::map<std::pair<DWORD64, DWORD>, std::string> inlineMap;
};

/////////////////////////////////////////////////////////////////////////////////////
#pragma warning(push)
#pragma warning(disable : 4996)
SymbolEngine::SymbolEngine(HANDLE hProcess) : pImpl_(new Impl) {
  static bool inited = false;
  if (!inited) {
    DWORD dwOpts = SymGetOptions();
    dwOpts |= SYMOPT_LOAD_LINES | SYMOPT_OMAP_FIND_NEAREST;
    char const *pOption = ::getenv("OR2_SYMOPT");
    if (pOption) {
      int extraOptions = 0;
      if (sscanf(pOption, "%x", &extraOptions) == 1) {
        dwOpts |= extraOptions;
      }
    }
    SymSetOptions(dwOpts);
    inited = true;
  }

  Initialise(hProcess);
}
#pragma warning(pop)

/////////////////////////////////////////////////////////////////////////////////////
// Destroy wrapper
SymbolEngine::~SymbolEngine() {
  fixSymSrv();
  delete pImpl_;
}

/////////////////////////////////////////////////////////////////////////////////////
// true to show line numbers if possible
void SymbolEngine::setLines(bool value) { showLines_ = value; }

bool SymbolEngine::getLines() const { return showLines_; }

// true to show parameters
void SymbolEngine::setParams(bool value) { showParams_ = value; }

bool SymbolEngine::getParams() const { return showParams_; }

// true to show variables
void SymbolEngine::setVariables(bool value) { showVariables_ = value; }

bool SymbolEngine::getVariables() const { return showVariables_; }

// set stack depth for walkbacks
void SymbolEngine::setMaxDepth(int value) { maxStackDepth_ = value; }

int SymbolEngine::getMaxDepth() const { return maxStackDepth_; }

// set skip count for stack walkbacks
void SymbolEngine::setSkipCount(int value) { skipCount_ = value; }

int SymbolEngine::getSkipCount() const { return skipCount_; }

// set seh stack depth walkbacks
void SymbolEngine::setSehDepth(int value) { maxSehDepth_ = value; }

int SymbolEngine::getSehDepth() const { return maxSehDepth_; }

/////////////////////////////////////////////////////////////////////////////////////
DWORD64 SymbolEngine::GetModuleBase(DWORD64 dwAddress) const {
  return ::GetModuleBase(GetProcess(), dwAddress);
}

/////////////////////////////////////////////////////////////////////////////////////
bool SymbolEngine::printAddress(DWORD64 address, std::ostream &os) const {
  bool cacheSymbol(true);

  // Despite having GetModuleBase in the call to StackWalk it needs help for the
  // addresses
  GetModuleBase(address);

  ///////////////////////////////
  // Log the module + offset
  MEMORY_BASIC_INFORMATION mbInfo;
  if (::VirtualQueryEx(GetProcess(), (PVOID)address, &mbInfo, sizeof mbInfo) &&
      ((mbInfo.State & MEM_FREE) == 0) && ((mbInfo.Type & MEM_IMAGE) != 0)) {
    std::ostringstream str;
    HMODULE const hmod = (HMODULE)mbInfo.AllocationBase;

    const std::string filename = GetModuleFileNameWrapper(GetProcess(), hmod);
    if (filename.empty()) {
      cacheSymbol = false;
      str << hmod;
    } else
      str << filename.substr(filename.find_last_of('\\') + 1);
    str << " + 0x" << std::hex << (address - (ULONG_PTR)mbInfo.AllocationBase)
        << std::dec;

    os << std::setw(30) << std::left << str.str().c_str()
       << std::right; // c_str() fixes VC6 bug with setw
  } else {
    os << (PVOID)address;
    return false;
  }

  ///////////////////////////////
  // Log the symbol name

  // The largest (undecorated) symbol that the MS code generators can handle is
  // 256. I can't believe it will increase more than fourfold when
  // undecorated...
#ifdef DBGHELP_6_1_APIS
  struct {
    SYMBOL_INFO symInfo;
    char name[4 * 256];
  } SymInfo = {{sizeof(SymInfo.symInfo)}, ""};

  PSYMBOL_INFO pSym = &SymInfo.symInfo;
  pSym->MaxNameLen = sizeof(SymInfo.name);

  DWORD64 dwDisplacement64(0);
  if (SymFromAddr(address, &dwDisplacement64, pSym))
#else
  struct {
    IMAGEHLP_SYMBOL64 symInfo;
    char name[4 * 256];
  } SymInfo = {{sizeof(SymInfo.symInfo)}, ""};

  PIMAGEHLP_SYMBOL64 pSym = &SymInfo.symInfo;
  pSym->MaxNameLength = sizeof(SymInfo.name);

  DWORD64 dwDisplacement64;
  if (GetSymFromAddr64(address, &dwDisplacement64, pSym))
#endif
  {
    os << " " << pSym->Name;
    if (dwDisplacement64 != 0) {
      int displacement = static_cast<int>(dwDisplacement64);
      if (displacement < 0)
        os << " - " << -displacement;
      else
        os << " + " << displacement;
    }
  }

  ///////////////////////////////
  // Log the line number

  if (showLines_) {
    DbgInit<IMAGEHLP_LINE64> lineInfo;
    DWORD dwDisplacement(0);
    if (GetLineFromAddr64(address, &dwDisplacement, &lineInfo)) {
      os << "   " << lineInfo.FileName << "(" << lineInfo.LineNumber << ")";
      if (dwDisplacement != 0) {
        os << " + " << dwDisplacement << " byte"
           << (dwDisplacement == 1 ? "" : "s");
      }
    }
  }
  return cacheSymbol;
}

/////////////////////////////////////////////////////////////////////////////////////
/** Print inline address to a stream */
void SymbolEngine::printInlineAddress(DWORD64 address, DWORD inline_context,
                                      std::ostream &os) const {
  ///////////////////////////////
  // Log the symbol name
#ifdef DBGHELP_6_2_APIS
  struct {
    SYMBOL_INFO symInfo;
    char name[4 * 256];
  } SymInfo = {{sizeof(SymInfo.symInfo)}, ""};

  PSYMBOL_INFO pSym = &SymInfo.symInfo;
  pSym->MaxNameLen = sizeof(SymInfo.name);

  DWORD64 dwDisplacement64(0);
  if (FromInlineContext(address, inline_context, &dwDisplacement64, pSym)) {
    os << pSym->Name;
    if (dwDisplacement64 != 0) {
      int displacement = static_cast<int>(dwDisplacement64);
      if (displacement < 0)
        os << " - " << -displacement;
      else
        os << " + " << displacement;
    }
  }

  ///////////////////////////////
  // Log the line number
  if (showLines_) {
    DbgInit<IMAGEHLP_LINE64> lineInfo;
    DWORD dwDisplacement(0);
    if (GetLineFromInlineContext(address, inline_context, 0, &dwDisplacement,
                                 &lineInfo)) {
      os << "   " << lineInfo.FileName << "(" << lineInfo.LineNumber << ")";
      if (dwDisplacement != 0) {
        os << " + " << dwDisplacement << " byte"
           << (dwDisplacement == 1 ? "" : "s");
      }
    }
  }
#endif // DBGHELP_6_2_APIS
}

/////////////////////////////////////////////////////////////////////////////////////
// Convert address to a string.
std::string SymbolEngine::addressToName(DWORD64 address) const {
  std::map<DWORD64, std::string>::iterator it =
      pImpl_->addressMap.find(address);
  if (it == pImpl_->addressMap.end()) {
    std::ostringstream oss;
    if (!printAddress(address, oss))
      return oss.str();
    it = pImpl_->addressMap.insert(std::make_pair(address, oss.str())).first;
  }

  return it->second;
}

// Convert pointer to a string.
std::string SymbolEngine::addressToName(PVOID pointer) const {
  return addressToName(DWORD64(pointer));
}

/////////////////////////////////////////////////////////////////////////////////////
// Convert inline address to a string.
std::string SymbolEngine::inlineToName(DWORD64 address,
                                       DWORD inline_context) const {
  std::pair<DWORD64, DWORD> key(address, inline_context);
  std::map<std::pair<DWORD64, DWORD>, std::string>::iterator it =
      pImpl_->inlineMap.find(key);
  if (it == pImpl_->inlineMap.end()) {
    std::ostringstream oss;
    printInlineAddress(address, inline_context, oss);
    it = pImpl_->inlineMap.insert(std::make_pair(key, oss.str())).first;
  }

  return it->second;
}

/////////////////////////////////////////////////////////////////////////////////////
// StackTrace: try to trace the stack to the given output stream
void SymbolEngine::StackTrace(HANDLE hThread, const CONTEXT &context,
                              std::ostream &os) const {
  STACKFRAME64 stackFrame{};
  CONTEXT rwContext{};
  try {
    rwContext = context;
  } catch (...) {
    // stack based context may be missing later sections -- based on the flags
  }
  PVOID pContext =
      &rwContext; // it is claimed this is not needed on Intel...lies

#ifdef _M_IX86
  DWORD const machineType = IMAGE_FILE_MACHINE_I386;

  // StackFrame needs to be set up on Intel
  stackFrame.AddrPC.Offset = context.Eip;
  stackFrame.AddrPC.Mode = AddrModeFlat;

  stackFrame.AddrFrame.Offset = context.Ebp;
  stackFrame.AddrFrame.Mode = AddrModeFlat;

  stackFrame.AddrStack.Offset = context.Esp;
  stackFrame.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
  DWORD machineType = IMAGE_FILE_MACHINE_AMD64;

  stackFrame.AddrPC.Offset = context.Rip;
  stackFrame.AddrPC.Mode = AddrModeFlat;

  stackFrame.AddrFrame.Offset = context.Rbp;
  stackFrame.AddrFrame.Mode = AddrModeFlat;

  stackFrame.AddrStack.Offset = context.Rsp;
  stackFrame.AddrStack.Mode = AddrModeFlat;

  BOOL bWow64(false);
  WOW64_CONTEXT wow64_context = {0};
  wow64_context.ContextFlags = WOW64_CONTEXT_FULL;

  if (IsWow64Process(GetProcess(), &bWow64) && bWow64) {
    if (getWow64ThreadContext(GetProcess(), hThread, rwContext,
                              &wow64_context)) {
      machineType = IMAGE_FILE_MACHINE_I386;
      pContext = &wow64_context;
      stackFrame.AddrPC.Offset = wow64_context.Eip;
      stackFrame.AddrFrame.Offset = wow64_context.Ebp;
      stackFrame.AddrStack.Offset = wow64_context.Esp;
    }
  }
#else
#error Unsupported target platform
#endif // _M_IX86

  // For loop detection
  DWORD64 currFrame = 0;

  // For 'wandering stack'
  DWORD nonExec(0);
  DWORD const maxNonExec(3);

  // use local copies of instance data
  int depth = maxStackDepth_;
  int skip = skipCount_;

  // Despite having GetModuleBase in the call to StackWalk it needs help for the
  // first address
  GetModuleBase(stackFrame.AddrPC.Offset);

  while (::StackWalk64(machineType, GetProcess(), hThread, &stackFrame,
                       pContext, nullptr,
                       nullptr, // implies ::SymFunctionTableAccess,
                       ::GetModuleBase, nullptr)) {
    const DWORD64 frame = stackFrame.AddrFrame.Offset;
    if (frame == 0)
      break;

    const DWORD64 pc = stackFrame.AddrPC.Offset;
    if (pc == 0) {
      os << "Null address\n";
      break;
    }

    if (currFrame != 0) {
      if (currFrame >= frame) {
        os << "Stack frame: " << (PVOID)frame << " out of sequence\n";
        break;
      }
    }
    currFrame = frame;

    // This helps x64 stack walking -- I think this might be a bug as the
    // function is already supplied in the StackWalk64 call ...
    GetModuleBase(pc);

    if (skip > 0) {
      skip--;
      continue;
    }

    if (depth > -1) {
      if (depth-- == 0)
        break;
    }

    if (isExecutable(pc)) {
      nonExec = 0;
    } else {
      ++nonExec;
      if (nonExec > maxNonExec)
        break;
    }

    // We now think we have a frame worth processing

    const DWORD inline_count = AddrIncludeInlineTrace(pc);

    os << addressToName(pc);

    if (inline_count) {
      os << " (" << inline_count << " inlined)";
    }

    if (nonExec) {
      os << " (non executable)";
    }

    os << "\n";

#if 0
    os << "AddrPC: " << (PVOID)stackFrame.AddrPC.Offset
       << " AddrReturn: " << (PVOID)stackFrame.AddrReturn.Offset
       << " AddrFrame: " << (PVOID)stackFrame.AddrFrame.Offset
       << " AddrStack: " << (PVOID)stackFrame.AddrStack.Offset
       << " FuncTableEntry: " << (PVOID)stackFrame.FuncTableEntry
       << " Far: " << stackFrame.Far
       << " Virtual: " << stackFrame.Virtual
       << " AddrBStore: " << (PVOID)stackFrame.AddrBStore.Offset
       << "\n";
#endif

    if (showParams_) {
      os << "  " << (PVOID)stackFrame.AddrFrame.Offset << ":";
      addParams(os, stackFrame.Params,
                sizeof(stackFrame.Params) / sizeof(stackFrame.Params[0]));
      os << "\n";
    }
    if (showVariables_) {
      showVariablesAt(os, stackFrame.AddrPC.Offset, stackFrame.AddrFrame.Offset,
                      rwContext, *this);
    }

    // Expand inline frames
    if (inline_count) {
      DWORD inline_context(0), frame_index(0);
      if (QueryInlineTrace(pc, 0, pc, pc, &inline_context, &frame_index)) {
        for (DWORD i = 0; i < inline_count; i++, inline_context++) {
          os << std::setw(31) << std::left << "[inline frame]"
             << inlineToName(pc, inline_context) << '\n';
          if (showVariables_) {
            showInlineVariablesAt(os, stackFrame.AddrPC.Offset,
                                  stackFrame.AddrFrame.Offset, rwContext,
                                  inline_context, *this);
          }
        }
      }
    }
  }

  os.flush();
}

//////////////////////////////////////////////////////////
// GetCurrentThreadContext
//
// Get context for the current thread, correcting the stack frame to the caller
//
// We sort out 3 key registers after GetThreadContext to
// prevent trying to stack walk after we've modified the stack...
// static
#ifdef _M_IX86
BOOL __declspec(naked)
SymbolEngine::GetCurrentThreadContext(CONTEXT *pContext) {
  DWORD regIp, regSp, regBp;
  BOOL rc;

  _asm push ebp;
  _asm mov ebp, esp;
  _asm sub esp, __LOCAL_SIZE;

  rc = ::GetThreadContext(GetCurrentThread(), pContext);

  if (rc) {
    _asm mov eax, [ebp + 4]; /* return address */
    _asm mov regIp, eax;
    _asm lea eax, [ebp + 0ch]; /* caller's SP before pushing pContext */
    _asm mov regSp, eax;
    _asm mov eax, [ebp]; /* caller's BP */
    _asm mov regBp, eax;

    pContext->Eip = regIp;
    pContext->Esp = regSp;
    pContext->Ebp = regBp;
  }

  _asm mov eax, rc;
  _asm mov esp, ebp;
  _asm pop ebp;
  _asm ret;
}
#else
/** get context for the current thread, correcting the stack frame to the caller
 */
void(WINAPI *SymbolEngine::GetCurrentThreadContext)(PCONTEXT pContext) =
    RtlCaptureContext;
#endif // _M_IX86

//////////////////////////////////////////////////////////
//
void SymbolEngine::SEHTrace(PVOID ExceptionList, std::ostream &os) const {
  // Got the first entry of the exception stack
  for (int i = 0; (maxSehDepth_ < 0) || (i < maxSehDepth_); ++i) {
    if (ExceptionList == (PVOID)(INT_PTR)-1 || ExceptionList == nullptr)
      break;

    struct Frame {
      PVOID previous;
      PVOID handler;
    } frame;

    if (!ReadMemory(ExceptionList, (PVOID)&frame, sizeof(frame))) {
      std::cerr << "ReadProcessMemory at " << ExceptionList
                << " failed: " << GetLastError() << std::endl;
      return;
    }

    os << addressToName(frame.handler);
    os << "\n";

    PVOID catchHandler = nullptr;
    bool isMsvcHandler = findMsvcCppHandler(frame.handler, &catchHandler);

    if (showParams_) {
      struct {
        Frame frame;
        DWORD Params[3];
      } extendedFrame;

      if (ReadMemory(ExceptionList, &extendedFrame, sizeof(extendedFrame))) {
        os << "  " << ExceptionList << ":";
        addParams(os, extendedFrame.Params,
                  sizeof(extendedFrame.Params) /
                      sizeof(extendedFrame.Params[0]));

        // For Msvc Cpp handlers params[2] is actually the return address
        if (isMsvcHandler) {
          os << " [" << addressToName(extendedFrame.Params[2]) << "]";
        }
        os << "\n";
      }
    }

    if (catchHandler) {
      os << "  => " << addressToName(catchHandler) << "\n";
    }

    if (ExceptionList < frame.previous) {
      ExceptionList = frame.previous;
    } else if (ExceptionList > frame.previous) {
      os << "  ends (" << frame.previous << "<" << ExceptionList << ")\n";
      break;
    } else {
      // Cygwin ends with self-referential handler
      break;
    }
  }
}

//////////////////////////////////////////////////////////
// Hack to try and get first catch handler for MSVC exception
//
// Attempt to walk the internal Microsoft structures
// to find the first catch handler for a C++ exception

// Returns:
//    false if not an Msvc C++ handler, msvcHandler = 0
//    true
//      msvcHandler = 0 if no catch handler [implicit frame unwind code]
//      else handler address

bool SymbolEngine::findMsvcCppHandler(PVOID sehHandler,
                                      PVOID *msvcHandler) const {
  // Set the default return value
  *msvcHandler = nullptr;

  BYTE buffer[1 + sizeof(PVOID)]; // read mov instruction
  PVOID *bufptr;                  // pointer into buffer

  // Read 5 bytes from handler address and check 1st byte is
  // a mov eax (0xb8)
  if ((!ReadMemory(sehHandler, (PVOID)buffer, sizeof(buffer))) ||
      (buffer[0] != 0xb8)) {
    return false;
  }

  // deref and read FrameHandler
  bufptr = (PVOID *)(buffer + 1);
  PVOID pFrameHandler = *bufptr;
  MsvcFrameHandler frameHandler;
  if (!ReadMemory(pFrameHandler, (PVOID)&frameHandler, sizeof(frameHandler)))
    return false;

  // Verify 'magic number'
  if (frameHandler.magic != MSVC_MAGIC_NUMBER1)
    return false;

  // We have definitely got an MSVC handler - has it got a catch address?

  if (frameHandler.cTryEntry == 0)
    return true;

  // Read first try entry
  MsvcTryEntry tryEntry;
  if (!ReadMemory(frameHandler.pTryEntry, (PVOID)&tryEntry, sizeof(tryEntry)))
    return true;

  // Read first catch entry
  MsvcCatchEntry catchEntry;
  if (!ReadMemory(tryEntry.pCatchEntry, (PVOID)&catchEntry, sizeof(catchEntry)))
    return true;

  // return first target address
  *msvcHandler = catchEntry.catchHandler;
  return true;
}

//////////////////////////////////////////////////////////
// showMsvcThrow
//
// Attempt to find type information for MSVC C++ throw parameter

void SymbolEngine::showMsvcThrow(std::ostream &os, PVOID throwInfo,
                                 PVOID base) const {
  MsvcThrow msvcThrow = {0};
  MsvcClassHeader msvcClassHeader = {0};
  MsvcClassInfo msvcClassInfo = {0};
  BYTE raw_type_info[sizeof(type_info) + 256] = "";

  if (!ReadMemory((PVOID)throwInfo, &msvcThrow, sizeof(msvcThrow)) ||
      !ReadMemory((PVOID)((ULONG_PTR)base + msvcThrow.pClassHeader),
                  &msvcClassHeader, sizeof(msvcClassHeader)) ||
      !ReadMemory((PVOID)((ULONG_PTR)base + msvcClassHeader.Info[0]),
                  &msvcClassInfo, sizeof(msvcClassInfo)) ||
      !ReadPartialProcessMemory(
          GetProcess(), (PVOID)((ULONG_PTR)base + msvcClassInfo.pTypeInfo),
          &raw_type_info, sizeof(type_info), sizeof(raw_type_info) - 1)) {
    return;
  }

  const std::type_info *pType_info = (const std::type_info *)raw_type_info;
  const char *decorated_name = pType_info->raw_name();

  char buffer[1024] = "";
  if ((decorated_name[0] == '.') &&
      UnDecorateSymbolName(decorated_name + 1, buffer, sizeof(buffer),
                           UNDNAME_32_BIT_DECODE | UNDNAME_NO_ARGUMENTS)) {
    os << " (" << buffer << ")";
  } else {
    os << " (" << decorated_name << ")";
  }
}

// Helper for ReadProcessMemory
bool SymbolEngine::ReadMemory(LPCVOID lpBaseAddress, // base of memory area
                              LPVOID lpBuffer,       // data buffer
                              SIZE_T nSize) const    // number of bytes to read
{
  return ReadProcessMemory(GetProcess(), lpBaseAddress, lpBuffer, nSize,
                           nullptr);
}

//////////////////////////////////////////////////////////
// DbgHelp 6.1 functionality - get the name for a symbol
BOOL SymbolEngine::decorateName(std::string &name, ULONG64 ModBase,
                                ULONG TypeIndex) const {
  BOOL bRet(false);

#ifdef DBGHELP_6_1_APIS

  WCHAR *typeName = nullptr;
  if (GetTypeInfo(ModBase, TypeIndex, TI_GET_SYMNAME, &typeName)) {
    bool const nested = (name.length() != 0);
    if (nested) {
      name.insert(0, " ");
    }
    name.insert(0, or2::strFromWchar(typeName));
    // free memory for typeName - by experiment with a debugger it comes from
    // LocalAlloc
    LocalFree(typeName);
    if (nested) {
      return true;
    }
  }

  bool bRecurse(false); // set to true to recurse down the type tree
  enum SymTagEnum tag = (enum SymTagEnum)0;
  GetTypeInfo(ModBase, TypeIndex, TI_GET_SYMTAG, &tag);
  switch (tag) {
  case SymTagBaseType: {
    DWORD baseType(0);
    ULONG64 length(0);
    GetTypeInfo(ModBase, TypeIndex, TI_GET_BASETYPE, &baseType);
    GetTypeInfo(ModBase, TypeIndex, TI_GET_LENGTH, &length);
    name.insert(0, " ");
    name.insert(0, getBaseType(baseType, length));
    bRet = true;
    break;
  }
  case SymTagPointerType:
    name.insert(0, "*");
    bRecurse = true;
    break;
  case SymTagFunctionType:
    if (name[0] == '*') {
      name.insert(0, "(");
      name += ")";
    }
    name += "()";
    bRecurse = true;
    break;
  case SymTagArrayType: {
    if (name[0] == '*') {
      name.insert(0, "(");
      name += ")";
    }
    DWORD Count(0);
    GetTypeInfo(ModBase, TypeIndex, TI_GET_COUNT, &Count);
    name += "[";
    if (Count) {
      std::ostringstream oss;
      oss << Count;
      name += oss.str();
    }
    name += "]";
    bRecurse = true;
    break;
  }
  case SymTagFunction:
  case SymTagData:
    bRecurse = true;
    break;
  case SymTagBaseClass:
    break;
  default: {
    std::ostringstream oss;
    oss << tag << " ";
    name.insert(0, oss.str());
    break;
  }
  }

  if (bRecurse) {
    DWORD ti = 0;
    if (GetTypeInfo(ModBase, TypeIndex, TI_GET_TYPEID, &ti)) {
      bRet = decorateName(name, ModBase, ti);
    }
  }

  VARIANT value; // do *not* use _variant_t as it is a shallow copy
  if (GetTypeInfo(ModBase, TypeIndex, TI_GET_VALUE, &value)) {
    _variant_t v(value);
    v.ChangeType(VT_BSTR);
    name += "=" + or2::strFromWchar(v.bstrVal);
  }

#endif // DBGHELP_6_1_APIS

  return bRet;
}

///////////////////////////////////////////////////////////////////////////
/** enumerate local variables at an address */
BOOL SymbolEngine::enumLocalVariables(DWORD64 codeOffset, DWORD64 frameOffset,
                                      EnumLocalCallBack &cb) const {
#ifdef DBGHELP_6_1_APIS

  IMAGEHLP_STACK_FRAME stackFrame = {0};
  stackFrame.InstructionOffset = codeOffset;
  stackFrame.FrameOffset = frameOffset;

  BOOL ret = SetContext(&stackFrame, nullptr);
  // Note: by experiment with SymUnpack must ignore failures from SetContext ...
  (void)ret;
  EngineCallBack callBack(*this, cb);
  ret = EnumSymbols(0, "*", EngineCallBack::enumSymbolsProc, &callBack);
  return ret;

#else

  return false;

#endif // DBGHELP_6_1_APIS
}

///////////////////////////////////////////////////////////
/* Write a simple mini-dump for an exception in the <b>current</b> thread */
BOOL SymbolEngine::dumpSelf(std::string const &miniDumpFile,
                            EXCEPTION_POINTERS *ExceptionInfo) {
  BOOL ret(FALSE);

#ifdef DBGHELP_6_1_APIS
  HANDLE const hDumpFile = CreateFile(miniDumpFile.c_str(), FILE_WRITE_DATA, 0,
                                      nullptr, CREATE_ALWAYS, 0, nullptr);

  if (hDumpFile != INVALID_HANDLE_VALUE) {
    MINIDUMP_EXCEPTION_INFORMATION ExceptionParam;
    ExceptionParam.ThreadId = GetCurrentThreadId();
    ExceptionParam.ExceptionPointers = ExceptionInfo;
    ExceptionParam.ClientPointers = TRUE;

    ret = WriteMiniDump(::GetCurrentProcessId(), hDumpFile, MiniDumpNormal,
                        &ExceptionParam, nullptr, nullptr);

    CloseHandle(hDumpFile);
  }
#endif // DBGHELP_6_1_APIS

  return ret;
}

/////////////////////////////////////////////////////////////////////////////////////
// Read a string from the target
std::string SymbolEngine::getString(PVOID address, BOOL unicode,
                                    DWORD maxStringLength) const {
  if (unicode) {
    std::vector<wchar_t> chVector(maxStringLength + 1);
    ReadPartialProcessMemory(GetProcess(), address, &chVector[0],
                             sizeof(wchar_t),
                             maxStringLength * sizeof(wchar_t));
    size_t const mbLen = Utf16ToMbs(nullptr, 0, &chVector[0], maxStringLength);
    if (mbLen == 0) {
      return "invalid string";
    } else {
      std::vector<char> mbStr(mbLen + 1);
      Utf16ToMbs(&mbStr[0], mbLen, &chVector[0], maxStringLength);
      return &mbStr[0];
    }
  } else {
    std::vector<char> chVector(maxStringLength + 1);
    ReadPartialProcessMemory(GetProcess(), address, &chVector[0], 1,
                             maxStringLength);
    return &chVector[0];
  }
}

/////////////////////////////////////////////////////////////////////////////////////
// Returns whether address points to executable code
bool SymbolEngine::isExecutable(DWORD64 address) const {
  bool ret(false);

  static const DWORD AnyExecute = PAGE_EXECUTE | PAGE_EXECUTE_READ |
                                  PAGE_EXECUTE_READWRITE |
                                  PAGE_EXECUTE_WRITECOPY;

  MEMORY_BASIC_INFORMATION mb = {nullptr};
  if (VirtualQueryEx(GetProcess(), (PVOID)address, &mb, sizeof(mb))) {
    if ((mb.Protect & (AnyExecute)) != 0) {
      ret = true; // executable code
    }
  }

  return ret;
}

namespace {

//////////////////////////////////////////////////////////
// fix for problem with resource leak in symsrv
void fixSymSrv() {
  static bool loaded = false;
  if (!loaded) {
    HMODULE const hSymSrv = ::GetModuleHandle("SymSrv");
    if (hSymSrv != nullptr) {
      ::LoadLibrary("SymSrv");
      loaded = true;
    }
  }
}

//////////////////////////////////////////////////////////
void showVariablesAt(std::ostream &os, DWORD64 codeOffset, DWORD64 frameOffset,
                     const CONTEXT &context, or2::SymbolEngine const &eng) {
#ifdef DBGHELP_6_1_APIS
  VariableCallBack cb(os, frameOffset, context);

  eng.enumLocalVariables(codeOffset, frameOffset, cb);
#endif // DBGHELP_6_1_APIS
}

void showInlineVariablesAt(std::ostream &os, DWORD64 codeOffset,
                           DWORD64 frameOffset, const CONTEXT &context,
                           DWORD inline_context, SymbolEngine const &eng) {
#ifdef DBGHELP_6_1_APIS
  if (eng.SetScopeFromInlineContext(codeOffset, inline_context)) {
    VariableCallBack cb(os, frameOffset, context);

    EngineCallBack callBack(eng, cb);
    eng.EnumSymbolsEx(0, "*", EngineCallBack::enumSymbolsProc, &callBack,
                      SYMENUM_OPTIONS_INLINE);
  }
#endif // DBGHELP_6_1_APIS
}

//////////////////////////////////////////////////////////
// Get register name and offset from the 'reg' value supplied
RegInfo getRegInfo(ULONG reg, const CONTEXT &context) {
  switch (reg) {
#ifdef _M_IX86
#define CASE(REG, NAME, SRC, MASK)                                             \
  case CV_REG_##REG:                                                           \
    return RegInfo(NAME, context.SRC & MASK)

    CASE(AL, "al", Eax, 0xff);
    CASE(BL, "bl", Ebx, 0xff);
    CASE(CL, "cl", Ecx, 0xff);
    CASE(DL, "dl", Edx, 0xff);

    CASE(AX, "ax", Eax, 0xffff);
    CASE(BX, "bx", Ebx, 0xffff);
    CASE(CX, "cx", Ecx, 0xffff);
    CASE(DX, "dx", Edx, 0xffff);
    CASE(SP, "sp", Esp, 0xffff);
    CASE(BP, "bp", Ebp, 0xffff);
    CASE(SI, "si", Esi, 0xffff);
    CASE(DI, "di", Edi, 0xffff);

    CASE(EAX, "eax", Eax, ~0u);
    CASE(EBX, "ebx", Ebx, ~0u);
    CASE(ECX, "ecx", Ecx, ~0u);
    CASE(EDX, "edx", Edx, ~0u);
    CASE(ESP, "esp", Esp, ~0u);
  case CV_ALLREG_VFRAME: // x86 alternative
    CASE(EBP, "ebp", Ebp, ~0u);
    CASE(ESI, "esi", Esi, ~0u);
    CASE(EDI, "edi", Edi, ~0u);
#elif _M_X64
#define CASE(REG, NAME, SRC, MASK)                                             \
  case CV_AMD64_##REG:                                                         \
    return RegInfo(NAME, context.SRC & MASK)

    CASE(AL, "al", Rax, 0xff);
    CASE(BL, "bl", Rbx, 0xff);
    CASE(CL, "cl", Rcx, 0xff);
    CASE(DL, "dl", Rdx, 0xff);

    CASE(AX, "ax", Rax, 0xffff);
    CASE(BX, "bx", Rbx, 0xffff);
    CASE(CX, "cx", Rcx, 0xffff);
    CASE(DX, "dx", Rdx, 0xffff);
    CASE(SP, "sp", Rsp, 0xffff);
    CASE(BP, "bp", Rbp, 0xffff);
    CASE(SI, "si", Rsi, 0xffff);
    CASE(DI, "di", Rdi, 0xffff);

    CASE(EAX, "eax", Rax, ~0u);
    CASE(EBX, "ebx", Rbx, ~0u);
    CASE(ECX, "ecx", Rcx, ~0u);
    CASE(EDX, "edx", Rdx, ~0u);
    CASE(ESP, "esp", Rsp, ~0u);
    CASE(EBP, "ebp", Rbp, ~0u);
    CASE(ESI, "esi", Rsi, ~0u);
    CASE(EDI, "edi", Rdi, ~0u);

    CASE(RAX, "rax", Rax, ~0ull);
    CASE(RBX, "rbx", Rbx, ~0ull);
    CASE(RCX, "rcx", Rcx, ~0ull);
    CASE(RDX, "rdx", Rdx, ~0ull);
    CASE(RSP, "rsp", Rsp, ~0ull);
    CASE(RBP, "rbp", Rbp, ~0ull);
    CASE(RSI, "rsi", Rsi, ~0ull);
    CASE(RDI, "rdi", Rdi, ~0ull);

    CASE(R8, "r8", R8, ~0ull);
    CASE(R9, "r9", R9, ~0ull);
    CASE(R10, "r10", R10, ~0ull);
    CASE(R11, "r11", R11, ~0ull);
    CASE(R12, "r12", R12, ~0ull);
    CASE(R13, "r13", R13, ~0ull);
    CASE(R14, "r14", R14, ~0ull);
    CASE(R15, "r15", R15, ~0ull);

    CASE(R8B, "r8b", R8, 0xff);
    CASE(R9B, "r9b", R9, 0xff);
    CASE(R10B, "r10b", R10, 0xff);
    CASE(R11B, "r11b", R11, 0xff);
    CASE(R12B, "r12b", R12, 0xff);
    CASE(R13B, "r13b", R13, 0xff);
    CASE(R14B, "r14b", R14, 0xff);
    CASE(R15B, "r15b", R15, 0xff);

    CASE(R8W, "r8w", R8, 0xffff);
    CASE(R9W, "r8w", R8, 0xffff);
    CASE(R10W, "r10w", R10, 0xffff);
    CASE(R11W, "r10w", R10, 0xffff);
    CASE(R12W, "r10w", R10, 0xffff);
    CASE(R13W, "r10w", R10, 0xffff);
    CASE(R14W, "r10w", R10, 0xffff);
    CASE(R15W, "r10w", R10, 0xffff);

    CASE(R8D, "r8d", R8, ~0u);
    CASE(R9D, "r8d", R8, ~0u);
    CASE(R10D, "r10d", R10, ~0u);
    CASE(R11D, "r11d", R11, ~0u);
    CASE(R12D, "r12d", R12, ~0u);
    CASE(R13D, "r13d", R13, ~0u);
    CASE(R14D, "r14d", R14, ~0u);
    CASE(R15D, "r15d", R15, ~0u);
#endif
  }
#undef CASE

  return RegInfo("", 0);
}

//////////////////////////////////////////////////////////
// Helper function: getBaseType maps PDB type + length to C++ name
std::string getBaseType(DWORD baseType, ULONG64 length) {
  static struct {
    DWORD baseType;
    ULONG64 length;
    const char *name;
  } baseList[] = {
      // Table generated from dumping out 'baseTypes.cpp'
      {btNoType, 0, "(null)"}, // Used for __$ReturnUdt
      {btVoid, 0, "void"},
      {btChar, sizeof(char), "char"},
      {btWChar, sizeof(wchar_t), "wchar_t"},
      {btInt, sizeof(signed char), "signed char"},
      {btInt, sizeof(short), "short"},
      {btInt, sizeof(int), "int"},
      {btInt, sizeof(__int64), "__int64"},
      {btUInt, sizeof(unsigned char),
       "unsigned char"}, // also used for 'bool' in VC6
      {btUInt, sizeof(unsigned short), "unsigned short"},
      {btUInt, sizeof(unsigned int), "unsigned int"},
      {btUInt, sizeof(unsigned __int64), "unsigned __int64"},
      {btFloat, sizeof(float), "float"},
      {btFloat, sizeof(double), "double"},
      {btFloat, sizeof(long double), "long double"},
      // btBCD
      {btBool, sizeof(bool), "bool"}, // VC 7.x
      {btLong, sizeof(long), "long"},
      {btULong, sizeof(unsigned long), "unsigned long"},
      // btCurrency
      // btDate
      // btVariant
      // btComplex
      // btBit
      // btBSTR
      {btHresult, sizeof(HRESULT), "HRESULT"},

  };

  for (auto &i : baseList) {
    if ((baseType == i.baseType) && (length == i.length)) {
      return i.name;
    }
  }

  // Unlisted type - use the data values and then fix the code (!)
  std::ostringstream oss;
  oss << "pdb type: " << baseType << "/" << (DWORD)length;
  return oss.str();
}

#ifdef _M_X64
static DWORD const WOW64_CS_32BIT =
    0x23; // Wow64 32-bit code segment on Windows 2003
static DWORD const TLS_OFFSET =
    0x1480; // offsetof(ntdll!_TEB, TlsSlots) on Windows 2003
#pragma pack(4)
struct Wow64_SaveContext {
  ULONG unknown1;
  WOW64_CONTEXT context;
  ULONG unknown2;
};
#pragma pack()

using fnWow64GetThreadContext = BOOL WINAPI(HANDLE, WOW64_CONTEXT *);

using fnNtQueryInformationThread = NTSTATUS WINAPI(HANDLE ThreadHandle,
                                                   ULONG ThreadInformationClass,
                                                   PVOID Buffer, ULONG Length,
                                                   PULONG ReturnLength);

// Helper function to delay load Wow64GetThreadContext or emulate on W2K3
BOOL getWow64ThreadContext(HANDLE hProcess, HANDLE hThread,
                           CONTEXT const &context, WOW64_CONTEXT *pWowContext) {
  static HMODULE hKernel32 = ::GetModuleHandle("KERNEL32");
  static fnWow64GetThreadContext *pFn =
      (fnWow64GetThreadContext *)::GetProcAddress(hKernel32,
                                                  "Wow64GetThreadContext");
  if (pFn) {
    // Vista and above
    return pFn(hThread, pWowContext);
  } else if (context.SegCs == WOW64_CS_32BIT) {
    if (pWowContext->ContextFlags & CONTEXT_CONTROL) {
      pWowContext->Ebp = (ULONG)context.Rbp;
      pWowContext->Eip = (ULONG)context.Rip;
      pWowContext->SegCs = context.SegCs;
      pWowContext->EFlags = context.EFlags;
      pWowContext->Esp = (ULONG)context.Rsp;
      pWowContext->SegSs = context.SegSs;
    }
    if (pWowContext->ContextFlags & CONTEXT_INTEGER) {
      pWowContext->Edi = (ULONG)context.Rdi;
      pWowContext->Esi = (ULONG)context.Rsi;
      pWowContext->Ebx = (ULONG)context.Rbx;
      pWowContext->Edx = (ULONG)context.Rdx;
      pWowContext->Ecx = (ULONG)context.Rcx;
      pWowContext->Eax = (ULONG)context.Rax;
    }
    return true;
  } else {
    static HMODULE hNtDll = ::GetModuleHandle("NTDLL");
    static fnNtQueryInformationThread *pNtQueryInformationThread =
        (fnNtQueryInformationThread *)::GetProcAddress(
            hNtDll, "NtQueryInformationThread");
    ULONG_PTR ThreadInfo[6] = {0};
    if (pNtQueryInformationThread &&
        pNtQueryInformationThread(hThread, 0, &ThreadInfo, sizeof(ThreadInfo),
                                  nullptr) == 0) {
      PVOID *pTls = (PVOID *)(ThreadInfo[1] + TLS_OFFSET);
      Wow64_SaveContext saveContext = {0}, *pSaveContext = nullptr;

      if (ReadProcessMemory(hProcess, pTls + 1, &pSaveContext,
                            sizeof(pSaveContext), nullptr) &&
          ReadProcessMemory(hProcess, pSaveContext, &saveContext,
                            sizeof(saveContext), nullptr)) {
        *pWowContext = saveContext.context;
        return true;
      }
    }
  }
  return false;
}
#endif // _M_X64

} // namespace
} // namespace or2
