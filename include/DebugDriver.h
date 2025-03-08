#ifndef DEBUGDRIVER_H_
#define DEBUGDRIVER_H_

/**
@file
    Simple base class for running the debug loop.

    @author Roger Orr <rogero@howzatt.co.uk>

    Copyright &copy; 2002, 2013.
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

    $Revision: 2621 $
*/

// $Id: DebugDriver.h 2621 2025-03-08 16:50:46Z roger $

#ifndef _WINDOWS_
#include <windows.h>
#endif // _WINDOWS_

#include <map>

namespace or2 {

//////////////////////////////////////////////////////////////////////////
/**
 * Base interface class for writing a debugger.
 *
 * This class provides the callbacks for the DebugDriver class.
 */
class Debugger {
public:
  // callbacks on events
  /** Exception occurred */
  virtual void OnException(DWORD processId, DWORD threadId, HANDLE hProcess,
                           HANDLE hThread,
                           EXCEPTION_DEBUG_INFO const &DebugEvent,
                           DWORD *pContinueExecution) = 0;

  /** Callback on thread creation */
  virtual void OnCreateThread(DWORD processId, DWORD threadId,
                              CREATE_THREAD_DEBUG_INFO const &CreateThread) = 0;

  /** Callback on process creation */
  virtual void
  OnCreateProcess(DWORD processId, DWORD threadId,
                  CREATE_PROCESS_DEBUG_INFO const &CreateProcessInfo) = 0;

  /** Callback on thread exit */
  virtual void OnExitThread(DWORD processId, DWORD threadId,
                            EXIT_THREAD_DEBUG_INFO const &ExitThread) = 0;

  /** Callback on process exit */
  virtual void OnExitProcess(DWORD processId, DWORD threadId,
                             EXIT_PROCESS_DEBUG_INFO const &ExitProcess) = 0;

  /** Callback on loading DLL */
  virtual void OnLoadDll(DWORD processId, DWORD threadId, HANDLE hProcess,
                         LOAD_DLL_DEBUG_INFO const &LoadDll) = 0;

  /** Callback on unloading DLL */
  virtual void OnUnloadDll(DWORD processId, DWORD threadId,
                           UNLOAD_DLL_DEBUG_INFO const &UnloadDll) = 0;

  /** Callback on outputting a debug string */
  virtual void
  OnOutputDebugString(DWORD processId, DWORD threadId, HANDLE hProcess,
                      OUTPUT_DEBUG_STRING_INFO const &DebugString) = 0;

  /** Is the debugger still active? */
  virtual bool Active() { return true; }

  /** Virtual dtor for safe inheritance */
  virtual ~Debugger() {}
};

//////////////////////////////////////////////////////////////////////////
/**
 * Adapter for stripped-down functionality of a Debugger
 */
class DebuggerAdapter : public Debugger {
#pragma warning(push)
#pragma warning(disable : 4100) // unreferenced formal parameter

public:
  // callbacks on events
  /** Exception occurred */
  void OnException(DWORD processId, DWORD threadId, HANDLE hProcess,
                   HANDLE hThread, EXCEPTION_DEBUG_INFO const &DebugEvent,
                   DWORD *pContinueExecution) override {}

  /** Callback on thread creation */
  void OnCreateThread(DWORD processId, DWORD threadId,
                      CREATE_THREAD_DEBUG_INFO const &CreateThread) override {}

  /** Callback on process creation */
  void
  OnCreateProcess(DWORD processId, DWORD threadId,
                  CREATE_PROCESS_DEBUG_INFO const &CreateProcessInfo) override {
  }

  /** Callback on thread exit */
  void OnExitThread(DWORD processId, DWORD threadId,
                    EXIT_THREAD_DEBUG_INFO const &ExitThread) override {}

  /** Callback on process exit */
  void OnExitProcess(DWORD processId, DWORD threadId,
                     EXIT_PROCESS_DEBUG_INFO const &ExitProcess) override {}

  /** Callback on loading DLL */
  void OnLoadDll(DWORD processId, DWORD threadId, HANDLE hProcess,
                 LOAD_DLL_DEBUG_INFO const &LoadDll) override {}

  /** Callback on unloading DLL */
  void OnUnloadDll(DWORD processId, DWORD threadId,
                   UNLOAD_DLL_DEBUG_INFO const &UnloadDll) override {}

  /** Callback on outputting a debug string */
  void
  OnOutputDebugString(DWORD processId, DWORD threadId, HANDLE hProcess,
                      OUTPUT_DEBUG_STRING_INFO const &DebugString) override {}

#pragma warning(pop)
};

//////////////////////////////////////////////////////////////////////////
/**
 * Simple class for running the debug loop.
 *
 * The DebugDriver class runs the debug loop until the debugee has finished.
 * Events are signified by callbacks to the supplied Debugger object
 */
class DebugDriver {
public:
  /** Runs till debugee finishes, calling back 'debugger' for each event */
  void Loop(Debugger &debugger);

private:
  //////////////////////////////////////////////////////////////////////////
  // Data structure used for handling thread/process id -> handle mapping
  using ThreadMap = std::map<DWORD, HANDLE>;
  struct ProcessEntry {
    bool attached_{};
    HANDLE hProcess_{};
    ThreadMap threadMap_;
  };
  using ProcessMap = std::map<DWORD, ProcessEntry>;

private:
  ProcessMap processMap_;
};

} // namespace or2

#endif // DEBUGDRIVER_H_