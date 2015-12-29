#ifndef GetFileNameFromHandle_H_
#define GetFileNameFromHandle_H_

#include <windows.h>
#include <string>

std::string GetFileNameFromHandle(HANDLE hFile);

#endif // GetFileNameFromHandle_H_