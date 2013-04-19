# *********************************************************************
#
#  $Id: httpsupport.h 2196 2011-07-28 15:58:38Z seb $
# 
#  windows Makefile for examples (use nmake)
#  - - - - - - - - - License information: - - - - - - - - -
# 
#  Copyright (C) 2011 and beyond by Yoctopuce Sarl, Switzerland.
# 
#  1) If you have obtained this file from www.yoctopuce.com using
#     a valid customer account established in your proper name,
#     Yoctopuce Sarl (hereafter Licensor) licenses to you (hereafter
#     Licensee) the right to use, modify, copy, and integrate this
#     source file into your own solution for the sole purpose of
#     interfacing a Yoctopuce product integrated into Licensee's
#     solution.
# 
#     You should refer to the license agreement accompanying this
#     Software for additional information regarding your rights
#     and obligations.
# 
#     THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
#     WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING
#     WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS
#     FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO
#     EVENT SHALL LICENSOR BE LIABLE FOR ANY INCIDENTAL, SPECIAL,
#     INDIRECT OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA,
#     COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR
#     SERVICES, ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT
#     LIMITED TO ANY DEFENSE THEREOF), ANY CLAIMS FOR INDEMNITY OR
#     CONTRIBUTION, OR OTHER SIMILAR COSTS, WHETHER ASSERTED ON THE
#     BASIS OF CONTRACT, TORT (INCLUDING NEGLIGENCE), BREACH OF
#     WARRANTY, OR OTHERWISE.
# 
#  2) If you have obtained this file from any other source, you
#     are not entitled to use it, read it or create any derived
#     material. You should delete this file immediately.
# 
# ********************************************************************
.SILENT:

YOCTO_API_SRC = ..\..\Sources\ 

YOCTO_API_LIB = ..\..\Binaries\windows\yocto-static.lib


Binary_Windows\demo.exe: main.cpp $(YOCTO_API_LIB)
	$(CPP) $(CPPFLAGS) /EHsc /nologo /I $(YOCTO_API_SRC) /Fe$@  main.cpp /link $(YOCTO_API_LIB) 


visual:
	echo msbuild VisualStudio\VisualStudio.vcxproj
	msbuild VisualStudio\VisualStudio.vcxproj


visualstudioclean:
	del /Q /F VisualStudio\debug
	rmdir VisualStudio\debug

release: Binary_Windows\demo.exe visual cleanobj visualstudioclean

clean: cleanobj
	del /Q /F Binary_Windows\demo.exe

cleanobj:
	del /Q /F main.lib main.obj main.exp

