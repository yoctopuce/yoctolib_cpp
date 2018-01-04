#-------------------------------------------------
#
# Project created by QtCreator 2013-09-05T23:50:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QTDemo
TEMPLATE = app


SOURCES += main.cpp\
        inventory.cpp

HEADERS  += inventory.h

FORMS    += inventory.ui


SOURCES +=  ../../../Sources/yocto_wireless.cpp \
    ../../../Sources/yocto_watchdog.cpp \
    ../../../Sources/yocto_wakeupschedule.cpp \
    ../../../Sources/yocto_wakeupmonitor.cpp \
    ../../../Sources/yocto_voltage.cpp \
    ../../../Sources/yocto_voc.cpp \
    ../../../Sources/yocto_temperature.cpp \
    ../../../Sources/yocto_servo.cpp \
    ../../../Sources/yocto_relay.cpp \
    ../../../Sources/yocto_realtimeclock.cpp \
    ../../../Sources/yocto_pressure.cpp \
    ../../../Sources/yocto_power.cpp \
    ../../../Sources/yocto_oscontrol.cpp \
    ../../../Sources/yocto_network.cpp \
    ../../../Sources/yocto_lightsensor.cpp \
    ../../../Sources/yocto_led.cpp \
    ../../../Sources/yocto_humidity.cpp \
    ../../../Sources/yocto_hubport.cpp \
    ../../../Sources/yocto_genericsensor.cpp \
    ../../../Sources/yocto_files.cpp \
    ../../../Sources/yocto_dualpower.cpp \
    ../../../Sources/yocto_display.cpp \
    ../../../Sources/yocto_digitalio.cpp \
    ../../../Sources/yocto_datalogger.cpp \
    ../../../Sources/yocto_current.cpp \
    ../../../Sources/yocto_colorled.cpp \
    ../../../Sources/yocto_carbondioxide.cpp \
    ../../../Sources/yocto_api.cpp \
    ../../../Sources/yocto_anbutton.cpp \
    ../../../Sources/yapi/ythread.c \
    ../../../Sources/yapi/ytcp.c \
    ../../../Sources/yapi/ystream.c \
    ../../../Sources/yapi/yprog.c \
    ../../../Sources/yapi/ypkt_win.c \
    ../../../Sources/yapi/ypkt_osx.c \
    ../../../Sources/yapi/ypkt_lin.c \
    ../../../Sources/yapi/ymemory.c \
    ../../../Sources/yapi/ykey.c \
    ../../../Sources/yapi/yjson.c \
    ../../../Sources/yapi/yhash.c \
    ../../../Sources/yapi/yfifo.c \
    ../../../Sources/yapi/yapi.c

HEADERS  += ../../../Sources/yocto_wireless.h \
    ../../../Sources/yocto_watchdog.h \
    ../../../Sources/yocto_wakeupschedule.h \
    ../../../Sources/yocto_wakeupmonitor.h \
    ../../../Sources/yocto_voltage.h \
    ../../../Sources/yocto_voc.h \
    ../../../Sources/yocto_temperature.h \
    ../../../Sources/yocto_servo.h \
    ../../../Sources/yocto_relay.h \
    ../../../Sources/yocto_realtimeclock.h \
    ../../../Sources/yocto_pressure.h \
    ../../../Sources/yocto_power.h \
    ../../../Sources/yocto_oscontrol.h \
    ../../../Sources/yocto_network.h \
    ../../../Sources/yocto_lightsensor.h \
    ../../../Sources/yocto_led.h \
    ../../../Sources/yocto_humidity.h \
    ../../../Sources/yocto_hubport.h \
    ../../../Sources/yocto_genericsensor.h \
    ../../../Sources/yocto_files.h \
    ../../../Sources/yocto_dualpower.h \
    ../../../Sources/yocto_display.h \
    ../../../Sources/yocto_digitalio.h \
    ../../../Sources/yocto_datalogger.h \
    ../../../Sources/yocto_current.h \
    ../../../Sources/yocto_colorled.h \
    ../../../Sources/yocto_carbondioxide.h \
    ../../../Sources/yocto_api.h \
    ../../../Sources/yocto_anbutton.h \
    ../../../Sources/yapi/yversion.h \
    ../../../Sources/yapi/ythread.h \
    ../../../Sources/yapi/ytcp.h \
    ../../../Sources/yapi/yproto.h \
    ../../../Sources/yapi/yprog.h \
    ../../../Sources/yapi/ymemory.h \
    ../../../Sources/yapi/ykey.h \
    ../../../Sources/yapi/yjson.h \
    ../../../Sources/yapi/yhash.h \
    ../../../Sources/yapi/yfifo.h \
    ../../../Sources/yapi/ydef.h \
    ../../../Sources/yapi/yapi.h


# add Yoctopuce Library to the Include path
INCLUDEPATH += $$PWD/../../../Sources/

# add the OSX frameworks used to access to the USB port
macx:  LIBS += -framework CoreFoundation
macx:  LIBS += -framework IOKit
# add windows socket library
win32: LIBS += -lws2_32
win32: LIBS += -lSetupApi
# add libusb 1.0
unix:!macx: LIBS += -lusb-1.0
