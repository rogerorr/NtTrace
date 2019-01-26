#ifndef ENTRYPOINT_H_
#define ENTRYPOINT_H_

/**
@file
    Manage a single entry point for NtTrace.

    @author Roger Orr <rogero@howzatt.demon.co.uk>

    Copyright &copy; 2002,2015.
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

    $Revision: 1775 $
*/

// $Id: EntryPoint.h 1775 2019-01-22 22:06:08Z Roger $

#include <windows.h>

#include <iosfwd>
#include <map>
#include <set>
#include <string>
#include <vector>

//////////////////////////////////////////////////////////////////////////
// Forward Reference
struct NtCall;

//////////////////////////////////////////////////////////////////////////
// Possible distinct argument types
typedef enum
{
    argULONG_PTR = 0, // also the default
    argULONG,
    argULONGLONG, // two adjacent dwords in 32bit, one qword in 64bit
    argENUM,
    argMASK,
    argBOOLEAN,
    argPOINTER,
    argPOBJECT_ATTRIBUTES,
    argPUNICODE_STRING,
    argPHANDLE,
    argPUSHORT,
    argPULONG,
    argACCESS_MASK,
    argPCLIENT_ID,
    argPIO_STATUS_BLOCK,
    argPLARGE_INTEGER,
    argPLPC_MESSAGE,
    argPFILE_BASIC_INFORMATION,
    argPFILE_NETWORK_OPEN_INFORMATION
} ArgType;

typedef enum
{
    argNONE = 0,
    argIN = 1,
    argOUT = 2,
    argOPTIONAL = 4,
    argCONST = 8,
    argRESERVED = 16,
    argUNDERSCORE = 32, // use _In_, _Out_ rather than IN, OUT
    argDOUBLE_UNDERSCORE = 64, // use __in, __out rather than IN, OUT
} ArgAttributes;

struct Argument
{
    Argument() : argType( argULONG_PTR ), argTypeName( "ULONG" ), name( "Unknown" ), attributes( argNONE ), dummy(false) {}
    Argument( ArgType argType, std::string const &argTypeName, std::string const &name, ArgAttributes attributes )
        : argType( argType ), argTypeName( argTypeName ), name( name ), attributes( attributes ), dummy(false)
    {}

#ifdef _M_IX86
    typedef DWORD ARG;
#elif _M_X64
    typedef DWORD64 ARG;
#endif

    /** Show the argument for the given process with the specified value. */
    void showArgument(std::ostream & os, HANDLE hProcess, ARG value, bool returnOk, bool dup) const;

    /** true if argument is output-only */
    bool outputOnly() const;

    ArgType argType; // Argument type for processing
    std::string argTypeName; // Actual argument type
    std::string name; // formal name of argument
    ArgAttributes attributes; // Optional attributes
    bool dummy; // True if this is a dummy argument (2nd part of 64bit item on 32bit Windows)
};

typedef enum
{
    retNTSTATUS = 0, // also the default
    retVOID,
    retPVOID,
    retULONG,
    retULONG_PTR,
} ReturnType;

class EntryPoint
{
public:
    typedef std::map<std::string, std::string> Typedefs;

    explicit EntryPoint( std::string const & name, std::string const & category ) : name( name ), category( category ), disabled(category[0] == '-'), targetAddress( 0 ), preSave(0), retType(retNTSTATUS)
    {
        if (disabled)
        {
            this->category.erase(0, 1);
        }
    }

    std::string const & getName() const { return name; }

    std::string const & getExported() const { return exported; }

    void setExported(std::string const &value) { exported = value; }

    std::string const & getCategory() const { return category; }

    bool isDisabled() const { return disabled; }

    size_t getArgumentCount() const { return arguments.size(); }

    void setArgumentCount( size_t newSize ) { arguments.resize( newSize ); }

    Argument const & getArgument( size_t idx ) const { return arguments[ idx ]; }

    void setArgument( int argNum, std::string const & argType,
                      std::string const & variableName, ArgAttributes attributes, Typedefs const & typedefs );

    void setDummyArgument(int argNum, ArgAttributes attributes);

    void setReturnType(std::string const &type, Typedefs const & typedefs);

    ReturnType getReturnType() const { return retType; }

    static bool readEntryPoints( std::istream & cfgFile, std::set<EntryPoint> & entryPoints, Typedefs & typedefs, std::string & target );

    void writeExport( std::ostream & os ) const;

    /** Set a trap for this entry point in the target process */
    NtCall setNtTrap(HANDLE hProcess, HMODULE hTargetDll, bool bPreTrace, DWORD dllOffset, bool verbose);

    /** Clear the trap for this entry in the target process */
    bool clearNtTrap(HANDLE hProcess, NtCall const & ntcall) const;

    void setAddress( unsigned char *brkptAddress ) { targetAddress = brkptAddress; }

    unsigned char * getAddress() const { return targetAddress; }

    void setPreSave( unsigned char *preSaveAddress ) { preSave = preSaveAddress; }

    unsigned char * getPreSave() const { return preSave; }

    void doPreSave(HANDLE hProcess, HANDLE hThread, CONTEXT const & Context);

    void trace( std::ostream & os, HANDLE hProcess, HANDLE hThread, CONTEXT const & Context, bool bNames, bool bStackTrace, bool before ) const;

    bool operator<( EntryPoint const & rhs ) const;

    static void stackTrace(std::ostream & os, HANDLE hProcess, HANDLE hThread);
private:
    std::string name; // name of entry point
    std::string exported; // (optional) exported name for entry point
    std::string category; // category of entry point
    bool disabled; // this entry point is disabled
    std::vector< Argument > arguments; // vector of arguments
    ReturnType retType; // Return type
    std::string retTypeName; // full name of return type
    unsigned char *targetAddress;
    unsigned char *preSave; // address of pre-save (for X64 fast-call)
    DWORD ssn; // System Service Number [Used to set Eax/Rax to pre-call breakpoint]

    NtCall insertBrkpt( HANDLE hProcess, unsigned char *address, unsigned int offset, unsigned char *setssn );
};

typedef std::set<EntryPoint> EntryPointSet;

//////////////////////////////////////////////////////////////////////////
// Our data structure for an NT call
struct NtCall
{
    NtCall() : entryPoint(0), nArgs(0) {}

    EntryPoint* entryPoint; // Pointer into EntryPointMap

    size_t nArgs; // Number of arguments

    typedef enum 
    {
        trapContinue,
        trapReturn,
        trapReturn0,
        trapJump
    } TrapType;
    TrapType trapType;
    DWORD jumpTarget; // used for trapJump
};

#endif // ENTRYPOINT_H_
