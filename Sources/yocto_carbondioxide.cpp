/*********************************************************************
 *
 * $Id: yocto_carbondioxide.cpp 22694 2016-01-12 23:13:27Z seb $
 *
 * Implements yFindCarbonDioxide(), the high-level API for CarbonDioxide functions
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
#include "yocto_carbondioxide.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

YCarbonDioxide::YCarbonDioxide(const string& func): YSensor(func)
//--- (CarbonDioxide initialization)
    ,_abcPeriod(ABCPERIOD_INVALID)
    ,_command(COMMAND_INVALID)
    ,_valueCallbackCarbonDioxide(NULL)
    ,_timedReportCallbackCarbonDioxide(NULL)
//--- (end of CarbonDioxide initialization)
{
    _className="CarbonDioxide";
}

YCarbonDioxide::~YCarbonDioxide()
{
//--- (YCarbonDioxide cleanup)
//--- (end of YCarbonDioxide cleanup)
}
//--- (YCarbonDioxide implementation)
// static attributes
const string YCarbonDioxide::COMMAND_INVALID = YAPI_INVALID_STRING;

int YCarbonDioxide::_parseAttr(yJsonStateMachine& j)
{
    if(!strcmp(j.token, "abcPeriod")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _abcPeriod =  atoi(j.token);
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
 * Returns the Automatic Baseline Calibration period, in hours. A negative value
 * means that automatic baseline calibration is disabled.
 *
 * @return an integer corresponding to the Automatic Baseline Calibration period, in hours
 *
 * On failure, throws an exception or returns Y_ABCPERIOD_INVALID.
 */
int YCarbonDioxide::get_abcPeriod(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YCarbonDioxide::ABCPERIOD_INVALID;
        }
    }
    return _abcPeriod;
}

/**
 * Modifies Automatic Baseline Calibration period, in hours. If you need
 * to disable automatic baseline calibration (for instance when using the
 * sensor in an environment that is constantly above 400ppm CO2), set the
 * period to -1. Remember to call the saveToFlash() method of the
 * module if the modification must be kept.
 *
 * @param newval : an integer
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YCarbonDioxide::set_abcPeriod(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("abcPeriod", rest_val);
}

string YCarbonDioxide::get_command(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YCarbonDioxide::COMMAND_INVALID;
        }
    }
    return _command;
}

int YCarbonDioxide::set_command(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("command", rest_val);
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
 * Use the method YCarbonDioxide.isOnline() to test if $THEFUNCTION$ is
 * indeed online at a given time. In case of ambiguity when looking for
 * $AFUNCTION$ by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * @param func : a string that uniquely characterizes $THEFUNCTION$
 *
 * @return a YCarbonDioxide object allowing you to drive $THEFUNCTION$.
 */
YCarbonDioxide* YCarbonDioxide::FindCarbonDioxide(string func)
{
    YCarbonDioxide* obj = NULL;
    obj = (YCarbonDioxide*) YFunction::_FindFromCache("CarbonDioxide", func);
    if (obj == NULL) {
        obj = new YCarbonDioxide(func);
        YFunction::_AddToCache("CarbonDioxide", func, obj);
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
int YCarbonDioxide::registerValueCallback(YCarbonDioxideValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackCarbonDioxide = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YCarbonDioxide::_invokeValueCallback(string value)
{
    if (_valueCallbackCarbonDioxide != NULL) {
        _valueCallbackCarbonDioxide(this, value);
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
int YCarbonDioxide::registerTimedReportCallback(YCarbonDioxideTimedReportCallback callback)
{
    YSensor* sensor = NULL;
    sensor = this;
    if (callback != NULL) {
        YFunction::_UpdateTimedReportCallbackList(sensor, true);
    } else {
        YFunction::_UpdateTimedReportCallbackList(sensor, false);
    }
    _timedReportCallbackCarbonDioxide = callback;
    return 0;
}

int YCarbonDioxide::_invokeTimedReportCallback(YMeasure value)
{
    if (_timedReportCallbackCarbonDioxide != NULL) {
        _timedReportCallbackCarbonDioxide(this, value);
    } else {
        YSensor::_invokeTimedReportCallback(value);
    }
    return 0;
}

/**
 * Triggers a baseline calibration at standard CO2 ambiant level (400ppm).
 * It is normally not necessary to manually calibrate the sensor, because
 * the built-in automatic baseline calibration procedure will automatically
 * fix any long-term drift based on the lowest level of CO2 observed over the
 * automatic calibration period. However, if you disable automatic baseline
 * calibration, you may want to manually trigger a calibration from time to
 * time. Before starting a baseline calibration, make sure to put the sensor
 * in a standard environment (e.g. outside in fresh air) at around 400ppm.
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YCarbonDioxide::triggetBaselineCalibration(void)
{
    return this->set_command("BC");
}

/**
 * Triggers a zero calibration of the sensor on carbon dioxide-free air.
 * It is normally not necessary to manually calibrate the sensor, because
 * the built-in automatic baseline calibration procedure will automatically
 * fix any long-term drift based on the lowest level of CO2 observed over the
 * automatic calibration period. However, if you disable automatic baseline
 * calibration, you may want to manually trigger a calibration from time to
 * time. Before starting a zero calibration, you should circulate carbon
 * dioxide-free air within the sensor for a minute or two, using a small pipe
 * connected to the sensor. Please contact support@yoctopuce.com for more details
 * on the zero calibration procedure.
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YCarbonDioxide::triggetZeroCalibration(void)
{
    return this->set_command("ZC");
}

YCarbonDioxide *YCarbonDioxide::nextCarbonDioxide(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YCarbonDioxide::FindCarbonDioxide(hwid);
}

YCarbonDioxide* YCarbonDioxide::FirstCarbonDioxide(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("CarbonDioxide", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YCarbonDioxide::FindCarbonDioxide(serial+"."+funcId);
}

//--- (end of YCarbonDioxide implementation)

//--- (CarbonDioxide functions)
//--- (end of CarbonDioxide functions)
