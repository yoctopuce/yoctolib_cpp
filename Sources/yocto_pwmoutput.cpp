/*********************************************************************
 *
 * $Id: yocto_pwmoutput.cpp 22191 2015-12-02 06:49:31Z mvuilleu $
 *
 * Implements yFindPwmOutput(), the high-level API for PwmOutput functions
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
#include "yocto_pwmoutput.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

YPwmOutput::YPwmOutput(const string& func): YFunction(func)
//--- (PwmOutput initialization)
    ,_enabled(ENABLED_INVALID)
    ,_frequency(FREQUENCY_INVALID)
    ,_period(PERIOD_INVALID)
    ,_dutyCycle(DUTYCYCLE_INVALID)
    ,_pulseDuration(PULSEDURATION_INVALID)
    ,_pwmTransition(PWMTRANSITION_INVALID)
    ,_enabledAtPowerOn(ENABLEDATPOWERON_INVALID)
    ,_dutyCycleAtPowerOn(DUTYCYCLEATPOWERON_INVALID)
    ,_valueCallbackPwmOutput(NULL)
//--- (end of PwmOutput initialization)
{
    _className="PwmOutput";
}

YPwmOutput::~YPwmOutput()
{
//--- (YPwmOutput cleanup)
//--- (end of YPwmOutput cleanup)
}
//--- (YPwmOutput implementation)
// static attributes
const double YPwmOutput::FREQUENCY_INVALID = YAPI_INVALID_DOUBLE;
const double YPwmOutput::PERIOD_INVALID = YAPI_INVALID_DOUBLE;
const double YPwmOutput::DUTYCYCLE_INVALID = YAPI_INVALID_DOUBLE;
const double YPwmOutput::PULSEDURATION_INVALID = YAPI_INVALID_DOUBLE;
const string YPwmOutput::PWMTRANSITION_INVALID = YAPI_INVALID_STRING;
const double YPwmOutput::DUTYCYCLEATPOWERON_INVALID = YAPI_INVALID_DOUBLE;

int YPwmOutput::_parseAttr(yJsonStateMachine& j)
{
    if(!strcmp(j.token, "enabled")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _enabled =  (Y_ENABLED_enum)atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "frequency")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _frequency =  floor(atof(j.token) * 1000.0 / 65536.0 + 0.5) / 1000.0;
        return 1;
    }
    if(!strcmp(j.token, "period")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _period =  floor(atof(j.token) * 1000.0 / 65536.0 + 0.5) / 1000.0;
        return 1;
    }
    if(!strcmp(j.token, "dutyCycle")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _dutyCycle =  floor(atof(j.token) * 1000.0 / 65536.0 + 0.5) / 1000.0;
        return 1;
    }
    if(!strcmp(j.token, "pulseDuration")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _pulseDuration =  floor(atof(j.token) * 1000.0 / 65536.0 + 0.5) / 1000.0;
        return 1;
    }
    if(!strcmp(j.token, "pwmTransition")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _pwmTransition =  _parseString(j);
        return 1;
    }
    if(!strcmp(j.token, "enabledAtPowerOn")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _enabledAtPowerOn =  (Y_ENABLEDATPOWERON_enum)atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "dutyCycleAtPowerOn")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _dutyCycleAtPowerOn =  floor(atof(j.token) * 1000.0 / 65536.0 + 0.5) / 1000.0;
        return 1;
    }
    failed:
    return YFunction::_parseAttr(j);
}


/**
 * Returns the state of the PWMs.
 *
 * @return either Y_ENABLED_FALSE or Y_ENABLED_TRUE, according to the state of the PWMs
 *
 * On failure, throws an exception or returns Y_ENABLED_INVALID.
 */
Y_ENABLED_enum YPwmOutput::get_enabled(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YPwmOutput::ENABLED_INVALID;
        }
    }
    return _enabled;
}

