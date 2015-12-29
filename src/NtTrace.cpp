/*
NAME
    NtTrace.cpp

DESCRIPTION
    Process to provide tracing for NT 'native' API

COPYRIGHT
    Copyright (C) 2002,2012 by Roger Orr <rogero@howzatt.demon.co.uk>

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

EXAMPLE
    NtTrace fred.exe
    - or -
    NtTrace 1234
*/

static char const szRCSID[] = "$Id: NtTrace.cpp 1439 2014-10-07 23:24:07Z Roger $";

#pragma warning( disable: 4786 ) // identifier was truncated to '255' characters
#pragma warning( disable: 4800 ) // forcing value to bool 'true' or 'false' (performance warning)
#pragma warning( disable: 4511 4512 ) // copy ctor/op= could not be generated
#pragma warning( disable: 4996 ) // 'asctime' and others were declared deprecated

#ifdef _M_X64
#include <ntstatus.h>
#define WIN32_NO_STATUS
#endif

#include <windows.h>
#include <iterator>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sys/timeb.h>
#include <time.h>

#include <psapi.h> // LOAD_DLL_DEBUG_INFO does not always give us lpImageName

// or2 includes
#include <displayError.h>
#include <DebugPriv.h>
#include <GetFileNameFromHandle.h>
#include <Options.h>
#include <ProcessHelper.h>
#include <ReadInt.h>
#include <NtDllStruct.h>
#include <SimpleTokenizer.h>
#include <MsvcExceptions.h>

#include "DebugDriver.h"
#include "EntryPoint.h"
#include "ShowData.h"
#include "SymbolEngine.h"

using namespace showData;
using namespace or2;

#pragma comment( lib, "advapi32" ) // for privilege stuff
#pragma comment( lib, "psapi" )

//////////////////////////////////////////////////////////////////////////
// Local classes

/** Debugger event handler for trapped entry points */
class TrapNtDebugger : public Debugger
{
public:
   /**
    * Construct a debugger
    * @param os the output stream to write to
    */
    TrapNtDebugger( std::ostream & os ) : os( os ), bNoExcept( false ), bActive(true) {}

    // callbacks on events
    virtual void OnException( HANDLE hProcess, HANDLE hThread, DWORD pid, DWORD tid, EXCEPTION_DEBUG_INFO const & DebugEvent, DWORD * pContinueExecution );
    virtual void OnCreateThread( DWORD dwThreadId, CREATE_THREAD_DEBUG_INFO const & CreateThread );
    virtual void OnCreateProcess( DWORD dwProcessId, DWORD dwThreadId, CREATE_PROCESS_DEBUG_INFO const & CreateProcessInfo );
    virtual void OnExitThread( DWORD dwThreadId, EXIT_THREAD_DEBUG_INFO const & ExitThread );
    virtual void OnExitProcess( DWORD dwProcessId, EXIT_PROCESS_DEBUG_INFO const & ExitProcess );
    virtual void OnLoadDll( HANDLE hProcess, LOAD_DLL_DEBUG_INFO const & LoadDll );
    virtual void OnUnloadDll( UNLOAD_DLL_DEBUG_INFO const & UnloadDll );
    virtual void OnOutputDebugString( HANDLE hProcess, OUTPUT_DEBUG_STRING_INFO const & DebugString );
    virtual bool Active() { return bActive; }

    /**
     * Set the 'noexception' flag.
     * @param b the new value: if true exceptions will be ignored
     */
    void setNoException( bool b ) { bNoExcept = b; }

    /**
     * Set the categories
     * @param category a comma-delimited list of categories to trace
     */
    void setCategory( std::string const & category ) { std::vector<std::string> vec; SimpleTokenizer( category, &vec, ',' ); categories.insert( vec.begin(), vec.end() ); }

    /**
     * Set the function-name filter
     * @param filter a comma-delimited list of function names to filter on
     */
    void setFilter( std::string const & filter )
    {
        SimpleTokenizer( filter, &filters, ',' );
        // starting with "-" sets an inverse filter
        inverseFilter = (filters.size() > 0 && filters[0].size() > 0 && filters[0][0] == '-');
        if (inverseFilter)
        {
            filters[0].erase(0, 1);
        }
    }

    /** initialise the debugger */
    bool initialise();
	
    /** List categories, if category supplied was '?' and return true. Otherwise return false */
    bool listCategories();

    /** Mapping NtXxx names to offsets within target DLL */
    typedef std::map<std::string, DWORD> Offsets;

