#ifndef SimpleTokenizer_h_
#define SimpleTokenizer_h_

/**
@file
    Turn a string delimited by a given character into a vector of strings.

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

#include <string>
#include <vector>

namespace or2 {

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
inline void
SimpleTokenizer(std::string const &str,            ///< string to tokenize
                std::vector<std::string> *pTokens, ///< vector to populate
                char delim = ',')                  ///< delimiter for strings
{
  std::string::size_type pos = 0;
  std::string::size_type len = str.size();

  while (pos < len) {
    std::string::size_type nextpos = str.find(delim, pos);

    if (nextpos == std::string::npos)
      nextpos = len;
    pTokens->push_back(str.substr(pos, nextpos - pos));
    pos = nextpos + 1;
  }
}

} // namespace or2

#endif // SimpleTokenizer_h_
