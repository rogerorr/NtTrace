/*
NAME
    ShowLoaderSnaps.cpp

DESCRIPTION
    Process to display loadder snap messages

COPYRIGHT
    Copyright (C) 2025 by Roger Orr <rogero@howzatt.co.uk>

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

EXAMPLE
    ShowLoaderSnaps fred.exe
*/

static char const szRCSID[] =
    "$Id: ShowLoaderSnaps.cpp 2618 2025-03-06 23:28:57Z roger $";

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
#include <Options.h>
#include <ProcessHelper.h>
#include <displayError.h>

#include "DebugDriver.h"

using namespace or2;

#pragma comment(lib, "ntdll")

//////////////////////////////////////////////////////////////////////////
/** Debugger event handler for showing loader snaps entry points */
class ShowLoaderSnaps : public DebuggerAdapter {
public:
  /**
   * Construct a debugger
   * @param os the output stream to write to
   */
  ShowLoaderSnaps(std::ostream &os) : os_(os) {}

  void setMinimal() {
    filters_.push_back(" - ENTER: ");
    filters_.push_back(" - RETURN: ");
    filters_.push_back(" - INFO: ");
  }

  // callbacks on events
  void
  OnOutputDebugString(DWORD /*processId*/, DWORD /*threadId*/, HANDLE hProcess,
                      OUTPUT_DEBUG_STRING_INFO const &DebugString) override {
    const auto message =
        readString(hProcess, DebugString.lpDebugStringData,
                   DebugString.fUnicode, DebugString.nDebugStringLength);
    // Filter out unwanted messages
    for (const auto &filter : filters_) {
      if (message.find(filter) != std::string::npos) {
        return;
      }
    }
    os_ << message << std::flush;
  }

  std::string readString(HANDLE hProcess, LPVOID lpString, bool bUnicode,
                         WORD nStringLength) {
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

private:
  std::ostream &os_;
  std::vector<std::string> filters_;
};

//////////////////////////////////////////////////////////////////////////
namespace {
void SetShowLoaderSnaps(HANDLE hProcess) {
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
} // namespace

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
    debugger.setMinimal();
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

  SetShowLoaderSnaps(ProcessInformation.hProcess);
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
