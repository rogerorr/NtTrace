#ifndef ENTRYPOINT_H_
#define ENTRYPOINT_H_

/**
@file
    Manage a single entry point for NtTrace.

    @author Roger Orr <rogero@howzatt.co.uk>

    Copyright &copy; 2002,2015.
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

    $Revision: 2622 $
*/

// $Id: EntryPoint.h 2622 2025-03-08 17:07:47Z roger $

#include <windows.h>

#include <iosfwd>
#include <map>
#include <set>
#include <string>
#include <vector>

//////////////////////////////////////////////////////////////////////////
// Forward Reference
struct NtCall;

//////////////////////////////////////////////////////////////////////////
// Possible distinct argument types
enum ArgType {
  argULONG_PTR = 0, // also the default
  argULONG,
  argULONGLONG, // two adjacent dwords in 32bit, one qword in 64bit
  argENUM,
  argMASK,
  argBOOLEAN,
  argBYTE,
  argHANDLE,
  argPOINTER,
  argPOBJECT_ATTRIBUTES,
  argPUNICODE_STRING,
  argPHANDLE,
  argPBYTE,
  argPUSHORT,
  argPULONG,
  argACCESS_MASK,
  argPCLIENT_ID,
  argPIO_STATUS_BLOCK,
  argPLARGE_INTEGER,
  argPLPC_MESSAGE,
  argPFILE_BASIC_INFORMATION,
  argPFILE_NETWORK_OPEN_INFORMATION,
  argPRTL_USER_PROCESS_PARAMETERS
};

enum ArgAttributes {
  argNONE = 0,
  argIN = 1,
  argOUT = 2,
  argOPTIONAL = 4,
  argCONST = 8,
  argRESERVED = 16,
};

struct Argument {
  Argument() = default;
  Argument(ArgType argType, std::string const &argTypeName,
           std::string const &name, ArgAttributes attributes)
      : argType_(argType), argTypeName_(argTypeName), name_(name),
        attributes_(attributes) {}

#ifdef _M_IX86
  using ARG = DWORD;
#elif _M_X64
  using ARG = DWORD64;
#endif

  /** Show the argument for the given process with the specified value. */
  void showArgument(std::ostream &os, HANDLE hProcess, ARG value, bool returnOk,
                    bool dup) const;

  /** true if argument is output-only */
  bool outputOnly() const;

  ArgType argType_{argULONG_PTR};     // Argument type for processing
  std::string argTypeName_{"ULONG"};  // Actual argument type
  std::string name_{"Unknown"};       // formal name of argument
  ArgAttributes attributes_{argNONE}; // Optional attributes
  bool dummy_{}; // True if this is a dummy argument (2nd part of 64bit item on
                 // 32bit Windows)
};

enum ReturnType {
  retNTSTATUS = 0, // also the default
  retVOID,
  retPVOID,
  retULONG,
  retULONG_PTR,
};

class EntryPoint {
public:
  using Typedefs = std::map<std::string, std::string>;

  explicit EntryPoint(std::string const &name, std::string const &category)
      : name_(name), category_(category), disabled_(category[0] == '-') {
    if (disabled_) {
      this->category_.erase(0, 1);
    }
  }

  std::string const &getName() const { return name_; }

  std::string const &getExported() const { return exported_; }

  void setExported(std::string const &value) { exported_ = value; }

  std::string const &getCategory() const { return category_; }

  bool isDisabled() const { return disabled_; }

  size_t getArgumentCount() const { return arguments_.size(); }

  void setArgumentCount(size_t newSize) { arguments_.resize(newSize); }

  Argument const &getArgument(size_t idx) const { return arguments_[idx]; }

  void setArgument(int argNum, std::string const &argType,
                   std::string const &variableName, ArgAttributes attributes,
                   Typedefs const &typedefs);

  void setDummyArgument(int argNum, ArgAttributes attributes);

  void setReturnType(std::string const &type, Typedefs const &typedefs);

  ReturnType getReturnType() const { return retType_; }

  static bool readEntryPoints(std::istream &cfgFile,
                              std::set<EntryPoint> &entryPoints,
                              Typedefs &typedefs, std::string &target);

  void writeExport(std::ostream &os) const;

  /** Set a trap for this entry point in the target process */
  NtCall setNtTrap(HANDLE hProcess, HMODULE hTargetDll, bool bPreTrace,
                   DWORD dllOffset, bool verbose);

  /** Clear the trap for this entry in the target process */
  bool clearNtTrap(HANDLE hProcess, NtCall const &ntcall) const;

  void setAddress(unsigned char *brkptAddress) {
    targetAddress_ = brkptAddress;
  }

  unsigned char *getAddress() const { return targetAddress_; }

  void setPreSave(unsigned char *preSaveAddress) { preSave_ = preSaveAddress; }

  unsigned char *getPreSave() const { return preSave_; }

  void doPreSave(HANDLE hProcess, HANDLE hThread, CONTEXT const &Context);

  void trace(std::ostream &os, HANDLE hProcess, HANDLE hThread,
             CONTEXT const &Context, bool bNames, bool bStackTrace,
             bool before) const;

  bool operator<(EntryPoint const &rhs) const;

  static void stackTrace(std::ostream &os, HANDLE hProcess, HANDLE hThread);

private:
  std::string name_;                // name of entry point
  std::string exported_;            // (optional) exported name for entry point
  std::string category_;            // category of entry point
  bool disabled_{};                 // this entry point is disabled
  std::vector<Argument> arguments_; // vector of arguments
  ReturnType retType_{};            // Return type
  std::string retTypeName_;         // full name of return type
  unsigned char *targetAddress_{};
  unsigned char *preSave_{}; // address of pre-save (for X64 fast-call)
  DWORD ssn_{};              // System Service Number
                             // Used to set Eax/Rax to pre-call breakpoint

  NtCall insertBrkpt(HANDLE hProcess, unsigned char *address,
                     unsigned int offset, unsigned char *setssn);
};

using EntryPointSet = std::set<EntryPoint>;

//////////////////////////////////////////////////////////////////////////
// Our data structure for an NT call
struct NtCall {
  EntryPoint *entryPoint_{}; // Pointer into EntryPointMap

  size_t nArgs_{}; // Number of arguments

  enum TrapType { trapContinue, trapReturn, trapReturn0, trapJump };
  TrapType trapType_{};
  DWORD jumpTarget_{}; // used for trapJump
};

#endif // ENTRYPOINT_H_
