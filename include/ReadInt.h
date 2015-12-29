#ifndef readInt_h_
#define readInt_h_

/**
@file
    Helper function for reading a string into an integer.

    @author Roger Orr <rogero@howzatt.demon.co.uk>

    Copyright &copy; 2004

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

// $Id: ReadInt.h 1405 2013-11-12 23:57:17Z Roger $

#include <string> // for string
#include <cstdlib> // for strtol 

namespace or2
{

/** Convert a string into an integer.
 *
 * Allows string to start with 0x to imply hexadecimal values.
 *
 * @return
 * - true on successful conversion
 * - false on failure
 */
inline
bool readInt(
   std::string const & value, ///< value to convert
   int & result ) ///< returned result if successful conversion, otherwise partial result
{
   char *delim = 0;

   if ( ( value.length() >= 2 ) &&
        ( value[0] == '0' ) &&
        ( value[1] == 'x' || value[1] == 'X' ) )
   {
      result = strtoul( value.c_str() + 2, &delim, 16 );
   }
   else
   {
      result = strtol( value.c_str(), &delim, 10 );
   }

   return *delim == '\0';
}

/** Convert a string into an unsigned integer.
 *
 * Allows string to start with 0x to imply hexadecimal values.
 *
 * @return
 * - true on successful conversion
 * - false on failure
 */
inline
bool readInt(
   std::string const & value, ///< value to convert
   unsigned int & result ) ///< returned result if successful conversion, otherwise partial result
{
   char *delim = 0;

   if ( ( value.length() >= 2 ) &&
        ( value[0] == '0' ) &&
        ( value[1] == 'x' || value[1] == 'X' ) )
   {
      result = strtoul( value.c_str() + 2, &delim, 16 );
   }
   else
   {
      result = strtoul( value.c_str(), &delim, 10 );
   }

   return *delim == '\0';
}

#if _MSC_VER >= 1500
/** Convert a string into a long long integer.
 *
 * Allows string to start with 0x to imply hexadecimal values.
 *
 * @return
 * - true on successful conversion
 * - false on failure
 */
inline
bool readInt(
   std::string const & value, ///< value to convert
   LONGLONG & result ) ///< returned result if successful conversion, otherwise partial result
{
   char *delim = 0;

   if ( ( value.length() >= 2 ) &&
        ( value[0] == '0' ) &&
        ( value[1] == 'x' || value[1] == 'X' ) )
   {
      result = _strtoi64( value.c_str() + 2, &delim, 16 );
   }
   else
   {
      result = _strtoi64( value.c_str(), &delim, 10 );
   }

   return *delim == '\0';
}
#endif // _MSC_VER >= 1500

}

#endif // readInt_h_