/*********************************************************************
 *
 *  $Id: yocto_power.cpp 53431 2023-03-06 14:19:35Z seb $
 *
 *  Implements yFindPower(), the high-level API for Power functions
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

#include "yocto_power.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#define  __FILE_ID__  "power"

#ifdef YOCTOLIB_NAMESPACE
using namespace YOCTOLIB_NAMESPACE;
#endif

YPower::YPower(const string& func): YSensor(func)
//--- (YPower initialization)
    ,_powerFactor(POWERFACTOR_INVALID)
    ,_cosPhi(COSPHI_INVALID)
    ,_meter(METER_INVALID)
    ,_deliveredEnergyMeter(DELIVEREDENERGYMETER_INVALID)
    ,_receivedEnergyMeter(RECEIVEDENERGYMETER_INVALID)
    ,_meterTimer(METERTIMER_INVALID)
    ,_valueCallbackPower(NULL)
    ,_timedReportCallbackPower(NULL)
//--- (end of YPower initialization)
{
    _className="Power";
}

YPower::~YPower()
{
//--- (YPower cleanup)
//--- (end of YPower cleanup)
}
//--- (YPower implementation)
// static attributes
const double YPower::POWERFACTOR_INVALID = YAPI_INVALID_DOUBLE;
const double YPower::COSPHI_INVALID = YAPI_INVALID_DOUBLE;
const double YPower::METER_INVALID = YAPI_INVALID_DOUBLE;
const double YPower::DELIVEREDENERGYMETER_INVALID = YAPI_INVALID_DOUBLE;
const double YPower::RECEIVEDENERGYMETER_INVALID = YAPI_INVALID_DOUBLE;

int YPower::_parseAttr(YJSONObject *json_val)
{
    if(json_val->has("powerFactor")) {
        _powerFactor =  floor(json_val->getDouble("powerFactor") / 65.536 + 0.5) / 1000.0;
    }
    if(json_val->has("cosPhi")) {
        _cosPhi =  floor(json_val->getDouble("cosPhi") / 65.536 + 0.5) / 1000.0;
    }
    if(json_val->has("meter")) {
        _meter =  floor(json_val->getDouble("meter") / 65.536 + 0.5) / 1000.0;
    }
    if(json_val->has("deliveredEnergyMeter")) {
        _deliveredEnergyMeter =  floor(json_val->getDouble("deliveredEnergyMeter") / 65.536 + 0.5) / 1000.0;
    }
    if(json_val->has("receivedEnergyMeter")) {
        _receivedEnergyMeter =  floor(json_val->getDouble("receivedEnergyMeter") / 65.536 + 0.5) / 1000.0;
    }
    if(json_val->has("meterTimer")) {
        _meterTimer =  json_val->getInt("meterTimer");
    }
    return YSensor::_parseAttr(json_val);
}


/**
 * Returns the power factor (PF), i.e. ratio between the active power consumed (in W)
 * and the apparent power provided (VA).
 *
 * @return a floating point number corresponding to the power factor (PF), i.e
 *
 * On failure, throws an exception or returns YPower::POWERFACTOR_INVALID.
 */
double YPower::get_powerFactor(void)
{
    double res = 0.0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YPower::POWERFACTOR_INVALID;
                }
            }
        }
        res = _powerFactor;
        if (res == YPower::POWERFACTOR_INVALID) {
            res = _cosPhi;
        }
        res = floor(res * 1000+0.5) / 1000;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the Displacement Power factor (DPF), i.e. cosine of the phase shift between
 * the voltage and current fundamentals.
 * On the Yocto-Watt (V1), the value returned by this method correponds to the
 * power factor as this device is cannot estimate the true DPF.
 *
 * @return a floating point number corresponding to the Displacement Power factor (DPF), i.e
 *
 * On failure, throws an exception or returns YPower::COSPHI_INVALID.
 */
double YPower::get_cosPhi(void)
{
    double res = 0.0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YPower::COSPHI_INVALID;
                }
            }
        }
        res = _cosPhi;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