    /** Set orderly close down on Ctrl+C */
    void setCtrlC();

private:
    bool bNoExcept;
    std::ostream & os;

    bool bActive;
    static TrapNtDebugger *ctrlcTarget;
    static BOOL __stdcall CtrlHandler( DWORD fdwCtrlType );

    std::map<DWORD, HANDLE> processes; // map of all active child processes
    EntryPointSet entryPoints; // Set of all entry points
    EntryPoint::Typedefs typedefs;

    Offsets offsets; // Offsets of potential Nt functions in the target Dll (not needed for NtDll)

    void populateOffsets();

    typedef std::map< LPVOID, NtCall > NTCALLS;
    NTCALLS NtCalls; // Complete list of all the calls we're tracking
    NTCALLS NtPreSave; // Pre save list of all the calls we're tracking

    HMODULE BaseOfNtDll; // base of NTDLL.DLL

    std::string target; // name of target DLL (blank => default)
    HMODULE TargetDll; // handle of the target DLL (by default, NTDLL.DLL)

    std::set< std::string > categories; // If not empty, categories to trace
    bool inverseFilter; // If true, exclude when filtered
    std::vector< std::string > filters; // If not empty, filter for 'active' entry points

    void OnBreakpoint(HANDLE hProcess, HANDLE hThread, DWORD pid, DWORD tid, LPVOID exceptionAddress);

    void SetDllBreakpoints( HANDLE hProcess );
    void showUnused( std::set<std::string> const & unused, std::string const & name );
    void showModuleNameEx( HANDLE hProcess, PVOID lpModuleBase, HANDLE hFile ) const;
    void header( DWORD pid, DWORD tid );
    bool detachAll();
    bool detach(DWORD processId, HANDLE hProcess);
};

//static
TrapNtDebugger *TrapNtDebugger::ctrlcTarget;


///////////////////////////////////////////////////////////////////////////////
// Helper functions
namespace
{
    ///////////////////////////////////////////////////////////////////////////
    // Return string for 'now' - substring of asctime + milliseconds
    std::string now()
    {
        struct _timeb timeNow;
        _ftime( &timeNow );

        static _timeb lasttime;
        static char seconds[] = "HH:MM:SS";
        if ( lasttime.time != timeNow.time )
        {
            memcpy( seconds, asctime( localtime( &timeNow.time ) ) + 11, 8 );
            lasttime.time = timeNow.time;
        }
        char result[8 + 1 + 3 + 1];
        sprintf( result, "%s.%03i", seconds, static_cast<int>(timeNow.millitm) );
        return result;
    }

    ///////////////////////////////////////////////////////////////////////////
    // Return string for 'delta time' - seconds + milliseconds (+[ss]s.mmm)
    std::string delta()
    {
        struct _timeb timeNow;
        _ftime( &timeNow );

        static _timeb lastTime;

        char result[4 + 1 + 3 + 1];
        result[0] = '\0';

        if ( lastTime.time != 0 )
        {
           struct _timeb diff = timeNow;
           if ( diff.millitm < lastTime.millitm )
           {
              diff.millitm += 1000;
              diff.time -= 1;
           }
           diff.time -= lastTime.time;
           #pragma warning( push )
           #pragma warning( disable: 4244 ) // conversion from 'int' to 'unsigned short'
           diff.millitm -= lastTime.millitm;
           #pragma warning( pop )
           
           if ( diff.time < 0 )
           {
               strcpy( result, "<0" );
           }
           else if ( diff.time > 999 )
           {
               strcpy( result, ">999s" );
           }
           else
           {
              sprintf( result, "+%i.%03i", static_cast<int>(diff.time), static_cast<int>(diff.millitm) );
           }
        }
        lastTime = timeNow;
        return result;
    }
}

//////////////////////////////////////////////////////////////////////////
// Local data

static bool bErrorsOnly( false );
static std::set<NTSTATUS> errorCodes;
static bool bNames( false );
static bool bPreTrace( false );
static bool bStackTrace( false );
static bool bTimestamp( false );
static bool bDelta( false );
static bool bPid( false );
static bool bTid( false );
static bool bNewline(false);
static std::string configFile; // override default config file

static std::string exportFile; // Export symbols here once loaded

//////////////////////////////////////////////////////////////////////////
// Set things up...
//
// Load the (fixed address!) NTDLL so we can
// (a) Check for it being loaded in the target process
// (b) Get the addresses of the entry points we want to hook

