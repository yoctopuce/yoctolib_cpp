# *********************************************************************
#
#  $Id: httpsupport.h 2196 2011-07-28 15:58:38Z seb $
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

UNAME := $(shell uname)
ARCH  := $(shell uname -m| sed -e s/i.86/i386/ -e s/arm.*/arm/)

#ALL directory definitions
OUTDIR_32 		= linux/32bits/
OUTDIR_64 		= linux/64bits/
OUTDIR_ARMEL 	= linux/armel/
OUTDIR_ARMHF 	= linux/armhf/
OUTDIR_OSX 		= osx/
OBJDIR_32 		= $(OUTDIR_32)obj/
OBJDIR_64 		= $(OUTDIR_64)obj/
OBJDIR_ARMEL 	= $(OUTDIR_ARMEL)obj/
OBJDIR_ARMHF 	= $(OUTDIR_ARMHF)obj/
OBJDIR_OSX 		= $(OUTDIR_OSX)obj/
YAPIOUT_32 		= $(OUTDIR_32)yapi/
YAPIOUT_64 		= $(OUTDIR_64)yapi/
YAPIOUT_ARMEL 	= $(OUTDIR_ARMEL)yapi/
YAPIOUT_ARMHF 	= $(OUTDIR_ARMHF)yapi/
YAPIOUT_OSX 	= $(OUTDIR_OSX)yapi/

#define all tragets
TARGETS_32 		= $(OUTDIR_32)libyocto-static.a $(YAPIOUT_32)libyapi-static.a $(OUTDIR_32)libyocto.so.1.0.1 $(YAPIOUT_32)libyapi.so.1.0.1
TARGETS_64 		= $(OUTDIR_64)libyocto-static.a $(YAPIOUT_64)libyapi-static.a $(OUTDIR_64)libyocto.so.1.0.1 $(YAPIOUT_64)libyapi.so.1.0.1
TARGETS_ARMEL 	= $(OUTDIR_ARMEL)libyocto-static.a $(YAPIOUT_ARMEL)libyapi-static.a $(OUTDIR_ARMEL)libyocto.so.1.0.1 $(YAPIOUT_ARMEL)libyapi.so.1.0.1
TARGETS_ARMHF 	= $(OUTDIR_ARMHF)libyocto-static.a $(YAPIOUT_ARMHF)libyapi-static.a $(OUTDIR_ARMHF)libyocto.so.1.0.1 $(YAPIOUT_ARMHF)libyapi.so.1.0.1
TARGETS_OSX 	= $(OUTDIR_OSX)libyocto-static.a  $(YAPIOUT_OSX)libyapi-static.a $(OUTDIR_OSX)libyocto.dylib $(YAPIOUT_OSX)libyapi.dylib



#define all object needed 
YOCTO_API_DIR = ../Sources/

YOCTO_API_OBJS_STAT = yocto_api.o yocto_anbutton.o yocto_carbondioxide.o yocto_colorled.o yocto_current.o yocto_datalogger.o yocto_display.o \
	yocto_dualpower.o yocto_files.o yocto_genericsensor.o yocto_hubport.o yocto_humidity.o yocto_led.o yocto_lightsensor.o \
	yocto_network.o yocto_oscontrol.o yocto_power.o yocto_pressure.o yocto_realtimeclock.o yocto_relay.o yocto_servo.o \
	yocto_temperature.o yocto_voc.o yocto_voltage.o yocto_wakeupmonitor.o yocto_wakeupschedule.o yocto_watchdog.o \
	yocto_wireless.o yocto_digitalio.o
YOCTO_API_OBJS_DYN	= $(patsubst %.o, %_dyn.o,$(YOCTO_API_OBJS_STAT))

YAPI_DIR    = $(YOCTO_API_DIR)yapi/

YAPI_OBJS_STAT = yapi.o ystream.o yprog.o ypkt_lin.o ypkt_win.o ypkt_osx.o yfifo.o ykey.o yhash.o yjson.o ytcp.o ymemory.o ythread.o
YAPI_OBJS_DYN	= $(patsubst %.o, %_dyn.o,$(YAPI_OBJS_STAT))

VPATH 		= $(YAPI_DIR) $(YOCTO_API_DIR)
ALL_H		= $(wildcard $(patsubst %,%*.h,$(subst :, ,$(YAPI_DIR) $(YOCTO_API_DIR))))

ALL_OBJS_STAT	= $(YAPI_OBJS_STAT) $(YOCTO_API_OBJS_STAT)
ALL_OBJS_DYN	= $(YAPI_OBJS_DYN)  $(YOCTO_API_OBJS_DYN)

