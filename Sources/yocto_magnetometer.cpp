/*********************************************************************
 *
 * $Id: yocto_magnetometer.cpp 22694 2016-01-12 23:13:27Z seb $
 *
 * Implements yFindMagnetometer(), the high-level API for Magnetometer functions
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
#include "yocto_magnetometer.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

YMagnetometer::YMagnetometer(const string& func): YSensor(func)
//--- (Magnetometer initialization)
    ,_xValue(XVALUE_INVALID)
    ,_yValue(YVALUE_INVALID)
    ,_zValue(ZVALUE_INVALID)
    ,_valueCallbackMagnetometer(NULL)
    ,_timedReportCallbackMagnetometer(NULL)
//--- (end of Magnetometer initialization)
{
    _className="Magnetometer";
}

YMagnetometer::~YMagnetometer()
{
//--- (YMagnetometer cleanup)
//--- (end of YMagnetometer cleanup)
}
//--- (YMagnetometer implementation)
// static attributes
const double YMagnetometer::XVALUE_INVALID = YAPI_INVALID_DOUBLE;
const double YMagnetometer::YVALUE_INVALID = YAPI_INVALID_DOUBLE;
const double YMagnetometer::ZVALUE_INVALID = YAPI_INVALID_DOUBLE;

int YMagnetometer::_parseAttr(yJsonStateMachine& j)
{
    if(!strcmp(j.token, "xValue")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _xValue =  floor(atof(j.token) * 1000.0 / 65536.0 + 0.5) / 1000.0;
        return 1;
    }
    if(!strcmp(j.token, "yValue")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _yValue =  floor(atof(j.token) * 1000.0 / 65536.0 + 0.5) / 1000.0;
        return 1;
    }
    if(!strcmp(j.token, "zValue")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _zValue =  floor(atof(j.token) * 1000.0 / 65536.0 + 0.5) / 1000.0;
        return 1;
    }
    failed:
    return YSensor::_parseAttr(j);
}


/**
 * Returns the X component of the magnetic field, as a floating point number.
 *
 * @return a floating point number corresponding to the X component of the magnetic field, as a
 * floating point number
 *
 * On failure, throws an exception or returns Y_XVALUE_INVALID.
 */
double YMagnetometer::get_xValue(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YMagnetometer::XVALUE_INVALID;
        }
    }
    return _xValue;
}

/**
 * Returns the Y component of the magnetic field, as a floating point number.
 *
 * @return a floating point number corresponding to the Y component of the magnetic field, as a
 * floating point number
 *
 * On failure, throws an exception or returns Y_YVALUE_INVALID.
 */
double YMagnetometer::get_yValue(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YMagnetometer::YVALUE_INVALID;
        }
    }
    return _yValue;
}

/**
 * Returns the Z component of the magnetic field, as a floating point number.
 *
 * @return a floating point number corresponding to the Z component of the magnetic field, as a
 * floating point number
 *
 * On failure, throws an exception or returns Y_ZVALUE_INVALID.
 */
double YMagnetometer::get_zValue(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YMagnetometer::ZVALUE_INVALID;
        }
    }
    return _zValue;
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
 * Use the method YMagnetometer.isOnline() to test if $THEFUNCTION$ is
 * indeed online at a given time. In case of ambiguity when looking for
 * $AFUNCTION$ by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * @param func : a string that uniquely characterizes $THEFUNCTION$
 *
 * @return a YMagnetometer object allowing you to drive $THEFUNCTION$.
 */
YMagnetometer* YMagnetometer::FindMagnetometer(string func)
{
    YMagnetometer* obj = NULL;
    obj = (YMagnetometer*) YFunction::_FindFromCache("Magnetometer", func);
    if (obj == NULL) {
        obj = new YMagnetometer(func);
        YFunction::_AddToCache("Magnetometer", func, obj);
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
int YMagnetometer::registerValueCallback(YMagnetometerValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackMagnetometer = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YMagnetometer::_invokeValueCallback(string value)
{
    if (_valueCallbackMagnetometer != NULL) {
        _valueCallbackMagnetometer(this, value);
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
int YMagnetometer::registerTimedReportCallback(YMagnetometerTimedReportCallback callback)
{
    YSensor* sensor = NULL;
    sensor = this;
    if (callback != NULL) {
        YFunction::_UpdateTimedReportCallbackList(sensor, true);
    } else {
        YFunction::_UpdateTimedReportCallbackList(sensor, false);
    }
    _timedReportCallbackMagnetometer = callback;
    return 0;
}

int YMagnetometer::_invokeTimedReportCallback(YMeasure value)
{
    if (_timedReportCallbackMagnetometer != NULL) {
        _timedReportCallbackMagnetometer(this, value);
    } else {
        YSensor::_invokeTimedReportCallback(value);
    }
    return 0;
}

YMagnetometer *YMagnetometer::nextMagnetometer(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YMagnetometer::FindMagnetometer(hwid);
}

YMagnetometer* YMagnetometer::FirstMagnetometer(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("Magnetometer", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YMagnetometer::FindMagnetometer(serial+"."+funcId);
}

//--- (end of YMagnetometer implementation)

//--- (Magnetometer functions)
//--- (end of Magnetometer functions)
