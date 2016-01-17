/*********************************************************************
 *
 * $Id: yocto_genericsensor.cpp 22694 2016-01-12 23:13:27Z seb $
 *
 * Implements yFindGenericSensor(), the high-level API for GenericSensor functions
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
#include "yocto_genericsensor.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

YGenericSensor::YGenericSensor(const string& func): YSensor(func)
//--- (GenericSensor initialization)
    ,_signalValue(SIGNALVALUE_INVALID)
    ,_signalUnit(SIGNALUNIT_INVALID)
    ,_signalRange(SIGNALRANGE_INVALID)
    ,_valueRange(VALUERANGE_INVALID)
    ,_signalBias(SIGNALBIAS_INVALID)
    ,_signalSampling(SIGNALSAMPLING_INVALID)
    ,_valueCallbackGenericSensor(NULL)
    ,_timedReportCallbackGenericSensor(NULL)
//--- (end of GenericSensor initialization)
{
    _className="GenericSensor";
}

YGenericSensor::~YGenericSensor()
{
//--- (YGenericSensor cleanup)
//--- (end of YGenericSensor cleanup)
}
//--- (YGenericSensor implementation)
// static attributes
const double YGenericSensor::SIGNALVALUE_INVALID = YAPI_INVALID_DOUBLE;
const string YGenericSensor::SIGNALUNIT_INVALID = YAPI_INVALID_STRING;
const string YGenericSensor::SIGNALRANGE_INVALID = YAPI_INVALID_STRING;
const string YGenericSensor::VALUERANGE_INVALID = YAPI_INVALID_STRING;
const double YGenericSensor::SIGNALBIAS_INVALID = YAPI_INVALID_DOUBLE;

int YGenericSensor::_parseAttr(yJsonStateMachine& j)
{
    if(!strcmp(j.token, "signalValue")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _signalValue =  floor(atof(j.token) * 1000.0 / 65536.0 + 0.5) / 1000.0;
        return 1;
    }
    if(!strcmp(j.token, "signalUnit")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _signalUnit =  _parseString(j);
        return 1;
    }
    if(!strcmp(j.token, "signalRange")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _signalRange =  _parseString(j);
        return 1;
    }
    if(!strcmp(j.token, "valueRange")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _valueRange =  _parseString(j);
        return 1;
    }
    if(!strcmp(j.token, "signalBias")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _signalBias =  floor(atof(j.token) * 1000.0 / 65536.0 + 0.5) / 1000.0;
        return 1;
    }
    if(!strcmp(j.token, "signalSampling")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _signalSampling =  (Y_SIGNALSAMPLING_enum)atoi(j.token);
        return 1;
    }
    failed:
    return YSensor::_parseAttr(j);
}


/**
 * Changes the measuring unit for the measured value.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : a string corresponding to the measuring unit for the measured value
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YGenericSensor::set_unit(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("unit", rest_val);
}

/**
 * Returns the measured value of the electrical signal used by the sensor.
 *
 * @return a floating point number corresponding to the measured value of the electrical signal used by the sensor
 *
 * On failure, throws an exception or returns Y_SIGNALVALUE_INVALID.
 */
double YGenericSensor::get_signalValue(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YGenericSensor::SIGNALVALUE_INVALID;
        }
    }
    return floor(_signalValue * 1000+0.5) / 1000;
}

/**
 * Returns the measuring unit of the electrical signal used by the sensor.
 *
 * @return a string corresponding to the measuring unit of the electrical signal used by the sensor
 *
 * On failure, throws an exception or returns Y_SIGNALUNIT_INVALID.
 */
string YGenericSensor::get_signalUnit(void)
{
    if (_cacheExpiration == 0) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YGenericSensor::SIGNALUNIT_INVALID;
        }
    }
    return _signalUnit;
}

/**
 * Returns the electric signal range used by the sensor.
 *
 * @return a string corresponding to the electric signal range used by the sensor
 *
 * On failure, throws an exception or returns Y_SIGNALRANGE_INVALID.
 */
string YGenericSensor::get_signalRange(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YGenericSensor::SIGNALRANGE_INVALID;
        }
    }
    return _signalRange;
}

/**
 * Changes the electric signal range used by the sensor.
 *
 * @param newval : a string corresponding to the electric signal range used by the sensor
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YGenericSensor::set_signalRange(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("signalRange", rest_val);
}

/**
 * Returns the physical value range measured by the sensor.
 *
 * @return a string corresponding to the physical value range measured by the sensor
 *
 * On failure, throws an exception or returns Y_VALUERANGE_INVALID.
 */
string YGenericSensor::get_valueRange(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YGenericSensor::VALUERANGE_INVALID;
        }
    }
    return _valueRange;
}