#create a new definitions for each platform using macro
ALL_OBJS_STAT_32    = $(patsubst %.o, $(OBJDIR_32)%.o, $(ALL_OBJS_STAT))
ALL_OBJS_STAT_64    = $(patsubst %.o, $(OBJDIR_64)%.o, $(ALL_OBJS_STAT))
ALL_OBJS_STAT_ARMEL = $(patsubst %.o, $(OBJDIR_ARMEL)%.o, $(ALL_OBJS_STAT))
ALL_OBJS_STAT_ARMHF = $(patsubst %.o, $(OBJDIR_ARMHF)%.o, $(ALL_OBJS_STAT))
ALL_OBJS_STAT_OSX   = $(patsubst %.o, $(OBJDIR_OSX)%.o, $(ALL_OBJS_STAT))
ALL_OBJS_DYN_32     = $(patsubst %.o, $(OBJDIR_32)%.o, $(ALL_OBJS_DYN))
ALL_OBJS_DYN_64     = $(patsubst %.o, $(OBJDIR_64)%.o, $(ALL_OBJS_DYN))
ALL_OBJS_DYN_ARMEL  = $(patsubst %.o, $(OBJDIR_ARMEL)%.o, $(ALL_OBJS_DYN))
ALL_OBJS_DYN_ARMHF  = $(patsubst %.o, $(OBJDIR_ARMHF)%.o, $(ALL_OBJS_DYN))
ALL_OBJS_DYN_OSX    = $(patsubst %.o, $(OBJDIR_OSX)%.o, $(ALL_OBJS_DYN))

YAPI_OBJS_STAT_32  = $(patsubst %.o, $(OBJDIR_32)%.o, $(YAPI_OBJS_STAT))
YAPI_OBJS_STAT_64  = $(patsubst %.o, $(OBJDIR_64)%.o, $(YAPI_OBJS_STAT))
YAPI_OBJS_STAT_ARMEL = $(patsubst %.o, $(OBJDIR_ARMEL)%.o, $(YAPI_OBJS_STAT))
YAPI_OBJS_STAT_ARMHF = $(patsubst %.o, $(OBJDIR_ARMHF)%.o, $(YAPI_OBJS_STAT))
YAPI_OBJS_STAT_OSX  = $(patsubst %.o, $(OBJDIR_OSX)%.o, $(YAPI_OBJS_STAT))
YAPI_OBJS_DYN_32 = $(patsubst %.o, $(OBJDIR_32)%.o, $(YAPI_OBJS_DYN))
YAPI_OBJS_DYN_64 = $(patsubst %.o, $(OBJDIR_64)%.o, $(YAPI_OBJS_DYN))
YAPI_OBJS_DYN_ARMEL = $(patsubst %.o, $(OBJDIR_ARMEL)%.o, $(YAPI_OBJS_DYN))
YAPI_OBJS_DYN_ARMHF = $(patsubst %.o, $(OBJDIR_ARMHF)%.o, $(YAPI_OBJS_DYN))
YAPI_OBJS_DYN_OSX = $(patsubst %.o, $(OBJDIR_OSX)%.o, $(YAPI_OBJS_DYN))

# most compatible ARMEL options, using soft-float
ARMEL_OPTS = -O2 -mfloat-abi=soft -march=armv5 -marm

# reduced ARMHF options to run properly on raspian-thing, but still be compatible with hard-floats VFP
ARMHF_OPTS = -O2 -mfloat-abi=hard -march=armv6 -marm
#linux compilation 64 bits
LIN64_OPTS = -O2 -m64
#linux compilaltion for 32 bits (limit it to i586 to support entry level computer)
LIN32_OPTS = -O2 -m32 -march=i586

#linux compilation 64 bist
OSX64_OPTS = -O1 -m64
GLOBAL_OPT = -g -Wall


fwd: default


#generic compilation rules
$(OBJDIR_64)%.o : %.c $(ALL_H)
	@echo compiling $@ 
	@gcc $(LIN64_OPTS) $(GLOBAL_OPT) -I$(YOCTO_API_DIR) -c $< -o $@

$(OBJDIR_64)%.o : %.cpp $(ALL_H)
	@echo compiling $@ 
	@g++ $(LIN64_OPTS) $(GLOBAL_OPT) -I$(YOCTO_API_DIR) -c $< -o $@

$(OBJDIR_64)%_dyn.o : %.c $(ALL_H)
	@echo compiling $@ 
	@gcc $(LIN64_OPTS) $(GLOBAL_OPT) -I$(YOCTO_API_DIR) -fPIC -c $< -o $@

