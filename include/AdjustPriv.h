#ifndef ADJUST_PRIV_H_
#define ADJUST_PRIV_H_

/**@file

    Function to grant a single privilege to a process.

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

// $Id: AdjustPriv.h 1405 2013-11-12 23:57:17Z Roger $

namespace or2
{

/**
 * Attempt to grant the named privilege to the specified process.
 *
 * @return
 * - TRUE - success
 * - FALSE - failure [typically the process hasn't been granted the privilege]
 */

BOOL
inline
EnableNamedPriv (
    LPCTSTR lpName, ///< name for the privilege token (from winnt.h)
    HANDLE hProcess = GetCurrentProcess() ///< handle of the process to which to give the privilege
    )
{
    HANDLE hToken;
    LUID privValue;
    TOKEN_PRIVILEGES tkp;


    //
    // Retrieve a handle of the access token
    //
    if (!OpenProcessToken( hProcess,
            TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
            &hToken))
    {
        std::cerr << "OpenProcessToken failed with: " << ::GetLastError() << std::endl;
        return FALSE;
    }

    if (!LookupPrivilegeValue((LPSTR) NULL,
            lpName,
            &privValue))
    {
        std::cerr << "LookupPrivilegeValue failed with: " << GetLastError() << std::endl;
        return FALSE;
    }

    //
    // Enable the privilege
    //
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Luid = privValue;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    AdjustTokenPrivileges(hToken,
        FALSE,
        &tkp,
        sizeof(TOKEN_PRIVILEGES),
        (PTOKEN_PRIVILEGES) NULL,
        (PDWORD) NULL);

    //
    // The return value of AdjustTokenPrivileges couldn't be tested on older NT versions
    //
    DWORD lastError = GetLastError();
    if ( lastError != ERROR_SUCCESS)
    {
        if ( lastError == ERROR_NOT_ALL_ASSIGNED )
        {
            // Can't enable permission we haven't got
        }
        else
        {
            std::cerr << "AdjustTokenPrivileges failed with: " << lastError << std::endl;
        }
        return FALSE;
    }

    return TRUE;
}

}

#endif // ADJUST_PRIV_H_
