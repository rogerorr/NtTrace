/*
NAME
  SymExplorer

DESCRIPTION
  Use DbgHelp API to explore symbols for a DLL

AUTHOR
  Roger Orr mailto:rogero@howzatt.co.uk
  Bug reports, comments, and suggestions are always welcome.

COPYRIGHT
  Copyright (C) 2002,2003 under the MIT license:

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
    "$Id: SymExplorer.cpp 3097 2026-02-14 13:56:19Z roger $";

#define NOMINMAX

#pragma warning(disable : 4512) // assignment operator could not be generated

#define WIN32_LEAN_AND_MEAN
#include <comutil.h>
#include <windows.h>

#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#ifdef DISASM
#include "../Disasm/wrapper.h"
#endif // DISASM
#include "GetModuleBase.h"
#include "SymbolEngine.h"

// In case compiled against an older DbgHelp.h
#ifndef SYMOPT_PUBLICS_ONLY
#define SYMOPT_PUBLICS_ONLY 0x00004000
#endif

#include "../include/BasicType.h"
#include "../include/Options.h"
#include "../include/StrFromWchar.h"
#include "StreamGUID.h"

using namespace std;
using namespace or2;

enum hexmode { eDec, eHex };

std::ostream &operator<<(std::ostream &os, hexmode const &mode) {
  switch (mode) {
  case eDec:
    os << std::dec;
    break;
  case eHex:
    os << std::hex;
    break;
  }
  return os;
}

namespace {
std::string expandFlags(DWORD flags) {
#ifndef SYMFLAG_REGREL_ALIASINDIR
// Absent from older DbgHelp.h
#define SYMFLAG_REGREL_ALIASINDIR (SYMFLAG_PUBLIC_CODE + 1)
#endif // SYMFLAG_REGREL_ALIASINDIR
#define DEF(X)                                                                 \
  {                                                                            \
    SYMFLAG_##X, #X                                                            \
  }
  static const struct {
    DWORD flag;
    const char *name;
  } flag_values[] = {
      DEF(VALUEPRESENT),   DEF(REGISTER),
      DEF(REGREL),         DEF(FRAMEREL),
      DEF(PARAMETER),      DEF(LOCAL),
      DEF(CONSTANT),       DEF(EXPORT),
      DEF(FORWARDER),      DEF(FUNCTION),
      DEF(VIRTUAL),        DEF(THUNK),
      DEF(TLSREL),         DEF(SLOT),
      DEF(ILREL),          DEF(METADATA),
      DEF(CLR_TOKEN),      DEF(NULL),
      DEF(FUNC_NO_RETURN), DEF(SYNTHETIC_ZEROBASE),
      DEF(PUBLIC_CODE),    DEF(REGREL_ALIASINDIR),
      {0, nullptr},
  };
#undef DEF

  std::string result;
  for (int idx = 0; flag_values[idx].flag != 0; ++idx) {
    if (flag_values[idx].flag & flags) {
      if (!result.empty()) {
        result.append("|");
      }
      result.append(flag_values[idx].name);
    }
  }
  return result;
}
} // namespace

class SymExplorer {
public:
  explicit SymExplorer(std::string prompt);

  // Called by Debug engine for each public/global symbol
  BOOL enumSymbolsCallback(std::string const &SymbolName,
                           DWORD64 SymbolAddress);

  // Called by Debug engine for each type
  BOOL enumTypesCallback(std::string const &SymbolName, ULONG Index,
                         ULONG Size);

  // Called by Debug engine during ODR detection
  BOOL odrCallback(std::string const &SymbolName, ULONG size);

  int run(std::istream &iss);

  // Load a module, unoading any existing one
  bool load(std::string const &module);

private:
  SymbolEngine eng_;
  HMODULE hMod_{nullptr};
  DWORD64 baseAddress_{0};
  std::string prompt_;
  hexmode mode_{eDec};
  std::map<std::string, std::set<ULONG>> odr_;

  // Callback helpers
  static BOOL CALLBACK enumSymbolsCallback(PSYMBOL_INFO pSym, ULONG SymbolSize,
                                           PVOID thisObject);
  static BOOL CALLBACK enumTypesCallback(PSYMBOL_INFO pSym, ULONG SymbolSize,
                                         PVOID thisObject);
  static BOOL CALLBACK odrCallback(PSYMBOL_INFO pSym, ULONG SymbolSize,
                                   PVOID thisObject);
  bool odrFalsePositive(PSYMBOL_INFO pSym);

  regex enumRegex;

  // User interface functions
  using PFunc = bool (SymExplorer::*)(std::istream &iss);
  bool help(std::istream &iss);
  bool find(std::istream &iss);
  bool children(std::istream &iss);
  bool dec(std::istream &iss);
#ifdef DISASM
  bool disasm(std::istream &iss);
#endif // DISASM
  bool hex(std::istream &iss);
  bool index(std::istream &iss);
  bool load(std::istream &iss);
  bool locals(std::istream &iss);
  bool odr(std::istream &iss);
  bool showModule(std::istream &iss);
  bool symopt(std::istream &iss);
  bool type(std::istream &iss);
  bool udt(std::istream &iss);

  using FuncMap = std::map<std::string, PFunc>;
  FuncMap funcMap_;
  using HelpMap = std::map<std::string, std::string>;
  HelpMap helpMap_;

  // Read type index
  DWORD getTypeIndex(std::istream &is);

  // Read symbol address
  DWORD64 getSymbolAddress(std::istream &is);

  std::vector<enum SymTagEnum> getTags(TI_FINDCHILDREN_PARAMS *children);

  void printBaseClasses(TI_FINDCHILDREN_PARAMS *const children,
                        std::vector<enum SymTagEnum> const &tags);

  void printDataMembers(TI_FINDCHILDREN_PARAMS *const children,
                        std::vector<enum SymTagEnum> const &tags);

  void printValues(TI_FINDCHILDREN_PARAMS *const children,
                   std::vector<enum SymTagEnum> const &tags);

  static bool ctrlc_;
  static BOOL CtrlHandler(DWORD fdwCtrlType);
};

bool SymExplorer::ctrlc_ = false;

namespace {
std::ostream &operator<<(std::ostream &os, VARIANT const &rhs) {
  _variant_t value(rhs);
  value.ChangeType(VT_BSTR);
  os << or2::strFromWchar(value.bstrVal);
  return os;
}

std::ostream &operator<<(std::ostream &os, enum CV_call_e const &rhs) {
#define CASE(X)                                                                \
  case X:                                                                      \
    os << #X;                                                                  \
    break;
  switch (rhs) {
    CASE(CV_CALL_NEAR_C);
    CASE(CV_CALL_FAR_C);
    CASE(CV_CALL_NEAR_PASCAL);
    CASE(CV_CALL_FAR_PASCAL);
    CASE(CV_CALL_NEAR_FAST);
    CASE(CV_CALL_FAR_FAST);
    CASE(CV_CALL_SKIPPED);
    CASE(CV_CALL_NEAR_STD);
    CASE(CV_CALL_FAR_STD);
    CASE(CV_CALL_NEAR_SYS);
    CASE(CV_CALL_FAR_SYS);
    CASE(CV_CALL_THISCALL);
    CASE(CV_CALL_MIPSCALL);
    CASE(CV_CALL_GENERIC);
    CASE(CV_CALL_ALPHACALL);
    CASE(CV_CALL_PPCCALL);
    CASE(CV_CALL_SHCALL);
    CASE(CV_CALL_ARMCALL);
    CASE(CV_CALL_AM33CALL);
    CASE(CV_CALL_TRICALL);
    CASE(CV_CALL_SH5CALL);
    CASE(CV_CALL_M32RCALL);
    CASE(CV_CALL_CLRCALL);
  default:
    os << "(?"
          "?)";
    break;
  }
#undef CASE
  return os;
}

std::ostream &operator<<(std::ostream &os, enum DataKind const &rhs) {
#define CASE(X)                                                                \
  case X:                                                                      \
    os << #X;                                                                  \
    break;
  switch (rhs) {
    CASE(DataIsUnknown);
    CASE(DataIsLocal);
    CASE(DataIsStaticLocal);
    CASE(DataIsParam);
    CASE(DataIsObjectPtr);
    CASE(DataIsFileStatic);
    CASE(DataIsGlobal);
    CASE(DataIsMember);
    CASE(DataIsStaticMember);
    CASE(DataIsConstant);
  default:
    os << "(?"
          "?)";
    break;
  }
#undef CASE
  return os;
}

std::ostream &operator<<(std::ostream &os, enum UdtKind const &rhs) {
#define CASE(X)                                                                \
  case Udt##X:                                                                 \
    os << #X;                                                                  \
    break;
  switch (rhs) {
    CASE(Struct);
    CASE(Class);
    CASE(Union);
    CASE(Interface);
#if _MSC_VER >= 1930
    CASE(TaggedUnion);
#endif
  default:
    os << "(?"
          "?)";
    break;
  }
#undef CASE
  return os;
}

// We've read a string starting with a double quote - read the whole thing
void appendQuotedString(std::string &name, std::istream &iss) {
  name.erase(0, 1);
  while (name.empty() || *name.rbegin() != '\"') {
    std::string fragment;
    if (!(iss >> fragment))
      return;
    name += ' ' + fragment;
  }
  name.resize(name.size() - 1);
}

} // namespace

////////////////////////////////////////////////////////////////////////////////////
// forward references

////////////////////////////////////////////////////////////////////////////////////
SymExplorer::SymExplorer(std::string prompt)
    : eng_(GetCurrentProcess()), prompt_(std::move(prompt)) {
  // We want to undecorate functions ourselves and we want to work with both the
  // old and new DbgHelp.dll ...
  DWORD dwOpts = SymGetOptions();
  dwOpts &= ~SYMOPT_UNDNAME; // We want control over the level of undecoration
  dwOpts &= ~SYMOPT_PUBLICS_ONLY; // We want any available symbols
  SymSetOptions(dwOpts);

  // Test we can enumerate symbols
  if (!eng_.IsEnumSymbolsAvailable()) {
    throw std::runtime_error(
        "Variables not available - requires DbgHelp.dll 6.1 or higher");
  }

  auto define = [this](const char *name, PFunc func, const char *help) {
    funcMap_[name] = func;
    helpMap_[name] = help;
  };

  define("exit", nullptr, "Exit the program");
  define("quit", nullptr, "Exit the program");
  define("find", &SymExplorer::find,
         "Find the functions matching <pattern> in the target");
  define("help", &SymExplorer::help, "Get help");
  define("children", &SymExplorer::children,
         "Show child items for supplied symbol");
  define("dec", &SymExplorer::dec, "Select decimal number format");
#ifdef DISASM
  define("disasm", &SymExplorer::disasm,
         "Disassemble code at <address> for <length>");
#endif // DISASM
  define("hex", &SymExplorer::hex, "Select hexadecimal number format");
  define("index", &SymExplorer::index, "Display data for symbol <index>");
  define("load", &SymExplorer::load, "Load the specified binary");
  define("locals", &SymExplorer::locals, "Shoe local variables for <function>");
  define("odr", &SymExplorer::odr,
         "Look for ODR violation in types matching <pattern>. May have false "
         "positives, especially for incremental linking.");
  define("show", &SymExplorer::showModule, "Show details of the loaded image");
  define("symopt", &SymExplorer::symopt,
         "Set symbol options: +<n> to add, -<n> to remove, or <n> to set");
  define("type", &SymExplorer::type,
         "Show types matching <pattern> in the target");
  define("udt", &SymExplorer::udt, "Display a user defined type in C++ format");
}

////////////////////////////////////////////////////////////////////////////////////
// enumSymbolsCallback
//
// Called by Debug engine for each public/global symbol

BOOL CALLBACK SymExplorer::enumSymbolsCallback(PSYMBOL_INFO pSym,
                                               ULONG /*SymbolSize*/,
                                               PVOID thisObject) {
  return (static_cast<SymExplorer *>(thisObject))
      ->enumSymbolsCallback(std::string(pSym->Name, pSym->NameLen),
                            pSym->Address);
}

