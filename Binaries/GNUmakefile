# *********************************************************************
#
#  $Id: GNUmakefile 59479 2024-02-26 09:26:58Z seb $
#
#  GNU Makefile for linux and Mac OS X
#
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

ifeq ($(OS),Windows_NT)
UNAME := Windows
ARCH  := "windows"
CCS_INSTALL_DIR ?= c:/ti/ccs930
XDCTOOLS ?= $(CCS_INSTALL_DIR)/xdctools_3_60_02_34_core/bin/
RM = $(XDCTOOLS)rm
MKDIR = $(XDCTOOLS)mkdir
else
UNAME := $(shell uname)
ARCH  := $(shell uname -m| sed -e s/i.86/i386/ -e s/arm.*/arm/)
RM = rm
MKDIR = mkdir
endif


MIPS_TARGET_ROOT = $(TARGET_MIPS)
MIPSEL_TARGET_ROOT = $(TARGET_MIPSEL)

TEST_SRC = ../Examples/Doc-Inventory/main.cpp

#C++ output directories
DIR_WIN32 = windows/
DIR_WIN64 = windows/amd64/
DIR_I386 = linux/i386/
DIR_X64 = linux/x86_64/
DIR_ARMEL = linux/armel/
DIR_ARMHF = linux/armhf/
DIR_AARCH64 = linux/aarch64/
DIR_MIPS = linux/mips/
DIR_MIPSEL = linux/mipsel/
DIR_OSX = osx/
DIR_OSX32 = osx32/

#C lowlevel output directories
YAPI_DIR_WIN32 =  windows/yapi/
YAPI_DIR_WIN64 =  windows/yapi/amd64/
YAPI_DIR_I386 = $(DIR_I386)yapi/
YAPI_DIR_X64 = $(DIR_X64)yapi/
YAPI_DIR_ARMEL = $(DIR_ARMEL)yapi/
YAPI_DIR_ARMHF = $(DIR_ARMHF)yapi/
YAPI_DIR_AARCH64 = $(DIR_AARCH64)yapi/
YAPI_DIR_MIPS = $(DIR_MIPS)yapi/
YAPI_DIR_MIPSEL = $(DIR_MIPSEL)yapi/
YAPI_DIR_OSX = $(DIR_OSX)yapi/
YAPI_DIR_OSX32 = $(DIR_OSX32)yapi/

# temp obj directories
OBJDIR_WIN32 = $(DIR_WIN32)objs/
OBJDIR_WIN64 = $(DIR_WIN64)objs/
OBJDIR_I386 = $(DIR_I386)objs/
OBJDIR_X64 = $(DIR_X64)objs/
OBJDIR_ARMEL = $(DIR_ARMEL)objs/
OBJDIR_ARMHF = $(DIR_ARMHF)objs/
OBJDIR_AARCH64 = $(DIR_AARCH64)objs/
OBJDIR_MIPS = $(DIR_MIPS)objs/
OBJDIR_MIPSEL = $(DIR_MIPSEL)objs/
OBJDIR_OSX = $(DIR_OSX)objs/
OBJDIR_OSX32 = $(DIR_OSX32)objs/


DEF_SSL_GCC  = -DNO_YSSL



#CC compiler for each arch
CC_WIN32 = cl
CC_WIN64 = cl
CC_I386 = gcc
CC_X64 = gcc
ifdef TOOLCHAIN_QARM
CC_ARMEL = $(TOOLCHAIN_QARM)/bin/arm-none-linux-gnueabi-gcc
else
CC_ARMEL = gcc
endif
ifdef TOOLCHAIN_ARMHF
CC_ARMHF = $(TOOLCHAIN_ARMHF)/bin/arm-linux-gnueabihf-gcc
else
CC_ARMHF = gcc
endif
ifdef TOOLCHAIN_AARCH64
CC_AARCH64 = $(TOOLCHAIN_AARCH64)/bin/arm-linux-gnueabihf-gcc
else
CC_AARCH64 = gcc
endif
CC_MIPS = mips-openwrt-linux-gcc
CC_MIPSEL = mipsel-openwrt-linux-gcc
CC_OSX = gcc
CC_OSX32 = gcc


#CPP compiler for each arch
CPP_WIN32 = cl
CPP_WIN64 = cl
CPP_I386 = g++
CPP_X64 = g++
ifdef TOOLCHAIN_QARM
CPP_ARMEL = $(TOOLCHAIN_QARM)/bin/arm-none-linux-gnueabi-g++
else
CPP_ARMEL = g++
endif
ifdef TOOLCHAIN_ARMHF
CPP_ARMHF = $(TOOLCHAIN_ARMHF)/bin/arm-linux-gnueabihf-g++
else
CPP_ARMHF = g++
endif
ifdef TOOLCHAIN_AARCH64
CPP_AARCH64 = $(TOOLCHAIN_AARCH64)/bin/arm-linux-gnueabihf-g++
else
CPP_AARCH64 = g++
endif
CPP_MIPS = mips-openwrt-linux-g++
CPP_MIPSEL = mipsel-openwrt-linux-g++
CPP_OSX = g++
CPP_OSX32 = g++

RC = rc

#generic flags
OPTS_DBG = -g -Wall $(DEF_FLAGS_GCC)
OPTS_PROD = -g -Wall $(DEF_FLAGS_GCC)
OPTS_STAT =
OPTS_DYN = -fPIC

OPTS_JAVA = -fPIC -DYAPI_WITH_JNI -I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/linux -I$(JAVA_HOME)/include/darwin
ifdef JAVA_HOME
OPTS_DYN = $(OPTS_JAVA)
endif


OPTS_DBG_WIN = $(DEF_FLAGS_WIN)
OPTS_PROD_WIN = /nologo  $(DEF_FLAGS_WIN)
OPTS_STAT_WIN =  /Zl
OPTS_DYN_WIN = /D GENERATE_DLL /W3 /WX- /O2 /Oi /Oy- /Gy /GL /MT

OPTS_WIN32 =
OPTS_WIN64 =


# most compatible ARMEL options, using soft-float
ifdef TOOLCHAIN_QARM
OPTS_ARMEL = -I$(STAGING_QARM)/usr/include/ -O2 -mfloat-abi=soft -march=armv5 -marm -DBUILD_ARMEL -DNO_YSSL
else
OPTS_ARMEL = -mfloat-abi=soft -march=armv5 -DBUILD_ARMEL -D_XOPEN_SOURCE=500 -D_BSD_SOURCE -DNO_YSSL
endif
ifdef TOOLCHAIN_ARMHF
# reduced ARMHF options to run properly on raspian-thing, but still be compatible with hard-floats VFP
OPTS_ARMHF = -I$(STAGING_ARMHF)/usr/include/ $(OPTS_DBG) -mfloat-abi=hard -march=armv6 -marm -DBUILD_ARMHF
else
OPTS_ARMHF = -mfloat-abi=hard -march=armv6 -marm -DBUILD_ARMHF -Wno-psabi
endif
ifdef TOOLCHAIN_AARCH64
OPTS_AARCH64 = -I$(STAGING_AARCH64)/usr/include/ $(OPTS_DBG) -march=native -DBUILD_AARCH64
else
OPTS_AARCH64 = -march=native -DBUILD_AARCH64
endif
OPTS_MIPS = -DCPU_BIG_ENDIAN -I$(MIPS_TARGET_ROOT)/usr/include/ -DNO_YSSL
OPTS_MIPSEL = -I$(MIPSEL_TARGET_ROOT)/usr/include/ -DNO_YSSL

OPTS_X64 = -m64 -g -Wall -D_GNU_SOURCE
OPTS_I386 = -m32 -D_GNU_SOURCE -march=i586
#osx specific flags
OPTS_OSX =
OPTS_OSX32 = -m32


#link optionsfor each arch
LINKWITH_WIN32 = "Advapi32.lib"
LINKWITH_WIN64 = "Advapi32.lib"
LINKWITH_I386 = -lm -lpthread -lusb-1.0
LINKWITH_X64 = -lm -lpthread -lusb-1.0
ifdef TOOLCHAIN_QARM
LINKWITH_ARMEL = -L$(STAGING_QARM)/usr/lib/ -lm -lpthread -lusb-1.0
else
LINKWITH_ARMEL = -lm -lpthread -lusb-1.0
endif
ifdef TOOLCHAIN_ARMHF
LINKWITH_ARMHF = -L$(STAGING_ARMHF)/usr/lib/ -lm -lpthread -lusb-1.0
else
LINKWITH_ARMHF = -lm -lpthread -lusb-1.0
endif
ifdef TOOLCHAIN_AARCH64
LINKWITH_AARCH64 = -L$(STAGING_AARCH64)/usr/lib/ -lm -lpthread -lusb-1.0
else
LINKWITH_AARCH64 = -lm -lpthread -lusb-1.0
endif
LINKWITH_MIPS = -L$(MIPS_TARGET_ROOT)/usr/lib/ -lm -lpthread -lusb-1.0
LINKWITH_MIPSEL = -L$(MIPSEL_TARGET_ROOT)/usr/lib/ -lm -lpthread -lusb-1.0
LINKWITH_OSX =

