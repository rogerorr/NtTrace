#ifndef READPARTIALMEMORY_H_
#define READPARTIALMEMORY_H_

/**@file

  Function to read partial memory buffer from a target process

  @author Roger Orr mailto:rogero@howzatt.co.uk
  Bug reports, comments, and suggestions are always welcome.

  Copyright &copy; 2011 under the MIT license:

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

  $Revision: 2675 $
*/

// $Id: ReadPartialMemory.h 2675 2025-04-21 16:52:13Z roger $

#include <Windows.h>

namespace or2 {
/**
 * Reads data from an area of memory in a specified process.
 *
 * The area to be read need only be partially accessible.
 * The function reads the largest contiguous amount of data from 'address' and
 * succeeds if this length is in [minSize, maxSize].
 *
 * @return the number of bytes read.  Zero implies failure - use GetLastError()
 * for details
 */
inline SIZE_T
ReadPartialProcessMemory(HANDLE hProcess, ///< Handle to process to read (needs
                                          ///< PROCESS_VM_READ access)
                         LPCVOID address, ///< Base address of area to read
                         LPVOID buffer,   ///< Buffer to hold read data
                         SIZE_T minSize, ///< Minimum amount to read for success
                         SIZE_T maxSize) ///< Maximum size of buffer
{
  SIZE_T length = maxSize;
  while (length >= minSize) {
    if (ReadProcessMemory(hProcess, address, buffer, length, nullptr)) {
      return length;
    }
    length--;

    static SYSTEM_INFO SystemInfo;
    static BOOL b = (GetSystemInfo(&SystemInfo), TRUE);

    SIZE_T pageOffset = ((ULONG_PTR)address + length) % SystemInfo.dwPageSize;
    if (pageOffset > length)
      break;
    length -= pageOffset;
  }
  return 0;
}

} // namespace or2

#endif // READPARTIALMEMORY_H_