int YPower::set_meter(double newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%" FMTs64, (s64)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
        res = _setAttr("meter", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the energy counter, maintained by the wattmeter by integrating the
 * power consumption over time. This is the sum of forward and backwad energy transfers,
 * if you are insterested in only one direction, use  get_receivedEnergyMeter() or
 * get_deliveredEnergyMeter(). Note that this counter is reset at each start of the device.
 *
 * @return a floating point number corresponding to the energy counter, maintained by the wattmeter by
 * integrating the
 *         power consumption over time
 *
 * On failure, throws an exception or returns YPower::METER_INVALID.
 */
double YPower::get_meter(void)
{
    double res = 0.0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YPower::METER_INVALID;
                }
            }
        }
        res = _meter;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the energy counter, maintained by the wattmeter by integrating the power consumption over time,
 * but only when positive. Note that this counter is reset at each start of the device.
 *
 * @return a floating point number corresponding to the energy counter, maintained by the wattmeter by
 * integrating the power consumption over time,
 *         but only when positive
 *
 * On failure, throws an exception or returns YPower::DELIVEREDENERGYMETER_INVALID.
 */
double YPower::get_deliveredEnergyMeter(void)
{
    double res = 0.0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YPower::DELIVEREDENERGYMETER_INVALID;
                }
            }
        }
        res = _deliveredEnergyMeter;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the energy counter, maintained by the wattmeter by integrating the power consumption over time,
 * but only when negative. Note that this counter is reset at each start of the device.
 *
 * @return a floating point number corresponding to the energy counter, maintained by the wattmeter by
 * integrating the power consumption over time,
 *         but only when negative
 *
 * On failure, throws an exception or returns YPower::RECEIVEDENERGYMETER_INVALID.
 */
double YPower::get_receivedEnergyMeter(void)
{
    double res = 0.0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YPower::RECEIVEDENERGYMETER_INVALID;
                }
            }
        }
        res = _receivedEnergyMeter;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the elapsed time since last energy counter reset, in seconds.
 *
 * @return an integer corresponding to the elapsed time since last energy counter reset, in seconds
 *
 * On failure, throws an exception or returns YPower::METERTIMER_INVALID.
 */
int YPower::get_meterTimer(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YPower::METERTIMER_INVALID;
                }
            }
        }
        res = _meterTimer;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Retrieves a electrical power sensor for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that the electrical power sensor is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the electrical power sensor is
 * indeed online at a given time. In case of ambiguity when looking for
 * a electrical power sensor by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the electrical power sensor, for instance
 *         YWATTMK1.power.
 *
 * @return a YPower object allowing you to drive the electrical power sensor.
 */
YPower* YPower::FindPower(string func)
{
    YPower* obj = NULL;
    int taken = 0;
    if (YAPI::_apiInitialized) {
        yEnterCriticalSection(&YAPI::_global_cs);
        taken = 1;
    }try {
        obj = (YPower*) YFunction::_FindFromCache("Power", func);
        if (obj == NULL) {
            obj = new YPower(func);
            YFunction::_AddToCache("Power", func, obj);
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
int YPower::registerValueCallback(YPowerValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackPower = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YPower::_invokeValueCallback(string value)
{
    if (_valueCallbackPower != NULL) {
        _valueCallbackPower(this, value);
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
int YPower::registerTimedReportCallback(YPowerTimedReportCallback callback)
{
    YSensor* sensor = NULL;
    sensor = this;
    if (callback != NULL) {
        YFunction::_UpdateTimedReportCallbackList(sensor, true);
    } else {
        YFunction::_UpdateTimedReportCallbackList(sensor, false);
    }
    _timedReportCallbackPower = callback;
    return 0;
}

int YPower::_invokeTimedReportCallback(YMeasure value)
{
    if (_timedReportCallbackPower != NULL) {
        _timedReportCallbackPower(this, value);
    } else {
        YSensor::_invokeTimedReportCallback(value);
    }
    return 0;
}

/**
 * Resets the energy counters.
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YPower::reset(void)
{
    return this->set_meter(0);
}

YPower *YPower::nextPower(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YPower::FindPower(hwid);
}

YPower *YPower::FirstPower(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("Power", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YPower::FindPower(serial+"."+funcId);
}

//--- (end of YPower implementation)

//--- (YPower functions)
//--- (end of YPower functions)