YAPI_LINKWITH_WIN32 = "Advapi32.lib"
YAPI_LINKWITH_WIN64 = "Advapi32.lib"
YAPI_LINKWITH_I386 = -lpthread -lusb-1.0
YAPI_LINKWITH_X64 = -lpthread -lusb-1.0
ifdef TOOLCHAIN_QARM
YAPI_LINKWITH_ARMEL = -L$(STAGING_QARM)/usr/lib/ -lpthread -lusb-1.0
else
YAPI_LINKWITH_ARMEL = -lpthread -lusb-1.0
endif
ifdef TOOLCHAIN_ARMHF
YAPI_LINKWITH_ARMHF = -L$(STAGING_ARMHF)/usr/lib/    -lpthread -lusb-1.0
else
YAPI_LINKWITH_ARMHF = -lpthread -lusb-1.0
endif
ifdef TOOLCHAIN_AARCH64
YAPI_LINKWITH_AARCH64 = -L$(STAGING_AARCH64)/usr/lib/    -lpthread -lusb-1.0
else
YAPI_LINKWITH_AARCH64 = -lpthread -lusb-1.0
endif
YAPI_LINKWITH_MIPS = -L$(MIPS_TARGET_ROOT)/usr/lib/ -lpthread -lusb-1.0
YAPI_LINKWITH_MIPSEL = -L$(MIPSEL_TARGET_ROOT)/usr/lib/ -lpthread -lusb-1.0
YAPI_LINKWITH_OSX =
YAPI_LINKWITH_OSX32 =



#striper for each arch :-)
STRIP_WIN32 = $(XDCTOOLS)strip
STRIP_WIN64 = $(XDCTOOLS)strip
STRIP_I386 = strip
STRIP_X64 = strip
ifdef TOOLCHAIN_QARM
STRIP_ARMEL = $(TOOLCHAIN_QARM)/bin/arm-none-linux-gnueabi-strip
else
STRIP_ARMEL = strip
endif
ifdef TOOLCHAIN_ARMHF
STRIP_ARMHF = $(TOOLCHAIN_ARMHF)/bin/arm-linux-gnueabihf-strip
else
STRIP_ARMHF = strip
endif
STRIP_AARCH64 = strip
STRIP_MIPS = mips-openwrt-linux-strip
STRIP_MIPSEL = mipsel-openwrt-linux-strip
STRIP_OSX = strip


#define all tragets
LIBS_YAPI_WIN32   = $(YAPI_DIR_WIN32)yapi-static.lib $(YAPI_DIR_WIN32)yapi.dll
LIBS_WIN32        = $(DIR_WIN32)yocto-static.lib $(DIR_WIN32)yocto.dll $(LIBS_YAPI_WIN32)
LIBS_YAPI_WIN64   = $(YAPI_DIR_WIN64)yapi-static.lib $(YAPI_DIR_WIN64)yapi.dll
LIBS_WIN64        = $(DIR_WIN64)yocto-static.lib $(DIR_WIN64)yocto.dll $(LIBS_YAPI_WIN64)
LIBS_YAPI_I386    = $(YAPI_DIR_I386)libyapi-static.a $(YAPI_DIR_I386)libyapi.so.1.0.1
LIBS_I386         = $(DIR_I386)libyocto-static.a $(DIR_I386)libyocto.so.1.0.1 $(LIBS_YAPI_I386)
LIBS_YAPI_X64     = $(YAPI_DIR_X64)libyapi-static.a $(YAPI_DIR_X64)libyapi.so.1.0.1
LIBS_X64          = $(DIR_X64)libyocto-static.a $(DIR_X64)libyocto.so.1.0.1 $(LIBS_YAPI_X64)
LIBS_YAPI_ARMEL   = $(YAPI_DIR_ARMEL)libyapi-static.a $(YAPI_DIR_ARMEL)libyapi.so.1.0.1
LIBS_ARMEL        = $(DIR_ARMEL)libyocto-static.a $(DIR_ARMEL)libyocto.so.1.0.1 $(LIBS_YAPI_ARMEL)
LIBS_YAPI_ARMHF   = $(YAPI_DIR_ARMHF)libyapi-static.a $(YAPI_DIR_ARMHF)libyapi.so.1.0.1
LIBS_ARMHF        = $(DIR_ARMHF)libyocto-static.a $(DIR_ARMHF)libyocto.so.1.0.1 $(LIBS_YAPI_ARMHF)
LIBS_YAPI_AARCH64 = $(YAPI_DIR_AARCH64)libyapi-static.a $(YAPI_DIR_AARCH64)libyapi.so.1.0.1
LIBS_AARCH64      = $(DIR_AARCH64)libyocto-static.a $(DIR_AARCH64)libyocto.so.1.0.1 $(LIBS_YAPI_AARCH64)
LIBS_YAPI_MIPS    = $(YAPI_DIR_MIPS)libyapi-static.a $(YAPI_DIR_MIPS)libyapi.so.1.0.1
LIBS_MIPS         = $(DIR_MIPS)libyocto-static.a $(DIR_MIPS)libyocto.so.1.0.1 $(LIBS_YAPI_MIPS)
LIBS_YAPI_MIPSEL  = $(YAPI_DIR_MIPSEL)libyapi-static.a $(YAPI_DIR_MIPSEL)libyapi.so.1.0.1
LIBS_MIPSEL       = $(DIR_MIPSEL)libyocto-static.a $(DIR_MIPSEL)libyocto.so.1.0.1 $(LIBS_YAPI_MIPSEL)
LIBS_OSX          = $(DIR_OSX)libyocto-static.a  $(YAPI_DIR_OSX)libyapi-static.a $(DIR_OSX)libyocto.dylib $(YAPI_DIR_OSX)libyapi.dylib
LIBS_OSX32        = $(YAPI_DIR_OSX32)libyapi.dylib



#define all object needed
SOURCES_DIR = ../Sources/


#-- yocto_patch_start --
YOCTO_API_OBJS_STAT = yocto_api.o\
	yocto_accelerometer.o \
	yocto_altitude.o \
	yocto_anbutton.o \
	yocto_arithmeticsensor.o \
	yocto_audioin.o \
	yocto_audioout.o \
	yocto_bluetoothlink.o \
	yocto_buzzer.o \
	yocto_carbondioxide.o \
	yocto_cellular.o \
	yocto_colorled.o \
	yocto_colorledcluster.o \
	yocto_compass.o \
	yocto_current.o \
	yocto_currentloopoutput.o \
	yocto_daisychain.o \
	yocto_digitalio.o \
	yocto_display.o \
	yocto_dualpower.o \
	yocto_files.o \
	yocto_genericsensor.o \
	yocto_gps.o \
	yocto_groundspeed.o \
	yocto_gyro.o \
	yocto_hubport.o \
	yocto_humidity.o \
	yocto_i2cport.o \
	yocto_inputcapture.o \
	yocto_inputchain.o \
	yocto_latitude.o \
	yocto_led.o \
	yocto_lightsensor.o \
	yocto_longitude.o \
	yocto_magnetometer.o \
	yocto_messagebox.o \
	yocto_motor.o \
	yocto_multiaxiscontroller.o \
	yocto_multicellweighscale.o \
	yocto_multisenscontroller.o \
	yocto_network.o \
	yocto_oscontrol.o \
	yocto_power.o \
	yocto_poweroutput.o \
	yocto_powersupply.o \
	yocto_pressure.o \
	yocto_proximity.o \
	yocto_pwminput.o \
	yocto_pwmoutput.o \
	yocto_pwmpowersource.o \
	yocto_quadraturedecoder.o \
	yocto_rangefinder.o \
	yocto_realtimeclock.o \
	yocto_refframe.o \
	yocto_relay.o \
	yocto_rfidreader.o \
	yocto_sdi12port.o \
	yocto_segmenteddisplay.o \
	yocto_serialport.o \
	yocto_servo.o \
	yocto_spiport.o \
	yocto_steppermotor.o \
	yocto_temperature.o \
	yocto_threshold.o \
	yocto_tilt.o \
	yocto_tvoc.o \
	yocto_voc.o \
	yocto_voltage.o \
	yocto_voltageoutput.o \
	yocto_wakeupmonitor.o \
	yocto_wakeupschedule.o \
	yocto_watchdog.o \
	yocto_weighscale.o \
	yocto_wireless.o \

#-- yocto_patch_stop --


include mbedtls.inc

YOCTO_API_OBJS_DYN  = $(patsubst %.o, %_dyn.o,$(YOCTO_API_OBJS_STAT))

