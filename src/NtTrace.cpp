/*
NAME
  NtTrace.cpp

DESCRIPTION
  Process to provide tracing for NT 'native' API

AUTHOR
  Roger Orr mailto:rogero@howzatt.co.uk
  Bug reports, comments, and suggestions are always welcome.

COPYRIGHT
  Copyright (C) 2002, 2025 under the MIT license:

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

EXAMPLE
  NtTrace fred.exe
  - or -
  NtTrace 1234
*/

static char const szRCSID[] =
    "$Id: NtTrace.cpp 3025 2025-12-22 20:14:03Z roger $";

#ifdef _M_X64
#include <ntstatus.h>
#define WIN32_NO_STATUS
#endif

#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <sys/timeb.h>
#include <vector>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <psapi.h> // LOAD_DLL_DEBUG_INFO does not always give us lpImageName

// or2 includes
#include "../include/DebugPriv.h"
#include "../include/DisplayError.h"
#include "../include/MsvcExceptions.h"
#include "../include/NtDllStruct.h"
#include "../include/Options.h"
#include "../include/ProcessHelper.h"
#include "../include/ReadInt.h"
#include "../include/SimpleTokenizer.h"
#include <GetFileNameFromHandle.h>
#include <GetModuleBase.h>
#include <SymbolEngine.h>

#include "DebugDriver.h"
#include "EntryPoint.h"
#include "ShowData.h"

using namespace showData;
using namespace or2;

#pragma comment(lib, "advapi32") // for privilege stuff
#pragma comment(lib, "psapi")

//////////////////////////////////////////////////////////////////////////
// Local classes

/** Debugger event handler for trapped entry points */
class TrapNtDebugger : public Debugger {
public:
  /**
   * Construct a debugger
   * @param os the output stream to write to
   */
  TrapNtDebugger(std::ostream &os) : os_(os) {}

  // callbacks on events
  void OnException(DWORD processId, DWORD threadId, HANDLE hProcess,
                   HANDLE hThread, EXCEPTION_DEBUG_INFO const &DebugEvent,
                   DWORD *pContinueExecution) override;
  void OnCreateThread(DWORD processId, DWORD threadId,
                      CREATE_THREAD_DEBUG_INFO const &CreateThread) override;
  void
  OnCreateProcess(DWORD processId, DWORD threadId,
                  CREATE_PROCESS_DEBUG_INFO const &CreateProcessInfo) override;
  void OnExitThread(DWORD processId, DWORD threadId,
                    EXIT_THREAD_DEBUG_INFO const &ExitThread) override;
  void OnExitProcess(DWORD processId, DWORD threadId, HANDLE hProcess,
                     EXIT_PROCESS_DEBUG_INFO const &ExitProcess) override;
  void OnLoadDll(DWORD processId, DWORD threadId, HANDLE hProcess,
                 LOAD_DLL_DEBUG_INFO const &LoadDll) override;
  void OnUnloadDll(DWORD processId, DWORD threadId,
                   UNLOAD_DLL_DEBUG_INFO const &UnloadDll) override;
  void
  OnOutputDebugString(DWORD processId, DWORD threadId, HANDLE hProcess,
                      OUTPUT_DEBUG_STRING_INFO const &DebugString) override;
  bool Active() override { return bActive_; }

  /**
   * Set the 'log dlls' flag.
   * @param b the new value: if true dll load/unload will be ignored
   */
  void setLogDlls(bool b) { bLogDlls_ = b; }

  /** Get the 'log dlls' flag */
  bool getLogDlls() const { return bLogDlls_; }

  /**
   * Set the 'noexception' flag.
   * @param b the new value: if true exceptions will be ignored
   */
  void setNoException(bool b) { bNoExcept_ = b; }

  /**
   * Set the 'nothread' flag.
   * @param b the new value: if true thread creation and exit will be ignored
   */
  void setNoThread(bool b) { bNoThread_ = b; }

  /**
   * Set the 'show loader snaps' flag.
   * @param b the new value: if true loader snaps will be logged
   */
  void setShowLoaderSnaps(bool b) { bShowLoaderSnaps_ = b; }

  /**
   * Set the categories
   * @param category a comma-delimited list of categories to trace
   */
  void setCategory(std::string const &category) {
    std::vector<std::string> vec;
    SimpleTokenizer(category, &vec, ',');
    categories_.insert(vec.begin(), vec.end());
  }

  /**
   * Set the function-name filter
   * @param filter a comma-delimited list of function names to filter on
   */
  void setFilter(std::string const &filter) {
    SimpleTokenizer(filter, &filters_, ',');
    // starting with "-" sets an inverse filter
    inverseFilter_ = (filters_.size() > 0 && filters_[0].size() > 0 &&
                      filters_[0][0] == '-');
    if (inverseFilter_) {
      filters_[0].erase(0, 1);
    }
  }

  /** initialise the debugger */
  bool initialise();

  /** List categories, if category supplied was '?' and return true. Otherwise
   * return false */
  bool listCategories();

  /** Mapping NtXxx names to offsets within target DLL */
  using Offsets = std::map<std::string, DWORD>;

