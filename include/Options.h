#ifndef OPTIONS_H_
#define OPTIONS_H_

/**
@file
    Provide command line option handling.

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

    $Revision: 1428 $
*/

// $Id: Options.h 1428 2014-10-04 21:55:56Z Roger $

#include <string>
#include <vector>

/** Namespace for general utility classes and functions */
namespace or2
{

/**
 * Class to handle command line options.
 *
 * This class is designed to handle simple command lines where a set of options (sometimes called flags)
 * precede a list of non-flag arguments.
 *
 * Options are identified by starting with either '-' or '/', and the option name is one or more characters.
 *<BR>
 * Options can be declared as 'bool', 'int', 'unsigned int', 'long', 'unsigned long', 'double' or 'string'.  For all
 * types other than 'bool' the value is supplied as the next command line argument.
 *
 * The number of (non-flag) arguments can be specified either as a single number
 * or a range of values, which can be open ended.  Once option processing is complete
 * the remaining arguments are available using STL iterators obtained from the begin() and end()
 * methods.
 *
 * Three in-built options are supported:<UL>
 *<LI>-h (or -?) displays syntax help for the command line.
 *<LI>-ver prints the version string (provided in the constructor - typically an RCS 'Id' string
 *<LI>-- terminates flag processing and allows non-flag arguments which start with '-' or '/'
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
class Options
{
public:
    /**
     * Construct from rcs ID - for version info.
     *
     * The supplied string is automatically printed when the -ver option is used.
     */
    Options( char const * pRcsId );

    /** Destructor  */
    ~Options();

    /** Set a 'bool' option */
    void set( std::string const & option, bool * pValue, std::string const & helpString = "" );

    /** Set an 'int' option */
    void set( std::string const & option, int * pValue, std::string const & helpString = "" );

    /** Set an 'unsigned int' option */
    void set( std::string const & option, unsigned int * pValue, std::string const & helpString = "" );

    /** Set a 'long' option */
    void set( std::string const & option, long * pValue, std::string const & helpString = "" );

    /** Set an 'unsigned long' option */
    void set( std::string const & option, unsigned long * pValue, std::string const & helpString = "" );

    /** Set an 'double' option */
    void set( std::string const & option, double * pValue, std::string const & helpString = "" );

    /** Set a 'string' option */
    void set( std::string const & option, std::string * pValue, std::string const & helpString = "");

    /** Set argument count (-1 => any number) */
    void setArgs( int argCount, std::string const & helpString = "" );

    /** Set argument count - min and max (-1 => any number) */
    void setArgs( int argCountMin, int argCountMax, std::string const & helpString = "" );

    /** Process the command line arguments, return false on bad argument or 'help' */
    bool process( int argc, char ** argv, std::string const & usage = "" );

    /** Get program name */
    std::string pname() const;

    /** Iterator type returned by begin/end */
    typedef std::vector<std::string>::const_iterator const_iterator; 

    /** Start of non-option arguments */
    const_iterator begin() const;

    /** End of non-option arguments */
    const_iterator end() const;

private:
    // no copy/assign
    Options( Options const & );
    Options & operator=( Options const & );

    struct Data;
    Data * pData;
};

}

// include the implementation
#include "Options.inl"

#endif // OPTIONS_H_