BOOL SymExplorer::enumSymbolsCallback(std::string const &SymbolName,
                                      DWORD64 SymbolAddress) {
  if (regex_search(SymbolName, enumRegex)) {
    std::cout << SymbolName;
    std::cout << " at " << (PVOID)SymbolAddress;

    DbgInit<IMAGEHLP_LINE64> lineInfo;
    DWORD dwDisplacement;
    if (eng_.GetLineFromAddr64(SymbolAddress, &dwDisplacement, &lineInfo)) {
      std::cout << "   " << lineInfo.FileName << "(" << lineInfo.LineNumber
                << ")";
    }
    std::cout << std::endl;
  }
  return !ctrlc_;
}

////////////////////////////////////////////////////////////////////////////////////
// enumTypesCallback
//
// Called by Debug engine for each type

BOOL CALLBACK SymExplorer::enumTypesCallback(PSYMBOL_INFO pSym,
                                             ULONG /*SymbolSize*/,
                                             PVOID thisObject) {
  return (static_cast<SymExplorer *>(thisObject))
      ->enumTypesCallback(std::string(pSym->Name, pSym->NameLen), pSym->Index,
                          pSym->Size);
}

BOOL SymExplorer::enumTypesCallback(std::string const &SymbolName, ULONG Index,
                                    ULONG Size) {
  if (regex_search(SymbolName, enumRegex)) {
    std::cout << SymbolName << " index: " << Index << " size: " << Size
              << std::endl;
  }
  return !ctrlc_;
}

