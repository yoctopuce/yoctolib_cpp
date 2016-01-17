/*********************************************************************
 *
 * $Id: yocto_quadraturedecoder.cpp 22694 2016-01-12 23:13:27Z seb $
 *
 * Implements yFindQuadratureDecoder(), the high-level API for QuadratureDecoder functions
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
 *  THE SOFTWARE AND DOCUMENTATION ARE PROVIDED 'AS IS' WITHOUT
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


#define _CRT_SECURE_NO_DEPRECATE //do not use windows secure crt
#include "yocto_quadraturedecoder.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

YQuadratureDecoder::YQuadratureDecoder(const string& func): YSensor(func)
//--- (QuadratureDecoder initialization)
    ,_speed(SPEED_INVALID)
    ,_decoding(DECODING_INVALID)
    ,_valueCallbackQuadratureDecoder(NULL)
    ,_timedReportCallbackQuadratureDecoder(NULL)
//--- (end of QuadratureDecoder initialization)
{
    _className="QuadratureDecoder";
}

YQuadratureDecoder::~YQuadratureDecoder()
{
//--- (YQuadratureDecoder cleanup)
//--- (end of YQuadratureDecoder cleanup)
}
//--- (YQuadratureDecoder implementation)
// static attributes
const double YQuadratureDecoder::SPEED_INVALID = YAPI_INVALID_DOUBLE;

int YQuadratureDecoder::_parseAttr(yJsonStateMachine& j)
{
    if(!strcmp(j.token, "speed")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _speed =  floor(atof(j.token) * 1000.0 / 65536.0 + 0.5) / 1000.0;
        return 1;
    }
    if(!strcmp(j.token, "decoding")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _decoding =  (Y_DECODING_enum)atoi(j.token);
        return 1;
    }
    failed:
    return YSensor::_parseAttr(j);
}


/**
 * Changes the current expected position of the quadrature decoder.
 * Invoking this function implicitely activates the quadrature decoder.
 *
 * @param newval : a floating point number corresponding to the current expected position of the quadrature decoder
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YQuadratureDecoder::set_currentValue(double newval)
{
    string rest_val;
    char buf[32]; sprintf(buf,"%d", (int)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
    return _setAttr("currentValue", rest_val);
}

/**
 * Returns the PWM frequency in Hz.
 *
 * @return a floating point number corresponding to the PWM frequency in Hz
 *
 * On failure, throws an exception or returns Y_SPEED_INVALID.
 */
double YQuadratureDecoder::get_speed(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YQuadratureDecoder::SPEED_INVALID;
        }
    }
    return _speed;
}

/**
 * Returns the current activation state of the quadrature decoder.
 *
 * @return either Y_DECODING_OFF or Y_DECODING_ON, according to the current activation state of the
 * quadrature decoder
 *
 * On failure, throws an exception or returns Y_DECODING_INVALID.
 */
Y_DECODING_enum YQuadratureDecoder::get_decoding(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YQuadratureDecoder::DECODING_INVALID;
        }
    }
    return _decoding;
}

/**
 * Changes the activation state of the quadrature decoder.
 *
 * @param newval : either Y_DECODING_OFF or Y_DECODING_ON, according to the activation state of the
 * quadrature decoder
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YQuadratureDecoder::set_decoding(Y_DECODING_enum newval)
{
    string rest_val;
    rest_val = (newval>0 ? "1" : "0");
    return _setAttr("decoding", rest_val);
}

/**
 * Retrieves $AFUNCTION$ for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that $THEFUNCTION$ is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method YQuadratureDecoder.isOnline() to test if $THEFUNCTION$ is
 * indeed online at a given time. In case of ambiguity when looking for
 * $AFUNCTION$ by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * @param func : a string that uniquely characterizes $THEFUNCTION$
 *
 * @return a YQuadratureDecoder object allowing you to drive $THEFUNCTION$.
 */
YQuadratureDecoder* YQuadratureDecoder::FindQuadratureDecoder(string func)
{
    YQuadratureDecoder* obj = NULL;
    obj = (YQuadratureDecoder*) YFunction::_FindFromCache("QuadratureDecoder", func);
    if (obj == NULL) {
        obj = new YQuadratureDecoder(func);
        YFunction::_AddToCache("QuadratureDecoder", func, obj);
    }
    return obj;
}

/**
 * Registers the callback function that is invoked on every change of advertised value.
 * The callback is invoked only during the execution of ySleep or yHandleEvents.
 * This provides control over the time when the callback is triggered. For good responsiveness, remember to call
 * one of these two functions periodically. To unregister a callback, pass a null pointer as argument.
 *
 * @param callback : the callback function to call, or a null pointer. The callback function should take two
 *         arguments: the function object of which the value has changed, and the character string describing
 *         the new advertised value.
 * @noreturn
 */
int YQuadratureDecoder::registerValueCallback(YQuadratureDecoderValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackQuadratureDecoder = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YQuadratureDecoder::_invokeValueCallback(string value)
{
    if (_valueCallbackQuadratureDecoder != NULL) {
        _valueCallbackQuadratureDecoder(this, value);
    } else {
        YSensor::_invokeValueCallback(value);
    }
    return 0;
}

/**
 * Registers the callback function that is invoked on every periodic timed notification.
 * The callback is invoked only during the execution of ySleep or yHandleEvents.
 * This provides control over the time when the callback is triggered. For good responsiveness, remember to call
 * one of these two functions periodically. To unregister a callback, pass a null pointer as argument.
 *
 * @param callback : the callback function to call, or a null pointer. The callback function should take two
 *         arguments: the function object of which the value has changed, and an YMeasure object describing
 *         the new advertised value.
 * @noreturn
 */
int YQuadratureDecoder::registerTimedReportCallback(YQuadratureDecoderTimedReportCallback callback)
{
    YSensor* sensor = NULL;
    sensor = this;
    if (callback != NULL) {
        YFunction::_UpdateTimedReportCallbackList(sensor, true);
    } else {
        YFunction::_UpdateTimedReportCallbackList(sensor, false);
    }
    _timedReportCallbackQuadratureDecoder = callback;
    return 0;
}

int YQuadratureDecoder::_invokeTimedReportCallback(YMeasure value)
{
    if (_timedReportCallbackQuadratureDecoder != NULL) {
        _timedReportCallbackQuadratureDecoder(this, value);
    } else {
        YSensor::_invokeTimedReportCallback(value);
    }
    return 0;
}

YQuadratureDecoder *YQuadratureDecoder::nextQuadratureDecoder(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YQuadratureDecoder::FindQuadratureDecoder(hwid);
}

YQuadratureDecoder* YQuadratureDecoder::FirstQuadratureDecoder(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("QuadratureDecoder", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YQuadratureDecoder::FindQuadratureDecoder(serial+"."+funcId);
}

//--- (end of YQuadratureDecoder implementation)

//--- (QuadratureDecoder functions)
//--- (end of QuadratureDecoder functions)
