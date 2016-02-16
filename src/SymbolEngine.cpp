/*
NAME
    SymbolEngine

DESCRIPTION
    Additional symbol engine functionality

COPYRIGHT
    Copyright (C) 2003 by Roger Orr <rogero@howzatt.demon.co.uk>

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
*/

static char const szRCSID[] = "$Id: SymbolEngine.cpp 1603 2016-02-15 22:24:56Z Roger $";

#ifdef _MSC_VER
#pragma warning( disable: 4786 ) // identifier was truncated to '255' chars
#pragma warning( disable: 4511 4512 ) // copy constructor/assignment operator could not be generated
#endif // _MSC_VER

#include "SymbolEngine.h"

#include <windows.h>
#include <psapi.h>
#include <comutil.h>
#pragma comment( lib, "oleaut32.lib" )
#pragma comment( lib, "comsupp.lib" )

// stl
#include <iostream>
#include <iomanip>
#include <sstream>
#include <typeinfo>
#include <map>

#include "../include/MsvcExceptions.h"
#include "../include/StrFromWchar.h"
#include "../include/BasicType.h"
#include "../include/readPartialMemory.h"

#include "GetModuleBase.h"

#pragma comment( lib, "psapi" )

// helper function
namespace
{
   // fix for problem with resource leak in symsrv
   void fixSymSrv();

    // Show function/SEH parameters
    template <typename WORDSIZE>
    void addParams( std::ostream & os, WORDSIZE * pParams, size_t maxParams )
    {
        for ( size_t i = 0; i < maxParams; ++i )
        {
            WORDSIZE const param = pParams[i];
            if ( ( -10 < (long)param ) && ( (long)param < 10 ) )
                os << " " << (long)param;
            else
                os << " " << (PVOID)(ULONG_PTR)param;
        }
    }

    void showVariablesAt( std::ostream& os, DWORD64 codeOffset, DWORD64 frameOffset, or2::SymbolEngine const & eng );

    //////////////////////////////////////////////////////////
    // Helper function: getBaseType maps PDB type + length to C++ name
    std::string getBaseType( DWORD baseType, ULONG64 length );

#ifdef _M_X64
    // Helper function to delay load Wow64GetThreadContext or emulate on W2K3
    BOOL getWow64ThreadContext(HANDLE hProcess, HANDLE hThread, CONTEXT const &context, WOW64_CONTEXT *pWowContext);
#endif // _M_X64
}

