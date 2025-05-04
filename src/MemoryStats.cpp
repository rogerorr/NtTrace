/*
/*
NAME
  MemoryStats

DESCRIPTION
  Produce memory statistics for each child process on exit

AUTHOR
  Roger Orr mailto:rogero@howzatt.co.uk
  Bug reports, comments, and suggestions are always welcome.

COPYRIGHT
  Copyright (C) 2025 under the MIT license:

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
    "$Id: MemoryStats.cpp 2824 2025-05-04 21:49:55Z roger $";

#ifdef _M_X64
#include <ntstatus.h>
#define WIN32_NO_STATUS
#endif // _M_X64

#include <windows.h>

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>

// o2 includes
#include "../include/DisplayError.h"
#include "../include/Options.h"
#include "../include/ProcessHelper.h"

#include "DebugDriver.h"
#include "ShowData.h"

/** Simple memory stats collector */
class MemoryStats : public or2::DebuggerAdapter {
private:
  std::ostream &os_;

public:
  /**
   * Construct a memory stats collector.
   * @param os the output stream to write debug event information to
   */
  MemoryStats(std::ostream &os) : os_(os) {}

  /** Process has been created */
  void OnCreateProcess(DWORD processId, DWORD threadId,
                       CREATE_PROCESS_DEBUG_INFO const &createProcess) override;

  /** Process has exited */
  void OnExitProcess(DWORD processId, DWORD threadId, HANDLE hProcess,
                     EXIT_PROCESS_DEBUG_INFO const &exitProcess) override;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
void MemoryStats::OnCreateProcess(
    DWORD processId, DWORD /*threadId*/,
    CREATE_PROCESS_DEBUG_INFO const &createProcess) {
  os_ << "Start process " << processId << " - "
      << showData::CommandLine(createProcess.hProcess) << std::endl;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
void MemoryStats::OnExitProcess(DWORD processId, DWORD /*threadId*/,
                                HANDLE hProcess,
                                EXIT_PROCESS_DEBUG_INFO const &exitProcess) {
  os_ << "End process " << processId << ": " << exitProcess.dwExitCode << " - "
      << showData::CommandLine(hProcess) << std::endl;
  PROCESS_MEMORY_COUNTERS pmc;
  if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
    os_ << "Memory stats for " << processId << ": "
        << "PageFaultCount: " << pmc.PageFaultCount
        << ", PeakWorkingSetSize: " << pmc.PeakWorkingSetSize
        << ", WorkingSetSize: " << pmc.WorkingSetSize
        << ", QuotaPeakPagedPoolUsage: " << pmc.QuotaPeakPagedPoolUsage
        << ", QuotaPagedPoolUsage: " << pmc.QuotaPagedPoolUsage
        << ", QuotaPeakNonPagedPoolUsage: " << pmc.QuotaPeakNonPagedPoolUsage
        << ", QuotaNonPagedPoolUsage: " << pmc.QuotaNonPagedPoolUsage
        << ", PagefileUsage: " << pmc.PagefileUsage
        << ", PeakPagefileUsage: " << pmc.PeakPagefileUsage << "\n";
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {
  bool bOnly(false);
  std::string outputFile;
  or2::Options options(szRCSID);
  options.set("out", &outputFile, "Output file");
  options.set("only", &bOnly,
              "Only debug the first process, don't debug child processes");
  options.setArgs(1, -1, "cmd [args]");
  if (!options.process(argc, argv,
                       "Display memory statistics for child process(es)"))
    return 1;

  (void)_putenv("_NO_DEBUG_HEAP=1");

  PROCESS_INFORMATION ProcessInformation;
  int ret = or2::CreateProcessHelper(
      options.begin(), options.end(),
      bOnly ? DEBUG_ONLY_THIS_PROCESS : DEBUG_PROCESS, &ProcessInformation);

  if (ret != 0) {
    std::cerr << "CreateProcess failed with: " << or2::displayError();
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
    std::cerr << "Unable to close process handle: " << or2::displayError()
              << std::endl;
  }
  if (!CloseHandle(ProcessInformation.hThread)) {
    std::cerr << "Unable to close thread handle: " << or2::displayError()
              << std::endl;
  }

  std::ofstream ofs;
  if (outputFile.length() != 0) {
    ofs.open(outputFile.c_str());
    if (!ofs) {
      std::cerr << "Cannot open: " << outputFile << std::endl;
      return 1;
    }
  }

  MemoryStats debugger((outputFile.length() != 0) ? (std::ostream &)ofs
                                                  : std::cout);
  or2::DebugDriver().Loop(debugger);

  return 0;
}
