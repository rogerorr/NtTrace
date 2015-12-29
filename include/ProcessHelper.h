#ifndef ProcessHelper_H_
#define ProcessHelper_H_

/**
@file
    Helper functions for process management.

    @author Roger Orr <rogero@howzatt.demon.co.uk>

    Copyright &copy; 2003

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

// $Id: ProcessHelper.h 1405 2013-11-12 23:57:17Z Roger $


#pragma warning( disable: 4786 )

#include <windows.h> // for CreateProcess
#include <psapi.h> // for EnumProcesses
#include <cctype> // for tolower
#include <algorithm> // for transform
#include <string>
#include <vector>
#include <iostream>

#include "Options.h"
#include "readInt.h"

namespace or2
{

//////////////////////////////////////////////////////////////////////////
/** Wrapper for create process.
 *
 * @return
 * - 0 on success
 * - non-zero on failure, and GetLastError() can be used
 */

inline
int CreateProcessHelper(
    Options::const_iterator it, ///< first string is command
    Options::const_iterator end, ///< rest of iteration are arguments
    DWORD create_options = 0, ///< options for CreateProcess
    PROCESS_INFORMATION* pProcessInformation = 0 ) ///< returned process information on success
{
    std::string executable = *it;

    // Search for possible executable matching the program name
    char szFullName[ MAX_PATH ];
    if ( 0 != SearchPath( NULL, executable.c_str(), ".exe", sizeof( szFullName ), szFullName, 0 ) )
        executable = szFullName;
    
    std::string cmdLine;
    for ( ; it != end; ++it )
    {
        std::string curr( *it );

        if ( cmdLine.length() )
            cmdLine += " ";

        if ( curr.find( ' ' ) != std::string::npos )
        {
            cmdLine += '"';
            cmdLine += curr;
            cmdLine += '"';
        }
        else
        {
            cmdLine += curr;
        }
    }

    STARTUPINFO startupInfo = { sizeof( startupInfo ) };
    startupInfo.dwFlags = STARTF_USESHOWWINDOW;
    startupInfo.wShowWindow = SW_SHOWNORMAL; // Assist GUI programs
    PROCESS_INFORMATION ProcessInformation;

    if ( ! CreateProcess(
        const_cast<char*>(executable.c_str()),                 // name of executable module
        const_cast<char*>(cmdLine.c_str()),                    // command line string
        0, // SD
        0,  // SD
        true,                      // handle inheritance option
        create_options,    // creation flags
        0,                      // new environment block
        0,                // current directory name
        &startupInfo,               // startup information
        &ProcessInformation // process information
        ) )
    {
        return 1;
    }

    if ( pProcessInformation )
        *pProcessInformation = ProcessInformation;
    else
    {
        CloseHandle( ProcessInformation.hProcess );
        CloseHandle( ProcessInformation.hThread );
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////////
/** Wrapper for create process as user.
 *
 * @return
 * - 0 on success
 * - non-zero on failure, and GetLastError() can be used
 */

inline
int CreateProcessAsUserHelper(
    HANDLE hToken, ///< security token to use for target process
    Options::const_iterator it, ///< first string is command
    Options::const_iterator end, ///< rest of iteration are arguments
    DWORD create_options = 0, ///< options for CreateProcess
    PROCESS_INFORMATION* pProcessInformation = 0 ) ///< returned process information on success
{
    std::string executable = *it;

    // Search for possible executable matching the program name
    char szFullName[ MAX_PATH ];
    if ( 0 != SearchPath( NULL, executable.c_str(), ".exe", sizeof( szFullName ), szFullName, 0 ) )
        executable = szFullName;
    
    std::string cmdLine;
    for ( ; it != end; ++it )
    {
        std::string curr( *it );

        if ( cmdLine.length() )
            cmdLine += " ";

        if ( curr.find( ' ' ) != std::string::npos )
        {
            cmdLine += '"';
            cmdLine += curr;
            cmdLine += '"';
        }
        else
        {
            cmdLine += curr;
        }
    }

    STARTUPINFO startupInfo = { sizeof( startupInfo ) };
    startupInfo.dwFlags = STARTF_USESHOWWINDOW;
    startupInfo.wShowWindow = SW_SHOWNORMAL; // Assist GUI programs
    PROCESS_INFORMATION ProcessInformation;

    if ( ! CreateProcessAsUser(
        hToken,
        const_cast<char*>(executable.c_str()),                 // name of executable module
        const_cast<char*>(cmdLine.c_str()),                    // command line string
        0, // SD
        0,  // SD
        true,                      // handle inheritance option
        create_options,    // creation flags
        0,                      // new environment block
        0,                // current directory name
        &startupInfo,               // startup information
        &ProcessInformation // process information
        ) )
    {
        return 1;
    }

    if ( pProcessInformation )
        *pProcessInformation = ProcessInformation;
    else
    {
        CloseHandle( ProcessInformation.hProcess );
        CloseHandle( ProcessInformation.hThread );
    }

    return 0;
}

//////////////////////////////////////////////////////////
/** Returns all process IDs matching 'pattern'.
 *
 * @return a vector of all matching process IDs
 */
inline
std::vector< DWORD > FindProcesses( const char *pattern = 0 ///< pattern to match for process name
 )
{
    std::vector< DWORD > ret;

    int pid(0);
    
    if ( ( pattern != 0 ) && ( readInt(pattern, pid ) ) )
    {
        ret.push_back( (DWORD)pid );
        return ret;
    }

    DWORD aProcesses[1024];
    DWORD cbNeeded;

    if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
        return ret;

    // Calculate how many process identifiers were returned.
    DWORD const cProcesses = cbNeeded / sizeof(DWORD);

    std::string lowerPattern( pattern == 0 ? "" : pattern );
    std::transform( lowerPattern.begin(), lowerPattern.end(), lowerPattern.begin(), tolower );

    for ( unsigned int iProcess = 0; iProcess < cProcesses; iProcess++ )
    {
        DWORD const processID( aProcesses[iProcess] );

        // Open the process to query the main module name

        if ( HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                                       PROCESS_VM_READ,
                                       FALSE, processID ) )
        {
            char szModName[MAX_PATH];

            // Get the full path to the module's file.

            if ( GetModuleBaseName( hProcess, 0, szModName, sizeof( szModName ) )
#ifdef GetProcessImageFileName
                 || GetProcessImageFileName(hProcess, szModName, sizeof( szModName ) )
#endif // GetProcessImageFileName
               )
            {
                std::string lower( szModName );
                std::transform( lower.begin(), lower.end(), lower.begin(), tolower );
                if ( ( lowerPattern.length() == 0 ) || ( lower.find( lowerPattern ) != std::string::npos ) )
                {
                    // Return the process identifier.
                    ret.push_back( processID );
                }
            }

            CloseHandle( hProcess );
        }
    }

    return ret;
}

}

#endif // ProcessHelper_H_