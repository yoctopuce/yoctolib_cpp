ECHO OFF
if "%VCINSTALLDIR%"=="" call "%ProgramFiles%\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
if "%VCINSTALLDIR%"=="" call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Professional\Common7\Tools\VsDevCmd.bat"
if "%VCINSTALLDIR%"=="" call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Entreprise\Common7\Tools\VsDevCmd.bat"
if "%VCINSTALLDIR%"=="" call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat"

if "%VCINSTALLDIR%"=="" (
	if "%VCINSTALLDIR%"=="" call "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Professional\Common7\Tools\VsDevCmd.bat"
	if "%VCINSTALLDIR%"=="" call "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Entreprise\Common7\Tools\VsDevCmd.bat"
	if "%VCINSTALLDIR%"=="" call "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.bat"
	msbuild VisualStudioDllExample.sln /p:PlatformToolset=v141
) else (
	msbuild VisualStudioDllExample.sln /p:PlatformToolset=v142
)

del /Q /F debug
rmdir /q /s  debug
del /Q /F YoctopuceDll\debug
rmdir /q /s YoctopuceDll\debug
del /Q /F UseYoctopuceDll\debug
rmdir /q /s UseYoctopuceDll\debug
