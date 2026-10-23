@echo off
setlocal

rem ---- Locate Visual Studio (its Installer ships vswhere too) ----
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" goto :novs

for /f "usebackq tokens=*" %%i in (
    `"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`
) do set "VSDIR=%%i"
if not defined VSDIR goto :novs

rem ---- vcpkg: respect someone who set VCPKG_ROOT; else use VS's bundled one ----
if not defined VCPKG_ROOT set "VCPKG_ROOT=%VSDIR%\VC\vcpkg"
set "VCPKG_ROOT=%VCPKG_ROOT:\=/%" 
if not exist "%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" goto :novcpkg

rem ---- Load VS's dev environment: this puts its bundled cmake on PATH ----
call "%VSDIR%\Common7\Tools\VsDevCmd.bat" -no_logo -arch=x64
if errorlevel 1 exit /b 1

cmake --preset release       || exit /b 1
cmake --build --preset release || exit /b 1
build\Release\TransformPlayground.exe
exit /b 0

:novs
echo Visual Studio 2017 or newer with the "Desktop development with C++" workload is required.
exit /b 1

:novcpkg
echo vcpkg not found at "%VCPKG_ROOT%".
echo Set VCPKG_ROOT yourself, or install VS with the C++ workload so vcpkg is bundled.
exit /b 1