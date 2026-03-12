ifeq ($(OS),Windows_NT)
UNAME := Windows
ARCH  := "windows"
else
UNAME := $(shell uname)
ARCH  := $(shell uname -m| sed -e s/i.86/i386/ -e s/arm.*/arm/)
endif


YOCTO_API_SRC = ../../Sources/

# yoctolib path
YOCTO_API_DIR_WIN32  = ../../Binaries/windows/
YOCTO_API_DIR_WIN64  = ../../Binaries/windows/amd64/
YOCTO_API_DIR_I386  = ../../Binaries/linux/i386/
YOCTO_API_DIR_x86_64  = ../../Binaries/linux/x86_64/
YOCTO_API_DIR_ARMEL = ../../Binaries/linux/armel/
YOCTO_API_DIR_ARMHF = ../../Binaries/linux/armhf/
YOCTO_API_DIR_AARCH64 = ../../Binaries/linux/aarch64/
YOCTO_API_DIR_OSX = ../../Binaries/osx


DIR_WIN32 = Binary_Windows/
DIR_WIN64 = Binary_Windows/amd64/
DIR_x86_64 = Binary_Linux/x86_64/
DIR_I386 = Binary_Linux/i386/
DIR_ARMEL = Binary_Linux/armel/
DIR_ARMHF = Binary_Linux/armhf/
DIR_AARCH64 = Binary_Linux/aarch64/
DIR_OSX = Binary_OSX/


DEMO_WIN32 = $(DIR_WIN32)demo.exe
DEMO_WIN64 = $(DIR_WIN64)demo.exe
DEMO_x86_64 = $(DIR_x86_64)demo
DEMO_I386 = $(DIR_I386)demo
DEMO_ARMEL = $(DIR_ARMEL)demo
DEMO_ARMHF = $(DIR_ARMHF)demo
DEMO_AARCH64 = $(DIR_AARCH64)demo
DEMO_OSX = $(DIR_OSX)demo


ifeq ($(OS),Windows_NT)
# WINDOWS COMPILATION

DEFAULT_BUILD = $(DEMO_WIN32)
RELEASE_BUILD = $(DEMO_WIN32)
STRIP_BUILD =  $(DEMO_x86_64)
# end windows
else ifeq ($(UNAME), Linux)
# LINUX COMPILATION

# most compatible ARMEL options, using soft-float
OPTS_ARMEL = -mfloat-abi=soft -march=armv5 -marm
# reduced ARMHF options to run properly on raspian-thing, but still be compatible with hard-floats VFP
OPTS_ARMHF = -mfloat-abi=hard -march=armv6 -marm
# most compatible ARMEL options, using soft-float
OPTS_x86_64 = -m64
# reduced ARMHF options to run properly on raspian-thing, but still be compatible with hard-floats VFP
OPTS_I386 = -m32
OPTS_GENERIC = -O2 -g -I$(YOCTO_API_SRC)
OPTS_LINK = -lyocto-static -lm -lpthread -lusb-1.0

# Linux
ifeq ($(ARCH), x86_64)
DEFAULT_BUILD = $(DEMO_x86_64)
RELEASE_BUILD = $(DEMO_I386) $(DEMO_x86_64)
STRIP_BUILD =  $(DEMO_I386) $(DEMO_x86_64)
else ifeq ($(ARCH),i386)
DEFAULT_BUILD = $(DEMO_I386)
RELEASE_BUILD = $(DEMO_I386)
STRIP_BUILD = $(DEMO_I386)
else ifeq ($(ARCH),aarch64)
DEFAULT_BUILD = $(DEMO_AARCH64)
RELEASE_BUILD = $(DEMO_AARCH64)
STRIP_BUILD = $(DEMO_AARCH64)
else
ifeq ($(ARM_BUILD_TYPE), hf)
DEFAULT_BUILD = $(DEMO_ARMHF)
RELEASE_BUILD = $(DEMO_ARMHF)
STRIP_BUILD = $(DEMO_ARMHF)
else
DEFAULT_BUILD = $(DEMO_ARMEL)
RELEASE_BUILD = $(DEMO_ARMEL)
STRIP_BUILD = $(DEMO_ARMEL)

invalid:
	@echo For ARM, use \"make armel\", \"make armhf\" or \"make aarch64\" depending on the distribution

armhf: $(DEMO_ARMHF)

armel: $(DEMO_ARMEL)

# end of hf
endif

endif

else
# MAC OS X COMPILATION

