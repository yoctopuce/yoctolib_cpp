/*********************************************************************
 *
 * $Id: yssl.c 60747 2024-04-25 08:15:16Z seb $
 *
 * Implementation of a client TCP stack with SSL
 *
 * - - - - - - - - - License information: - - - - - - - - -
 *
 *  Copyright (C) 2011 and beyond by Yoctopuce Sarl, Switzerland.
 *
 *  Yoctopuce Sarl (hereafter Licensor) grants to you a perpetual
 *  non-exclusive license to use, modify, copy and integrate this
 *  file into your software for the sole purpose of interfacing
 *  with Yoctopuce products.
 *
 *  You may reproduce and distribute copies of this file in
 *  source or object form, as long as the sole purpose of this
 *  code is to interface with Yoctopuce products. You must retain
 *  this notice in the distributed source file.
 *
 *  You should refer to Yoctopuce General Terms and Conditions
 *  for additional information regarding your rights and
 *  obligations.
 *
 *  THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 *  WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING
 *  WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS
 *  FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO
 *  EVENT SHALL LICENSOR BE LIABLE FOR ANY INCIDENTAL, SPECIAL,
 *  INDIRECT OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA,
 *  COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR
 *  SERVICES, ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT
 *  LIMITED TO ANY DEFENSE THEREOF), ANY CLAIMS FOR INDEMNITY OR
 *  CONTRIBUTION, OR OTHER SIMILAR COSTS, WHETHER ASSERTED ON THE
 *  BASIS OF CONTRACT, TORT (INCLUDING NEGLIGENCE), BREACH OF
 *  WARRANTY, OR OTHERWISE.
 *
 *********************************************************************/

#include "ydef_private.h"
#define __FILE_ID__     MK_FILEID('S','S','L')
#define __FILENAME__   "yssl"

#ifndef NO_YSSL
#include "mbedtls/build_info.h"
#include "mbedtls/ssl.h"
#include "mbedtls/debug.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls/error.h"
#include "mbedtls/threading.h"
#include <mbedtls/base64.h>
#include <mbedtls/pem.h>
#endif

#include "yproto.h"
#include "ytcp.h"
#include "yssl.h"


#ifndef NO_YSSL
#include "mbedtls/platform.h"

#ifdef WINDOWS_API
#define SOCK_ERR    (WSAGetLastError())
#else
#define SOCK_ERR    (errno)
#endif
#define REPORT_ERR(msg) if(errmsg){ YSPRINTF(errmsg,YOCTO_ERRMSG_LEN,"%s (%s:%d / errno=%d)",(msg), __FILENAME__, __LINE__, SOCK_ERR);errmsg[YOCTO_ERRMSG_LEN-1]='\0';}


#define FMT_MBEDTLS_ERR(errno) format_mbedtls_err(__FILENAME__, __LINE__, errno, errmsg)

static int format_mbedtls_err(const char *fileid, int lineno, int err, char *errmsg)
{
    int errcode = YAPI_SSL_ERROR;
    int ofs;
    const char *clearmsg;

#ifdef DEBUG_SSL
    dbglogf(fileid, lineno, "mbedtls error %d (%s)\n", err, errmsg);
#endif
    // handle error codes missing in mbedtls_strerror
    switch (err) {
    case MBEDTLS_ERR_NET_SOCKET_FAILED:
        clearmsg = "Failed to open a socket";
        errcode = YAPI_IO_ERROR;
        break;
    case MBEDTLS_ERR_NET_CONNECT_FAILED:
        clearmsg = "The connection to the given server / port failed";
        errcode = YAPI_IO_ERROR;
        break;
    case MBEDTLS_ERR_NET_BIND_FAILED:
        clearmsg = "Binding of the socket failed";
        errcode = YAPI_IO_ERROR;
        break;
    case MBEDTLS_ERR_NET_LISTEN_FAILED:
        clearmsg = "Could not listen on the socket";
        errcode = YAPI_IO_ERROR;
        break;
    case MBEDTLS_ERR_NET_ACCEPT_FAILED:
        clearmsg = "Could not accept the incoming connection";
        errcode = YAPI_IO_ERROR;
        break;
    case MBEDTLS_ERR_NET_RECV_FAILED:
        clearmsg = "Reading information from the socket failed";
        errcode = YAPI_IO_ERROR;
        break;
    case MBEDTLS_ERR_NET_SEND_FAILED:
        clearmsg = "Sending information through the socket failed";
        errcode = YAPI_IO_ERROR;
        break;
    case MBEDTLS_ERR_NET_CONN_RESET:
        clearmsg = "Connection was reset by peer";
        errcode = YAPI_IO_ERROR;
        break;
    case MBEDTLS_ERR_NET_UNKNOWN_HOST:
        clearmsg = "Failed to get an IP address for the given hostname";
        errcode = YAPI_IO_ERROR;
        break;
    case MBEDTLS_ERR_NET_BUFFER_TOO_SMALL:
        clearmsg = "Buffer is too small to hold the data";
        break;
    case MBEDTLS_ERR_NET_INVALID_CONTEXT:
        clearmsg = "The context is invalid, eg because it was free()ed";
        break;
    case MBEDTLS_ERR_NET_POLL_FAILED:
        clearmsg = "Polling the net context failed";
        errcode = YAPI_IO_ERROR;
        break;
    case MBEDTLS_ERR_NET_BAD_INPUT_DATA:
        clearmsg = "Input invalid";
        break;
    default:
        ofs = YSPRINTF(errmsg, YOCTO_ERRMSG_LEN, "%s:%d:", fileid, lineno);
        mbedtls_strerror(err, errmsg + ofs, YOCTO_ERRMSG_LEN - ofs);
        return YAPI_SSL_ERROR;
    }
    YSPRINTF(errmsg, YOCTO_ERRMSG_LEN, "%s:%d:%s", fileid, lineno, clearmsg);
    return errcode;
}

