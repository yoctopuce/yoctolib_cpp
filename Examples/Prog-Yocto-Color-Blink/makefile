# *********************************************************************
#
#  Windows Makefile for examples (use nmake)
#
# ********************************************************************
.SILENT:

YOCTO_API_SRC = ..\..\Sources\

YOCTO_API_LIB = ..\..\Binaries\windows\yocto-static.lib


Binary_Windows\demo.exe: main.cpp $(YOCTO_API_LIB)
	IF NOT EXIST Binary_Windows mkdir Binary_Windows
	$(CPP) $(CPPFLAGS) /EHsc /nologo /I $(YOCTO_API_SRC) /Fe$@  main.cpp /link $(YOCTO_API_LIB)


visual:
	echo msbuild VisualStudio\demo.vcxproj
	msbuild VisualStudio\demo.vcxproj


visualstudioclean:
	del /Q /F VisualStudio\debug
	rmdir /s /q VisualStudio\debug

release: Binary_Windows\demo.exe visual cleanobj visualstudioclean

clean: cleanobj
	del /Q /F Binary_Windows\demo.exe

cleanobj:
	del /Q /F main.lib main.obj main.exp