YAPI_SOURCES_DIR    = $(SOURCES_DIR)yapi/

YAPI_OBJS_STAT = yapi.o ystream.o yprog.o yfifo.o ykey.o yhash.o yjson.o ytcp.o ymemory.o ythread.o yjni.o yssl.o
YAPI_OBJS_DYN   = $(patsubst %.o, %_dyn.o,$(YAPI_OBJS_STAT))

MBEDTLS_DIR           = $(YAPI_SOURCES_DIR)mbedtls/
MBEDTLS_INCLUDE_DIR   = $(MBEDTLS_DIR)include/
MBEDTLS_SOURCES_DIR  = $(MBEDTLS_DIR)library/

MBEDTSL_OBJS_STAT = $(OBJS_CRYPTO) $(OBJS_X509) $(OBJS_TLS)
MBEDTSL_OBJS_DYN   = $(patsubst %.o, %_dyn.o,$(MBEDTSL_OBJS_STAT))

ALL_OBJS_STAT   = $(YAPI_OBJS_STAT) $(YOCTO_API_OBJS_STAT)
ALL_OBJS_DYN    = $(YAPI_OBJS_DYN)  $(YOCTO_API_OBJS_DYN)

#create a new definitions for each platform using macro
ALL_OBJS_STAT_WIN32    = $(patsubst %.o, $(OBJDIR_WIN32)%.obj, $(ALL_OBJS_STAT) $(MBEDTSL_OBJS_STAT) ypkt_win.o)
ALL_OBJS_STAT_WIN64    = $(patsubst %.o, $(OBJDIR_WIN64)%.obj, $(ALL_OBJS_STAT) $(MBEDTSL_OBJS_STAT) ypkt_win.o)
ALL_OBJS_STAT_I386    = $(patsubst %.o, $(OBJDIR_I386)%.o, $(ALL_OBJS_STAT) $(MBEDTSL_OBJS_STAT) ypkt_lin.o)
ALL_OBJS_STAT_X64    = $(patsubst %.o, $(OBJDIR_X64)%.o, $(ALL_OBJS_STAT) $(MBEDTSL_OBJS_STAT) ypkt_lin.o)
ALL_OBJS_STAT_ARMEL = $(patsubst %.o, $(OBJDIR_ARMEL)%.o, $(ALL_OBJS_STAT) ypkt_lin.o)
ALL_OBJS_STAT_ARMHF = $(patsubst %.o, $(OBJDIR_ARMHF)%.o, $(ALL_OBJS_STAT) $(MBEDTSL_OBJS_STAT) ypkt_lin.o)
ALL_OBJS_STAT_AARCH64 = $(patsubst %.o, $(OBJDIR_AARCH64)%.o, $(ALL_OBJS_STAT) $(MBEDTSL_OBJS_STAT) ypkt_lin.o)
ALL_OBJS_STAT_MIPS = $(patsubst %.o, $(OBJDIR_MIPS)%.o, $(ALL_OBJS_STAT) ypkt_lin.o)
ALL_OBJS_STAT_MIPSEL = $(patsubst %.o, $(OBJDIR_MIPSEL)%.o, $(ALL_OBJS_STAT) ypkt_lin.o)
ALL_OBJS_STAT_OSX   = $(patsubst %.o, $(OBJDIR_OSX)%.o, $(ALL_OBJS_STAT) $(MBEDTSL_OBJS_STAT) ypkt_osx.o)

ALL_OBJS_DYN_WIN32    = $(patsubst %.o, $(OBJDIR_WIN32)%.obj, $(ALL_OBJS_DYN) $(MBEDTSL_OBJS_DYN) ypkt_win_dyn.o)
ALL_OBJS_DYN_WIN64    = $(patsubst %.o, $(OBJDIR_WIN64)%.obj, $(ALL_OBJS_DYN) $(MBEDTSL_OBJS_DYN) ypkt_win_dyn.o)
ALL_OBJS_DYN_I386     = $(patsubst %.o, $(OBJDIR_I386)%.o, $(ALL_OBJS_DYN) $(MBEDTSL_OBJS_DYN) ypkt_lin_dyn.o)
ALL_OBJS_DYN_X64     = $(patsubst %.o, $(OBJDIR_X64)%.o, $(ALL_OBJS_DYN) $(MBEDTSL_OBJS_DYN) ypkt_lin_dyn.o)
ALL_OBJS_DYN_ARMEL  = $(patsubst %.o, $(OBJDIR_ARMEL)%.o, $(ALL_OBJS_DYN) ypkt_lin_dyn.o)
ALL_OBJS_DYN_ARMHF  = $(patsubst %.o, $(OBJDIR_ARMHF)%.o, $(ALL_OBJS_DYN) $(MBEDTSL_OBJS_DYN) ypkt_lin_dyn.o)
ALL_OBJS_DYN_AARCH64  = $(patsubst %.o, $(OBJDIR_AARCH64)%.o, $(ALL_OBJS_DYN) $(MBEDTSL_OBJS_DYN) ypkt_lin_dyn.o)
ALL_OBJS_DYN_MIPS  = $(patsubst %.o, $(OBJDIR_MIPS)%.o, $(ALL_OBJS_DYN) ypkt_lin_dyn.o)
ALL_OBJS_DYN_MIPSEL  = $(patsubst %.o, $(OBJDIR_MIPSEL)%.o, $(ALL_OBJS_DYN) ypkt_lin_dyn.o)
ALL_OBJS_DYN_OSX    = $(patsubst %.o, $(OBJDIR_OSX)%.o, $(ALL_OBJS_DYN) $(MBEDTSL_OBJS_DYN) ypkt_osx_dyn.o)

YAPI_OBJS_STAT_WIN32  = $(patsubst %.o, $(OBJDIR_WIN32)%.obj, $(YAPI_OBJS_STAT) $(MBEDTSL_OBJS_STAT) ypkt_win.o)
YAPI_OBJS_STAT_WIN64  = $(patsubst %.o, $(OBJDIR_WIN64)%.obj, $(YAPI_OBJS_STAT) $(MBEDTSL_OBJS_STAT) ypkt_win.o)
YAPI_OBJS_STAT_I386  = $(patsubst %.o, $(OBJDIR_I386)%.o, $(YAPI_OBJS_STAT) $(MBEDTSL_OBJS_STAT) ypkt_lin.o)
YAPI_OBJS_STAT_X64  = $(patsubst %.o, $(OBJDIR_X64)%.o, $(YAPI_OBJS_STAT) $(MBEDTSL_OBJS_STAT) ypkt_lin.o)
YAPI_OBJS_STAT_ARMEL = $(patsubst %.o, $(OBJDIR_ARMEL)%.o, $(YAPI_OBJS_STAT) ypkt_lin.o)
YAPI_OBJS_STAT_ARMHF = $(patsubst %.o, $(OBJDIR_ARMHF)%.o, $(YAPI_OBJS_STAT) $(MBEDTSL_OBJS_STAT) ypkt_lin.o)
YAPI_OBJS_STAT_AARCH64 = $(patsubst %.o, $(OBJDIR_AARCH64)%.o, $(YAPI_OBJS_STAT) $(MBEDTSL_OBJS_STAT) ypkt_lin.o)
YAPI_OBJS_STAT_MIPS = $(patsubst %.o, $(OBJDIR_MIPS)%.o, $(YAPI_OBJS_STAT) ypkt_lin.o)
YAPI_OBJS_STAT_MIPSEL = $(patsubst %.o, $(OBJDIR_MIPSEL)%.o, $(YAPI_OBJS_STAT) ypkt_lin.o)
YAPI_OBJS_STAT_OSX  = $(patsubst %.o, $(OBJDIR_OSX)%.o, $(YAPI_OBJS_STAT) $(MBEDTSL_OBJS_STAT) ypkt_osx.o)