static mbedtls_x509_crt cachain, srvcert;
static mbedtls_ctr_drbg_context ctr_drbg;
static mbedtls_entropy_context entropy;
static mbedtls_pk_context pkey;

#ifdef DEBUG_SSL

static void my_debug(void* ctx, int level, const char* file, int line, const char* str)
{
    dbglog("%s:%04d: %s", file, line, str);
}

#endif

int yssl_generate_private_key(const char *keyfile, u32 nbits, char *errmsg)
{
    int ret;
    mbedtls_pk_context key;
    FILE *fd;
    unsigned char output_buf[16000];
    unsigned char *c = output_buf;
    size_t len = 0;

    mbedtls_pk_init(&key);
    ret = mbedtls_pk_setup(&key, mbedtls_pk_info_from_type(MBEDTLS_PK_RSA));
    if (ret != 0) {
        return FMT_MBEDTLS_ERR(ret);
    }

    ret = mbedtls_rsa_gen_key(mbedtls_pk_rsa(key), mbedtls_ctr_drbg_random, &ctr_drbg, nbits, 65537);
    if (ret != 0) {
        return FMT_MBEDTLS_ERR(ret);
    }


    memset(output_buf, 0, 16000);
    if ((ret = mbedtls_pk_write_key_pem(&key, output_buf, 16000)) != 0)
        return (ret);

    len = strlen((char*)output_buf);

    if (YFOPEN(&fd, keyfile, "wb") != 0) {
        YSPRINTF(errmsg, YOCTO_ERRMSG_LEN, "Unable to save private key to file %s", keyfile);
        return YAPI_IO_ERROR;
    }

    if (fwrite(c, 1, len, fd) != len) {
        fclose(fd);
        YSPRINTF(errmsg, YOCTO_ERRMSG_LEN, "Unable to write private in file %s", keyfile);
        return YAPI_IO_ERROR;
    }

    fclose(fd);

    return YAPI_SUCCESS;
}


int yssl_write_certificate(void *crt_void, const char *certfilename, char *errmsg)
{
    int ret;
    FILE *fd;
    unsigned char buffer[4096];
    size_t len = 0;
    mbedtls_x509write_cert *crt = crt_void;

    memset(buffer, 0, 4096);
    ret = mbedtls_x509write_crt_pem(crt, buffer, 4096, mbedtls_ctr_drbg_random, &ctr_drbg);
    if (ret < 0) {
        return FMT_MBEDTLS_ERR(ret);
    }

    len = strlen((char*)buffer);

    if (YFOPEN(&fd, certfilename, "w") != 0) {
        YSPRINTF(errmsg, YOCTO_ERRMSG_LEN, "Unable to save certificate in file %s", certfilename);
        return YAPI_IO_ERROR;
    }

    if (fwrite(buffer, 1, len, fd) != len) {
        fclose(fd);
        YSPRINTF(errmsg, YOCTO_ERRMSG_LEN, "Unable to write certificate in file %s", certfilename);
        return YAPI_IO_ERROR;
    }
    fclose(fd);
    return YAPI_SUCCESS;
}