////////////////////////////////////////////////////////////////////////////////////
// odrCallback
//
// Called by Debug engine during ODR detection

BOOL CALLBACK SymExplorer::odrCallback(PSYMBOL_INFO pSym, ULONG /*SymbolSize*/,
                                       PVOID thisObject) {
  if (pSym->Tag != SymTagUDT)
    return true;

  auto *pThis = static_cast<SymExplorer *>(thisObject);

  if (pThis->odrFalsePositive(pSym)) {
    return true;
  }

  return pThis->odrCallback(std::string(pSym->Name, pSym->NameLen), pSym->Size);
}

BOOL SymExplorer::odrCallback(std::string const &SymbolName, ULONG size) {
  if (regex_search(SymbolName, enumRegex)) {
    auto &set = odr_[SymbolName];
    if (set.insert(size).second && set.size() == 2) {
      std::cout << SymbolName << " has changed size (" << *set.rbegin()
                << " != " << *set.begin() << ")" << std::endl;
    }
  }
  return !ctrlc_;
}

bool SymExplorer::odrFalsePositive(PSYMBOL_INFO pSym) {
  static const std::string prefixes[] = {"<unnamed-",
                                         "`anonymous-namespace'::"};

  for (const auto &prefix : prefixes) {
    if ((pSym->NameLen > prefix.size()) &&
        (prefix.compare(0, prefix.size(), pSym->Name, prefix.size()) == 0)) {
      return true;
    }
  }

  DWORD64 nested{};
  (void)eng_.GetTypeInfo(baseAddress_, pSym->Index, TI_GET_NESTED, &nested);
  if (nested) {
    // There will also be an entry with the fully qualified name
    return true;
  }

  return false;
}

// Display a base type in the canonical form
std::ostream &operator<<(std::ostream &os, enum BasicType const &rhs) {
  switch (rhs) {
#define CASE(X)                                                                \
  case X:                                                                      \
    os << #X;                                                                  \
    break

    CASE(btNoType);
    CASE(btVoid);
    CASE(btChar);
    CASE(btWChar);
    CASE(btInt);
    CASE(btUInt);
    CASE(btFloat);
    CASE(btBCD);
    CASE(btBool);
    CASE(btLong);
    CASE(btULong);
    CASE(btCurrency);
    CASE(btDate);
    CASE(btVariant);
    CASE(btComplex);
    CASE(btBit);
    CASE(btBSTR);
    CASE(btHresult);

#undef CASE
  default:
    break;
  }
  return os;
}

/**
 * Look for One Definition Rule (odr) violations
 */
bool SymExplorer::odr(std::istream &iss) {
  bool result = false;
  std::string pattern;
  if (iss >> pattern && pattern[0] == '"') {
    appendQuotedString(pattern, iss);
  }
  try {
    enumRegex.assign(pattern);

    odr_.clear();
    result = eng_.EnumTypes(baseAddress_, odrCallback, (PVOID)this);
  } catch (std::exception &ex) {
    std::cout << "Error: " << ex.what() << std::endl;
    result = false;
  }
  return result;
}

