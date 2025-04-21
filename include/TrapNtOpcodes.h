#ifndef TRAPNTOPCODES_H_
#define TRAPNTOPCODES_H_

/**@file

  List of opcodes used by NtTrace to trap calls in NTDLL.

  @author Roger Orr mailto:rogero@howzatt.co.uk
  Bug reports, comments, and suggestions are always welcome.

  Copyright &copy; 2002, 2013 under the MIT license:

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

  $Revision: 2716 $
*/

// $Id: TrapNtOpcodes.h 2716 2025-04-21 22:23:58Z roger $

// Define the op codes used for NtTrace
static unsigned char const XOR = 0x33;         ///< xor reg1,reg2
static unsigned char const AddEsp = 0x83;      ///< add esp,04h
static unsigned char const MOVreg = 0x8B;      ///< mov reg1,reg2
static unsigned char const LEA = 0x8D;         ///< lea reg1,operand
static unsigned char const NOP = 0x90;         ///< nop
static unsigned char const MOVdwordEax = 0xB8; ///< mov eax,dword
static unsigned char const MOVdwordEcx = 0xB9; ///< mov ecx,dword
static unsigned char const MOVdwordEdx = 0xBA; ///< mov edx,dword
static unsigned char const Call = 0xff;        ///< call prefix
static unsigned char const Indirect = 0x25;    ///< indirect call modifier
static unsigned char const RETn = 0xC2;        ///< ret n
static unsigned char const RET = 0xC3;         ///< ret
static unsigned char const BRKPT = 0xCC;       ///< breakpoint
static unsigned char const INTn = 0xCD;        ///< interrupt
static unsigned char const JMP = 0xE9;         ///< far jump
static unsigned char const FS = 0x64;          ///< fs:

#endif // TRAPNTOPCODES_H_