$(OBJDIR_64)%_dyn.o : %.cpp $(ALL_H)
	@echo compiling $@ 
	@g++ $(LIN64_OPTS) $(GLOBAL_OPT) -I$(YOCTO_API_DIR) -fPIC -c $< -o $@

$(OBJDIR_32)%.o : %.c $(ALL_H)
	@echo compiling $@ 
	@gcc $(LIN32_OPTS) $(GLOBAL_OPT) -I$(YOCTO_API_DIR) -c $< -o $@

$(OBJDIR_32)%.o : %.cpp $(ALL_H)
	@echo compiling $@ 
	@g++ $(LIN32_OPTS) $(GLOBAL_OPT) -I$(YOCTO_API_DIR) -c $< -o $@

$(OBJDIR_32)%_dyn.o : %.c $(ALL_H)
	@echo compiling $@ 
	@gcc $(LIN32_OPTS) $(GLOBAL_OPT) -I$(YOCTO_API_DIR) -fPIC -c $< -o $@

$(OBJDIR_32)%_dyn.o : %.cpp $(ALL_H)
	@echo compiling $@ 
	@g++ $(LIN32_OPTS) $(GLOBAL_OPT) -I$(YOCTO_API_DIR) -fPIC -c $< -o $@

$(OBJDIR_ARMEL)%.o : %.c $(ALL_H)
	@echo compiling $@ 
	@gcc $(ARMEL_OPTS) $(GLOBAL_OPT) -I$(YOCTO_API_DIR) -c $< -o $@

$(OBJDIR_ARMEL)%.o : %.cpp $(ALL_H)
	@echo compiling $@ 
	@g++ $(ARMEL_OPTS) $(GLOBAL_OPT) -I$(YOCTO_API_DIR) -c $< -o $@

$(OBJDIR_ARMEL)%_dyn.o : %.c $(ALL_H)
	@echo compiling $@ 
	@gcc $(ARMEL_OPTS) $(GLOBAL_OPT) -I$(YOCTO_API_DIR) -fPIC -c $< -o $@

$(OBJDIR_ARMEL)%_dyn.o : %.cpp $(ALL_H)
	@echo compiling $@ 
	@g++ $(ARMEL_OPTS) $(GLOBAL_OPT) -I$(YOCTO_API_DIR) -fPIC -c $< -o $@

$(OBJDIR_ARMHF)%.o : %.c $(ALL_H)
	@echo compiling $@ 
	@gcc $(ARMHF_OPTS) $(GLOBAL_OPT) -I$(YOCTO_API_DIR) -c $< -o $@

$(OBJDIR_ARMHF)%.o : %.cpp $(ALL_H)
	@echo compiling $@ 
	@g++ $(ARMHF_OPTS) $(GLOBAL_OPT) -I$(YOCTO_API_DIR) -c $< -o $@

$(OBJDIR_ARMHF)%_dyn.o : %.c $(ALL_H)
	@echo compiling $@ 
	@gcc $(ARMHF_OPTS) $(GLOBAL_OPT) -I$(YOCTO_API_DIR) -fPIC -c $< -o $@

$(OBJDIR_ARMHF)%_dyn.o : %.cpp $(ALL_H)
	@echo compiling $@ 
	@g++ $(ARMHF_OPTS) $(GLOBAL_OPT) -I$(YOCTO_API_DIR) -fPIC -c $< -o $@

$(OBJDIR_OSX)%.o : %.c $(ALL_H)
	@echo compiling $@ 
	@gcc $(OSX64_OPTS) $(GLOBAL_OPT) -I$(YOCTO_API_DIR) -c $< -o $@

$(OBJDIR_OSX)%.o : %.cpp $(ALL_H)
	@echo compiling $@ 
	@g++ $(OSX64_OPTS) $(GLOBAL_OPT) -I$(YOCTO_API_DIR) -c $< -o $@

$(OBJDIR_OSX)%_dyn.o : %.c $(ALL_H)
	@echo compiling $@ 
	@gcc $(OSX64_OPTS) $(GLOBAL_OPT) -I$(YOCTO_API_DIR) -fPIC -c $< -o $@

$(OBJDIR_OSX)%_dyn.o : %.cpp $(ALL_H)
	@echo compiling $@ 
	@g++ $(OSX64_OPTS) $(GLOBAL_OPT) -I$(YOCTO_API_DIR) -fPIC -c $< -o $@

$(OUTDIR_64)libyocto-static.a : $(ALL_OBJS_STAT_64) $(ALL_H)
	@rm -rf $@
	@ar rcs $@  $(ALL_OBJS_STAT_64) 

