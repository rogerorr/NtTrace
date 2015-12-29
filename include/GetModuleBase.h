#ifndef GETMODULEBASE_H_
#define GETMODULEBASE_H_

/**@file

    Function to provide an implementation of SymGetModuleBase.

    @author Roger Orr <rogero@howzatt.demon.co.uk>

    Copyright &copy; 2003.
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

// $Id: GetModuleBase.h 1405 2013-11-12 23:57:17Z Roger $

#ifndef _WINDOWS_
#include <windows.h>
#endif // _WINDOWS_

/**
 * GetModuleBase: try to get information about a module being loaded.
 * @param hProcess handle to the target process
 * @param dwAddress the target address
 * @return the base address of the module, or zero if not identified
 */
DWORD64 CALLBACK GetModuleBase( HANDLE hProcess, DWORD64 dwAddress );

/**
 * Get module file name, correcting for a couple of common issues.
 *
 * @param hProcess the process to query
 * @param hMod the module to query
 * @param szBuff the output filename buffer
 * @param bufLen the size of the output buffer
 * @returns the length of the string copied to the buffer, or zero on failure, 
 * in which case call GetLastError for the underlying error code.
 */
DWORD GetModuleFileNameWrapper( HANDLE hProcess, HMODULE hMod, char * szBuff, DWORD bufLen );

#endif // GETMODULEBASE_H_
