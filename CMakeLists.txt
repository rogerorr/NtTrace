cmake_minimum_required(VERSION 3.20)

project(NtTrace CXX)

set(CMAKE_CXX_STANDARD 23)

if (MSVC)
    add_compile_options(/W4 /WX)
else() # either clang or gcc
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()
if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
add_compile_options(-Wno-unused-const-variable -Wno-cast-function-type-mismatch -Wno-microsoft-cast -Wno-missing-field-initializers)
endif()

# Debugging library
add_library(debugging STATIC
src/DebugDriver.cpp
src/EntryPoint.cpp
src/Enumerations.cpp
src/GetFileNameFromHandle.cpp
src/GetModuleBase.cpp
src/NtTrace.cpp
src/ShowData.cpp
src/SymbolEngine.cpp)
target_include_directories(debugging PUBLIC include "$ENV{VSINSTALLDIR}/DIA SDK/include")

# Nt Trace
add_executable(${PROJECT_NAME} src/${PROJECT_NAME}.cpp src/${PROJECT_NAME}.rc)
set_source_files_properties(src/${PROJECT_NAME}.rc PROPERTIES INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC debugging)

# Memory Statistics
add_executable(MemoryStats src/MemoryStats.cpp)
target_link_libraries(MemoryStats PUBLIC debugging)

# Show Loader Snaps
add_executable(ShowLoaderSnaps src/ShowLoaderSnaps.cpp)
target_link_libraries(ShowLoaderSnaps PUBLIC debugging)

# Symbol Explorer
add_executable(SymExplorer src/SymExplorer.cpp)
target_link_libraries(SymExplorer PUBLIC debugging)