namespace or2
{

/////////////////////////////////////////////////////////////////////////////////////
/** Implementation class */
struct SymbolEngine::Impl
{
   std::map< void const *, std::string > addressMap;
};


/////////////////////////////////////////////////////////////////////////////////////
SymbolEngine::SymbolEngine( HANDLE hProcess )
: showLines( true )
, showParams( false )
, showVariables( false )
, maxStackDepth( -1 )
, skipCount( 0 )
, maxSehDepth( 0 )
, pImpl( new Impl )
{
    static bool inited = false;
    if ( ! inited )
    {
        DWORD dwOpts = SymGetOptions ( );
        dwOpts |= SYMOPT_LOAD_LINES | SYMOPT_OMAP_FIND_NEAREST;
        char const * pOption = ::getenv( "OR2_SYMOPT" );
        if ( pOption )
        {
            int extraOptions = 0;
            if ( sscanf( pOption, "%x", &extraOptions ) == 1 )
            {
                dwOpts |= extraOptions;
            }
        }
        SymSetOptions ( dwOpts );
        inited = true;
    }

    Initialise( hProcess );
}

/////////////////////////////////////////////////////////////////////////////////////
//Destroy wrapper
SymbolEngine::~SymbolEngine( )
{
   fixSymSrv();
   delete pImpl;
}


/////////////////////////////////////////////////////////////////////////////////////
// true to show line numbers if possible
void SymbolEngine::setLines( bool value )
{
    showLines = value;
}

bool SymbolEngine::getLines() const
{
    return showLines;
}

// true to show parameters
void SymbolEngine::setParams( bool value )
{
    showParams = value;
}

bool SymbolEngine::getParams() const
{
    return showParams;
}

// true to show variables
void SymbolEngine::setVariables( bool value )
{
    showVariables = value;
}

bool SymbolEngine::getVariables() const
{
    return showVariables;
}

// set stack depth for walkbacks
void SymbolEngine::setMaxDepth( int value )
{
    maxStackDepth = value;
}

int SymbolEngine::getMaxDepth() const
{
    return maxStackDepth;
}

// set skip count for stack walkbacks
void SymbolEngine::setSkipCount( int value )
{
    skipCount = value;
}

int SymbolEngine::getSkipCount() const
{
    return skipCount;
}

// set seh stack depth walkbacks
void SymbolEngine::setSehDepth( int value )
{
    maxSehDepth = value;
}

int SymbolEngine::getSehDepth() const
{
    return maxSehDepth;
}

/////////////////////////////////////////////////////////////////////////////////////
bool SymbolEngine::printAddress( PVOID address, std::ostream& os ) const
{
    bool cacheSymbol(true);

    // Despite having GetModuleBase in the call to StackWalk it needs help for the addresses
    ::GetModuleBase( GetProcess(), (DWORD64)address );

    ///////////////////////////////
    // Log the module + offset
    MEMORY_BASIC_INFORMATION mbInfo;
    if ( ::VirtualQueryEx( GetProcess(), address, &mbInfo, sizeof mbInfo ) &&
         ( ( mbInfo.State & MEM_FREE ) == 0 ) &&
         ( ( mbInfo.Type & MEM_IMAGE ) != 0 ) )
    {
        std::ostringstream str;
        HMODULE const hmod = (HMODULE)mbInfo.AllocationBase;

        char szFileName[ MAX_PATH ] = "";
        if ( ! GetModuleFileNameWrapper( GetProcess(), hmod, szFileName, sizeof szFileName/sizeof szFileName[0] ) )
        {
            cacheSymbol = false;
            str << hmod;
        }
        else 
            str << strrchr(szFileName, '\\') + 1;
        str << " + 0x" << std::hex << ((ULONG_PTR)address - (ULONG_PTR)mbInfo.AllocationBase) << std::dec;

        os << std::setw(30) << std::left << str.str().c_str() << std::right; // c_str() fixes VC6 bug with setw
    }
    else 
    {
        os << address;
        return false;
    }

    ///////////////////////////////
    // Log the symbol name

    // The largest (undecorated) symbol that the MS code generators can handle is 256.
    // I can't believe it will increase more than fourfold when undecorated...
#ifdef DBGHELP_6_1_APIS
    struct 
    {
        SYMBOL_INFO symInfo;
        char name[ 4 * 256 ];
    } SymInfo = { { sizeof( SymInfo.symInfo ) }, "" };

    PSYMBOL_INFO pSym = &SymInfo.symInfo;
    pSym->MaxNameLen = sizeof( SymInfo.name );

    DWORD64 dwDisplacement64(0);
    if ( SymFromAddr( (DWORD64)address, &dwDisplacement64, pSym) )
#else
    struct 
    {
        IMAGEHLP_SYMBOL64 symInfo;
        char name[ 4 * 256 ];
    } SymInfo = { { sizeof( SymInfo.symInfo ) }, "" };

    PIMAGEHLP_SYMBOL64 pSym = &SymInfo.symInfo;
    pSym->MaxNameLength = sizeof( SymInfo.name );

    DWORD64 dwDisplacement64;
    if ( GetSymFromAddr64( (DWORD)address, &dwDisplacement64, pSym) )
#endif
    {
        os << " " << pSym->Name;
        if ( dwDisplacement64 != 0 )
        {
            int displacement = static_cast<int>(dwDisplacement64);
            if ( displacement < 0 )
               os << " - " << -displacement;
            else
               os << " + " << displacement;
        }
    }
        
    ///////////////////////////////
    // Log the line number

    if ( showLines )
    {
        DbgInit<IMAGEHLP_LINE64> lineInfo;
        DWORD dwDisplacement(0);
        if ( GetLineFromAddr64( (DWORD64)address, &dwDisplacement, &lineInfo ) )
        {
            os << "   " << lineInfo.FileName << "(" << lineInfo.LineNumber << ")";
            if (dwDisplacement != 0)
            {
                os << " + " << dwDisplacement << " byte" << (dwDisplacement == 1 ? "" : "s");
            }
        }
    }
    return cacheSymbol;
}

/////////////////////////////////////////////////////////////////////////////////////
// Convert address to a string.
std::string SymbolEngine::addressToName( PVOID address ) const
{
   std::map< void const *, std::string >::iterator it = pImpl->addressMap.find( address );
   if ( it == pImpl->addressMap.end() )
   {
      std::ostringstream oss;
      if (!printAddress( address, oss ))
         return oss.str();
      it = pImpl->addressMap.insert( std::make_pair( address, oss.str() ) ).first;
   }

   return it->second;
}

/////////////////////////////////////////////////////////////////////////////////////
// StackTrace: try to trace the stack to the given output stream
void SymbolEngine::StackTrace( HANDLE hThread, const CONTEXT & context, std::ostream & os ) const
{
    STACKFRAME64 stackFrame = {0};
    CONTEXT rwContext = {0};
    try
    {
        rwContext = context;
    }
    catch (...)
    {
        // stack based context may be missing later sections -- based on the flags
    }
    PVOID pContext = &rwContext; // it is claimed this is not needed on Intel...lies

#ifdef _M_IX86
    DWORD const machineType = IMAGE_FILE_MACHINE_I386;

    // StackFrame needs to be set up on Intel
    stackFrame.AddrPC.Offset = context.Eip;
    stackFrame.AddrPC.Mode = AddrModeFlat;

    stackFrame.AddrFrame.Offset = context.Ebp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;

    stackFrame.AddrStack.Offset = context.Esp;
    stackFrame.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
    DWORD machineType = IMAGE_FILE_MACHINE_AMD64;

    stackFrame.AddrPC.Offset = context.Rip;
    stackFrame.AddrPC.Mode = AddrModeFlat;

    stackFrame.AddrFrame.Offset = context.Rbp;
    stackFrame.AddrFrame.Mode = AddrModeFlat;

    stackFrame.AddrStack.Offset = context.Rsp;
    stackFrame.AddrStack.Mode = AddrModeFlat;

    BOOL bWow64(false);
    WOW64_CONTEXT wow64_context = {0};
    wow64_context.ContextFlags = WOW64_CONTEXT_FULL;

    if (IsWow64Process(GetProcess(), &bWow64) && bWow64)
    {
        if (getWow64ThreadContext(GetProcess(), hThread, rwContext, &wow64_context))
        {
            machineType = IMAGE_FILE_MACHINE_I386;
            pContext = &wow64_context;
            stackFrame.AddrPC.Offset = wow64_context.Eip;
            stackFrame.AddrFrame.Offset = wow64_context.Ebp;
            stackFrame.AddrStack.Offset  = wow64_context.Esp;
        }
    }
#else
#error Unsupported target platform
#endif // _M_IX86

    // For loop detection
    DWORD64 currBp = 0;

    // For 'wandering stack'
    DWORD nonExec(0);
    DWORD const maxNonExec(3);

    // use local copies of instance data
    int depth = maxStackDepth;
    int skip = skipCount;

    // Despite having GetModuleBase in the call to StackWalk it needs help for the first address
    ::GetModuleBase(GetProcess(), stackFrame.AddrPC.Offset);

    while ( ::StackWalk64(machineType,
       GetProcess(), hThread,
       &stackFrame, pContext,
       NULL,
       0, // implies ::SymFunctionTableAccess,
       ::GetModuleBase,
       NULL ) )
    {
        if ( stackFrame.AddrFrame.Offset == 0 )
            break;

        if (stackFrame.AddrPC.Offset == 0)
        {
            os << "Null address\n";
            break;
        }

        if ( currBp != 0 )
        {
            if ( currBp >= stackFrame.AddrFrame.Offset )
            {
                os << "Stack frame: " << (PVOID)stackFrame.AddrFrame.Offset << " out of sequence\n";
                break;
            }
        }
        currBp = stackFrame.AddrFrame.Offset;

        // This helps x64 stack walking -- I think this might be a bug as the function is
        // already supplied in the StackWalk64 call ...
        ::GetModuleBase(GetProcess(), stackFrame.AddrPC.Offset);

        if ( skip > 0 )
        {
            skip--;
            continue;
        }

        if ( depth > -1 )
        {
            if ( depth-- == 0 )
                break;
        }

        if ( isExecutable(stackFrame.AddrPC.Offset))
        {
            nonExec = 0;
        }
        else
        {
            ++nonExec;
            if (nonExec > maxNonExec)
               break;
        }

        os << addressToName( (PVOID)stackFrame.AddrPC.Offset );

        if (nonExec)
        {
            os << " (non executable)";
        }

        os << "\n";

#if 0
        os << "AddrPC: " << (PVOID)stackFrame.AddrPC.Offset
           << " AddrReturn: " << (PVOID)stackFrame.AddrReturn.Offset
           << " AddrFrame: " << (PVOID)stackFrame.AddrFrame.Offset
           << " AddrStack: " << (PVOID)stackFrame.AddrStack.Offset
           << " FuncTableEntry: " << (PVOID)stackFrame.FuncTableEntry
           << " Far: " << stackFrame.Far
           << " Virtual: " << stackFrame.Virtual
           << " AddrBStore: " << (PVOID)stackFrame.AddrBStore.Offset
           << "\n";
#endif

        if ( showParams )
        {
            os << "  " << (PVOID)stackFrame.AddrFrame.Offset << ":";
            addParams( os, stackFrame.Params, sizeof( stackFrame.Params ) / sizeof( stackFrame.Params[0] ) );
            os << "\n";
        }
        if ( showVariables )
        {
            showVariablesAt( os, stackFrame.AddrPC.Offset, stackFrame.AddrFrame.Offset, *this );
        }
    }

    os.flush();
}

//////////////////////////////////////////////////////////
// GetCurrentThreadContext
//
// Get context for the current thread, correcting the stack frame to the caller
//
// We sort out 3 key registers after GetThreadContext to
// prevent trying to stack walk after we've modified the stack...
// static
#ifdef _M_IX86
BOOL __declspec( naked ) SymbolEngine::GetCurrentThreadContext( CONTEXT * pContext )
{
    DWORD regIp, regSp, regBp;
    BOOL rc;

    _asm push ebp
    _asm mov  ebp,esp
    _asm sub esp,__LOCAL_SIZE

    rc = ::GetThreadContext( GetCurrentThread(), pContext );
    
    if ( rc )
    {
        _asm mov eax,[ebp+4] ; return address
        _asm mov regIp,eax
        _asm lea eax,[ebp+0ch] ; caller's SP before pushing pContext
        _asm mov regSp,eax
        _asm mov eax,[ebp] ; caller's BP
        _asm mov regBp,eax

        pContext->Eip = regIp;
        pContext->Esp = regSp;
        pContext->Ebp = regBp;
    }

    _asm mov eax,rc
    _asm mov esp,ebp
    _asm pop ebp
    _asm ret
}
#else
/** get context for the current thread, correcting the stack frame to the caller */
void (WINAPI *SymbolEngine::GetCurrentThreadContext)(PCONTEXT pContext) = RtlCaptureContext;
#endif // _M_IX86

//////////////////////////////////////////////////////////
//
void SymbolEngine::SEHTrace( PVOID ExceptionList, std::ostream& os ) const
{
    // Got the first entry of the exception stack
    for ( int i = 0; (maxSehDepth < 0) || (i < maxSehDepth); ++i )
    {
        if ( ExceptionList == (PVOID)(INT_PTR)-1 || ExceptionList == 0 )
            break;

        struct Frame
        {
            PVOID previous;
            PVOID handler;
        } frame;

        if ( ! ReadMemory( ExceptionList, (PVOID)&frame, sizeof( frame ) ) )
        {
            std::cerr << "ReadProcessMemory at " << ExceptionList << " failed: " << GetLastError() << std::endl;
            return;
        }

        os << addressToName( frame.handler );
        os << "\n";

        PVOID catchHandler = 0;
        bool isMsvcHandler = findMsvcCppHandler( frame.handler, &catchHandler );

        if ( showParams )
        {
            struct
            {
                Frame frame;
                DWORD Params[3];
            } extendedFrame;

            if ( ReadMemory( ExceptionList, &extendedFrame, sizeof( extendedFrame ) ) )
            {
                os << "  " << ExceptionList << ":";
                addParams( os, extendedFrame.Params, sizeof( extendedFrame.Params ) / sizeof( extendedFrame.Params[0] ) );

                // For Msvc Cpp handlers params[2] is actually the return address
                if ( isMsvcHandler )
                {
                    os << " [" << addressToName( (PVOID)(ULONG_PTR)extendedFrame.Params[2] ) << "]";
                }
                os << "\n";
            }
        }

        if ( catchHandler )
        {
            os << "  => " << addressToName( catchHandler ) << "\n";
        }

        if ( ExceptionList < frame.previous )
        {
            ExceptionList = frame.previous;
        }
        else if ( ExceptionList > frame.previous )
        {
            os << "  ends (" << frame.previous << "<" << ExceptionList << ")\n";
            break;
        }
        else
        {
            // Cygwin ends with self-referential handler
            break;
        }
    }
}

//////////////////////////////////////////////////////////
// Hack to try and get first catch handler for MSVC exception
//
// Attempt to walk the internal Microsoft structures
// to find the first catch handler for a C++ exception

// Returns:
//    false if not an Msvc C++ handler, msvcHandler = 0
//    true
//      msvcHandler = 0 if no catch handler [implicit frame unwind code]
//      else handler address

bool SymbolEngine::findMsvcCppHandler( PVOID sehHandler, PVOID *msvcHandler ) const
{
    // Set the default return value
    *msvcHandler = 0;

    BYTE buffer[ 1 + sizeof( PVOID ) ]; // read mov instruction
    PVOID *bufptr; // pointer into buffer

    // Read 5 bytes from handler address and check 1st byte is
    // a mov eax (0xb8)
    if ( ( ! ReadMemory( sehHandler, (PVOID)buffer, sizeof(buffer) ) ) ||
         ( buffer[0] != 0xb8 ) )
    {
        return false;
    }

    // deref and read FrameHandler
    bufptr = (PVOID*)(buffer+1);
    PVOID pFrameHandler = *bufptr;
    MsvcFrameHandler frameHandler;
    if ( ! ReadMemory( pFrameHandler, (PVOID)&frameHandler, sizeof( frameHandler ) ) )
        return false;

    // Verify 'magic number'
    if ( frameHandler.magic != MSVC_MAGIC_NUMBER1 )
        return false;

    // We have definitely got an MSVC handler - has it got a catch address?

    if ( frameHandler.cTryEntry == 0 )
        return true;

    // Read first try entry
    MsvcTryEntry tryEntry;
    if ( ! ReadMemory( frameHandler.pTryEntry, (PVOID)&tryEntry, sizeof( tryEntry ) ) )
        return true;

    // Read first catch entry
    MsvcCatchEntry catchEntry;
    if ( ! ReadMemory( tryEntry.pCatchEntry, (PVOID)&catchEntry, sizeof( catchEntry ) ) )
        return true;

    // return first target address
    *msvcHandler = catchEntry.catchHandler;
    return true;
}

//////////////////////////////////////////////////////////
// showMsvcThrow
//
// Attempt to find type information for MSVC C++ throw parameter

void SymbolEngine::showMsvcThrow( std::ostream &os, PVOID throwInfo, PVOID base ) const
{
    MsvcThrow msvcThrow = {0};
    MsvcClassHeader msvcClassHeader = {0};
    MsvcClassInfo msvcClassInfo = {0};
    BYTE raw_type_info[ sizeof( type_info ) + 256 ] = "";

    if ( ! ReadMemory( (PVOID)throwInfo, &msvcThrow, sizeof( msvcThrow ) ) ||
         ! ReadMemory( (PVOID)((ULONG_PTR)base + msvcThrow.pClassHeader), &msvcClassHeader, sizeof( msvcClassHeader ) ) ||
         ! ReadMemory( (PVOID)((ULONG_PTR)base + msvcClassHeader.Info[0]), &msvcClassInfo, sizeof( msvcClassInfo ) ) ||
         ! ReadPartialProcessMemory( GetProcess(), (PVOID)((ULONG_PTR)base + msvcClassInfo.pTypeInfo), &raw_type_info, sizeof( type_info ), sizeof( raw_type_info ) -1 ) )
    {
        return;
    }

    const std::type_info *pType_info = (const std::type_info *)raw_type_info;

    char buffer[ 1024 ] = ""; 
    if ( UnDecorateSymbolName( pType_info->raw_name() + 1, buffer, sizeof( buffer ),
         UNDNAME_32_BIT_DECODE | UNDNAME_NO_ARGUMENTS ) )
    { 
        os << " (" << buffer << ")"; 
    } 
}

// Helper for ReadProcessMemory
bool SymbolEngine::ReadMemory( 
          LPCVOID lpBaseAddress,        // base of memory area
          LPVOID lpBuffer,              // data buffer
          SIZE_T nSize ) const          // number of bytes to read
{
#pragma warning( disable: 4800 ) // forcing value to bool 'true' or 'false'

    return ReadProcessMemory( GetProcess(), lpBaseAddress, lpBuffer, nSize, 0 );
}

//////////////////////////////////////////////////////////
// DbgHelp 6.1 functionality - get the name for a symbol
BOOL SymbolEngine::decorateName( std::string & name, ULONG64 ModBase, ULONG TypeIndex ) const
{
    BOOL bRet( false );

#ifdef DBGHELP_6_1_APIS

    WCHAR *typeName = 0;
    if ( GetTypeInfo( ModBase, TypeIndex, TI_GET_SYMNAME, &typeName ) )
    {
        bool const nested = (name.length() != 0);
        if ( nested )
        {
            name.insert( 0, " ");
        }
        name.insert( 0, or2::strFromWchar( typeName ) );
        // free memory for typeName - by experiment with a debugger it comes from LocalAlloc
        LocalFree( typeName );
        if ( nested )
        {
            return true;
        }
    }

    bool bRecurse( false ); // set to true to recurse down the type tree
    enum SymTagEnum tag = (enum SymTagEnum)0;
    GetTypeInfo( ModBase, TypeIndex, TI_GET_SYMTAG, &tag );
    switch ( tag )
    {
    case SymTagBaseType:
    {
        DWORD baseType(0);
        ULONG64 length(0);
        GetTypeInfo( ModBase, TypeIndex, TI_GET_BASETYPE, &baseType );
        GetTypeInfo( ModBase, TypeIndex, TI_GET_LENGTH, &length );
        name.insert( 0, " " );
        name.insert( 0, getBaseType( baseType, length ) );
        bRet = true;
        break;
    }
    case SymTagPointerType:
        name.insert( 0, "*" );
        bRecurse = true;
        break;
    case SymTagFunctionType:
        if ( name[0] == '*' )
        {
            name.insert( 0, "(" );
            name += ")";
        }
        name += "()";
        bRecurse = true;
        break;
    case SymTagArrayType:
    {
        if ( name[0] == '*' )
        {
            name.insert( 0, "(" );
            name += ")";
        }
        DWORD Count(0);
        GetTypeInfo( ModBase, TypeIndex, TI_GET_COUNT, &Count );
        name += "[";
        if ( Count )
        {
            std::ostringstream oss;
            oss << Count;
            name += oss.str();
        }
        name += "]";
        bRecurse = true;
        break;
    }
    case SymTagFunction:
    case SymTagData:
        bRecurse = true;
        break;
    case SymTagBaseClass:
        break;
    default:
        {
        std::ostringstream oss;
        oss << tag << " ";
        name.insert( 0, oss.str() );
        break;
        }
    }

    if ( bRecurse )
    {
        DWORD ti = 0;
        if ( GetTypeInfo( ModBase, TypeIndex, TI_GET_TYPEID, &ti ) )
        {
            bRet = decorateName( name, ModBase, ti );
        }
    }

    _variant_t value;
    if ( GetTypeInfo( ModBase, TypeIndex, TI_GET_VALUE, &value ) )
    {
       value.ChangeType( VT_BSTR );
       name += "=" + or2::strFromWchar( value.bstrVal );
    }

#endif // DBGHELP_6_1_APIS

    return bRet;
}

///////////////////////////////////////////////////////////////////////////
/** enumerate local variables at an address */
BOOL SymbolEngine::enumLocalVariables( DWORD64 codeOffset, DWORD64 frameOffset, EnumLocalCallBack & cb ) const
{
#ifdef DBGHELP_6_1_APIS

    struct CallBack
    {
        CallBack( SymbolEngine const & eng, EnumLocalCallBack & cb )
        : eng( eng ), cb( cb ) {}

        static BOOL CALLBACK enumSymbolsProc(
            PSYMBOL_INFO pSymInfo,
            ULONG /*SymbolSize*/,
            PVOID UserContext )
        {
            CallBack& thisCb = *(CallBack*)UserContext;

            return thisCb.cb( thisCb.eng, pSymInfo );
        }

        SymbolEngine const & eng;
        EnumLocalCallBack & cb;
    };

    IMAGEHLP_STACK_FRAME stackFrame = {0};
    stackFrame.InstructionOffset = codeOffset;
    stackFrame.FrameOffset = frameOffset;

    BOOL ret = SetContext( &stackFrame, 0 );
    // Note: by experiment with SymUnpack must ignore failures from SetContext ...
    CallBack callBack( *this, cb );
    ret = EnumSymbols( 0, "*", CallBack::enumSymbolsProc, &callBack );
    return ret;

#else

	return false;

#endif // DBGHELP_6_1_APIS
}

///////////////////////////////////////////////////////////
/* Write a simple mini-dump for an exception in the <b>current</b> thread */
BOOL SymbolEngine::dumpSelf( std::string const & miniDumpFile, EXCEPTION_POINTERS *ExceptionInfo )
{
   BOOL ret( FALSE );

#ifdef DBGHELP_6_1_APIS
   HANDLE const hDumpFile = CreateFile( miniDumpFile.c_str(), FILE_WRITE_DATA, 0, NULL, CREATE_ALWAYS, 0, 0 );

   if ( hDumpFile != INVALID_HANDLE_VALUE )
   {
      MINIDUMP_EXCEPTION_INFORMATION ExceptionParam;
      ExceptionParam.ThreadId = GetCurrentThreadId();
      ExceptionParam.ExceptionPointers = ExceptionInfo;
      ExceptionParam.ClientPointers = TRUE;

      ret = WriteMiniDump( ::GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &ExceptionParam, 0, 0 );

      CloseHandle( hDumpFile );
   }
#endif // DBGHELP_6_1_APIS

   return ret;
}

/////////////////////////////////////////////////////////////////////////////////////
// Read a string from the target
std::string SymbolEngine::getString(PVOID address, BOOL unicode, DWORD maxStringLength) const
{
  if (unicode)
  {
    std::vector<wchar_t> chVector(maxStringLength + 1);
    ReadPartialProcessMemory(GetProcess(), address, &chVector[0], sizeof(wchar_t), maxStringLength * sizeof(wchar_t));
    size_t const wcLen = wcstombs(0, &chVector[0], 0);
    if (wcLen == (size_t)-1)
    {
       return "invalid string";
    }
    else
    {
       std::vector<char> mbStr(wcLen + 1);
       wcstombs(&mbStr[0], &chVector[0], wcLen);
       return &mbStr[0];
    }
  }
  else
  {
    std::vector<char> chVector(maxStringLength + 1);
    ReadPartialProcessMemory(GetProcess(), address, &chVector[0], 1, maxStringLength);
    return &chVector[0];
  }
}

/////////////////////////////////////////////////////////////////////////////////////
// Returns whether address points to executable code
bool SymbolEngine::isExecutable(DWORD64 address) const
{
    bool ret(false);

    static const DWORD AnyExecute = 
        PAGE_EXECUTE |
        PAGE_EXECUTE_READ | 
        PAGE_EXECUTE_READWRITE |
        PAGE_EXECUTE_WRITECOPY;

    MEMORY_BASIC_INFORMATION mb = { 0 };
    if ( VirtualQueryEx( GetProcess(), (PVOID)address, &mb, sizeof( mb ) ) )
    {
        if ( ( mb.Protect & (AnyExecute) ) != 0 )
        {
            ret = true; // executable code
        }
    }

    return ret;
}

} // namespace or2

