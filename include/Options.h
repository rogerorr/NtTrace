#ifndef OPTIONS_H_
#define OPTIONS_H_

/**
@file
  Provide command line option handling.

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

  $Revision: 2675 $
*/

// $Id: Options.h 2675 2025-04-21 16:52:13Z roger $

#include <string>
#include <vector>

/** Namespace for general utility classes and functions */
namespace or2 {

/**
 * Class to handle command line options.
 *
 * This class is designed to handle simple command lines where a set of options
 *(sometimes called flags) precede a list of non-flag arguments.
 *
 * Options are identified by starting with either '-' or '/', and the option
 *name is one or more characters. <BR> Options can be declared as 'bool', 'int',
 *'unsigned int', 'long', 'unsigned long', 'double' or 'string'.  For all types
 *other than 'bool' the value is supplied as the next command line argument.
 *
 * The number of (non-flag) arguments can be specified either as a single number
 * or a range of values, which can be open ended.  Once option processing is
 *complete the remaining arguments are available using STL iterators obtained
 *from the begin() and end() methods.
 *
 * Three in-built options are supported:<UL>
 *<LI>-h (or -?) displays syntax help for the command line.
 *<LI>-ver prints the version string (provided in the constructor - typically an
 *RCS 'Id' string <LI>-- terminates flag processing and allows non-flag
 *arguments which start with '-' or '/'
 *</UL>
 *
 * Example:
 *<PRE><CODE>
 *   Options opt( rcsId );
 *   opt.set( "b", &boolValue, "batch mode" );
 *   opt.set( "i", &intValue, "number of repeats" );
 *   if  (! opt.process( argc, argv, "This program does something" ) )
 *       return 1;
 *   for ( Options::const_iterator it = opts.begin();
 *         it != opts.end(); ++it )
 *   { // do something with *it }
 *</CODE></PRE>
 * This example code will successfully cope with command lines like this:<UL>
 *<LI>program.exe <BR>
 *  [no flags set, no iterations round the loop]
 *<LI>program.exe -b -i 42 fred joe bill <BR>
 *  [boolValue set to true, intValue set to 42, 3 iterations round the loop]
 *<LI>program.exe -i 12 -- -i -j <BR>
 *  [intValue set to true, 2 iterations round the loop]
 *<LI>program.exe -h <BR>
 *  This prints:
 *<PRE>Syntax: program.exe [-b] [-i #]
 *
 *  Options:
 *    -b batch mode
 *    -i number of repeats
 *
 *  This program does something</PRE>
 *</UL>
 */
class Options {
public:
  /**
   * Construct from rcs ID - for version info.
   *
   * The supplied string is automatically printed when the -ver option is used.
   */
  Options(char const *pRcsId);

  /** Destructor  */
  ~Options();

  /** Set a 'bool' option */
  void set(std::string const &option, bool *pValue,
           std::string const &helpString = "");

  /** Set an 'int' option */
  void set(std::string const &option, int *pValue,
           std::string const &helpString = "");

  /** Set an 'unsigned int' option */
  void set(std::string const &option, unsigned int *pValue,
           std::string const &helpString = "");

  /** Set a 'long' option */
  void set(std::string const &option, long *pValue,
           std::string const &helpString = "");

  /** Set an 'unsigned long' option */
  void set(std::string const &option, unsigned long *pValue,
           std::string const &helpString = "");

  /** Set an 'double' option */
  void set(std::string const &option, double *pValue,
           std::string const &helpString = "");

  /** Set a 'string' option */
  void set(std::string const &option, std::string *pValue,
           std::string const &helpString = "");

  /** Set argument count (-1 => any number) */
  void setArgs(int argCount, std::string const &helpString = "");

  /** Set argument count - min and max (-1 => any number) */
  void setArgs(int argCountMin, int argCountMax,
               std::string const &helpString = "");

  /** Process the command line arguments, return false on bad argument or 'help'
   */
  bool process(int argc, char **argv, std::string const &usage = "");

  /** Get program name */
  std::string pname() const;

  /** Iterator type returned by begin/end */
  using const_iterator = std::vector<std::string>::const_iterator;

  /** Start of non-option arguments */
  const_iterator begin() const;

  /** End of non-option arguments */
  const_iterator end() const;

private:
  // no copy/assign
  Options(Options const &);
  Options &operator=(Options const &);

  struct Data;
  Data *pData;
};

} // namespace or2

// include the implementation
#include "Options.inl"

#endif // OPTIONS_H_