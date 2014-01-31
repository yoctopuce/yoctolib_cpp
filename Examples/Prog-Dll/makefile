# *********************************************************************
#
#  Windows Makefile for examples (use nmake)
# 
# ********************************************************************
.SILENT:

YOCTO_API_SRC = ..\..\Sources\ 

YOCTO_API_DLL_LIB = ..\..\Binaries\windows\yocto.lib
YOCTO_API_DLL     = ..\..\Binaries\windows\yocto.dll


Binary_Windows\demo.exe: main.cpp $(YOCTO_API_DLL)  $(YOCTO_API_DLL_LIB)
	IF NOT EXIST Binary_Windows mkdir Binary_Windows
	$(CPP) $(CPPFLAGS) /EHsc /nologo /I $(YOCTO_API_SRC) /Fe$@  main.cpp /link $(YOCTO_API_DLL_LIB) 
	copy /y $(YOCTO_API_DLL) Binary_Windows

visual:
	msbuild VisualStudioDllExample\VisualStudioDllExample.sln

visualstudioclean:
	del /Q /F VisualStudioDllExample\debug
	rmdir VisualStudioDllExample\debug
	del /Q /F VisualStudioDllExample\YoctopuceDll\debug
	rmdir VisualStudioDllExample\YoctopuceDll\debug
	del /Q /F VisualStudioDllExample\UseYoctopuceDll\debug
	rmdir VisualStudioDllExample\UseYoctopuceDll\debug

release: Binary_Windows\demo.exe visual cleanobj visualstudioclean

clean: cleanobj
	del /Q /F Binary_Windows\demo.exe Binary_Windows\yocto.dll

cleanobj:
	del /Q /F main.lib main.obj main.exp