namespace {

//////////////////////////////////////////////////////////
// fix for problem with resource leak in symsrv
void fixSymSrv()
{
   static bool loaded = false;   
   if ( !loaded )
   {
      HMODULE const hSymSrv = ::GetModuleHandle( "SymSrv" );
      if ( hSymSrv != 0 )
      {
         ::LoadLibrary( "SymSrv" );
         loaded = true;
      }
   }
}

//////////////////////////////////////////////////////////
void showVariablesAt( std::ostream& os, DWORD64 codeOffset, DWORD64 frameOffset, or2::SymbolEngine const & eng )
{
#ifdef DBGHELP_6_1_APIS

    struct CallBack : public or2::SymbolEngine::EnumLocalCallBack
    {
        CallBack( std::ostream &opf, DWORD64 frameOffset )
        : opf(opf), frameOffset( frameOffset ) {}

        virtual bool operator()( or2::SymbolEngine const & eng, PSYMBOL_INFO pSymInfo )
        {
            if ( ( pSymInfo->Flags & SYMFLAG_LOCAL ) &&
                 ( pSymInfo->Flags & SYMFLAG_REGREL ) )
            {
                std::string name( pSymInfo->Name, pSymInfo->NameLen );
                eng.decorateName( name, pSymInfo->ModBase, pSymInfo->TypeIndex );
                opf << "  " << name;
                opf << " [ebp";
                if ( pSymInfo->Address > 0x7fffffff )
                    opf << "-" << std::hex << -(int)pSymInfo->Address << std::dec;
                else
                    opf << "+" << std::hex << (int)pSymInfo->Address << std::dec;
                opf << "]";
                if ( pSymInfo->Size == sizeof( char ) )
                {
                    unsigned char data;
                    eng.ReadMemory( (PVOID)( frameOffset + pSymInfo->Address ), &data, sizeof( data ) );
                    if (isprint(data))
                        opf << " = '" << data << '\'';
                    else
                        opf << " = " << (int)data;
                }
                else if ( pSymInfo->Size == sizeof( short ) )
                {
                    unsigned short data;
                    eng.ReadMemory( (PVOID)( frameOffset + pSymInfo->Address ), &data, sizeof( data ) );
                    opf << " = " << data;
                }
                else if ( ( pSymInfo->Size == sizeof(int) ) || ( pSymInfo->Size == 0 ) )
                {
                    unsigned int data;
                    eng.ReadMemory( (PVOID)( frameOffset + pSymInfo->Address ), &data, sizeof( data ) );
                    opf << " = 0x" << std::hex << data << std::dec;
                }
                else if ( ( pSymInfo->Size == 8 ) && ( name.compare( 0, 6, "double" ) == 0 ) )
                {
                    double data;
                    eng.ReadMemory( (PVOID)( frameOffset + pSymInfo->Address ), &data, sizeof( data ) );
                    opf << " = " << data;
                }
                else if ( ( pSymInfo->Size == 8 ) )
                {
                    LONGLONG data;
                    eng.ReadMemory( (PVOID)( frameOffset + pSymInfo->Address ), &data, sizeof( data ) );
#if _MSC_VER <= 1200
                    opf << " = 0x" << std::hex << (double)data << std::dec;
#else
                    opf << " = 0x" << std::hex << data << std::dec;
#endif // _MSC_VER
                }
                opf << std::endl;
            }

            return true;
        }

        std::ostream &opf;
        DWORD64 frameOffset;
    };

    CallBack cb( os, frameOffset );

    eng.enumLocalVariables( codeOffset, frameOffset, cb );
#endif // DBGHELP_6_1_APIS
}

//////////////////////////////////////////////////////////
// Helper function: getBaseType maps PDB type + length to C++ name
std::string getBaseType( DWORD baseType, ULONG64 length )
{
    static struct
    {
        DWORD baseType;
        ULONG64 length;
        const char * name;
    } baseList[] = 
    {
        // Table generated from dumping out 'baseTypes.cpp'
        { btNoType,  0,                        "(null)" }, // Used for __$ReturnUdt
        { btVoid,    0,                        "void" },
        { btChar,    sizeof( char ),           "char" },
        { btWChar,   sizeof( wchar_t ),        "wchar_t" },
        { btInt,  sizeof( signed char ),    "signed char" },
        { btInt,  sizeof( short ),          "short" },
        { btInt,  sizeof( int ),            "int" },
        { btInt,  sizeof( __int64 ),        "__int64" },
        { btUInt,  sizeof( unsigned char ),  "unsigned char" }, // also used for 'bool' in VC6
        { btUInt,  sizeof( unsigned short ), "unsigned short" },
        { btUInt,  sizeof( unsigned int ),   "unsigned int" },
        { btUInt,  sizeof( unsigned __int64 ),"unsigned __int64" },
        { btFloat,  sizeof( float ),          "float" },
        { btFloat,  sizeof( double ),         "double" },
        { btFloat,  sizeof( long double ),    "long double" },
        // btBCD
        { btBool, sizeof( bool ),           "bool" }, // VC 7.x
        { btLong, sizeof( long ),           "long" },
        { btULong, sizeof( unsigned long ),  "unsigned long" },
        // btCurrency
        // btDate
        // btVariant
        // btComplex
        // btBit
        // btBSTR
        { btHresult, sizeof( HRESULT ), "HRESULT" },

    };

    for ( int i = 0; i < sizeof( baseList ) / sizeof( baseList[0] ); ++i )
    {
        if ( ( baseType == baseList[i].baseType ) &&
             ( length == baseList[i].length ) )
        {
            return baseList[i].name;
        }
    }

    // Unlisted type - use the data values and then fix the code (!)
    std::ostringstream oss;
    oss << "pdb type: " << baseType << "/" << (DWORD)length;
    return oss.str();
}

#ifdef _M_X64
    static DWORD const WOW64_CS_32BIT = 0x23; // Wow64 32-bit code segment on Windows 2003
    static DWORD const TLS_OFFSET = 0x1480; // offsetof(ntdll!_TEB, TlsSlots) on Windows 2003
    #pragma pack(4)
    struct Wow64_SaveContext {
        ULONG unknown1;
        WOW64_CONTEXT context;
        ULONG unknown2;
    };
    #pragma pack()