YAPI_OBJS_DYN_WIN32  = $(patsubst %.o, $(OBJDIR_WIN32)%.obj, $(YAPI_OBJS_DYN) $(MBEDTSL_OBJS_DYN) ypkt_win.o)
YAPI_OBJS_DYN_WIN64  = $(patsubst %.o, $(OBJDIR_WIN64)%.obj, $(YAPI_OBJS_DYN) $(MBEDTSL_OBJS_DYN) ypkt_win.o)
YAPI_OBJS_DYN_I386 = $(patsubst %.o, $(OBJDIR_I386)%.o, $(YAPI_OBJS_DYN) $(MBEDTSL_OBJS_DYN) ypkt_lin_dyn.o)
YAPI_OBJS_DYN_X64 = $(patsubst %.o, $(OBJDIR_X64)%.o, $(YAPI_OBJS_DYN) $(MBEDTSL_OBJS_DYN) ypkt_lin_dyn.o)
YAPI_OBJS_DYN_ARMEL = $(patsubst %.o, $(OBJDIR_ARMEL)%.o, $(YAPI_OBJS_DYN) ypkt_lin_dyn.o)
YAPI_OBJS_DYN_ARMHF = $(patsubst %.o, $(OBJDIR_ARMHF)%.o, $(YAPI_OBJS_DYN) $(MBEDTSL_OBJS_DYN) ypkt_lin_dyn.o)
YAPI_OBJS_DYN_AARCH64 = $(patsubst %.o, $(OBJDIR_AARCH64)%.o, $(YAPI_OBJS_DYN) $(MBEDTSL_OBJS_DYN) ypkt_lin_dyn.o)
YAPI_OBJS_DYN_MIPS = $(patsubst %.o, $(OBJDIR_MIPS)%.o, $(YAPI_OBJS_DYN) ypkt_lin_dyn.o)
YAPI_OBJS_DYN_MIPSEL = $(patsubst %.o, $(OBJDIR_MIPSEL)%.o, $(YAPI_OBJS_DYN) ypkt_lin_dyn.o)
YAPI_OBJS_DYN_OSX = $(patsubst %.o, $(OBJDIR_OSX)%.o, $(YAPI_OBJS_DYN) $(MBEDTSL_OBJS_DYN) ypkt_osx_dyn.o)
YAPI_OBJS_DYN_OSX32 = $(patsubst %.o, $(OBJDIR_OSX32)%.o, $(YAPI_OBJS_DYN) $(MBEDTSL_OBJS_DYN) ypkt_osx_dyn.o)

VPATH       = $(YAPI_SOURCES_DIR) $(SOURCES_DIR) $(MBEDTLS_SOURCES_DIR)
INCLUDES    = $(YAPI_SOURCES_DIR) $(SOURCES_DIR) $(MBEDTLS_INCLUDE_DIR)

INC_PATH    = $(patsubst %,-I%,$(subst :, ,$(INCLUDES)))
INC_PATH_WIN = $(patsubst %,/I %,$(subst :, ,$(INCLUDES)))


ALL_H       = $(wildcard $(patsubst %,%*.h,$(subst :, ,$(YAPI_SOURCES_DIR) $(SOURCES_DIR)  $(MBEDTLS_INCLUDE_DIR))))

fwd: default

# WINDOW 32bits
$(OBJDIR_WIN32)%.obj: %.c $(ALL_H)
	@$(CC_WIN32) $(OPTS_WIN32) $(OPTS_PROD_WIN) $(OPTS_STAT_WIN) $(INC_PATH_WIN) /Fo$@ /c $<

$(OBJDIR_WIN32)%.obj: %.cpp $(ALL_H)
	@$(CPP_WIN32) $(OPTS_WIN32) $(OPTS_PROD_WIN) $(OPTS_STAT_WIN) $(INC_PATH_WIN) /EHsc /Fo$@ /c $<

$(OBJDIR_WIN32)%_dyn.obj: %.c $(ALL_H)
	@$(CC_WIN32) $(OPTS_WIN32) $(OPTS_PROD_WIN) $(OPTS_DYN_WIN) $(INC_PATH_WIN) /Fo$@ /c $<

$(OBJDIR_WIN32)%_dyn.obj: %.cpp $(ALL_H)
	@$(CPP_WIN32) $(OPTS_WIN32) $(OPTS_PROD_WIN) $(OPTS_DYN_WIN) $(INC_PATH_WIN) /EHsc /Fo$@ /c $<

$(OBJDIR_WIN32)%.res: %.rc
	$(RC) /fo $@ $<

# WINDOW 64bits
$(OBJDIR_WIN64)%.obj: %.c $(ALL_H)
	@$(CC_WIN64) $(OPTS_WIN64) $(OPTS_PROD_WIN) $(OPTS_STAT_WIN) $(INC_PATH_WIN) /Fo$@ /c $<

$(OBJDIR_WIN64)%.obj: %.cpp $(ALL_H)
	@$(CPP_WIN64) $(OPTS_WIN64) $(OPTS_PROD_WIN) $(OPTS_STAT_WIN) $(INC_PATH_WIN) /EHsc /Fo$@ /c $<

$(OBJDIR_WIN64)%_dyn.obj: %.c $(ALL_H)
	@$(CC_WIN64) $(OPTS_WIN64) $(OPTS_PROD_WIN) $(OPTS_DYN_WIN) $(INC_PATH_WIN) /Fo$@ /c $<

$(OBJDIR_WIN64)%_dyn.obj: %.cpp $(ALL_H)
	@$(CPP_WIN64) $(OPTS_WIN64) $(OPTS_PROD_WIN) $(OPTS_DYN_WIN) $(INC_PATH_WIN) /EHsc /Fo$@ /c $<

$(OBJDIR_WIN64)%.res: %.rc
	$(RC) /fo $@ $<

#generic compilation rules
$(OBJDIR_I386)%.o: %.c $(ALL_H)
	@echo compiling $@
	@$(CC_I386) $(OPTS_I386) $(OPTS_PROD) $(OPTS_STAT) $(INC_PATH)  -c $< -o $@

$(OBJDIR_I386)%.o: %.cpp $(ALL_H)
	@echo compiling $@
	@$(CPP_I386) $(OPTS_I386) $(OPTS_PROD) $(OPTS_STAT) $(INC_PATH)  -c $< -o $@

$(OBJDIR_I386)%_dyn.o: %.c $(ALL_H)
	@echo compiling $@
	@$(CC_I386) $(OPTS_I386) $(OPTS_PROD) $(OPTS_DYN) $(INC_PATH)  -c $< -o $@

$(OBJDIR_I386)%_dyn.o: %.cpp $(ALL_H)
	@echo compiling $@
	@$(CPP_I386) $(OPTS_I386) $(OPTS_PROD) $(OPTS_DYN) $(INC_PATH)  -c $< -o $@

$(OBJDIR_X64)%.o: %.c $(ALL_H)
	@echo compiling $@
	@$(CC_X64) $(OPTS_X64) $(OPTS_PROD) $(OPTS_STAT) $(INC_PATH)  -c $< -o $@

$(OBJDIR_X64)%.o: %.cpp $(ALL_H)
	@echo compiling $@
	@$(CPP_X64) $(OPTS_X64) $(OPTS_PROD) $(OPTS_STAT) $(INC_PATH)  -c $< -o $@

$(OBJDIR_X64)%_dyn.o: %.c $(ALL_H)
	@echo compiling $@
	@$(CC_X64) $(OPTS_X64) $(OPTS_PROD) $(OPTS_DYN) $(INC_PATH)  -c $< -o $@

$(OBJDIR_X64)%_dyn.o: %.cpp $(ALL_H)
	@echo compiling $@
	@$(CPP_X64) $(OPTS_X64) $(OPTS_PROD) $(OPTS_DYN) $(INC_PATH)  -c $< -o $@

$(OBJDIR_ARMEL)%.o: %.c $(ALL_H)
	@echo compiling $@
	@$(CC_ARMEL) $(OPTS_ARMEL) $(OPTS_PROD) $(OPTS_STAT) $(INC_PATH)  -c $< -o $@

$(OBJDIR_ARMEL)%.o: %.cpp $(ALL_H)
	@echo compiling $@
	@$(CPP_ARMEL) $(OPTS_ARMEL) $(OPTS_PROD) $(OPTS_STAT) $(INC_PATH)  -c $< -o $@

$(OBJDIR_ARMEL)%_dyn.o: %.c $(ALL_H)
	@echo compiling $@
	@$(CC_ARMEL) $(OPTS_ARMEL) $(OPTS_PROD) $(OPTS_DYN) $(INC_PATH)  -c $< -o $@

$(OBJDIR_ARMEL)%_dyn.o: %.cpp $(ALL_H)
	@echo compiling $@
	@$(CPP_ARMEL) $(OPTS_ARMEL) $(OPTS_PROD) $(OPTS_DYN) $(INC_PATH)  -c $< -o $@

$(OBJDIR_ARMHF)%.o: %.c $(ALL_H)
	@echo compiling $@
	@$(CC_ARMHF) $(OPTS_ARMHF) $(OPTS_PROD) $(OPTS_STAT) $(INC_PATH)  -c $< -o $@

$(OBJDIR_ARMHF)%.o: %.cpp $(ALL_H)
	@echo compiling $@
	@$(CPP_ARMHF) $(OPTS_ARMHF) $(OPTS_PROD) $(OPTS_STAT) $(INC_PATH)  -c $< -o $@

$(OBJDIR_ARMHF)%_dyn.o: %.c $(ALL_H)
	@echo compiling $@
	@$(CC_ARMHF) $(OPTS_ARMHF) $(OPTS_PROD) $(OPTS_DYN) $(INC_PATH)  -c $< -o $@

