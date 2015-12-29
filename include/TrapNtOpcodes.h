#ifndef TRAPNTOPCODES_H_
#define TRAPNTOPCODES_H_

/**@file

    List of opcodes used by NtTrace to trap calls in NTDLL.

    @author Roger Orr <rogero@howzatt.demon.co.uk>

    Copyright &copy; 2002,2003.
    This software is distributed in the hope that it will be useful, but
    without WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    Permission is granted to anyone to make or distribute verbatim
    copies of this software provided that the copyright notice and
    this permission notice are preserved, and that the distributor
    grants the recipient permission for further distribution as permitted
    by this notice.

    Comments and suggestions are always welcome.
    Please report bugs to rogero@howzatt.demon.co.uk.

    $Revision: 1405 $
*/

// $Id: TrapNtOpcodes.h 1405 2013-11-12 23:57:17Z Roger $

// Define the op codes used for NtTrace
static unsigned char const XOR = 0x33;         ///< xor reg1,reg2
static unsigned char const AddEsp = 0x83;      ///< add esp,04h
static unsigned char const MOVreg = 0x8B;      ///< mov reg1,reg2
static unsigned char const LEA = 0x8D;         ///< lea reg1,operand
static unsigned char const NOP = 0x90;         ///< nop
static unsigned char const MOVdwordEax = 0xB8; ///< mov eax,dword
static unsigned char const MOVdwordEcx = 0xB9; ///< mov ecx,dword
static unsigned char const MOVdwordEdx = 0xBA; ///< mov edx,dword
static unsigned char const CallReg = 0xff;     ///< call edx
static unsigned char const RETn = 0xC2;        ///< ret n
static unsigned char const RET = 0xC3;         ///< ret
static unsigned char const BRKPT = 0xCC;       ///< breakpoint
static unsigned char const INTn = 0xCD;        ///< interrupt
static unsigned char const JMP = 0xE9;         ///< far jump
static unsigned char const FS = 0x64;          ///< fs:

#endif // TRAPNTOPCODES_H_