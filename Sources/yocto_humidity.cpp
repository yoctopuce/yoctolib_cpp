/*********************************************************************
 *
 * $Id: yocto_humidity.cpp 22694 2016-01-12 23:13:27Z seb $
 *
 * Implements yFindHumidity(), the high-level API for Humidity functions
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
#include "yocto_humidity.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

YHumidity::YHumidity(const string& func): YSensor(func)
//--- (Humidity initialization)
    ,_relHum(RELHUM_INVALID)
    ,_absHum(ABSHUM_INVALID)
    ,_valueCallbackHumidity(NULL)
    ,_timedReportCallbackHumidity(NULL)
//--- (end of Humidity initialization)
{
    _className="Humidity";
}

YHumidity::~YHumidity()
{
//--- (YHumidity cleanup)
//--- (end of YHumidity cleanup)
}
//--- (YHumidity implementation)
// static attributes
const double YHumidity::RELHUM_INVALID = YAPI_INVALID_DOUBLE;
const double YHumidity::ABSHUM_INVALID = YAPI_INVALID_DOUBLE;

int YHumidity::_parseAttr(yJsonStateMachine& j)
{
    if(!strcmp(j.token, "relHum")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _relHum =  floor(atof(j.token) * 1000.0 / 65536.0 + 0.5) / 1000.0;
        return 1;
    }
    if(!strcmp(j.token, "absHum")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _absHum =  floor(atof(j.token) * 1000.0 / 65536.0 + 0.5) / 1000.0;
        return 1;
    }
    failed:
    return YSensor::_parseAttr(j);
}


/**
 * Changes the primary unit for measuring humidity. That unit is a string.
 * If that strings starts with the letter 'g', the primary measured value is the absolute
 * humidity, in g/m3. Otherwise, the primary measured value will be the relative humidity
 * (RH), in per cents.
 *
 * Remember to call the saveToFlash() method of the module if the modification
 * must be kept.
 *
 * @param newval : a string corresponding to the primary unit for measuring humidity
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YHumidity::set_unit(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("unit", rest_val);
}

/**
 * Returns the current relative humidity, in per cents.
 *
 * @return a floating point number corresponding to the current relative humidity, in per cents
 *
 * On failure, throws an exception or returns Y_RELHUM_INVALID.
 */
double YHumidity::get_relHum(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YHumidity::RELHUM_INVALID;
        }
    }
    return _relHum;
}

/**
 * Returns the current absolute humidity, in grams per cubic meter of air.
 *
 * @return a floating point number corresponding to the current absolute humidity, in grams per cubic meter of air
 *
 * On failure, throws an exception or returns Y_ABSHUM_INVALID.
 */
double YHumidity::get_absHum(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YHumidity::ABSHUM_INVALID;
        }
    }
    return _absHum;
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
 * Use the method YHumidity.isOnline() to test if $THEFUNCTION$ is
 * indeed online at a given time. In case of ambiguity when looking for
 * $AFUNCTION$ by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * @param func : a string that uniquely characterizes $THEFUNCTION$
 *
 * @return a YHumidity object allowing you to drive $THEFUNCTION$.
 */
YHumidity* YHumidity::FindHumidity(string func)
{
    YHumidity* obj = NULL;
    obj = (YHumidity*) YFunction::_FindFromCache("Humidity", func);
    if (obj == NULL) {
        obj = new YHumidity(func);
        YFunction::_AddToCache("Humidity", func, obj);
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
int YHumidity::registerValueCallback(YHumidityValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackHumidity = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YHumidity::_invokeValueCallback(string value)
{
    if (_valueCallbackHumidity != NULL) {
        _valueCallbackHumidity(this, value);
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
int YHumidity::registerTimedReportCallback(YHumidityTimedReportCallback callback)
{
    YSensor* sensor = NULL;
    sensor = this;
    if (callback != NULL) {
        YFunction::_UpdateTimedReportCallbackList(sensor, true);
    } else {
        YFunction::_UpdateTimedReportCallbackList(sensor, false);
    }
    _timedReportCallbackHumidity = callback;
    return 0;
}

int YHumidity::_invokeTimedReportCallback(YMeasure value)
{
    if (_timedReportCallbackHumidity != NULL) {
        _timedReportCallbackHumidity(this, value);
    } else {
        YSensor::_invokeTimedReportCallback(value);
    }
    return 0;
}

YHumidity *YHumidity::nextHumidity(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YHumidity::FindHumidity(hwid);
}

YHumidity* YHumidity::FirstHumidity(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("Humidity", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YHumidity::FindHumidity(serial+"."+funcId);
}

//--- (end of YHumidity implementation)

//--- (Humidity functions)
//--- (end of Humidity functions)
