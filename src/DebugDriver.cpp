/*
NAME
  DebugDriver.cpp

DESCRIPTION
  Implementation of the debug driver code

COPYRIGHT
  Copyright (C) 2002-2013 under the MIT license:

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

#include "DebugDriver.h"

#include <iostream>

#include "../include/DisplayError.h"

static char const szRCSID[] =
    "$Id: DebugDriver.cpp 2724 2025-04-24 22:36:40Z roger $";

//////////////////////////////////////////////////////////////////////////
// Main debugger loop
void or2::DebugDriver::Loop(Debugger &debugger) {
  ULONG timeout = INFINITE;
  DEBUG_EVENT DebugEvent;

  while (WaitForDebugEvent(&DebugEvent, // debug event information
                           timeout      // time-out value
                           ) &&
         debugger.Active()) {
    DWORD continueFlag = DBG_CONTINUE;
    switch (DebugEvent.dwDebugEventCode) {
    case EXCEPTION_DEBUG_EVENT: {
      ProcessEntry &pe = processMap_[DebugEvent.dwProcessId];
      ThreadMap &tm = pe.threadMap_;
      HANDLE hThread = tm[DebugEvent.dwThreadId];

      // The first breakpoint is called by attaching to the process.
      if (!pe.attached_ &&
          (DebugEvent.u.Exception.ExceptionRecord.ExceptionCode ==
           STATUS_BREAKPOINT)) {
        pe.attached_ = true;
      } else {
        continueFlag = DBG_EXCEPTION_NOT_HANDLED;
      }

      debugger.OnException(DebugEvent.dwProcessId, DebugEvent.dwThreadId,
                           pe.hProcess_, hThread, DebugEvent.u.Exception,
                           &continueFlag);
    } break;

    case CREATE_THREAD_DEBUG_EVENT: {
      ProcessEntry &processEntry = processMap_[DebugEvent.dwProcessId];
      processEntry.threadMap_[DebugEvent.dwThreadId] =
          DebugEvent.u.CreateThread.hThread;

      debugger.OnCreateThread(DebugEvent.dwProcessId, DebugEvent.dwThreadId,
                              DebugEvent.u.CreateThread);
    } break;

    case CREATE_PROCESS_DEBUG_EVENT: {
      ProcessEntry pe;
      pe.hProcess_ = DebugEvent.u.CreateProcessInfo.hProcess;
      pe.threadMap_[DebugEvent.dwThreadId] =
          DebugEvent.u.CreateProcessInfo.hThread;
      processMap_[DebugEvent.dwProcessId] = pe;

      debugger.OnCreateProcess(DebugEvent.dwProcessId, DebugEvent.dwThreadId,
                               DebugEvent.u.CreateProcessInfo);

      // Close unwanted handle (following John Robbins)
      if ((DebugEvent.u.CreateProcessInfo.hFile != nullptr) &&
          (!CloseHandle(DebugEvent.u.CreateProcessInfo.hFile))) {
        std::cerr << "Unable to close process file handle: " << displayError()
                  << std::endl;
      }

      break;
    }

    case EXIT_THREAD_DEBUG_EVENT: {
      ThreadMap &threadMap = processMap_[DebugEvent.dwProcessId].threadMap_;

      debugger.OnExitThread(DebugEvent.dwProcessId, DebugEvent.dwThreadId,
                            DebugEvent.u.ExitThread);

      threadMap.erase(DebugEvent.dwThreadId);

    } break;

    case EXIT_PROCESS_DEBUG_EVENT: {
      ProcessEntry &entry = processMap_[DebugEvent.dwProcessId];
      debugger.OnExitProcess(DebugEvent.dwProcessId, DebugEvent.dwThreadId,
                             entry.hProcess_, DebugEvent.u.ExitProcess);

      processMap_.erase(DebugEvent.dwProcessId);
      if (processMap_.empty()) {
        timeout = 1; // Nothing left to live for :-)
      }

      break;
    }

    case LOAD_DLL_DEBUG_EVENT: {
      HANDLE hProcess = processMap_[DebugEvent.dwProcessId].hProcess_;

      debugger.OnLoadDll(DebugEvent.dwProcessId, DebugEvent.dwThreadId,
                         hProcess, DebugEvent.u.LoadDll);

      // Close unwanted handle (following John Robbins)
      if ((DebugEvent.u.LoadDll.hFile != nullptr) &&
          (!CloseHandle(DebugEvent.u.LoadDll.hFile))) {
        std::cerr << "Unable to close dll file handle: " << displayError()
                  << std::endl;
      }
    } break;

    case UNLOAD_DLL_DEBUG_EVENT:
      debugger.OnUnloadDll(DebugEvent.dwProcessId, DebugEvent.dwThreadId,
                           DebugEvent.u.UnloadDll);
      break;

    case OUTPUT_DEBUG_STRING_EVENT: {
      HANDLE hProcess = processMap_[DebugEvent.dwProcessId].hProcess_;

      debugger.OnOutputDebugString(DebugEvent.dwProcessId,
                                   DebugEvent.dwThreadId, hProcess,
                                   DebugEvent.u.DebugString);
    } break;

    default:
      std::cerr << "Got unexpected debug event " << DebugEvent.dwDebugEventCode
                << std::endl;
      break;
    }

    if (!ContinueDebugEvent(DebugEvent.dwProcessId, DebugEvent.dwThreadId,
                            continueFlag)) {
      std::cerr << "Error " << displayError() << " continuing debug event"
                << std::endl;
    }
  }
}
