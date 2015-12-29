#ifndef STRFROMWCHAR_H_
#define STRFROMWCHAR_H_

/**@file

    Function to create a std::string from a wide string.

    @author Roger Orr <rogero@howzatt.demon.co.uk>

    Copyright &copy; 2005.
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

    $Revision: 1569 $
*/

// $Id: StrFromWchar.h 1569 2015-09-10 22:25:51Z Roger $

#include <string>
#include <vector>

namespace or2
{

///////////////////////////////////////////////////////////////////////////////
/**
 * Convert wide string to MB string
 * @param wString the (NUL terminated) wide character array to convert
 * @return a MBCS string 
 */
inline
std::string strFromWchar( wchar_t const * const wString )
{
   size_t const len = wcslen( wString ) + 1;
   size_t const nBytes = len * sizeof( wchar_t );
   std::vector<char> chArray( nBytes );
   wcstombs( &chArray[0], wString, nBytes );
   return std::string( &chArray[0] );
}

///////////////////////////////////////////////////////////////////////////////
/**
 * Convert MB string to wide string
 * @param mbString the (NUL terminated) multi byte wide character array to convert
 * @return a wide string 
 */
inline
std::wstring wstrFromMBchar( char const * const mbString )
{
   size_t const len = strlen( mbString ) + 1;
   std::vector<wchar_t> chArray( len );
   mbstowcs( &chArray[0], mbString, len );
   return std::wstring( &chArray[0] );
}

}

#endif // STRFROMWCHAR_H_