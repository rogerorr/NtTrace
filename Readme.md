# NtTrace readme file

See also http://rogerorr.github.io/NtTrace/

# Contents

   * Readme.md	- this file
   * NtTrace.cfg	- default configuration file for NtTrace
   * NtTrace.ico	- icon file
   * NtTrace.mak	- make file for Microsoft Visual Studio command prompt
   * NtTrace.rc	- resource file
   * src		- source files
   * include		- include files
   * version.rc	- version resource
   * Gdi32Trace.cfg - configuration file for GDI native calls
   * User32Trace.cfg - configuration file for USER32 native calls

# Build instructions

You will need either or both of the 32-bit and 64-bit version of NtTrace, whichever matches your target application(s)
From a windows command prompt:
- cd to this top-level directory
- Set up the target VC environment
-- Eg "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
-- or "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
- run `nmake /f NtTrace.mak`

# Running NtTrace

NtTrace is designed to run from the command line.

It can be used to execute a program, with arguments if needed.
For example:
<br>
`NtTrace -filter File cmd`

or to attach to an existing process by PID or, using the -a option, by name.
For example:
`NtTrace 1234`
or
`NtTrace -a notepad`

Run `NtTrace -h` for a complete list of options.

## Gdi and User calls

By default NtTrace targets the Windows Native API exposed through NtDll.dll

Configuration files are also supplied for many of the entry points that support the
Windows graphics interface: 

Gdi32Trace.cfg for the Graphics Device Interface

User32Trace.cfg for the User Interface

You can select one of these, rather than the default NtTrace.cfg by using the `-config` command line argument.

## Note on DbgHelp.dll

Windows ships with DbgHelp.dll in the system32 directory.
However, newer versions of this DLL are sometimes available from Microsoft in their SDKs.

The version of the DLL installed with the OS tends to
take precedence over a newer version that may be downloaded.

If you find you require a newer version than the one installed in the system directory you
can simply copy of the latest `DbgHelp.dll` to the same directory that `NtTrace.exe` is in.
(You will also need to copy `SymSrv.dll` and `SrcSrv.dll`)

## How it works

### Technology

NtTrace works by using the Windows debug interface to place breakpoints in NtDll around the native Windows calls into the kernel.
Each time a breakpoint is hit NtTrace reads the arguments pased to/values returned by the associated call.

It works on user level and only affects the specific process (and its children). No attempt is made to go into the kernel.
This was a deliberate design decision as it means the tool can be used without requiring adminstrative rights
or permission to install device drivers.
However this also means the tool will not catch:
 * kernel calls made by device drivers on behalf of an application
 * any direct use of the int 2e, syscall, or sysenter mechanism

NtTrace also traces into child processes of the initial process started, the -pid option will add the process Id to each output line.

It also deals with multi-threaded applications (and the -tid option will help identify which thread is making each call).
However note that, since the Windows debug interface is event based, if multiple threads all make calls simultaneously
then the tracing will be serialised.

By default the calls in NtDll are traced; the configuration files for Gdi32 and User32 select a different target DLL.
NtTrace does not currently support simultaneously tracing different types of system calls.

### Configuration

The configuration for the native calls is held in NtTrace.cfg and this file is parsed when the program starts up.
Note that different versions of Windows support different sets of calls.

Some of the Native functions are officially documented by Microsoft but many are undocumented.
The complete list was arrived at by a combination of detective work on the functions and from web sites, such as ReactOS.

Each function is assigned to a category, and filtering can be done by function name (partial match) and function category.

By default NtTrace traces on return from the system call, but the `-pre` option can be used to trace before the call as well.
This can be useful if, for example, the same structure is used for a request and a response buffer.

--
$Id: Readme.md 1817 2019-03-18 22:48:56Z Roger $
