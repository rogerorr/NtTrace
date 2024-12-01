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

    @author Roger Orr <rogero@howzatt.co.uk>

    Copyright &copy; 2004.
    This software is distributed in the hope that it will be useful, but
    without WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    Permission is granted to anyone to make or distribute verbatim
    copies of this software provided that the copyright notice and
    this permission notice are preserved, and that the distributor
    grants the recipient permission for further distribution as permitted
    by this notice.

    Comments and suggestions are always welcome.
    Please report bugs to rogero@howzatt.co.uk.

    $Revision: 2480 $
*/

// $Id: displayError.inl 2480 2024-09-28 19:32:14Z roger $

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