$(OBJDIR_ARMHF)%_dyn.o: %.cpp $(ALL_H)
	@echo compiling $@
	@$(CPP_ARMHF) $(OPTS_ARMHF) $(OPTS_PROD) $(OPTS_DYN) $(INC_PATH)  -c $< -o $@

$(OBJDIR_AARCH64)%.o: %.c $(ALL_H)
	@echo compiling $@
	@$(CC_AARCH64) $(OPTS_AARCH64) $(OPTS_PROD) $(OPTS_STAT) $(INC_PATH)  -c $< -o $@

$(OBJDIR_AARCH64)%.o: %.cpp $(ALL_H)
	@echo compiling $@
	@$(CPP_AARCH64) $(OPTS_AARCH64) $(OPTS_PROD) $(OPTS_STAT) $(INC_PATH)  -c $< -o $@

$(OBJDIR_AARCH64)%_dyn.o: %.c $(ALL_H)
	@echo compiling $@
	@$(CC_AARCH64) $(OPTS_AARCH64) $(OPTS_PROD) $(OPTS_DYN) $(INC_PATH)  -c $< -o $@

$(OBJDIR_AARCH64)%_dyn.o: %.cpp $(ALL_H)
	@echo compiling $@
	@$(CPP_AARCH64) $(OPTS_AARCH64) $(OPTS_PROD) $(OPTS_DYN) $(INC_PATH)  -c $< -o $@

$(OBJDIR_MIPS)%.o: %.c $(ALL_H)
	@echo compiling $@
	@$(CC_MIPS) $(OPTS_MIPS) $(OPTS_PROD) $(OPTS_STAT) $(INC_PATH)  -c $< -o $@

$(OBJDIR_MIPS)%.o: %.cpp $(ALL_H)
	@echo compiling $@
	@$(CPP_MIPS) $(OPTS_MIPS) $(OPTS_PROD) $(OPTS_STAT) $(INC_PATH)  -c $< -o $@

$(OBJDIR_MIPS)%_dyn.o: %.c $(ALL_H)
	@echo compiling $@
	@$(CC_MIPS) $(OPTS_MIPS) $(OPTS_PROD) $(OPTS_DYN) $(INC_PATH)  -c $< -o $@

$(OBJDIR_MIPS)%_dyn.o: %.cpp $(ALL_H)
	@echo compiling $@
	@$(CPP_MIPS) $(OPTS_MIPS) $(OPTS_PROD) $(OPTS_DYN) $(INC_PATH)  -c $< -o $@

$(OBJDIR_MIPSEL)%.o: %.c $(ALL_H)
	@echo compiling $@
	@$(CC_MIPSEL) $(OPTS_MIPSEL) $(OPTS_PROD) $(OPTS_STAT) $(INC_PATH)  -c $< -o $@

$(OBJDIR_MIPSEL)%.o: %.cpp $(ALL_H)
	@echo compiling $@
	@$(CPP_MIPSEL) $(OPTS_MIPSEL) $(OPTS_PROD) $(OPTS_STAT) $(INC_PATH)  -c $< -o $@

$(OBJDIR_MIPSEL)%_dyn.o: %.c $(ALL_H)
	@echo compiling $@
	@$(CC_MIPSEL) $(OPTS_MIPSEL) $(OPTS_PROD) $(OPTS_DYN) $(INC_PATH)  -c $< -o $@

$(OBJDIR_MIPSEL)%_dyn.o: %.cpp $(ALL_H)
	@echo compiling $@
	@$(CPP_MIPSEL) $(OPTS_MIPSEL) $(OPTS_PROD) $(OPTS_DYN) $(INC_PATH)  -c $< -o $@

$(OBJDIR_OSX)%.o: %.c $(ALL_H)
	@echo compiling $@
	@$(CC_OSX) $(OPTS_OSX) $(OPTS_PROD) $(OPTS_STAT) $(INC_PATH)  -c $< -o $@

$(OBJDIR_OSX)%.o: %.cpp $(ALL_H)
	@echo compiling $@
	@$(CPP_OSX) $(OPTS_OSX) $(OPTS_PROD) $(OPTS_STAT) $(INC_PATH)  -c $< -o $@

$(OBJDIR_OSX)%_dyn.o: %.c $(ALL_H)
	@echo compiling $@
	@$(CC_OSX) $(OPTS_OSX) $(OPTS_PROD) $(OPTS_DYN) $(INC_PATH)  -c $< -o $@

$(OBJDIR_OSX)%_dyn.o: %.cpp $(ALL_H)
	@echo compiling $@
	@$(CPP_OSX) $(OPTS_OSX) $(OPTS_PROD) $(OPTS_DYN) $(INC_PATH)  -c $< -o $@

$(OBJDIR_OSX32)%_dyn.o: %.c $(ALL_H)
	@echo compiling $@
	@$(CC_OSX32) $(OPTS_OSX32) $(OPTS_PROD) $(OPTS_DYN) $(INC_PATH)  -c $< -o $@

# WINDOW 32 LIBRARY RULES
$(DIR_WIN32)yocto-static.lib: $(OBJDIR_WIN32) $(ALL_OBJS_STAT_WIN32) $(ALL_H)
	@$(RM) -rf $@
	@echo building lib ($@)
	@lib /OUT:$@ $(ALL_OBJS_STAT_WIN32)

$(YAPI_DIR_WIN32)yapi-static.lib: $(OBJDIR_WIN32) $(YAPI_DIR_WIN32) $(YAPI_OBJS_STAT_WIN32) $(ALL_H)
	@$(RM) -rf $@
	@echo building lib ($@)
	@lib /OUT:$@ $(YAPI_OBJS_STAT_WIN32)

$(DIR_WIN32)yocto.dll: $(OBJDIR_WIN32) $(ALL_OBJS_DYN_WIN32) $(OBJDIR_WIN32)winresource.res  $(ALL_H)
	@echo building dll $@
	@$(CPP_WIN32) $(ALL_OBJS_DYN_WIN32) $(OBJDIR_WIN32)winresource.res OleAut32.lib /nologo /Fe$@ /Fa(R) /link /DLL $(LINKWITH_WIN32)

$(YAPI_DIR_WIN32)yapi.dll: $(OBJDIR_WIN32) $(YAPI_DIR_WIN32) $(YAPI_OBJS_DYN_WIN32) $(OBJDIR_WIN32)yapiresource.res $(ALL_H)
	@echo building dll $@
ifdef JAVA_HOME
	@echo java_home set to $(JAVA_HOME)
endif
	@$(CC_WIN32) $(YAPI_OBJS_DYN_WIN32) $(OBJDIR_WIN32)yapiresource.res /W3 /WX- /O2 /Oi /Oy- /MT /Gy /GL /nologo /Fe$@ /Fa(R) /link /DLL $(LINKWITH_WIN32) /DEF:$(YAPI_SOURCES_DIR)yapi.def

$(DIR_WIN32)test.exe: $(OBJDIR_WIN32) $(YAPI_DIR_WIN32) $(ALL_OBJS_STAT_WIN32) $(ALL_H) $(TEST_SRC)
	@$(RM) -rf $@
	@echo linking $@
	@echo $(ARCH)
	@$(CPP_WIN32) $(OPTS_WIN32) $(INC_PATH_WIN) /EHsc /Fe$@ $(TEST_SRC) $(ALL_OBJS_STAT_WIN32)  /link  $(LINKWITH_WIN32)


# WINDOW 64 LIBRARY RULES
$(DIR_WIN64)yocto-static.lib: $(OBJDIR_WIN64) $(ALL_OBJS_STAT_WIN64) $(ALL_H)
	@$(RM) -rf $@
	@echo building lib ($@)
	@lib /OUT:$@ $(ALL_OBJS_STAT_WIN64)

$(YAPI_DIR_WIN64)yapi-static.lib: $(OBJDIR_WIN64) $(YAPI_DIR_WIN64) $(YAPI_OBJS_STAT_WIN64) $(ALL_H)
	@$(RM) -rf $@
	@echo building lib ($@)
	@lib /OUT:$@ $(YAPI_OBJS_STAT_WIN64)

$(DIR_WIN64)yocto.dll: $(OBJDIR_WIN64) $(ALL_OBJS_DYN_WIN64) $(OBJDIR_WIN64)winresource64.res $(ALL_H)
	@echo building dll $@
	@$(CPP_WIN64) $(ALL_OBJS_DYN_WIN64) $(OBJDIR_WIN64)winresource64.res /nologo /Fe$@ /Fa(R) /link  /MACHINE:X64  /DLL  $(LINKWITH_WIN64)

$(YAPI_DIR_WIN64)yapi.dll: $(OBJDIR_WIN64) $(YAPI_DIR_WIN64) $(YAPI_OBJS_DYN_WIN64) $(OBJDIR_WIN64)yapiresource64.res  $(ALL_H)
	@echo building dll $@
