@echo off
setlocal enabledelayedexpansion
set /a errorno=1
for /F "delims=#" %%E in ('"prompt #$E# & for %%E in (1) do rem"') do set "_ESC=%%E"

set "vswhere=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%vswhere%" (
  echo Failed to find "vswhere.exe".  Please install the latest version of Visual Studio.
  goto :ERROR
)

set "vsdir="
for /f "usebackq tokens=*" %%i in (
  `"%vswhere%" -latest ^
               -products * ^
               -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 ^
               -property installationPath`
) do (
  set "vsdir=%%i"
)
if "%vsdir%" == "" (
  echo Failed to find Visual C++.  Please install the latest version of Visual C++.
  goto :ERROR
)

call "%vsdir%\VC\Auxiliary\Build\vcvarsall.bat" %* || goto :ERROR

nmake /f NtTrace.mak || goto :ERROR

echo %_ESC%[2K %~n0 : Status =%_ESC%[92m OK %_ESC%[0m
set /a errorno=0
goto :END

:ERROR
echo %_ESC%[2K %~n0 : Status =%_ESC%[92m ERROR %_ESC%[0m

:END
exit /B %errorno%
