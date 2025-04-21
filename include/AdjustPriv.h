#ifndef ADJUST_PRIV_H_
#define ADJUST_PRIV_H_

/**@file

    Function to grant a single privilege to a process.

    @author Roger Orr mailto:rogero@howzatt.co.uk
    Bug reports, comments, and suggestions are always welcome.

    Copyright &copy; 2003-2025 under the MIT license:

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

    $Revision: 2655 $
*/

// $Id: AdjustPriv.h 2655 2025-04-21 12:54:42Z roger $

namespace or2 {

/**
 * Attempt to grant the named privilege to the specified process.
 *
 * @return
 * - TRUE - success
 * - FALSE - failure [typically the process hasn't been granted the privilege]
 */

BOOL inline EnableNamedPriv(
    LPCTSTR lpName, ///< name for the privilege token (from winnt.h)
    HANDLE hProcess = GetCurrentProcess() ///< handle of the process to which to
                                          ///< give the privilege
) {
  HANDLE hToken;
  LUID privValue;
  TOKEN_PRIVILEGES tkp;

  //
  // Retrieve a handle of the access token
  //
  if (!OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
                        &hToken)) {
    std::cerr << "OpenProcessToken failed with: " << ::GetLastError()
              << std::endl;
    return FALSE;
  }

  if (!LookupPrivilegeValue((LPSTR) nullptr, lpName, &privValue)) {
    std::cerr << "LookupPrivilegeValue failed with: " << GetLastError()
              << " for " << lpName << std::endl;
    return FALSE;
  }

  //
  // Enable the privilege
  //
  tkp.PrivilegeCount = 1;
  tkp.Privileges[0].Luid = privValue;
  tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

  AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(TOKEN_PRIVILEGES),
                        (PTOKEN_PRIVILEGES) nullptr, (PDWORD) nullptr);

  //
  // The return value of AdjustTokenPrivileges couldn't be tested on older NT
  // versions
  //
  DWORD lastError = GetLastError();
  if (lastError != ERROR_SUCCESS) {
    if (lastError == ERROR_NOT_ALL_ASSIGNED) {
      // Can't enable permission we haven't got
    } else {
      std::cerr << "AdjustTokenPrivileges failed with: " << lastError << " for "
                << lpName << std::endl;
    }
    return FALSE;
  }

  return TRUE;
}

} // namespace or2

#endif // ADJUST_PRIV_H_
