#ifndef ENUMERATIONS_H_
#define ENUMERATIONS_H_

/**
@file

  Manage Nt enumerations for NtTrace.

  @author Roger Orr mailto:rogero@howzatt.co.uk
  Bug reports, comments, and suggestions are always welcome.

  Copyright &copy; 2011-2014 under the MIT license:

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

  $Revision: 2670 $
*/

// $Id: Enumerations.h 2670 2025-04-21 14:35:30Z roger $

namespace Enumerations {
/** Provide a single mapping from an enumeration value to a name */
struct EnumMap {
  unsigned long value_; ///< The enumeration value
  char const *name_;    ///< The corresponding name
};

/** Provide a mapping from an enumeration name to a zero-terminated array of
 * EnumMap entries */
struct AllEnum {
  const char *name_; ///< The enumeration name
  EnumMap *pMap_;    ///< The corresponding EnumMap
};

extern AllEnum allEnums[];
} // namespace Enumerations

#endif // ENUMERATIONS_H_