ifdef JAVA_HOME
	@echo java_home set to $(JAVA_HOME)
endif
	@$(CC_WIN64) $(YAPI_OBJS_DYN_WIN64) $(OBJDIR_WIN64)yapiresource64.res /W3 /WX- /O2 /Oi /Oy- /MT /Gy /GL /nologo /Fe$@ /Fa(R) /link  /MACHINE:X64  /DLL $(LINKWITH_WIN64)

$(DIR_WIN64)test.exe: $(OBJDIR_WIN64) $(YAPI_DIR_WIN64) $(ALL_OBJS_STAT_WIN64) $(ALL_H) $(TEST_SRC)
	@$(RM) -rf $@
	@echo linking $@
	@$(CPP_WIN64) $(OPTS_WIN64) $(INC_PATH_WIN)  /Fe$@ $(TEST_SRC) $(ALL_OBJS_STAT_WIN64) $(LINKWITH_WIN64)


# INTEL 386 LIBRARY RULES
$(DIR_I386)libyocto-static.a: $(OBJDIR_I386) $(ALL_OBJS_STAT_I386) $(ALL_H)
	@$(RM) -rf $@
	@echo linking $@
	@ar rcs $@  $(ALL_OBJS_STAT_I386)

$(YAPI_DIR_I386)libyapi-static.a: $(OBJDIR_I386) $(YAPI_DIR_I386) $(YAPI_OBJS_STAT_I386) $(ALL_H)
	@$(RM) -rf $@
	@echo linking $@
	@ar rcs $@  $(YAPI_OBJS_STAT_I386)

$(DIR_I386)libyocto.so.1.0.1: $(OBJDIR_I386) $(ALL_OBJS_DYN_I386) $(ALL_H)
	@echo linking $@
	@$(CPP_I386) $(OPTS_I386) -shared -Wl,-soname,$(@F) -o $@ $(ALL_OBJS_DYN_I386) $(LINKWITH_I386)

$(YAPI_DIR_I386)libyapi.so.1.0.1: $(OBJDIR_I386) $(YAPI_DIR_I386) $(YAPI_OBJS_DYN_I386) $(ALL_H)
	@echo linking $@
ifdef JAVA_HOME
	@echo java_home set to $(JAVA_HOME)
endif
	@$(CC_I386) $(OPTS_I386) -shared -Wl,-soname,$(@F) -o $@ $(YAPI_OBJS_DYN_I386) $(YAPI_LINKWITH_I386)
	@$(STRIP_I386) $@

$(DIR_I386)test: $(OBJDIR_I386) $(YAPI_DIR_I386) $(ALL_OBJS_STAT_I386) $(ALL_H) $(TEST_SRC)
	@$(RM) -rf $@
	@echo linking $@
	@$(CPP_I386) $(OPTS_I386) $(INC_PATH) -o $@ $(TEST_SRC) $(ALL_OBJS_STAT_I386) $(LINKWITH_I386)

# INTEL X64 LIBRARY RULES
$(DIR_X64)libyocto-static.a: $(OBJDIR_X64) $(ALL_OBJS_STAT_X64) $(ALL_H)
	@$(RM) -rf $@
	@echo linking $@
	@ar rcs $@  $(ALL_OBJS_STAT_X64)

$(YAPI_DIR_X64)libyapi-static.a: $(OBJDIR_X64) $(YAPI_DIR_X64) $(YAPI_OBJS_STAT_X64) $(ALL_H)
	@$(RM) -rf $@
	@echo linking $@
	@ar rcs $@  $(YAPI_OBJS_STAT_X64)

$(DIR_X64)libyocto.so.1.0.1: $(OBJDIR_X64) $(ALL_OBJS_DYN_X64) $(ALL_H)
	@echo linking $@
	@$(CPP_X64) $(OPTS_X64) -shared -Wl,-soname,$(@F) -o $@ $(ALL_OBJS_DYN_X64) $(LINKWITH_X64)

$(YAPI_DIR_X64)libyapi.so.1.0.1: $(OBJDIR_X64) $(YAPI_DIR_X64) $(YAPI_OBJS_DYN_X64) $(ALL_H)
	@echo linking $@
ifdef JAVA_HOME
	@echo java_home set to $(JAVA_HOME)
endif
	@$(CC_X64) $(OPTS_X64) -shared -Wl,-soname,$(@F) -o $@ $(YAPI_OBJS_DYN_X64) $(YAPI_LINKWITH_X64)
	@$(STRIP_X64) $@

$(DIR_X64)test: $(OBJDIR_X64) $(YAPI_DIR_X64) $(ALL_OBJS_STAT_X64) $(ALL_H) $(TEST_SRC)
	@$(RM) -rf $@
	@echo linking $@
	@$(CPP_X64) $(OPTS_X64) $(INC_PATH) -o $@ $(TEST_SRC) $(ALL_OBJS_STAT_X64) $(LINKWITH_X64)

# ARM SOFT FLOAT LIBRARY RULES
$(DIR_ARMEL)libyocto-static.a: $(OBJDIR_ARMEL) $(ALL_OBJS_STAT_ARMEL) $(ALL_H)
	@$(RM) -rf $@
	@echo linking $@
	@ar rcs $@  $(ALL_OBJS_STAT_ARMEL)

$(YAPI_DIR_ARMEL)libyapi-static.a: $(OBJDIR_ARMEL) $(YAPI_DIR_ARMEL) $(YAPI_OBJS_STAT_ARMEL) $(ALL_H)
	@$(RM) -rf $@
	@echo linking $@
	@ar rcs $@  $(YAPI_OBJS_STAT_ARMEL)

$(DIR_ARMEL)libyocto.so.1.0.1: $(OBJDIR_ARMEL) $(ALL_OBJS_DYN_ARMEL) $(ALL_H)
	@echo linking $@
	@$(CPP_ARMEL) $(OPTS_ARMEL) -shared -Wl,-soname,$(@F) -o $@ $(ALL_OBJS_DYN_ARMEL) $(LINKWITH_ARMEL)

$(YAPI_DIR_ARMEL)libyapi.so.1.0.1: $(OBJDIR_ARMEL) $(YAPI_DIR_ARMEL) $(YAPI_OBJS_DYN_ARMEL) $(ALL_H)
	@echo linking $@
ifdef JAVA_HOME
	@echo java_home set to $(JAVA_HOME)
endif
	@$(CC_ARMEL) $(OPTS_ARMEL) -shared -Wl,-soname,$(@F) -o $@ $(YAPI_OBJS_DYN_ARMEL) $(YAPI_LINKWITH_ARMEL)

$(DIR_ARMEL)test: $(OBJDIR_ARMEL) $(YAPI_DIR_ARMEL) $(ALL_OBJS_STAT_ARMEL) $(ALL_H) $(TEST_SRC)
	@$(RM) -rf $@
	@echo linking $@
	@$(CPP_ARMEL) $(OPTS_ARMEL) $(INC_PATH) -o $@ $(TEST_SRC) $(ALL_OBJS_STAT_ARMEL) $(LINKWITH_ARMEL)

# ARM HARD FLOAT LIBRARY RULES
$(DIR_ARMHF)libyocto-static.a: $(OBJDIR_ARMHF) $(ALL_OBJS_STAT_ARMHF) $(ALL_H)
	@$(RM) -rf $@
	@echo linking $@
	@ar rcs $@  $(ALL_OBJS_STAT_ARMHF)

$(YAPI_DIR_ARMHF)libyapi-static.a: $(OBJDIR_ARMHF) $(YAPI_DIR_ARMHF) $(YAPI_OBJS_STAT_ARMHF) $(ALL_H)
	@$(RM) -rf $@
	@echo linking $@
	@ar rcs $@  $(YAPI_OBJS_STAT_ARMHF)

$(DIR_ARMHF)libyocto.so.1.0.1: $(OBJDIR_ARMHF) $(ALL_OBJS_DYN_ARMHF) $(ALL_H)
	@echo linking $@
	@$(CPP_ARMHF) $(OPTS_ARMHF) -shared -Wl,-soname,$(@F) -o $@ $(ALL_OBJS_DYN_ARMHF) $(LINKWITH_ARMHF)

$(YAPI_DIR_ARMHF)libyapi.so.1.0.1: $(OBJDIR_ARMHF) $(YAPI_DIR_ARMHF) $(YAPI_OBJS_DYN_ARMHF) $(ALL_H)
	@echo linking $@
ifdef JAVA_HOME
	@echo java_home set to $(JAVA_HOME)
endif
	@$(CC_ARMHF) $(OPTS_ARMHF) -shared -Wl,-soname,$(@F) -o $@ $(YAPI_OBJS_DYN_ARMHF) $(YAPI_LINKWITH_ARMHF)
	@$(STRIP_ARMHF) $@

