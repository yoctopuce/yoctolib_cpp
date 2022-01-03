# Create a library called "YoctoLib" which includes the .cpp and .c source files and
cmake_minimum_required(VERSION 3.8)


project(YoctoLib VERSION 1.0.1 DESCRIPTION "Yoctopuce C++ library")


set(YoctoLibHSources

#-- yocto_patch_h_start --
	yocto_api.h
	yocto_accelerometer.h
	yocto_altitude.h
	yocto_anbutton.h
	yocto_arithmeticsensor.h
	yocto_audioin.h
	yocto_audioout.h
	yocto_bluetoothlink.h
	yocto_buzzer.h
	yocto_carbondioxide.h
	yocto_cellular.h
	yocto_colorled.h
	yocto_colorledcluster.h
	yocto_compass.h
	yocto_current.h
	yocto_currentloopoutput.h
	yocto_daisychain.h
	yocto_digitalio.h
	yocto_display.h
	yocto_dualpower.h
	yocto_files.h
	yocto_genericsensor.h
	yocto_gps.h
	yocto_groundspeed.h
	yocto_gyro.h
	yocto_hubport.h
	yocto_humidity.h
	yocto_i2cport.h
	yocto_inputchain.h
	yocto_latitude.h
	yocto_led.h
	yocto_lightsensor.h
	yocto_longitude.h
	yocto_magnetometer.h
	yocto_messagebox.h
	yocto_motor.h
	yocto_multiaxiscontroller.h
	yocto_multicellweighscale.h
	yocto_multisenscontroller.h
	yocto_network.h
	yocto_oscontrol.h
	yocto_power.h
	yocto_poweroutput.h
	yocto_powersupply.h
	yocto_pressure.h
	yocto_proximity.h
	yocto_pwminput.h
	yocto_pwmoutput.h
	yocto_pwmpowersource.h
	yocto_quadraturedecoder.h
	yocto_rangefinder.h
	yocto_realtimeclock.h
	yocto_refframe.h
	yocto_relay.h
	yocto_segmenteddisplay.h
	yocto_serialport.h
	yocto_servo.h
	yocto_spiport.h
	yocto_steppermotor.h
	yocto_temperature.h
	yocto_tilt.h
	yocto_tvoc.h
	yocto_voc.h
	yocto_voltage.h
	yocto_voltageoutput.h
	yocto_wakeupmonitor.h
	yocto_wakeupschedule.h
	yocto_watchdog.h
	yocto_weighscale.h
	yocto_wireless.h

#-- yocto_patch_h_stop --
)


set(YoctoLibCppSources
#-- yocto_patch_start --
	yocto_api.cpp
	yocto_accelerometer.cpp
	yocto_altitude.cpp
	yocto_anbutton.cpp
	yocto_arithmeticsensor.cpp
	yocto_audioin.cpp
	yocto_audioout.cpp
	yocto_bluetoothlink.cpp
	yocto_buzzer.cpp
	yocto_carbondioxide.cpp
	yocto_cellular.cpp
	yocto_colorled.cpp
	yocto_colorledcluster.cpp
	yocto_compass.cpp
	yocto_current.cpp
	yocto_currentloopoutput.cpp
	yocto_daisychain.cpp
	yocto_digitalio.cpp
	yocto_display.cpp
	yocto_dualpower.cpp
	yocto_files.cpp
	yocto_genericsensor.cpp
	yocto_gps.cpp
	yocto_groundspeed.cpp
	yocto_gyro.cpp
	yocto_hubport.cpp
	yocto_humidity.cpp
	yocto_i2cport.cpp
	yocto_inputchain.cpp
	yocto_latitude.cpp
	yocto_led.cpp
	yocto_lightsensor.cpp
	yocto_longitude.cpp
	yocto_magnetometer.cpp
	yocto_messagebox.cpp
	yocto_motor.cpp
	yocto_multiaxiscontroller.cpp
	yocto_multicellweighscale.cpp
	yocto_multisenscontroller.cpp
	yocto_network.cpp
	yocto_oscontrol.cpp
	yocto_power.cpp
	yocto_poweroutput.cpp
	yocto_powersupply.cpp
	yocto_pressure.cpp
	yocto_proximity.cpp
	yocto_pwminput.cpp
	yocto_pwmoutput.cpp
	yocto_pwmpowersource.cpp
	yocto_quadraturedecoder.cpp
	yocto_rangefinder.cpp
	yocto_realtimeclock.cpp
	yocto_refframe.cpp
	yocto_relay.cpp
	yocto_segmenteddisplay.cpp
	yocto_serialport.cpp
	yocto_servo.cpp
	yocto_spiport.cpp
	yocto_steppermotor.cpp
	yocto_temperature.cpp
	yocto_tilt.cpp
	yocto_tvoc.cpp
	yocto_voc.cpp
	yocto_voltage.cpp
	yocto_voltageoutput.cpp
	yocto_wakeupmonitor.cpp
	yocto_wakeupschedule.cpp
	yocto_watchdog.cpp
	yocto_weighscale.cpp
	yocto_wireless.cpp

#-- yocto_patch_stop --
)

add_library (YoctoLib ${YoctoLibCppSources} ${YoctoLibHSources})

# add yapi low level api
add_subdirectory(yapi)
target_link_libraries(YoctoLib PRIVATE yapi)
target_include_directories (YoctoLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(YoctoLib PROPERTIES SOVERSION 1)
set_target_properties(YoctoLib PROPERTIES LIBRARY_OUTPUT_NAME yocto)
install(TARGETS YoctoLib DESTINATION lib)
install(FILES ${YoctoLibHSources} DESTINATION include)
install(FILES yapi/yapi.h yapi/ydef.h DESTINATION include/yapi)

