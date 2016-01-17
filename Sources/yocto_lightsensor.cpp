/*********************************************************************
 *
 * $Id: yocto_lightsensor.cpp 22694 2016-01-12 23:13:27Z seb $
 *
 * Implements yFindLightSensor(), the high-level API for LightSensor functions
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
#include "yocto_lightsensor.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

YLightSensor::YLightSensor(const string& func): YSensor(func)
//--- (LightSensor initialization)
    ,_measureType(MEASURETYPE_INVALID)
    ,_valueCallbackLightSensor(NULL)
    ,_timedReportCallbackLightSensor(NULL)
//--- (end of LightSensor initialization)
{
    _className="LightSensor";
}

YLightSensor::~YLightSensor()
{
//--- (YLightSensor cleanup)
//--- (end of YLightSensor cleanup)
}
//--- (YLightSensor implementation)
// static attributes

int YLightSensor::_parseAttr(yJsonStateMachine& j)
{
    if(!strcmp(j.token, "measureType")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _measureType =  (Y_MEASURETYPE_enum)atoi(j.token);
        return 1;
    }
    failed:
    return YSensor::_parseAttr(j);
}


int YLightSensor::set_currentValue(double newval)
{
    string rest_val;
    char buf[32]; sprintf(buf,"%d", (int)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
    return _setAttr("currentValue", rest_val);
}

/**
 * Changes the sensor-specific calibration parameter so that the current value
 * matches a desired target (linear scaling).
 *
 * @param calibratedVal : the desired target value.
 *
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YLightSensor::calibrate(double calibratedVal)
{
    string rest_val;
    char buf[32]; sprintf(buf,"%d", (int)floor(calibratedVal * 65536.0 + 0.5)); rest_val = string(buf);
    return _setAttr("currentValue", rest_val);
}

/**
 * Returns the type of light measure.
 *
 * @return a value among Y_MEASURETYPE_HUMAN_EYE, Y_MEASURETYPE_WIDE_SPECTRUM, Y_MEASURETYPE_INFRARED,
 * Y_MEASURETYPE_HIGH_RATE and Y_MEASURETYPE_HIGH_ENERGY corresponding to the type of light measure
 *
 * On failure, throws an exception or returns Y_MEASURETYPE_INVALID.
 */
Y_MEASURETYPE_enum YLightSensor::get_measureType(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YLightSensor::MEASURETYPE_INVALID;
        }
    }
    return _measureType;
}

/**
 * Modify the light sensor type used in the device. The measure can either
 * approximate the response of the human eye, focus on a specific light
 * spectrum, depending on the capabilities of the light-sensitive cell.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : a value among Y_MEASURETYPE_HUMAN_EYE, Y_MEASURETYPE_WIDE_SPECTRUM,
 * Y_MEASURETYPE_INFRARED, Y_MEASURETYPE_HIGH_RATE and Y_MEASURETYPE_HIGH_ENERGY
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YLightSensor::set_measureType(Y_MEASURETYPE_enum newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("measureType", rest_val);
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
 * Use the method YLightSensor.isOnline() to test if $THEFUNCTION$ is
 * indeed online at a given time. In case of ambiguity when looking for
 * $AFUNCTION$ by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * @param func : a string that uniquely characterizes $THEFUNCTION$
 *
 * @return a YLightSensor object allowing you to drive $THEFUNCTION$.
 */
YLightSensor* YLightSensor::FindLightSensor(string func)
{
    YLightSensor* obj = NULL;
    obj = (YLightSensor*) YFunction::_FindFromCache("LightSensor", func);
    if (obj == NULL) {
        obj = new YLightSensor(func);
        YFunction::_AddToCache("LightSensor", func, obj);
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
int YLightSensor::registerValueCallback(YLightSensorValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackLightSensor = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YLightSensor::_invokeValueCallback(string value)
{
    if (_valueCallbackLightSensor != NULL) {
        _valueCallbackLightSensor(this, value);
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
int YLightSensor::registerTimedReportCallback(YLightSensorTimedReportCallback callback)
{
    YSensor* sensor = NULL;
    sensor = this;
    if (callback != NULL) {
        YFunction::_UpdateTimedReportCallbackList(sensor, true);
    } else {
        YFunction::_UpdateTimedReportCallbackList(sensor, false);
    }
    _timedReportCallbackLightSensor = callback;
    return 0;
}

int YLightSensor::_invokeTimedReportCallback(YMeasure value)
{
    if (_timedReportCallbackLightSensor != NULL) {
        _timedReportCallbackLightSensor(this, value);
    } else {
        YSensor::_invokeTimedReportCallback(value);
    }
    return 0;
}

YLightSensor *YLightSensor::nextLightSensor(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YLightSensor::FindLightSensor(hwid);
}

YLightSensor* YLightSensor::FirstLightSensor(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("LightSensor", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YLightSensor::FindLightSensor(serial+"."+funcId);
}

//--- (end of YLightSensor implementation)

//--- (LightSensor functions)
//--- (end of LightSensor functions)
