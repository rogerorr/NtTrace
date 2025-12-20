# $Id: NtTrace.mak 2980 2025-12-20 15:58:41Z roger $

#
# This makefile requires Microsoft Visual Studio 2010 and above,
#

# AUTHOR
#   Roger Orr mailto:rogero@howzatt.co.uk
#   Bugs, comments, and suggestions are always welcome.
#
# COPYRIGHT
#   Copyright (C) 2007,2021 under the MIT license:
# 
#   "Permission is hereby granted, free of charge, to any person obtaining a
#   copy of this software and associated documentation files (the "Software"),
#   to deal in the Software without restriction, including without limitation
#   the rights to use, copy, modify, merge, publish, distribute, sublicense,
#   and/or sell copies of the Software, and to permit persons to whom the
#   Software is furnished to do so, subject to the following conditions:
#
#   The above copyright notice and this permission notice shall be included in
#   all copies or substantial portions of the Software.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
#   IN THE SOFTWARE."

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

CCFLAGS = /nologo /MD /W3 /WX /Zi /Iinclude /I "$(VSINSTALLDIR)\DIA SDK\include" /permissive-
LINKFLAGS = /link /opt:ref,icf

{src}.cpp{$(BUILD)}.obj::
	cl $(CCFLAGS) /Fo$(BUILD)\ /c /EHsc /I. $<

{src}.rc{$(BUILD)}.res:
	rc /nologo -r /fo$@ -r src/$(*B)

NtTrace.exe : $(BUILD)\$(*B).obj $(BUILD)\$(*B).res 
	cl $(CCFLAGS) /Fe$@ $** $(LINKFLAGS)

MemoryStats.exe : $(BUILD)\$(*B).obj $(BUILD)\$(*B).res 
	cl $(CCFLAGS) /Fe$@ $** $(LINKFLAGS)

ShowLoaderSnaps.exe : $(BUILD)\$(*B).obj $(BUILD)\$(*B).res 
	cl $(CCFLAGS) /Fe$@ $** $(LINKFLAGS)

SymExplorer.exe : $(BUILD)\$(*B).obj $(BUILD)\$(*B).res 
	cl $(CCFLAGS) /Fe$@ $** $(LINKFLAGS)

# Dependencies

$(BUILD)\NtTrace.obj : \
	"include/AdjustPriv.h" \
	"include/DebugPriv.h" \
	"include/DisplayError.h" \
	"include/DisplayError.inl" \
	"include/MsvcExceptions.h" \
	"include/NtDllStruct.h" \
	"include/Options.h" \
	"include/Options.inl" \
	"include/ProcessHelper.h" \
	"include/ProcessInfo.h" \
	"include/SimpleTokenizer.h" \
	"include/DebugDriver.h" \
	"include/EntryPoint.h" \
	"include/GetFileNameFromHandle.h" \
	"include/GetModuleBase.h" \
	"include/ShowData.h" \
	"include/TrapNtOpcodes.h"

MemoryStats.res: $(*B).rc "version.rc"

MemoryStats.exe : $(BUILD)\DebugDriver.obj $(BUILD)\ShowData.obj

NtTrace.res: $(*B).rc "version.rc"

NtTrace.exe : $(BUILD)\DebugDriver.obj $(BUILD)\EntryPoint.obj $(BUILD)\Enumerations.obj $(BUILD)\ShowData.obj \
	$(BUILD)\GetFileNameFromHandle.obj $(BUILD)\GetModuleBase.obj $(BUILD)\SymbolEngine.obj

ShowLoaderSnaps.res: $(*B).rc "version.rc"

ShowLoaderSnaps.exe : $(BUILD)\DebugDriver.obj $(BUILD)\GetModuleBase.obj

SymExplorer.res: $(*B).rc "version.rc"

SymExplorer.exe : $(BUILD)\GetModuleBase.obj $(BUILD)\GetFileNameFromHandle.obj $(BUILD)\SymbolEngine.obj

$(BUILD)\DebugDriver.obj : \
	"include/DisplayError.h" \
	"include/DisplayError.inl" \
	"include/DebugDriver.h"

$(BUILD)\EntryPoint.obj : \
	"include/DisplayError.h" \
	"include/DisplayError.inl" \
	"include/DbgHelper.h" \
	"include/DbgHelper.inl" \
	"include/NtDllStruct.h" \
	"include/ProcessInfo.h" \
	"include/SymbolEngine.h" \
	"include/TrapNtOpcodes.h" \
	"include/ShowData.h"

$(BUILD)\MemoryStats.obj: \
	"include/DisplayError.h" \
	"include/DisplayError.inl" \
	"include/Options.h" \
	"include/Options.inl" \
	"include/ProcessHelper.h" \
	"include/ReadInt.h" \
	"include/DebugDriver.h" \
	"include/ShowData.h"

$(BUILD)\ShowData.obj: \
	"include/MsvcExceptions.h" \
	"include/NtDllStruct.h" \
	"include/ProcessInfo.h" \
	"include/ReadPartialMemory.h" \
	"include/ShowData.h"

$(BUILD)\ShowLoaderSnaps.obj: \
	"include/DisplayError.h" \
	"include/DisplayError.inl" \
	"include/Options.h" \
	"include/Options.inl" \
	"include/ProcessHelper.h" \
	"include/ReadInt.h" \
	"include/DebugDriver.h" \
	"include/GetModuleBase.h"

$(BUILD)\GetModuleBase.obj: \
	"include/GetModuleBase.h"

$(BUILD)\SymbolEngine.obj: \
	"include/SymbolEngine.h" \
	"include/DbgHelper.h" \
	"include/DbgHelper.inl" \
	"include/MSvcExceptions.h" \
	"include/ReadPartialMemory.h" \
	"include/StrFromWchar.h"

$(BUILD)\SymExplorer.obj: \
	"include/BasicType.h" \
	"include/DbgHelper.h" \
	"include/DbgHelper.inl" \
	"include/GetModuleBase.h" \
	"include/NtDllStruct.h" \
	"include/Options.h" \
	"include/Options.inl" \
	"include/StrFromWchar.h" \
	"include/StreamGUID.h" \
	"include/SymbolEngine.h"
