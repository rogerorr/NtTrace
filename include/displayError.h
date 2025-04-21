#ifndef DISPLAY_ERROR_H_
#define DISPLAY_ERROR_H_

/**
@file
    Function for displaying Win32 error to a stream.

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

// $Id: displayError.h 2656 2025-04-21 12:59:05Z roger $

#include <iosfwd>
#include <windows.h>

namespace or2 {

#pragma warning(push)
#pragma warning(disable : 4512) // assignment operator could not be generated

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
std::ostream &operator<<(std::ostream &os, displayError const &se);

} // namespace or2

#include "displayError.inl"

#endif // DISPLAY_ERROR_H_