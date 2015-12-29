#ifndef DISPLAY_ERROR_H_
#define DISPLAY_ERROR_H_

/**
@file
    Function for displaying Win32 error to a stream.

    EXAMPLE<PRE>
    displayError( std::cerr, GetLastError() );
    </PRE>

    @author Roger Orr <rogero@howzatt.demon.co.uk>

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
    Please report bugs to rogero@howzatt.demon.co.uk.

    $Revision: 1405 $
*/

// $Id: displayError.h 1405 2013-11-12 23:57:17Z Roger $

#include <iosfwd>
#include <windows.h>

namespace or2
{

#pragma warning(push)
#pragma warning(disable: 4512) // assignment operator could not be generated

/** manipulator */
class displayError {
public:
  /** Default ctor wraps GetLastError() */
  displayError();
 
  /**
   * Construct from an hresult
   * @param hresult the error code to display
   */
  displayError(HRESULT hresult);

  /**
   * Print the error
   * @param os the output stream to write to
   */
  void printOn(std::ostream &os) const;
private:
  HRESULT const hresult;
};

#pragma warning(pop)

/** Stream a displayError manipulator */
std::ostream & operator<<(std::ostream & os, displayError const &se);

}

#include "displayError.inl"

#endif // DISPLAY_ERROR_H_