  /** Set orderly close down on Ctrl+C */
  void setCtrlC();

private:
  bool bLogDlls_{true};
  bool bNoExcept_{false};
  bool bNoThread_{false};
  bool bShowLoaderSnaps_{false};
  std::ostream &os_;

  bool bActive_{true};
  static TrapNtDebugger *ctrlcTarget_;
  static BOOL __stdcall CtrlHandler(DWORD fdwCtrlType);

  std::map<DWORD, HANDLE> processes_; // map of all active child processes
  EntryPointSet entryPoints_;         // Set of all entry points
  EntryPoint::Typedefs typedefs_;

  Offsets offsets_; // Offsets of potential Nt functions in the target Dll (not
                    // needed for NtDll)

  void populateOffsets();

  using NTCALLS = std::map<LPVOID, NtCall>;
  NTCALLS NtCalls_;   // Complete list of all the calls we're tracking
  NTCALLS NtPreSave_; // Pre save list of all the calls we're tracking

  HMODULE BaseOfNtDll_ = nullptr; // base of NTDLL.DLL

  std::string target_; // name of target DLL (blank => default)
  HMODULE TargetDll_ =
      nullptr; // handle of the target DLL (by default, NTDLL.DLL)

  std::set<std::string> categories_; // If not empty, categories to trace
  bool inverseFilter_ = false;       // If true, exclude when filtered
  std::vector<std::string>
      filters_; // If not empty, filter for 'active' entry points

  std::set<DWORD> initialised_processes_;

  std::map<DWORD, std::map<PVOID, std::string>> dll_names_;

  bool OnBreakpoint(DWORD processId, DWORD threadId, HANDLE hProcess,
                    HANDLE hThread, LPVOID exceptionAddress);

  void SetDllBreakpoints(HANDLE hProcess);
  void showUnused(std::set<std::string> const &unused, std::string const &name);
  void showModuleNameEx(HANDLE hProcess, PVOID lpModuleBase,
                        HANDLE hFile) const;
  void header(DWORD processId, DWORD threadId);
  bool detachAll();
  bool detach(DWORD processId, HANDLE hProcess);
  void setShowLoaderSnaps(HANDLE hProcess);
};

// static
TrapNtDebugger *TrapNtDebugger::ctrlcTarget_;

///////////////////////////////////////////////////////////////////////////////
// Helper functions
namespace {
///////////////////////////////////////////////////////////////////////////
// Return string for 'now' - substring of asctime + milliseconds
std::string now() {
  struct _timeb timeNow;
  (void)_ftime_s(&timeNow);

  static _timeb lasttime;
  static char seconds[] = "HH:MM:SS";
  if (lasttime.time != timeNow.time) {
    struct tm tm_buf;
    (void)localtime_s(&tm_buf, &timeNow.time);
    strftime(seconds, sizeof(seconds), "%H:%M:%S", &tm_buf);
    lasttime.time = timeNow.time;
  }
  char result[8 + 1 + 3 + 1];
  snprintf(result, sizeof(result), "%s.%03i", seconds,
           static_cast<int>(timeNow.millitm));
  return result;
}

///////////////////////////////////////////////////////////////////////////
// Return string for 'delta time' - seconds + milliseconds (+[ss]s.mmm)
std::string delta() {
  struct _timeb timeNow;
  (void)_ftime_s(&timeNow);

  static _timeb lastTime;

  char result[4 + 1 + 3 + 1];
  result[0] = '\0';

  if (lastTime.time != 0) {
    struct _timeb diff = timeNow;
    if (diff.millitm < lastTime.millitm) {
      diff.millitm += 1000;
      diff.time -= 1;
    }
    diff.time -= lastTime.time;
    diff.millitm -= lastTime.millitm;

    if (diff.time < 0) {
      (void)strcpy_s(result, sizeof(result), "<0");
    } else if (diff.time > 999) {
      (void)strcpy_s(result, sizeof(result), ">999s");
    } else {
      snprintf(result, sizeof(result), "+%i.%03i", static_cast<int>(diff.time),
               static_cast<int>(diff.millitm));
    }
  }
  lastTime = timeNow;
  return result;
}
} // namespace

//////////////////////////////////////////////////////////////////////////
// Local data

static bool bErrorsOnly(false);
static bool bVerbose(false);

static std::set<NTSTATUS> errorCodes;
static bool bNames(false);
static bool bPreTrace(false);
static bool bStackTrace(false);
static bool bTimestamp(false);
static bool bDelta(false);
static bool bPid(false);
static bool bTid(false);
static bool bNewline(false);
static std::string configFile; // override default config file

static std::string exportFile; // Export symbols here once loaded

//////////////////////////////////////////////////////////////////////////
// Set things up...
//
// Load the (fixed address!) NTDLL so we can
// (a) Check for it being loaded in the target process
// (b) Get the addresses of the entry points we want to hook

