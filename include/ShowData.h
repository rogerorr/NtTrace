#ifndef SHOWDATA_H_
#define SHOWDATA_H_

/**@file

    Functions to handle showing data from another process

        @author Roger Orr <rogero@howzatt.demon.co.uk>

    Copyright &copy; 2002, 2003.
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

#include <windows.h>
#include <ostream>

// or2 includes
#include <NtDllStruct.h> // For Nt native data types

/** namespace for functions showing data from another process */
namespace showData
{
    /** show a DWORD from the debuggee */
    void showDword( std::ostream & os, ULONG_PTR value );

    /** show a BOOL from the debuggee */
    void showBoolean( std::ostream & os, BOOLEAN value );

    /** Show an enumeration name, if available */
    void showEnum(std::ostream & os, ULONG_PTR value, std::string const & enumName);

    /** Show an mask enumeration name, if available */
    void showMask(std::ostream & os, ULONG_PTR value, std::string const & enumName);

    /** show an HRESULT from the debuggee */
    void showWinError( std::ostream & os, HRESULT hResult );

    /** show an image name from the debuggee (in ANSI or Unicode) */
    bool showName( std::ostream & os, HANDLE hProcess, LPVOID lpImageName, bool bUnicode );

    /**
     * Show a string from the debuggee (in ANSI or Unicode)
     * @return true if ends with a newline, false if not
     */
    bool showString( std::ostream & os, HANDLE hProcess, LPVOID lpString, bool bUnicode, WORD nStringLength );

    /** show Object Attrributes from the debuggee */
    void showObjectAttributes( std::ostream & os, HANDLE hProcess, LPVOID pvoid );

    /** show an Unicode string from the debuggee */
    void showUnicodeString( std::ostream & os, HANDLE hProcess, PUNICODE_STRING pTargetUnicodeString );

    /** show a generic pointer from the debuggee */
    void showPointer( std::ostream & os, HANDLE hProcess, ULONG_PTR argVal );

    /** show a pointer to handle from the debuggee */
    void showPHandle( std::ostream & os, HANDLE hProcess, ULONG_PTR argVal );

    /** show a pointer to USHORT from the debuggee */
    void showPUshort( std::ostream & os, HANDLE hProcess, ULONG_PTR argVal );

    /** show a pointer to ULONG from the debuggee */
    void showPUlong( std::ostream & os, HANDLE hProcess, ULONG_PTR argVal );

    /** show an access mask from the debuggee */
    void showAccessMask( std::ostream & os, HANDLE hProcess, ULONG_PTR argVal );

    /** show a client ID from the debuggee */
    void showPClientId( std::ostream & os, HANDLE hProcess, PCLIENT_ID pClientId );

    /** show an OP status block from the debuggee */
    void showPIoStatus( std::ostream & os, HANDLE hProcess, PIO_STATUS_BLOCK pIoStatusBlock );

    /** show a large integer from the debuggee */
    void showPLargeInteger( std::ostream & os, HANDLE hProcess, PLARGE_INTEGER pLargeInteger );

    /** Display an LPC message */
    void showPLpcMessage(std::ostream & os, HANDLE hProcess, PLPC_MESSAGE pLpcMessage);

    /** show file attributes from the debuggee */
    void showFileAttributes( std::ostream & os, ULONG argVal );

    /** show file basic information from the debuggee */
    void showPFileBasicInfo( std::ostream & os, HANDLE hProcess, PFILE_BASIC_INFORMATION pFileBasicInfo );

    /** show network open information from the debuggee */
    void showPFileNetworkInfo( std::ostream & os, HANDLE hProcess, PFILE_NETWORK_OPEN_INFORMATION pFileNetworkInfo );

    /** Convert msvc throw information into a type name */
    void showThrowType( std::ostream & os, HANDLE hProcess, ULONG_PTR throwInfo, ULONG_PTR base );
}

#endif // SHOWDATA_H_