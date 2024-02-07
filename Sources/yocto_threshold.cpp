/*********************************************************************
 *
 *  $Id: svn_id $
 *
 *  Implements yFindThreshold(), the high-level API for Threshold functions
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

#include "yocto_threshold.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#define  __FILE_ID__  "threshold"

#ifdef YOCTOLIB_NAMESPACE
using namespace YOCTOLIB_NAMESPACE;
#endif

YThreshold::YThreshold(const string& func): YFunction(func)
//--- (YThreshold initialization)
    ,_thresholdState(THRESHOLDSTATE_INVALID)
    ,_targetSensor(TARGETSENSOR_INVALID)
    ,_alertLevel(ALERTLEVEL_INVALID)
    ,_safeLevel(SAFELEVEL_INVALID)
    ,_valueCallbackThreshold(NULL)
//--- (end of YThreshold initialization)
{
    _className="Threshold";
}

YThreshold::~YThreshold()
{
//--- (YThreshold cleanup)
//--- (end of YThreshold cleanup)
}
//--- (YThreshold implementation)
// static attributes
const string YThreshold::TARGETSENSOR_INVALID = YAPI_INVALID_STRING;
const double YThreshold::ALERTLEVEL_INVALID = YAPI_INVALID_DOUBLE;
const double YThreshold::SAFELEVEL_INVALID = YAPI_INVALID_DOUBLE;

int YThreshold::_parseAttr(YJSONObject *json_val)
{
    if(json_val->has("thresholdState")) {
        _thresholdState =  (Y_THRESHOLDSTATE_enum)json_val->getInt("thresholdState");
    }
    if(json_val->has("targetSensor")) {
        _targetSensor =  json_val->getString("targetSensor");
    }
    if(json_val->has("alertLevel")) {
        _alertLevel =  floor(json_val->getDouble("alertLevel") / 65.536 + 0.5) / 1000.0;
    }
    if(json_val->has("safeLevel")) {
        _safeLevel =  floor(json_val->getDouble("safeLevel") / 65.536 + 0.5) / 1000.0;
    }
    return YFunction::_parseAttr(json_val);
}


/**
 * Returns current state of the threshold function.
 *
 * @return either YThreshold::THRESHOLDSTATE_SAFE or YThreshold::THRESHOLDSTATE_ALERT, according to
 * current state of the threshold function
 *
 * On failure, throws an exception or returns YThreshold::THRESHOLDSTATE_INVALID.
 */
Y_THRESHOLDSTATE_enum YThreshold::get_thresholdState(void)
{
    Y_THRESHOLDSTATE_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YThreshold::THRESHOLDSTATE_INVALID;
                }
            }
        }
        res = _thresholdState;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the name of the sensor monitored by the threshold function.
 *
 * @return a string corresponding to the name of the sensor monitored by the threshold function
 *
 * On failure, throws an exception or returns YThreshold::TARGETSENSOR_INVALID.
 */
string YThreshold::get_targetSensor(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YThreshold::TARGETSENSOR_INVALID;
                }
            }
        }
        res = _targetSensor;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the sensor alert level triggering the threshold function.
 * Remember to call the matching module saveToFlash()
 * method if you want to preserve the setting after reboot.
 *
 * @param newval : a floating point number corresponding to the sensor alert level triggering the
 * threshold function
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YThreshold::set_alertLevel(double newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%" FMTs64, (s64)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
        res = _setAttr("alertLevel", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the sensor alert level, triggering the threshold function.
 *
 * @return a floating point number corresponding to the sensor alert level, triggering the threshold function
 *
 * On failure, throws an exception or returns YThreshold::ALERTLEVEL_INVALID.
 */
double YThreshold::get_alertLevel(void)
{
    double res = 0.0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YThreshold::ALERTLEVEL_INVALID;
                }
            }
        }
        res = _alertLevel;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the sensor acceptable level for disabling the threshold function.
 * Remember to call the matching module saveToFlash()
 * method if you want to preserve the setting after reboot.
 *
 * @param newval : a floating point number corresponding to the sensor acceptable level for disabling
 * the threshold function
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YThreshold::set_safeLevel(double newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%" FMTs64, (s64)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
        res = _setAttr("safeLevel", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the sensor acceptable level for disabling the threshold function.
 *
 * @return a floating point number corresponding to the sensor acceptable level for disabling the
 * threshold function
 *
 * On failure, throws an exception or returns YThreshold::SAFELEVEL_INVALID.
 */
double YThreshold::get_safeLevel(void)
{
    double res = 0.0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YThreshold::SAFELEVEL_INVALID;
                }
            }
        }
        res = _safeLevel;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Retrieves a threshold function for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that the threshold function is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the threshold function is
 * indeed online at a given time. In case of ambiguity when looking for
 * a threshold function by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the threshold function, for instance
 *         MyDevice.threshold1.
 *
 * @return a YThreshold object allowing you to drive the threshold function.
 */
YThreshold* YThreshold::FindThreshold(string func)
{
    YThreshold* obj = NULL;
    int taken = 0;
    if (YAPI::_apiInitialized) {
        yEnterCriticalSection(&YAPI::_global_cs);
        taken = 1;
    }try {
        obj = (YThreshold*) YFunction::_FindFromCache("Threshold", func);
        if (obj == NULL) {
            obj = new YThreshold(func);
            YFunction::_AddToCache("Threshold", func, obj);
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
 * The callback is invoked only during the execution of ySleep or yHandleEvents.
 * This provides control over the time when the callback is triggered. For good responsiveness, remember to call
 * one of these two functions periodically. To unregister a callback, pass a NULL pointer as argument.
 *
 * @param callback : the callback function to call, or a NULL pointer. The callback function should take two
 *         arguments: the function object of which the value has changed, and the character string describing
 *         the new advertised value.
 * @noreturn
 */
int YThreshold::registerValueCallback(YThresholdValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackThreshold = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YThreshold::_invokeValueCallback(string value)
{
    if (_valueCallbackThreshold != NULL) {
        _valueCallbackThreshold(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

YThreshold *YThreshold::nextThreshold(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YThreshold::FindThreshold(hwid);
}

YThreshold *YThreshold::FirstThreshold(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("Threshold", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YThreshold::FindThreshold(serial+"."+funcId);
}

//--- (end of YThreshold implementation)

//--- (YThreshold functions)
//--- (end of YThreshold functions)