/**
 * Stops or starts the PWM.
 *
 * @param newval : either Y_ENABLED_FALSE or Y_ENABLED_TRUE
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YPwmOutput::set_enabled(Y_ENABLED_enum newval)
{
    string rest_val;
    rest_val = (newval>0 ? "1" : "0");
    return _setAttr("enabled", rest_val);
}

/**
 * Changes the PWM frequency. The duty cycle is kept unchanged thanks to an
 * automatic pulse width change.
 *
 * @param newval : a floating point number corresponding to the PWM frequency
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YPwmOutput::set_frequency(double newval)
{
    string rest_val;
    char buf[32]; sprintf(buf,"%d", (int)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
    return _setAttr("frequency", rest_val);
}

/**
 * Returns the PWM frequency in Hz.
 *
 * @return a floating point number corresponding to the PWM frequency in Hz
 *
 * On failure, throws an exception or returns Y_FREQUENCY_INVALID.
 */
double YPwmOutput::get_frequency(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YPwmOutput::FREQUENCY_INVALID;
        }
    }
    return _frequency;
}

/**
 * Changes the PWM period in milliseconds.
 *
 * @param newval : a floating point number corresponding to the PWM period in milliseconds
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YPwmOutput::set_period(double newval)
{
    string rest_val;
    char buf[32]; sprintf(buf,"%d", (int)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
    return _setAttr("period", rest_val);
}

/**
 * Returns the PWM period in milliseconds.
 *
 * @return a floating point number corresponding to the PWM period in milliseconds
 *
 * On failure, throws an exception or returns Y_PERIOD_INVALID.
 */
double YPwmOutput::get_period(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YPwmOutput::PERIOD_INVALID;
        }
    }
    return _period;
}

/**
 * Changes the PWM duty cycle, in per cents.
 *
 * @param newval : a floating point number corresponding to the PWM duty cycle, in per cents
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YPwmOutput::set_dutyCycle(double newval)
{
    string rest_val;
    char buf[32]; sprintf(buf,"%d", (int)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
    return _setAttr("dutyCycle", rest_val);
}

/**
 * Returns the PWM duty cycle, in per cents.
 *
 * @return a floating point number corresponding to the PWM duty cycle, in per cents
 *
 * On failure, throws an exception or returns Y_DUTYCYCLE_INVALID.
 */
double YPwmOutput::get_dutyCycle(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YPwmOutput::DUTYCYCLE_INVALID;
        }
    }
    return _dutyCycle;
}

/**
 * Changes the PWM pulse length, in milliseconds. A pulse length cannot be longer than period,
 * otherwise it is truncated.
 *
 * @param newval : a floating point number corresponding to the PWM pulse length, in milliseconds
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YPwmOutput::set_pulseDuration(double newval)
{
    string rest_val;
    char buf[32]; sprintf(buf,"%d", (int)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
    return _setAttr("pulseDuration", rest_val);
}

/**
 * Returns the PWM pulse length in milliseconds, as a floating point number.
 *
 * @return a floating point number corresponding to the PWM pulse length in milliseconds, as a
 * floating point number
 *
 * On failure, throws an exception or returns Y_PULSEDURATION_INVALID.
 */
double YPwmOutput::get_pulseDuration(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YPwmOutput::PULSEDURATION_INVALID;
        }
    }
    return _pulseDuration;
}

string YPwmOutput::get_pwmTransition(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YPwmOutput::PWMTRANSITION_INVALID;
        }
    }
    return _pwmTransition;
}

int YPwmOutput::set_pwmTransition(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("pwmTransition", rest_val);
}

/**
 * Returns the state of the PWM at device power on.
 *
 * @return either Y_ENABLEDATPOWERON_FALSE or Y_ENABLEDATPOWERON_TRUE, according to the state of the
 * PWM at device power on
 *
 * On failure, throws an exception or returns Y_ENABLEDATPOWERON_INVALID.
 */
Y_ENABLEDATPOWERON_enum YPwmOutput::get_enabledAtPowerOn(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YPwmOutput::ENABLEDATPOWERON_INVALID;
        }
    }
    return _enabledAtPowerOn;
}

