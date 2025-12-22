#ifndef ENTRYPOINT_H_
#define ENTRYPOINT_H_

/**
@file

  Manage a single entry point for NtTrace.

  @author Roger Orr mailto:rogero@howzatt.co.uk
  Bug reports, comments, and suggestions are always welcome.

  Copyright &copy; 2002-2025 under the MIT license:

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

  $Revision: 3014 $
*/

// $Id: EntryPoint.h 3014 2025-12-22 11:29:33Z roger $

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
  Argument(ArgType argType, std::string argTypeName, std::string name,
           ArgAttributes attributes)
      : argType_(argType), argTypeName_(std::move(argTypeName)),
        name_(std::move(name)), attributes_(attributes) {}

#ifdef _M_IX86
  using ARG = DWORD;
#elif _M_X64
  using ARG = DWORD64;
#endif

  /** Show the argument for the given process with the specified value. */
  void showArgument(std::ostream &os, HANDLE hProcess, ARG value, bool returnOk,
                    bool dup, bool showName) const;

  /** true if argument is output-only */
  bool outputOnly() const;

  /** true if argument is second part of 64bit item on 32bit Windows */
  void setDummy(bool value) { dummy_ = value; }

  bool isDummy() const { return dummy_; }

  /** Write argument to the output stream */
  void printOn(std::ostream &os) const;

private:
  ArgType argType_{argULONG_PTR};     // Argument type for processing
  std::string argTypeName_{"ULONG"};  // Actual argument type
  std::string name_{"Unknown"};       // formal name of argument
  ArgAttributes attributes_{argNONE}; // Optional attributes
  bool dummy_{}; // True if this is a dummy argument (2nd part of 64bit item on
                 // 32bit Windows)
};

inline std::ostream &operator<<(std::ostream &os, const Argument &argument) {
  argument.printOn(os);
  return os;
}

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

  explicit EntryPoint(std::string name, std::string category)
      : name_(std::move(name)), category_(std::move(category)),
        disabled_(category_[0] == '-'), optional_(category_[0] == '?') {
    if (disabled_ || optional_) {
      this->category_.erase(0, 1);
    }
  }

  std::string const &getName() const { return name_; }

  std::string const &getExported() const { return exported_; }

  void setExported(std::string const &value) { exported_ = value; }

  std::string const &getCategory() const { return category_; }

  bool isDisabled() const { return disabled_; }

  bool isOptional() const { return optional_; }

  size_t getArgumentCount() const { return arguments_.size(); }

  void setArgumentCount(size_t newSize) { arguments_.resize(newSize); }

  Argument const &getArgument(size_t idx) const { return arguments_[idx]; }

  void setArgument(size_t argNum, ArgType eArgType, std::string const &argType,
                   std::string const &variableName, ArgAttributes attributes);

  void setDummyArgument(size_t argNum, ArgAttributes attributes);

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
  bool optional_{};                 // this entry point is optional
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
