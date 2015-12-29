#ifndef SimpleTokenizer_h_
#define SimpleTokenizer_h_

/**
@file
    Turn a string delimited by a given character into a vector of strings.

    @author Roger Orr <rogero@howzatt.demon.co.uk>

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
    Please report bugs to rogero@howzatt.demon.co.uk.

    $Revision: 1405 $
*/

#include <string>
#include <vector>

namespace or2
{

////////////////////////////////////////////////////////////////
/** Turn a string delimited by a given character into a vector of strings. 
 *
 * EXAMPLE<pre>
 *    \#include "SimpleTokenizer.h"
 *    ...
 *    std::vector\< std::string \> sv;
 *
 *    SimpleTokenizer( "This is a test", &sv, ' ' );
 *
 *    std::copy( sv.begin(), sv.end(),
 *        std::ostream_iterator( std::cout, "\n" ) );
 *    </pre>
 *
 */
inline
void SimpleTokenizer(
    std::string const & str, ///< string to tokenize
    std::vector< std::string > * pTokens, ///< vector to populate
    char delim = ',' ) ///< delimiter for strings
{
    std::string::size_type pos = 0;
    std::string::size_type len = str.size();

    while ( pos < len )
    {
        std::string::size_type nextpos = str.find( delim, pos );

        if ( nextpos == std::string::npos )
           nextpos = len;
        pTokens->push_back( str.substr(pos, nextpos - pos ) );
        pos = nextpos+1;
    }
}

}

#endif // SimpleTokenizer_h_
