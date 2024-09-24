/*********************************************************************
 *
 *  $Id: svn_id $
 *
 *  Implements yFindSpectralSensor(), the high-level API for SpectralSensor functions
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

#include "yocto_spectralsensor.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#define  __FILE_ID__  "spectralsensor"

#ifdef YOCTOLIB_NAMESPACE
using namespace YOCTOLIB_NAMESPACE;
#endif

YSpectralSensor::YSpectralSensor(const string& func): YFunction(func)
//--- (YSpectralSensor initialization)
    ,_ledCurrent(LEDCURRENT_INVALID)
    ,_resolution(RESOLUTION_INVALID)
    ,_integrationTime(INTEGRATIONTIME_INVALID)
    ,_gain(GAIN_INVALID)
    ,_saturation(SATURATION_INVALID)
    ,_ledCurrentAtPowerOn(LEDCURRENTATPOWERON_INVALID)
    ,_integrationTimeAtPowerOn(INTEGRATIONTIMEATPOWERON_INVALID)
    ,_gainAtPowerOn(GAINATPOWERON_INVALID)
    ,_valueCallbackSpectralSensor(NULL)
//--- (end of YSpectralSensor initialization)
{
    _className="SpectralSensor";
}

YSpectralSensor::~YSpectralSensor()
{
//--- (YSpectralSensor cleanup)
//--- (end of YSpectralSensor cleanup)
}
//--- (YSpectralSensor implementation)
// static attributes
const double YSpectralSensor::RESOLUTION_INVALID = YAPI_INVALID_DOUBLE;

int YSpectralSensor::_parseAttr(YJSONObject *json_val)
{
    if(json_val->has("ledCurrent")) {
        _ledCurrent =  json_val->getInt("ledCurrent");
    }
    if(json_val->has("resolution")) {
        _resolution =  floor(json_val->getDouble("resolution") / 65.536 + 0.5) / 1000.0;
    }
    if(json_val->has("integrationTime")) {
        _integrationTime =  json_val->getInt("integrationTime");
    }
    if(json_val->has("gain")) {
        _gain =  json_val->getInt("gain");
    }
    if(json_val->has("saturation")) {
        _saturation =  json_val->getInt("saturation");
    }
    if(json_val->has("ledCurrentAtPowerOn")) {
        _ledCurrentAtPowerOn =  json_val->getInt("ledCurrentAtPowerOn");
    }
    if(json_val->has("integrationTimeAtPowerOn")) {
        _integrationTimeAtPowerOn =  json_val->getInt("integrationTimeAtPowerOn");
    }
    if(json_val->has("gainAtPowerOn")) {
        _gainAtPowerOn =  json_val->getInt("gainAtPowerOn");
    }
    return YFunction::_parseAttr(json_val);
}


/**
 *
 * @return an integer
 *
 * On failure, throws an exception or returns YSpectralSensor::LEDCURRENT_INVALID.
 */
int YSpectralSensor::get_ledCurrent(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSpectralSensor::LEDCURRENT_INVALID;
                }
            }
        }
        res = _ledCurrent;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the luminosity of the module leds. The parameter is a
 * value between 0 and 100.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : an integer corresponding to the luminosity of the module leds
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YSpectralSensor::set_ledCurrent(int newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("ledCurrent", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the resolution of the measured physical values. The resolution corresponds to the numerical precision
 * when displaying value. It does not change the precision of the measure itself.
 * Remember to call the saveToFlash() method of the module if the modification must be kept.
 *
 * @param newval : a floating point number corresponding to the resolution of the measured physical values
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YSpectralSensor::set_resolution(double newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%" FMTs64, (s64)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
        res = _setAttr("resolution", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the resolution of the measured values. The resolution corresponds to the numerical precision
 * of the measures, which is not always the same as the actual precision of the sensor.
 * Remember to call the saveToFlash() method of the module if the modification must be kept.
 *
 * @return a floating point number corresponding to the resolution of the measured values
 *
 * On failure, throws an exception or returns YSpectralSensor::RESOLUTION_INVALID.
 */
double YSpectralSensor::get_resolution(void)
{
    double res = 0.0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSpectralSensor::RESOLUTION_INVALID;
                }
            }
        }
        res = _resolution;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 *
 * @return an integer
 *
 * On failure, throws an exception or returns YSpectralSensor::INTEGRATIONTIME_INVALID.
 */
int YSpectralSensor::get_integrationTime(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSpectralSensor::INTEGRATIONTIME_INVALID;
                }
            }
        }
        res = _integrationTime;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Change the integration time for a measure. The parameter is a
 * value between 0 and 100.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : an integer
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YSpectralSensor::set_integrationTime(int newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("integrationTime", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 *
 * @return an integer
 *
 * On failure, throws an exception or returns YSpectralSensor::GAIN_INVALID.
 */
int YSpectralSensor::get_gain(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSpectralSensor::GAIN_INVALID;
                }
            }
        }
        res = _gain;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : an integer
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YSpectralSensor::set_gain(int newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("gain", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 *
 * @return an integer
 *
 * On failure, throws an exception or returns YSpectralSensor::SATURATION_INVALID.
 */
int YSpectralSensor::get_saturation(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSpectralSensor::SATURATION_INVALID;
                }
            }
        }
        res = _saturation;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 *
 * @return an integer
 *
 * On failure, throws an exception or returns YSpectralSensor::LEDCURRENTATPOWERON_INVALID.
 */
