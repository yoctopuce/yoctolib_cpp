ECHO OFF
if "%VCINSTALLDIR%"=="" call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat"
if "%VCINSTALLDIR%"=="" call "%VS140COMNTOOLS%vsvars32.bat"
if "%VCINSTALLDIR%"=="" call "%VS100COMNTOOLS%vsvars32.bat"
if "%VCINSTALLDIR%"=="" call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Professional\Common7\Tools\VsDevCmd.bat"
if "%VCINSTALLDIR%"=="" call "%ProgramFiles(x86)%\Microsoft Visual Studio\2019\Entreprise\Common7\Tools\VsDevCmd.bat"
@nmake /nologo %1
