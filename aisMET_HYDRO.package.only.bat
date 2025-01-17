call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat"
@echo off

if exist build (rmdir /s /q build)
mkdir build && cd build

set SCRIPTDIR=D:\Documents\Github\aisMET_HYDRO_pi
set CONFIGURATION=RelWithDebInfo

set "wxWidgets_ROOT_DIR=D:\Documents\Github\aisMET_HYDRO_pi\buildwin\..\cache\wxWidgets-3.2.2.1"
set "wxWidgets_LIB_DIR=D:\Documents\Github\aisMET_HYDRO_pi\buildwin\..\cache\wxWidgets-3.2.2.1\lib\vc14x_dll"


echo USING wxWidgets_LIB_DIR: %wxWidgets_LIB_DIR%
echo USING wxWidgets_ROOT_DIR: %wxWidgets_ROOT_DIR%



cmake -A Win32 -G "Visual Studio 17 2022" ^
    -DCMAKE_GENERATOR_PLATFORM=Win32 ^
    -DCMAKE_BUILD_TYPE=%CONFIGURATION% ^
    -DwxWidgets_LIB_DIR=%wxWidgets_LIB_DIR% ^
    -DwxWidgets_ROOT_DIR=%wxWidgets_ROOT_DIR% ^
    -DOCPN_TARGET_TUPLE=msvc-wx32;10;x86_64 ^
    ..
cmake --build . --target tarball --config %CONFIGURATION%

cmd /k