int YSpectralSensor::get_ledCurrentAtPowerOn(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSpectralSensor::LEDCURRENTATPOWERON_INVALID;
                }
            }
        }
        res = _ledCurrentAtPowerOn;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 *
 * @param newval : an integer
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YSpectralSensor::set_ledCurrentAtPowerOn(int newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("ledCurrentAtPowerOn", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 *
 * @return an integer
 *
 * On failure, throws an exception or returns YSpectralSensor::INTEGRATIONTIMEATPOWERON_INVALID.
 */
int YSpectralSensor::get_integrationTimeAtPowerOn(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSpectralSensor::INTEGRATIONTIMEATPOWERON_INVALID;
                }
            }
        }
        res = _integrationTimeAtPowerOn;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 *
 * @param newval : an integer
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YSpectralSensor::set_integrationTimeAtPowerOn(int newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("integrationTimeAtPowerOn", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 *
 * @return an integer
 *
 * On failure, throws an exception or returns YSpectralSensor::GAINATPOWERON_INVALID.
 */
int YSpectralSensor::get_gainAtPowerOn(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSpectralSensor::GAINATPOWERON_INVALID;
                }
            }
        }
        res = _gainAtPowerOn;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 *
 * @param newval : an integer
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YSpectralSensor::set_gainAtPowerOn(int newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("gainAtPowerOn", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Retrieves a spectral sensor for a given identifier.
 * The identifier can be specified using several formats:
 *
 * - FunctionLogicalName
 * - ModuleSerialNumber.FunctionIdentifier
 * - ModuleSerialNumber.FunctionLogicalName
 * - ModuleLogicalName.FunctionIdentifier
 * - ModuleLogicalName.FunctionLogicalName
 *
 *
 * This function does not require that the spectral sensor is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the spectral sensor is
 * indeed online at a given time. In case of ambiguity when looking for
 * a spectral sensor by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the spectral sensor, for instance
 *         MyDevice.spectralSensor.
 *
 * @return a YSpectralSensor object allowing you to drive the spectral sensor.
 */
YSpectralSensor* YSpectralSensor::FindSpectralSensor(string func)
{
    YSpectralSensor* obj = NULL;
    int taken = 0;
    if (YAPI::_apiInitialized) {
        yEnterCriticalSection(&YAPI::_global_cs);
        taken = 1;
    }try {
        obj = (YSpectralSensor*) YFunction::_FindFromCache("SpectralSensor", func);
        if (obj == NULL) {
            obj = new YSpectralSensor(func);
            YFunction::_AddToCache("SpectralSensor", func, obj);
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
int YSpectralSensor::registerValueCallback(YSpectralSensorValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackSpectralSensor = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YSpectralSensor::_invokeValueCallback(string value)
{
    if (_valueCallbackSpectralSensor != NULL) {
        _valueCallbackSpectralSensor(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

YSpectralSensor *YSpectralSensor::nextSpectralSensor(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YSpectralSensor::FindSpectralSensor(hwid);
}

YSpectralSensor *YSpectralSensor::FirstSpectralSensor(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("SpectralSensor", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YSpectralSensor::FindSpectralSensor(serial+"."+funcId);
}

//--- (end of YSpectralSensor implementation)

//--- (YSpectralSensor functions)
//--- (end of YSpectralSensor functions)