static mbedtls_x509_san_list* alloc_san_obj(const char *host)
{
    uint8_t ipv6[16] = {0};
    size_t ip_addr_len;
    mbedtls_x509_san_list *san_ptr = yMalloc(sizeof(mbedtls_x509_san_list));
    memset(san_ptr, 0, sizeof(mbedtls_x509_san_list));
    ip_addr_len = mbedtls_x509_crt_parse_cn_inet_pton(host, ipv6);
    if (ip_addr_len != 0) {
        san_ptr->node.type = MBEDTLS_X509_SAN_IP_ADDRESS;
        san_ptr->node.san.unstructured_name.len = ip_addr_len;
        san_ptr->node.san.unstructured_name.p = (unsigned char*)yMalloc(san_ptr->node.san.unstructured_name.len);
        memcpy(san_ptr->node.san.unstructured_name.p, ipv6, ip_addr_len);
    } else {
        san_ptr->node.type = MBEDTLS_X509_SAN_DNS_NAME;
        san_ptr->node.san.unstructured_name.len = strlen(host);
        san_ptr->node.san.unstructured_name.p = (unsigned char*)yMalloc(san_ptr->node.san.unstructured_name.len);
        memcpy(san_ptr->node.san.unstructured_name.p, host, san_ptr->node.san.unstructured_name.len);
    }
    return san_ptr;
}

int yssl_generate_certificate(const char *keyfile, const char *certfile,
                              const char *country, const char *state,
                              const char *organisation, const char *common_name,
                              const os_ifaces *ifaces, int nbiface,
                              time_t expiration, char *errmsg)
{
    mbedtls_pk_context key;
    mbedtls_x509write_cert crt;
    mbedtls_mpi serial;
    char subject_name[1024];
    uint8_t rand_serial[20];
    struct tm timeinfo;
    time_t rawtime;
    char from[16];
    char to[16];
    int i;
    const char *fmt = "%Y%m%d%H%M%S";
    mbedtls_x509_san_list *san_list = alloc_san_obj("localhost");
    mbedtls_x509_san_list *san_p = san_list;


    mbedtls_pk_init(&key);
    mbedtls_x509write_crt_init(&crt);

    int ret = mbedtls_pk_parse_keyfile(&key, keyfile, NULL,
                                       mbedtls_ctr_drbg_random, &ctr_drbg);
    if (ret != 0) {
        return FMT_MBEDTLS_ERR(ret);
    }

    mbedtls_mpi_init(&serial);
    mbedtls_ctr_drbg_random(&ctr_drbg, rand_serial, 20);
    ret = mbedtls_mpi_read_binary(&serial, rand_serial, 20);
    if (ret != 0) {
        return FMT_MBEDTLS_ERR(ret);
    }

    // self signed certificate use same key for subject and issuer
    mbedtls_x509write_crt_set_subject_key(&crt, &key);
    mbedtls_x509write_crt_set_issuer_key(&crt, &key);

    YSPRINTF(subject_name, 1024, "C=%s,ST=%s,O=%s",
             country, state, organisation
    );

    if (common_name) {
        YSTRCAT(subject_name, 1024, ",CN=");
        YSTRCAT(subject_name, 1024, common_name);
    }
    san_p->next = alloc_san_obj("::1");
    san_p = san_p->next;
    san_p->next = alloc_san_obj("127.0.0.1");
    for (i = 0; i < nbiface; i++) {
        unsigned char bytes[4];
        char buffer[128];
        san_p = san_p->next;
        bytes[0] = ifaces[i].ip & 0xFF;
        bytes[1] = (ifaces[i].ip >> 8) & 0xFF;
        bytes[2] = (ifaces[i].ip >> 16) & 0xFF;
        bytes[3] = (ifaces[i].ip >> 24) & 0xFF;
        YSPRINTF(buffer, 125, "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
        san_p->next = alloc_san_obj(buffer);
    }
    ret = mbedtls_x509write_crt_set_subject_alternative_name(&crt, san_list);
    if (ret != 0) {
        return FMT_MBEDTLS_ERR(ret);
    }
    while (san_list) {
        san_p = san_list->next;
        yFree(san_list->node.san.unstructured_name.p);
        yFree(san_list);
        san_list = san_p;
    }
    ret = mbedtls_x509write_crt_set_subject_name(&crt, subject_name);
    if (ret != 0) {
        return FMT_MBEDTLS_ERR(ret);
    }
    ret = mbedtls_x509write_crt_set_issuer_name(&crt, subject_name);
    if (ret != 0) {
        return FMT_MBEDTLS_ERR(ret);
    }
    mbedtls_x509write_crt_set_version(&crt, MBEDTLS_X509_CRT_VERSION_3);
    mbedtls_x509write_crt_set_md_alg(&crt, MBEDTLS_MD_SHA256);
    ret = mbedtls_x509write_crt_set_serial(&crt, &serial);
    if (ret != 0) {
        return FMT_MBEDTLS_ERR(ret);
    }
    // compute time string
    time(&rawtime);
    ygmtime(&timeinfo, &rawtime);
    strftime(from, sizeof(from), fmt, &timeinfo);
    rawtime += expiration;
    ygmtime(&timeinfo, &rawtime);
    strftime(to, sizeof(to), fmt, &timeinfo);

    ret = mbedtls_x509write_crt_set_validity(&crt, from, to);
    if (ret != 0) {
        return FMT_MBEDTLS_ERR(ret);
    }
    ret = mbedtls_x509write_crt_set_basic_constraints(&crt, 1, -1);
    if (ret < 0) {
        return FMT_MBEDTLS_ERR(ret);
    }
    ret = mbedtls_x509write_crt_set_subject_key_identifier(&crt);
    if (ret != 0) {
        return FMT_MBEDTLS_ERR(ret);
    }
    ret = yssl_write_certificate(&crt, certfile, errmsg);
    mbedtls_pk_free(&pkey);
    return ret;
}

static void yssl_mutex_init(mbedtls_threading_mutex_t *mutex)
{
    yInitializeCriticalSection(&mutex->mutex);
}

static void yssl_mutex_free(mbedtls_threading_mutex_t *mutex)
{
    yDeleteCriticalSection(&mutex->mutex);
}

static int yssl_mutex_lock(mbedtls_threading_mutex_t *mutex)
{
    yEnterCriticalSection(&mutex->mutex);
    return 0;
}

static int yssl_mutex_unlock(mbedtls_threading_mutex_t *mutex)
{
    yLeaveCriticalSection(&mutex->mutex);
    return 0;
}

int yTcpInitSSL(char *errmsg)
{
    int ret;
#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
    psa_status_t status;
#endif
    SSLLOG("Init OpenSSL\n");


#ifdef DEBUG_SSL
    // activate debug logs
    //mbedtls_debug_set_threshold(1);
#endif


    mbedtls_threading_set_alt(yssl_mutex_init,
                              yssl_mutex_free,
                              yssl_mutex_lock,
                              yssl_mutex_unlock);


    mbedtls_x509_crt_init(&cachain);
    mbedtls_x509_crt_init(&srvcert);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_pk_init(&pkey);
    SSLLOG("Seeding the random number generator...\n");
    mbedtls_entropy_init(&entropy);
    ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func,
                                &entropy, NULL, 0);
    if (ret != 0) {
        return FMT_MBEDTLS_ERR(ret);
    }
#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
    status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        return YERRMSG(YAPI_SSL_ERROR, "Failed to initialize PSA Crypto");
    }
