#-------------------------------------------------------
#
# Project created by M. FURIC Franck 2020-06-15T09:35:54
#
#-------------------------------------------------------

QT       += core

TEMPLATE = lib
DEFINES += YOCTOPUCEDLL_EXPORTS # permet de crée les .dll et .lib/.a voir L.55 yocto_api.h

CONFIG += skip_target_version_ext
CONFIG += shared debug_and_release build_all

QMAKE_TARGET_PRODUCT = "YoctoAPI"
QMAKE_TARGET_DESCRIPTION = "API Yoctopuce for Qt"

VERSION = 40411 # Don't forget to change if you use others versions

# remove -$$VERSION if you want
# YoctopuceQtLib-40411.dll => YoctopuceQtLib.dll
TARGET =  YoctopuceQtLib-$$VERSION

CONFIG(debug, debug|release){
   TARGET = $$join(TARGET,,,_d)
}

unix$ {
    target.path = /usr/lib
    INSTALLS += target
}

win32: LIBS += -lws2_32
win32: LIBS += -lSetupApi

# add the OSX frameworks used to access to the USB port
macx:  LIBS += -framework CoreFoundation
macx:  LIBS += -framework IOKit

# Copie de la lib dans le dossier out [Windows]
DLLDESTDIR += $$PWD/lib/

# D'ont forget to add new file if there are
HEADERS += \
    ../../Sources/winresource.rc \
    ../../Sources/winresource64.rc \
    ../../Sources/yapi/yapi.h \
    ../../Sources/yapi/yapiresource.rc \
    ../../Sources/yapi/yapiresource64.rc \
    ../../Sources/yapi/ydef.h \
    ../../Sources/yapi/yfifo.h \
    ../../Sources/yapi/yhash.h \
    ../../Sources/yapi/yjni.h \
    ../../Sources/yapi/yjson.h \
    ../../Sources/yapi/ykey.h \
    ../../Sources/yapi/ymemory.h \
    ../../Sources/yapi/yprog.h \
    ../../Sources/yapi/yproto.h \
    ../../Sources/yapi/ytcp.h \
    ../../Sources/yapi/ythread.h \
    ../../Sources/yapi/yversion.h \
    ../../Sources/yocto_accelerometer.h \
    ../../Sources/yocto_altitude.h \
    ../../Sources/yocto_anbutton.h \
    ../../Sources/yocto_api.h \
    ../../Sources/yocto_arithmeticsensor.h \
    ../../Sources/yocto_audioin.h \
    ../../Sources/yocto_audioout.h \
    ../../Sources/yocto_bluetoothlink.h \
    ../../Sources/yocto_buzzer.h \
    ../../Sources/yocto_carbondioxide.h \
    ../../Sources/yocto_cellular.h \
    ../../Sources/yocto_colorled.h \
    ../../Sources/yocto_colorledcluster.h \
    ../../Sources/yocto_compass.h \
    ../../Sources/yocto_current.h \
    ../../Sources/yocto_currentloopoutput.h \
    ../../Sources/yocto_daisychain.h \
    ../../Sources/yocto_datalogger.h \
    ../../Sources/yocto_digitalio.h \
    ../../Sources/yocto_display.h \
    ../../Sources/yocto_dualpower.h \
    ../../Sources/yocto_files.h \
    ../../Sources/yocto_genericsensor.h \
    ../../Sources/yocto_gps.h \
    ../../Sources/yocto_groundspeed.h \
    ../../Sources/yocto_gyro.h \
    ../../Sources/yocto_hubport.h \
    ../../Sources/yocto_humidity.h \
    ../../Sources/yocto_i2cport.h \
    ../../Sources/yocto_latitude.h \
    ../../Sources/yocto_led.h \
    ../../Sources/yocto_lightsensor.h \
    ../../Sources/yocto_longitude.h \
    ../../Sources/yocto_magnetometer.h \
    ../../Sources/yocto_messagebox.h \
    ../../Sources/yocto_motor.h \
    ../../Sources/yocto_multiaxiscontroller.h \
    ../../Sources/yocto_multicellweighscale.h \
    ../../Sources/yocto_multisenscontroller.h \
    ../../Sources/yocto_network.h \
    ../../Sources/yocto_oscontrol.h \
    ../../Sources/yocto_power.h \
    ../../Sources/yocto_poweroutput.h \
    ../../Sources/yocto_powersupply.h \
    ../../Sources/yocto_pressure.h \
    ../../Sources/yocto_proximity.h \
    ../../Sources/yocto_pwminput.h \
    ../../Sources/yocto_pwmoutput.h \
    ../../Sources/yocto_pwmpowersource.h \
    ../../Sources/yocto_quadraturedecoder.h \
    ../../Sources/yocto_rangefinder.h \
    ../../Sources/yocto_realtimeclock.h \
    ../../Sources/yocto_refframe.h \
    ../../Sources/yocto_relay.h \
    ../../Sources/yocto_segmenteddisplay.h \
    ../../Sources/yocto_serialport.h \
    ../../Sources/yocto_servo.h \
    ../../Sources/yocto_spiport.h \
    ../../Sources/yocto_steppermotor.h \
    ../../Sources/yocto_temperature.h \
    ../../Sources/yocto_tilt.h \
    ../../Sources/yocto_tvoc.h \
    ../../Sources/yocto_voc.h \
    ../../Sources/yocto_voltage.h \
    ../../Sources/yocto_voltageoutput.h \
    ../../Sources/yocto_wakeupmonitor.h \
    ../../Sources/yocto_wakeupschedule.h \
    ../../Sources/yocto_watchdog.h \
    ../../Sources/yocto_weighscale.h \
    ../../Sources/yocto_wireless.h

