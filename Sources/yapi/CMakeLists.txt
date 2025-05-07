cmake_minimum_required(VERSION 3.8)

project(yapi VERSION 1.0.1 DESCRIPTION "Yoctopuce YAPI V2 library")

option(USE_YSSL "Include SSL/TLS support" ON)

if(USE_YSSL)
message(STATUS "Include Mbed TLS library")
set(MBEDTLS_C_FILES
	mbedtls/library/aes.c
	mbedtls/library/aesce.c
	mbedtls/library/aesni.c
	mbedtls/library/aria.c
	mbedtls/library/asn1parse.c
	mbedtls/library/asn1write.c
	mbedtls/library/base64.c
	mbedtls/library/bignum.c
	mbedtls/library/bignum_core.c
	mbedtls/library/camellia.c
	mbedtls/library/ccm.c
	mbedtls/library/chacha20.c
	mbedtls/library/chachapoly.c
	mbedtls/library/cipher.c
	mbedtls/library/cipher_wrap.c
	mbedtls/library/cmac.c
	mbedtls/library/constant_time.c
	mbedtls/library/ctr_drbg.c
	mbedtls/library/debug.c
	mbedtls/library/des.c
	mbedtls/library/dhm.c
	mbedtls/library/ecdh.c
	mbedtls/library/ecdsa.c
	mbedtls/library/ecjpake.c
	mbedtls/library/ecp.c
	mbedtls/library/ecp_curves.c
	mbedtls/library/entropy.c
	mbedtls/library/entropy_poll.c
	mbedtls/library/error.c
	mbedtls/library/gcm.c
	mbedtls/library/hkdf.c
	mbedtls/library/hmac_drbg.c
	mbedtls/library/lmots.c
	mbedtls/library/lms.c
	mbedtls/library/md.c
	mbedtls/library/md5.c
	mbedtls/library/net_sockets.c
	mbedtls/library/nist_kw.c
	mbedtls/library/oid.c
	mbedtls/library/padlock.c
	mbedtls/library/pem.c
	mbedtls/library/pk.c
	mbedtls/library/pk_ecc.c
	mbedtls/library/pkcs12.c
	mbedtls/library/pkcs5.c
	mbedtls/library/pkcs7.c
	mbedtls/library/pkparse.c
	mbedtls/library/pkwrite.c
	mbedtls/library/pk_wrap.c
	mbedtls/library/platform.c
	mbedtls/library/platform_util.c
	mbedtls/library/poly1305.c
	mbedtls/library/psa_crypto.c
	mbedtls/library/psa_crypto_aead.c
	mbedtls/library/psa_crypto_cipher.c
	mbedtls/library/psa_crypto_client.c
	mbedtls/library/psa_crypto_driver_wrappers_no_static.c
	mbedtls/library/psa_crypto_ecp.c
	mbedtls/library/psa_crypto_ffdh.c
	mbedtls/library/psa_crypto_hash.c
	mbedtls/library/psa_crypto_mac.c
	mbedtls/library/psa_crypto_pake.c
	mbedtls/library/psa_crypto_rsa.c
	mbedtls/library/psa_crypto_slot_management.c
	mbedtls/library/psa_crypto_storage.c
	mbedtls/library/psa_its_file.c
	mbedtls/library/psa_util.c
	mbedtls/library/ripemd160.c
	mbedtls/library/rsa.c
	mbedtls/library/rsa_alt_helpers.c
	mbedtls/library/sha1.c
	mbedtls/library/sha256.c
	mbedtls/library/sha3.c
	mbedtls/library/sha512.c
	mbedtls/library/ssl_cache.c
	mbedtls/library/ssl_ciphersuites.c
	mbedtls/library/ssl_client.c
	mbedtls/library/ssl_cookie.c
	mbedtls/library/ssl_debug_helpers_generated.c
	mbedtls/library/ssl_msg.c
	mbedtls/library/ssl_ticket.c
	mbedtls/library/ssl_tls.c
	mbedtls/library/ssl_tls12_client.c
	mbedtls/library/ssl_tls12_server.c
	mbedtls/library/ssl_tls13_client.c
	mbedtls/library/ssl_tls13_generic.c
	mbedtls/library/ssl_tls13_keys.c
	mbedtls/library/ssl_tls13_server.c
	mbedtls/library/threading.c
	mbedtls/library/timing.c
	mbedtls/library/version.c
	mbedtls/library/version_features.c
	mbedtls/library/x509.c
	mbedtls/library/x509write.c
	mbedtls/library/x509write_crt.c
	mbedtls/library/x509write_csr.c
	mbedtls/library/x509_create.c
	mbedtls/library/x509_crl.c
	mbedtls/library/x509_crt.c
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

add_library (yapi ${YAPI_C_FILES}  ${MBEDTLS_C_FILES})
if(USE_YSSL)
	target_include_directories(yapi PRIVATE mbedtls/include)
endif()

target_include_directories (yapi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# add pthread usb-1.0 library only on linux
if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
target_link_libraries (yapi LINK_PUBLIC pthread usb-1.0)
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
install(TARGETS yapi DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES yapi.h ydef.h yversion.h yjson.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yapi)


