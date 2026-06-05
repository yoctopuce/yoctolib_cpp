Yoctopuce library for C++ v2.0 
==============================

## License information

Copyright (C) 2011 and beyond by Yoctopuce Sarl, Switzerland.

Yoctopuce Sarl (hereafter Licensor) grants to you a perpetual
non-exclusive license to use, modify, copy and integrate this
file into your software for the sole purpose of interfacing
with Yoctopuce products.

You may reproduce and distribute copies of this file in
source or object form, as long as the sole purpose of this
code is to interface with Yoctopuce products. You must retain
this notice in the distributed source file.

You should refer to Yoctopuce General Terms and Conditions
for additional information regarding your rights and
obligations.

THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING
WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO
EVENT SHALL LICENSOR BE LIABLE FOR ANY INCIDENTAL, SPECIAL,
INDIRECT OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA,
COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR
SERVICES, ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT
LIMITED TO ANY DEFENSE THEREOF), ANY CLAIMS FOR INDEMNITY OR
CONTRIBUTION, OR OTHER SIMILAR COSTS, WHETHER ASSERTED ON THE
BASIS OF CONTRACT, TORT (INCLUDING NEGLIGENCE), BREACH OF
WARRANTY, OR OTHERWISE.

## Content of this package

 * build.bat

 		Automated build script for Windows

 * build.sh

 		Automated build script for UNIX platforms

 * FILES.txt

 		List of files contained in this archive

 * RELEASE.txt

 		Release notes

 * Binaries/GNUmakefile

 		GNU Makefile for all platforms

 * Binaries/make.bat

 		Batch to start make on Windows with right paths

 * Binaries/windows/

 		Directory that contains Windows 32 bits executables

 * Binaries/windows/amd64

 		Directory that contains Windows 64 bitsexecutables

 * Binaries/osx/

 		Directory that contains Max OS X executables

 * Binaries/linux/i386/

 		Directory that contains Linux Intel 32bit executables

 * Binaries/linux/x86_64/

 		Directory that contains Linux Intel 64bit executables

 * Binaries/linux/armel/

 		Directory that contains Linux ARM soft float executables

 * Binaries/linux/armhf/

 		Directory that contains Linux ARM hard float executables

 * Binaries/linux/aarch64/

 		Directory that contains Linux ARM 64 bits executables

 * Documentation/

 		API Reference, in HTML and PDF format

 * Examples/

 		Directory with sample programs in C++

 * Sources/

 		Source code of the high-level library (in C++)

 * Sources/yapi/

 		Source code of the low-level library (in C)

 * Sources/yapi/mbedtls

 		Source code of mbedTLS library (used for encryption)

 * udev_conf/

 		Udev rules for Linux (see Linux Release Notes)


## Installation

The archive is shipped without precompiled libraries. If you want to build
them from source, or to compile the examples, use the following command:

on Windows: build
```bash
build
```
on UNIX:
```bash
./build.sh
```

For more details, refer to the documentation specific to each product, which
includes sample code with explanations, and a programming reference manual.
In case of trouble, contact support@yoctopuce.com

Have fun !


## Linux Notes

### Libusb 1.0

In order to compile the library you have to install the version 1.0 of libusb.
Take care to use version 1.0 and not version 0.1. To install libusb 1.0 on
Ubuntu, run "sudo apt-get install libusb-1.0-0-dev".


### Configure udev access rights

In order to work properly, the Yoctopuce VirtualHub and library need write
access to all Yoctopuce devices. By default, Linux access rights for USB
device are read only for all users, except root. If you want to avoid running
VirtualHub as root, you need to add a new rule to your udev configuration.

To add a new udev rules to your Linux installation, you need to create a text
file in the directory "/etc/udev/rules.d" following the naming pattern "##-
arbitraryName.rules". Upon startup, udev will process all files in this
directory with the extension ".rules" according to there alphabetical order.
For instance, the file "51-first.rules" will be processed before  the file "50-
udev-default.rules". The file "50-udev-default.rules" is actually used to
implement the default rules of the system. Therefore, to modify the default
handling behaviour of the system, you have to create a file that start with a
number lower than 50. Note that to add a rules to your udev configuration you
have to be root.

In the sub directory udev_conf we have put two examples of rules that you can
use as reference for your rules.

Example 1: 51-yoctopuce.rules

This rule will add write access to Yoctopuce USB devices for all users. Access
rights for all other devices will be left unchanged. If this is what you want,
copy the file "51-yoctopuce_all.rules" to the directory  "/etc/udev/rules.d"
and restart your system.

    # udev rules to allow write access to all users for Yoctopuce USB devices
    SUBSYSTEM=="usb", ATTR{idVendor}=="24e0", MODE="0666"

Example 2: 51-yoctopuce_group.rules

