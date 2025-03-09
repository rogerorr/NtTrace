/*
NAME
    EntryPoint.cpp

DESCRIPTION
    Handle a single entry point for NtTrace

COPYRIGHT
    Copyright (C) 2002, 2019 by Roger Orr <rogero@howzatt.co.uk>

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

static char const szRCSID[] =
    "$Id: EntryPoint.cpp 2638 2025-03-08 23:00:02Z roger $";

#include "EntryPoint.h"
#include "Enumerations.h"

#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <windows.h>

#include "NtDllStruct.h"
#include "SymbolEngine.h"
#include "displayError.h"

#include "ShowData.h"
#include "TrapNtOpcodes.h"

using namespace showData;
using or2::displayError;

namespace {
void printStackTrace(std::ostream &os, HANDLE hProcess, HANDLE hThread,
                     CONTEXT const &Context);
std::string buffToHex(unsigned char *buffer, size_t length);
ArgType getArgType(const std::string typeName,
                   EntryPoint::Typedefs const &typedefs);
bool deadExport(unsigned char instruction[], size_t length);

#pragma warning(push)
#pragma warning(disable : 4592) // symbol will be dynamically initialized
const std::map<std::string, ArgAttributes> sal_attributes = {
    {"_In_", argIN},
    {"__in", argIN},
    {"IN", argIN},

    {"_In_opt_", ArgAttributes(argIN | argOPTIONAL)},
    {"__in_opt", ArgAttributes(argIN | argOPTIONAL)},

    {"_Out_", argOUT},
    {"__out", argOUT},
    {"OUT", argOUT},

    {"_Out_opt_", ArgAttributes(argOUT | argOPTIONAL)},
    {"__out_opt", ArgAttributes(argOUT | argOPTIONAL)},

    {"_Inout_", ArgAttributes(argIN | argOUT)},
    {"__inout", ArgAttributes(argIN | argOUT)},

    {"_Inout_opt_", ArgAttributes(argIN | argOUT | argOPTIONAL)},
    {"__inout_opt", ArgAttributes(argIN | argOUT | argOPTIONAL)},

    {"const", argCONST},
    {"CONST", argCONST},

    {"_Reserved_", argRESERVED},
    {"__reserved", argRESERVED},
};
#pragma warning(pop)
} // namespace

//////////////////////////////////////////////////////////////////////////
// Module data

extern "C" {
// Function to convert NT status codes to normal NT error codes
#ifdef _M_IX86
using PFNRtlNtStatusToDosError = DWORD(NTAPI *)(DWORD);
#elif _M_X64
using PFNRtlNtStatusToDosError = DWORD64(NTAPI *)(DWORD64);
#endif // _M_IX86
}

static PFNRtlNtStatusToDosError RtlNtStatusToDosError =
    (PFNRtlNtStatusToDosError)GetProcAddress(GetModuleHandle("NtDll"),
                                             "RtlNtStatusToDosError");

//////////////////////////////////////////////////////////////////////////
// The various NtDll signatures

#ifdef _M_IX86

// Check for basic NT4/W2K signature...
//  B8 24 00 00 00       mov         eax,24h
//  8D 54 24 04          lea         edx,[esp+4]
//  CD 2E                int         2Eh
//  C2 20 00             ret         20h      // or just 'ret'

static unsigned char const signature1[] = {MOVdwordEax, 5, LEA, 4,
                                           INTn,        2, 0,   0}; // 11 bytes

// Check for basic W2K3 signature...
//  B8 1E 00 00 00       mov         eax,1Eh
//  BA 00 03 FE 7F       mov         edx,7FFE0300h
//  FF D2                call        edx
//  C2 0C 00             ret         0Ch

static unsigned char const signature2[] = {
    MOVdwordEax, 5, MOVdwordEdx, 5, Call, 2, 0, 0}; // 12 bytes

// Check for basic W2K8/64 32-bit signature...
//  B8 1E 00 00 00       mov         eax,1Eh
//  B9 03 00 00 00       mov         ecx,03h
//  8D 54 24 04          lea         edx,[esp+4]
//  64 FF 15 C0 00 00 00 call        fs:[0c0h]
//  C2 0C 00             ret         0Ch

static unsigned char const signature3[] = {
    MOVdwordEax, 5, MOVdwordEcx, 5, LEA, 4, FS, 1, Call, 6, 0, 0}; // 21 bytes

// Check for type-2 W2K8/64 32-bit signature...
//  B8 1E 00 00 00       mov         eax,1Eh
//  33 C9                xor         ecx,ecx
//  8D 54 24 04          lea         edx,[esp+4]
//  64 FF 15 C0 00 00 00 call        fs:[0c0h]
//  C2 0C 00             ret         0Ch

static unsigned char const signature4[] = {
    MOVdwordEax, 5, XOR, 2, LEA, 4, FS, 1, Call, 6, 0, 0}; // 18 bytes

// Check for Windows 8.1 32bit signature
// b8 0e 00 03 00        mov     eax,0x3000e
// 64 ff 15 c0 00 00 00  call    dword ptr fs:[000000c0]
// c2 04 00              ret     0x4

static unsigned char const signature5[] = {MOVdwordEax, 5, FS, 1,
                                           Call,        6, 0,  0}; // 12 bytes

// Check for Windows 10 NtQueryInformationProcess (and trap the
// Wow64SystemServiceCall) ntdll!NtQueryInformationProcess:
// b8 19 00 00 00        mov     eax,19h
// e8 04 00 00 00        call    ntdll!NtQueryInformationProcess+0xe
// 00 00 1d 77           <ntdll>
// 5a                    pop     edx
// 80 7a 03 4b           cmp     byte ptr [edx+3],4Bh
// 75 0a                 jne     ntdll!NtQueryInformationProcess+0x1f
// 64 ff 15 c0 00 00 00  call    dword ptr fs:[0C0h]
// c2 14 00              ret     14h
// ba c0 b4 25 77        mov     edx,offset ntdll!Wow64SystemServiceCall
// ff d2                 call    edx c2
// 14 00                 ret     14h

static unsigned char const signature6[] = {
    MOVdwordEax, 5, 0xe8, 5 + 4, 0x5a, 1, 0x80, 4, 0x75, 2, FS, 1,
    Call,        6, 0xc2, 3,     0xba, 5, Call, 2, 0,    0}; // 38 bytes

// Check for Windows 10 Creator NtQueryInformationProcess (and trap the
// Wow64SystemServiceCall) ntdll!NtQueryInformationProcess:
// b8 19 00 00 00 mov    eax,19h
// e8 00 00 00 00        call    ntdll!NtQueryInformationProcess+0xa
// 5a                    pop     edx
// 80 7a 14 4b           cmp     byte ptr [edx+14h],4Bh
// 75 0e                 jne     ntdll!NtQueryInformationProcess+0x1f
// 64 ff 15 c0 00 00 00  call    dword ptr fs:[0C0h]
// c2 14 00              ret     14h
// 00 00 9a 77           <ntdll>
// ba 40 61 a2 77        mov     edx,offset ntdll!Wow64SystemServiceCall
// ff d2                 call    edx
// c2 14 00              ret     14h

static unsigned char const signature6b[] = {
    MOVdwordEax, 5, 0xe8, 5,     0x5a, 1, 0x80, 4, 0x75, 2, FS, 1,
    Call,        6, 0xc2, 3 + 4, 0xba, 5, Call, 2, 0,    0}; // 38 bytes

static unsigned char const *signatures[] = {
    signature1, signature2, signature3,  signature4,
    signature5, signature6, signature6b,
};

// Dead Export from Win32u.dll for example for NtUserCallHwnd
// e8 d1 ff ff ff       call    __stdcall DeadExport(void) (74d41006)
// c2 08 00             ret     0x8
// cc                   int     3

static unsigned char const dead_export1[] = {0xe8, 5, 0xc2, 3, 0xcc, 1, 0, 0};

// Dead Export from Win32u.dll for example for NtUserYieldTask
// e9 a1 ff ff ff       jmp     DeadExport
// cc                   int     3

static unsigned char const dead_export2[] = {0xe9, 5, 0xcc, 1, 0, 0};

static unsigned char const *dead_exports[] = {
    dead_export1,
    dead_export2,
};

static unsigned int const MAX_PREAMBLE(38);

#elif _M_X64

// Check for W2K8/64 64-bit signature...
//  4c 8b d1             mov         r10,rcx
//  b8 52 00 00 00       mov         eax,0x52
//  0f 05                syscall
//  C3                   ret

static unsigned char const signature1[] = {0x4c, 3, MOVdwordEax, 5,
                                           0x0f, 2, 0,           0}; // 10 bytes

// Check for W10 update 1 64-bit signature...
// Note: we don't currently trap on the older 'int' case.
// 4c 8b d1                mov     r10,rcx
// b8 0f 00 00 00          mov     eax,0Fh
// f6 04 25 08 03 fe 7f 01 test    byte ptr [SharedUserData.SystemCall],1
// 75 03                   jne     $+3
// 0f 05                   syscall
// c3                      ret
// cd 2e                   int     2Eh
// c3                      ret

static unsigned char const signature2[] = {
    0x4c, 3, MOVdwordEax, 5, 0xf6, 8, 0x75, 2, 0x0f, 2, 0, 0}; // 21 bytes

static unsigned char const *signatures[] = {
    signature1,
    signature2,
};

static unsigned int const MAX_PREAMBLE(21);

// Dead Export from Win32u.dll for example for NtUserCallHwnd
// 48 83 ec 28           sub     rsp,28h
// 45 33 c0              xor     r8d,r8d
// 33 d2                 xor     edx,edx
// 33 c9                 xor     ecx,ecx
// 48 ff 15 2e c1 00 00  call    qword ptr [win32u!_imp_RaiseFailFastException]

static unsigned char const dead_export1[] = {0x48, 4, 0x45, 3, 0x33, 2, 0x33, 2,
                                             0x48, 1, 0xff, 7, 0,    0};

static unsigned char const *dead_exports[] = {
    dead_export1,
};

#endif // _M_IX86

//////////////////////////////////////////////////////////////////////////
// Show the argument for the given process with the specified value.
void Argument::showArgument(std::ostream &os, HANDLE hProcess, ARG argVal,
                            bool returnOk, bool dup, bool showNames) const {
  if ((attributes_ & argRESERVED) && (argVal == 0)) {
    // An empty reserved argument
    os << '0';
    return;
  }

  if (showNames && !name_.empty())
    os << name_ << "=";

  // Don't dereference output only arguments on failure
  if ((!returnOk && outputOnly()) || dup) {
    switch (argType_) {
    case argULONG_PTR:
    case argULONG:
    case argULONGLONG:
    case argENUM:
    case argMASK:
    case argBOOLEAN:
    case argACCESS_MASK:
    case argHANDLE:
      break;
    default:
      showPointer(os, hProcess, argVal);
      return;
    }
  }

  switch (argType_) {
  case argULONG_PTR:
  case argULONGLONG:
    showDword(os, argVal);
    break;

  case argULONG:
    showDword(os, static_cast<ULONG>(argVal));
    break;

  case argENUM:
    showEnum(os, static_cast<ULONG>(argVal), argTypeName_);
    break;

  case argMASK:
    showMask(os, static_cast<ULONG>(argVal), argTypeName_);
    break;

  case argBOOLEAN:
    showBoolean(os, static_cast<BOOLEAN>(argVal));
    break;

  case argBYTE:
    showDword(os, static_cast<BYTE>(argVal));
    break;

  case argHANDLE:
    showHandle(os, reinterpret_cast<HANDLE>(argVal));
    break;

  case argPOINTER:
    showPointer(os, hProcess, argVal);
    break;

  case argPOBJECT_ATTRIBUTES:
    showObjectAttributes(os, hProcess,
                         reinterpret_cast<POBJECT_ATTRIBUTES>(argVal));
    break;

  case argPUNICODE_STRING:
    showUnicodeString(os, hProcess, reinterpret_cast<PUNICODE_STRING>(argVal));
    break;

  case argPHANDLE:
    showPHandle(os, hProcess, argVal);
    break;

  case argPBYTE:
    showPByte(os, hProcess, argVal);
    break;

  case argPUSHORT:
    showPUshort(os, hProcess, argVal);
    break;

  case argPULONG:
    showPUlong(os, hProcess, argVal);
    break;

  case argACCESS_MASK:
    showAccessMask(os, hProcess, static_cast<ACCESS_MASK>(argVal),
                   argTypeName_);
    break;

  case argPCLIENT_ID:
    showPClientId(os, hProcess, reinterpret_cast<PCLIENT_ID>(argVal));
    break;

  case argPIO_STATUS_BLOCK:
    showPIoStatus(os, hProcess, reinterpret_cast<PIO_STATUS_BLOCK>(argVal));
    break;

  case argPLARGE_INTEGER:
    showPLargeInteger(os, hProcess, reinterpret_cast<PLARGE_INTEGER>(argVal));
    break;

  case argPLPC_MESSAGE:
    showPLpcMessage(os, hProcess, reinterpret_cast<PLPC_MESSAGE>(argVal));
    break;

  case argPFILE_BASIC_INFORMATION:
    showPFileBasicInfo(os, hProcess,
                       reinterpret_cast<PFILE_BASIC_INFORMATION>(argVal));
    break;

  case argPFILE_NETWORK_OPEN_INFORMATION:
    showPFileNetworkInfo(
        os, hProcess, reinterpret_cast<PFILE_NETWORK_OPEN_INFORMATION>(argVal));
    break;

  case argPRTL_USER_PROCESS_PARAMETERS:
    showUserProcessParams(
        os, hProcess, reinterpret_cast<PRTL_USER_PROCESS_PARAMETERS>(argVal));
    break;
  }
}

//////////////////////////////////////////////////////////////////////////
// true if argument is output-only
bool Argument::outputOnly() const {
  return (attributes_ & (argIN | argOUT)) == argOUT;
}

//////////////////////////////////////////////////////////////////////////
// Write argument to the output stream
void Argument::printOn(std::ostream &os) const {
  std::string const opt(attributes_ & argOPTIONAL ? "opt_" : "");
  if (attributes_ & argRESERVED)
    os << "_Reserved_ ";
  if ((attributes_ & (argIN | argOUT)) == (argIN | argOUT))
    os << "_Inout_" << opt << ' ';
  else if (attributes_ & argIN)
    os << "_In_" << opt << ' ';
  else if (attributes_ & argOUT)
    os << "_Out_" << opt << ' ';
  if (attributes_ & argCONST)
    os << "const ";
  os << argTypeName_ << " " << name_;
}

//////////////////////////////////////////////////////////////////////////
NtCall EntryPoint::insertBrkpt(HANDLE hProcess, unsigned char *address,
                               unsigned int offset, unsigned char *setssn) {
  // (The post-call code is at address + offset)
  // Looks like:-
  //  C2 20 00           ret         20h
  //  8B C0              mov         eax,eax  // optional padding
  //
  // or:-
  //  C3                 ret
  //
  // or:-
  //  E9 XX XX XX XX     jmp         commonExit

  NtCall nt;

  unsigned char instruction[8];

  if (!ReadProcessMemory(hProcess, address + offset, instruction, 8, nullptr)) {
    std::cerr << "Cannot read instructions for " << name_ << ": "
              << displayError() << std::endl;
    return NtCall();
  }

  switch (instruction[0]) {
  case RETn:
    nt.nArgs_ = (instruction[1] + instruction[2] * 256) / 4;

    if ((instruction[3] == MOVreg) && (instruction[4] == 0xc0)) {
      instruction[3] = instruction[2];
      instruction[2] = instruction[1];
      instruction[1] = instruction[0];
      instruction[0] = BRKPT;

      if (!WriteProcessMemory(hProcess, address + offset, instruction, 4,
                              nullptr)) {
        std::cerr << "Cannot write trap for " << name_ << ": " << displayError()
                  << std::endl;
        return NtCall();
      }
      nt.trapType_ = NtCall::trapContinue;
    } else {
      // We must replace the return itself
      instruction[0] = BRKPT;
      if (!WriteProcessMemory(hProcess, address + offset, instruction, 1,
                              nullptr)) {
        std::cerr << "Cannot write trap for " << name_ << ": " << displayError()
                  << std::endl;
        return NtCall();
      }
      nt.trapType_ = NtCall::trapReturn;
    }
    break;

  case RET:
    nt.nArgs_ = 0;

    // We must replace the return itself
    instruction[0] = BRKPT;
    if (!WriteProcessMemory(hProcess, address + offset, instruction, 1,
                            nullptr)) {
      std::cerr << "Cannot write trap for " << name_ << ": " << displayError()
                << std::endl;
      return NtCall();
    }
    nt.trapType_ = NtCall::trapReturn0;
    break;

  case JMP:
#ifdef _M_IX86
    nt.nArgs_ = 0; // UNKNOWN!

    // We must replace the jump itself
    instruction[0] = BRKPT;
    if (!WriteProcessMemory(hProcess, address + offset, instruction, 1, 0)) {
      std::cerr << "Cannot write trap for " << name_ << ": " << displayError()
                << std::endl;
      return NtCall();
    }
    nt.trapType_ = NtCall::trapJump;
    nt.jumpTarget_ =
        (DWORD)(*(DWORD *)(&instruction[1]) + address + offset + 5);

    // If the target is a return we can work out nArgs
    if (ReadProcessMemory(hProcess, (LPVOID)nt.jumpTarget_, instruction, 3,
                          0)) {
      if (instruction[0] == RETn) {
        nt.nArgs_ = *(short *)(&instruction[1]);
      } else if (instruction[0] == RET) {
        // ret [no args]
        nt.nArgs_ = 0;
      } else {
        std::cerr << "Warning: unknown arg count for " << name_ << std::endl;
        nt.nArgs_ = getArgumentCount(); // "Trust me"
      }
    } else {
      std::cerr << "Warning: can't read target for " << name_ << " at "
                << nt.jumpTarget_ << std::endl;
    }
#else
    std::cerr << "Cannot trap " << name_ << " - wrong signature ('jmp' 0xE9)"
              << std::endl;
#endif // _M_IX86
    break;

  default:
    std::cerr << "Cannot trap " << name_
              << " - wrong signature (expecting 'ret' 0xC2/0xC3 or 'jmp' 0xE9, "
                 "found 0x"
              << std::hex << std::setw(2) << (int)instruction[0] << std::dec
              << ")" << std::endl;
    return NtCall();
  }

  // Now we know the actual argument count...
  size_t nKnown(getArgumentCount());
  if (nt.nArgs_ > nKnown) {
    setArgumentCount(nt.nArgs_);
    if (nKnown) {
      size_t nExtra = nt.nArgs_ - nKnown;
      std::cerr << "Warning: " << nExtra << " additional argument"
                << (nExtra == 1 ? "" : "s") << " for " << name_ << std::endl;
    }
  } else if (nt.nArgs_ < nKnown) {
    if (nt.nArgs_ > 0) {
      size_t nExtra = nKnown - nt.nArgs_;
      std::cerr << "Warning: " << nExtra << " spurious argument"
                << (nExtra == 1 ? "" : "s") << " for " << name_ << std::endl;
    }
  }
  setAddress(address + offset);

  if (setssn) {
    instruction[0] = BRKPT;
    instruction[1] = NOP;
    instruction[2] = NOP;
    instruction[3] = NOP;
    instruction[4] = NOP;

    if (!WriteProcessMemory(hProcess, setssn, instruction, 5, nullptr)) {
      std::cerr << "Cannot write trap for " << name_ << ": " << displayError()
                << std::endl;
      return NtCall();
    }
    setPreSave(setssn);
  }

  nt.entryPoint_ = this;

  return nt;
}

//////////////////////////////////////////////////////////////////////////
// Attempt to set a trap for the entry point in the target DLL.
NtCall EntryPoint::setNtTrap(HANDLE hProcess, HMODULE hTargetDll,
                             bool bPreTrace, DWORD dllOffset, bool verbose) {
#ifdef _M_X64
  // We need the pretrace on X64 to save the volatile registers
  bPreTrace = true;
#endif // _M_X64

  unsigned char *address;
  if (dllOffset != 0) {
    address = reinterpret_cast<unsigned char *>(hTargetDll) + dllOffset;
  } else {
    FARPROC pProc = GetProcAddress(hTargetDll, name_.c_str());
    if (nullptr == pProc) {
      DWORD errorCode = GetLastError();
      if (errorCode == ERROR_PROC_NOT_FOUND) {
        if (!exported_.empty() &&
            (pProc = GetProcAddress(hTargetDll, exported_.c_str())) !=
                nullptr) {
          // Found entry point using the exported name
        } else {
          // Entry points are allowed to be absent!
          if (verbose) {
            std::cout << "Unable to locate " << name_ << "\n";
          }
          return NtCall();
        }

      } else {
        std::cerr << "Cannot resolve " << name_ << ": "
                  << displayError(errorCode) << std::endl;
        return NtCall();
      }
    }
    address = reinterpret_cast<unsigned char *>(pProc);
  }

  unsigned int preamble = 0;
  unsigned char instruction[MAX_PREAMBLE];
  if (!ReadProcessMemory(hProcess, address, instruction, sizeof(instruction),
                         nullptr)) {
    std::cerr << "Cannot trap " << name_ << " - unable to read memory at "
              << (void *)address << ": " << displayError() << std::endl;
    return NtCall();
  }

  // Check for indirect jump (eg Windows 10 NtUserXxx moved from user32.dll to
  // win32u.dll) Note that at this point when loading the DLL the target address
  // has not yet been resolved so we cannot (easily) follow the jump to its
  // target
  if (instruction[0] == Call && instruction[1] == Indirect) {
    std::cerr << "Cannot trap " << name_
              << " (maybe implemented in another DLL)" << std::endl;
    return NtCall();
  }

  if (optional_ && deadExport(instruction, MAX_PREAMBLE)) {
    if (verbose) {
      std::cout << "Dead export found for: " << name_ << '\n';
    }
    return NtCall();
  }

  unsigned char *setssn = nullptr;
  for (auto pCheck : signatures) {
    unsigned int offset = 0;
    setssn = nullptr;
    for (; *pCheck != 0; pCheck += 2) {
      if (instruction[offset] == BRKPT) {
        // already pre-trace trapping!
        preamble = offset;
        break;
      }
      if (instruction[offset] != pCheck[0])
        break;
      if (instruction[offset] == MOVdwordEax) {
        setssn = address + offset;
      }
      offset += pCheck[1];
    }
    if (pCheck[0] == 0) {
      // Check for possible esp adjustment
      if (instruction[offset] == AddEsp) {
        offset += 3;
      }
      preamble = offset;
      break;
    }
  }

  if (instruction[preamble] == BRKPT) {
    std::cerr << "Already trapping: " << name_ << std::endl;
    return NtCall();
  } else if (preamble == 0) {
    std::cerr << "Cannot trap " << name_
              << " - wrong signature: " << buffToHex(instruction, MAX_PREAMBLE)
              << std::endl;
    return NtCall();
  } else if (setssn == nullptr) {
    std::cerr << "Cannot trap " << name_
              << " - cannot find system service number" << std::endl;
    return NtCall();
  }

  memcpy(&ssn_, instruction + (setssn - address) + 1, sizeof(ssn_));
  if (verbose) {
    std::cout << "Instrumenting " << name_ << " at: " << (void *)address
              << ", ssn: 0x" << std::hex << ssn_ << std::dec << "\n";
  }
  return insertBrkpt(hProcess, address, preamble, bPreTrace ? setssn : nullptr);
}

//////////////////////////////////////////////////////////////////////////
// Attempt to set a trap for the entry point in the target DLL.
bool EntryPoint::clearNtTrap(HANDLE hProcess, NtCall const &ntCall) const {
  if (preSave_) {
    char instruction[1 + 4];
    instruction[0] = MOVdwordEax;
    memcpy(instruction + 1, &ssn_, sizeof(ssn_));
    if (!WriteProcessMemory(hProcess, preSave_, instruction, 5, nullptr)) {
      std::cerr << "Cannot clear trap for " << name_ << ": " << displayError()
                << std::endl;
      return false;
    }
  }

  if (targetAddress_) {
    char instruction[4];
    int len(0);

    switch (ntCall.trapType_) {
    case NtCall::trapContinue:
      instruction[0] = RETn;
      instruction[1] = static_cast<unsigned char>(ntCall.nArgs_ * 4);
      instruction[2] = static_cast<unsigned char>(ntCall.nArgs_ * 4 / 256);
      instruction[3] = MOVreg;
      len = 4;
      break;

    case NtCall::trapReturn:
      instruction[0] = RETn;
      len = 1;
      break;

    case NtCall::trapReturn0:
      instruction[0] = RET;
      len = 1;
      break;

    case NtCall::trapJump:
      instruction[0] = JMP;
      len = 1;
      break;
    }
    if (len) {
      if (!WriteProcessMemory(hProcess, targetAddress_, instruction, len,
                              nullptr)) {
        std::cerr << "Cannot clear trap for " << name_ << ": " << displayError()
                  << std::endl;
        return false;
      }
    }
  }
  return true;
}

//////////////////////////////////////////////////////////////////////////
// Eg "NtOpenFile", 2, "POBJECT_ATTRIBUTES", "ObjectAttributes", argIN
void EntryPoint::setArgument(int argNum, ArgType eArgType,
                             std::string const &argType,
                             std::string const &variableName,
                             ArgAttributes attributes) {
  if (argNum >= (int)arguments_.size())
    arguments_.resize(argNum + 1);
  arguments_[argNum] = Argument(eArgType, argType, variableName, attributes);
}

//////////////////////////////////////////////////////////////////////////
// Eg 2nd stack argument for "POINT"
void EntryPoint::setDummyArgument(int argNum, ArgAttributes attributes) {
  if (argNum >= (int)arguments_.size())
    arguments_.resize(argNum + 1);
  arguments_[argNum] =
      Argument(argULONG_PTR, std::string(), std::string(), attributes);
  arguments_[argNum].setDummy(true);
}

//////////////////////////////////////////////////////////////////////////
// Eg "NTSTATUS"
void EntryPoint::setReturnType(std::string const &typeName,
                               Typedefs const &typedefs) {
  static const struct {
    ReturnType eRetType_;
    char const *retTypeName_;
  } retTypes[] = {
      {retNTSTATUS, "NTSTATUS"},

      {retVOID, "VOID"},

      {retPVOID, "PVOID"},         {retPVOID, "HANDLE"},

      {retULONG, "BOOL"},          {retULONG, "DWORD"},
      {retULONG, "UINT"},          {retULONG, "ULONG"},
      {retULONG, "INT"},           {retULONG, "int"},
      {retULONG, "LONG"},          {retULONG, "long"},
      {retULONG, "WORD"},

      {retULONG_PTR, "UINT_PTR"},  {retULONG_PTR, "INT_PTR"},
      {retULONG_PTR, "ULONG_PTR"}, {retULONG_PTR, "LONG_PTR"},
  };

  Typedefs::const_iterator it = typedefs.find(typeName);
  std::string const alias = (it == typedefs.end() ? std::string() : it->second);
  ReturnType eRetType = retNTSTATUS;
  bool found(false);
  for (auto type : retTypes) {
    if ((typeName == type.retTypeName_) || (alias == type.retTypeName_)) {
      found = true;
      eRetType = type.eRetType_;
      break;
    }
  }

  if (!found) {
    if (typeName.find('*') != std::string::npos) {
      retType_ = retPVOID;
    } else {
      std::cerr << "Assuming NTSTATUS return for: " << typeName << std::endl;
    }
  }

  retType_ = eRetType;
  retTypeName_ = typeName;
}

//////////////////////////////////////////////////////////////////////////
// Handle pre-saving register arguments before the fast call

void EntryPoint::doPreSave(HANDLE hProcess, HANDLE hThread,
                           CONTEXT const &Context) {
#ifdef _M_X64
  CONTEXT newContext = Context;
  newContext.ContextFlags = CONTEXT_INTEGER;
  newContext.Rax = ssn_;
  if (!SetThreadContext(hThread, &newContext)) {
    std::cerr << "Can't set thread context: " << displayError() << std::endl;
  }
  ULONG_PTR saveArea[4];
  saveArea[0] = Context.Rcx;
  saveArea[1] = Context.Rdx;
  saveArea[2] = Context.R8;
  saveArea[3] = Context.R9;
  PVOID saveTarget = (PVOID)(Context.Rsp + sizeof(ULONG_PTR));
  if (!WriteProcessMemory(hProcess, saveTarget, saveArea, sizeof(saveArea),
                          nullptr)) {
    std::cerr << "Can't save register values at " << saveTarget << ": "
              << displayError() << std::endl;
  }
#else
  // Unused arguments
  hProcess;
  CONTEXT newContext = Context;
  newContext.ContextFlags = CONTEXT_INTEGER;
  newContext.Eax = ssn_;
  if (!SetThreadContext(hThread, &newContext)) {
    std::cerr << "Can't set thread context: " << displayError() << std::endl;
  }
#endif // _M_X64
}

//////////////////////////////////////////////////////////////////////////
// Trace a call to the entry point
void EntryPoint::trace(std::ostream &os, HANDLE hProcess, HANDLE hThread,
                       CONTEXT const &Context, bool bNames, bool bStackTrace,
                       bool before) const {
#ifdef _M_IX86
  DWORD stack = Context.Esp;
  DWORD returnCode = Context.Eax;
#elif _M_X64
  DWORD64 stack = Context.Rsp;
  DWORD64 returnCode = Context.Rax;
#endif
  os << getName() << "(";

  bool success(false);

  switch (retType_) {
  case retNTSTATUS:
    success = NT_SUCCESS(returnCode);
    break;
  case retULONG:
    success = ((ULONG)returnCode != 0);
    break;
  case retULONG_PTR:
    success = (returnCode != 0);
    break;
  default:
    break;
  }

  if (getArgumentCount()) {
    std::set<Argument::ARG> args;
    std::vector<Argument::ARG> argv(getArgumentCount());
    if (!ReadProcessMemory(
            hProcess, (LPVOID)(stack + sizeof(Argument::ARG)), &argv[0],
            sizeof(sizeof(Argument::ARG)) * argv.size(), nullptr)) {
      os << "read error: " << GetLastError() << std::endl;
      return;
    }

    for (size_t i = 0, end = getArgumentCount(); i < end; i++) {
      Argument::ARG argVal = argv[i];
      Argument const &argument = getArgument(i);
      if (i)
        os << ", ";
      bool const dup = !args.insert(argVal).second;
      argument.showArgument(os, hProcess, argVal, !before && success, dup,
                            bNames);
    }
  }

  if (before) {
    os << ") ...";
  } else {
    os << ") => ";
    showDword(os, returnCode);

    if (IS_ERROR(returnCode) && retType_ == retNTSTATUS) {
      showWinError(os, static_cast<HRESULT>(RtlNtStatusToDosError(returnCode)));
    }
    if (bStackTrace) {
      os << std::endl;
      printStackTrace(os, hProcess, hThread, Context);
    }
  }
  os << std::endl;
}

//////////////////////////////////////////////////////////////////////////
// Sort by category and then by name
bool EntryPoint::operator<(EntryPoint const &rhs) const {
  if (category_ < rhs.category_)
    return true;
  if (category_ == rhs.category_)
    return name_ < rhs.name_;
  return false;
}

//////////////////////////////////////////////////////////////////////////
// Print a stack trace for the supplied thread and process
// static
void EntryPoint::stackTrace(std::ostream &os, HANDLE hProcess, HANDLE hThread) {
  CONTEXT Context;
  Context.ContextFlags = CONTEXT_FULL;
  if (!GetThreadContext(hThread, &Context)) {
    os << "Can't get thread context: " << displayError() << std::endl;
  } else {
    printStackTrace(os, hProcess, hThread, Context);
  }
}

namespace {
void printStackTrace(std::ostream &os, HANDLE hProcess, HANDLE hThread,
                     CONTEXT const &Context) {
  static std::map<HANDLE, or2::SymbolEngine *> engines;

  or2::SymbolEngine *pEngine = engines[hProcess];
  if (pEngine == nullptr) {
    pEngine = new or2::SymbolEngine(hProcess);
    // Ensure ntdll.dll is in place (early on dbghelp doesn't find it)
    pEngine->LoadModule64(nullptr, "ntdll.dll", nullptr,
                          (DWORD64)GetModuleHandle("ntdll.dll"), 0);
    engines[hProcess] = pEngine;
  }
  pEngine->StackTrace(hThread, Context, os);
}

bool isBlankOrComment(std::string const &lbuf) {
  return ((lbuf.length() == 0) || (lbuf[0] == ';') || (lbuf[0] == '#'));
}

// Convert buffer to hex characters, eg "[fe 00 ab]"
std::string buffToHex(unsigned char *buffer, size_t length) {
  std::ostringstream oss;
  oss << std::setfill('0') << std::hex << '[';
  for (size_t idx = 0; idx != length; ++idx) {
    unsigned int value = buffer[idx];
    if (idx)
      oss << " ";
    oss << std::setw(2) << value;
  }
  oss << ']';
  return oss.str();
}

std::map<std::string, ArgType> getArgTypes() {

  static const struct {
    ArgType eArgType_;
    char const *argTypeName_;
  } argTypes[] = {
      {argULONG_PTR, "ULONG_PTR"},
      {argULONG_PTR, "LONG_PTR"},
      {argULONG_PTR, "UINT_PTR"},
      {argULONG_PTR, "INT_PTR"},
      {argULONG_PTR, "HANDLE"},
      {argULONG_PTR, "SIZE_T"},
      {argULONG_PTR, "NTSTATUS"},

      {argULONG, "ULONG"},
      {argULONG, "long"},
      {argULONG, "LONG"},
      {argULONG, "UINT"},
      {argULONG, "int"},
      {argULONG, "INT"},
      {argULONG, "DWORD"},
      {argULONG, "WORD"},
      {argULONG, "USHORT"},
      {argULONG, "WCHAR"},

      {argULONG, "LCID"},

      {argULONGLONG, "ULONGLONG"},

      {argBYTE, "BYTE"},

      // Enumerations obtained from Enumerations (see below)

      {argMASK, "NOTIFICATION_MASK"},
      {argMASK, "SECURITY_INFORMATION"},

      {argBOOLEAN, "BOOLEAN"},
      {argBOOLEAN, "BOOL"},

      {argACCESS_MASK, "ACCESS_MASK"},
      {argACCESS_MASK, "DIRECTORY_ACCESS_MASK"},
      {argACCESS_MASK, "EVENT_ACCESS_MASK"},
      {argACCESS_MASK, "FILE_ACCESS_MASK"},
      {argACCESS_MASK, "JOB_ACCESS_MASK"},
      {argACCESS_MASK, "KEY_ACCESS_MASK"},
      {argACCESS_MASK, "MUTANT_ACCESS_MASK"},
      {argACCESS_MASK, "PROCESS_ACCESS_MASK"},
      {argACCESS_MASK, "SECTION_ACCESS_MASK"},
      {argACCESS_MASK, "SEMAPHORE_ACCESS_MASK"},
      {argACCESS_MASK, "TIMER_ACCESS_MASK"},
      {argACCESS_MASK, "THREAD_ACCESS_MASK"},
      {argACCESS_MASK, "TOKEN_ACCESS_MASK"},

      {argHANDLE, "HANDLE"},

      {argPOINTER, "PVOID"},
      {argPOINTER, "PSTR"},
      {argPOINTER, "PWSTR"},

      {argPHANDLE, "PHANDLE"},
      {argPHANDLE, "PPVOID"},
      {argPHANDLE, "PSIZE_T"},

      {argPBYTE, "PBYTE"},
      {argPBYTE, "PUCHAR"},

      {argPUSHORT, "PUSHORT"},
      {argPUSHORT, "PWORD"},

      {argPULONG, "PULONG"},
      {argPULONG, "PDWORD"},
      {argPULONG, "PINT"},
      {argPULONG, "PUINT"},
      {argPULONG, "PULONGLONG"},
      {argPULONG, "PULONG_PTR"},
      {argPULONG, "PBOOLEAN"},
      {argPULONG, "PLCID"},

      {argPCLIENT_ID, "PCLIENT_ID"},
      {argPFILE_BASIC_INFORMATION, "PFILE_BASIC_INFORMATION"},
      {argPFILE_NETWORK_OPEN_INFORMATION, "PFILE_NETWORK_OPEN_INFORMATION"},
      {argPIO_STATUS_BLOCK, "PIO_STATUS_BLOCK"},
      {argPLARGE_INTEGER, "PLARGE_INTEGER"},
      {argPLPC_MESSAGE, "PLPC_MESSAGE"},
      {argPOBJECT_ATTRIBUTES, "POBJECT_ATTRIBUTES"},
      {argPRTL_USER_PROCESS_PARAMETERS, "PRTL_USER_PROCESS_PARAMETERS"},
      {argPUNICODE_STRING, "PUNICODE_STRING"},
  };

  std::map<std::string, ArgType> result;

  for (auto idx : argTypes) {
    result[idx.argTypeName_] = idx.eArgType_;
  }

  // Add the known enumerations, without other handling
  for (Enumerations::AllEnum *p = Enumerations::allEnums; p->name_; ++p) {
    result.insert(std::make_pair(p->name_, argENUM));
  }

  return result;
}

ArgType getArgType(const std::string typeName,
                   EntryPoint::Typedefs const &typedefs) {
  static const std::map<std::string, ArgType> argTypes = getArgTypes();

  // First try the type name
  auto it = argTypes.find(typeName);
  if (it != argTypes.end()) {
    return it->second;
  }

  // Then use any typedef alias for the type name
  const auto alias_it = typedefs.find(typeName);
  if (alias_it != typedefs.end()) {
    it = argTypes.find(alias_it->second);
    if (it != argTypes.end()) {
      return it->second;
    }
  }

  std::cerr << "Assuming ULONG for: " << typeName << std::endl;
  return argULONG_PTR;
}

// win32u implements some "dead export" logic (the functions
// simply call RaiseFailFastException)
bool deadExport(unsigned char instruction[], size_t length) {
  for (auto pCheck : dead_exports) {
    unsigned int offset = 0;
    for (; *pCheck != 0; pCheck += 2) {
      if (offset >= length)
        break;
      if (instruction[offset] != pCheck[0])
        break;
      offset += pCheck[1];
    }
    if (pCheck[0] == 0) {
      return true;
    }
  }
  return false;
}

} // namespace

// Process a typedef line (starting after the 'typedef')
void processTypedef(std::string lbuf, EntryPoint::Typedefs &typedefs) {
  std::string::size_type space = lbuf.find(' ');
  if (space == std::string::npos) {
    std::cerr << "invalid typedef '" << lbuf << "'" << std::endl;
    return;
  }
  if (lbuf.rfind(';') == lbuf.size() - 1) {
    lbuf.resize(lbuf.size() - 1);
  }

  typedefs[lbuf.substr(space + 1)] = lbuf.substr(0, space);
}

// Process a using declaration (starting after the 'using')
void processUsing(std::string lbuf, EntryPoint::Typedefs &typedefs) {
  std::string::size_type equals = lbuf.find(" = ");
  if (equals == std::string::npos) {
    std::cerr << "invalid using '" << lbuf << "'" << std::endl;
    return;
  }
  if (lbuf.rfind(';') == lbuf.size() - 1) {
    lbuf.resize(lbuf.size() - 1);
  }

  typedefs[lbuf.substr(0, equals)] = lbuf.substr(equals + strlen(" = "));
}
//////////////////////////////////////////////////////////////////////////
//
// Read set of entry points from a configuration file with lines like:-
// NTSTATUS
// NTAPI
// NtXxx(
//   _In_ DWORD fred
// );
//
// or (simple) typedefs or using declarations:
// typedef HANDLE HKL;
// using NTSTATUS = LONG;
//
// static
bool EntryPoint::readEntryPoints(std::istream &cfgFile,
                                 EntryPointSet &entryPoints, Typedefs &typedefs,
                                 std::string &target) {
  using FunctionMap = std::map<std::string, EntryPoint *>;
  FunctionMap existingFunctions; // For handling duplicate definitions
  std::string sCategory("Other");
  std::string lastTypeName;
  std::string lbuf;
  int argNum = -1;
  EntryPoint *currEntryPoint = nullptr;
  int lineNo = 0;
  while (std::getline(cfgFile, lbuf)) {
    lineNo++;
    if (isBlankOrComment(lbuf)) {
      continue; // Skip blank lines and comments
    }
    if (lbuf.substr(0, 2) == "//") {
      size_t const len(lbuf.size());
      if ((len > 4) && (lbuf[2] == '[') && (lbuf[len - 1] == ']')) {
        std::string const argument = lbuf.substr(3, len - 4);
        if (argument.find('.') != std::string::npos) {
          // set target as the first DLL that we can load
          // to support NtUser moving from user32 -> win32u
          if (target.empty() && ::LoadLibrary(argument.c_str())) {
            target = argument;
          }
        } else if (argument.find('=') == 0) {
          // Exported name for current function
          if (currEntryPoint) {
            currEntryPoint->setExported(argument.substr(1));
          } else {
            std::cerr << "unexpected export line '" << lbuf << "'" << std::endl;
          }
        } else {
          sCategory = argument;
        }
      }
      continue; // Skip this comment
    }
    if (argNum == -1) {
      if (lbuf == "NTAPI" || lbuf == "WINUSERAPI" || lbuf == "WINAPI") {
        // ignore API function decorations
        continue;
      }
      if ((lbuf.find(' ') == strlen("typedef")) &&
          (memcmp(&lbuf[0], "typedef", 7) == 0)) {
        processTypedef(lbuf.substr(strlen("typedef") + 1), typedefs);
        continue;
      }
      if ((lbuf.find(' ') == strlen("using")) &&
          (memcmp(&lbuf[0], "using", 5) == 0)) {
        processUsing(lbuf.substr(strlen("using") + 1), typedefs);
        continue;
      }
      std::string::size_type idx = lbuf.find('(');
      if (idx == std::string::npos) {
        // only a name
        if (lastTypeName.empty()) {
          lastTypeName = lbuf;
        } else {
          std::cerr << "unexpected input '" << lbuf << "'" << std::endl;
        }
        continue;
      } else {
        // a function prototype
        if (lbuf.find(')', idx) == std::string::npos) {
          // Arguments to follow
          argNum = 0;
        }
        lbuf = lbuf.substr(0, idx);
      }
      // just a raw name....
      std::istringstream is(lbuf);
      std::string functionName;
      is >> functionName;

      // Add the new entry point (or update the existing one)
      currEntryPoint = existingFunctions[functionName];
      if (currEntryPoint == nullptr) {
        currEntryPoint = const_cast<EntryPoint *>(
            &*entryPoints.insert(EntryPoint(functionName, sCategory))
                  .first); // std::set constness
        existingFunctions[functionName] = currEntryPoint;
      }
      if (!lastTypeName.empty()) {
        currEntryPoint->setReturnType(lastTypeName, typedefs);
        lastTypeName.resize(0);
      }
    } else {
      bool bEnded = false;
      std::string::size_type const closing_bracket = lbuf.find(')');
      if (closing_bracket != std::string::npos) {
        bEnded = true;
        lbuf.resize(closing_bracket);
      }
      std::string::size_type idx = lbuf.find(',');
      if (idx != std::string::npos) {
        lbuf.resize(idx);
      }
      std::istringstream is(lbuf);
      int attributes = argNONE;
      std::string typeName;
      std::string variableName;

      do {
        is >> typeName;
        const auto it = sal_attributes.find(typeName);
        if (it != sal_attributes.end()) {
          attributes |= it->second;
        } else
          break;
      } while (is);
      if (bEnded &&
          (typeName.length() == 0 || (argNum == 0 && typeName == "VOID"))) {
        // ignore single trailing ")" or "VOID"
      } else {
        if (!is) {
          std::cerr << "Syntax error at line " << lineNo
                    << " reading config file" << std::endl;
          return false;
        }
        is >> variableName;
        std::string optional;
        is >> optional;
        if (optional == "OPTIONAL")
          attributes |= argOPTIONAL;

        const ArgType eArgType = getArgType(typeName, typedefs);
        currEntryPoint->setArgument(argNum, eArgType, typeName, variableName,
                                    (ArgAttributes)attributes);
#ifdef _M_IX86
        if (eArgType == argULONGLONG) {
          // Insert an unnamed dummy argument for the high dword
          argNum++;
          currEntryPoint->setDummyArgument(argNum, (ArgAttributes)attributes);
        }
#endif // _M_IX86
        argNum++;
      }
      if (bEnded) {
        // Done with this function
        argNum = -1;
        currEntryPoint = nullptr;
      }
    }
  }
  return true;
}

//////////////////////////////////////////////////////////////////////////
// Print self to a stream, as a function prototype
void EntryPoint::writeExport(std::ostream &os) const {
  if (targetAddress_ == nullptr && !disabled_)
    os << "//inactive\n";
  os << "//[" << (disabled_ ? "-" : "") << (optional_ ? "?" : "") << category_
     << "]\n";
  if (retType_ == retNTSTATUS) {
    os << "NTSTATUS";
  } else {
    os << retTypeName_;
  }
  os << "\nNTAPI\n" << name_ << "(\n";
  // Write the export alias if there is one
  if (!exported_.empty()) {
    os << "//[=" << exported_ << "]\n";
  }

  for (size_t i = 0, end = arguments_.size(); i != end; i++) {
    Argument const &argument = arguments_[i];
    if (argument.isDummy()) {
      continue;
    }
    os << "    " << argument;

    if (i != end - 1)
      os << ",";
    os << "\n";
  }
  os << "    );" << std::endl;
}
