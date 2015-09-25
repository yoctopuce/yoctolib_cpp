/*********************************************************************
 *
 * $Id: yapi.c 21383 2015-09-02 11:55:24Z seb $
 *
 * Implementation of public entry points to the low-level API
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
#define __FILE_ID__  "yjni"

#include "yapi.h"
#include "yproto.h"
#include "yhash.h"
#include "yjson.h"
#include "yprog.h"

#ifdef YAPI_WITH_JNI

#include <jni.h>
#include <stdio.h>
#include "yjni.h"



 jint throwYAPI_Exception( JNIEnv *env, char *message )
{
    jclass exClass;
    char *className = "com/yoctopuce/YoctoAPI/YAPI_Exception" ;
    dbglog("Exception:%s\n", message);
    exClass = (*env)->FindClass( env, className );
    return (*env)->ThrowNew( env, exClass, message );
}


/*
 * Class:     com_yoctopuce_YoctoAPI_YJniWrapper
 * Method:    getAPIVersion
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_yoctopuce_YoctoAPI_YJniWrapper_getAPIVersion(JNIEnv *env, jclass thisObj)
{
    const char *version;
    const char *apidate;
    yapiGetAPIVersion(&version, &apidate);

    return (*env)->NewStringUTF(env, version);
}


/*
 * Class:     com_yoctopuce_YoctoAPI_YJniWrapper
 * Method:    reserveUSBAccess
 * Signature: ()V;
 */
JNIEXPORT void JNICALL Java_com_yoctopuce_YoctoAPI_YJniWrapper_reserveUSBAccess(JNIEnv *env, jclass thisObj)
{
    char errmsg[YOCTO_ERRMSG_LEN];
    YRETCODE res;

    res = yapiInitAPI(Y_DETECT_USB, errmsg);
    if(YISERR(res)) {
        throwYAPI_Exception(env, errmsg);
    }
}

/*
 * Class:     com_yoctopuce_YoctoAPI_YJniWrapper
 * Method:    releaseUSBAccess
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_yoctopuce_YoctoAPI_YJniWrapper_releaseUSBAccess(JNIEnv *env, jclass thisObj)
{
    yapiFreeAPI();
}


/*
 * Class:     com_yoctopuce_YoctoAPI_YJniWrapper
 * Method:    getBootloaders
 * Signature: ()Ljava/util/String;
 */
JNIEXPORT jobject JNICALL Java_com_yoctopuce_YoctoAPI_YJniWrapper_getBootloaders(JNIEnv *env, jclass thisObj)
{
    char errmsg[YOCTO_ERRMSG_LEN];
    char buffer[1024];
    char *p;
    int fullsize;
    YRETCODE res;
    jobject result;


    res = yapiGetBootloaders(buffer, 1024, &fullsize, errmsg);
    if(YISERR(res)) {
        throwYAPI_Exception(env, errmsg);
    }
    if (res == fullsize) {
        return (*env)->NewStringUTF(env, buffer);
    }

    p = yMalloc(fullsize+1);
    memset(p, 0, fullsize+1);
    res = yapiGetBootloaders(buffer, fullsize, &fullsize, errmsg);
    if(YISERR(res)) {
        yFree(p);
        throwYAPI_Exception(env, errmsg);
    }
    result = (*env)->NewStringUTF(env, buffer);
    yFree(p);
    return result;
}


static jobject allocWPEntry(JNIEnv *env, yDeviceSt *infos)
{
    jstring logicalName;
    jstring productName;
    jint productId;
    jstring networkUrl;
    jint beacon;
    jstring serialNumber;
    jobject res;

    jclass cls = (*env)->FindClass(env, "com/yoctopuce/YoctoAPI/WPEntry");
    if (cls == 0) {
        throwYAPI_Exception(env, "Unable to find class WPEntry");
        return NULL;
    }

    jmethodID constructor = (*env)->GetMethodID(env, cls, "<init>", "(Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;ILjava/lang/String;)V");
    if (constructor == 0) {
        throwYAPI_Exception(env, "Unable to find constructor for WPEntry");
        return NULL;
    }

    logicalName = (*env)->NewStringUTF(env, infos->logicalname);
    productName = (*env)->NewStringUTF(env, infos->productname);
    networkUrl = (*env)->NewStringUTF(env, "");
    serialNumber = (*env)->NewStringUTF(env, infos->serial);
    productId = infos->vendorid;
    beacon = 0; //fixme : use real beacon vallue

    res = (*env)->NewObject(env, cls, constructor, logicalName, productName, productId, networkUrl, beacon, serialNumber);
    return res;

}



/*
 * Class:     com_yoctopuce_YoctoAPI_YJniWrapper
 * Method:    updateDeviceList
 * Signature: (Ljava/util/ArrayList;Ljava/util/HashMap;)V
 */
