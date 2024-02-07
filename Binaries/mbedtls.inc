

OBJS_CRYPTO= \
	     aes.o \
	     aesni.o \
	     arc4.o \
	     aria.o \
	     asn1parse.o \
	     asn1write.o \
	     base64.o \
	     bignum.o \
	     blowfish.o \
	     camellia.o \
	     ccm.o \
	     chacha20.o \
	     chachapoly.o \
	     cipher.o \
	     cipher_wrap.o \
	     cmac.o \
	     ctr_drbg.o \
	     des.o \
	     dhm.o \
	     ecdh.o \
	     ecdsa.o \
	     ecjpake.o \
	     ecp.o \
	     ecp_curves.o \
	     entropy.o \
	     entropy_poll.o \
	     error.o \
	     gcm.o \
	     havege.o \
	     hkdf.o \
	     hmac_drbg.o \
	     md.o \
	     md2.o \
	     md4.o \
	     md5.o \
	     memory_buffer_alloc.o \
	     nist_kw.o \
	     oid.o \
	     padlock.o \
	     pem.o \
	     pk.o \
	     pk_wrap.o \
	     pkcs12.o \
	     pkcs5.o \
	     pkparse.o \
	     pkwrite.o \
	     platform.o \
	     platform_util.o \
	     poly1305.o \
	     psa_crypto.o \
	     psa_crypto_client.o \
	     psa_crypto_driver_wrappers.o \
	     psa_crypto_ecp.o \
	     psa_crypto_rsa.o \
	     psa_crypto_se.o \
	     psa_crypto_slot_management.o \
	     psa_crypto_storage.o \
	     psa_its_file.o \
	     ripemd160.o \
	     rsa.o \
	     rsa_internal.o \
	     sha1.o \
	     sha256.o \
	     sha512.o \
	     threading.o \
	     timing.o \
	     version.o \
	     version_features.o \
	     xtea.o \
	     # This line is intentionally left blank

OBJS_X509= \
	   certs.o \
	   pkcs11.o \
	   x509.o \
	   x509_create.o \
	   x509_crl.o \
	   x509_crt.o \
	   x509_csr.o \
	   x509write_crt.o \
	   x509write_csr.o \
	   # This line is intentionally left blank

OBJS_TLS= \
	  debug.o \
	  net_sockets.o \
	  ssl_cache.o \
	  ssl_ciphersuites.o \
	  ssl_cli.o \
	  ssl_cookie.o \
	  ssl_msg.o \
	  ssl_srv.o \
	  ssl_ticket.o \
	  ssl_tls.o \
	  ssl_tls13_keys.o \
	  # This line is intentionally left blank


MBED_TLS_SRC_FILES=  $(patsubst %.o, %.c, $(OBJS_CRYPTO) $(OBJS_X509) $(OBJS_TLS))