This rule will allow write access to Yoctopuce USB devices for all users of
the group "yoctogoup". Access right for all other devices will be left
unchanged. If this is what you want, you need to copy the file "51-
yoctopuce_all.rules" to the directory  "/etc/udev/rules.d" and restart your
system.

    # udev rules to allow write access to all users of "yoctogroup" for Yoctopuce USB devices
    SUBSYSTEM=="usb", ATTR{idVendor}=="24e0", MODE="0664",  GROUP="yoctogroup"

## Compilation

The easiest way to compile the library is to use our build script, either ``build.bat``
(for Windows) or ``build.sh`` (for Unix-based systems), located at the root of the library.
This script will automatically build the library for your current architecture and
compile all included examples.

If you only need to compile the library itself, you can use the GNU Makefile available
in the ``Binary`` folder. Alternatively, you can use CMake by including the ``CMakeLists.txt``
file from the ``Sources`` folder in your project.

For those who wish to create a custom build script, below is a list of all the files
required by the library:

### Folders to Add to the Include Path

```
/Sources
/Sources/yapi/include
/Sources/yapi/tf-psa-crypto/core
/Sources/yapi/tf-psa-crypto/drivers/builtin/include
/Sources/yapi/tf-psa-crypto/drivers/builtin/src
/Sources/yapi/tf-psa-crypto/include
/Sources/yapi/tf-psa-crypto/extras
/Sources/yapi/tf-psa-crypto/utilities
/Sources/yapi/tf-psa-crypto/platform
```

