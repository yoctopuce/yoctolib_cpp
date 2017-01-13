/*********************************************************************
 *
 * $Id: yocto_rangefinder.cpp 26329 2017-01-11 14:04:39Z mvuilleu $
 *
 * Implements yFindRangeFinder(), the high-level API for RangeFinder functions
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
#include "yocto_rangefinder.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

YRangeFinder::YRangeFinder(const string& func): YSensor(func)
//--- (RangeFinder initialization)
    ,_rangeFinderMode(RANGEFINDERMODE_INVALID)
    ,_command(COMMAND_INVALID)
    ,_valueCallbackRangeFinder(NULL)
    ,_timedReportCallbackRangeFinder(NULL)
//--- (end of RangeFinder initialization)
{
    _className="RangeFinder";
}

YRangeFinder::~YRangeFinder()
{
//--- (YRangeFinder cleanup)
//--- (end of YRangeFinder cleanup)
}
//--- (YRangeFinder implementation)
// static attributes
const string YRangeFinder::COMMAND_INVALID = YAPI_INVALID_STRING;

int YRangeFinder::_parseAttr(yJsonStateMachine& j)
{
    if(!strcmp(j.token, "rangeFinderMode")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _rangeFinderMode =  (Y_RANGEFINDERMODE_enum)atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "command")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _command =  _parseString(j);
        return 1;
    }
    failed:
    return YSensor::_parseAttr(j);
}


/**
 * Changes the measuring unit for the measured temperature. That unit is a string.
 * String value can be " or mm. Any other value will be ignored.
 * Remember to call the saveToFlash() method of the module if the modification must be kept.
 * WARNING: if a specific calibration is defined for the rangeFinder function, a
 * unit system change will probably break it.
 *
 * @param newval : a string corresponding to the measuring unit for the measured temperature
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YRangeFinder::set_unit(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("unit", rest_val);
}

/**
 * Returns the rangefinder running mode. The rangefinder running mode
 * allows to put priority on precision, speed or maximum range.
 *
 * @return a value among Y_RANGEFINDERMODE_DEFAULT, Y_RANGEFINDERMODE_LONG_RANGE,
 * Y_RANGEFINDERMODE_HIGH_ACCURACY and Y_RANGEFINDERMODE_HIGH_SPEED corresponding to the rangefinder running mode
 *
 * On failure, throws an exception or returns Y_RANGEFINDERMODE_INVALID.
 */
Y_RANGEFINDERMODE_enum YRangeFinder::get_rangeFinderMode(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YRangeFinder::RANGEFINDERMODE_INVALID;
        }
    }
    return _rangeFinderMode;
}

/**
 * Changes the rangefinder running mode, allowing to put priority on
 * precision, speed or maximum range.
 *
 * @param newval : a value among Y_RANGEFINDERMODE_DEFAULT, Y_RANGEFINDERMODE_LONG_RANGE,
 * Y_RANGEFINDERMODE_HIGH_ACCURACY and Y_RANGEFINDERMODE_HIGH_SPEED corresponding to the rangefinder
 * running mode, allowing to put priority on
 *         precision, speed or maximum range
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YRangeFinder::set_rangeFinderMode(Y_RANGEFINDERMODE_enum newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("rangeFinderMode", rest_val);
}

string YRangeFinder::get_command(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YRangeFinder::COMMAND_INVALID;
        }
    }
    return _command;
}

int YRangeFinder::set_command(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("command", rest_val);
}

/**
 * Retrieves a range finder for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that the range finder is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method YRangeFinder.isOnline() to test if the range finder is
 * indeed online at a given time. In case of ambiguity when looking for
 * a range finder by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * @param func : a string that uniquely characterizes the range finder
 *
 * @return a YRangeFinder object allowing you to drive the range finder.
 */
YRangeFinder* YRangeFinder::FindRangeFinder(string func)
{
    YRangeFinder* obj = NULL;
    obj = (YRangeFinder*) YFunction::_FindFromCache("RangeFinder", func);
    if (obj == NULL) {
        obj = new YRangeFinder(func);
        YFunction::_AddToCache("RangeFinder", func, obj);
    }
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
int YRangeFinder::registerValueCallback(YRangeFinderValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackRangeFinder = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YRangeFinder::_invokeValueCallback(string value)
{
    if (_valueCallbackRangeFinder != NULL) {
        _valueCallbackRangeFinder(this, value);
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
int YRangeFinder::registerTimedReportCallback(YRangeFinderTimedReportCallback callback)
{
    YSensor* sensor = NULL;
    sensor = this;
    if (callback != NULL) {
        YFunction::_UpdateTimedReportCallbackList(sensor, true);
    } else {
        YFunction::_UpdateTimedReportCallbackList(sensor, false);
    }
    _timedReportCallbackRangeFinder = callback;
    return 0;
}

int YRangeFinder::_invokeTimedReportCallback(YMeasure value)
{
    if (_timedReportCallbackRangeFinder != NULL) {
        _timedReportCallbackRangeFinder(this, value);
    } else {
        YSensor::_invokeTimedReportCallback(value);
    }
    return 0;
}

/**
 * Triggers a sensor calibration according to the current ambient temperature. That
 * calibration process needs no physical interaction with the sensor. It is performed
 * automatically at device startup, but it is recommended to start it again when the
 * temperature delta since last calibration exceeds 8°C.
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *         On failure, throws an exception or returns a negative error code.
 */
int YRangeFinder::triggerTempCalibration(void)
{
    return this->set_command("T");
}

YRangeFinder *YRangeFinder::nextRangeFinder(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YRangeFinder::FindRangeFinder(hwid);
}

YRangeFinder* YRangeFinder::FirstRangeFinder(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("RangeFinder", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YRangeFinder::FindRangeFinder(serial+"."+funcId);
}

//--- (end of YRangeFinder implementation)

//--- (RangeFinder functions)
//--- (end of RangeFinder functions)