#endif
#ifdef ADD_DEFAULT_X509_CERTS
    ret = mbedtls_x509_crt_parse(&cachain, (const unsigned char*)SSL_CA_PEM,
        sizeof(SSL_CA_PEM));

    if (ret != 0) {
        return FMT_MBEDTLS_ERR(ret);
    }
#endif
    return YAPI_SUCCESS;
}


// must be called after TcpInitSSL
int yTcpSetSrvCertificateSSL(const char *certfile, const char *keyfile, char *errmsg)
{
    int ret;
    FILE *fd;

    /* Load certificate and private key files, and check consistency */
    if (keyfile) {
        if (YFOPEN(&fd, keyfile, "r") != 0) {
            return YERRMSG(YAPI_SSL_ERROR, "Private key file does not exist!");
        }
        fclose(fd);

        mbedtls_pk_free(&pkey);
        ret = mbedtls_pk_parse_keyfile(&pkey, keyfile, NULL,
                                       mbedtls_ctr_drbg_random, &ctr_drbg);
        if (ret < 0) {
            return FMT_MBEDTLS_ERR(ret);
        }
        SSLLOG("certificate and private key loaded and verified\n");
    } else {
        mbedtls_pk_free(&pkey);
        mbedtls_pk_init(&pkey);
    }

    if (certfile) {
        // load settings file
        if (YFOPEN(&fd, certfile, "r") != 0) {
            return YERRMSG(YAPI_SSL_ERROR, "SSL certificate file does not exist!");
        }
        fclose(fd);
        mbedtls_x509_crt_free(&srvcert);
        ret = mbedtls_x509_crt_parse_file(&srvcert, certfile);
        if (ret < 0) {
            return FMT_MBEDTLS_ERR(ret);
        }
        SSLLOG("certificate and private key loaded and verified\n");
    } else {
        mbedtls_x509_crt_free(&srvcert);
        mbedtls_x509_crt_init(&srvcert);
    }

    return YAPI_SUCCESS;
}

