/*********************************************************************
 *
 * $Id: yocto_altitude.cpp 22694 2016-01-12 23:13:27Z seb $
 *
 * Implements yFindAltitude(), the high-level API for Altitude functions
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
#include "yocto_altitude.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

YAltitude::YAltitude(const string& func): YSensor(func)
//--- (Altitude initialization)
    ,_qnh(QNH_INVALID)
    ,_technology(TECHNOLOGY_INVALID)
    ,_valueCallbackAltitude(NULL)
    ,_timedReportCallbackAltitude(NULL)
//--- (end of Altitude initialization)
{
    _className="Altitude";
}

YAltitude::~YAltitude()
{
//--- (YAltitude cleanup)
//--- (end of YAltitude cleanup)
}
//--- (YAltitude implementation)
// static attributes
const double YAltitude::QNH_INVALID = YAPI_INVALID_DOUBLE;
const string YAltitude::TECHNOLOGY_INVALID = YAPI_INVALID_STRING;

int YAltitude::_parseAttr(yJsonStateMachine& j)
{
    if(!strcmp(j.token, "qnh")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _qnh =  floor(atof(j.token) * 1000.0 / 65536.0 + 0.5) / 1000.0;
        return 1;
    }
    if(!strcmp(j.token, "technology")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _technology =  _parseString(j);
        return 1;
    }
    failed:
    return YSensor::_parseAttr(j);
}


/**
 * Changes the current estimated altitude. This allows to compensate for
 * ambient pressure variations and to work in relative mode.
 *
 * @param newval : a floating point number corresponding to the current estimated altitude
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YAltitude::set_currentValue(double newval)
{
    string rest_val;
    char buf[32]; sprintf(buf,"%d", (int)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
    return _setAttr("currentValue", rest_val);
}

/**
 * Changes the barometric pressure adjusted to sea level used to compute
 * the altitude (QNH). This enables you to compensate for atmospheric pressure
 * changes due to weather conditions.
 *
 * @param newval : a floating point number corresponding to the barometric pressure adjusted to sea
 * level used to compute
 *         the altitude (QNH)
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YAltitude::set_qnh(double newval)
{
    string rest_val;
    char buf[32]; sprintf(buf,"%d", (int)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
    return _setAttr("qnh", rest_val);
}

/**
 * Returns the barometric pressure adjusted to sea level used to compute
 * the altitude (QNH).
 *
 * @return a floating point number corresponding to the barometric pressure adjusted to sea level used to compute
 *         the altitude (QNH)
 *
 * On failure, throws an exception or returns Y_QNH_INVALID.
 */
double YAltitude::get_qnh(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YAltitude::QNH_INVALID;
        }
    }
    return _qnh;
}

/**
 * Returns the technology used by the sesnor to compute
 * altitude. Possibles values are  "barometric" and "gps"
 *
 * @return a string corresponding to the technology used by the sesnor to compute
 *         altitude
 *
 * On failure, throws an exception or returns Y_TECHNOLOGY_INVALID.
 */
string YAltitude::get_technology(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YAltitude::TECHNOLOGY_INVALID;
        }
    }
    return _technology;
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
 * Use the method YAltitude.isOnline() to test if $THEFUNCTION$ is
 * indeed online at a given time. In case of ambiguity when looking for
 * $AFUNCTION$ by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * @param func : a string that uniquely characterizes $THEFUNCTION$
 *
 * @return a YAltitude object allowing you to drive $THEFUNCTION$.
 */
YAltitude* YAltitude::FindAltitude(string func)
{
    YAltitude* obj = NULL;
    obj = (YAltitude*) YFunction::_FindFromCache("Altitude", func);
    if (obj == NULL) {
        obj = new YAltitude(func);
        YFunction::_AddToCache("Altitude", func, obj);
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
int YAltitude::registerValueCallback(YAltitudeValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackAltitude = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YAltitude::_invokeValueCallback(string value)
{
    if (_valueCallbackAltitude != NULL) {
        _valueCallbackAltitude(this, value);
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
int YAltitude::registerTimedReportCallback(YAltitudeTimedReportCallback callback)
{
    YSensor* sensor = NULL;
    sensor = this;
    if (callback != NULL) {
        YFunction::_UpdateTimedReportCallbackList(sensor, true);
    } else {
        YFunction::_UpdateTimedReportCallbackList(sensor, false);
    }
    _timedReportCallbackAltitude = callback;
    return 0;
}

int YAltitude::_invokeTimedReportCallback(YMeasure value)
{
    if (_timedReportCallbackAltitude != NULL) {
        _timedReportCallbackAltitude(this, value);
    } else {
        YSensor::_invokeTimedReportCallback(value);
    }
    return 0;
}

YAltitude *YAltitude::nextAltitude(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YAltitude::FindAltitude(hwid);
}

YAltitude* YAltitude::FirstAltitude(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("Altitude", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YAltitude::FindAltitude(serial+"."+funcId);
}

//--- (end of YAltitude implementation)

//--- (Altitude functions)
//--- (end of Altitude functions)
