@ECHO OFF

echo "%VCINSTALLDIR%"

if "%~1"=="win64" (goto use_win64)
if "%~1"=="release_win64" (goto use_win64)
if "%~1"=="win32" (goto use_win32)
if "%~1"=="release_win32" (goto use_win32)

echo Leagacy build command. Change your script to specify win32 or win64
if NOT "%VCToolsInstallDir%"=="" goto compile
if "%VCINSTALLDIR%"=="" call "%VS150COMNTOOLS%vsvars32.bat"
if "%VCINSTALLDIR%"=="" call "%VS140COMNTOOLS%vsvars32.bat"
if "%VCINSTALLDIR%"=="" call "%VS100COMNTOOLS%vsvars32.bat"
if "%VCINSTALLDIR%"=="" call "%VS110COMNTOOLS%vsvars32.bat"
if "%VCINSTALLDIR%"=="" call "%VS120COMNTOOLS%vsvars32.bat"

if NOT "%VCINSTALLDIR%"=="" goto compile

:use_win32
echo compile using 32 bit environement varialbles
SET "VCINSTALLDIR="
if "%VCINSTALLDIR%"=="" call "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Professional\Common7\Tools\VsDevCmd.bat"
if "%VCINSTALLDIR%"=="" call "%ProgramFiles%\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars32.bat"
if "%VCINSTALLDIR%"=="" call "%ProgramFiles%\Microsoft Visual Studio\2022\Entreprise\VC\Auxiliary\Build\vcvars32.bat"
if "%VCINSTALLDIR%"=="" call "%ProgramFiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat"
if "%VCINSTALLDIR%"=="" call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Professional\Common7\Tools\VsDevCmd.bat"
if "%VCINSTALLDIR%"=="" call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Entreprise\Common7\Tools\VsDevCmd.bat"
if "%VCINSTALLDIR%"=="" call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat"
if "%VCINSTALLDIR%"=="" call "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Entreprise\Common7\Tools\VsDevCmd.bat"
if "%VCINSTALLDIR%"=="" call "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.bat"

goto compile

:use_win64
echo compile using 64 bit environement varialbles
SET "VCINSTALLDIR="
if "%VCINSTALLDIR%"=="" call "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars64.bat"
if "%VCINSTALLDIR%"=="" call "%ProgramFiles%\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
if "%VCINSTALLDIR%"=="" call "%ProgramFiles%\Microsoft Visual Studio\2022\Entreprise\VC\Auxiliary\Build\vcvars64.bat"
if "%VCINSTALLDIR%"=="" call "%ProgramFiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
if "%VCINSTALLDIR%"=="" call "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Entreprise\VC\Auxiliary\Build\vcvars64.bat"
if "%VCINSTALLDIR%"=="" call "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
if "%VCINSTALLDIR%"=="" call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars64.bat"
if "%VCINSTALLDIR%"=="" call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Entreprise\VC\Auxiliary\Build\vcvars64.bat"
if "%VCINSTALLDIR%"=="" call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"



goto compile
:compile
echo "%VCINSTALLDIR%"
@nmake /nologo %1
