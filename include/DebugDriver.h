#ifndef DEBUGDRIVER_H_
#define DEBUGDRIVER_H_

/**
@file
    Simple base class for running the debug loop.

    @author Roger Orr <rogero@howzatt.demon.co.uk>

    Copyright &copy; 2002,2003.
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

// $Id: DebugDriver.h 1405 2013-11-12 23:57:17Z Roger $

#ifndef _WINDOWS_
#include <windows.h>
#endif // _WINDOWS_

#include <map>

namespace or2
{

//////////////////////////////////////////////////////////////////////////
/**
 * Base interface class for writing a debugger.
 *
 * This class provides the callbacks for the DebugDriver class.
 */
class Debugger
{
public:
    // callbacks on events
    /** Exception occurred */
    virtual void OnException( HANDLE hProcess, HANDLE hThread, DWORD pid, DWORD tid, EXCEPTION_DEBUG_INFO const & DebugEvent, DWORD * pContinueExecution ) = 0;

    /** Callback on thread creation */
    virtual void OnCreateThread( DWORD dwThreadId, CREATE_THREAD_DEBUG_INFO const & CreateThread ) = 0;

    /** Callback on process creation */
    virtual void OnCreateProcess( DWORD dwProcessId, DWORD dwThreadId, CREATE_PROCESS_DEBUG_INFO const & CreateProcessInfo ) = 0;

    /** Callback on thread exit */
    virtual void OnExitThread( DWORD dwThreadId, EXIT_THREAD_DEBUG_INFO const & ExitThread ) = 0;

    /** Callback on process exit */
    virtual void OnExitProcess( DWORD dwProcessId, EXIT_PROCESS_DEBUG_INFO const & ExitProcess ) = 0;

    /** Callback on loading DLL */
    virtual void OnLoadDll( HANDLE hProcess, LOAD_DLL_DEBUG_INFO const & LoadDll ) = 0;

    /** Callback on unloading DLL */
    virtual void OnUnloadDll( UNLOAD_DLL_DEBUG_INFO const & UnloadDll ) = 0;

    /** Callback on outputting a debug string */
    virtual void OnOutputDebugString( HANDLE hProcess, OUTPUT_DEBUG_STRING_INFO const & DebugString ) = 0;

    /** Is the debugger still active? */
    virtual bool Active() { return true; }

    /** Virtual dtor for safe inheritance */
    virtual ~Debugger() {}
};

//////////////////////////////////////////////////////////////////////////
/**
 * Adapter for stripped-down functionality of a Debugger
 */
class DebuggerAdapter : public Debugger
{
#pragma warning( push )
#pragma warning( disable: 4100 ) // unreferenced formal parameter

public:
    // callbacks on events
    /** Exception occurred */
    virtual void OnException( HANDLE hProcess, HANDLE hThread, DWORD pid, DWORD tid, EXCEPTION_DEBUG_INFO const & DebugEvent, DWORD * pContinueExecution ) {}

    /** Callback on thread creation */
    virtual void OnCreateThread( DWORD dwThreadId, CREATE_THREAD_DEBUG_INFO const & CreateThread ) {}

    /** Callback on process creation */
    virtual void OnCreateProcess( DWORD dwProcessId, DWORD dwThreadId, CREATE_PROCESS_DEBUG_INFO const & CreateProcessInfo ) {}

    /** Callback on thread exit */
    virtual void OnExitThread( DWORD dwThreadId, EXIT_THREAD_DEBUG_INFO const & ExitThread ) {}

    /** Callback on process exit */
    virtual void OnExitProcess( DWORD dwProcessId, EXIT_PROCESS_DEBUG_INFO const & ExitProcess ) {}

    /** Callback on loading DLL */
    virtual void OnLoadDll( HANDLE hProcess, LOAD_DLL_DEBUG_INFO const & LoadDll ) {}

    /** Callback on unloading DLL */
    virtual void OnUnloadDll( UNLOAD_DLL_DEBUG_INFO const & UnloadDll ) {}

    /** Callback on outputting a debug string */
    virtual void OnOutputDebugString( HANDLE hProcess, OUTPUT_DEBUG_STRING_INFO const & DebugString ) {}

#pragma warning( pop )
};

//////////////////////////////////////////////////////////////////////////
/**
 * Simple class for running the debug loop.
 *
 * The DebugDriver class runs the debug loop until the debugee has finished.
 * Events are signified by callbacks to the supplied Debugger object
 */
class DebugDriver
{
public:
    /** Runs till debugee finishes, calling back 'debugger' for each event */
    void Loop( Debugger & debugger );

private:
    //////////////////////////////////////////////////////////////////////////
    // Data structure used for handling thread/process id -> handle mapping
    typedef std::map< DWORD, HANDLE > ThreadMap;
    struct ProcessEntry
    {
        ProcessEntry() : attached( false ) {}
        bool attached;
    	HANDLE hProcess;
    	ThreadMap threadMap;
    };
    typedef std::map< DWORD, ProcessEntry > ProcessMap;

private:
    ProcessMap processMap;
};

} // namespace

#endif // DEBUGDRIVER_H_