bool TrapNtDebugger::initialise()
{
    BaseOfNtDll = LoadLibrary( "NTDLL" );
    if ( BaseOfNtDll == 0 )
    {
        std::cerr << "Unable to load NTDLL: " << displayError() << std::endl;
        return false;
    }

    // Open the config file
    if (configFile.empty())
    {
        char chExeName[ MAX_PATH + 1 ] = "";
        GetModuleFileName( 0, chExeName, sizeof ( chExeName ) );
        char *pDelim = strrchr( chExeName, '.' );
        configFile = std::string( chExeName, pDelim + 1 - chExeName ) + "cfg";
    }
    std::ifstream cfgFile( configFile.c_str() );
    if ( cfgFile )
    {
        if (! EntryPoint::readEntryPoints( cfgFile, entryPoints, typedefs, target ))
           return false;
    }
    else
    {
        std::cerr << "Unable to read configuration from " << configFile << std::endl;
        return false;
    }

    if (target.empty())
    {
        TargetDll = BaseOfNtDll;
    }
    else
    {
        TargetDll = LoadLibrary(target.c_str());
        if ( TargetDll == 0 )
        {
            std::cerr << "Unable to load " << target << ": " << displayError() << std::endl;
            return false;
        }
        populateOffsets();
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////
BOOL CALLBACK populateCallback(
   PSYMBOL_INFO pSymInfo,
   ULONG /*SymbolSize*/,
   PVOID UserContext     )
{
   if (pSymInfo->NameLen <= 3)
     return TRUE;
   char * ptr = pSymInfo->Name;
   size_t len = pSymInfo->NameLen;
   if (ptr[0] == '_')
   {
      ++ptr;
      --len;
   }
   if (ptr[0] == 'N' && ptr[1] == 't')
   {
      char const *end = (char const *)memchr(ptr, '@', len);
      if (end)
      {
         len = end - ptr;
      }
      std::string key(ptr, len);
      DWORD offset = (DWORD)(pSymInfo->Address - pSymInfo->ModBase);
      TrapNtDebugger::Offsets & offsets = *(TrapNtDebugger::Offsets*)UserContext;
      offsets[key] = offset;
   }

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// Populate the 'offsets' collection
void TrapNtDebugger::populateOffsets()
{
   or2::SymbolEngine eng(GetCurrentProcess());
   DWORD64 baseAddress((DWORD64)TargetDll);
   eng.LoadModule64(0, target.c_str(), 0, baseAddress, 0);
   DbgInit<IMAGEHLP_MODULE64> ModuleInfo;
   if ( !eng.GetModuleInfo64(baseAddress, &ModuleInfo) ||
        (ModuleInfo.SymType != SymPdb) )
   {
      std::cerr << "Warning: No PDB found for '" << target << "' - some entry points may be missing\n";
   }
   eng.EnumSymbols(baseAddress, 0, populateCallback, &offsets);
}

//////////////////////////////////////////////////////////////////////////
// Print common header to trace lines
void TrapNtDebugger::header( DWORD pid, DWORD tid )
{
   if ( bTimestamp || bDelta)
   {
      if ( bTimestamp )           os << now();
      if ( bTimestamp && bDelta ) os << " ";
      if ( bDelta )               os << delta();
      
      os << ": ";
   }

   if ( bPid || bTid )
   {
      os << "[";
      if ( bPid )         os << std::setw(4) << pid;
      if ( bPid && bTid ) os << '/';
      if ( bTid )         os << std::setw(4) << tid;

      os << "] ";
   }
}


//////////////////////////////////////////////////////////////////////////
// The heart of NtTrace: if this is one of our added breakpoint exceptions
// then trace the arguments and return code for the entry point.
void TrapNtDebugger::OnBreakpoint( HANDLE hProcess, HANDLE hThread, DWORD pid, DWORD tid, PVOID exceptionAddress)
{
  CONTEXT Context;
  Context.ContextFlags = CONTEXT_FULL;
  if ( ! GetThreadContext( hThread, &Context ) )
  {
    std::cerr << "Can't get thread context: " << displayError() << std::endl;
  }
  else
  {
    NTCALLS::const_iterator it = NtPreSave.find(exceptionAddress);
    if (it != NtPreSave.end())
    {
      it->second.entryPoint->doPreSave(hProcess, hThread, Context);
      if (bPreTrace)
      {
        header( pid, tid );

        it->second.entryPoint->trace( os, hProcess, hThread, Context, bNames, bStackTrace, true );
      }
      return;
    }
    it = NtCalls.find(exceptionAddress);
    if ( it == NtCalls.end() )
    {
      os << "Breakpoint at " << exceptionAddress << std::endl;
    }
    else
    {
#ifdef _M_IX86
      NTSTATUS rc = Context.Eax;
#elif _M_X64
      NTSTATUS rc = NTSTATUS(Context.Rax);
#endif
      if (bErrorsOnly && NT_SUCCESS(rc))
      {
        // don't trace
      }
      else if ( errorCodes.empty() || ( errorCodes.count( rc ) > 0 ) )
      {
        header( pid, tid );

        it->second.entryPoint->trace( os, hProcess, hThread, Context, bNames, bStackTrace, false );
      }

      if ( it->second.trapType == NtCall::trapReturn || it->second.trapType == NtCall::trapReturn0 )
      {
        // Fake a return 'n'
#ifdef _M_IX86
        DWORD eip = 0;
        ReadProcessMemory( hProcess, (LPVOID)( Context.Esp ), &eip, sizeof( eip ), 0 );

        Context.Eip = eip;
        Context.Esp += sizeof( eip ) + it->second.nArgs * sizeof( DWORD );
#elif _M_X64
        DWORD64 rip = 0;
        ReadProcessMemory( hProcess, (LPVOID)( Context.Rsp ), &rip, sizeof( rip ), 0 );

        Context.Rip = rip;
        Context.Rsp += sizeof( rip ) + it->second.nArgs * sizeof( DWORD );
#endif // _M_IX86
      }
      else if ( it->second.trapType == NtCall::trapJump )
      {
        // Fake a jump
#ifdef _M_IX86
        Context.Eip = it->second.jumpTarget;
#elif _M_X64
        Context.Rip = it->second.jumpTarget;
#endif // _M_IX86
      }
      Context.ContextFlags = CONTEXT_CONTROL;
      if ( ! SetThreadContext( hThread, &Context ) )
      {
        os << "Can't set thread context: " << displayError() << std::endl;
      }
    }
  }
}
 
//////////////////////////////////////////////////////////////////////////
void TrapNtDebugger::OnException( HANDLE hProcess, HANDLE hThread, DWORD pid, DWORD tid, EXCEPTION_DEBUG_INFO const & Exception, DWORD * pContinueFlag )
{
    if ( Exception.ExceptionRecord.ExceptionCode == STATUS_BREAKPOINT )
    {
        *pContinueFlag = DBG_CONTINUE;
        OnBreakpoint(hProcess, hThread, pid, tid, Exception.ExceptionRecord.ExceptionAddress);
    }
    else if ( bNoExcept )
    {
        // ignore...
    }
#ifdef _M_X64
    else if (Exception.ExceptionRecord.ExceptionCode == STATUS_WX86_BREAKPOINT)
    {
        os << "WOW64 initialised" << std::endl;
        *pContinueFlag = DBG_CONTINUE;
    }
#endif // _M_X64
    else if ( Exception.ExceptionRecord.ExceptionCode == EXCEPTION_ACCESS_VIOLATION )
    {
        // Only defined contents is for an access violation...
        header( pid, tid );
        os << "Access violation at " << Exception.ExceptionRecord.ExceptionAddress << ": "
           << ( Exception.ExceptionRecord.ExceptionInformation[0] ? "Write to " : "Read from " ) 
           << (PVOID)Exception.ExceptionRecord.ExceptionInformation[1]
           << " (" << ( Exception.dwFirstChance ? "first" : "last" ) << " chance)" << std::endl;
        if (bStackTrace) EntryPoint::stackTrace(os, hProcess, hThread);
    }
    else if ( Exception.ExceptionRecord.ExceptionCode == MSVC_EXCEPTION )
    {
        header( pid, tid );
        os << "C++ exception at " << Exception.ExceptionRecord.ExceptionAddress;
        if ( ( Exception.dwFirstChance ) &&
             ( Exception.ExceptionRecord.NumberParameters == 3 || Exception.ExceptionRecord.NumberParameters == 4 ) &&
             ( Exception.ExceptionRecord.ExceptionInformation[0] == MSVC_MAGIC_NUMBER1 ) )
        {
            if (Exception.ExceptionRecord.ExceptionInformation[1] == 0 && Exception.ExceptionRecord.ExceptionInformation[2] == 0)
            {
                os << " rethrow";
            }
            else
            {
                ULONG_PTR base = Exception.ExceptionRecord.NumberParameters == 3 ? 0 : static_cast<ULONG_PTR>(Exception.ExceptionRecord.ExceptionInformation[3]);
                showThrowType( os, hProcess, static_cast<ULONG_PTR>(Exception.ExceptionRecord.ExceptionInformation[2]), base );
            }
        }
        os << std::endl;
        if (bStackTrace) EntryPoint::stackTrace(os, hProcess, hThread);
    }
    else if (Exception.ExceptionRecord.ExceptionCode == CLR_EXCEPTION || 
             Exception.ExceptionRecord.ExceptionCode == CLR_EXCEPTION_V4)
    {
        os << "CLR exception, HR: " << (PVOID)(UINT_PTR)(HRESULT)Exception.ExceptionRecord.ExceptionInformation[0] << std::endl;
    }
    else if (Exception.ExceptionRecord.ExceptionCode == MSVC_NOTIFICATION)
    {
        if (Exception.ExceptionRecord.ExceptionInformation[0] == 0x1000)
        {
            os << now() << ": SetThreadName \"";
            showString( os, hProcess, (PVOID)Exception.ExceptionRecord.ExceptionInformation[1], FALSE, MAX_PATH );
            os << '"' << std::endl;
        }
        else
        {
            os << now() << ": MSVC Notification: " << (PVOID)Exception.ExceptionRecord.ExceptionInformation[0] << std::endl;
        }
    }
    else if (Exception.ExceptionRecord.ExceptionCode == CLR_NOTIFICATION)
    {
        os << now() << ": CLR Notification: " << (PVOID)Exception.ExceptionRecord.ExceptionInformation[0] << std::endl;
    }
    else
    {
        header( pid, tid );
        os << "Exception: " << std::hex << Exception.ExceptionRecord.ExceptionCode << std::dec
           << " at " << Exception.ExceptionRecord.ExceptionAddress
           << " (" << ( Exception.dwFirstChance ? "first" : "last" ) << " chance)" << std::endl;
        if (bStackTrace) EntryPoint::stackTrace(os, hProcess, hThread);
    }
}
 
//////////////////////////////////////////////////////////////////////////
void TrapNtDebugger::OnCreateThread( DWORD dwThreadId, CREATE_THREAD_DEBUG_INFO const & CreateThread )
{
    os << "Created thread: " << dwThreadId << " at " << 
        CreateThread.lpStartAddress << std::endl;
}

//////////////////////////////////////////////////////////////////////////
void TrapNtDebugger::OnCreateProcess( DWORD dwProcessId, DWORD /*dwThreadId*/, CREATE_PROCESS_DEBUG_INFO const & CreateProcessInfo )
{
    os << "Process " << dwProcessId << " starting at " << CreateProcessInfo.lpStartAddress << std::endl;
    processes[dwProcessId] = CreateProcessInfo.hProcess;

    if ( ! CreateProcessInfo.lpImageName || ! showName( os, CreateProcessInfo.hProcess,
                         CreateProcessInfo.lpImageName, CreateProcessInfo.fUnicode  ) )
    {
        showModuleNameEx( CreateProcessInfo.hProcess, CreateProcessInfo.lpBaseOfImage, CreateProcessInfo.hFile );
    }
    os << std::endl;
}

//////////////////////////////////////////////////////////////////////////
void TrapNtDebugger::OnExitThread( DWORD dwThreadId, EXIT_THREAD_DEBUG_INFO const & ExitThread )
{
    os << "Thread " << dwThreadId << " exit code: " << ExitThread.dwExitCode << std::endl;
}
 
//////////////////////////////////////////////////////////////////////////
void TrapNtDebugger::OnExitProcess( DWORD dwProcessId, EXIT_PROCESS_DEBUG_INFO const & ExitProcess )
{
    os << "Process " << dwProcessId << " exit code: " << ExitProcess.dwExitCode << std::endl;
    processes.erase(dwProcessId);
}
 
//////////////////////////////////////////////////////////////////////////
void TrapNtDebugger::OnLoadDll( HANDLE hProcess, LOAD_DLL_DEBUG_INFO const & LoadDll )
{
    os << "Loaded DLL at " << LoadDll.lpBaseOfDll << " ";
    if ( LoadDll.lpBaseOfDll == 0 )
    {
        os << "Null DLL";
    }
    else
    {
        if ( ! LoadDll.lpImageName || ! showName( os, hProcess, LoadDll.lpImageName, LoadDll.fUnicode ) )
        {
            showModuleNameEx( hProcess, LoadDll.lpBaseOfDll, LoadDll.hFile );
        }
    }
    os << std::endl;

    if ( LoadDll.lpBaseOfDll == TargetDll )
    {
        SetDllBreakpoints( hProcess );
        
    }
}
 
//////////////////////////////////////////////////////////////////////////
void TrapNtDebugger::OnUnloadDll( UNLOAD_DLL_DEBUG_INFO const & UnloadDll )
{
    os << "Unload of DLL at " << UnloadDll.lpBaseOfDll << std::endl; 
}

//////////////////////////////////////////////////////////////////////////
void TrapNtDebugger::OnOutputDebugString( HANDLE hProcess, OUTPUT_DEBUG_STRING_INFO const & DebugString )
{
    bool const newline = showString( os, hProcess, 
        DebugString.lpDebugStringData,
        DebugString.fUnicode,
        DebugString.nDebugStringLength );
    if (!newline && bNewline)
    {
        os << '\n';
    }
    os << std::flush;
}

//////////////////////////////////////////////////////////////////////////
// Get module name of debuggee when the debug interface didn't give it us
// Work around for the special case of the first DLL (NTDLL.DLL)
void TrapNtDebugger::showModuleNameEx( HANDLE hProcess, PVOID lpModuleBase, HANDLE hFile ) const
{
    if ( lpModuleBase == BaseOfNtDll )
        hProcess = GetCurrentProcess();

    char chFileName[ MAX_PATH + 1 ] = "";
    if ( ! GetModuleFileNameEx( hProcess, (HMODULE)lpModuleBase, chFileName, sizeof ( chFileName ) ) )
    {
        if (hFile)
        {
	    os << GetFileNameFromHandle(hFile);
        }
        else
        {
            std::cerr << "unknown module: " << lpModuleBase << ": " << displayError() << std::endl;
        }
    }
    else
    {
        os << chFileName;
    }
}

bool TrapNtDebugger::listCategories()
{
   bool result = false;
   if (categories.count("?"))
   {
      result = true;
	  std::set<std::string> allCategories;
      for ( EntryPointSet::iterator it = entryPoints.begin(); it != entryPoints.end(); ++it )
      {
         allCategories.insert(it->getCategory());
      }
	  
	  std::cout << "Valid categories:\n";
	  std::copy(allCategories.begin(), allCategories.end(), std::ostream_iterator<std::string>(std::cout,"\n"));
	  std::cout << std::endl;
   }
   return result;
}


//////////////////////////////////////////////////////////////////////////
// Set up the NT breakpoints loaded from the configuration file
void TrapNtDebugger::SetDllBreakpoints( HANDLE hProcess )
{
    std::set<std::string> unusedCategories( categories );
    std::set<std::string> unusedFilters( filters.begin(), filters.end() );

    unsigned int trapped(0);
    unsigned int total(0);

    for ( EntryPointSet::iterator it = entryPoints.begin(); it != entryPoints.end(); ++it )
    {
       bool bRequired( true );
       if (it->isDisabled())
       {
           bRequired = false;
       }
       if ( categories.size() != 0 )
       {
          if ( categories.find( it->getCategory() ) == categories.end() )
          {
             bRequired = false;
          }
          else
          {
             unusedCategories.erase( it->getCategory() );
          }
       }
       if ( bRequired && ( filters.size() != 0 ) )
       {
          bRequired = inverseFilter;
          for ( std::vector< std::string >::const_iterator filter = filters.begin(); filter != filters.end(); ++filter )
          {
             if ( it->getName().find( *filter ) != std::string::npos )
             {
                bRequired = !bRequired;
                unusedFilters.erase( *filter );
                break;
             }
          }
       }

       if ( bRequired )
       {
          EntryPoint &ep = const_cast<EntryPoint &>(*it); // set iterator returns const object :-(
          NtCall nt = ep.setNtTrap( hProcess, TargetDll, bPreTrace, offsets[ep.getName()] );
          if ( nt.entryPoint != 0 )
          {
              NtCalls[ ep.getAddress() ] = nt;
              if (ep.getPreSave())
              {
                NtPreSave[ ep.getPreSave() ] = nt;
              }
              ++trapped;
          }
          ++total;
       }
    }
    
    showUnused(unusedCategories, "category");
    showUnused(unusedFilters, "filter");
    if (trapped < total/2)
    {
       std::cerr << "Warning: Only " << trapped << " entry points active out of " << total << '\n';
    }

    if ( exportFile.length() != 0 )
    {
        std::ofstream exp( exportFile.c_str() );

        if (!target.empty())
        {
            exp << "//[" << target << "]\n";
        }

        // Print any typdefs, sorted by the underlying type
        std::multimap<std::string, std::string> sorted;
        for (EntryPoint::Typedefs::const_iterator it = typedefs.begin(); it != typedefs.end(); ++it)
        {
            sorted.insert(std::make_pair(it->second, it->first));
        }

        std::string lastType;
        for (std::multimap<std::string, std::string>::const_iterator it2 = sorted.begin(); it2 != sorted.end(); ++it2)
        {
            if (lastType != it2->first)
            {
                lastType = it2->first;
                exp << '\n';
            }
            exp << "typedef " << it2->first << " " << it2->second << ";\n";
        }
        if (!sorted.empty())
        {
            exp << '\n';
        }

        for ( EntryPointSet::const_iterator currEntry = entryPoints.begin(); currEntry != entryPoints.end(); ++currEntry )
        {
            currEntry->writeExport( exp );
            exp << std::endl;
        }
    }

    FlushInstructionCache( hProcess, 0, 0 );  
}

void TrapNtDebugger::showUnused( std::set<std::string> const & unused, std::string const & name )
{
   if ( ! unused.empty() )
    {
       std::cerr << "Warning: invalid " << name << " '" << *unused.begin() << "'" << std::endl;
    }
}

bool TrapNtDebugger::detach(DWORD processId, HANDLE hProcess)
{
   for ( NTCALLS::const_iterator it = NtCalls.begin(); it != NtCalls.end(); ++it )
   {
      NtCall const & ntCall = it->second;
      if (!ntCall.entryPoint->clearNtTrap(hProcess, ntCall))
      {
         std::cerr << "Cannot clear trap for " << ntCall.entryPoint->getName() << " in " << processId << '\n';
         return false;
      }
   }
   FlushInstructionCache(hProcess, 0, 0);

   return true;
}

bool TrapNtDebugger::detachAll()
{
   for (std::map<DWORD, HANDLE>::const_iterator it = processes.begin(); it != processes.end(); ++it)
   {
      if (it->second)
      {
         if (!detach(it->first, it->second))
         {
            return false;
         }
      }
   }
   std::cout << "Detached\n";

   // Break out of the debugging loop
   bActive = false;

   for (std::map<DWORD, HANDLE>::const_iterator it2 = processes.begin(); it2 != processes.end(); ++it2)
   {
      DebugBreakProcess(it2->second);
   }
   return true;
}

void TrapNtDebugger::setCtrlC()
{
    ctrlcTarget = this;
    SetConsoleCtrlHandler( TrapNtDebugger::CtrlHandler, TRUE );
}

//static
BOOL TrapNtDebugger::CtrlHandler(DWORD fdwCtrlType)
{
   BOOL ret(false);
   switch(fdwCtrlType)
   {
   // Handle the CTRL-C signal.
   case CTRL_C_EVENT:
      if (ctrlcTarget->detachAll())
      {
         ret = TRUE;
      }
      break;
   default:
      break;
   }
   return ret;
}

void setErrorCodes( std::string const & codeFilter )
{
   std::vector<std::string> codes;

   SimpleTokenizer( codeFilter, &codes, ',' );

   for ( std::vector<std::string>::const_iterator it = codes.begin(), past = codes.end();
      it != past; ++it )
   {
      NTSTATUS code(0);
      char scrap(0);
      if ( sscanf( it->c_str(), "%x%c", &code, &scrap ) != 1 )
      {
         throw std::runtime_error( "Unrecognised error code value '" + *it + "'" );
      }
      errorCodes.insert( code );
   }

}

//////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv )
{
    bool attach( false );
    std::string outputFile;
    std::string category;
    std::string filter;
    std::string codeFilter;
    bool bNoExcept( false );
    bool noDebugHeap(false);
    bool bNoNames(false);

    Options options( szRCSID );
    options.set( "a", &attach, "attach to existing process <cmd> rather than starting a fresh <cmd>" );
    options.set( "e", &bErrorsOnly, "Only log errors" );
    options.set( "config", &configFile, "Specify config file" );
    options.set( "errors", &codeFilter, "Comma delimited list of error codes to filter on" );
    options.set( "export", &exportFile, "Export symbols once loaded [for testing]" );
    options.set( "filter", &filter, "Comma delimited list of substrings to filter on (leading '-' to filter off)" );
    options.set( "category", &category, "Comma delimited list of categories to trace (eg File,Process,Registry, ? for list)" );
    options.set( "hd", &noDebugHeap, "Don't use debug heap" ); 
    options.set( "nonames", &bNoNames, "Don't name arguments" );
    options.set( "noexcept", &bNoExcept, "Don't process exceptions" );
    options.set( "out", &outputFile, "Output file" );
    options.set( "pre", &bPreTrace, "Trace pre-call as well as post-call" );
    options.set( "stack", &bStackTrace, "show stack trace" );
    options.set( "time", &bTimestamp, "show timestamp" );
    options.set( "delta", &bDelta, "show delta time" );
    options.set( "pid", &bPid, "show process ID" );
    options.set( "tid", &bTid, "show thread ID" );
    options.set( "nl", &bNewline, "force newline on OutputDebugString");

    options.setArgs( 1, -1, "[pid | cmd <args>]" );
    if ( ! options.process( argc, argv, "Provide trapping for calls to NT native API" ) )
    {
        return 1;
    }
    bNames = !bNoNames; // avoid double negatives

    Options::const_iterator it = options.begin();
    
    std::ofstream ofs;
    if ( outputFile.length() != 0 )
    {
       ofs.open( outputFile.c_str() );
       if ( ! ofs )
       {
          std::cerr << "Cannot open: " << outputFile << std::endl;
          return 1;
       }
    }

    if ( codeFilter.length() )
        setErrorCodes( codeFilter );

    TrapNtDebugger debugger( ( outputFile.length() != 0 ) ? (std::ostream&)ofs : std::cout );
    debugger.setNoException( bNoExcept );
    debugger.setFilter( filter );
    debugger.setCategory( category );

    // Load initialisation data
    if ( ! debugger.initialise() )
    {
        std::cerr << "Failed to initialise" << std::endl;
        return 1;
    }

    if (debugger.listCategories())
    {
        return 0;
    }

    int pid = 0;
    bool havePid( false );

    if ( attach )
    {
       std::vector< DWORD > pidList = FindProcesses( it->c_str() );

       if ( pidList.size() == 0 )
       {
          std::cerr << "Bad pid: " << *it << std::endl;
          return 1;
       }
       else if ( pidList.size() > 1 )
       {
          std::cerr << "Duplicate processes match: " << *it << std::endl;
          return 1;
       }
       pid = pidList[0];
       havePid = true;
    }
    else if ( readInt( *it, pid ) )
    {
       havePid = true;
    }
    
    if ( havePid )
    {
        // only required if we didn't start the process
        EnableDebugPriv();

        HANDLE hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, pid );
        if ( 0 == hProcess )
        {
            DWORD const err(GetLastError());
            if (err == ERROR_INVALID_PARAMETER)
            {
                std::cerr << "OpenProcess failed to find pid " << pid << std::endl;
            }
            else
            {
                std::cerr << "OpenProcess failed with " << displayError(err) << std::endl;
            }
            return 1;
        }

        BOOL rc = DebugActiveProcess( pid );
        if ( ! rc )
        {
            std::cerr << "DebugActiveProcess failed with " << displayError() << std::endl;
            return 1;
        }
    }
    else
    {
        if (noDebugHeap)
        {
            putenv( "_NO_DEBUG_HEAP=1");
        }

        PROCESS_INFORMATION ProcessInformation;
        int ret = CreateProcessHelper( options.begin(), options.end(), 
            DEBUG_PROCESS, // creation flags (DEBUG_ONLY_THIS_PROCESS would make the debugger's life easier...)
            &ProcessInformation );

        if ( ret != 0 )
        {
            std::cerr << "CreateProcess failed with " << displayError();
#ifdef _M_IX86
            if (GetLastError() == ERROR_NOT_SUPPORTED)
            {
                std::cerr << " - is the target process 64bit?";
            }
#endif // _M_IX86
            std::cerr << std::endl;
            return 1;
        }

        // Close unwanted handles
        if ( ! CloseHandle( ProcessInformation.hProcess ) )
        {
            std::cerr << "Unable to close process handle: " << displayError() << std::endl;
        }
        if ( ! CloseHandle( ProcessInformation.hThread ) )
        {
            std::cerr << "Unable to close thread handle: " << displayError() << std::endl;
        }
    }

    debugger.setCtrlC();

    DebugDriver().Loop( debugger );

    if (havePid && !debugger.Active())
    {
       // We've detached from all targets, so don't kill them on exit
       DebugSetProcessKillOnExit(false);
    }

    return 0;
}