int yTcpAddClientCertificateSSL(const u8 *cert, u32 cert_len, char *errmsg)
{

    int ret = mbedtls_x509_crt_parse(&cachain, (const unsigned char*)cert, cert_len);
    if (ret != 0) {
        return FMT_MBEDTLS_ERR(ret);
    }
    return YAPI_SUCCESS;
}

int yTcpDownloadSSLCert(const char *host, int port, u64 mstimeout, u8 *buffer, u32 maxsize, u32 *neededsize, char *errmsg)
{
    YSSL_SOCKET sslSkt;
    const char *pem_begin_crt = "-----BEGIN CERTIFICATE-----\n";
    const char *pem_end_crt = "-----END CERTIFICATE-----\n";
    int res;
    res = yTcpOpenSSL(&sslSkt, host, port, 1, mstimeout, errmsg);
    if (res < 0) {
        return res;
    }

    // allow non trusted certificate since we have no root CA installed
    const mbedtls_x509_crt *chain;
    chain = mbedtls_ssl_get_peer_cert(sslSkt->ssl);
    *neededsize = 0;
    res = YAPI_SUCCESS;
    while (chain != NULL) {
        size_t written = 0;
        int mbedtls_res = mbedtls_pem_write_buffer(
            pem_begin_crt, pem_end_crt, chain->raw.p, chain->raw.len,
            buffer, maxsize, &written);
        if (mbedtls_res == MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL) {
            *neededsize += (u32)written;
            if (errmsg) {
                YSPRINTF(errmsg, YOCTO_ERRMSG_LEN, "Certificate is too big to fit in the buffer");
            }
            res = YAPI_BUFFER_TOO_SMALL;
        } else if (mbedtls_res < 0) {
            yTcpCloseSSL(sslSkt);
            return FMT_MBEDTLS_ERR(mbedtls_res);
        } else {
            if (written > 0) {
                written--; //mbedtls_pem_write_buffer return a null terminated string
                *neededsize += (u32)written;
                buffer += (u32)written;
                maxsize -= (u32)written;
            }
        }
        chain = chain->next;
    }
    yTcpCloseSSL(sslSkt);
    return res;
}

void yTcpShutdownSSL(void)
{
    SSLLOG("YSSL: shutdown\n");

#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
    mbedtls_psa_crypto_free();
#endif
    mbedtls_x509_crt_free(&cachain);
    mbedtls_x509_crt_free(&srvcert);
    mbedtls_pk_free(&pkey);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
    mbedtls_threading_free_alt();
}


static int mbedtls_ysend(void *ctx, const unsigned char *buf, size_t tosend)
{
    char errmsg[YOCTO_ERRMSG_LEN];
    YSSL_SOCKET yssl = ctx;
    //dbglog("need to send %d bytes encrypted\n", tosend);
    int res = yTcpWriteBasic(yssl->tcpskt, buf, (int)tosend, errmsg);
    if (res < 0) {
        return MBEDTLS_ERR_NET_SEND_FAILED;
    }
    //dbglog("sent %d bytes encrypted on socket %d\n", res, yssl->tcpskt);
    return res;
}


// The callback must return the number of bytes received, or a non-zero error code.
// If performing non-blocking I/O, MBEDTLS_ERR_SSL_WANT_READ must be returned when the operation would block.

static int mbedtls_yread(void *ctx, unsigned char *buf, size_t avail)
{
    char errmsg[YOCTO_ERRMSG_LEN];
    YSSL_SOCKET yssl = ctx;
    //dbglog("try to read %d bytes encrypted on socket %d\n", avail, yssl->tcpskt);
    int nread = yTcpReadBasic(yssl->tcpskt, buf, (int)avail, errmsg);
    //dbglog("nread %d bytes encrypted on socket %d\n", nread, yssl->tcpskt);
    if (nread == 0) {
        return MBEDTLS_ERR_SSL_WANT_READ;
    } else if (nread == YAPI_NO_MORE_DATA) {
        return 0;
    } else if (nread < 0) {
        dbglog("Unable to read encrypted data(%s)\n", errmsg);
        return MBEDTLS_ERR_NET_RECV_FAILED;
    }

    return nread;
}


#define CAN_BE_SOLVED_BY_SEC_OPTIONS 1
#define CAN_BE_SOLVED_BY_ADD_CERT 2

