# $Id: NtTrace.mak 1609 2016-02-16 21:31:39Z Roger $

#
# This makefile requires Microsoft Visual Studio 2005 and above,
# for dbghelp.h and the support for manifest files
#

# COPYRIGHT
#     Copyright (C) 2007,2015 by Roger Orr <rogero@howzatt.demon.co.uk>
# 
#     This software is distributed in the hope that it will be useful, but
#     without WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# 
#     Permission is granted to anyone to make or distribute verbatim
#     copies of this software provided that the copyright notice and
#     this permission notice are preserved, and that the distributor
#     grants the recipient permission for further distribution as permitted
#     by this notice.
# 
#     Comments and suggestions are always welcome.
#     Please report bugs to rogero@howzatt.demon.co.uk.

all : build NtTrace.exe

build :
	mkdir build

clean :
	@-del NtTrace.exe NtTrace.exe.manifest NtTrace.res *.pdb
	@-rd /q /s build

CCFLAGS = /nologo /MD /W3 /WX /Zi /Iinclude /D_CRT_SECURE_NO_WARNINGS
LINKFLAGS = /link /opt:ref,icf

{src}.cpp{build}.obj::
	cl $(CCFLAGS) /Fobuild\ /c /EHsc /I. $<

.rc.res:
	rc -r /Iinclude $(*B)

NtTrace.exe : build\$(*B).obj $(*B).res 
	cl $(CCFLAGS) /Fe$@ $** $(LINKFLAGS)
	if exist $(@).manifest mt.exe -nologo -manifest $(@).manifest -outputresource:$@

# Dependencies

build\NtTrace.obj : "include\DebugPriv.h" \
	"include\AdjustPriv.h" \
	"include\displayError.h" \
	"include\displayError.inl" \
	"include\Options.h" \
	"include\Options.inl" \
	"include\ProcessHelper.h" \
	"include\NtDllStruct.h" \
	"include\SimpleTokenizer.h" \
	"include\MsvcExceptions.h" \
	include\DebugDriver.h \
	include\EntryPoint.h \
	include\ShowData.h \
	include\TrapNtOpcodes.h

NtTrace.res: $(*B).rc "version.rc"

NtTrace.exe : build\DebugDriver.obj build\EntryPoint.obj build\Enumerations.obj build\ShowData.obj \
	build\GetFileNameFromHandle.obj build\GetModuleBase.obj build\SymbolEngine.obj

build\DebugDriver.obj : \
	"include\displayError.h" \
	"include\displayError.inl" \
	"include\DebugDriver.h"

build\EntryPoint.obj : \
	"include\displayError.h" \
	"include\displayError.inl" \
	"include\DbgHelper.h" \
	"include\DbgHelper.inl" \
	"include\SymbolEngine.h" \
	"include\TrapNtOpcodes.h" \
	"include\ShowData.h"

build\ShowData.obj: \
	"include\Enumerations.h" \
	"include\NtDllStruct.h" \
	"include\MsvcExceptions.h" \
	"include\ReadPartialMemory.h" \
	"include\ShowData.h"

build\GetModuleBase.obj: include\GetModuleBase.h

build\SymbolEngine.obj: \
	"include/SymbolEngine.h" \
	"include/DbgHelper.h" \
	"include/DbgHelper.inl" \
	"include/StrFromWchar.h" \
	"include/MSvcExceptions.h" \
	"include/BasicType.h"
