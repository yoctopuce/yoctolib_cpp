/*********************************************************************
 *
 *  $Id: svn_id $
 *
 *  Implements yFindAirQuality(), the high-level API for AirQuality functions
 *
 *  - - - - - - - - - License information: - - - - - - - - -
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
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "yocto_airquality.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#define  __FILE_ID__  "airquality"

#ifdef YOCTOLIB_NAMESPACE
using namespace YOCTOLIB_NAMESPACE;
#endif

YAirQuality::YAirQuality(const string& func): YSensor(func)
//--- (YAirQuality initialization)
    ,_ubaIndex(UBAINDEX_INVALID)
    ,_relativeIndex(RELATIVEINDEX_INVALID)
    ,_aqiMode(AQIMODE_INVALID)
    ,_valueCallbackAirQuality(NULL)
    ,_timedReportCallbackAirQuality(NULL)
//--- (end of YAirQuality initialization)
{
    _className="AirQuality";
}

YAirQuality::~YAirQuality()
{
//--- (YAirQuality cleanup)
//--- (end of YAirQuality cleanup)
}
//--- (YAirQuality implementation)
// static attributes
const double YAirQuality::UBAINDEX_INVALID = YAPI_INVALID_DOUBLE;
const double YAirQuality::RELATIVEINDEX_INVALID = YAPI_INVALID_DOUBLE;

int YAirQuality::_parseAttr(YJSONObject *json_val)
{
    if(json_val->has("ubaIndex")) {
        _ubaIndex =  floor(json_val->getDouble("ubaIndex") / 65.536 + 0.5) / 1000.0;
    }
    if(json_val->has("relativeIndex")) {
        _relativeIndex =  floor(json_val->getDouble("relativeIndex") / 65.536 + 0.5) / 1000.0;
    }
    if(json_val->has("aqiMode")) {
        _aqiMode =  (Y_AQIMODE_enum)json_val->getInt("aqiMode");
    }
    return YSensor::_parseAttr(json_val);
}


/**
 * Returns the current air quality index, according to UBA (from 1 to 5).
 *
 * @return a floating point number corresponding to the current air quality index, according to UBA (from 1 to 5)
 *
 * On failure, throws an exception or returns YAirQuality::UBAINDEX_INVALID.
 */
double YAirQuality::get_ubaIndex(void)
{
    double res = 0.0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YAirQuality::UBAINDEX_INVALID;
                }
            }
        }
        res = _ubaIndex;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the relative air quality index, according to ScioSense (from 0 to 500).
 * A value below 100 indicates better-than-average air quality compared to the past 24 hours,
 * while a value above 100 indicates poorer-than-average air quality compared to the past 24 hours.
 *
 * @return a floating point number corresponding to the relative air quality index, according to
 * ScioSense (from 0 to 500)
 *
 * On failure, throws an exception or returns YAirQuality::RELATIVEINDEX_INVALID.
 */
double YAirQuality::get_relativeIndex(void)
{
    double res = 0.0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YAirQuality::RELATIVEINDEX_INVALID;
                }
            }
        }
        res = _relativeIndex;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the type of index reported by the get_currentValue function and callbacks (UBA index or relative index).
 *
 * @return either YAirQuality::AQIMODE_RELATIVE or YAirQuality::AQIMODE_UBA, according to the type of
 * index reported by the get_currentValue function and callbacks (UBA index or relative index)
 *
 * On failure, throws an exception or returns YAirQuality::AQIMODE_INVALID.
 */
