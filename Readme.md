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

For Visual Studio versions since VC6:
- start a "Visual Studio command prompt",
- cd to this directory
- run `nmake /f NtTrace.mak`

## Visual Studio 6

The platform SDK shipped with Visual Studio 6 did not include the Psapi and DbgHelp headers
and libraries used by the program.

You need to download and install a newer version of the platform SDK, and ensure the include
path has the platform SDK include directory *before* the VC6 ones.

## 64bit builds

The 64bit version of NtTrace builds in the amd64 environment.

This can be selected using, for example:
"C:\Program Files\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" amd64

## Note on DbgHelp.dll

Windows ships with DbgHelp.dll in the system32 directory.
However, newer versions of this DLL are packaged with "Debugging Tools for Windows"
(See http://www.microsoft.com/whdc/DevTools/Debugging/default.mspx)

The problem with this mechanism is that the version installed with the OS tends to
take precendence over a newer version that may be downloaded.

If you find you require a newer version than the one installed in the system directory you
can simply copy of the latest `DbgHelp.dll` to the same directory that `NtTrace.exe` is in.
(You will also need to copy `SymSrv.dll` and `SrcSrv.dll`)

# Running NtTrace

NtTrace is designed to run from the command line.

It can be used to execute a program, with arguments if needed.
For example:
	NtTrace -filter File cmd

or to attach to an existing process by PID or, using the -a option, by name.
For example:
	NtTrace 1234
or
	NtTrace -a notepad

Run `NtTrace -h` for a complete list of options.

## How it works

1. Technology

NtTrace works by using the Windows debug interface to place breakpoints in NtDll around the native Windows calls into the kernel.
Each time a breakpoint is hit NtTrace reads the arguments pased to/values returned by the associated call.
Note: it does not attempt to trap the native graphics calls that are made to Win32k.sys

It works on user level and only affects the specific process (and its children). No attempt is made to go into the kernel.
This was a deliberate design decision as it means the tool can be used without requiring adminstrative rights
or permission to install device drivers.
However this also means the tool will not catch:
 * kernel calls made by device drivers on behalf of an application
 * any direct use of the int 2e or syscall mechanism

NtTrace also traces into child processes of the initial process started, the -pid option will add the process Id to each output line.

It also deals with multi-threaded applications (and the -tid option will help identify which thread is making each call).
However note that, since the Windows debug interface is event based, if multiple threads all make calls simultaneously
then the tracing will be serialised.

2. Configuration

The configuration for the native calls is held in NtTrace.cfg and this file is parsed when the program starts up.
Note that different versions of Windows support different sets of calls.

Some of the Native functions are officially documented by Microsoft but many are undocumented.
The complete list was arrived at by a combination of detective work on the functions and from web sites, such as ReactOS.

Each function is assigned to a category, and filtering can be done by function name (partial match) and function category.

By default NtTrace traces on return from the system call, but the `-pre` option can be used to trace before the call as well.
This can be useful if, for example, the same structure is used for a request and a response buffer.

--
$Id: Readme.md 1606 2016-02-16 21:08:07Z Roger $