//*************************************************************************************//
// showModule: try to get information about a module being loaded //
//*************************************************************************************//
bool SymExplorer::showModule(std::istream & /* iss */) {
  bool result(false);
  DbgInit<IMAGEHLP_MODULE64> ModuleInfo;
  if (eng_.GetModuleInfo64(baseAddress_, &ModuleInfo)) {
    result = true;

    cout << "  ModuleName: " << ModuleInfo.ModuleName << endl;
    cout << "  ImageName: " << ModuleInfo.ImageName << endl;
    cout << "  LoadedImageName: " << ModuleInfo.LoadedImageName << endl;
    cout << "  BaseOfImage: " << (PVOID)ModuleInfo.BaseOfImage << endl;
    cout << "  ImageSize: " << std::hex << ModuleInfo.ImageSize << mode_
         << endl;
    cout << "  TimeDateStamp: " << ModuleInfo.TimeDateStamp << endl;
    cout << "  CheckSum: " << ModuleInfo.CheckSum << endl;
    cout << "  NumSyms: " << ModuleInfo.NumSyms << endl;
    cout << "  SymType: " << ModuleInfo.SymType << endl;
    cout << "  LoadedPdbName: " << ModuleInfo.LoadedPdbName
         << endl; // pdb file name
    cout << "  CVSig: " << std::hex << ModuleInfo.CVSig << mode_
         << endl; // Signature of the CV record in the debug directories
    cout << "  CVData: " << ModuleInfo.CVData
         << endl; // Contents of the CV record
    cout << "  PdbSig: " << std::hex << ModuleInfo.PdbSig << mode_
         << endl; // Signature of PDB
    cout << "  PdbSig70: " << ModuleInfo.PdbSig70
         << endl; // Signature of PDB (VC 7 and up)
    cout << "  PdbAge: " << ModuleInfo.PdbAge << endl; // DBI age of pdb
    cout << "  PdbUnmatched: " << ModuleInfo.PdbUnmatched
         << endl; // loaded an unmatched pdb
    cout << "  DbgUnmatched: " << ModuleInfo.DbgUnmatched
         << endl; // loaded an unmatched dbg
    cout << "  LineNumbers: " << ModuleInfo.LineNumbers
         << endl; // we have line number information
    cout << "  GlobalSymbols: " << ModuleInfo.GlobalSymbols
         << endl; // we have internal symbol information
    cout << "  TypeInfo: " << ModuleInfo.TypeInfo
         << endl; // we have type information
    cout << "  SourceIndexed: " << ModuleInfo.SourceIndexed
         << endl; // pdb supports source server
    cout << "  Publics: " << ModuleInfo.Publics
         << endl; // contains public symbols
  } else {
    std::cout << "No module info for " << (PVOID)baseAddress_ << std::endl;
  }
  return result;
}

/**
 *
 */
bool SymExplorer::help(std::istream &iss) {
  std::string command;
  if (iss >> command) {
    const auto &cmd_it = helpMap_.find(command);
    if (cmd_it == helpMap_.end()) {
      std::cout << "No help available for " << command << '\n';
      return false;
    } else {
      std::cout << cmd_it->second << '\n';
      return true;
    }
  }

  std::cout << "help -- the following commands are available:" << '\n';
  for (const auto &it : funcMap_) {
    std::cout << " " << it.first;
  }
  std::cout << std::endl;
  return true;
}

/**
 * Display children for specified type index
 */
bool SymExplorer::children(std::istream &iss) {
  bool result(false);
  DWORD const index = getTypeIndex(iss);

  DWORD childCount(0);
  if (!eng_.GetTypeInfo(baseAddress_, index, TI_GET_CHILDRENCOUNT,
                        &childCount) ||
      childCount == 0) {
    std::cout << "No children for " << index << std::endl;
  } else {
    TI_FINDCHILDREN_PARAMS *children;
    size_t const length = offsetof(TI_FINDCHILDREN_PARAMS, ChildId) +
                          sizeof(children->ChildId) * childCount;
    children = static_cast<TI_FINDCHILDREN_PARAMS *>(::operator new(length));
    children->Count = childCount;
    children->Start = 0;
    if (eng_.GetTypeInfo(baseAddress_, index, TI_FINDCHILDREN, children)) {
      for (size_t idx = 0; idx != childCount; ++idx) {
        DWORD const child(children->ChildId[idx]);
        std::string name;
        eng_.decorateName(name, baseAddress_, child);
        std::cout << "Child: " << child << " " << name << std::endl;
      }
      result = true;
    } else {
      std::cout << "No info returned for " << index << std::endl;
    }
    ::operator delete(children);
  }
  return result;
}

/**
 * Set decimal output mode
 */
bool SymExplorer::dec(std::istream & /*iss*/) {
  mode_ = eDec;
  std::cout << mode_;
  return true;
}

#ifdef DISASM
/**
 * Disassemble code
 */
bool SymExplorer::disasm(std::istream &iss) {
  bool result(false);
  DWORD64 address = getSymbolAddress(iss);

  if (address) {
    int disasm(10);
    iss >> disasm;
    for (int idx = 0; idx != disasm; ++idx) {
      address = disassemble(std::cout, ::GetCurrentProcess(), address, nullptr);
    }
  }
  return result;
}
#endif // DISASM

/**
 * Find a symbol
 */
bool SymExplorer::find(std::istream &iss) {
  bool result = false;
  std::string pattern;
  if (iss >> pattern && pattern[0] == '"') {
    appendQuotedString(pattern, iss);
  }
  try {
    enumRegex.assign(pattern);

    result = eng_.EnumSymbols(baseAddress_, nullptr, enumSymbolsCallback,
                              (PVOID)this);
  } catch (std::exception &ex) {
    std::cout << "Error: " << ex.what() << std::endl;
    result = false;
  }
  return result;
}

/**
 * Set hex output mode
 */
bool SymExplorer::hex(std::istream & /*iss*/) {
  mode_ = eHex;
  std::cout << mode_;
  return true;
}

