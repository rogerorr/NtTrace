/*
NAME
  ShowLoaderSnaps.cpp

DESCRIPTION
  Process to display loader snap messages

AUTHOR
  Roger Orr mailto:rogero@howzatt.co.uk
  Bug reports, comments, and suggestions are always welcome.

COPYRIGHT (MIT License)

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

EXAMPLE
  ShowLoaderSnaps fred.exe
*/

static char const szRCSID[] =
    "$Id: ShowLoaderSnaps.cpp 2721 2025-04-24 21:00:30Z roger $";

#pragma warning(disable : 4800) // forcing value to bool 'true' or 'false'
                                // (performance warning)
#pragma warning(disable : 4996) // 'asctime' and others were declared deprecated

#define WIN32_LEAN_AND_MEAN

#include <fstream>
#include <iostream>
#include <string>
#include <windows.h>
#include <winternl.h>

// or2 includes
#include <DisplayError.h>
#include <Options.h>
#include <ProcessHelper.h>

#include "DebugDriver.h"

using namespace or2;

#pragma comment(lib, "ntdll")

//////////////////////////////////////////////////////////////////////////
/** Debugger event handler for showing loader snaps entry points */
class ShowLoaderSnaps : public DebuggerAdapter {
public:
  // Constructor: output will be written to `os`
  ShowLoaderSnaps(std::ostream &os) : os_(os) {}

  // Set minimal output
  void SetQuiet();

  // Turn on loader snaps for the target process
  void SetShowLoaderSnaps(HANDLE hProcess);

  // Callback on output debug string event
  void
  OnOutputDebugString(DWORD /*processId*/, DWORD /*threadId*/, HANDLE hProcess,
                      OUTPUT_DEBUG_STRING_INFO const &DebugString) override;

private:
  std::ostream &os_;
  std::vector<std::string> filters_;

  std::string ReadString(HANDLE hProcess, LPVOID lpString, bool bUnicode,
                         WORD nStringLength);
};

//////////////////////////////////////////////////////////////////////////
void ShowLoaderSnaps::SetQuiet() {
  filters_.push_back(" - ENTER: ");
  filters_.push_back(" - RETURN: ");
  filters_.push_back(" - INFO: ");
}

//////////////////////////////////////////////////////////////////////////
void ShowLoaderSnaps::SetShowLoaderSnaps(HANDLE hProcess) {
  PROCESS_BASIC_INFORMATION pbi = {};
  if (0 == NtQueryInformationProcess(hProcess, ProcessBasicInformation, &pbi,
                                     sizeof(pbi), 0)) {
#ifdef _WIN64
    // GlobalFlag is not officially documented
    // Offsets obtained from PDB file for ntdll.dll
    PVOID pGlobalFlag = ((char *)pbi.PebBaseAddress) + 188;
#else
    PVOID pGlobalFlag = ((char *)pbi.PebBaseAddress) + 104;
#endif // _WIN64
    ULONG GlobalFlag{0};
    const ULONG SHOW_LDR_SNAPS = 2;
    ReadProcessMemory(hProcess, pGlobalFlag, &GlobalFlag, sizeof(GlobalFlag),
                      0);
    GlobalFlag |= SHOW_LDR_SNAPS;
    WriteProcessMemory(hProcess, pGlobalFlag, &GlobalFlag, sizeof(GlobalFlag),
                       0);
  }
}

//////////////////////////////////////////////////////////////////////////
void ShowLoaderSnaps::OnOutputDebugString(
    DWORD /*processId*/, DWORD /*threadId*/, HANDLE hProcess,
    OUTPUT_DEBUG_STRING_INFO const &DebugString) {
  const auto message =
      ReadString(hProcess, DebugString.lpDebugStringData, DebugString.fUnicode,
                 DebugString.nDebugStringLength);
  // Filter out unwanted messages
  for (const auto &filter : filters_) {
    if (message.find(filter) != std::string::npos) {
      return;
    }
  }
  os_ << message << std::flush;
}

//////////////////////////////////////////////////////////////////////////
std::string ShowLoaderSnaps::ReadString(HANDLE hProcess, LPVOID lpString,
                                        bool bUnicode, WORD nStringLength) {
  std::string message;
  if (nStringLength == 0) {
  } else if (bUnicode) {
    std::vector<wchar_t> chVector(nStringLength + 1);
    if (ReadProcessMemory(hProcess, lpString, &chVector[0],
                          nStringLength * sizeof(wchar_t), nullptr)) {
      size_t const wcLen = wcstombs(nullptr, &chVector[0], 0);
      if (wcLen == (size_t)-1) {
        os_ << "invalid string\n";
      } else {
        message.resize(wcLen);
        wcstombs(&message[0], &chVector[0], wcLen);
      }
    }
  } else {
    message.resize(nStringLength);
    (void)ReadProcessMemory(hProcess, lpString, &message[0], nStringLength,
                            nullptr);
  }

  // Remove any trailing string terminator
  message.resize(message.find_last_not_of('\0') + 1);

  return message;
}

//////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {
  std::string outputFile;
  bool quiet{};

  Options options(szRCSID);
  options.set("out", &outputFile, "Output file");
  options.set("q", &quiet, "quiet output (suppress 'normal' messages)");

  options.setArgs(1, -1, "[pid | cmd <args>]");
  if (!options.process(argc, argv,
                       "Show Loader Snaps from executing the target program")) {
    return 1;
  }

  Options::const_iterator it = options.begin();

  std::ofstream ofs;
  if (outputFile.length() != 0) {
    ofs.open(outputFile.c_str());
    if (!ofs) {
      std::cerr << "Cannot open: " << outputFile << std::endl;
      return 1;
    }
  }

  ShowLoaderSnaps debugger((outputFile.length() != 0) ? (std::ostream &)ofs
                                                      : std::cout);
  if (quiet) {
    debugger.SetQuiet();
  }

  putenv("_NO_DEBUG_HEAP=1");

  PROCESS_INFORMATION ProcessInformation;
  int ret = CreateProcessHelper(options.begin(), options.end(),
                                DEBUG_PROCESS | CREATE_SUSPENDED,
                                &ProcessInformation);

  if (ret != 0) {
    std::cerr << "CreateProcess failed with " << displayError() << std::endl;
    return 1;
  }

  debugger.SetShowLoaderSnaps(ProcessInformation.hProcess);
  ResumeThread(ProcessInformation.hThread);

  // Close unwanted handles
  if (!CloseHandle(ProcessInformation.hProcess)) {
    std::cerr << "Unable to close process handle: " << displayError()
              << std::endl;
  }
  if (!CloseHandle(ProcessInformation.hThread)) {
    std::cerr << "Unable to close thread handle: " << displayError()
              << std::endl;
  }

  DebugDriver().Loop(debugger);

  return 0;
}
