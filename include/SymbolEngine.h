#ifndef OR2_SYMBOLENGINE_H
#define OR2_SYMBOLENGINE_H

/**@file

   Wrapper for DbgHelper to provide common utility functions for processing
   Microsoft PDB information.

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

    $Revision: 2427 $
*/

// $Id: SymbolEngine.h 2427 2024-06-16 15:33:16Z roger $

#include <iosfwd>
#include <string>

#include "..\include\DbgHelper.h"

namespace or2 {

/** Symbol Engine wrapper to assist with processing PDB information */
class SymbolEngine : public DbgHelper {
public:
  /** Construct wrapper for specified process */
  SymbolEngine(HANDLE hProcess);

  /** Destroy wrapper */
  ~SymbolEngine();

  /**
   * GetModuleBase: try to get information about a module being loaded.
   * @param dwAddress the target address
   * @return the base address of the module, or zero if not identified
   */
  DWORD64 GetModuleBase(DWORD64 dwAddress) const;

  /** Print address to a stream, return true if information cacheable */
  bool printAddress(DWORD64 address, std::ostream &os) const;

  /** Print inline address to a stream */
  void printInlineAddress(DWORD64 address, DWORD inline_context,
                          std::ostream &os) const;

  /** Convert address to a string */
  std::string addressToName(DWORD64 address) const;

  /** Convert pointer to a string */
  std::string addressToName(PVOID pointer) const;

  /** Convert inline address to a string */
  std::string inlineToName(DWORD64 address, DWORD inline_context) const;

  /** Provide a stack trace for the 'origContext' using current depth and params
   */
  void StackTrace(HANDLE hThread, const CONTEXT &context,
                  std::ostream &os) const;

  /** get context for the current thread, correcting the stack frame to the
   * caller */
#ifdef _M_IX86
  static BOOL GetCurrentThreadContext(CONTEXT *pContext);
#else
  static void(WINAPI *GetCurrentThreadContext)(PCONTEXT pContext);

#endif // _M_IX86

  /** Provide an stack trace of the structured exception records starting at
   * 'ExceptionList' using current seh depth */
  void SEHTrace(PVOID ExceptionList, std::ostream &os) const;

  /** Set true to show line numbers if possible */
  void setLines(bool value);
  /** Get whether line numbers are being shown if possible */
  bool getLines() const;

  /** Set true to show parameters */
  void setParams(bool value);
  /** Get whether to show parameters */
  bool getParams() const;

  /** Set true to show variables */
  void setVariables(bool value);
  /** Get whether to show variables */
  bool getVariables() const;

  /** Set stack depth for walkbacks */
  void setMaxDepth(int value);
  /** Get stack depth for walkbacks */
  int getMaxDepth() const;

  /** Set skip count for stack walkbacks */
  void setSkipCount(int value);
  /** Get skip count for stack walkbacks */
  int getSkipCount() const;

  /** Set SEH stack depth for walkbacks */
  void setSehDepth(int value);
  /** Get SEH stack depth for walkbacks */
  int getSehDepth() const;

  /** Hack to try and get first catch handler for MSVC exception */
  bool findMsvcCppHandler(PVOID sehHandler, PVOID *msvcHandler) const;

  /** Attempt to find type information for MSVC C++ throw parameter */
  void showMsvcThrow(std::ostream &ofs, PVOID throwInfo, PVOID base) const;

  /** ReadMemory from the target process */
  bool ReadMemory(LPCVOID lpBaseAddress, // base of memory area
                  LPVOID lpBuffer,       // data buffer
                  SIZE_T nSize) const;   // number of bytes to read

  /** decorate name for a symbol with the full data type */
  BOOL decorateName(std::string &name, ULONG64 ModBase, ULONG TypeIndex) const;

  /** call back interface for enumerating locals */
  class EnumLocalCallBack {
  public:
    /** callback function called for each enumerated local */
    virtual bool operator()(SymbolEngine const &eng,
                            struct _SYMBOL_INFO *pSymInfo) = 0;
    virtual ~EnumLocalCallBack() {}
  };

  /** enumerate local variables at an address */
  BOOL enumLocalVariables(DWORD64 codeOffset, DWORD64 frameOffset,
                          EnumLocalCallBack &cb) const;

  /** Write a simple mini-dump for an exception in the current thread */
  BOOL dumpSelf(std::string const &miniDumpFile,
                EXCEPTION_POINTERS *ExceptionInfo);

  /** Read a string from the target */
  std::string getString(PVOID address, BOOL unicode,
                        DWORD maxStringLength) const;

  /** Returns whether address points to executable code */
  bool isExecutable(DWORD64 address) const;

private:
  SymbolEngine(SymbolEngine const &);
  SymbolEngine &operator=(SymbolEngine const &);

  bool showLines;     // true to show lines
  bool showParams;    // true to show parameters
  bool showVariables; // true to show variables
  int maxStackDepth;  // max stack depth to show, -1 for all
  int skipCount;      // stack frames to skip
  int maxSehDepth;    // max SEH stack depth to show

  struct Impl;
  Impl *pImpl;
};

} // namespace or2

#endif // OR2_SYMBOLENGINE_H