Y_AQIMODE_enum YAirQuality::get_aqiMode(void)
{
    Y_AQIMODE_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YAirQuality::AQIMODE_INVALID;
                }
            }
        }
        res = _aqiMode;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the the type of index reported by the get_currentValue function and callbacks (UBA index or
 * relative index).
 * Remember to call the saveToFlash() method of the module if the modification must be kept.
 *
 * @param newval : either YAirQuality::AQIMODE_RELATIVE or YAirQuality::AQIMODE_UBA, according to the
 * the type of index reported by the get_currentValue function and callbacks (UBA index or relative index)
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YAirQuality::set_aqiMode(Y_AQIMODE_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("aqiMode", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Retrieves a air quality sensor for a given identifier.
 * The identifier can be specified using several formats:
 *
 * - FunctionLogicalName
 * - ModuleSerialNumber.FunctionIdentifier
 * - ModuleSerialNumber.FunctionLogicalName
 * - ModuleLogicalName.FunctionIdentifier
 * - ModuleLogicalName.FunctionLogicalName
 *
 *
 * This function does not require that the air quality sensor is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the air quality sensor is
 * indeed online at a given time. In case of ambiguity when looking for
 * a air quality sensor by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the air quality sensor, for instance
 *         MyDevice.airQuality.
 *
 * @return a YAirQuality object allowing you to drive the air quality sensor.
 */
YAirQuality* YAirQuality::FindAirQuality(string func)
{
    YAirQuality* obj = NULL;
    int taken = 0;
    if (YAPI::_apiInitialized) {
        yEnterCriticalSection(&YAPI::_global_cs);
        taken = 1;
    }try {
        obj = (YAirQuality*) YFunction::_FindFromCache("AirQuality", func);
        if (obj == NULL) {
            obj = new YAirQuality(func);
            YFunction::_AddToCache("AirQuality", func, obj);
        }
    } catch (std::exception &) {
        if (taken) yLeaveCriticalSection(&YAPI::_global_cs);
        throw;
    }
    if (taken) yLeaveCriticalSection(&YAPI::_global_cs);
    return obj;
}

/**
 * Registers the callback function that is invoked on every change of advertised value.
 * The callback is then invoked only during the execution of ySleep or yHandleEvents.
 * This provides control over the time when the callback is triggered. For good responsiveness,
 * remember to call one of these two functions periodically. The callback is called once juste after beeing
 * registered, passing the current advertised value  of the function, provided that it is not an empty string.
 * To unregister a callback, pass a NULL pointer as argument.
 *
 * @param callback : the callback function to call, or a NULL pointer. The callback function should take two
 *         arguments: the function object of which the value has changed, and the character string describing
 *         the new advertised value.
 * @noreturn
 */
int YAirQuality::registerValueCallback(YAirQualityValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackAirQuality = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YAirQuality::_invokeValueCallback(string value)
{
    if (_valueCallbackAirQuality != NULL) {
        _valueCallbackAirQuality(this, value);
    } else {
        YSensor::_invokeValueCallback(value);
    }
    return 0;
}

/**
 * Registers the callback function that is invoked on every periodic timed notification.
 * The callback is invoked only during the execution of ySleep or yHandleEvents.
 * This provides control over the time when the callback is triggered. For good responsiveness, remember to call
 * one of these two functions periodically. To unregister a callback, pass a NULL pointer as argument.
 *
 * @param callback : the callback function to call, or a NULL pointer. The callback function should take two
 *         arguments: the function object of which the value has changed, and an YMeasure object describing
 *         the new advertised value.
 * @noreturn
 */
int YAirQuality::registerTimedReportCallback(YAirQualityTimedReportCallback callback)
{
    YSensor* sensor = NULL;
    sensor = this;
    if (callback != NULL) {
        YFunction::_UpdateTimedReportCallbackList(sensor, true);
    } else {
        YFunction::_UpdateTimedReportCallbackList(sensor, false);
    }
    _timedReportCallbackAirQuality = callback;
    return 0;
}

int YAirQuality::_invokeTimedReportCallback(YMeasure value)
{
    if (_timedReportCallbackAirQuality != NULL) {
        _timedReportCallbackAirQuality(this, value);
    } else {
        YSensor::_invokeTimedReportCallback(value);
    }
    return 0;
}

YAirQuality *YAirQuality::nextAirQuality(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YAirQuality::FindAirQuality(hwid);
}

YAirQuality *YAirQuality::FirstAirQuality(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("AirQuality", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YAirQuality::FindAirQuality(serial+"."+funcId);
}

//--- (end of YAirQuality implementation)

//--- (YAirQuality functions)
//--- (end of YAirQuality functions)
