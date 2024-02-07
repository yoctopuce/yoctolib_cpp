/*********************************************************************
 *
 *  $Id: yocto_powersupply.cpp 55635 2023-07-26 09:20:02Z seb $
 *
 *  Implements yFindPowerSupply(), the high-level API for PowerSupply functions
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

#include "yocto_powersupply.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#define  __FILE_ID__  "powersupply"

#ifdef YOCTOLIB_NAMESPACE
using namespace YOCTOLIB_NAMESPACE;
#endif

YPowerSupply::YPowerSupply(const string& func): YFunction(func)
//--- (YPowerSupply initialization)
    ,_voltageLimit(VOLTAGELIMIT_INVALID)
    ,_currentLimit(CURRENTLIMIT_INVALID)
    ,_powerOutput(POWEROUTPUT_INVALID)
    ,_measuredVoltage(MEASUREDVOLTAGE_INVALID)
    ,_measuredCurrent(MEASUREDCURRENT_INVALID)
    ,_inputVoltage(INPUTVOLTAGE_INVALID)
    ,_voltageTransition(VOLTAGETRANSITION_INVALID)
    ,_voltageLimitAtStartUp(VOLTAGELIMITATSTARTUP_INVALID)
    ,_currentLimitAtStartUp(CURRENTLIMITATSTARTUP_INVALID)
    ,_powerOutputAtStartUp(POWEROUTPUTATSTARTUP_INVALID)
    ,_command(COMMAND_INVALID)
    ,_valueCallbackPowerSupply(NULL)
//--- (end of YPowerSupply initialization)
{
    _className="PowerSupply";
}

YPowerSupply::~YPowerSupply()
{
//--- (YPowerSupply cleanup)
//--- (end of YPowerSupply cleanup)
}
//--- (YPowerSupply implementation)
// static attributes
const double YPowerSupply::VOLTAGELIMIT_INVALID = YAPI_INVALID_DOUBLE;
const double YPowerSupply::CURRENTLIMIT_INVALID = YAPI_INVALID_DOUBLE;
const double YPowerSupply::MEASUREDVOLTAGE_INVALID = YAPI_INVALID_DOUBLE;
const double YPowerSupply::MEASUREDCURRENT_INVALID = YAPI_INVALID_DOUBLE;
const double YPowerSupply::INPUTVOLTAGE_INVALID = YAPI_INVALID_DOUBLE;
const string YPowerSupply::VOLTAGETRANSITION_INVALID = YAPI_INVALID_STRING;
const double YPowerSupply::VOLTAGELIMITATSTARTUP_INVALID = YAPI_INVALID_DOUBLE;
const double YPowerSupply::CURRENTLIMITATSTARTUP_INVALID = YAPI_INVALID_DOUBLE;
const string YPowerSupply::COMMAND_INVALID = YAPI_INVALID_STRING;

int YPowerSupply::_parseAttr(YJSONObject *json_val)
{
    if(json_val->has("voltageLimit")) {
        _voltageLimit =  floor(json_val->getDouble("voltageLimit") / 65.536 + 0.5) / 1000.0;
    }
    if(json_val->has("currentLimit")) {
        _currentLimit =  floor(json_val->getDouble("currentLimit") / 65.536 + 0.5) / 1000.0;
    }
    if(json_val->has("powerOutput")) {
        _powerOutput =  (Y_POWEROUTPUT_enum)json_val->getInt("powerOutput");
    }
    if(json_val->has("measuredVoltage")) {
        _measuredVoltage =  floor(json_val->getDouble("measuredVoltage") / 65.536 + 0.5) / 1000.0;
    }
    if(json_val->has("measuredCurrent")) {
        _measuredCurrent =  floor(json_val->getDouble("measuredCurrent") / 65.536 + 0.5) / 1000.0;
    }
    if(json_val->has("inputVoltage")) {
        _inputVoltage =  floor(json_val->getDouble("inputVoltage") / 65.536 + 0.5) / 1000.0;
    }
    if(json_val->has("voltageTransition")) {
        _voltageTransition =  json_val->getString("voltageTransition");
    }
    if(json_val->has("voltageLimitAtStartUp")) {
        _voltageLimitAtStartUp =  floor(json_val->getDouble("voltageLimitAtStartUp") / 65.536 + 0.5) / 1000.0;
    }
    if(json_val->has("currentLimitAtStartUp")) {
        _currentLimitAtStartUp =  floor(json_val->getDouble("currentLimitAtStartUp") / 65.536 + 0.5) / 1000.0;
    }
    if(json_val->has("powerOutputAtStartUp")) {
        _powerOutputAtStartUp =  (Y_POWEROUTPUTATSTARTUP_enum)json_val->getInt("powerOutputAtStartUp");
    }
    if(json_val->has("command")) {
        _command =  json_val->getString("command");
    }
    return YFunction::_parseAttr(json_val);
}


/**
 * Changes the voltage limit, in V.
 *
 * @param newval : a floating point number corresponding to the voltage limit, in V
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YPowerSupply::set_voltageLimit(double newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%" FMTs64, (s64)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
        res = _setAttr("voltageLimit", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the voltage limit, in V.
 *
 * @return a floating point number corresponding to the voltage limit, in V
 *
 * On failure, throws an exception or returns YPowerSupply::VOLTAGELIMIT_INVALID.
 */
