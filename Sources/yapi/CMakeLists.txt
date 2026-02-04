cmake_minimum_required(VERSION 3.8)

project(yapi VERSION 1.0.1 DESCRIPTION "Yoctopuce YAPI V2 library")

option(USE_YSSL "Include SSL/TLS support" ON)

if(USE_YSSL)
message(STATUS "Include Mbed TLS library")
set(MBEDTLS_C_FILES
#-- mbedtls_patch_start --
	mbedtls/library/error.c
	mbedtls/library/x509write_crt.c
	mbedtls/library/x509_create.c
	mbedtls/library/debug.c
	mbedtls/library/x509.c
	mbedtls/library/x509_crt.c
	mbedtls/library/x509write.c
	mbedtls/library/x509_oid.c
	mbedtls/library/ssl_tls.c
	mbedtls/library/ssl_client.c
	mbedtls/library/ssl_msg.c
	mbedtls/library/ssl_ciphersuites.c
	mbedtls/library/ssl_tls13_client.c
	mbedtls/library/ssl_tls12_client.c
	mbedtls/library/ssl_tls12_server.c
	mbedtls/library/ssl_tls13_generic.c
	mbedtls/library/ssl_tls13_server.c
	mbedtls/library/ssl_tls13_keys.c
	mbedtls/tf-psa-crypto/core/psa_its_file.c
	mbedtls/tf-psa-crypto/core/psa_crypto_slot_management.c
	mbedtls/tf-psa-crypto/core/psa_crypto.c
	mbedtls/tf-psa-crypto/core/psa_crypto_client.c
	mbedtls/tf-psa-crypto/core/psa_crypto_storage.c
	mbedtls/tf-psa-crypto/core/psa_crypto_driver_wrappers_no_static.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/cmac.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/md5.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/gcm.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/aes.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/aesce.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/rsa.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/pk_ecc.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/oid.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/poly1305.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/ctr_drbg.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/chacha20.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/ecdh.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/ecdsa.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/cipher_wrap.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/base64.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/pk_rsa.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/pkcs5.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/aesni.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/rsa_alt_helpers.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/entropy_poll.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/hmac_drbg.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/camellia.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/aria.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/ripemd160.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/sha1.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/sha256.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/sha512.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/sha3.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/ccm.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/chachapoly.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/ecjpake.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/psa_crypto_mac.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/psa_crypto_rsa.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/psa_crypto_ecp.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/psa_crypto_ffdh.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/pk_wrap.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/ecp.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/cipher.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/md.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/psa_crypto_aead.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/psa_crypto_hash.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/psa_crypto_pake.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/constant_time.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/ecp_curves.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/psa_crypto_cipher.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/entropy.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/platform_util.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/threading.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/asn1write.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/asn1parse.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/pem.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/pk.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/pkparse.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/pkwrite.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/bignum.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/psa_util.c
	mbedtls/tf-psa-crypto/drivers/builtin/src/bignum_core.c

#-- mbedtls_patch_stop --
)
else()
add_compile_definitions(NO_YSSL)
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND ${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
SET(CMAKE_INSTALL_PREFIX "/usr")
endif()
include(GNUInstallDirs)

set(YAPI_C_FILES
	yapi.c
	ystream.c
	yprog.c
	yfifo.c
	ykey.c
	yhash.c
	yjson.c
	ytcp.c
	ymemory.c
	ythread.c
	yjni.c
	ypkt_win.c
	ypkt_osx.c
	ypkt_lin.c
	yssl.c
)

add_library (yapi STATIC ${YAPI_C_FILES}  ${MBEDTLS_C_FILES})
if(USE_YSSL)
#-- mbedtls_patch_inc_start --
    target_include_directories(yapi PRIVATE mbedtls/include)
    target_include_directories(yapi PRIVATE mbedtls/tf-psa-crypto/core)
    target_include_directories(yapi PRIVATE mbedtls/tf-psa-crypto/drivers/builtin/include)
    target_include_directories(yapi PRIVATE mbedtls/tf-psa-crypto/drivers/builtin/src)
    target_include_directories(yapi PRIVATE mbedtls/tf-psa-crypto/include)

#-- mbedtls_patch_inc_stop --
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
target_link_libraries(yapi PRIVATE bcrypt.lib)
endif()
endif()

target_include_directories(yapi PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/yapi>
)

# add pthread usb-1.0 library only on linux
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set(THREADS_PREFER_PTHREAD_FLAG TRUE)
	find_package(Threads REQUIRED)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(libusb REQUIRED IMPORTED_TARGET libusb-1.0)
	target_link_libraries(yapi LINK_PUBLIC Threads::Threads PkgConfig::libusb)
endif()

if (APPLE)
	find_library(CORE_FRAMEWORK CoreFoundation)
	if (NOT CORE_FRAMEWORK)
	    message(FATAL_ERROR "CoreFoundation not found")
	endif()
	find_library(IOKIT_FRAMEWORK IOKit)
	if (NOT IOKIT_FRAMEWORK)
	    message(FATAL_ERROR "IOKit not found")
	endif()
	target_link_libraries(yapi LINK_PUBLIC ${IOKIT_FRAMEWORK} ${CORE_FRAMEWORK})
endif()

set_target_properties(yapi PROPERTIES SOVERSION 1)
