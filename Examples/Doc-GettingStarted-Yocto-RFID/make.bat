ECHO OFF
if "%VCINSTALLDIR%"=="" call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Professional\Common7\Tools\VsDevCmd.bat"
if "%VCINSTALLDIR%"=="" call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Entreprise\Common7\Tools\VsDevCmd.bat"
if "%VCINSTALLDIR%"=="" call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat"

if "%VCINSTALLDIR%"=="" (
	if "%VCINSTALLDIR%"=="" call "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Professional\Common7\Tools\VsDevCmd.bat"
	if "%VCINSTALLDIR%"=="" call "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Entreprise\Common7\Tools\VsDevCmd.bat"
	if "%VCINSTALLDIR%"=="" call "%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.bat"
	echo msbuild VisualStudio\demo.vcxproj  /p:PlatformToolset=v141
	msbuild VisualStudio\demo.vcxproj /p:PlatformToolset=v141
) else (
	echo msbuild VisualStudio\demo.vcxproj  /p:PlatformToolset=v142
	msbuild VisualStudio\demo.vcxproj /p:PlatformToolset=v142
)


del /Q /F VisualStudio\Debug
rmdir /q /s VisualStudio\Debug