SOURCES += \
    ../../Sources/yapi/yapi.c \
    ../../Sources/yapi/yfifo.c \
    ../../Sources/yapi/yhash.c \
    ../../Sources/yapi/yjni.c \
    ../../Sources/yapi/yjson.c \
    ../../Sources/yapi/ykey.c \
    ../../Sources/yapi/ymemory.c \
    ../../Sources/yapi/ypkt_lin.c \
    ../../Sources/yapi/ypkt_osx.c \
    ../../Sources/yapi/ypkt_win.c \
    ../../Sources/yapi/yprog.c \
    ../../Sources/yapi/ystream.c \
    ../../Sources/yapi/ytcp.c \
    ../../Sources/yapi/ythread.c \
    ../../Sources/yocto_accelerometer.cpp \
    ../../Sources/yocto_altitude.cpp \
    ../../Sources/yocto_anbutton.cpp \
    ../../Sources/yocto_api.cpp \
    ../../Sources/yocto_arithmeticsensor.cpp \
    ../../Sources/yocto_audioin.cpp \
    ../../Sources/yocto_audioout.cpp \
    ../../Sources/yocto_bluetoothlink.cpp \
    ../../Sources/yocto_buzzer.cpp \
    ../../Sources/yocto_carbondioxide.cpp \
    ../../Sources/yocto_cellular.cpp \
    ../../Sources/yocto_colorled.cpp \
    ../../Sources/yocto_colorledcluster.cpp \
    ../../Sources/yocto_compass.cpp \
    ../../Sources/yocto_current.cpp \
    ../../Sources/yocto_currentloopoutput.cpp \
    ../../Sources/yocto_daisychain.cpp \
    ../../Sources/yocto_datalogger.cpp \
    ../../Sources/yocto_digitalio.cpp \
    ../../Sources/yocto_display.cpp \
    ../../Sources/yocto_dualpower.cpp \
    ../../Sources/yocto_files.cpp \
    ../../Sources/yocto_genericsensor.cpp \
    ../../Sources/yocto_gps.cpp \
    ../../Sources/yocto_groundspeed.cpp \
    ../../Sources/yocto_gyro.cpp \
    ../../Sources/yocto_hubport.cpp \
    ../../Sources/yocto_humidity.cpp \
    ../../Sources/yocto_i2cport.cpp \
    ../../Sources/yocto_latitude.cpp \
    ../../Sources/yocto_led.cpp \
    ../../Sources/yocto_lightsensor.cpp \
    ../../Sources/yocto_longitude.cpp \
    ../../Sources/yocto_magnetometer.cpp \
    ../../Sources/yocto_messagebox.cpp \
    ../../Sources/yocto_motor.cpp \
    ../../Sources/yocto_multiaxiscontroller.cpp \
    ../../Sources/yocto_multicellweighscale.cpp \
    ../../Sources/yocto_multisenscontroller.cpp \
    ../../Sources/yocto_network.cpp \
    ../../Sources/yocto_oscontrol.cpp \
    ../../Sources/yocto_power.cpp \
    ../../Sources/yocto_poweroutput.cpp \
    ../../Sources/yocto_powersupply.cpp \
    ../../Sources/yocto_pressure.cpp \
    ../../Sources/yocto_proximity.cpp \
    ../../Sources/yocto_pwminput.cpp \
    ../../Sources/yocto_pwmoutput.cpp \
    ../../Sources/yocto_pwmpowersource.cpp \
    ../../Sources/yocto_quadraturedecoder.cpp \
    ../../Sources/yocto_rangefinder.cpp \
    ../../Sources/yocto_realtimeclock.cpp \
    ../../Sources/yocto_refframe.cpp \
    ../../Sources/yocto_relay.cpp \
    ../../Sources/yocto_segmenteddisplay.cpp \
    ../../Sources/yocto_serialport.cpp \
    ../../Sources/yocto_servo.cpp \
    ../../Sources/yocto_spiport.cpp \
    ../../Sources/yocto_steppermotor.cpp \
    ../../Sources/yocto_temperature.cpp \
    ../../Sources/yocto_tilt.cpp \
    ../../Sources/yocto_tvoc.cpp \
    ../../Sources/yocto_voc.cpp \
    ../../Sources/yocto_voltage.cpp \
    ../../Sources/yocto_voltageoutput.cpp \
    ../../Sources/yocto_wakeupmonitor.cpp \
    ../../Sources/yocto_wakeupschedule.cpp \
    ../../Sources/yocto_watchdog.cpp \
    ../../Sources/yocto_weighscale.cpp \
    ../../Sources/yocto_wireless.cpp

DISTFILES += \
    ../../Sources/yapi/yapi.def


