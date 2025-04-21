#ifndef STRFROMWCHAR_H_
#define STRFROMWCHAR_H_

/**@file

    Function to create a std::string from a wide string.

    @author Roger Orr mailto:rogero@howzatt.co.uk
    Bug reports, comments, and suggestions are always welcome.

    Copyright &copy; 2005 under the MIT license:

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

    $Revision: 2656 $
*/

// $Id: StrFromWchar.h 2656 2025-04-21 12:59:05Z roger $

#include <string>
#include <vector>

namespace or2 {

///////////////////////////////////////////////////////////////////////////////
/**
 * Convert wide string to MB string
 * @param wString the (NUL terminated) wide character array to convert
 * @return a MBCS string
 */
inline std::string strFromWchar(wchar_t const *const wString) {
  size_t const len = wcslen(wString) + 1;
  size_t const nBytes = len * sizeof(wchar_t);
  std::vector<char> chArray(nBytes);
  wcstombs(&chArray[0], wString, nBytes);
  return std::string(&chArray[0]);
}

///////////////////////////////////////////////////////////////////////////////
/**
 * Convert MB string to wide string
 * @param mbString the (NUL terminated) multi byte wide character array to
 * convert
 * @return a wide string
 */
inline std::wstring wstrFromMBchar(char const *const mbString) {
  size_t const len = strlen(mbString) + 1;
  std::vector<wchar_t> chArray(len);
  mbstowcs(&chArray[0], mbString, len);
  return std::wstring(&chArray[0]);
}

} // namespace or2

#endif // STRFROMWCHAR_H_