### C/C++ Source Files
```
/Sources/yocto_api.cpp
/Sources/yocto_accelerometer.cpp
/Sources/yocto_airquality.cpp
/Sources/yocto_altitude.cpp
/Sources/yocto_anbutton.cpp
/Sources/yocto_angularspeed.cpp
/Sources/yocto_arithmeticsensor.cpp
/Sources/yocto_audioin.cpp
/Sources/yocto_audioout.cpp
/Sources/yocto_bluetoothlink.cpp
/Sources/yocto_buzzer.cpp
/Sources/yocto_carbondioxide.cpp
/Sources/yocto_cellular.cpp
/Sources/yocto_colorled.cpp
/Sources/yocto_colorledcluster.cpp
/Sources/yocto_colorsensor.cpp
/Sources/yocto_compass.cpp
/Sources/yocto_counter.cpp
/Sources/yocto_current.cpp
/Sources/yocto_currentloopoutput.cpp
/Sources/yocto_daisychain.cpp
/Sources/yocto_digitalio.cpp
/Sources/yocto_display.cpp
/Sources/yocto_dualpower.cpp
/Sources/yocto_files.cpp
/Sources/yocto_genericsensor.cpp
/Sources/yocto_gps.cpp
/Sources/yocto_groundspeed.cpp
/Sources/yocto_gyro.cpp
/Sources/yocto_hubport.cpp
/Sources/yocto_humidity.cpp
/Sources/yocto_i2cport.cpp
/Sources/yocto_inputcapture.cpp
/Sources/yocto_inputchain.cpp
/Sources/yocto_latitude.cpp
/Sources/yocto_led.cpp
/Sources/yocto_lightsensor.cpp
/Sources/yocto_longitude.cpp
/Sources/yocto_magnetometer.cpp
/Sources/yocto_messagebox.cpp
/Sources/yocto_micropython.cpp
/Sources/yocto_motor.cpp
/Sources/yocto_multiaxiscontroller.cpp
/Sources/yocto_multicellweighscale.cpp
/Sources/yocto_multisenscontroller.cpp
/Sources/yocto_network.cpp
/Sources/yocto_orientation.cpp
/Sources/yocto_oscontrol.cpp
/Sources/yocto_power.cpp
/Sources/yocto_poweroutput.cpp
/Sources/yocto_powersupply.cpp
/Sources/yocto_pressure.cpp
/Sources/yocto_proximity.cpp
/Sources/yocto_pwminput.cpp
/Sources/yocto_pwmoutput.cpp
/Sources/yocto_pwmpowersource.cpp
/Sources/yocto_quadraturedecoder.cpp
/Sources/yocto_rangefinder.cpp
/Sources/yocto_realtimeclock.cpp
/Sources/yocto_refframe.cpp
/Sources/yocto_relay.cpp
/Sources/yocto_rfidreader.cpp
/Sources/yocto_sdi12port.cpp
/Sources/yocto_segmenteddisplay.cpp
/Sources/yocto_serialport.cpp
/Sources/yocto_servo.cpp
/Sources/yocto_soundlevel.cpp
/Sources/yocto_soundspectrum.cpp
/Sources/yocto_spectralchannel.cpp
/Sources/yocto_spiport.cpp
/Sources/yocto_steppermotor.cpp
/Sources/yocto_temperature.cpp
/Sources/yocto_threshold.cpp
/Sources/yocto_tilt.cpp
/Sources/yocto_tvoc.cpp
/Sources/yocto_virtualsensor.cpp
/Sources/yocto_voc.cpp
/Sources/yocto_voltage.cpp
/Sources/yocto_voltageoutput.cpp
/Sources/yocto_wakeupmonitor.cpp
/Sources/yocto_wakeupschedule.cpp
/Sources/yocto_watchdog.cpp
/Sources/yocto_weighscale.cpp
/Sources/yocto_wireless.cpp
/Sources/yapi/yapi.c
/Sources/yapi/yfifo.c
/Sources/yapi/yhash.c
/Sources/yapi/yjson.c
/Sources/yapi/ykey.c
/Sources/yapi/ymemory.c
/Sources/yapi/yprog.c
/Sources/yapi/ystream.c
/Sources/yapi/ytcp.c
/Sources/yapi/ythread.c
/Sources/yapi/ypkt_lin.c
/Sources/yapi/ypkt_win.c
/Sources/yapi/ypkt_osx.c
/Sources/yapi/yssl.c
/Sources/yapi/yapi.h
/Sources/yapi/ydef.h
/Sources/yapi/yfifo.h
/Sources/yapi/yhash.h
/Sources/yapi/yjson.h
/Sources/yapi/ykey.h
/Sources/yapi/ymemory.h
/Sources/yapi/yprog.h
/Sources/yapi/yproto.h
/Sources/yapi/ytcp.h
/Sources/yapi/ythread.h
/Sources/yapi/yversion.h
/Sources/yapi/yssl.h
/Sources/yapi/ydef_private.h
/Sources/yapi/mbedtls/library/debug.c
/Sources/yapi/mbedtls/library/error.c
/Sources/yapi/mbedtls/library/ssl_ciphersuites.c
/Sources/yapi/mbedtls/library/ssl_client.c
/Sources/yapi/mbedtls/library/ssl_cookie.c
/Sources/yapi/mbedtls/library/ssl_debug_helpers_generated.c
/Sources/yapi/mbedtls/library/ssl_msg.c
/Sources/yapi/mbedtls/library/ssl_tls.c
/Sources/yapi/mbedtls/library/ssl_tls12_client.c
/Sources/yapi/mbedtls/library/ssl_tls12_server.c
/Sources/yapi/mbedtls/library/ssl_tls13_client.c
/Sources/yapi/mbedtls/library/ssl_tls13_generic.c
/Sources/yapi/mbedtls/library/ssl_tls13_keys.c
/Sources/yapi/mbedtls/library/ssl_tls13_server.c
/Sources/yapi/mbedtls/library/x509.c
/Sources/yapi/mbedtls/library/x509_create.c
/Sources/yapi/mbedtls/library/x509_crt.c
/Sources/yapi/mbedtls/library/x509_oid.c
/Sources/yapi/mbedtls/library/x509write.c
/Sources/yapi/mbedtls/library/x509write_crt.c
/Sources/yapi/mbedtls/tf-psa-crypto/core/psa_crypto.c
/Sources/yapi/mbedtls/tf-psa-crypto/core/psa_crypto_client.c
/Sources/yapi/mbedtls/tf-psa-crypto/core/psa_crypto_driver_wrappers_no_static.c
/Sources/yapi/mbedtls/tf-psa-crypto/core/psa_crypto_random.c
/Sources/yapi/mbedtls/tf-psa-crypto/core/psa_crypto_slot_management.c
/Sources/yapi/mbedtls/tf-psa-crypto/core/psa_crypto_storage.c
/Sources/yapi/mbedtls/tf-psa-crypto/core/psa_its_file.c
/Sources/yapi/mbedtls/tf-psa-crypto/core/psa_util.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/aes.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/aesce.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/aesni.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/aria.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/bignum.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/bignum_core.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/block_cipher.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/camellia.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/ccm.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/chacha20.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/chacha20_neon.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/chachapoly.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/cipher.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/cipher_wrap.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/cmac.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/ctr_drbg.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/ecdsa.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/ecjpake.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/ecp.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/ecp_curves.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/entropy.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/entropy_poll.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/gcm.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/hmac_drbg.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/md5.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/poly1305.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/psa_crypto_aead.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/psa_crypto_cipher.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/psa_crypto_ecp.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/psa_crypto_ffdh.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/psa_crypto_hash.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/psa_crypto_mac.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/psa_crypto_pake.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/psa_crypto_rsa.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/psa_crypto_xof.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/psa_util_internal.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/ripemd160.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/rsa.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/rsa_alt_helpers.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/sha1.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/sha256.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/sha3.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/sha512.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/p256-m/p256-m/p256-m.c
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/p256-m/p256-m_driver_entrypoints.c
/Sources/yapi/mbedtls/tf-psa-crypto/extras/lmots.c
/Sources/yapi/mbedtls/tf-psa-crypto/extras/lms.c
/Sources/yapi/mbedtls/tf-psa-crypto/extras/md.c
/Sources/yapi/mbedtls/tf-psa-crypto/extras/nist_kw.c
/Sources/yapi/mbedtls/tf-psa-crypto/extras/pk.c
/Sources/yapi/mbedtls/tf-psa-crypto/extras/pk_ecc.c
/Sources/yapi/mbedtls/tf-psa-crypto/extras/pk_rsa.c
/Sources/yapi/mbedtls/tf-psa-crypto/extras/pk_wrap.c
/Sources/yapi/mbedtls/tf-psa-crypto/extras/pkparse.c
/Sources/yapi/mbedtls/tf-psa-crypto/extras/pkwrite.c
/Sources/yapi/mbedtls/tf-psa-crypto/platform/memory_buffer_alloc.c
/Sources/yapi/mbedtls/tf-psa-crypto/platform/platform.c
/Sources/yapi/mbedtls/tf-psa-crypto/platform/platform_util.c
/Sources/yapi/mbedtls/tf-psa-crypto/platform/threading.c
/Sources/yapi/mbedtls/tf-psa-crypto/utilities/asn1parse.c
/Sources/yapi/mbedtls/tf-psa-crypto/utilities/asn1write.c
/Sources/yapi/mbedtls/tf-psa-crypto/utilities/base64.c
/Sources/yapi/mbedtls/tf-psa-crypto/utilities/constant_time.c
/Sources/yapi/mbedtls/tf-psa-crypto/utilities/oid.c
/Sources/yapi/mbedtls/tf-psa-crypto/utilities/pem.c
/Sources/yapi/mbedtls/tf-psa-crypto/utilities/pkcs5.c
```