bool TrapNtDebugger::initialise() {
  BaseOfNtDll_ = LoadLibrary("NTDLL");
  if (BaseOfNtDll_ == nullptr) {
    std::cerr << "Unable to load NTDLL: " << displayError() << std::endl;
    return false;
  }

  // Open the config file
  std::ifstream cfgFile;
  if (configFile.empty()) {
    // First try in current directory
    configFile = "NtTrace.cfg";
    cfgFile.open(configFile.c_str());
    if (!cfgFile) {
      // Fall back to the location of the exe
      char chExeName[MAX_PATH + 1] = "";
      GetModuleFileName(nullptr, chExeName, sizeof(chExeName));
      char *pDelim = strrchr(chExeName, '.');
      size_t namelen = pDelim - chExeName;
// Remove optional architectural suffix from executable name
#ifdef _M_IX86
      static const std::string suffix = "86";
#else
      static const std::string suffix = "64";
#endif // _M_IX86
      if (namelen > suffix.size() &&
          memcmp(chExeName + namelen - suffix.size(), suffix.c_str(),
                 suffix.size()) == 0) {
        namelen -= suffix.size();
      }
      chExeName[namelen] = '\0';

      configFile = std::string(chExeName, namelen) + ".cfg";
      cfgFile.open(configFile.c_str());
    }
  } else {
    cfgFile.open(configFile.c_str());
  }

  if (cfgFile) {
    if (!EntryPoint::readEntryPoints(cfgFile, entryPoints_, typedefs_, target_))
      return false;
  } else {
    std::cerr << "Unable to read configuration from " << configFile
              << std::endl;
    return false;
  }

  if (target_.empty()) {
    TargetDll_ = BaseOfNtDll_;
  } else {
    TargetDll_ = LoadLibrary(target_.c_str());
    if (TargetDll_ == nullptr) {
      std::cerr << "Unable to load " << target_ << ": " << displayError()
                << std::endl;
      return false;
    }
    populateOffsets();
  }

  return true;
}

