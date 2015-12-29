#ifndef ENUMERATIONS_H_
#define ENUMERATIONS_H_

/**
@file
    Manage Nt enumerations for NtTrace.

    @author Roger Orr <rogero@howzatt.demon.co.uk>

    Copyright &copy; 2011.
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

    $Revision: 1460 $
*/

// $Id: Enumerations.h 1460 2014-10-09 22:31:44Z Roger $

namespace Enumerations
{
  /** Provide a single mapping from an enumeration value to a name */
  struct EnumMap
  {
    unsigned long value; ///< The enumeration value
    char const * name; ///< The corresponding name
  };

  /** Provide a mapping from an enumeration name to a zero-terminated array of EnumMap entries */
  struct AllEnum
  {
    const char *name; ///< The enumeration name
    EnumMap *pMap; ///< The corresponding EnumMap
  };

  extern AllEnum allEnums[];
}

#endif // ENUMERATIONS_H_
