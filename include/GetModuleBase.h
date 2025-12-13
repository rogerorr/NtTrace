#ifndef GETMODULEBASE_H_
#define GETMODULEBASE_H_

/**@file

  Function to provide an implementation of SymGetModuleBase.

  @author Roger Orr mailto:rogero@howzatt.co.uk
  Bug reports, comments, and suggestions are always welcome.

  Copyright &copy; 2003 under the MIT license:

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

  $Revision: 2950 $
*/

// $Id: GetModuleBase.h 2950 2025-12-06 16:10:34Z roger $

#ifndef _WINDOWS_
#include <windows.h>
#endif // _WINDOWS_
#include <string>

/**
 * GetModuleBase: try to get information about a module being loaded.
 * @param hProcess handle to the target process
 * @param dwAddress the target address
 * @return the base address of the module, or zero if not identified
 */
DWORD64 CALLBACK GetModuleBase(HANDLE hProcess, DWORD64 dwAddress);

/**
 * Get module file name, correcting for a couple of common issues.
 *
 * @param hProcess the process to query
 * @param hMod the module to query
 * @returns the filename, which is empty on failure,
 * in which case call GetLastError for the underlying error code.
 */
std::string GetModuleFileNameWrapper(HANDLE hProcess, HMODULE hMod);

#endif // GETMODULEBASE_H_