JNIEXPORT void JNICALL Java_com_yoctopuce_YoctoAPI_YJniWrapper_updateDeviceList(JNIEnv *env, jclass thisObj, jobject wpArray, jobject ypMap)
{
    char    errmsg[YOCTO_ERRMSG_LEN];
    YAPI_DEVICE *buffer,*p;
    int     nbdev, buffsize, i;
    jobject wpEntry;
    jclass arrayList_class;
    jmethodID arrayList_add;

    printf("JNI: test\n");


    if (yapiUpdateDeviceList(1, errmsg) < 0) {
        throwYAPI_Exception(env, errmsg);
        return;
    }


    if (yapiGetAllDevices(NULL, 0, &buffsize, errmsg) < 0) {
        throwYAPI_Exception(env, errmsg);
        return;
    }
    buffer = (YAPI_DEVICE *)yMalloc(buffsize);
    nbdev = yapiGetAllDevices(buffer, buffsize, &buffsize, errmsg);

    arrayList_class = (*env)->FindClass(env, "java/util/ArrayList");
    if (arrayList_class == 0) {
        throwYAPI_Exception(env, "Unable to find class ArrayList");
    }
    arrayList_add = (*env)->GetMethodID(env, arrayList_class, "add", "(Ljava/lang/Object;)Z");
    if (arrayList_add == 0) {
        throwYAPI_Exception(env, "Unable to find add method of ArrayList");
        return;
    }

    printf("JNI: %d devices detected\n", nbdev);

    p = buffer;
    for (i = 0 ; i < nbdev; i++, p++) {
        yDeviceSt infos;
        if (yapiGetDeviceInfo(*p, &infos, errmsg) < 0) {
            throwYAPI_Exception(env, errmsg);
            return;
        }
        wpEntry = allocWPEntry(env, &infos);
        dbglog("JNI: add it to wp (%X)\n", wpEntry);
        (*env)->CallBooleanMethod(env, wpArray, arrayList_add, wpEntry);
        dbglog("JNI: added (%X)\n", wpEntry);
        //(*env)->DeleteLocalRef(env, wpEntry);

    }

    yFree(buffer);


}


/*
 * Class:     com_yoctopuce_YoctoAPI_YJniWrapper
 * Method:    devRequestSync
 * Signature: (Ljava/lang/String;[B)[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_yoctopuce_YoctoAPI_YJniWrapper_devRequestSync(JNIEnv *env, jclass thisObj, jstring serial_java, jbyteArray request_java)
{
    char        errmsg[YOCTO_ERRMSG_LEN];
    YRETCODE    res;
    YIOHDL      iohdl;
    char        *reply;
    int         replysize = 0;
    const char  *serial = NULL;
    jbyte       *request_bytes = NULL;
    jsize       length;
    jbyteArray  result = NULL;

    // get serial
    serial = (*env)->GetStringUTFChars(env, serial_java, NULL);
    if (NULL == serial) {
        throwYAPI_Exception(env, "Invalid String");
        goto exit;
    }

    // get request
    request_bytes = (*env)->GetByteArrayElements(env, request_java, NULL);
    if (NULL == request_bytes) {
        throwYAPI_Exception(env, "Invalid Byte Array");
        goto exit;
    }
    length = (*env)->GetArrayLength(env, request_java);


    if(YISERR(res=yapiHTTPRequestSyncStartEx(&iohdl, serial, request_bytes, length, &reply, &replysize, errmsg))) {
        throwYAPI_Exception(env, errmsg);
        goto exit;
    }
    if (replysize < 0 || reply == NULL) {
        replysize = 0;
    }

   // compute return value
   result = (*env)->NewByteArray(env, replysize);  // allocate
   if (NULL == result) {
        throwYAPI_Exception(env, "Unable to allocate bytes array");
        goto exit;
    }

    if (replysize > 0) {
        (*env)->SetByteArrayRegion(env, result, 0 , replysize, reply);  // copy
    }

    if(YISERR(res=yapiHTTPRequestSyncDone(&iohdl, errmsg))) {
        throwYAPI_Exception(env, errmsg);
    }

exit:
    if (serial != NULL) {
        (*env)->ReleaseStringUTFChars(env, serial_java, serial);  // release resources
    }

    if (request_bytes != NULL) {
       (*env)->ReleaseByteArrayElements(env, request_java, request_bytes, 0); // release resources
    }
    return result;
}


/*
 * Class:     com_yoctopuce_YoctoAPI_YJniWrapper
 * Method:    devRequestAsync
 * Signature: (Ljava/lang/String;[BLcom/yoctopuce/YoctoAPI/YGenericHub/RequestAsyncResult;Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_com_yoctopuce_YoctoAPI_YJniWrapper_devRequestAsync(JNIEnv *env, jclass thisObj, jstring serial_java, jbyteArray request_java, jobject callback, jobject context)
{
    char        errmsg[YOCTO_ERRMSG_LEN];
    YRETCODE    res;
    const char  *serial = NULL;
    jbyte       *request_bytes = NULL;
    jsize       length;

    // get serial
    serial = (*env)->GetStringUTFChars(env, serial_java, NULL);
    if (NULL == serial) {
        throwYAPI_Exception(env, "Invalid String");
        goto exit;
    }

    // get request
    request_bytes = (*env)->GetByteArrayElements(env, request_java, NULL);
    if (NULL == request_bytes) {
        throwYAPI_Exception(env, "Invalid Byte Array");
        goto exit;
    }
    length = (*env)->GetArrayLength(env, request_java);


    if(YISERR(res=yapiHTTPRequestAsyncEx(serial, request_bytes, length, NULL, NULL, errmsg))) {
        throwYAPI_Exception(env, errmsg);
        goto exit;
    }

exit:
    if (serial != NULL) {
        (*env)->ReleaseStringUTFChars(env, serial_java, serial);  // release resources
    }

    if (request_bytes != NULL) {
       (*env)->ReleaseByteArrayElements(env, request_java, request_bytes, 0); // release resources
    }
}





#endif