/**
 * Changes the physical value range measured by the sensor. As a side effect, the range modification may
 * automatically modify the display resolution.
 *
 * @param newval : a string corresponding to the physical value range measured by the sensor
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YGenericSensor::set_valueRange(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("valueRange", rest_val);
}

/**
 * Changes the electric signal bias for zero shift adjustment.
 * If your electric signal reads positif when it should be zero, setup
 * a positive signalBias of the same value to fix the zero shift.
 *
 * @param newval : a floating point number corresponding to the electric signal bias for zero shift adjustment
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YGenericSensor::set_signalBias(double newval)
{
    string rest_val;
    char buf[32]; sprintf(buf,"%d", (int)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
    return _setAttr("signalBias", rest_val);
}

/**
 * Returns the electric signal bias for zero shift adjustment.
 * A positive bias means that the signal is over-reporting the measure,
 * while a negative bias means that the signal is underreporting the measure.
 *
 * @return a floating point number corresponding to the electric signal bias for zero shift adjustment
 *
 * On failure, throws an exception or returns Y_SIGNALBIAS_INVALID.
 */
double YGenericSensor::get_signalBias(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YGenericSensor::SIGNALBIAS_INVALID;
        }
    }
    return _signalBias;
}

/**
 * Returns the electric signal sampling method to use.
 * The HIGH_RATE method uses the highest sampling frequency, without any filtering.
 * The HIGH_RATE_FILTERED method adds a windowed 7-sample median filter.
 * The LOW_NOISE method uses a reduced acquisition frequency to reduce noise.
 * The LOW_NOISE_FILTERED method combines a reduced frequency with the median filter
 * to get measures as stable as possible when working on a noisy signal.
 *
 * @return a value among Y_SIGNALSAMPLING_HIGH_RATE, Y_SIGNALSAMPLING_HIGH_RATE_FILTERED,
 * Y_SIGNALSAMPLING_LOW_NOISE and Y_SIGNALSAMPLING_LOW_NOISE_FILTERED corresponding to the electric
 * signal sampling method to use
 *
 * On failure, throws an exception or returns Y_SIGNALSAMPLING_INVALID.
 */
Y_SIGNALSAMPLING_enum YGenericSensor::get_signalSampling(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YGenericSensor::SIGNALSAMPLING_INVALID;
        }
    }
    return _signalSampling;
}

/**
 * Changes the electric signal sampling method to use.
 * The HIGH_RATE method uses the highest sampling frequency, without any filtering.
 * The HIGH_RATE_FILTERED method adds a windowed 7-sample median filter.
 * The LOW_NOISE method uses a reduced acquisition frequency to reduce noise.
 * The LOW_NOISE_FILTERED method combines a reduced frequency with the median filter
 * to get measures as stable as possible when working on a noisy signal.
 *
 * @param newval : a value among Y_SIGNALSAMPLING_HIGH_RATE, Y_SIGNALSAMPLING_HIGH_RATE_FILTERED,
 * Y_SIGNALSAMPLING_LOW_NOISE and Y_SIGNALSAMPLING_LOW_NOISE_FILTERED corresponding to the electric
 * signal sampling method to use
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YGenericSensor::set_signalSampling(Y_SIGNALSAMPLING_enum newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("signalSampling", rest_val);
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
 * Use the method YGenericSensor.isOnline() to test if $THEFUNCTION$ is
 * indeed online at a given time. In case of ambiguity when looking for
 * $AFUNCTION$ by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * @param func : a string that uniquely characterizes $THEFUNCTION$
 *
 * @return a YGenericSensor object allowing you to drive $THEFUNCTION$.
 */
YGenericSensor* YGenericSensor::FindGenericSensor(string func)
{
    YGenericSensor* obj = NULL;
    obj = (YGenericSensor*) YFunction::_FindFromCache("GenericSensor", func);
    if (obj == NULL) {
        obj = new YGenericSensor(func);
        YFunction::_AddToCache("GenericSensor", func, obj);
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
int YGenericSensor::registerValueCallback(YGenericSensorValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackGenericSensor = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YGenericSensor::_invokeValueCallback(string value)
{
    if (_valueCallbackGenericSensor != NULL) {
        _valueCallbackGenericSensor(this, value);
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
int YGenericSensor::registerTimedReportCallback(YGenericSensorTimedReportCallback callback)
{
    YSensor* sensor = NULL;
    sensor = this;
    if (callback != NULL) {
        YFunction::_UpdateTimedReportCallbackList(sensor, true);
    } else {
        YFunction::_UpdateTimedReportCallbackList(sensor, false);
    }
    _timedReportCallbackGenericSensor = callback;
    return 0;
}

int YGenericSensor::_invokeTimedReportCallback(YMeasure value)
{
    if (_timedReportCallbackGenericSensor != NULL) {
        _timedReportCallbackGenericSensor(this, value);
    } else {
        YSensor::_invokeTimedReportCallback(value);
    }
    return 0;
}

/**
 * Adjusts the signal bias so that the current signal value is need
 * precisely as zero.
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YGenericSensor::zeroAdjust(void)
{
    double currSignal = 0.0;
    double currBias = 0.0;
    currSignal = this->get_signalValue();
    currBias = this->get_signalBias();
    return this->set_signalBias(currSignal + currBias);
}

YGenericSensor *YGenericSensor::nextGenericSensor(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YGenericSensor::FindGenericSensor(hwid);
}

YGenericSensor* YGenericSensor::FirstGenericSensor(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("GenericSensor", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YGenericSensor::FindGenericSensor(serial+"."+funcId);
}

//--- (end of YGenericSensor implementation)

//--- (GenericSensor functions)
//--- (end of GenericSensor functions)