### C/C++ Header Files
```
/Sources/yocto_api.h
/Sources/yocto_accelerometer.h
/Sources/yocto_airquality.h
/Sources/yocto_altitude.h
/Sources/yocto_anbutton.h
/Sources/yocto_angularspeed.h
/Sources/yocto_arithmeticsensor.h
/Sources/yocto_audioin.h
/Sources/yocto_audioout.h
/Sources/yocto_bluetoothlink.h
/Sources/yocto_buzzer.h
/Sources/yocto_carbondioxide.h
/Sources/yocto_cellular.h
/Sources/yocto_colorled.h
/Sources/yocto_colorledcluster.h
/Sources/yocto_colorsensor.h
/Sources/yocto_compass.h
/Sources/yocto_counter.h
/Sources/yocto_current.h
/Sources/yocto_currentloopoutput.h
/Sources/yocto_daisychain.h
/Sources/yocto_digitalio.h
/Sources/yocto_display.h
/Sources/yocto_dualpower.h
/Sources/yocto_files.h
/Sources/yocto_genericsensor.h
/Sources/yocto_gps.h
/Sources/yocto_groundspeed.h
/Sources/yocto_gyro.h
/Sources/yocto_hubport.h
/Sources/yocto_humidity.h
/Sources/yocto_i2cport.h
/Sources/yocto_inputcapture.h
/Sources/yocto_inputchain.h
/Sources/yocto_latitude.h
/Sources/yocto_led.h
/Sources/yocto_lightsensor.h
/Sources/yocto_longitude.h
/Sources/yocto_magnetometer.h
/Sources/yocto_messagebox.h
/Sources/yocto_micropython.h
/Sources/yocto_motor.h
/Sources/yocto_multiaxiscontroller.h
/Sources/yocto_multicellweighscale.h
/Sources/yocto_multisenscontroller.h
/Sources/yocto_network.h
/Sources/yocto_orientation.h
/Sources/yocto_oscontrol.h
/Sources/yocto_power.h
/Sources/yocto_poweroutput.h
/Sources/yocto_powersupply.h
/Sources/yocto_pressure.h
/Sources/yocto_proximity.h
/Sources/yocto_pwminput.h
/Sources/yocto_pwmoutput.h
/Sources/yocto_pwmpowersource.h
/Sources/yocto_quadraturedecoder.h
/Sources/yocto_rangefinder.h
/Sources/yocto_realtimeclock.h
/Sources/yocto_refframe.h
/Sources/yocto_relay.h
/Sources/yocto_rfidreader.h
/Sources/yocto_sdi12port.h
/Sources/yocto_segmenteddisplay.h
/Sources/yocto_serialport.h
/Sources/yocto_servo.h
/Sources/yocto_soundlevel.h
/Sources/yocto_soundspectrum.h
/Sources/yocto_spectralchannel.h
/Sources/yocto_spiport.h
/Sources/yocto_steppermotor.h
/Sources/yocto_temperature.h
/Sources/yocto_threshold.h
/Sources/yocto_tilt.h
/Sources/yocto_tvoc.h
/Sources/yocto_virtualsensor.h
/Sources/yocto_voc.h
/Sources/yocto_voltage.h
/Sources/yocto_voltageoutput.h
/Sources/yocto_wakeupmonitor.h
/Sources/yocto_wakeupschedule.h
/Sources/yocto_watchdog.h
/Sources/yocto_weighscale.h
/Sources/yocto_wireless.h
/Sources/yapi/yapi.h
/Sources/yapi/ydef.h
/Sources/yapi/yfifo.h
/Sources/yapi/yhash.h
/Sources/yapi/yjson.h
/Sources/yapi/ykey.h
/Sources/yapi/ymemory.h
/Sources/yapi/yprog.h
/Sources/yapi/yproto.h
/Sources/yapi/ytcp.h
/Sources/yapi/ythread.h
/Sources/yapi/yversion.h
/Sources/yapi/yssl.h
/Sources/yapi/ydef_private.h
/Sources/yapi/mbedtls/include/mbedtls/build_info.h
/Sources/yapi/mbedtls/include/mbedtls/debug.h
/Sources/yapi/mbedtls/include/mbedtls/error.h
/Sources/yapi/mbedtls/include/mbedtls/mbedtls_config.h
/Sources/yapi/mbedtls/include/mbedtls/net_sockets.h
/Sources/yapi/mbedtls/include/mbedtls/oid.h
/Sources/yapi/mbedtls/include/mbedtls/pkcs7.h
/Sources/yapi/mbedtls/include/mbedtls/private/config_adjust_ssl.h
/Sources/yapi/mbedtls/include/mbedtls/private/config_adjust_x509.h
/Sources/yapi/mbedtls/include/mbedtls/ssl.h
/Sources/yapi/mbedtls/include/mbedtls/ssl_cache.h
/Sources/yapi/mbedtls/include/mbedtls/ssl_ciphersuites.h
/Sources/yapi/mbedtls/include/mbedtls/ssl_cookie.h
/Sources/yapi/mbedtls/include/mbedtls/ssl_ticket.h
/Sources/yapi/mbedtls/include/mbedtls/timing.h
/Sources/yapi/mbedtls/include/mbedtls/version.h
/Sources/yapi/mbedtls/include/mbedtls/x509.h
/Sources/yapi/mbedtls/include/mbedtls/x509_crl.h
/Sources/yapi/mbedtls/include/mbedtls/x509_crt.h
/Sources/yapi/mbedtls/include/mbedtls/x509_csr.h
/Sources/yapi/mbedtls/include/threading_alt.h
/Sources/yapi/mbedtls/library/debug_internal.h
/Sources/yapi/mbedtls/library/mbedtls_check_config.h
/Sources/yapi/mbedtls/library/mps_common.h
/Sources/yapi/mbedtls/library/mps_error.h
/Sources/yapi/mbedtls/library/mps_reader.h
/Sources/yapi/mbedtls/library/mps_trace.h
/Sources/yapi/mbedtls/library/ssl_ciphersuites_internal.h
/Sources/yapi/mbedtls/library/ssl_client.h
/Sources/yapi/mbedtls/library/ssl_debug_helpers.h
/Sources/yapi/mbedtls/library/ssl_misc.h
/Sources/yapi/mbedtls/library/ssl_tls13_invasive.h
/Sources/yapi/mbedtls/library/ssl_tls13_keys.h
/Sources/yapi/mbedtls/library/x509_internal.h
/Sources/yapi/mbedtls/library/x509_oid.h
/Sources/yapi/mbedtls/tf-psa-crypto/core/alignment.h
/Sources/yapi/mbedtls/tf-psa-crypto/core/psa_crypto_core.h
/Sources/yapi/mbedtls/tf-psa-crypto/core/psa_crypto_core_common.h
/Sources/yapi/mbedtls/tf-psa-crypto/core/psa_crypto_invasive.h
/Sources/yapi/mbedtls/tf-psa-crypto/core/psa_crypto_its.h
/Sources/yapi/mbedtls/tf-psa-crypto/core/psa_crypto_random_impl.h
/Sources/yapi/mbedtls/tf-psa-crypto/core/psa_crypto_slot_management.h
/Sources/yapi/mbedtls/tf-psa-crypto/core/psa_crypto_storage.h
/Sources/yapi/mbedtls/tf-psa-crypto/core/tf_psa_crypto_check_config.h
/Sources/yapi/mbedtls/tf-psa-crypto/core/tf_psa_crypto_common.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/include/mbedtls/private/aes.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/include/mbedtls/private/aria.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/include/mbedtls/private/bignum.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/include/mbedtls/private/block_cipher.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/include/mbedtls/private/camellia.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/include/mbedtls/private/ccm.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/include/mbedtls/private/chacha20.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/include/mbedtls/private/chachapoly.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/include/mbedtls/private/cipher.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/include/mbedtls/private/cmac.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/include/mbedtls/private/config_adjust_test_accelerators.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/include/mbedtls/private/ctr_drbg.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/include/mbedtls/private/ecdsa.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/include/mbedtls/private/ecjpake.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/include/mbedtls/private/ecp.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/include/mbedtls/private/entropy.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/include/mbedtls/private/error_common.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/include/mbedtls/private/gcm.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/include/mbedtls/private/hmac_drbg.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/include/mbedtls/private/md5.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/include/mbedtls/private/pkcs5.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/include/mbedtls/private/poly1305.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/include/mbedtls/private/ripemd160.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/include/mbedtls/private/rsa.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/include/mbedtls/private/sha1.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/include/mbedtls/private/sha256.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/include/mbedtls/private/sha3.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/include/mbedtls/private/sha512.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/include/mbedtls/private_access.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/aesce.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/aesni.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/bignum_core.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/bignum_core_invasive.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/bignum_internal.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/bignum_mod.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/bignum_mod_raw.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/bignum_mod_raw_invasive.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/block_cipher_internal.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/bn_mul.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/cipher_invasive.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/cipher_wrap.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/ctr.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/ecp_invasive.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/entropy_poll.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/md_psa.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/psa_crypto_aead.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/psa_crypto_cipher.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/psa_crypto_ecp.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/psa_crypto_ffdh.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/psa_crypto_hash.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/psa_crypto_mac.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/psa_crypto_pake.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/psa_crypto_rsa.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/psa_util_internal.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/rsa_alt_helpers.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/builtin/src/rsa_internal.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/everest/include/tf-psa-crypto/private/everest/Hacl_Curve25519.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/everest/include/tf-psa-crypto/private/everest/kremlib.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/everest/include/tf-psa-crypto/private/everest/kremlib/FStar_UInt128.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/everest/include/tf-psa-crypto/private/everest/kremlib/FStar_UInt64_FStar_UInt32_FStar_UInt16_FStar_UInt8.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/everest/include/tf-psa-crypto/private/everest/kremlin/c_endianness.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/everest/include/tf-psa-crypto/private/everest/kremlin/internal/builtin.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/everest/include/tf-psa-crypto/private/everest/kremlin/internal/callconv.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/everest/include/tf-psa-crypto/private/everest/kremlin/internal/compat.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/everest/include/tf-psa-crypto/private/everest/kremlin/internal/debug.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/everest/include/tf-psa-crypto/private/everest/kremlin/internal/target.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/everest/include/tf-psa-crypto/private/everest/kremlin/internal/types.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/everest/include/tf-psa-crypto/private/everest/kremlin/internal/wasmsupport.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/everest/include/tf-psa-crypto/private/everest/vs2013/Hacl_Curve25519.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/everest/include/tf-psa-crypto/private/everest/x25519.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/p256-m/p256-m/p256-m.h
/Sources/yapi/mbedtls/tf-psa-crypto/drivers/p256-m/p256-m_driver_entrypoints.h
/Sources/yapi/mbedtls/tf-psa-crypto/extras/lmots.h
/Sources/yapi/mbedtls/tf-psa-crypto/extras/md_wrap.h
/Sources/yapi/mbedtls/tf-psa-crypto/extras/pk_internal.h
/Sources/yapi/mbedtls/tf-psa-crypto/extras/pk_wrap.h
/Sources/yapi/mbedtls/tf-psa-crypto/extras/pkwrite.h
/Sources/yapi/mbedtls/tf-psa-crypto/include/mbedtls/asn1.h
/Sources/yapi/mbedtls/tf-psa-crypto/include/mbedtls/asn1write.h
/Sources/yapi/mbedtls/tf-psa-crypto/include/mbedtls/base64.h
/Sources/yapi/mbedtls/tf-psa-crypto/include/mbedtls/compat-3-crypto.h
/Sources/yapi/mbedtls/tf-psa-crypto/include/mbedtls/constant_time.h
/Sources/yapi/mbedtls/tf-psa-crypto/include/mbedtls/lms.h
/Sources/yapi/mbedtls/tf-psa-crypto/include/mbedtls/md.h
/Sources/yapi/mbedtls/tf-psa-crypto/include/mbedtls/memory_buffer_alloc.h
/Sources/yapi/mbedtls/tf-psa-crypto/include/mbedtls/nist_kw.h
/Sources/yapi/mbedtls/tf-psa-crypto/include/mbedtls/pem.h
/Sources/yapi/mbedtls/tf-psa-crypto/include/mbedtls/pk.h
/Sources/yapi/mbedtls/tf-psa-crypto/include/mbedtls/platform.h
/Sources/yapi/mbedtls/tf-psa-crypto/include/mbedtls/platform_time.h
/Sources/yapi/mbedtls/tf-psa-crypto/include/mbedtls/platform_util.h
/Sources/yapi/mbedtls/tf-psa-crypto/include/mbedtls/private/pk_private.h
/Sources/yapi/mbedtls/tf-psa-crypto/include/mbedtls/psa_util.h
/Sources/yapi/mbedtls/tf-psa-crypto/include/mbedtls/threading.h
/Sources/yapi/mbedtls/tf-psa-crypto/include/psa/crypto.h
/Sources/yapi/mbedtls/tf-psa-crypto/include/psa/crypto_compat.h
/Sources/yapi/mbedtls/tf-psa-crypto/include/psa/crypto_config.h
/Sources/yapi/mbedtls/tf-psa-crypto/include/psa/crypto_driver_common.h
/Sources/yapi/mbedtls/tf-psa-crypto/include/psa/crypto_driver_contexts_composites.h
/Sources/yapi/mbedtls/tf-psa-crypto/include/psa/crypto_driver_contexts_key_derivation.h
/Sources/yapi/mbedtls/tf-psa-crypto/include/psa/crypto_driver_contexts_primitives.h
/Sources/yapi/mbedtls/tf-psa-crypto/include/psa/crypto_driver_random.h
/Sources/yapi/mbedtls/tf-psa-crypto/include/psa/crypto_extra.h
/Sources/yapi/mbedtls/tf-psa-crypto/include/psa/crypto_platform.h
/Sources/yapi/mbedtls/tf-psa-crypto/include/psa/crypto_sizes.h
/Sources/yapi/mbedtls/tf-psa-crypto/include/psa/crypto_struct.h
/Sources/yapi/mbedtls/tf-psa-crypto/include/psa/crypto_types.h
/Sources/yapi/mbedtls/tf-psa-crypto/include/psa/crypto_values.h
/Sources/yapi/mbedtls/tf-psa-crypto/include/tf-psa-crypto/build_info.h
/Sources/yapi/mbedtls/tf-psa-crypto/include/tf-psa-crypto/version.h
/Sources/yapi/mbedtls/tf-psa-crypto/utilities/base64_internal.h
/Sources/yapi/mbedtls/tf-psa-crypto/utilities/constant_time_impl.h
/Sources/yapi/mbedtls/tf-psa-crypto/utilities/constant_time_internal.h
/Sources/yapi/mbedtls/tf-psa-crypto/utilities/crypto_oid.h
```
## License Notice for Mbed TLS Library

