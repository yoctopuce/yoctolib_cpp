if "%VCINSTALLDIR%"=="" call "%VS100COMNTOOLS%vsvars32.bat"
@nmake /nologo %1