double YPowerSupply::get_voltageLimit(void)
{
    double res = 0.0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YPowerSupply::VOLTAGELIMIT_INVALID;
                }
            }
        }
        res = _voltageLimit;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the current limit, in mA.
 *
 * @param newval : a floating point number corresponding to the current limit, in mA
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YPowerSupply::set_currentLimit(double newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%" FMTs64, (s64)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
        res = _setAttr("currentLimit", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the current limit, in mA.
 *
 * @return a floating point number corresponding to the current limit, in mA
 *
 * On failure, throws an exception or returns YPowerSupply::CURRENTLIMIT_INVALID.
 */
double YPowerSupply::get_currentLimit(void)
{
    double res = 0.0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YPowerSupply::CURRENTLIMIT_INVALID;
                }
            }
        }
        res = _currentLimit;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the power supply output switch state.
 *
 * @return either YPowerSupply::POWEROUTPUT_OFF or YPowerSupply::POWEROUTPUT_ON, according to the power
 * supply output switch state
 *
 * On failure, throws an exception or returns YPowerSupply::POWEROUTPUT_INVALID.
 */
Y_POWEROUTPUT_enum YPowerSupply::get_powerOutput(void)
{
    Y_POWEROUTPUT_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YPowerSupply::POWEROUTPUT_INVALID;
                }
            }
        }
        res = _powerOutput;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the power supply output switch state.
 *
 * @param newval : either YPowerSupply::POWEROUTPUT_OFF or YPowerSupply::POWEROUTPUT_ON, according to
 * the power supply output switch state
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YPowerSupply::set_powerOutput(Y_POWEROUTPUT_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = (newval>0 ? "1" : "0");
        res = _setAttr("powerOutput", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the measured output voltage, in V.
 *
 * @return a floating point number corresponding to the measured output voltage, in V
 *
 * On failure, throws an exception or returns YPowerSupply::MEASUREDVOLTAGE_INVALID.
 */
double YPowerSupply::get_measuredVoltage(void)
{
    double res = 0.0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YPowerSupply::MEASUREDVOLTAGE_INVALID;
                }
            }
        }
        res = _measuredVoltage;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the measured output current, in mA.
 *
 * @return a floating point number corresponding to the measured output current, in mA
 *
 * On failure, throws an exception or returns YPowerSupply::MEASUREDCURRENT_INVALID.
 */
double YPowerSupply::get_measuredCurrent(void)
{
    double res = 0.0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YPowerSupply::MEASUREDCURRENT_INVALID;
                }
            }
        }
        res = _measuredCurrent;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the measured input voltage, in V.
 *
 * @return a floating point number corresponding to the measured input voltage, in V
 *
 * On failure, throws an exception or returns YPowerSupply::INPUTVOLTAGE_INVALID.
 */
double YPowerSupply::get_inputVoltage(void)
{
    double res = 0.0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YPowerSupply::INPUTVOLTAGE_INVALID;
                }
            }
        }
        res = _inputVoltage;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

string YPowerSupply::get_voltageTransition(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YPowerSupply::VOLTAGETRANSITION_INVALID;
                }
            }
        }
        res = _voltageTransition;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

