/*
NAME
    DebugDriver.cpp

DESCRIPTION
    Implementation of the debug driver code

COPYRIGHT
    Copyright (C) 2002, 2013 by Roger Orr <rogero@howzatt.co.uk>

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
*/

#include "DebugDriver.h"

#include <iostream>

#include "displayError.h"

static char const szRCSID[] =
    "$Id: DebugDriver.cpp 2621 2025-03-08 16:50:46Z roger $";

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
      debugger.OnExitProcess(DebugEvent.dwProcessId, DebugEvent.dwThreadId,
                             DebugEvent.u.ExitProcess);

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
