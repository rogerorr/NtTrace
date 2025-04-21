#ifndef DISPLAY_ERROR_INL_
#define DISPLAY_ERROR_INL_

#include <ostream>
#include <windows.h>

namespace or2 {

/**
    Implementation of function for displaying Win32 error to a stream.

    EXAMPLE<PRE>
    displayError( std::cerr, GetLastError() );
    </PRE>

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

    $Revision: 2656 $
*/

// $Id: displayError.inl 2656 2025-04-21 12:59:05Z roger $

inline displayError::displayError() : hresult(GetLastError()) {}

inline displayError::displayError(HRESULT hresult) : hresult(hresult) {}

inline void displayError::printOn(std::ostream &os) const {
  if (hresult == 0) {
    os << "no error";
    return;
  }

  LPTSTR pszMsg = nullptr;

  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                nullptr, hresult, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR)&pszMsg, 0, nullptr);
  if (pszMsg != nullptr) {
    size_t nLen = strlen(pszMsg);
    if (nLen > 1 && pszMsg[nLen - 1] == '\n') {
      pszMsg[--nLen] = 0;
      if (nLen > 1 && pszMsg[nLen - 1] == '\r') {
        pszMsg[--nLen] = 0;
      }
      if (nLen > 1 && pszMsg[nLen - 1] == '.') {
        pszMsg[--nLen] = 0;
      }
    }

    if (hresult < 0)
      os << std::hex;
    os << "error " << hresult << std::dec << " (" << pszMsg << ")";
    ::LocalFree(pszMsg);
  } else {
    if (hresult < 0)
      os << std::hex;
    os << "Win32 error: " << hresult << std::dec;
  }
}

/* Stream a displayError manipulator */
inline std::ostream &operator<<(std::ostream &os, displayError const &se) {
  se.printOn(os);
  return os;
}

} // namespace or2

#endif // DISPLAY_ERROR_INL_