/**
 * Changes the state of the PWM at device power on. Remember to call the matching module saveToFlash()
 * method, otherwise this call will have no effect.
 *
 * @param newval : either Y_ENABLEDATPOWERON_FALSE or Y_ENABLEDATPOWERON_TRUE, according to the state
 * of the PWM at device power on
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YPwmOutput::set_enabledAtPowerOn(Y_ENABLEDATPOWERON_enum newval)
{
    string rest_val;
    rest_val = (newval>0 ? "1" : "0");
    return _setAttr("enabledAtPowerOn", rest_val);
}

/**
 * Changes the PWM duty cycle at device power on. Remember to call the matching
 * module saveToFlash() method, otherwise this call will have no effect.
 *
 * @param newval : a floating point number corresponding to the PWM duty cycle at device power on
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YPwmOutput::set_dutyCycleAtPowerOn(double newval)
{
    string rest_val;
    char buf[32]; sprintf(buf,"%d", (int)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
    return _setAttr("dutyCycleAtPowerOn", rest_val);
}

/**
 * Returns the PWMs duty cycle at device power on as a floating point number between 0 and 100
 *
 * @return a floating point number corresponding to the PWMs duty cycle at device power on as a
 * floating point number between 0 and 100
 *
 * On failure, throws an exception or returns Y_DUTYCYCLEATPOWERON_INVALID.
 */
double YPwmOutput::get_dutyCycleAtPowerOn(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YPwmOutput::DUTYCYCLEATPOWERON_INVALID;
        }
    }
    return _dutyCycleAtPowerOn;
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
 * Use the method YPwmOutput.isOnline() to test if $THEFUNCTION$ is
 * indeed online at a given time. In case of ambiguity when looking for
 * $AFUNCTION$ by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * @param func : a string that uniquely characterizes $THEFUNCTION$
 *
 * @return a YPwmOutput object allowing you to drive $THEFUNCTION$.
 */
YPwmOutput* YPwmOutput::FindPwmOutput(string func)
{
    YPwmOutput* obj = NULL;
    obj = (YPwmOutput*) YFunction::_FindFromCache("PwmOutput", func);
    if (obj == NULL) {
        obj = new YPwmOutput(func);
        YFunction::_AddToCache("PwmOutput", func, obj);
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
int YPwmOutput::registerValueCallback(YPwmOutputValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackPwmOutput = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YPwmOutput::_invokeValueCallback(string value)
{
    if (_valueCallbackPwmOutput != NULL) {
        _valueCallbackPwmOutput(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

/**
 * Performs a smooth transistion of the pulse duration toward a given value. Any period,
 * frequency, duty cycle or pulse width change will cancel any ongoing transition process.
 *
 * @param ms_target   : new pulse duration at the end of the transition
 *         (floating-point number, representing the pulse duration in milliseconds)
 * @param ms_duration : total duration of the transition, in milliseconds
 *
 * @return YAPI_SUCCESS when the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YPwmOutput::pulseDurationMove(double ms_target,int ms_duration)
{
    string newval;
    if (ms_target < 0.0) {
        ms_target = 0.0;
    }
    newval = YapiWrapper::ysprintf("%dms:%d", (int) floor(ms_target*65536+0.5),ms_duration);
    return this->set_pwmTransition(newval);
}

/**
 * Performs a smooth change of the pulse duration toward a given value.
 *
 * @param target      : new duty cycle at the end of the transition
 *         (floating-point number, between 0 and 1)
 * @param ms_duration : total duration of the transition, in milliseconds
 *
 * @return YAPI_SUCCESS when the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YPwmOutput::dutyCycleMove(double target,int ms_duration)
{
    string newval;
    if (target < 0.0) {
        target = 0.0;
    }
    if (target > 100.0) {
        target = 100.0;
    }
    newval = YapiWrapper::ysprintf("%d:%d", (int) floor(target*65536+0.5),ms_duration);
    return this->set_pwmTransition(newval);
}

YPwmOutput *YPwmOutput::nextPwmOutput(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YPwmOutput::FindPwmOutput(hwid);
}

YPwmOutput* YPwmOutput::FirstPwmOutput(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("PwmOutput", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YPwmOutput::FindPwmOutput(serial+"."+funcId);
}

//--- (end of YPwmOutput implementation)

//--- (PwmOutput functions)
//--- (end of PwmOutput functions)
