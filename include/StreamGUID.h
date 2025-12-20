#ifndef STREAMGUID_H_
#define STREAMGUID_H_

/**@file

  Function to provide access to process parameters.

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

  $Revision: 2662 $
*/

// $Id: StreamGUID.h 2662 2025-04-21 13:52:03Z roger $

#include <iostream>
#include <sstream>

#include <windows.h>

/** Display a GUID in the canonical form */
std::ostream &operator<<(std::ostream &os, GUID const &rhs) {
  std::ostringstream oss;
  oss.fill('0');
  oss << std::hex << std::setw(8) << rhs.Data1 << "-" << std::setw(4)
      << rhs.Data2 << "-" << std::setw(4) << rhs.Data3 << "-";
  for (int idx = 0; idx != sizeof(rhs.Data4); ++idx) {
    if (idx == 2)
      oss << "-";
    unsigned int ch = rhs.Data4[idx];
    oss << std::setw(2) << ch;
  }
  os << oss.str();
  return os;
}

#endif // STREAMGUID_H_
