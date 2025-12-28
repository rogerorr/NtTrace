#ifndef UTF16TOMBS_H_
#define UTF16TOMBS_H_

/**@file

  Function to create a std::string from a UTF16 string.

  @author Roger Orr mailto:rogero@howzatt.co.uk
  Bug reports, comments, and suggestions are always welcome.

  Copyright &copy; 2025 under the MIT license:

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

  $Revision$
*/

// $Id$

#include <string>
#include <vector>

namespace or2 {

///////////////////////////////////////////////////////////////////////////////
/**
 * Convert UTF16 wchar_t array to MB char array
 * @param utf16_str the (possibly NUL terminated) wide character array to
 * convert
 * @return the number of characters written (or to be written if mb_size is o)
 */
inline size_t Utf16ToMbs(char *mb_str, size_t mb_size, const wchar_t *utf16_str,
                         size_t utf16_len) {
  return WideCharToMultiByte(CP_UTF8, 0, utf16_str, static_cast<int>(utf16_len),
                             mb_str, static_cast<int>(mb_size), nullptr,
                             nullptr);
}

} // namespace or2

#endif // UTF16TOMBS_H_