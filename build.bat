@ECHO OFF
if NOT "%VCToolsInstallDir%"=="" goto compile
if "%VCINSTALLDIR%"=="" call "%VS150COMNTOOLS%vsvars32.bat"
if "%VCINSTALLDIR%"=="" call "%VS140COMNTOOLS%vsvars32.bat"
if "%VCINSTALLDIR%"=="" call "%VS100COMNTOOLS%vsvars32.bat"
if "%VCINSTALLDIR%"=="" call "%VS110COMNTOOLS%vsvars32.bat"
if "%VCINSTALLDIR%"=="" call "%VS120COMNTOOLS%vsvars32.bat"

:compile
echo.
echo Build yapi static and dynamic libray for C++
echo ============================================

cd Binaries\
call nmake /nologo %1
IF ERRORLEVEL 1 goto error
cd ..\
echo done

set failled=
FOR /D %%A IN (Examples\*) DO (call:BuildDir %%A %1)
IF NOT DEFINED failled goto end

echo.
echo COMPILATION HAS FAILLED ON DIRECTORIES :
echo %failled%

goto error
:BuildDir
echo build %~1 %~2
cd %~1
call nmake /nologo %~2
IF ERRORLEVEL 1 set failled=%failled% %~1
cd ..\..\
echo done
GOTO:EOF

goto end
:error
echo error
exit /b 1
:end