/**
 * Display details for specified type index
 */
bool SymExplorer::index(std::istream &iss) {
  bool result(false);
  DWORD const index = getTypeIndex(iss);

  union {
    DWORD64 value;
    VOID *pointer;
    TI_FINDCHILDREN_PARAMS children;
    VARIANT variant;
  } info;
  for (int type = 0; type != IMAGEHLP_SYMBOL_TYPE_INFO_MAX; ++type) {
    if (type == TI_FINDCHILDREN) {
      info.children.Count = 1;
      info.children.Start = 0;
    } else {
      info.value = 0;
    }
    if (eng_.GetTypeInfo(baseAddress_, index, IMAGEHLP_SYMBOL_TYPE_INFO(type),
                         &info)) {
      switch (type) {
      case TI_GET_SYMTAG: {
        const auto tag{static_cast<enum SymTagEnum>(info.value)};
        std::cout << "Tag: " << info.value << " " << tag << std::endl;
        break;
      }
      case TI_GET_SYMNAME: {
        auto *const pName = static_cast<wchar_t *>(info.pointer);
        std::cout << "Name: " << or2::strFromWchar(pName) << std::endl;
        LocalFree(pName); // By experiment the memory comes from LocalAlloc!
        break;
      }
      case TI_GET_LENGTH:
        std::cout << "Length: " << info.value << std::endl;
        break;
      case TI_GET_TYPE:
        std::cout << "Type: " << info.value << std::endl;
        break;
      case TI_GET_TYPEID:
        std::cout << "Type ID: " << info.value << std::endl;
        break;
      case TI_GET_BASETYPE: {
        const auto basicType{static_cast<enum BasicType>(info.value)};
        std::cout << "Base Type: " << info.value << " " << basicType
                  << std::endl;
        break;
      }
      case TI_GET_ARRAYINDEXTYPEID:
        std::cout << "Array Type ID: " << info.value << std::endl;
        break;
      case TI_FINDCHILDREN:
        std::cout << "Find (First) Child: " << info.children.ChildId[0]
                  << std::endl;
        break;
      case TI_GET_DATAKIND: {
        const auto kind{static_cast<enum DataKind>(info.value)};
        std::cout << "Data Kind: " << info.value << " " << kind << std::endl;
        break;
      }
      case TI_GET_ADDRESSOFFSET:
        std::cout << "Address Offset: " << (PVOID)info.value << std::endl;
        break;
      case TI_GET_OFFSET:
        std::cout << "Offset: " << (PVOID)info.value << std::endl;
        break;
      case TI_GET_VALUE:
        std::cout << "Value: " << info.variant << std::endl;
        break;
      case TI_GET_COUNT:
        std::cout << "Count: " << info.value << std::endl;
        break;
      case TI_GET_CHILDRENCOUNT:
        std::cout << "Children Count: " << info.value << std::endl;
        break;
      case TI_GET_BITPOSITION:
        std::cout << "Bit Position: " << info.value << std::endl;
        break;
      case TI_GET_VIRTUALBASECLASS:
        std::cout << "Virtual Base Class: " << info.value << std::endl;
        break;
      case TI_GET_VIRTUALTABLESHAPEID:
        std::cout << "Virtual Table Shape ID: " << info.value << std::endl;
        break;
      case TI_GET_VIRTUALBASEPOINTEROFFSET:
        std::cout << "Base Pointer Offset: " << info.value << std::endl;
        break;
      case TI_GET_CLASSPARENTID:
        std::cout << "Parent ID: " << info.value << std::endl;
        break;
      case TI_GET_NESTED:
        std::cout << "Nested: " << info.value << std::endl;
        break;
      case TI_GET_SYMINDEX:
        std::cout << "Index: " << info.value << std::endl;
        break;
      case TI_GET_LEXICALPARENT:
        std::cout << "Lexical Parent: " << info.value << std::endl;
        break;
      case TI_GET_ADDRESS:
        std::cout << "Address: " << (PVOID)info.value << std::endl;
        break;
      case TI_GET_THISADJUST:
        std::cout << "This Adjust: " << info.value << std::endl;
        break;
      case TI_GET_UDTKIND: {
        const auto kind{static_cast<UdtKind>(info.value)};
        std::cout << "UDT Kind: " << info.value << " " << kind << std::endl;
        break;
      }
      case TI_IS_EQUIV_TO:
        std::cout << "Equiv To: " << info.value << std::endl;
        break;
      case TI_GET_CALLING_CONVENTION: {
        const auto call{static_cast<enum CV_call_e>(info.value)};
        std::cout << "Calling Convention: " << info.value << " " << call
                  << std::endl;
        break;
      }
      case TI_IS_CLOSE_EQUIV_TO:
        std::cout << "Close Equivalent To: " << info.value << std::endl;
        break;
      case TI_GTIEX_REQS_VALID:
        std::cout << "Reqs Valid: " << info.value << std::endl;
        break;
      case TI_GET_VIRTUALBASEOFFSET:
        std::cout << "Virtual Base Offset: " << (PVOID)info.value << std::endl;
        break;
      case TI_GET_VIRTUALBASEDISPINDEX:
        std::cout << "Virtual Base Dispatch Index: " << info.value << std::endl;
        break;
      case TI_GET_IS_REFERENCE:
        std::cout << "Is Reference: " << info.value << std::endl;
        break;
#ifdef TI_GET_INDIRECTVIRTUALBASECLASS // Not in older dbghelp.h
      case TI_GET_INDIRECTVIRTUALBASECLASS:
        std::cout << "Indirect Virtual Base Class: " << info.value << std::endl;
        break;
#endif                             // TI_GET_INDIRECTVIRTUALBASECLASS
#ifdef TI_GET_VIRTUALBASETABLETYPE // Not in older dbghelp.h
      case TI_GET_VIRTUALBASETABLETYPE:
        std::cout << "Virtual Base Table Type: " << info.value << std::endl;
        break;
#endif // TI_GET_VIRTUALBASETABLETYPE
      default:
        std::cout << "Result(" << type << "): " << info.value << std::endl;
        break;
      }
      result = true;
    }
  }
  if (!result) {
    std::cout << "No info returned for " << index << std::endl;
  }
  return result;
}

