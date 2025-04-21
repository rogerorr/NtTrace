#ifndef readInt_h_
#define readInt_h_

/**
@file
  Helper function for reading a string into an integer.

  @author Roger Orr mailto:rogero@howzatt.co.uk
  Bug reports, comments, and suggestions are always welcome.

  Copyright &copy; 2004 under the MIT license:

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

// $Id: ReadInt.h 2675 2025-04-21 16:52:13Z roger $

#include <cstdlib>   // for strtol
#include <string>    // for string
#include <windows.h> // for LONGLONG

namespace or2 {

/** Convert a string into an integer.
 *
 * Allows string to start with 0x to imply hexadecimal values.
 *
 * @return
 * - true on successful conversion
 * - false on failure
 */
inline bool readInt(std::string const &value, ///< value to convert
                    int &result) ///< returned result if successful conversion,
                                 ///< otherwise partial result
{
  char *delim = nullptr;

  if ((value.length() >= 2) && (value[0] == '0') &&
      (value[1] == 'x' || value[1] == 'X')) {
    result = strtoul(value.c_str() + 2, &delim, 16);
  } else {
    result = strtol(value.c_str(), &delim, 10);
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
inline bool
readInt(std::string const &value, ///< value to convert
        unsigned int &result)     ///< returned result if successful conversion,
                                  ///< otherwise partial result
{
  char *delim = nullptr;

  if ((value.length() >= 2) && (value[0] == '0') &&
      (value[1] == 'x' || value[1] == 'X')) {
    result = strtoul(value.c_str() + 2, &delim, 16);
  } else {
    result = strtoul(value.c_str(), &delim, 10);
  }

  return *delim == '\0';
}

/** Convert a string into a long long integer.
 *
 * Allows string to start with 0x to imply hexadecimal values.
 *
 * @return
 * - true on successful conversion
 * - false on failure
 */
inline bool readInt(std::string const &value, ///< value to convert
                    LONGLONG &result)         ///< returned result if successful
                                      ///< conversion, otherwise partial result
{
  char *delim = nullptr;

  if ((value.length() >= 2) && (value[0] == '0') &&
      (value[1] == 'x' || value[1] == 'X')) {
    result = _strtoi64(value.c_str() + 2, &delim, 16);
  } else {
    result = _strtoi64(value.c_str(), &delim, 10);
  }

  return *delim == '\0';
}

} // namespace or2

#endif // readInt_h_