$(DIR_ARMHF)test: $(OBJDIR_ARMHF) $(YAPI_DIR_ARMHF) $(ALL_OBJS_STAT_ARMHF) $(ALL_H) $(TEST_SRC)
	@$(RM) -rf $@
	@echo linking $@
	@$(CPP_ARMHF) $(OPTS_ARMHF) $(INC_PATH) -o $@ $(TEST_SRC) $(ALL_OBJS_STAT_ARMHF) $(LINKWITH_ARMHF)

# AARCH64 LIBRARY RULES
$(DIR_AARCH64)libyocto-static.a: $(OBJDIR_AARCH64) $(ALL_OBJS_STAT_AARCH64) $(ALL_H)
	@$(RM) -rf $@
	@echo linking $@
	@ar rcs $@  $(ALL_OBJS_STAT_AARCH64)

$(YAPI_DIR_AARCH64)libyapi-static.a: $(OBJDIR_AARCH64) $(YAPI_DIR_AARCH64) $(YAPI_OBJS_STAT_AARCH64) $(ALL_H)
	@$(RM) -rf $@
	@echo linking $@
	@ar rcs $@  $(YAPI_OBJS_STAT_AARCH64)

$(DIR_AARCH64)libyocto.so.1.0.1: $(OBJDIR_AARCH64) $(ALL_OBJS_DYN_AARCH64) $(ALL_H)
	@echo linking $@
	@$(CPP_AARCH64) $(OPTS_AARCH64) -shared -Wl,-soname,$(@F) -o $@ $(ALL_OBJS_DYN_AARCH64) $(LINKWITH_AARCH64)

$(YAPI_DIR_AARCH64)libyapi.so.1.0.1: $(OBJDIR_AARCH64) $(YAPI_DIR_AARCH64) $(YAPI_OBJS_DYN_AARCH64) $(ALL_H)
	@echo linking $@
ifdef JAVA_HOME
	@echo java_home set to $(JAVA_HOME)
endif
	@$(CC_AARCH64) $(OPTS_AARCH64) -shared -Wl,-soname,$(@F) -o $@ $(YAPI_OBJS_DYN_AARCH64) $(YAPI_LINKWITH_AARCH64)
	@$(STRIP_AARCH64) $@

$(DIR_AARCH64)test: $(OBJDIR_AARCH64) $(YAPI_DIR_AARCH64) $(ALL_OBJS_STAT_AARCH64) $(ALL_H) $(TEST_SRC)
	@$(RM) -rf $@
	@echo linking $@
	@$(CPP_AARCH64) $(OPTS_AARCH64) $(INC_PATH) -o $@ $(TEST_SRC) $(ALL_OBJS_STAT_AARCH64) $(LINKWITH_AARCH64)

# MIPS LIBRARY RULES
$(DIR_MIPS)libyocto-static.a: $(OBJDIR_MIPS) $(ALL_OBJS_STAT_MIPS) $(ALL_H)
	@$(RM) -rf $@
	@echo linking $@
	@ar rcs $@  $(ALL_OBJS_STAT_MIPS)

$(YAPI_DIR_MIPS)libyapi-static.a: $(OBJDIR_MIPS) $(YAPI_DIR_MIPS) $(YAPI_OBJS_STAT_MIPS) $(ALL_H)
	@$(RM) -rf $@
	@echo linking $@
	@ar rcs $@  $(YAPI_OBJS_STAT_MIPS)

$(DIR_MIPS)libyocto.so.1.0.1: $(OBJDIR_MIPS) $(ALL_OBJS_DYN_MIPS) $(ALL_H)
	@echo linking $@
	@$(CPP_MIPS) $(OPTS_MIPS) -shared -Wl,-soname,$(@F) -o $@ $(ALL_OBJS_DYN_MIPS) $(LINKWITH_MIPS)

$(YAPI_DIR_MIPS)libyapi.so.1.0.1: $(OBJDIR_MIPS) $(YAPI_DIR_MIPS) $(YAPI_OBJS_DYN_MIPS) $(ALL_H)
	@echo linking $@
ifdef JAVA_HOME
	@echo java_home set to $(JAVA_HOME)
endif
	@$(CC_MIPS) $(OPTS_MIPS) -shared -Wl,-soname,$(@F) -o $@ $(YAPI_OBJS_DYN_MIPS) $(YAPI_LINKWITH_MIPS)

$(DIR_MIPS)test: $(OBJDIR_MIPS) $(YAPI_DIR_MIPS) $ALL_OBJS_STAT_MIPS) $(ALL_H) $(TEST_SRC)
	@$(RM) -rf $@
	@echo linking $@
	@$(CPP_MIPS) $(OPTS_MIPS) $(INC_PATH) -o $@ $(TEST_SRC) $(ALL_OBJS_STAT_MIPS) $(LINKWITH_MIPS)

# MIPSEL LIBRARY RULES
$(DIR_MIPSEL)libyocto-static.a: $(OBJDIR_MIPSEL) $(ALL_OBJS_STAT_MIPSEL) $(ALL_H)
	@$(RM) -rf $@
	@echo linking $@
	@ar rcs $@  $(ALL_OBJS_STAT_MIPSEL)

$(YAPI_DIR_MIPSEL)libyapi-static.a: $(OBJDIR_MIPSEL) $(YAPI_DIR_MIPSEL) $(YAPI_OBJS_STAT_MIPSEL) $(ALL_H)
	@$(RM) -rf $@
	@echo linking $@
	@ar rcs $@  $(YAPI_OBJS_STAT_MIPSEL)

$(DIR_MIPSEL)libyocto.so.1.0.1: $(OBJDIR_MIPSEL) $(ALL_OBJS_DYN_MIPSEL) $(ALL_H)
	@echo linking $@
	@$(CPP_MIPSEL) $(OPTS_MIPSEL) -shared -Wl,-soname,$(@F) -o $@ $(ALL_OBJS_DYN_MIPSEL) $(LINKWITH_MIPSEL)

$(YAPI_DIR_MIPSEL)libyapi.so.1.0.1: $(OBJDIR_MIPSEL) $(YAPI_DIR_MIPSEL) $(YAPI_OBJS_DYN_MIPSEL) $(ALL_H)
	@echo linking $@
ifdef JAVA_HOME
	@echo java_home set to $(JAVA_HOME)
endif
	@$(CC_MIPSEL) $(OPTS_MIPSEL) -shared -Wl,-soname,$(@F) -o $@ $(YAPI_OBJS_DYN_MIPSEL) $(YAPI_LINKWITH_MIPSEL)

$(DIR_MIPSEL)test: $(OBJDIR_MIPSEL) $(YAPI_DIR_MIPSEL) $ALL_OBJS_STAT_MIPSEL) $(ALL_H) $(TEST_SRC)
	@$(RM) -rf $@
	@echo linking $@
	@$(CPP_MIPSEL) $(OPTS_MIPSEL) $(INC_PATH) -o $@ $(TEST_SRC) $(ALL_OBJS_STAT_MIPSEL) $(LINKWITH_MIPSEL)

# OSX LIBRARY RULES
$(DIR_OSX)libyocto-static.a: $(OBJDIR_OSX) $(ALL_OBJS_STAT_OSX) $(ALL_H)
	@$(RM) -rf $@
	@echo linking $@
	@ar rcs $@  $(ALL_OBJS_STAT_OSX)

$(YAPI_DIR_OSX)libyapi-static.a: $(OBJDIR_OSX) $(YAPI_DIR_OSX) $(YAPI_OBJS_STAT_OSX) $(ALL_H)
	@$(RM) -rf $@
	@echo linking $@
	@ar rcs $@  $(YAPI_OBJS_STAT_OSX)

$(DIR_OSX)libyocto.dylib: $(DIR_OSX) $(ALL_OBJS_DYN_OSX) $(ALL_H)
	@echo linking $@
	@$(CPP_OSX)  -m64 -dynamiclib -Wl,-reexport_framework,IOKit,-reexport_framework,CoreFoundation,-headerpad_max_install_names,-compatibility_version,1.0,-current_version,1.0,-install_name,/usr/local/lib/libyocto.dylib -o $@ $(ALL_OBJS_DYN_OSX)

OSX_DYLIB_AS = xcode/yapi/build/Release/libyapi.dylib

.PHONY: $(OSX_DYLIB_AS)
$(OSX_DYLIB_AS):
ifdef JAVA_HOME
	@echo java_home set to $(JAVA_HOME)
endif
	@echo "build yapi.dylib from xcode project (phony)"
	xcodebuild -project xcode/yapi/yapi.xcodeproj

$(YAPI_DIR_OSX)libyapi.dylib: $(YAPI_DIR_OSX) $(OSX_DYLIB_AS)
	cp -R $(OSX_DYLIB_AS) $(YAPI_DIR_OSX)