/**
 * Load a DLL.
 */
bool SymExplorer::load(std::istream &iss) {
  std::string module;
  if (!(iss >> module)) {
    return false;
  }
  return load(module);
}

bool SymExplorer::load(std::string const &module) {
  if (baseAddress_) {
    eng_.UnloadModule64(baseAddress_);
    ::FreeLibrary(hMod_);
    baseAddress_ = 0;
    hMod_ = nullptr;
  }

  hMod_ = ::LoadLibraryEx(module.c_str(), nullptr, DONT_RESOLVE_DLL_REFERENCES);
  if (nullptr == hMod_) {
    cout << "Unable to load " << module << " - error: " << GetLastError()
         << endl;
    return false;
  }
  baseAddress_ = (DWORD64)hMod_;

  const std::string file_name =
      GetModuleFileNameWrapper(GetCurrentProcess(), hMod_);

  bool const bRet =
      eng_.LoadModule64(nullptr, file_name.c_str(), nullptr, baseAddress_, 0);
  if (bRet) {
    std::cout << "Loaded " << module << " at " << hMod_;
    DbgInit<IMAGEHLP_MODULE64> ModuleInfo;
    (void)eng_.GetModuleInfo64(baseAddress_, &ModuleInfo);
    if (ModuleInfo.SymType == SymNone)
      std::cout << " (no symbols)";
    else if (!ModuleInfo.TypeInfo)
      std::cout << " (no types)";
    std::cout << endl;
  } else {
    std::cout << "Unable to load module information" << std::endl;
  }

  return bRet;
}

/**
 * Display local variables at <addr>.
 */
bool SymExplorer::locals(std::istream &iss) {
  bool result(false);
  DWORD64 const address = getSymbolAddress(iss);

  struct CallBack : public SymbolEngine::EnumLocalCallBack {
    explicit CallBack(hexmode mode) : mode_(mode) {}

    bool operator()(SymbolEngine const &symEng,
                    PSYMBOL_INFO pSymInfo) override {
      std::string name(pSymInfo->Name, pSymInfo->NameLen);
      symEng.decorateName(name, pSymInfo->ModBase, pSymInfo->TypeIndex);
      std::cout << " " << name << " Flags: " << std::hex << pSymInfo->Flags
                << mode_;
      if (pSymInfo->Flags) {
        std::cout << " (" << expandFlags(pSymInfo->Flags) << ") ";
      }
      std::cout << " Index: " << pSymInfo->Index << std::endl;
      return true;
    }

    hexmode mode_;
  };

  CallBack callback(mode_);
  result = eng_.enumLocalVariables(address, 0, callback);
  if (!result) {
    std::cout << "No locals found" << std::endl;
  }
  return result;
}

/**
 * show the symopt settings
 */
bool SymExplorer::symopt(std::istream &iss) {
  bool result(true);
  DWORD const options = SymGetOptions();
  DWORD newoptions(options);

  char op('\0');

  if (iss >> std::hex >> op) {
    if (op == '+') {
      DWORD change(0);
      iss >> change;
      newoptions |= change;
    } else if (op == '-') {
      DWORD change(0);
      iss >> change;
      newoptions &= ~change;
    } else {
      DWORD change(0);
      iss.putback(op);
      if (iss >> change)
        newoptions = change;
      else {
        std::cout << "Unknown operation: '" << op << "'" << std::endl;
        result = false;
      }
    }
  } else if (op != '\0') {
    std::cout << "Unknown operation: expect number, +number or -number"
              << std::endl;
    result = false;
  }

  if (options != newoptions) {
    SymSetOptions(newoptions);
  }

  std::cout << "Options: " << std::hex << newoptions << mode_;

#define SHOW(X)                                                                \
  if (newoptions & X)                                                          \
  std::cout << " " << #X

  SHOW(SYMOPT_CASE_INSENSITIVE);
  SHOW(SYMOPT_UNDNAME);
  SHOW(SYMOPT_DEFERRED_LOADS);
  SHOW(SYMOPT_NO_CPP);
  SHOW(SYMOPT_LOAD_LINES);
  SHOW(SYMOPT_OMAP_FIND_NEAREST);
  SHOW(SYMOPT_LOAD_ANYTHING);
  SHOW(SYMOPT_IGNORE_CVREC);
  SHOW(SYMOPT_NO_UNQUALIFIED_LOADS);
  SHOW(SYMOPT_FAIL_CRITICAL_ERRORS);
  SHOW(SYMOPT_EXACT_SYMBOLS);
  SHOW(SYMOPT_ALLOW_ABSOLUTE_SYMBOLS);
  SHOW(SYMOPT_IGNORE_NT_SYMPATH);
  SHOW(SYMOPT_INCLUDE_32BIT_MODULES);
  SHOW(SYMOPT_PUBLICS_ONLY);
  SHOW(SYMOPT_NO_PUBLICS);
  SHOW(SYMOPT_AUTO_PUBLICS);
  SHOW(SYMOPT_NO_IMAGE_SEARCH);
  SHOW(SYMOPT_SECURE);
  SHOW(SYMOPT_NO_PROMPTS);
  SHOW(SYMOPT_OVERWRITE);
  SHOW(SYMOPT_IGNORE_IMAGEDIR);
  SHOW(SYMOPT_FLAT_DIRECTORY);
  SHOW(SYMOPT_FAVOR_COMPRESSED);
  SHOW(SYMOPT_ALLOW_ZERO_ADDRESS);
#ifdef SYMOPT_DISABLE_SYMSRV_AUTODETECT // Not in older dbghelp.h
  SHOW(SYMOPT_DISABLE_SYMSRV_AUTODETECT);
#endif // SYMOPT_DISABLE_SYMSRV_AUTODETECT
  SHOW(SYMOPT_DEBUG);

#undef SHOW

  std::cout << std::endl;

  return result;
}

