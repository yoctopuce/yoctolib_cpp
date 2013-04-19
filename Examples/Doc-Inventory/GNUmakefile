# *********************************************************************
#
#  $Id: httpsupport.h 2196 2011-07-28 15:58:38Z seb $
# 
#  unix Makefile for examples (use  GNU make)
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

YOCTO_API_SRC = ../../Sources/

UNAME := $(shell uname)

ifeq ($(UNAME), Linux)

#LINUX COMPILATION
ARCH  := $(shell uname -m| sed -e s/i.86/i386/ -e s/arm.*/arm/)

YOCTO_API_DIR_32  = ../../Binaries/linux/32bits/
YOCTO_API_DIR_64  = ../../Binaries/linux/64bits/
YOCTO_API_DIR_ARMEL = ../../Binaries/linux/armel/
YOCTO_API_DIR_ARMHF = ../../Binaries/linux/armhf/


# most compatible ARMEL options, using soft-float
OPTS_ARMEL = -mfloat-abi=soft -march=armv5 -marm
# reduced ARMHF options to run properly on raspian-thing, but still be compatible with hard-floats VFP
OPTS_ARMHF = -mfloat-abi=hard -march=armv6 -marm
# most compatible ARMEL options, using soft-float
OPTS_64 = -m64
# reduced ARMHF options to run properly on raspian-thing, but still be compatible with hard-floats VFP
OPTS_32 = -m32 
OPTS_GENERIC = -g -I$(YOCTO_API_SRC)
OPTS_LINK = -lyocto-static -lm -lpthread -lusb-1.0 
#linux targets
DEMO_64 = Binary_Linux/64bits/demo
DEMO_32 = Binary_Linux/32bits/demo
DEMO_ARMEL = Binary_Linux/armel/demo
DEMO_ARMHF = Binary_Linux/armhf/demo


ifeq ($(ARCH), x86_64)
DEFAULT_BUILD = $(DEMO_64)
RELEASE_BUILD = $(DEMO_32) $(DEMO_64)
else ifeq ($(ARCH),i386)
DEFAULT_BUILD = $(DEMO_32)
RELEASE_BUILD = $(DEMO_32) 
else
ifeq ($(ARM_BUILD_TYPE), hf)
DEFAULT_BUILD = $(DEMO_ARMHF)
RELEASE_BUILD = $(DEMO_ARMHF)
else
DEFAULT_BUILD = $(DEMO_ARMEL)
RELEASE_BUILD = $(DEMO_ARMEL)

invalid:
	@echo For ARM, use \"make armel\" or \"make armhf\" depending on the floating point ABI used by your system 

armhf: $(DEMO_ARMHF)

armel: $(DEMO_ARMEL)

endif

endif


default: $(DEFAULT_BUILD)

release: $(RELEASE_BUILD)

#linux rules
$(DEMO_64) : main.cpp $(YOCTO_API_DIR_64)*
	@g++ $(OPTS_GENERIC) $(OPTS_64) -o $@ main.cpp -L$(YOCTO_API_DIR_64) $(OPTS_LINK)

$(DEMO_32) : main.cpp $(YOCTO_API_DIR_32)*
	@g++ $(OPTS_GENERIC) $(OPTS_32) -o $@ main.cpp -L$(YOCTO_API_DIR_32) $(OPTS_LINK)

$(DEMO_ARMEL) : main.cpp $(YOCTO_API_DIR_ARMEL)*
	@g++ $(OPTS_GENERIC) $(OPTS_ARMEL) -o $@ main.cpp -L$(YOCTO_API_DIR_ARMEL) $(OPTS_LINK)

$(DEMO_ARMHF) : main.cpp $(YOCTO_API_DIR_ARMHF)*
	@g++ $(OPTS_GENERIC) $(OPTS_ARMHF) -o $@ main.cpp -L$(YOCTO_API_DIR_ARMHF) $(OPTS_LINK)


codeblock:
	codeblocks CodeBlocks/CodeBlocks_lin.cbp --build

codeblockclean:
	codeblocks CodeBlocks/CodeBlocks_lin.cbp --clean
	@rm -rf CodeBlocks/CodeBlocks_lin.depend*
	@rm -rf CodeBlocks/CodeBlocks_lin.layout*

clean:
	@rm -rf  $(DEMO_64) $(DEMO_32) $(DEMO_ARMEL) $(DEMO_ARMHF)

else
# MAC OS X COMPILATION

YOCTO_API_DIR = ../../Binaries/osx

Binary_OSX/demo: main.cpp $(YOCTO_API_DIR)*
	@gcc -g -I$(YOCTO_API_SRC) -o $@ main.cpp -L$(YOCTO_API_DIR) -lyocto-static -lstdc++  -framework IOKit -framework CoreFoundation 

xcode4:
	xcodebuild -project Xcode/project.xcodeproj

cleanxcode4:
	@rm -rf  Xcode/build


release: Binary_OSX/demo xcode4 cleanobj cleanxcode4

clean: cleanobj
	@rm -rf  Binary_OSX/demo

cleanobj:
	@rm -rf   Binary_OSX/*.dSYM

endif