DEFAULT_BUILD = $(DEMO_OSX)
RELEASE_BUILD = compile_release clean
STRIP_BUILD = $(DEMO_OSX)


endif


default: $(DEFAULT_BUILD)

release: $(RELEASE_BUILD)
	@rm -f $(STRIP_BUILD)

../../Binaries/%/libyocto-static.a:
	@echo compiling Yoctopuce C++ lib for $*
	@make -C ../../Binaries $*/libyocto-static.a

../../Binaries/%/libyocto-static.lib:
	@echo compiling Yoctopuce C++ lib for $*
	@make -C ../../Binaries $*/libyocto-static.lib

#windows rules
$(DEMO_WIN32) : main.cpp $(YOCTO_API_DIR_WIN32)yocto-static.lib $(DIR_WIN32)
	@cl /EHsc /nologo /I $(YOCTO_API_SRC) /Fe$@ main.cpp /link $(YOCTO_API_DIR_WIN32)yocto-static.lib

$(DEMO_WIN64) :  main.cpp $(YOCTO_API_DIR_WIN64)yocto-static.lib $(DIR_WIN64)
	@cl /EHsc /nologo /I $(YOCTO_API_SRC) /Fe$@ main.cpp /link $(YOCTO_API_DIR_WIN64)yocto-static.lib

#linux rules
$(DEMO_x86_64) :  main.cpp $(YOCTO_API_DIR_x86_64)libyocto-static.a $(DIR_x86_64)
	@g++ $(OPTS_GENERIC) $(OPTS_x86_64) -o $@ main.cpp -L$(YOCTO_API_DIR_x86_64) $(OPTS_LINK)

$(DEMO_I386) : main.cpp $(YOCTO_API_DIR_I386)libyocto-static.a $(DIR_I386)
	@g++ $(OPTS_GENERIC) $(OPTS_I386) -o $@ main.cpp -L$(YOCTO_API_DIR_I386) $(OPTS_LINK)

$(DEMO_ARMEL) : main.cpp $(YOCTO_API_DIR_ARMEL)libyocto-static.a $(DIR_ARMEL)
	@g++ $(OPTS_GENERIC) $(OPTS_ARMEL) -o $@ main.cpp -L$(YOCTO_API_DIR_ARMEL) $(OPTS_LINK)

$(DEMO_ARMHF) : main.cpp $(YOCTO_API_DIR_ARMHF)libyocto-static.a $(DIR_ARMHF)
	@g++ $(OPTS_GENERIC) $(OPTS_ARMHF) -o $@ main.cpp -L$(YOCTO_API_DIR_ARMHF) $(OPTS_LINK)

$(DEMO_AARCH64) : main.cpp $(YOCTO_API_DIR_AARCH64)libyocto-static.a $(DIR_AARCH64)
	@g++ $(OPTS_GENERIC) $(OPTS_AARCH64) -o $@ main.cpp -L$(YOCTO_API_DIR_AARCH64) $(OPTS_LINK)

#OSX rules
$(DEMO_OSX): main.cpp $(YOCTO_API_DIR_OSX)*  $(DIR_OSX)
	@gcc -g -I$(YOCTO_API_SRC) -o $@ main.cpp -L$(YOCTO_API_DIR_OSX) -lyocto-static -lstdc++  -framework IOKit -framework CoreFoundation

codeblock:
	codeblocks CodeBlocks/CodeBlocks_lin.cbp --build

codeblockclean:
	codeblocks CodeBlocks/CodeBlocks_lin.cbp --clean
	@rm -rf CodeBlocks/CodeBlocks_lin.depend*
	@rm -rf CodeBlocks/CodeBlocks_lin.layout*

xcode4:
	@mkdir -p Xcode/build/Release
	xcodebuild -project Xcode/project.xcodeproj

cleanxcode4:
	@rm -rf  Xcode/build

compile_release: $(DEMO_OSX) xcode4 cleanobj cleanxcode4
	strip $(DEMO_OSX)


cleanobj:
	@rm -rf $(DIR_OSX)*.dSYM

clean: cleanobj
	@rm -rf  $(DEMO_OSX)
	@rm -rf  $(DEMO_x86_64) $(DEMO_I386) $(DEMO_ARMEL) $(DEMO_ARMHF) $(DEMO_AARCH64)



$(DIR_WIN32) $(DIR_WIN64) $(DIR_OSX) $(DIR_x86_64) $(DIR_I386) $(DIR_ARMEL) $(DIR_ARMHF) $(DIR_AARCH64):
	@mkdir -p $@