/**
 * Find a type
 */
bool SymExplorer::type(std::istream &iss) {
  bool result = false;
  std::string pattern;
  if (iss >> pattern && pattern[0] == '"') {
    appendQuotedString(pattern, iss);
  }
  try {
    enumRegex.assign(pattern);

    result = eng_.EnumTypes(baseAddress_, enumTypesCallback, (PVOID)this);
  } catch (std::exception &ex) {
    std::cout << "Error: " << ex.what() << std::endl;
    result = false;
  }
  return result;
}

/**
 * Display UDT for specified type index
 */
bool SymExplorer::udt(std::istream &iss) {
  bool result(false);
  DWORD index = getTypeIndex(iss);

  DWORD64 tag(0);
  eng_.GetTypeInfo(baseAddress_, index, TI_GET_SYMTAG, &tag);

  // Dereference bases and typedefs
  if ((tag == SymTagBaseClass) || (tag == SymTagTypedef)) {
    if (tag == SymTagTypedef) {
      wchar_t *name = nullptr;
      eng_.GetTypeInfo(baseAddress_, index, TI_GET_SYMNAME, &name);
      if (name) {
        std::cout << "using " << or2::strFromWchar(name) << " = ";
        LocalFree(name); // By experiment the memory comes from LocalAlloc!
      }
    }
    DWORD typeidx(0);
    if (eng_.GetTypeInfo(baseAddress_, index, TI_GET_TYPEID, &typeidx)) {
      index = typeidx;
      eng_.GetTypeInfo(baseAddress_, index, TI_GET_SYMTAG, &tag);
    }
  }

  if (tag == SymTagEnum) {
    std::cout << "enum ";
  } else if (tag == SymTagUDT) {
    DWORD64 udtKind(0);
    eng_.GetTypeInfo(baseAddress_, index, TI_GET_UDTKIND, &udtKind);
    switch (udtKind) {
    case UdtStruct:
      std::cout << "struct ";
      break;
    case UdtClass:
      std::cout << "class ";
      break;
    case UdtUnion:
      std::cout << "union ";
      break;
    default:
      std::cout << "UDT #" << udtKind << " ";
      break;
    }
  } else if (tag == SymTagPointerType) {
    std::cout << "Pointer";
    DWORD typeidx(0);
    if (eng_.GetTypeInfo(baseAddress_, index, TI_GET_TYPEID, &typeidx)) {
      wchar_t *name = nullptr;
      eng_.GetTypeInfo(baseAddress_, typeidx, TI_GET_SYMNAME, &name);
      if (name) {
        std::cout << " to " << or2::strFromWchar(name);
        LocalFree(name); // By experiment the memory comes from LocalAlloc!
      }
    }
    std::cout << '\n';
  } else {
    std::cout << "Not a UDT type: " << static_cast<enum SymTagEnum>(tag)
              << std::endl;
    return result;
  }

  wchar_t *name = nullptr;
  eng_.GetTypeInfo(baseAddress_, index, TI_GET_SYMNAME, &name);
  if (name) {
    std::cout << or2::strFromWchar(name);
    LocalFree(name); // By experiment the memory comes from LocalAlloc!
  }
  DWORD childCount(0);
  if (eng_.GetTypeInfo(baseAddress_, index, TI_GET_CHILDRENCOUNT,
                       &childCount) &&
      childCount != 0) {
    TI_FINDCHILDREN_PARAMS *children;
    size_t const length = offsetof(TI_FINDCHILDREN_PARAMS, ChildId) +
                          sizeof(children->ChildId) * childCount;
    children = static_cast<TI_FINDCHILDREN_PARAMS *>(::operator new(length));
    children->Count = childCount;
    children->Start = 0;
    if (eng_.GetTypeInfo(baseAddress_, index, TI_FINDCHILDREN, children)) {
      std::vector<enum SymTagEnum> const tags = getTags(children);
      printBaseClasses(children, tags);
      std::cout << " {" << std::endl;
      if (tag == SymTagEnum)
        printValues(children, tags);
      else
        printDataMembers(children, tags);
      std::cout << "};" << std::endl;
      result = true;
    } else {
      std::cout << "No info returned for " << index << std::endl;
    }
    ::operator delete(children);
  }
  return result;
}

std::vector<enum SymTagEnum>
SymExplorer::getTags(TI_FINDCHILDREN_PARAMS *children) {
  std::vector<enum SymTagEnum> result(children->Count);

  for (size_t idx = 0; idx != children->Count; ++idx) {
    (void)eng_.GetTypeInfo(baseAddress_, children->ChildId[idx], TI_GET_SYMTAG,
                           &result[idx]);
  }
  return result;
}

void SymExplorer::printBaseClasses(TI_FINDCHILDREN_PARAMS *const children,
                                   std::vector<enum SymTagEnum> const &tags) {
  string delim = " : ";
  for (size_t idx = 0; idx != children->Count; ++idx) {
    switch (tags[idx]) {
    case SymTagBaseClass: {
      DWORD const child(children->ChildId[idx]);
      std::string name;
      eng_.decorateName(name, baseAddress_, child);
      std::cout << delim << name;
      delim = ", ";
      break;
    }
    default:
      break;
    }
  }
}