$(DIR_OSX)test: $(OBJDIR_OSX) $(YAPI_DIR_OSX) ALL_OBJS_STAT_OSX) $(ALL_H) $(TEST_SRC)
	@$(RM) -rf $@
	@echo linking $@
	@$(CPP_OSX) $(OPTS_OSX) $(INC_PATH) -o $@ $(TEST_SRC) $(ALL_OBJS_STAT_OSX) $(LINKWITH_OSX)

$(YAPI_DIR_OSX32)libyapi.dylib : OPTS_DYN = $(OPTS_JAVA)
$(YAPI_DIR_OSX32)libyapi.dylib: $(OBJDIR_OSX32) $(YAPI_DIR_OSX32) $(YAPI_OBJS_DYN_OSX32) $(ALL_H)
	@echo linking $@
ifdef JAVA_HOME
	@echo java_home set to $(JAVA_HOME)
endif
	@$(CC_OSX32) -m32 -dynamiclib -Wl,-reexport_framework,IOKit,-reexport_framework,CoreFoundation,-headerpad_max_install_names,-compatibility_version,1.0,-current_version,1.0,-install_name,/usr/local/lib/libyapi.dylib -o $@ $(YAPI_OBJS_DYN_OSX32)

win32: $(LIBS_WIN32)

release_win32: $(LIBS_WIN32)
	@$(MAKE) clean_obj_win32

win64: $(LIBS_WIN64)

release_win64: $(LIBS_WIN64)
	@$(MAKE) clean_obj_win64

i386: $(LIBS_I386)

release_i386: $(LIBS_I386)
	@$(MAKE) clean_obj_i386

x64: $(LIBS_X64)

release_x64: $(LIBS_X64)
	@$(MAKE) clean_obj_x64

armel: $(LIBS_ARMEL)

release_armel: $(LIBS_ARMEL)
	@$(MAKE) clean_obj_armel

yapi_armel: $(LIBS_YAPI_ARMEL)

armhf: $(LIBS_ARMHF)

release_armhf: $(LIBS_ARMHF)
	@$(MAKE) clean_obj_armhf

yapi_armhf: $(LIBS_YAPI_ARMHF)

aarch64: $(LIBS_AARCH64)

release_aarch64: $(LIBS_AARCH64)
	@$(MAKE) clean_obj_aarch64

yapi_aarch64: $(LIBS_YAPI_AARCH64)

mips : export STAGING_DIR = $(STAGING_MIPS)
mips: $(LIBS_MIPS)

release_mips : export STAGING_DIR = $(STAGING_MIPS)
release_mips: $(LIBS_MIPS)
	@$(MAKE) clean_obj_mips

mipsel : export STAGING_DIR = $(STAGING_MIPSEL)
mipsel: $(LIBS_MIPSEL)

release_mipsel : export STAGING_DIR = $(STAGING_MIPSEL)
release_mipsel: $(LIBS_MIPSEL)
	@$(MAKE) clean_obj_mipsel

$(YAPI_DIR_OSX)/yapi.bundle:
	xcodebuild -project xcode/yapi/yapi.xcodeproj
	cp -R xcode/yapi/build/Release/* $(YAPI_DIR_OSX)

clean_bundle:
	rm -rf xcode/yapi/build

osx: $(LIBS_OSX) $(YAPI_DIR_OSX)/yapi.bundle

release_osx32: $(LIBS_OSX32)

release_osx: $(LIBS_OSX) $(YAPI_DIR_OSX)/yapi.bundle  clean_obj_osx clean_bundle

ifeq ($(UNAME), Linux)
#linux
ifeq ($(ARCH), x86_64)
default: x64

release: release_x64 release_i386 release_mips

else
ifeq ($(ARCH),i386)
default: i386

release: release_i386

else
ifeq ($(ARCH), aarch64)
default: aarch64

release: release_aarch64

else

#$(warning $(ARM_BUILD_TYPE))
ifeq ($(ARM_BUILD_TYPE), hf)
default: armhf

release: release_armhf

else
default: armel

release: release_armel

endif
endif
endif
endif
else

ifeq ($(OS),Windows_NT)
#windows
default: win32

release: release_win32


test: $(DIR_WIN32)test.exe

else
#mac os
default: osx

release: release_osx
endif
endif

libyapi-i386.so : OPTS_DYN = $(OPTS_JAVA)
libyapi-i386.so: $(YAPI_DIR_I386)libyapi.so.1.0.1
	cp $< $@

libyapi-amd64.so : OPTS_DYN = $(OPTS_JAVA)
libyapi-amd64.so: $(YAPI_DIR_X64)libyapi.so.1.0.1
	cp $< $@

libyapi-armel.so : OPTS_DYN = $(OPTS_JAVA)
libyapi-armel.so: $(YAPI_DIR_ARMEL)libyapi.so.1.0.1
	cp $< $@

libyapi-armhf.so : OPTS_DYN = $(OPTS_JAVA)
libyapi-armhf.so:  $(YAPI_DIR_ARMHF)libyapi.so.1.0.1
	cp $< $@

libyapi-aarch64.so : OPTS_DYN = $(OPTS_JAVA)
libyapi-aarch64.so: $(YAPI_DIR_AARCH64)libyapi.so.1.0.1
	cp $< $@


check_java :
ifdef JAVA_HOME
	@echo java_home set to $(JAVA_HOME)
else
	@echo "JAVA_HOME IS NOT DEFINED" 1>&2
	exit 1
endif

$(DIR_WIN32) $(DIR_WIN64) $(DIR_I386) $(DIR_X64) $(DIR_ARMEL) $(DIR_ARMHF) $(DIR_AARCH64) $(DIR_MIPS) $(DIR_MIPSEL) $(DIR_OSX) $(DIR_OSX32) \
$(YAPI_DIR_WIN32) $(YAPI_DIR_WIN64) $(YAPI_DIR_I386) $(YAPI_DIR_X64) $(YAPI_DIR_ARMEL) $(YAPI_DIR_ARMHF) $(YAPI_DIR_AARCH64) $(YAPI_DIR_MIPS) $(YAPI_DIR_MIPSEL) $(YAPI_DIR_OSX) $(YAPI_DIR_OSX32) \
$(OBJDIR_WIN32) $(OBJDIR_WIN64) $(OBJDIR_I386) $(OBJDIR_X64) $(OBJDIR_ARMEL) $(OBJDIR_ARMHF) $(OBJDIR_AARCH64) $(OBJDIR_MIPS) $(OBJDIR_MIPSEL) $(OBJDIR_OSX) $(OBJDIR_OSX32) :
	@$(MKDIR) -p $@

clean_obj_win32:
	@$(RM) -rf $(OBJDIR_WIN32)

clean_obj_win64:
	@$(RM) -rf $(OBJDIR_WIN64)

clean_obj_i386:
	@$(RM) -rf $(OBJDIR_I386)

clean_obj_x64:
	@$(RM) -rf $(OBJDIR_X64)

clean_obj_armel:
	@$(RM) -rf $(OBJDIR_ARMEL)

clean_obj_armhf:
	@$(RM) -rf $(OBJDIR_ARMHF)

clean_obj_aarch64:
	@$(RM) -rf $(OBJDIR_AARCH64)

clean_obj_mips:
	@$(RM) -rf $(OBJDIR_MIPS)

clean_obj_mipsel:
	@$(RM) -rf $(OBJDIR_MIPSEL)

clean_obj_osx:
	@$(RM) -rf $(OBJDIR_OSX)

clean_obj_osx32:
	@$(RM) -rf $(OBJDIR_OSX)

clean_i386: clean_obj_i386
	@$(RM) -rf $(DIR_I386)

clean_x64: clean_obj_x64
	@$(RM) -rf $(DIR_X64)

clean_win32: clean_obj_win32
	@$(RM) -rf $(DIR_WIN32)

clean_win64: clean_obj_win64
	@$(RM) -rf $(DIR_WIN64)

clean_armel: clean_obj_armel
	@$(RM) -rf $(DIR_ARMEL)

clean_armhf: clean_obj_armhf
	@$(RM) -rf $(DIR_ARMHF)

clean_aarch64: clean_obj_aarch64
	@$(RM) -rf $(DIR_AARCH64)

clean_mips: clean_obj_mips
	@$(RM) -rf $(DIR_MIPS)

clean_mipsel: clean_obj_mipsel
	@$(RM) -rf $(DIR_MIPSEL)

clean_osx: clean_obj_osx
	@$(RM) -rf $(DIR_OSX)

clean_osx32: clean_obj_osx32
	@$(RM) -rf $(DIR_OSX32)

clean clean_all: clean_win32 clean_win64 clean_i386 clean_x64 clean_armel clean_armhf clean_mips clean_mipsel clean_osx clean_osx32 clean_aarch64
	echo clean