static int do_ssl_handshake(YSSL_SOCKET yssl, int skip_cert_validation, char *errmsg)
{
    int ret;
    u32 flags;

    while ((ret = mbedtls_ssl_handshake(yssl->ssl)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            return FMT_MBEDTLS_ERR(ret);
        }
    }
    SSLLOG("SSL handshake done\n");
    SSLLOG("SSL Verifying peer X.509 certificate...\n");
    if ((flags = mbedtls_ssl_get_verify_result(yssl->ssl)) != 0) {
        int err_msg_flag = 0;
        if (yssl->flags & YSSL_TCP_SERVER_MODE && flags == MBEDTLS_X509_BADCERT_SKIP_VERIFY) {
            return YAPI_SUCCESS;
        }
        if (flags & MBEDTLS_X509_BADCERT_NOT_TRUSTED) {
            if ((yContext->sslCertOptions & YAPI_NO_TRUSTED_CA_CHECK) || skip_cert_validation) {
                // allow non trusted certificate since we have no root CA installed
                flags &= ~MBEDTLS_X509_BADCERT_NOT_TRUSTED;
            } else {
                err_msg_flag |= CAN_BE_SOLVED_BY_SEC_OPTIONS | CAN_BE_SOLVED_BY_ADD_CERT;
            }
        }
        if (flags & MBEDTLS_X509_BADCERT_EXPIRED) {
            if ((yContext->sslCertOptions & YAPI_NO_EXPIRATION_CHECK) || skip_cert_validation) {
                // allow non trusted certificate since we have no root CA installed
                flags &= ~MBEDTLS_X509_BADCERT_EXPIRED;
            } else {
                err_msg_flag |= CAN_BE_SOLVED_BY_SEC_OPTIONS;
            }
        }
        if (flags & MBEDTLS_X509_BADCERT_CN_MISMATCH) {
            if ((yContext->sslCertOptions & YAPI_NO_HOSTNAME_CHECK) || skip_cert_validation) {
                // allow non trusted certificate since we have no root CA installed
                flags &= ~MBEDTLS_X509_BADCERT_CN_MISMATCH;
            } else {
                err_msg_flag |= CAN_BE_SOLVED_BY_SEC_OPTIONS;
            }
        }
        if (flags) {
            int len, hint_len = 0;
            char long_errmsg[2048];
            const char *hint = NULL;
            mbedtls_x509_crt_verify_info(long_errmsg, 2048,
                                         "", flags);
            len = ystrlen(long_errmsg);
            // remove trailing \n
            while (len > 0 && long_errmsg[len - 1] == '\n') {
                long_errmsg[len - 1] = 0;
                len--;
            }

            if (err_msg_flag == (CAN_BE_SOLVED_BY_SEC_OPTIONS | CAN_BE_SOLVED_BY_ADD_CERT)) {
                hint = ". Consider using YAPI.AddTrustedCertificates or YAPI.SetNetworkSecurityOptions methods.";
                hint_len = ystrlen(hint);
            } else if (err_msg_flag == CAN_BE_SOLVED_BY_SEC_OPTIONS) {
                hint = ". Consider using YAPI.AddTrustedCertificates method.";
                hint_len = ystrlen(hint);
            } else if (err_msg_flag == CAN_BE_SOLVED_BY_SEC_OPTIONS) {
                hint = ". Consider using YAPI.AddTrustedCertificates method.";
                hint_len = ystrlen(hint);
            }
            if (len + 1 < (YOCTO_ERRMSG_LEN - hint_len)) {
                ystrcpy(errmsg, YOCTO_ERRMSG_LEN, long_errmsg);
            } else {
                char buff[YOCTO_ERRMSG_LEN];
                int nberr = 0;
                int i, tail_size, used;
                const char *tail = "...]";

                for (i = 0; i < 32; i++) {
                    if (flags & (1 << i)) {
                        nberr++;
                    }
                }
                YSPRINTF(buff, YOCTO_ERRMSG_LEN, "SSL handshake failed with %d error%s (res=%x)\n[", nberr, nberr > 1 ? "s" : "", flags);
                ystrcat(errmsg, YOCTO_ERRMSG_LEN, buff);
                tail_size = ystrlen(tail) + 1;
                used = ystrlen(errmsg) + hint_len;
                ystrncpy(errmsg + used, YOCTO_ERRMSG_LEN - used, long_errmsg, YOCTO_ERRMSG_LEN - used-1);
                memcpy(errmsg + YOCTO_ERRMSG_LEN - tail_size, tail, tail_size);
            }
            if (hint_len) {
                ystrcat(errmsg, YOCTO_ERRMSG_LEN, hint);
            }
            if (flags & MBEDTLS_X509_BADCERT_NOT_TRUSTED) {
                return YAPI_SSL_UNK_CERT;
            }
            return YAPI_SSL_ERROR;
        }
    }
#ifdef DEBUG_SSL
    else {
        dbglog("SSL remote criticate is valid\n");
    }
#endif
    return YAPI_SUCCESS;
}