Yoctopuce library for C++ v2.0 uses Mbed TLS Library, which is subject to Apache License 2.0.

```
                             Apache License
                       Version 2.0, January 2004
                    http://www.apache.org/licenses/

TERMS AND CONDITIONS FOR USE, REPRODUCTION, AND DISTRIBUTION

1. Definitions.

  "License" shall mean the terms and conditions for use, reproduction,
  and distribution as defined by Sections 1 through 9 of this document.

  "Licensor" shall mean the copyright owner or entity authorized by
  the copyright owner that is granting the License.

  "Legal Entity" shall mean the union of the acting entity and all
  other entities that control, are controlled by, or are under common
  control with that entity. For the purposes of this definition,
  "control" means (i) the power, direct or indirect, to cause the
  direction or management of such entity, whether by contract or
  otherwise, or (ii) ownership of fifty percent (50%) or more of the
  outstanding shares, or (iii) beneficial ownership of such entity.

  "You" (or "Your") shall mean an individual or Legal Entity
  exercising permissions granted by this License.

  "Source" form shall mean the preferred form for making modifications,
  including but not limited to software source code, documentation
  source, and configuration files.

  "Object" form shall mean any form resulting from mechanical
  transformation or translation of a Source form, including but
  not limited to compiled object code, generated documentation,
  and conversions to other media types.

  "Work" shall mean the work of authorship, whether in Source or
  Object form, made available under the License, as indicated by a
  copyright notice that is included in or attached to the work
  (an example is provided in the Appendix below).

  "Derivative Works" shall mean any work, whether in Source or Object
  form, that is based on (or derived from) the Work and for which the
  editorial revisions, annotations, elaborations, or other modifications
  represent, as a whole, an original work of authorship. For the purposes
  of this License, Derivative Works shall not include works that remain
  separable from, or merely link (or bind by name) to the interfaces of,
  the Work and Derivative Works thereof.

  "Contribution" shall mean any work of authorship, including
  the original version of the Work and any modifications or additions
  to that Work or Derivative Works thereof, that is intentionally
  submitted to Licensor for inclusion in the Work by the copyright owner
  or by an individual or Legal Entity authorized to submit on behalf of
  the copyright owner. For the purposes of this definition, "submitted"
  means any form of electronic, verbal, or written communication sent
  to the Licensor or its representatives, including but not limited to
  communication on electronic mailing lists, source code control systems,
  and issue tracking systems that are managed by, or on behalf of, the
  Licensor for the purpose of discussing and improving the Work, but
  excluding communication that is conspicuously marked or otherwise
  designated in writing by the copyright owner as "Not a Contribution."

  "Contributor" shall mean Licensor and any individual or Legal Entity
  on behalf of whom a Contribution has been received by Licensor and
  subsequently incorporated within the Work.

2. Grant of Copyright License. Subject to the terms and conditions of
  this License, each Contributor hereby grants to You a perpetual,
  worldwide, non-exclusive, no-charge, royalty-free, irrevocable
  copyright license to reproduce, prepare Derivative Works of,
  publicly display, publicly perform, sublicense, and distribute the
  Work and such Derivative Works in Source or Object form.

3. Grant of Patent License. Subject to the terms and conditions of
  this License, each Contributor hereby grants to You a perpetual,
  worldwide, non-exclusive, no-charge, royalty-free, irrevocable
  (except as stated in this section) patent license to make, have made,
  use, offer to sell, sell, import, and otherwise transfer the Work,
  where such license applies only to those patent claims licensable
  by such Contributor that are necessarily infringed by their
  Contribution(s) alone or by combination of their Contribution(s)
  with the Work to which such Contribution(s) was submitted. If You
  institute patent litigation against any entity (including a
  cross-claim or counterclaim in a lawsuit) alleging that the Work
  or a Contribution incorporated within the Work constitutes direct
  or contributory patent infringement, then any patent licenses
  granted to You under this License for that Work shall terminate
  as of the date such litigation is filed.

4. Redistribution. You may reproduce and distribute copies of the
  Work or Derivative Works thereof in any medium, with or without
  modifications, and in Source or Object form, provided that You
  meet the following conditions:

  (a) You must give any other recipients of the Work or
      Derivative Works a copy of this License; and

  (b) You must cause any modified files to carry prominent notices
      stating that You changed the files; and

  (c) You must retain, in the Source form of any Derivative Works
      that You distribute, all copyright, patent, trademark, and
      attribution notices from the Source form of the Work,
      excluding those notices that do not pertain to any part of
      the Derivative Works; and

  (d) If the Work includes a "NOTICE" text file as part of its
      distribution, then any Derivative Works that You distribute must
      include a readable copy of the attribution notices contained
      within such NOTICE file, excluding those notices that do not
      pertain to any part of the Derivative Works, in at least one
      of the following places: within a NOTICE text file distributed
      as part of the Derivative Works; within the Source form or
      documentation, if provided along with the Derivative Works; or,
      within a display generated by the Derivative Works, if and
      wherever such third-party notices normally appear. The contents
      of the NOTICE file are for informational purposes only and
      do not modify the License. You may add Your own attribution
      notices within Derivative Works that You distribute, alongside
      or as an addendum to the NOTICE text from the Work, provided
      that such additional attribution notices cannot be construed
      as modifying the License.

  You may add Your own copyright statement to Your modifications and
  may provide additional or different license terms and conditions
  for use, reproduction, or distribution of Your modifications, or
  for any such Derivative Works as a whole, provided Your use,
  reproduction, and distribution of the Work otherwise complies with
  the conditions stated in this License.

5. Submission of Contributions. Unless You explicitly state otherwise,
  any Contribution intentionally submitted for inclusion in the Work
  by You to the Licensor shall be under the terms and conditions of
  this License, without any additional terms or conditions.
  Notwithstanding the above, nothing herein shall supersede or modify
  the terms of any separate license agreement you may have executed
  with Licensor regarding such Contributions.

6. Trademarks. This License does not grant permission to use the trade
  names, trademarks, service marks, or product names of the Licensor,
  except as required for reasonable and customary use in describing the
  origin of the Work and reproducing the content of the NOTICE file.

7. Disclaimer of Warranty. Unless required by applicable law or
  agreed to in writing, Licensor provides the Work (and each
  Contributor provides its Contributions) on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
  implied, including, without limitation, any warranties or conditions
  of TITLE, NON-INFRINGEMENT, MERCHANTABILITY, or FITNESS FOR A
  PARTICULAR PURPOSE. You are solely responsible for determining the
  appropriateness of using or redistributing the Work and assume any
  risks associated with Your exercise of permissions under this License.

8. Limitation of Liability. In no event and under no legal theory,
  whether in tort (including negligence), contract, or otherwise,
  unless required by applicable law (such as deliberate and grossly
  negligent acts) or agreed to in writing, shall any Contributor be
  liable to You for damages, including any direct, indirect, special,
  incidental, or consequential damages of any character arising as a
  result of this License or out of the use or inability to use the
  Work (including but not limited to damages for loss of goodwill,
  work stoppage, computer failure or malfunction, or any and all
  other commercial damages or losses), even if such Contributor
  has been advised of the possibility of such damages.

9. Accepting Warranty or Additional Liability. While redistributing
  the Work or Derivative Works thereof, You may choose to offer,
  and charge a fee for, acceptance of support, warranty, indemnity,
  or other liability obligations and/or rights consistent with this
  License. However, in accepting such obligations, You may act only
  on Your own behalf and on Your sole responsibility, not on behalf
  of any other Contributor, and only if You agree to indemnify,
  defend, and hold each Contributor harmless for any liability
  incurred by, or claims asserted against, such Contributor by reason
  of your accepting any such warranty or additional liability.

END OF TERMS AND CONDITIONS

APPENDIX: How to apply the Apache License to your work.

  To apply the Apache License to your work, attach the following
  boilerplate notice, with the fields enclosed by brackets "[]"
  replaced with your own identifying information. (Don't include
  the brackets!)  The text should be enclosed in the appropriate
  comment syntax for the file format. We also recommend that a
  file or class name and description of purpose be included on the
  same "printed page" as the copyright notice for easier
  identification within third-party archives.

Copyright [yyyy] [name of copyright owner]

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```