    typedef BOOL (WINAPI *pfnWow64GetThreadContext)
        (HANDLE, WOW64_CONTEXT*);

    typedef NTSTATUS (WINAPI *pfnNtQueryInformationThread)
        (HANDLE ThreadHandle, ULONG ThreadInformationClass, PVOID Buffer, ULONG Length, PULONG ReturnLength);

    // Helper function to delay load Wow64GetThreadContext or emulate on W2K3
    BOOL getWow64ThreadContext(HANDLE hProcess, HANDLE hThread, CONTEXT const &context, WOW64_CONTEXT *pWowContext)
    {
        static HMODULE hKernel32 = ::GetModuleHandle("KERNEL32");
        static pfnWow64GetThreadContext pFn = (pfnWow64GetThreadContext)::GetProcAddress(hKernel32, "Wow64GetThreadContext");
        if (pFn)
        {
            // Vista and above
            return pFn(hThread, pWowContext);
        }
        else if (context.SegCs == WOW64_CS_32BIT)
        {
            if (pWowContext->ContextFlags & CONTEXT_CONTROL)
            {
                pWowContext->Ebp = (ULONG)context.Rbp;
                pWowContext->Eip = (ULONG)context.Rip;
                pWowContext->SegCs = context.SegCs;
                pWowContext->EFlags = context.EFlags;
                pWowContext->Esp = (ULONG)context.Rsp;
                pWowContext->SegSs = context.SegSs;
            }
            if (pWowContext->ContextFlags & CONTEXT_INTEGER)
            {
                pWowContext->Edi = (ULONG)context.Rdi;
                pWowContext->Esi = (ULONG)context.Rsi;
                pWowContext->Ebx = (ULONG)context.Rbx;
                pWowContext->Edx = (ULONG)context.Rdx;
                pWowContext->Ecx = (ULONG)context.Rcx;
                pWowContext->Eax = (ULONG)context.Rax;
            }
            return true;
        }
        else
        {
            static HMODULE hNtDll = ::GetModuleHandle("NTDLL");
            static pfnNtQueryInformationThread pNtQueryInformationThread = (pfnNtQueryInformationThread)::GetProcAddress(hNtDll, "NtQueryInformationThread");
            ULONG_PTR ThreadInfo[6] = {0};
            if (pNtQueryInformationThread && pNtQueryInformationThread(hThread, 0, &ThreadInfo, sizeof(ThreadInfo), 0) == 0)
            {
                PVOID *pTls = (PVOID *)(ThreadInfo[1] + TLS_OFFSET);
                Wow64_SaveContext saveContext = {0}, *pSaveContext = 0;

                if (ReadProcessMemory(hProcess, pTls + 1, &pSaveContext, sizeof(pSaveContext), 0) &&
                    ReadProcessMemory(hProcess, pSaveContext, &saveContext, sizeof(saveContext), 0))
                {
                    *pWowContext = saveContext.context;
                    return true;
                }
            }
        }
        return false;
    }
#endif // _M_X64

} // namespace