$(YAPIOUT_64)libyapi-static.a : $(YAPI_OBJS_STAT_64) $(ALL_H)
	@rm -rf $@
	@ar rcs $@  $(YAPI_OBJS_STAT_64)

$(OUTDIR_64)libyocto.so.1.0.1 : $(ALL_OBJS_DYN_64) $(ALL_H)
	@g++ $(LIN64_OPTS) -shared -Wl,-soname,libyocto.so.1 -o $@ $(ALL_OBJS_DYN_64) -lm -lpthread -lusb-1.0

$(YAPIOUT_64)libyapi.so.1.0.1 : $(YAPI_OBJS_DYN_64) $(ALL_H)
	@gcc $(LIN64_OPTS) -shared -Wl,-soname,libyapi.so.1 -o $@ $(YAPI_OBJS_DYN_64) -lpthread -lusb-1.0

$(OUTDIR_32)libyocto-static.a : $(ALL_OBJS_STAT_32) $(ALL_H)
	@rm -rf $@
	@ar rcs $@  $(ALL_OBJS_STAT_32) 

$(YAPIOUT_32)libyapi-static.a : $(YAPI_OBJS_STAT_32) $(ALL_H)
	@rm -rf $@
	@ar rcs $@  $(YAPI_OBJS_STAT_32) 

$(OUTDIR_32)libyocto.so.1.0.1 : $(ALL_OBJS_DYN_32) $(ALL_H)
	@g++ $(LIN32_OPTS) -shared -Wl,-soname,libyocto.so.1 -o $@ $(ALL_OBJS_DYN_32) -lm -lpthread -lusb-1.0

$(YAPIOUT_32)libyapi.so.1.0.1 : $(YAPI_OBJS_DYN_32) $(ALL_H)
	@gcc $(LIN32_OPTS) -shared -Wl,-soname,libyapi.so.1 -o $@ $(YAPI_OBJS_DYN_32) -lpthread -lusb-1.0

$(OUTDIR_ARMEL)libyocto-static.a : $(ALL_OBJS_STAT_ARMEL) $(ALL_H)
	@rm -rf $@
	@ar rcs $@  $(ALL_OBJS_STAT_ARMEL) 

$(YAPIOUT_ARMEL)libyapi-static.a : $(YAPI_OBJS_STAT_ARMEL) $(ALL_H)
	@rm -rf $@
	@ar rcs $@  $(YAPI_OBJS_STAT_ARMEL) 

$(OUTDIR_ARMEL)libyocto.so.1.0.1 : $(ALL_OBJS_DYN_ARMEL) $(ALL_H)
	@g++ $(ARMEL_OPTS) -shared -Wl,-soname,libyocto.so.1 -o $@ $(ALL_OBJS_DYN_ARMEL) -lm -lpthread -lusb-1.0

$(YAPIOUT_ARMEL)libyapi.so.1.0.1 : $(YAPI_OBJS_DYN_ARMEL) $(ALL_H)
	@gcc $(ARMEL_OPTS) -shared -Wl,-soname,libyapi.so.1 -o $@ $(YAPI_OBJS_DYN_ARMEL) -lpthread -lusb-1.0

$(OUTDIR_ARMHF)libyocto-static.a : $(ALL_OBJS_STAT_ARMHF) $(ALL_H)
	@rm -rf $@
	@ar rcs $@  $(ALL_OBJS_STAT_ARMHF) 

$(YAPIOUT_ARMHF)libyapi-static.a : $(YAPI_OBJS_STAT_ARMHF) $(ALL_H)
	@rm -rf $@
	@ar rcs $@  $(YAPI_OBJS_STAT_ARMHF) 

$(OUTDIR_ARMHF)libyocto.so.1.0.1 : $(ALL_OBJS_DYN_ARMHF) $(ALL_H)
	@g++ $(ARMHF_OPTS) -shared -Wl,-soname,libyocto.so.1 -o $@ $(ALL_OBJS_DYN_ARMHF) -lm -lpthread -lusb-1.0

$(YAPIOUT_ARMHF)libyapi.so.1.0.1 : $(YAPI_OBJS_DYN_ARMHF) $(ALL_H)
	@gcc $(ARMHF_OPTS) -shared -Wl,-soname,libyapi.so.1 -o $@ $(YAPI_OBJS_DYN_ARMHF) -lpthread -lusb-1.0

$(OUTDIR_OSX)libyocto-static.a : $(ALL_OBJS_STAT_OSX) $(ALL_H)
	@rm -rf $@
	@ar rcs $@  $(ALL_OBJS_STAT_OSX) 