static int setup_ssl(yssl_socket_st *yssl, const char *remote_hostname, int skip_cert_validation, char *errmsg)
{
    int res;
    int server_mode = yssl->flags & YSSL_TCP_SERVER_MODE;

    // we cannot share mbedtls config can be between multiples context
    // since some of our socket work as client and other work as server.
    yssl->ssl_conf = yMalloc(sizeof(mbedtls_ssl_config));
    mbedtls_ssl_config_init(yssl->ssl_conf);
    res = mbedtls_ssl_config_defaults(yssl->ssl_conf,
                                      server_mode ? MBEDTLS_SSL_IS_SERVER : MBEDTLS_SSL_IS_CLIENT,
                                      MBEDTLS_SSL_TRANSPORT_STREAM,
                                      MBEDTLS_SSL_PRESET_DEFAULT);

    if (res != 0) {
        yFree(yssl->ssl_conf);
        return FMT_MBEDTLS_ERR(res);
    }

#ifdef DEBUG_SSL
    // activate debug logs
    mbedtls_ssl_conf_dbg(yssl->ssl_conf, my_debug, yssl);
#endif
    if (!server_mode) {
        mbedtls_ssl_conf_ca_chain(yssl->ssl_conf, &cachain, NULL);
    }
    mbedtls_ssl_conf_rng(yssl->ssl_conf, mbedtls_ctr_drbg_random, &ctr_drbg);

    if (server_mode) {
        res = mbedtls_ssl_conf_own_cert(yssl->ssl_conf, &srvcert, &pkey);
        if (res != 0) {
            yFree(yssl->ssl_conf);
            return FMT_MBEDTLS_ERR(res);
        }
    } else {
        mbedtls_ssl_conf_authmode(yssl->ssl_conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
    }


    // create SSL context
    yssl->ssl = yMalloc(sizeof(mbedtls_ssl_context));
    mbedtls_ssl_init(yssl->ssl);
    if ((res = mbedtls_ssl_setup(yssl->ssl, yssl->ssl_conf)) != 0) {
        yFree(yssl->ssl_conf);
        yFree(yssl->ssl);
        return FMT_MBEDTLS_ERR(res);
    }
    if (remote_hostname && (res = mbedtls_ssl_set_hostname(yssl->ssl, remote_hostname)) != 0) {
        yFree(yssl->ssl_conf);
        yFree(yssl->ssl);
        return FMT_MBEDTLS_ERR(res);
    }

    mbedtls_ssl_set_bio(yssl->ssl, yssl, mbedtls_ysend, mbedtls_yread, NULL);

    // do handshake
    res = do_ssl_handshake(yssl, skip_cert_validation, errmsg);
    if (res < 0) {
        yFree(yssl->ssl_conf);
        yFree(yssl->ssl);
        return res;
    }
    return YAPI_SUCCESS;
}


int yTcpOpenSSL(YSSL_SOCKET*newskt, const char *host, u16 port, int skip_cert_validation, u64 mstimeout, char *errmsg)
{
    int res;
    yssl_socket_st *yssl;

    SSLLOG("YSSL: openssl %p [dst=%s:%d %dms]\n", newskt, host, port, mstimeout);
    yssl = yMalloc(sizeof(yssl_socket_st));
    memset(yssl, 0, sizeof(yssl_socket_st));

    res = yTcpOpenBasic(&yssl->tcpskt, host, port, mstimeout, errmsg);
    if (res < 0) {
        return res;
    }

    res = setup_ssl(yssl, host, skip_cert_validation, errmsg);
    if (res < 0) {
        yFree(yssl);
        return res;
    }

    SSLLOG("SSL socket opened\n");
    *newskt = yssl;
    return YAPI_SUCCESS;
}

int yTcpAcceptSSL(YSSL_SOCKET*newskt, YSOCKET sock, char *errmsg)
{
    int res;
    yssl_socket_st *yssl;
    SSLLOG("YSSL: accept %p [skt=%d]\n", newskt, sock);
    yssl = yMalloc(sizeof(yssl_socket_st));
    memset(yssl, 0, sizeof(yssl_socket_st));
    yssl->flags |= YSSL_TCP_SERVER_MODE;
    yssl->tcpskt = sock;
    res = setup_ssl(yssl, NULL, 0, errmsg);
    if (res < 0) {
        yFree(yssl);
        return res;
    }
    SSLLOG("SSL socket opened\n");
    *newskt = yssl;
    return YAPI_SUCCESS;
}


void yTcpCloseSSL(YSSL_SOCKET yssl)
{
    SSLLOG("YSSL: close (sock=%p)\n", yssl);
    yTcpCloseBasic(yssl->tcpskt);
    mbedtls_ssl_free(yssl->ssl);
    mbedtls_ssl_config_free(yssl->ssl_conf);
    yFree(yssl->ssl);
    yFree(yssl->ssl_conf);
    yFree(yssl);
}

YSOCKET yTcpFdSetSSL(YSSL_SOCKET yssl, void *set, YSOCKET sktmax)
{
#if 0
    //SSLLOG("SSL: FD_SET %p\n", yssl);
    char errmsg[YOCTO_ERRMSG_LEN];
    int res = ssl_flush_on_socket(yssl, 0, errmsg);
    if (res < 0) {
        dbglog("SSL err %d:%s\n", res, errmsg);
    }

#endif
    FD_SET(yssl->tcpskt, (fd_set*)set);
    if (yssl->tcpskt > sktmax) {
        sktmax = yssl->tcpskt;
    }
    //int pending = mbedtls_ssl_check_pending(yssl->ssl);
    //SSLLOG("YSSL: %d bytes pending on setfd\n", pending);
    return sktmax;
}

int yTcpFdIsSetSSL(YSSL_SOCKET yssl, void *set)
{
    int res = FD_ISSET(yssl->tcpskt, (fd_set*)set);
    //dbglog("YSSL: socket is_set -> %d\n", res);
    if (!res) {
        int peek_res = mbedtls_ssl_check_pending(yssl->ssl);
        //dbglog("YSSL: fd_isset=%d peek returned %d -> %d\n", res, peek_res);
        if (peek_res > 0) {
            res = 1;
        }
    }
    //SSLLOG("SSL: FD_ISSET %p->%d\n", yssl, res);
    return res;
}


// check it a socket is still valid and empty (ie: nothing to read and writable)
// return 1 if the socket is valid or a error code
int yTcpCheckSocketStillValidSSL(YSSL_SOCKET yssl, char *errmsg)
{
    SSLLOG("YSSL: check validity (sock=%p)\n", yssl);
    if (yssl->flags & YSSL_TCP_SOCK_CLOSED) {
        return 0;
    }
    return yTcpCheckSocketStillValidBasic(yssl->tcpskt, errmsg);;
}


int yTcpWriteSSL(YSSL_SOCKET yssl, const u8 *buffer, int len, char *errmsg)
{
    SSLLOG("YSSL: write %d  bytes (sock=%p)\n", len, yssl);

    int consumed = mbedtls_ssl_write(yssl->ssl, buffer, len);
    if (consumed <= 0) {
        if (consumed == MBEDTLS_ERR_SSL_WANT_WRITE || consumed == MBEDTLS_ERR_SSL_WANT_READ) {
            return 0;
        } else {
            return FMT_MBEDTLS_ERR(consumed);
        }
    }

    return consumed;
}


int yTcpReadSSL(YSSL_SOCKET yssl, u8 *buffer, int len, char *errmsg)
{
    int res;
    int nread = 0;
    u8 *ptr = buffer;
    SSLLOG("YSSL: look for data on sock %p (buflen=%d)\n", yssl, len);

    if (yssl->flags & YSSL_TCP_SOCK_CLOSED) {
        // previous call may have ended with connection close
        return YERR(YAPI_NO_MORE_DATA);
    }

    do {
        int decrypted = 0;
        res = mbedtls_ssl_read(yssl->ssl, ptr, len);
        if (res == 0 || res == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
            yssl->flags |= YSSL_TCP_SOCK_CLOSED;
        } else if (res < 0) {
            if (res != MBEDTLS_ERR_SSL_WANT_READ && res != MBEDTLS_ERR_SSL_WANT_WRITE) {
                return FMT_MBEDTLS_ERR(res);
            }
        } else {
            decrypted = res;
        }
        ptr += decrypted;
        len -= decrypted;
        nread += decrypted;
    } while (len > 0 && res > 0);

    if (nread == 0 && yssl->flags & YSSL_TCP_SOCK_CLOSED) {
        return YERR(YAPI_NO_MORE_DATA);
    }

    SSLLOG("YSSL: read %d  bytes (sock=%p)\n", nread, yssl);
    return nread;
}


u32 yTcpGetRcvBufSizeSSL(YSSL_SOCKET skt)
{
    return yTcpGetRcvBufSizeBasic(skt->tcpskt);
}

#endif
