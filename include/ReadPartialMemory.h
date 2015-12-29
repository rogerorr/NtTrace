#ifndef READPARTIALMEMORY_H_
#define READPARTIALMEMORY_H_

/**@file

    Function to read partial memory buffer from a target process

    @author Roger Orr <rogero@howzatt.demon.co.uk>

    Copyright &copy; 2011.
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

// $Id: ReadPartialMemory.h 1405 2013-11-12 23:57:17Z Roger $

#include <Windows.h>

namespace or2
{
  /**
   * Reads data from an area of memory in a specified process.
   *
   * The area to be read need only be partially accessible.
   * The function reads the largest contiguous amount of data from 'address' and
   * succeeds if this length is in [minSize, maxSize].
   *
   * @return the number of bytes read.  Zero implies failure - use GetLastError() for details
   */
  inline
  SIZE_T ReadPartialProcessMemory(
    HANDLE hProcess, ///< Handle to process to read (needs PROCESS_VM_READ access)
    LPCVOID address, ///< Base address of area to read
    LPVOID buffer,   ///< Buffer to hold read data
    SIZE_T minSize,  ///< Minimum amount to read for success
    SIZE_T maxSize)  ///< Maximum size of buffer
  {
    SIZE_T length = maxSize;
    while (length >= minSize)
    {
      if ( ReadProcessMemory(hProcess, address, buffer, length, 0) )
      {
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


} // namespace

#endif // READPARTIALMEMORY_H_