$(YAPIOUT_OSX)libyapi-static.a : $(YAPI_OBJS_STAT_OSX) $(ALL_H)
	@rm -rf $@
	@ar rcs $@  $(YAPI_OBJS_STAT_OSX)

$(OUTDIR_OSX)libyocto.dylib : $(ALL_OBJS_DYN_OSX) $(ALL_H)
	@g++ -m64 -dynamiclib -Wl,-reexport_framework,IOKit,-reexport_framework,CoreFoundation,-headerpad_max_install_names,-undefined,dynamic_lookup,-compatibility_version,1.0,-current_version,1.0,-install_name,/usr/local/lib/libyocto.dylib -o $@ $(ALL_OBJS_DYN_OSX) 

$(YAPIOUT_OSX)libyapi.dylib : $(YAPI_OBJS_DYN_OSX) $(ALL_H)
	@gcc -m64 -dynamiclib -Wl,-reexport_framework,IOKit,-reexport_framework,CoreFoundation,-headerpad_max_install_names,-undefined,dynamic_lookup,-compatibility_version,1.0,-current_version,1.0,-install_name,/usr/local/lib/libyapi.dylib -o $@ $(YAPI_OBJS_DYN_OSX) 





ifeq ($(UNAME), Linux)
#linux
ifeq ($(ARCH), x86_64)
DEFAULT_TARGETS = $(OBJDIR_32) $(YAPIOUT_32) $(OUTDIR_32) $(OUTDIR_64) $(OBJDIR_64) $(YAPIOUT_64) $(TARGETS_64)
RELEASE_TARGETS = $(OBJDIR_32) $(YAPIOUT_32) $(OUTDIR_32) $(OUTDIR_64) $(OBJDIR_64) $(YAPIOUT_64) $(TARGETS_32) $(TARGETS_64)
OBJ_TO_CLEAN    = $(OBJDIR_32) $(OBJDIR_64) 
else
ifeq ($(ARCH),i386)
DEFAULT_TARGETS = $(OBJDIR_32) $(YAPIOUT_32) $(OUTDIR_32) $(TARGETS_32)
RELEASE_TARGETS = $(OBJDIR_32) $(YAPIOUT_32) $(OUTDIR_32) $(TARGETS_32) 
OBJ_TO_CLEAN    = $(OBJDIR_32)
else
#$(warning $(ARM_BUILD_TYPE))
ifeq ($(ARM_BUILD_TYPE), hf)
DEFAULT_TARGETS = $(OBJDIR_ARMHF) $(YAPIOUT_ARMHF) $(OUTDIR_ARMHF) $(TARGETS_ARMHF)
RELEASE_TARGETS = $(OBJDIR_ARMHF) $(YAPIOUT_ARMHF) $(OUTDIR_ARMHF) $(TARGETS_ARMHF)
OBJ_TO_CLEAN    = $(OBJDIR_ARMHF)
else
DEFAULT_TARGETS = $(OBJDIR_ARMEL) $(YAPIOUT_ARMEL) $(OUTDIR_ARMEL) $(TARGETS_ARMEL)
RELEASE_TARGETS = $(OBJDIR_ARMEL) $(YAPIOUT_ARMEL) $(OUTDIR_ARMEL) $(TARGETS_ARMEL)
OBJ_TO_CLEAN    = $(OBJDIR_ARMEL)
endif
endif
endif
else
#mac os
DEFAULT_TARGETS = $(OBJDIR_OSX) $(YAPIOUT_OSX) $(OUTDIR_OSX) $(TARGETS_OSX)
RELEASE_TARGETS = $(OBJDIR_OSX) $(YAPIOUT_OSX) $(OUTDIR_OSX) $(TARGETS_OSX)
OBJ_TO_CLEAN    = $(OBJDIR_OSX)
endif

default: $(DEFAULT_TARGETS) 

release: $(RELEASE_TARGETS) clean_obj	

$(OBJDIR_32) $(YAPIOUT_32) $(OUTDIR_32) $(OUTDIR_64) $(OBJDIR_64) $(YAPIOUT_64):
	@mkdir -p $@

$(OBJDIR_ARMEL) $(YAPIOUT_ARMEL) $(OUTDIR_ARMEL) $(OBJDIR_ARMHF) $(YAPIOUT_ARMHF) $(OUTDIR_ARMHF):
	@mkdir -p $@

$(OBJDIR_OSX) $(YAPIOUT_OSX) $(OUTDIR_OSX):
	@mkdir -p $@

clean: clean_obj
	@rm -rf  $(RELEASE_TARGETS) $(DEFAULT_TARGETS)

clean_obj:
	@rm -rf  $(OBJ_TO_CLEAN)

