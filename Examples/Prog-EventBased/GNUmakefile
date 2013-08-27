# *********************************************************************
#
#  Unix Makefile for examples (use  GNU make)
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
OPTS_GENERIC = -O2 -g -I$(YOCTO_API_SRC)
OPTS_LINK = -lyocto-static -lm -lpthread -lusb-1.0 
# linux targets
DIR_64 = Binary_Linux/64bits/
DIR_32 = Binary_Linux/32bits/
DIR_ARMEL = Binary_Linux/armel/
DIR_ARMHF = Binary_Linux/armhf/
DEMO_64 = $(DIR_64)demo
DEMO_32 = $(DIR_32)demo
DEMO_ARMEL = $(DIR_ARMEL)demo
DEMO_ARMHF = $(DIR_ARMHF)demo


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
	strip $(RELEASE_BUILD)

#linux rules
$(DEMO_64) :  main.cpp $(YOCTO_API_DIR_64)* $(DIR_64) 
	@g++ $(OPTS_GENERIC) $(OPTS_64) -o $@ main.cpp -L$(YOCTO_API_DIR_64) $(OPTS_LINK)

$(DEMO_32) : main.cpp $(YOCTO_API_DIR_32)*  $(DIR_32) 
	@g++ $(OPTS_GENERIC) $(OPTS_32) -o $@ main.cpp -L$(YOCTO_API_DIR_32) $(OPTS_LINK)

$(DEMO_ARMEL) : main.cpp $(YOCTO_API_DIR_ARMEL)*  $(DIR_ARMEL) 
	@g++ $(OPTS_GENERIC) $(OPTS_ARMEL) -o $@ main.cpp -L$(YOCTO_API_DIR_ARMEL) $(OPTS_LINK)

$(DEMO_ARMHF) : main.cpp $(YOCTO_API_DIR_ARMHF)*  $(DIR_ARMHF) 
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
DIR_OSX = Binary_OSX/

$(DIR_OSX)demo: main.cpp $(YOCTO_API_DIR)*  $(DIR_OSX) 
	@gcc -g -I$(YOCTO_API_SRC) -o $@ main.cpp -L$(YOCTO_API_DIR) -lyocto-static -lstdc++  -framework IOKit -framework CoreFoundation 

xcode4:
	xcodebuild -project Xcode/project.xcodeproj

cleanxcode4:
	@rm -rf  Xcode/build


release: $(DIR_OSX)demo xcode4 cleanobj cleanxcode4
	strip $(DIR_OSX)demo

clean: cleanobj
	@rm -rf  $(DIR_OSX)demo

cleanobj:
	@rm -rf   $(DIR_OSX)*.dSYM

endif
 
$(DIR_OSX)  $(DIR_64) $(DIR_32) $(DIR_ARMEL) $(DIR_ARMHF):
	@mkdir -p $@