int YPowerSupply::set_voltageTransition(const string& newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = newval;
        res = _setAttr("voltageTransition", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the voltage set point at device start up. Remember to call the matching
 * module saveToFlash() method, otherwise this call has no effect.
 *
 * @param newval : a floating point number corresponding to the voltage set point at device start up
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YPowerSupply::set_voltageLimitAtStartUp(double newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%" FMTs64, (s64)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
        res = _setAttr("voltageLimitAtStartUp", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the selected voltage limit at device startup, in V.
 *
 * @return a floating point number corresponding to the selected voltage limit at device startup, in V
 *
 * On failure, throws an exception or returns YPowerSupply::VOLTAGELIMITATSTARTUP_INVALID.
 */
double YPowerSupply::get_voltageLimitAtStartUp(void)
{
    double res = 0.0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YPowerSupply::VOLTAGELIMITATSTARTUP_INVALID;
                }
            }
        }
        res = _voltageLimitAtStartUp;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the current limit at device start up. Remember to call the matching
 * module saveToFlash() method, otherwise this call has no effect.
 *
 * @param newval : a floating point number corresponding to the current limit at device start up
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YPowerSupply::set_currentLimitAtStartUp(double newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%" FMTs64, (s64)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
        res = _setAttr("currentLimitAtStartUp", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the selected current limit at device startup, in mA.
 *
 * @return a floating point number corresponding to the selected current limit at device startup, in mA
 *
 * On failure, throws an exception or returns YPowerSupply::CURRENTLIMITATSTARTUP_INVALID.
 */
double YPowerSupply::get_currentLimitAtStartUp(void)
{
    double res = 0.0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YPowerSupply::CURRENTLIMITATSTARTUP_INVALID;
                }
            }
        }
        res = _currentLimitAtStartUp;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the power supply output switch state.
 *
 * @return either YPowerSupply::POWEROUTPUTATSTARTUP_OFF or YPowerSupply::POWEROUTPUTATSTARTUP_ON,
 * according to the power supply output switch state
 *
 * On failure, throws an exception or returns YPowerSupply::POWEROUTPUTATSTARTUP_INVALID.
 */
Y_POWEROUTPUTATSTARTUP_enum YPowerSupply::get_powerOutputAtStartUp(void)
{
    Y_POWEROUTPUTATSTARTUP_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YPowerSupply::POWEROUTPUTATSTARTUP_INVALID;
                }
            }
        }
        res = _powerOutputAtStartUp;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the power supply output switch state at device start up. Remember to call the matching
 * module saveToFlash() method, otherwise this call has no effect.
 *
 * @param newval : either YPowerSupply::POWEROUTPUTATSTARTUP_OFF or
 * YPowerSupply::POWEROUTPUTATSTARTUP_ON, according to the power supply output switch state at device start up
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YPowerSupply::set_powerOutputAtStartUp(Y_POWEROUTPUTATSTARTUP_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = (newval>0 ? "1" : "0");
        res = _setAttr("powerOutputAtStartUp", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

string YPowerSupply::get_command(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YPowerSupply::COMMAND_INVALID;
                }
            }
        }
        res = _command;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

int YPowerSupply::set_command(const string& newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = newval;
        res = _setAttr("command", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Retrieves a regulated power supply for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that the regulated power supply is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the regulated power supply is
 * indeed online at a given time. In case of ambiguity when looking for
 * a regulated power supply by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the regulated power supply, for instance
 *         MyDevice.powerSupply.
 *
 * @return a YPowerSupply object allowing you to drive the regulated power supply.
 */
YPowerSupply* YPowerSupply::FindPowerSupply(string func)
{
    YPowerSupply* obj = NULL;
    int taken = 0;
    if (YAPI::_apiInitialized) {
        yEnterCriticalSection(&YAPI::_global_cs);
        taken = 1;
    }try {
        obj = (YPowerSupply*) YFunction::_FindFromCache("PowerSupply", func);
        if (obj == NULL) {
            obj = new YPowerSupply(func);
            YFunction::_AddToCache("PowerSupply", func, obj);
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
int YPowerSupply::registerValueCallback(YPowerSupplyValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackPowerSupply = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YPowerSupply::_invokeValueCallback(string value)
{
    if (_valueCallbackPowerSupply != NULL) {
        _valueCallbackPowerSupply(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

/**
 * Performs a smooth transition of output voltage. Any explicit voltage
 * change cancels any ongoing transition process.
 *
 * @param V_target   : new output voltage value at the end of the transition
 *         (floating-point number, representing the end voltage in V)
 * @param ms_duration : total duration of the transition, in milliseconds
 *
 * @return YAPI::SUCCESS when the call succeeds.
 */
int YPowerSupply::voltageMove(double V_target,int ms_duration)
{
    string newval;
    if (V_target < 0.0) {
        V_target  = 0.0;
    }
    newval = YapiWrapper::ysprintf("%d:%d", (int) floor(V_target*65536+0.5),ms_duration);

    return this->set_voltageTransition(newval);
}

YPowerSupply *YPowerSupply::nextPowerSupply(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YPowerSupply::FindPowerSupply(hwid);
}

YPowerSupply *YPowerSupply::FirstPowerSupply(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("PowerSupply", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YPowerSupply::FindPowerSupply(serial+"."+funcId);
}

//--- (end of YPowerSupply implementation)

//--- (YPowerSupply functions)
//--- (end of YPowerSupply functions)
