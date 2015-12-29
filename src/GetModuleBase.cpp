/*
NAME
    GetModuleBase.cpp

DESCRIPTION
    Helper for stack walking.

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

#include "GetModuleBase.h"

#include <windows.h>
#pragma warning(push)
#pragma warning(disable: 4091) // 'typedef ': ignored on left of '' when no variable is declared
#include <dbghelp.h>
#pragma warning(pop)
#include <psapi.h> // GetModuleFileNameEx
#include <string>
#include <vector>

#pragma comment( lib, "psapi" )

static char const szRCSID[] = "$Id: GetModuleBase.cpp 1592 2015-12-29 20:33:46Z Roger $";

namespace
{
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
}

/////////////////////////////////////////////////////////////////////////////////////
/// GetModuleBase: try to get information about a module being loaded
///
/// This function is called by the stack walker to load symbols for a new address.
/// NOTES:
/// This function is needed because we are using FALSE for the last parameter of
/// SymInitialize(), which prevents ALL modules being automatically loaded and so
/// dramatically improves the speed of the stack walk
/// We check whether the address is valid first because some invalid addresses
/// cause access violations inside DbgHelp.dll
///
DWORD64 CALLBACK GetModuleBase( HANDLE hProcess, DWORD64 dwAddress )
{
    DWORD64 baseAddress = 0;

    MEMORY_BASIC_INFORMATION mbInfo;
    if ( ::VirtualQueryEx( hProcess, (PVOID)dwAddress, &mbInfo, sizeof( mbInfo ) ) &&
         ( ( mbInfo.State & MEM_FREE ) == 0 ) )
    {
        // It is already in the symbol engine?
        IMAGEHLP_MODULE64 stIHM = { sizeof ( IMAGEHLP_MODULE64 ) };

        if ( ::SymGetModuleInfo64( hProcess, dwAddress, &stIHM ) )
        {
            baseAddress = stIHM.BaseOfImage;
            //ATLTRACE("showGetModuleBase got addr from SymGetModuleInfo = %x\n", baseAddress);
        }
        else
        {
            baseAddress = (DWORD64)mbInfo.AllocationBase;
            HMODULE const hmod = (HMODULE)mbInfo.AllocationBase;

            char szFileName[ MAX_PATH ] = "";
            DWORD const dwNameLen = GetModuleFileNameWrapper( hProcess, hmod, szFileName, sizeof szFileName/sizeof szFileName[0] );

            if ( 0 != dwNameLen )
            {
                bool bPathSet( false );
                std::vector<char> searchpath( 1024 );
                if ( ::SymGetSearchPath( hProcess, &searchpath[0], 1024 ) )
                {
                    // symbol files often stored with binary image
                    char const * delim = strrchr( szFileName, '\\' );
                    if ( delim )
                    {
                        std::string fullpath( szFileName, delim - szFileName );
                        fullpath += ";";
                        fullpath += &searchpath[0];
                        ::SymSetSearchPath( hProcess, const_cast<char*>(fullpath.c_str()) ); // Some versions of DbgHelp.h not const-correct
                        bPathSet = true;
                    }
                }
                // We do not need to pass a file handle - trapNtCalls reveals that DbgHelp simply opens the file
                // if we don't provide a handle or duplicates the handle if we do.
                if (!::SymLoadModule64( hProcess, NULL, szFileName, NULL, baseAddress, 0 ))
                {
                    //ATLTRACE("SymLoadModule, failed for %s\n", szFileName);
                }
                fixSymSrv();
                if ( bPathSet )
                {
                    ::SymSetSearchPath( hProcess, const_cast<char*>(&searchpath[0]) );
                }
            }
            else
            {
                //ATLTRACE("Module not found at %X\n", baseAddress);
            }

            //ATLTRACE("GetModuleBase got addr from VirtualQueryEx = %x\n", baseAddress);
        }
    }

    return baseAddress;
}

/**
 * Get module file name, correcting for a couple of common issues.
 *
 * @param hProcess the process to query
 * @param hMod the module to query
 * @param szBuff the output filename buffer
 * @param bufLen the size of the output buffer
 * @returns the length of the string copied to the buffer, or zero on failure, 
 * in which case call GetLastError for the underlying error code.
 */
DWORD GetModuleFileNameWrapper( HANDLE hProcess, HMODULE hMod, char * szBuff, DWORD bufLen )
{
   DWORD ret = ::GetModuleFileNameEx( hProcess, hMod, szBuff, bufLen );
   if (ret == 0 && hMod == 0)
   {
      DWORD lastError = GetLastError();
      if (lastError == ERROR_PARTIAL_COPY || lastError == ERROR_INVALID_HANDLE)
      {
         // Use alternate API to get exe name in 64-bit windows.
         typedef BOOL (WINAPI *pfnQueryFullProcessImageName)(
           /*__in*/     HANDLE hProcess,
           /*__in*/     DWORD dwFlags,
           /*__out*/    LPCSTR lpExeName,
           /*__inout*/  PDWORD lpdwSize
         );

         static pfnQueryFullProcessImageName pQueryFullProcessImageName = (pfnQueryFullProcessImageName)GetProcAddress(GetModuleHandle("KERNEL32"), "QueryFullProcessImageNameA");

         if (pQueryFullProcessImageName && pQueryFullProcessImageName(hProcess, 0, szBuff, &bufLen))
         {
            ret = bufLen;
         }
         else
         {
             // Windows 2003 only has this one: which returns paths in device form
             typedef DWORD (WINAPI *pfnGetProcessImageFileName)(
                 IN   HANDLE hProcess,
                 OUT  LPTSTR lpImageFileName,
                 IN   DWORD nSize
             );

             static pfnGetProcessImageFileName pGetProcessImageFileName = (pfnGetProcessImageFileName)GetProcAddress(GetModuleHandle("psapi"), "GetProcessImageFileNameA");
             if (pGetProcessImageFileName)
             {
                 ret = pGetProcessImageFileName(hProcess, szBuff, bufLen);
             }
          }
      }
      if (ret == 0)
      {
         SetLastError(lastError);
      }
   }
   if ( ret != 0 )
   {
      if ( memcmp( szBuff, "\\??\\", 4 ) == 0 )
      {
         szBuff[1] = '\\'; // For some reason the wrong UNC header is returned
      }
   }
   return ret;
}
