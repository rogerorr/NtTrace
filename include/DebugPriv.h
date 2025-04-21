#ifndef DEBUG_PRIV_H_
#define DEBUG_PRIV_H_

/**
@file
    Function to grant debug privileges to a process.

    Granting debug privileges enables being able to access other processes
    to read/write memory and debug them.

    @author Roger Orr mailto:rogero@howzatt.co.uk
    Bug reports, comments, and suggestions are always welcome.

    Copyright &copy; 2003 under the MIT license:

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

// $Id: DebugPriv.h 2656 2025-04-21 12:59:05Z roger $

#include "AdjustPriv.h"

namespace or2 {

/**
 * Changes the process's privilege so that kill works properly.
 *
 * @return
 * - TRUE - success
 * - FALSE - failure
 */
BOOL inline EnableDebugPriv(
    HANDLE hProcess = GetCurrentProcess() ///< handle of the process to which to
                                          ///< give debug privilege
) {
  return EnableNamedPriv(SE_DEBUG_NAME, hProcess);
}

} // namespace or2

#endif // DEBUG_PRIV_H_
