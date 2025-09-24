/*********************************************************************
 *
 *  $Id: svn_id $
 *
 *  Implements yFindVirtualSensor(), the high-level API for VirtualSensor functions
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

#include "yocto_virtualsensor.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#define  __FILE_ID__  "virtualsensor"

#ifdef YOCTOLIB_NAMESPACE
using namespace YOCTOLIB_NAMESPACE;
#endif

YVirtualSensor::YVirtualSensor(const string& func): YSensor(func)
//--- (YVirtualSensor initialization)
    ,_invalidValue(INVALIDVALUE_INVALID)
    ,_valueCallbackVirtualSensor(NULL)
    ,_timedReportCallbackVirtualSensor(NULL)
//--- (end of YVirtualSensor initialization)
{
    _className="VirtualSensor";
}

YVirtualSensor::~YVirtualSensor()
{
//--- (YVirtualSensor cleanup)
//--- (end of YVirtualSensor cleanup)
}
//--- (YVirtualSensor implementation)
// static attributes
const double YVirtualSensor::INVALIDVALUE_INVALID = YAPI_INVALID_DOUBLE;

int YVirtualSensor::_parseAttr(YJSONObject *json_val)
{
    if(json_val->has("invalidValue")) {
        _invalidValue =  floor(json_val->getDouble("invalidValue") / 65.536 + 0.5) / 1000.0;
    }
    return YSensor::_parseAttr(json_val);
}


/**
 * Changes the measuring unit for the measured value.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : a string corresponding to the measuring unit for the measured value
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YVirtualSensor::set_unit(const string& newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = newval;
        res = _setAttr("unit", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the current value of the sensor (raw value, before calibration).
 *
 * @param newval : a floating point number corresponding to the current value of the sensor (raw
 * value, before calibration)
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YVirtualSensor::set_currentRawValue(double newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%" FMTs64, (s64)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
        res = _setAttr("currentRawValue", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

int YVirtualSensor::set_sensorState(int newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("sensorState", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the invalid value of the sensor, returned if the sensor is read when in invalid state
 * (for instance before having been set). Remember to call the saveToFlash()
 * method of the module if the modification must be kept.
 *
 * @param newval : a floating point number corresponding to the invalid value of the sensor, returned
 * if the sensor is read when in invalid state
 *         (for instance before having been set)
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YVirtualSensor::set_invalidValue(double newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%" FMTs64, (s64)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
        res = _setAttr("invalidValue", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the invalid value of the sensor, returned if the sensor is read when in invalid state
 * (for instance before having been set).
 *
 * @return a floating point number corresponding to the invalid value of the sensor, returned if the
 * sensor is read when in invalid state
 *         (for instance before having been set)
 *
 * On failure, throws an exception or returns YVirtualSensor::INVALIDVALUE_INVALID.
 */
double YVirtualSensor::get_invalidValue(void)
{
    double res = 0.0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YVirtualSensor::INVALIDVALUE_INVALID;
                }
            }
        }
        res = _invalidValue;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Retrieves a virtual sensor for a given identifier.
 * The identifier can be specified using several formats:
 *
 * - FunctionLogicalName
 * - ModuleSerialNumber.FunctionIdentifier
 * - ModuleSerialNumber.FunctionLogicalName
 * - ModuleLogicalName.FunctionIdentifier
 * - ModuleLogicalName.FunctionLogicalName
 *
 *
 * This function does not require that the virtual sensor is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the virtual sensor is
 * indeed online at a given time. In case of ambiguity when looking for
 * a virtual sensor by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the virtual sensor, for instance
 *         MyDevice.virtualSensor1.
 *
 * @return a YVirtualSensor object allowing you to drive the virtual sensor.
 */
YVirtualSensor* YVirtualSensor::FindVirtualSensor(string func)
{
    YVirtualSensor* obj = NULL;
    int taken = 0;
    if (YAPI::_apiInitialized) {
        yEnterCriticalSection(&YAPI::_global_cs);
        taken = 1;
    }try {
        obj = (YVirtualSensor*) YFunction::_FindFromCache("VirtualSensor", func);
        if (obj == NULL) {
            obj = new YVirtualSensor(func);
            YFunction::_AddToCache("VirtualSensor", func, obj);
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
int YVirtualSensor::registerValueCallback(YVirtualSensorValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackVirtualSensor = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YVirtualSensor::_invokeValueCallback(string value)
{
    if (_valueCallbackVirtualSensor != NULL) {
        _valueCallbackVirtualSensor(this, value);
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
int YVirtualSensor::registerTimedReportCallback(YVirtualSensorTimedReportCallback callback)
{
    YSensor* sensor = NULL;
    sensor = this;
    if (callback != NULL) {
        YFunction::_UpdateTimedReportCallbackList(sensor, true);
    } else {
        YFunction::_UpdateTimedReportCallbackList(sensor, false);
    }
    _timedReportCallbackVirtualSensor = callback;
    return 0;
}

int YVirtualSensor::_invokeTimedReportCallback(YMeasure value)
{
    if (_timedReportCallbackVirtualSensor != NULL) {
        _timedReportCallbackVirtualSensor(this, value);
    } else {
        YSensor::_invokeTimedReportCallback(value);
    }
    return 0;
}

/**
 * Changes the current sensor state to invalid (as if no value would have been ever set).
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YVirtualSensor::set_sensorAsInvalid(void)
{
    return this->set_sensorState(1);
}

YVirtualSensor *YVirtualSensor::nextVirtualSensor(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YVirtualSensor::FindVirtualSensor(hwid);
}

YVirtualSensor *YVirtualSensor::FirstVirtualSensor(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("VirtualSensor", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YVirtualSensor::FindVirtualSensor(serial+"."+funcId);
}

//--- (end of YVirtualSensor implementation)

//--- (YVirtualSensor functions)
//--- (end of YVirtualSensor functions)