//////////////////////////////////////////////////////////////////////////
BOOL CALLBACK populateCallback(PSYMBOL_INFO pSymInfo, ULONG /*SymbolSize*/,
                               PVOID UserContext) {
  if (pSymInfo->NameLen <= 3)
    return TRUE;
  char *ptr = pSymInfo->Name;
  size_t len = pSymInfo->NameLen;
  if (ptr[0] == '_') {
    ++ptr;
    --len;
  }
  if (ptr[0] == 'N' && ptr[1] == 't') {
    char const *end = (char const *)memchr(ptr, '@', len);
    if (end) {
      len = end - ptr;
    }
    std::string key(ptr, len);
    const auto offset =
        static_cast<DWORD>(pSymInfo->Address - pSymInfo->ModBase);
    auto &offsets = *static_cast<TrapNtDebugger::Offsets *>(UserContext);
    offsets[key] = offset;
  }

  return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// Populate the 'offsets' collection
void TrapNtDebugger::populateOffsets() {
  or2::SymbolEngine eng(GetCurrentProcess());
  const auto baseAddress(reinterpret_cast<DWORD64>(TargetDll_));
  std::string file_name =
      GetModuleFileNameWrapper(GetCurrentProcess(), TargetDll_);
  if (file_name.empty()) {
    file_name = target_;
  }
  if (0 ==
      eng.LoadModule64(nullptr, file_name.c_str(), nullptr, baseAddress, 0)) {
    std::cerr << "Warning: Unable to load module for " << target_ << " at "
              << TargetDll_ << '\n';
  }
  DbgInit<IMAGEHLP_MODULE64> ModuleInfo;
  if (!eng.GetModuleInfo64(baseAddress, &ModuleInfo) ||
      (ModuleInfo.SymType != SymPdb)) {
    std::cerr << "Warning: No PDB found for '" << target_
              << "' - some entry points may be missing\n";
  }
  eng.EnumSymbols(baseAddress, nullptr, populateCallback, &offsets_);
}

//////////////////////////////////////////////////////////////////////////
// Print common header to trace lines
void TrapNtDebugger::header(DWORD processId, DWORD threadId) {
  if (bTimestamp || bDelta) {
    if (bTimestamp)
      os_ << now();
    if (bTimestamp && bDelta)
      os_ << " ";
    if (bDelta)
      os_ << delta();

    os_ << ": ";
  }

  if (bPid || bTid) {
    os_ << "[";
    if (bPid)
      os_ << std::setw(4) << processId;
    if (bPid && bTid)
      os_ << '/';
    if (bTid)
      os_ << std::setw(4) << threadId;

    os_ << "] ";
  }
}

//////////////////////////////////////////////////////////////////////////
// The heart of NtTrace: if this is one of our added breakpoint exceptions
// then trace the arguments and return code for the entry point.
bool TrapNtDebugger::OnBreakpoint(DWORD processId, DWORD threadId,
                                  HANDLE hProcess, HANDLE hThread,
                                  PVOID exceptionAddress) {
  CONTEXT Context;
  Context.ContextFlags = CONTEXT_FULL;
  if (!GetThreadContext(hThread, &Context)) {
    std::cerr << "Can't get thread context: " << displayError() << std::endl;
    return false; // We couldn't handle this breakpoint
  }

  NTCALLS::const_iterator it = NtPreSave_.find(exceptionAddress);
  if (it != NtPreSave_.end()) {
    it->second.entryPoint_->doPreSave(hProcess, hThread, Context);
    if (bPreTrace) {
      header(processId, threadId);

      it->second.entryPoint_->trace(os_, hProcess, hThread, Context, bNames,
                                    bStackTrace, true);
    }
    return true; // Breakpoint handled
  }
  it = NtCalls_.find(exceptionAddress);
  if (it != NtCalls_.end()) {
#ifdef _M_IX86
    const auto rc{static_cast<NTSTATUS>(Context.Eax)};
#elif _M_X64
    const auto rc{static_cast<NTSTATUS>(Context.Rax)};
#endif
    if (bErrorsOnly && NT_SUCCESS(rc)) {
      // don't trace
    } else if (errorCodes.empty() || (errorCodes.count(rc) > 0)) {
      header(processId, threadId);

      it->second.entryPoint_->trace(os_, hProcess, hThread, Context, bNames,
                                    bStackTrace, false);
    }

    if (it->second.trapType_ == NtCall::trapReturn ||
        it->second.trapType_ == NtCall::trapReturn0) {
      // Fake a return 'n'
#ifdef _M_IX86
      DWORD eip = 0;
      ReadProcessMemory(hProcess, (LPVOID)(Context.Esp), &eip, sizeof(eip), 0);

      Context.Eip = eip;
      Context.Esp += sizeof(eip) + it->second.nArgs_ * sizeof(DWORD);
#elif _M_X64
      DWORD64 rip = 0;
      ReadProcessMemory(hProcess, (LPVOID)(Context.Rsp), &rip, sizeof(rip),
                        nullptr);

      Context.Rip = rip;
      Context.Rsp += sizeof(rip) + it->second.nArgs_ * sizeof(DWORD);
#endif // _M_IX86
    } else if (it->second.trapType_ == NtCall::trapJump) {
      // Fake a jump
#ifdef _M_IX86
      Context.Eip = it->second.jumpTarget_;
#elif _M_X64
      Context.Rip = it->second.jumpTarget_;
#endif // _M_IX86
    }
    Context.ContextFlags = CONTEXT_CONTROL;
    if (!SetThreadContext(hThread, &Context)) {
      os_ << "Can't set thread context: " << displayError() << std::endl;
    }
    return true; // Breakpoint handled
  }
  return false; // Not an NtTrace breakpoint
}

//////////////////////////////////////////////////////////////////////////
void TrapNtDebugger::OnException(DWORD processId, DWORD threadId,
                                 HANDLE hProcess, HANDLE hThread,
                                 EXCEPTION_DEBUG_INFO const &Exception,
                                 DWORD *pContinueFlag) {
  const auto status =
      static_cast<NTSTATUS>(Exception.ExceptionRecord.ExceptionCode);
  if (status == STATUS_BREAKPOINT) {
    if (OnBreakpoint(processId, threadId, hProcess, hThread,
                     Exception.ExceptionRecord.ExceptionAddress)) {
      *pContinueFlag = DBG_CONTINUE;
    } else {
      // Not an NtTrace breakpoint
      header(processId, threadId);
      if (initialised_processes_.insert(processId).second) {
        os_ << "Initial breakpoint" << std::endl;
      } else {
        os_ << "Breakpoint at " << Exception.ExceptionRecord.ExceptionAddress
            << " (" << (Exception.dwFirstChance ? "first" : "last")
            << " chance)" << std::endl;
      }
    }
  } else if (bNoExcept_) {
    // ignore...
  }
#ifdef _M_X64
  else if (status == STATUS_WX86_BREAKPOINT) {
    header(processId, threadId);
    os_ << "WOW64 initialised" << std::endl;
    *pContinueFlag = DBG_CONTINUE;
  }
#endif // _M_X64
  else if (status == EXCEPTION_ACCESS_VIOLATION) {
    // Only defined contents is for an access violation...
    header(processId, threadId);
    os_ << "Access violation at " << Exception.ExceptionRecord.ExceptionAddress
        << ": "
        << (Exception.ExceptionRecord.ExceptionInformation[0] ? "Write to "
                                                              : "Read from ")
        << (PVOID)Exception.ExceptionRecord.ExceptionInformation[1] << " ("
        << (Exception.dwFirstChance ? "first" : "last") << " chance)"
        << std::endl;
    if (bStackTrace)
      EntryPoint::stackTrace(os_, hProcess, hThread);
  } else if (status == STATUS_INVALID_HANDLE) {
    // CloseHandle raises this exception when a process is being debugged
    header(processId, threadId);
    if (Exception.dwFirstChance) {
      os_ << "Exception raised by attempted close of an invalid handle"
          << std::endl;
    } else {
      os_ << "Ignoring unhandled exception from close of an invalid handle"
          << std::endl;
      *pContinueFlag = DBG_CONTINUE;
    }
    if (bStackTrace)
      EntryPoint::stackTrace(os_, hProcess, hThread);
  } else if (Exception.ExceptionRecord.ExceptionCode == MSVC_EXCEPTION) {
    header(processId, threadId);
    os_ << "C++ exception at " << Exception.ExceptionRecord.ExceptionAddress;
    if ((Exception.dwFirstChance) &&
        (Exception.ExceptionRecord.NumberParameters == 3 ||
         Exception.ExceptionRecord.NumberParameters == 4) &&
        (Exception.ExceptionRecord.ExceptionInformation[0] ==
         MSVC_MAGIC_NUMBER1)) {
      if (Exception.ExceptionRecord.ExceptionInformation[1] == 0 &&
          Exception.ExceptionRecord.ExceptionInformation[2] == 0) {
        os_ << " rethrow";
      } else {
        ULONG_PTR base =
            Exception.ExceptionRecord.NumberParameters == 3
                ? 0
                : static_cast<ULONG_PTR>(
                      Exception.ExceptionRecord.ExceptionInformation[3]);
        showThrowType(os_, hProcess,
                      static_cast<ULONG_PTR>(
                          Exception.ExceptionRecord.ExceptionInformation[2]),
                      base);
      }
    }
    os_ << std::endl;
    if (bStackTrace)
      EntryPoint::stackTrace(os_, hProcess, hThread);
  } else if (Exception.ExceptionRecord.ExceptionCode == CLR_EXCEPTION ||
             Exception.ExceptionRecord.ExceptionCode == CLR_EXCEPTION_V4) {
    header(processId, threadId);
    os_ << "CLR exception, HR: " << std::hex
        << (HRESULT)Exception.ExceptionRecord.ExceptionInformation[0]
        << std::dec << std::endl;
  } else if (Exception.ExceptionRecord.ExceptionCode == MSVC_NOTIFICATION) {
    if (Exception.ExceptionRecord.ExceptionInformation[0] == 0x1000) {
      header(processId, threadId);
      os_ << "SetThreadName \"";
      showString(os_, hProcess,
                 (PVOID)Exception.ExceptionRecord.ExceptionInformation[1],
                 FALSE, MAX_PATH);
      os_ << '"' << std::endl;
    } else {
      header(processId, threadId);
      os_ << "MSVC Notification: "
          << (PVOID)Exception.ExceptionRecord.ExceptionInformation[0]
          << std::endl;
    }
  } else if (Exception.ExceptionRecord.ExceptionCode == CLR_NOTIFICATION) {
    header(processId, threadId);
    os_ << "CLR Notification: "
        << (PVOID)Exception.ExceptionRecord.ExceptionInformation[0]
        << std::endl;
  } else {
    header(processId, threadId);
    os_ << "Exception: " << std::hex << Exception.ExceptionRecord.ExceptionCode
        << std::dec << " at " << Exception.ExceptionRecord.ExceptionAddress
        << " (" << (Exception.dwFirstChance ? "first" : "last") << " chance)"
        << std::endl;
    if (bStackTrace)
      EntryPoint::stackTrace(os_, hProcess, hThread);
  }
}

//////////////////////////////////////////////////////////////////////////
void TrapNtDebugger::OnCreateThread(
    DWORD processId, DWORD threadId,
    CREATE_THREAD_DEBUG_INFO const &CreateThread) {
  if (bNoThread_) {
    // ignore...
    return;
  }
  header(processId, threadId);
  os_ << "Created thread: " << threadId << " at " << CreateThread.lpStartAddress
      << std::endl;
}

//////////////////////////////////////////////////////////////////////////
void TrapNtDebugger::OnCreateProcess(
    DWORD processId, DWORD threadId,
    CREATE_PROCESS_DEBUG_INFO const &CreateProcessInfo) {
  header(processId, threadId);
  os_ << "Process " << processId << " starting at "
      << CreateProcessInfo.lpStartAddress
      << " with command line: " << CommandLine(CreateProcessInfo.hProcess)
      << std::endl;

  processes_[processId] = CreateProcessInfo.hProcess;

  if (!CreateProcessInfo.lpImageName ||
      !showName(os_, CreateProcessInfo.hProcess, CreateProcessInfo.lpImageName,
                CreateProcessInfo.fUnicode)) {
    showModuleNameEx(CreateProcessInfo.hProcess,
                     CreateProcessInfo.lpBaseOfImage, CreateProcessInfo.hFile);
  }
  os_ << std::endl;
}

//////////////////////////////////////////////////////////////////////////
void TrapNtDebugger::OnExitThread(DWORD processId, DWORD threadId,
                                  EXIT_THREAD_DEBUG_INFO const &ExitThread) {
  if (bNoThread_) {
    // ignore...
    return;
  }
  header(processId, threadId);
  os_ << "Thread " << threadId << " exit code: " << ExitThread.dwExitCode
      << std::endl;
}

//////////////////////////////////////////////////////////////////////////
void TrapNtDebugger::OnExitProcess(DWORD processId, DWORD threadId,
                                   HANDLE /*hProcess*/,
                                   EXIT_PROCESS_DEBUG_INFO const &ExitProcess) {
  header(processId, threadId);
  os_ << "Process " << processId << " exit code: " << ExitProcess.dwExitCode
      << std::endl;
  processes_.erase(processId);
  initialised_processes_.erase(processId);
  dll_names_.erase(processId);
}

//////////////////////////////////////////////////////////////////////////
void TrapNtDebugger::OnLoadDll(DWORD processId, DWORD threadId, HANDLE hProcess,
                               LOAD_DLL_DEBUG_INFO const &LoadDll) {
  if (bLogDlls_) {
    header(processId, threadId);
    os_ << "Loaded DLL at " << LoadDll.lpBaseOfDll << " ";
    if (LoadDll.lpBaseOfDll == nullptr) {
      os_ << "Null DLL";
    } else {
      if (!LoadDll.lpImageName ||
          !showName(os_, hProcess, LoadDll.lpImageName, LoadDll.fUnicode)) {
        showModuleNameEx(hProcess, LoadDll.lpBaseOfDll, LoadDll.hFile);
      }
      if (LoadDll.hFile) {
        const std::string filename = GetFileNameFromHandle(LoadDll.hFile);
        if (!filename.empty()) {
          dll_names_[processId][LoadDll.lpBaseOfDll] = filename;
        }
      }
    }
    os_ << std::endl;
  }

  if (LoadDll.lpBaseOfDll == BaseOfNtDll_) {
    if (bShowLoaderSnaps_) {
      header(processId, threadId);
      os_ << "Setting SHOW_LDR_SNAPS\n";
      setShowLoaderSnaps(hProcess);
    }
  }

  if (LoadDll.lpBaseOfDll == TargetDll_) {
    SetDllBreakpoints(hProcess);
  }
}

//////////////////////////////////////////////////////////////////////////
void TrapNtDebugger::OnUnloadDll(DWORD processId, DWORD threadId,
                                 UNLOAD_DLL_DEBUG_INFO const &UnloadDll) {
  if (bLogDlls_) {
    header(processId, threadId);
    os_ << "Unload of DLL at " << UnloadDll.lpBaseOfDll;
    auto it = dll_names_[processId].find(UnloadDll.lpBaseOfDll);
    if (it != dll_names_[processId].end()) {
      os_ << " (" << it->second << ")";
      dll_names_[processId].erase(it);
    }
    os_ << std::endl;
  }
}

//////////////////////////////////////////////////////////////////////////
void TrapNtDebugger::OnOutputDebugString(
    DWORD processId, DWORD threadId, HANDLE hProcess,
    OUTPUT_DEBUG_STRING_INFO const &DebugString) {
  if (bNewline) {
    // If we're not adding newlines then the header is simply confusing
    header(processId, threadId);
  }
  bool const newline =
      showString(os_, hProcess, DebugString.lpDebugStringData,
                 DebugString.fUnicode, DebugString.nDebugStringLength);
  if (!newline && bNewline) {
    os_ << '\n';
  }
  os_ << std::flush;
}

//////////////////////////////////////////////////////////////////////////
// Get module name of debuggee when the debug interface didn't give it us
// Work around for the special case of the first DLL (NTDLL.DLL)
void TrapNtDebugger::showModuleNameEx(HANDLE hProcess, PVOID lpModuleBase,
                                      HANDLE hFile) const {
  if (lpModuleBase == BaseOfNtDll_)
    hProcess = GetCurrentProcess();

  std::string file_name =
      GetModuleFileNameWrapper(hProcess, (HMODULE)lpModuleBase);
  if (file_name.empty()) {
    if (hFile) {
      file_name = GetFileNameFromHandle(hFile);
    } else {
      std::cerr << "unknown module: " << lpModuleBase << ": " << displayError()
                << std::endl;
      return;
    }
  }
  os_ << file_name;
}

bool TrapNtDebugger::listCategories() {
  bool result = false;
  if (categories_.count("?")) {
    result = true;
    std::set<std::string> allCategories;
    for (const auto &entryPoint : entryPoints_) {
      allCategories.insert(entryPoint.getCategory());
    }

    std::cout << "Valid categories:\n";
    std::copy(allCategories.begin(), allCategories.end(),
              std::ostream_iterator<std::string>(std::cout, "\n"));
    std::cout << std::endl;
  }
  return result;
}

//////////////////////////////////////////////////////////////////////////
// Set up the NT breakpoints loaded from the configuration file
void TrapNtDebugger::SetDllBreakpoints(HANDLE hProcess) {
  std::set<std::string> unusedCategories(categories_);
  std::set<std::string> unusedFilters(filters_.begin(), filters_.end());

  unsigned int trapped(0);
  unsigned int total(0);

  for (const auto &entryPoint : entryPoints_) {
    bool bRequired(true);
    if (entryPoint.isDisabled()) {
      bRequired = false;
    }
    if (categories_.size() != 0) {
      if (categories_.find(entryPoint.getCategory()) == categories_.end()) {
        bRequired = false;
      } else {
        unusedCategories.erase(entryPoint.getCategory());
      }
    }
    if (bRequired && (filters_.size() != 0)) {
      bRequired = inverseFilter_;
      for (const auto &filter : filters_) {
        if (entryPoint.getName().find(filter) != std::string::npos) {
          bRequired = !bRequired;
          unusedFilters.erase(filter);
          break;
        }
      }
    }

    if (bRequired) {
      auto &ep = const_cast<EntryPoint &>(
          entryPoint); // set iterator returns const object :-(
      NtCall nt = ep.setNtTrap(hProcess, TargetDll_, bPreTrace,
                               offsets_[ep.getName()], bVerbose);
      if (nt.entryPoint_ != nullptr) {
        NtCalls_[ep.getAddress()] = nt;
        if (ep.getPreSave()) {
          NtPreSave_[ep.getPreSave()] = nt;
        }
        ++trapped;
      }
      ++total;
    }
  }

  showUnused(unusedCategories, "category");
  showUnused(unusedFilters, "filter");
  if (trapped < total / 2) {
    std::cerr << "Warning: Only " << trapped << " entry points active out of "
              << total << '\n';
  }

  if (exportFile.length() != 0) {
    std::ofstream exp(exportFile.c_str());

    if (!target_.empty()) {
      exp << "//[" << target_ << "]\n";
    }

    // Print any typdefs, sorted by the underlying type
    std::multimap<std::string, std::string> sorted;
    for (const auto &it : typedefs_) {
      sorted.insert(std::make_pair(it.second, it.first));
    }

    std::string lastType;
    for (const auto &it2 : sorted) {
      if (lastType != it2.first) {
        lastType = it2.first;
        exp << '\n';
      }
      exp << "using " << it2.second << " = " << it2.first << ";\n";
    }
    if (!sorted.empty()) {
      exp << '\n';
    }

    for (const auto &entryPoint : entryPoints_) {
      entryPoint.writeExport(exp);
      exp << std::endl;
    }
  }

  FlushInstructionCache(hProcess, nullptr, 0);
}

void TrapNtDebugger::showUnused(std::set<std::string> const &unused,
                                std::string const &name) {
  if (!unused.empty()) {
    std::cerr << "Warning: invalid " << name << " '" << *unused.begin() << "'"
              << std::endl;
  }
}

bool TrapNtDebugger::detach(DWORD processId, HANDLE hProcess) {
  for (const auto &it : NtCalls_) {
    NtCall const &ntCall = it.second;
    if (!ntCall.entryPoint_->clearNtTrap(hProcess, ntCall)) {
      std::cerr << "Cannot clear trap for " << ntCall.entryPoint_->getName()
                << " in " << processId << '\n';
      return false;
    }
  }
  FlushInstructionCache(hProcess, nullptr, 0);

  return true;
}

bool TrapNtDebugger::detachAll() {
  for (auto processe : processes_) {
    if (processe.second) {
      if (!detach(processe.first, processe.second)) {
        return false;
      }
    }
  }
  std::cout << "Detached\n";

  // Break out of the debugging loop
  bActive_ = false;

  for (auto processe : processes_) {
    DebugBreakProcess(processe.second);
  }
  return true;
}

void TrapNtDebugger::setCtrlC() {
  ctrlcTarget_ = this;
  SetConsoleCtrlHandler(TrapNtDebugger::CtrlHandler, TRUE);
}

// static
BOOL TrapNtDebugger::CtrlHandler(DWORD fdwCtrlType) {
  BOOL ret(false);
  switch (fdwCtrlType) {
  // Handle the CTRL-C signal.
  case CTRL_C_EVENT:
    if (ctrlcTarget_->detachAll()) {
      ret = TRUE;
    }
    break;
  default:
    break;
  }
  return ret;
}

void TrapNtDebugger::setShowLoaderSnaps(HANDLE hProcess) {
  static auto *pfn = (NtQueryInformationProcess *)(uintptr_t)GetProcAddress(
      BaseOfNtDll_, "NtQueryInformationProcess");
  if (pfn) {
    PROCESS_BASIC_INFORMATION pbi{};
    if (0 == pfn(hProcess, ProcessBasicInformation, &pbi, sizeof(pbi),
                 nullptr) &&
        pbi.PebBaseAddress) {
      PULONG pGlobalFlag = &pbi.PebBaseAddress->GlobalFlag;
      ULONG GlobalFlag{0};
      const ULONG SHOW_LDR_SNAPS = 2;
      ReadProcessMemory(hProcess, pGlobalFlag, &GlobalFlag, sizeof(GlobalFlag),
                        nullptr);
      GlobalFlag |= SHOW_LDR_SNAPS;
      WriteProcessMemory(hProcess, pGlobalFlag, &GlobalFlag, sizeof(GlobalFlag),
                         nullptr);
    }
  }
}

void setErrorCodes(std::string const &codeFilter) {
  std::vector<std::string> codes;

  SimpleTokenizer(codeFilter, &codes, ',');

  for (const auto &it : codes) {
    unsigned int code(0);
    char scrap(0);
#pragma warning(push)
#pragma warning(disable : 4996)
    if (sscanf(it.c_str(), "%x%c", &code, &scrap) != 1) {
      throw std::runtime_error("Unrecognised error code value '" + it + "'");
    }
#pragma warning(pop)
    errorCodes.insert(static_cast<NTSTATUS>(code));
  }
}

//////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {
  bool attach(false);
  std::string outputFile;
  std::string category;
  std::string filter;
  std::string codeFilter;
  bool bOnly(false);
  bool bNoDlls(false);
  bool bNoExcept(false);
  bool bNoThread(false);
  bool noDebugHeap(false);
  bool bNoNames(false);
  bool bShowLoaderSnaps(false);

  Options options(szRCSID);
  options.set(
      "a", &attach,
      "attach to existing process <cmd> rather than starting a fresh <cmd>");
  options.set("e", &bErrorsOnly, "Only log errors");
  options.set("v", &bVerbose, "More verbose logging");
  options.set("config", &configFile, "Specify config file");
  options.set("errors", &codeFilter,
              "Comma delimited list of error codes to filter on");
  options.set("export", &exportFile,
              "Export symbols once loaded [for testing]");
  options.set("filter", &filter,
              "Comma delimited list of substrings to filter on (leading '-' to "
              "filter off)");
  options.set("category", &category,
              "Comma delimited list of categories to trace (eg "
              "File,Process,Registry, ? for list)");
  options.set("hd", &noDebugHeap, "Don't use debug heap");
  options.set("nonames", &bNoNames, "Don't name arguments");
  options.set("nodlls", &bNoDlls, "Don't process DLL load/unload");
  options.set("noexcept", &bNoExcept, "Don't process exceptions");
  options.set("nothread", &bNoThread, "Don't process thread creation or exit");
  options.set("only", &bOnly,
              "Only debug the first process, don't debug child processes");
  options.set("out", &outputFile, "Output file");
  options.set("pre", &bPreTrace, "Trace pre-call as well as post-call");
  options.set("stack", &bStackTrace, "show stack trace");
  options.set("time", &bTimestamp, "show timestamp");
  options.set("delta", &bDelta, "show delta time");
  options.set("pid", &bPid, "show process ID");
  options.set("tid", &bTid, "show thread ID");
  options.set("nl", &bNewline, "force newline on OutputDebugString");
  options.set("sls", &bShowLoaderSnaps, "Show Loader Snaps");

  options.setArgs(1, -1, "[pid | cmd <args>]");
  if (!options.process(argc, argv,
                       "Provide trapping for calls to NT native API")) {
    return 1;
  }
  bNames = !bNoNames; // avoid double negatives

  auto it = options.begin();

  std::ofstream ofs;
  if (outputFile.length() != 0) {
    ofs.open(outputFile.c_str());
    if (!ofs) {
      std::cerr << "Cannot open: " << outputFile << std::endl;
      return 1;
    }
  }

  if (codeFilter.length())
    setErrorCodes(codeFilter);

  TrapNtDebugger debugger((outputFile.length() != 0) ? (std::ostream &)ofs
                                                     : std::cout);
  debugger.setLogDlls(!bNoDlls);
  debugger.setNoException(bNoExcept);
  debugger.setNoThread(bNoThread);
  debugger.setShowLoaderSnaps(bShowLoaderSnaps);
  debugger.setFilter(filter);
  debugger.setCategory(category);

  // Load initialisation data
  if (!debugger.initialise()) {
    std::cerr << "Failed to initialise" << std::endl;
    return 1;
  }

  if (debugger.listCategories()) {
    return 0;
  }

  int pid = 0;
  bool havePid(false);

  if (attach) {
    std::vector<DWORD> pidList = FindProcesses(it->c_str());

    if (pidList.size() == 0) {
      std::cerr << "Bad pid: " << *it << std::endl;
      return 1;
    } else if (pidList.size() > 1) {
      std::cerr << "Duplicate processes match: " << *it << std::endl;
      return 1;
    }
    pid = pidList[0];
    havePid = true;
  } else if (readInt(*it, pid)) {
    havePid = true;
  }

  if (havePid) {
    // only required if we didn't start the process
    EnableDebugPriv();

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (nullptr == hProcess) {
      DWORD const err(GetLastError());
      if (err == ERROR_INVALID_PARAMETER) {
        std::cerr << "OpenProcess failed to find pid " << pid << std::endl;
      } else {
        std::cerr << "OpenProcess failed with " << displayError(err)
                  << std::endl;
      }
      return 1;
    }

    BOOL rc = DebugActiveProcess(pid);
    if (!rc) {
      std::cerr << "DebugActiveProcess failed with " << displayError()
                << std::endl;
      return 1;
    }
  } else {
    if (noDebugHeap) {
      (void)_putenv("_NO_DEBUG_HEAP=1");
    }

    PROCESS_INFORMATION ProcessInformation;
    int ret = CreateProcessHelper(
        options.begin(), options.end(),
        bOnly ? DEBUG_ONLY_THIS_PROCESS : DEBUG_PROCESS, &ProcessInformation);

    if (ret != 0) {
      std::cerr << "CreateProcess failed with " << displayError();
#ifdef _M_IX86
      if (GetLastError() == ERROR_NOT_SUPPORTED) {
        std::cerr << " - is the target process 64bit?";
      }
#endif // _M_IX86
      std::cerr << std::endl;
      return 1;
    }

    // Close unwanted handles
    if (!CloseHandle(ProcessInformation.hProcess)) {
      std::cerr << "Unable to close process handle: " << displayError()
                << std::endl;
    }
    if (!CloseHandle(ProcessInformation.hThread)) {
      std::cerr << "Unable to close thread handle: " << displayError()
                << std::endl;
    }
  }

  debugger.setCtrlC();

  DebugDriver().Loop(debugger);

  if (havePid && !debugger.Active()) {
    // We've detached from all targets, so don't kill them on exit
    DebugSetProcessKillOnExit(false);
  }

  return 0;
}
