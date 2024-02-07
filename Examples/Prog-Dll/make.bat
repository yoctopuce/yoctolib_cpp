if "%VCINSTALLDIR%"=="" call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Professional\Common7\Tools\VsDevCmd.bat"
if "%VCINSTALLDIR%"=="" call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Entreprise\Common7\Tools\VsDevCmd.bat"
if "%VCINSTALLDIR%"=="" call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat"

if "%VCINSTALLDIR%"=="" (
	if "%VCINSTALLDIR%"=="" call "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Professional\Common7\Tools\VsDevCmd.bat"
	if "%VCINSTALLDIR%"=="" call "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Entreprise\Common7\Tools\VsDevCmd.bat"
	if "%VCINSTALLDIR%"=="" call "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.bat"
	msbuild VisualStudioDllExample\VisualStudioDllExample.sln /p:PlatformToolset=v141
) else (
	msbuild VisualStudioDllExample\VisualStudioDllExample.sln /p:PlatformToolset=v142
)

del /Q /F VisualStudioDllExample\debug
rmdir /q /s  VisualStudioDllExample\debug
del /Q /F VisualStudioDllExample\YoctopuceDll\debug
rmdir /q /s VisualStudioDllExample\YoctopuceDll\debug
del /Q /F VisualStudioDllExample\UseYoctopuceDll\debug
rmdir /q /s VisualStudioDllExample\UseYoctopuceDll\debug