void SymExplorer::printDataMembers(TI_FINDCHILDREN_PARAMS *const children,
                                   std::vector<enum SymTagEnum> const &tags) {
  std::string const indent("  ");
  for (size_t idx = 0; idx != children->Count; ++idx) {
    switch (tags[idx]) {
    case SymTagData: {
      DWORD const child(children->ChildId[idx]);
      std::string name;
      eng_.decorateName(name, baseAddress_, child);
      DWORD offset(0);
      if (eng_.GetTypeInfo(baseAddress_, child, TI_GET_OFFSET, &offset)) {
        std::cout << indent << name << "; // " << offset;
        DWORD bitPosition(0);
        if (eng_.GetTypeInfo(baseAddress_, child, TI_GET_BITPOSITION,
                             &bitPosition)) {
          std::cout << ':' << bitPosition;
        }
        std::cout << std::endl;
      }
      break;
    }
    default:
      break;
    }
  }
}

void SymExplorer::printValues(TI_FINDCHILDREN_PARAMS *const children,
                              std::vector<enum SymTagEnum> const &tags) {
  std::string const indent("  ");
  for (size_t idx = 0; idx != children->Count; ++idx) {
    switch (tags[idx]) {
    case SymTagData: {
      DWORD const child(children->ChildId[idx]);
      _variant_t value;
      if (eng_.GetTypeInfo(baseAddress_, child, TI_GET_VALUE, &value)) {
        std::string name("= ");
        eng_.decorateName(name, baseAddress_, child);
        value.ChangeType(VT_I8);
        std::cout << indent << name << value.llVal << "," << std::endl;
      }
      break;
    }
    default:
      break;
    }
  }
}

// Read type index
DWORD SymExplorer::getTypeIndex(std::istream &is) {
  DWORD result(0);
  if (!(is >> result)) {
    std::string name;
    is.clear();
    if (is >> name) {
      if (name[0] == '"') {
        appendQuotedString(name, is);
      }
      DbgInit<SYMBOL_INFO> info;
      if (eng_.GetTypeFromName(baseAddress_, name.c_str(), &info)) {
        result = info.TypeIndex;
      } else {
        throw std::runtime_error("Type " + name + " not found");
      }
    } else {
      throw std::runtime_error("No index suppplied");
    }
  }
  return result;
}

// Read symbol address - either raw hex or a symbol name
DWORD64 SymExplorer::getSymbolAddress(std::istream &is) {
  DWORD64 result(0);
  std::string name;
  is >> name;
  long address(0);
  char dummy(0);

#pragma warning(push)
#pragma warning(disable : 4996)
  if (sscanf(name.c_str(), "%lx%c", &address, &dummy) == 1) {
    result = address;
  } else {
    if (name.length()) {
      DbgInit<SYMBOL_INFO> info;
      if (eng_.SymFromName(name.c_str(), &info)) {
        result = info.Address;
      } else {
        throw std::runtime_error("Symbol " + name + " not found");
      }
    } else {
      throw std::runtime_error("No index suppplied");
    }
  }
#pragma warning(pop)
  return result;
}

/**
 * Process input commands
 */
int SymExplorer::run(std::istream &is) {
  std::cout.setf(std::ios::showbase);

  std::string lbufr;
  int ret(0);
  SetConsoleCtrlHandler(reinterpret_cast<PHANDLER_ROUTINE>(CtrlHandler), TRUE);
  while (std::cout << prompt_, std::getline(is, lbufr)) {
    std::istringstream iss(lbufr);
    iss.unsetf(std::ios::dec); // Allow oct, dec and hex input
    std::string command;
    if (iss >> command) {
      FuncMap::const_iterator const it = funcMap_.find(command);
      if (it == funcMap_.end()) {
        std::cout << "Command not found -- use help" << std::endl;
        ++ret;
      } else if (it->second == 0) {
        break;
      } else {
        PFunc const pFunc = it->second;
        try {
          ctrlc_ = false;
          if (!(this->*pFunc)(iss))
            ++ret;
        } catch (std::exception &ex) {
          std::cout << "Error: " << ex.what() << std::endl;
          ++ret;
        }
      }
    }
  }

  return ret;
}

BOOL SymExplorer::CtrlHandler(DWORD fdwCtrlType) {
  BOOL ret(false);
  switch (fdwCtrlType) {
  // Handle the CTRL-C signal.
  case CTRL_C_EVENT:
    ctrlc_ = true;
    ret = TRUE;
    break;
  default:
    break;
  }
  return ret;
}

///////////////////////////////////////////////////////////////////////////
// Main program

int main(int argc, char **argv) {
  SetErrorMode(SEM_FAILCRITICALERRORS);
  std::string prompt("cmd> ");
  // process arguments
  Options options(szRCSID);
  options.set("prompt", &prompt, "prompt to use for input");
  options.setArgs(0, 1, "<image_name>");
  if (!options.process(argc, argv,
                       "Provides access to symbols in the module specified"))
    return 1;

  int ret = 0;
  try {
    SymExplorer app(prompt);

    if (options.begin() != options.end()) {
      if (!app.load(*options.begin()))
        ret++;
    }
    if (ret == 0)
      ret = app.run(std::cin);
    if (ret != 0) {
      std::cout << ret << " error" << (ret == 1 ? "" : "s") << std::endl;
    }
  } catch (std::exception &ex) {
    std::cerr << "Unexpected error: " << ex.what() << std::endl;
    ret = 1;
  }

  return ret;
}
