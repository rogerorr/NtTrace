#ifndef DEBUG_PRIV_H_
#define DEBUG_PRIV_H_

/**
@file
    Function to grant debug privileges to a process.

    Granting debug privileges enables being able to access other processes
    to read/write memory and debug them.

    @author Roger Orr <rogero@howzatt.co.uk>

    Copyright &copy; 2003.
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

    $Revision: 2074 $
*/

// $Id: DebugPriv.h 2074 2021-07-17 17:07:41Z roger $

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
