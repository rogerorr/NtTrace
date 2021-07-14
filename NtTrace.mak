# $Id: NtTrace.mak 1971 2021-07-08 21:26:10Z roger $

#
# This makefile requires Microsoft Visual Studio 2010 and above,
#

# COPYRIGHT
#     Copyright (C) 2007,2021 by Roger Orr <rogero@howzatt.co.uk>
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
#     Please report bugs to rogero@howzatt.co.uk.

!if ("$(PLATFORM)" == "X64" || "$(PLATFORM)" == "x64")
BUILD=build64
!else
BUILD=build32
!endif

all : $(BUILD) NtTrace.exe

$(BUILD) :
	mkdir $(BUILD)

clean :
	@-del NtTrace.exe NtTrace.res *.pdb
	@-rd /q /s $(BUILD)

CCFLAGS = /nologo /MD /W3 /WX /Zi /Iinclude /D_CRT_SECURE_NO_WARNINGS
LINKFLAGS = /link /opt:ref,icf

{src}.cpp{$(BUILD)}.obj::
	cl $(CCFLAGS) /Fo$(BUILD)\ /c /EHsc /I. $<

.rc.res:
	rc -r /Iinclude $(*B)

NtTrace.exe : $(BUILD)\$(*B).obj $(*B).res 
	cl $(CCFLAGS) /Fe$@ $** $(LINKFLAGS)

# Dependencies

$(BUILD)\NtTrace.obj : "include\DebugPriv.h" \
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

NtTrace.exe : $(BUILD)\DebugDriver.obj $(BUILD)\EntryPoint.obj $(BUILD)\Enumerations.obj $(BUILD)\ShowData.obj \
	$(BUILD)\GetFileNameFromHandle.obj $(BUILD)\GetModuleBase.obj $(BUILD)\SymbolEngine.obj

$(BUILD)\DebugDriver.obj : \
	"include\displayError.h" \
	"include\displayError.inl" \
	"include\DebugDriver.h"

$(BUILD)\EntryPoint.obj : \
	"include\displayError.h" \
	"include\displayError.inl" \
	"include\DbgHelper.h" \
	"include\DbgHelper.inl" \
	"include\SymbolEngine.h" \
	"include\TrapNtOpcodes.h" \
	"include\ShowData.h"

$(BUILD)\ShowData.obj: \
	"include\Enumerations.h" \
	"include\NtDllStruct.h" \
	"include\MsvcExceptions.h" \
	"include\ReadPartialMemory.h" \
	"include\ShowData.h"

$(BUILD)\GetModuleBase.obj: include\GetModuleBase.h

$(BUILD)\SymbolEngine.obj: \
	"include/SymbolEngine.h" \
	"include/DbgHelper.h" \
	"include/DbgHelper.inl" \
	"include/StrFromWchar.h" \
	"include/MSvcExceptions.h" \
	"include/BasicType.h"
