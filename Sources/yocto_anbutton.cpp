/*********************************************************************
 *
 * $Id: yocto_anbutton.cpp 22191 2015-12-02 06:49:31Z mvuilleu $
 *
 * Implements yFindAnButton(), the high-level API for AnButton functions
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
#include "yocto_anbutton.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

YAnButton::YAnButton(const string& func): YFunction(func)
//--- (AnButton initialization)
    ,_calibratedValue(CALIBRATEDVALUE_INVALID)
    ,_rawValue(RAWVALUE_INVALID)
    ,_analogCalibration(ANALOGCALIBRATION_INVALID)
    ,_calibrationMax(CALIBRATIONMAX_INVALID)
    ,_calibrationMin(CALIBRATIONMIN_INVALID)
    ,_sensitivity(SENSITIVITY_INVALID)
    ,_isPressed(ISPRESSED_INVALID)
    ,_lastTimePressed(LASTTIMEPRESSED_INVALID)
    ,_lastTimeReleased(LASTTIMERELEASED_INVALID)
    ,_pulseCounter(PULSECOUNTER_INVALID)
    ,_pulseTimer(PULSETIMER_INVALID)
    ,_valueCallbackAnButton(NULL)
//--- (end of AnButton initialization)
{
    _className="AnButton";
}

YAnButton::~YAnButton()
{
//--- (YAnButton cleanup)
//--- (end of YAnButton cleanup)
}
//--- (YAnButton implementation)
// static attributes

int YAnButton::_parseAttr(yJsonStateMachine& j)
{
    if(!strcmp(j.token, "calibratedValue")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _calibratedValue =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "rawValue")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _rawValue =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "analogCalibration")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _analogCalibration =  (Y_ANALOGCALIBRATION_enum)atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "calibrationMax")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _calibrationMax =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "calibrationMin")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _calibrationMin =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "sensitivity")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _sensitivity =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "isPressed")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _isPressed =  (Y_ISPRESSED_enum)atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "lastTimePressed")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _lastTimePressed =  atol(j.token);
        return 1;
    }
    if(!strcmp(j.token, "lastTimeReleased")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _lastTimeReleased =  atol(j.token);
        return 1;
    }
    if(!strcmp(j.token, "pulseCounter")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _pulseCounter =  atol(j.token);
        return 1;
    }
    if(!strcmp(j.token, "pulseTimer")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _pulseTimer =  atol(j.token);
        return 1;
    }
    failed:
    return YFunction::_parseAttr(j);
}


/**
 * Returns the current calibrated input value (between 0 and 1000, included).
 *
 * @return an integer corresponding to the current calibrated input value (between 0 and 1000, included)
 *
 * On failure, throws an exception or returns Y_CALIBRATEDVALUE_INVALID.
 */
int YAnButton::get_calibratedValue(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YAnButton::CALIBRATEDVALUE_INVALID;
        }
    }
    return _calibratedValue;
}

/**
 * Returns the current measured input value as-is (between 0 and 4095, included).
 *
 * @return an integer corresponding to the current measured input value as-is (between 0 and 4095, included)
 *
 * On failure, throws an exception or returns Y_RAWVALUE_INVALID.
 */
int YAnButton::get_rawValue(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YAnButton::RAWVALUE_INVALID;
        }
    }
    return _rawValue;
}

/**
 * Tells if a calibration process is currently ongoing.
 *
 * @return either Y_ANALOGCALIBRATION_OFF or Y_ANALOGCALIBRATION_ON
 *
 * On failure, throws an exception or returns Y_ANALOGCALIBRATION_INVALID.
 */
Y_ANALOGCALIBRATION_enum YAnButton::get_analogCalibration(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YAnButton::ANALOGCALIBRATION_INVALID;
        }
    }
    return _analogCalibration;
}

/**
 * Starts or stops the calibration process. Remember to call the saveToFlash()
 * method of the module at the end of the calibration if the modification must be kept.
 *
 * @param newval : either Y_ANALOGCALIBRATION_OFF or Y_ANALOGCALIBRATION_ON
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YAnButton::set_analogCalibration(Y_ANALOGCALIBRATION_enum newval)
{
    string rest_val;
    rest_val = (newval>0 ? "1" : "0");
    return _setAttr("analogCalibration", rest_val);
}

/**
 * Returns the maximal value measured during the calibration (between 0 and 4095, included).
 *
 * @return an integer corresponding to the maximal value measured during the calibration (between 0
 * and 4095, included)
 *
 * On failure, throws an exception or returns Y_CALIBRATIONMAX_INVALID.
 */
int YAnButton::get_calibrationMax(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YAnButton::CALIBRATIONMAX_INVALID;
        }
    }
    return _calibrationMax;
}

/**
 * Changes the maximal calibration value for the input (between 0 and 4095, included), without actually
 * starting the automated calibration.  Remember to call the saveToFlash()
 * method of the module if the modification must be kept.
 *
 * @param newval : an integer corresponding to the maximal calibration value for the input (between 0
 * and 4095, included), without actually
 *         starting the automated calibration
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YAnButton::set_calibrationMax(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("calibrationMax", rest_val);
}

/**
 * Returns the minimal value measured during the calibration (between 0 and 4095, included).
 *
 * @return an integer corresponding to the minimal value measured during the calibration (between 0
 * and 4095, included)
 *
 * On failure, throws an exception or returns Y_CALIBRATIONMIN_INVALID.
 */
int YAnButton::get_calibrationMin(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YAnButton::CALIBRATIONMIN_INVALID;
        }
    }
    return _calibrationMin;
}

/**
 * Changes the minimal calibration value for the input (between 0 and 4095, included), without actually
 * starting the automated calibration.  Remember to call the saveToFlash()
 * method of the module if the modification must be kept.
 *
 * @param newval : an integer corresponding to the minimal calibration value for the input (between 0
 * and 4095, included), without actually
 *         starting the automated calibration
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YAnButton::set_calibrationMin(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("calibrationMin", rest_val);
}

/**
 * Returns the sensibility for the input (between 1 and 1000) for triggering user callbacks.
 *
 * @return an integer corresponding to the sensibility for the input (between 1 and 1000) for
 * triggering user callbacks
 *
 * On failure, throws an exception or returns Y_SENSITIVITY_INVALID.
 */
int YAnButton::get_sensitivity(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YAnButton::SENSITIVITY_INVALID;
        }
    }
    return _sensitivity;
}

/**
 * Changes the sensibility for the input (between 1 and 1000) for triggering user callbacks.
 * The sensibility is used to filter variations around a fixed value, but does not preclude the
 * transmission of events when the input value evolves constantly in the same direction.
 * Special case: when the value 1000 is used, the callback will only be thrown when the logical state
 * of the input switches from pressed to released and back.
 * Remember to call the saveToFlash() method of the module if the modification must be kept.
 *
 * @param newval : an integer corresponding to the sensibility for the input (between 1 and 1000) for
 * triggering user callbacks
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YAnButton::set_sensitivity(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("sensitivity", rest_val);
}

/**
 * Returns true if the input (considered as binary) is active (closed contact), and false otherwise.
 *
 * @return either Y_ISPRESSED_FALSE or Y_ISPRESSED_TRUE, according to true if the input (considered as
 * binary) is active (closed contact), and false otherwise
 *
 * On failure, throws an exception or returns Y_ISPRESSED_INVALID.
 */
Y_ISPRESSED_enum YAnButton::get_isPressed(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YAnButton::ISPRESSED_INVALID;
        }
    }
    return _isPressed;
}

/**
 * Returns the number of elapsed milliseconds between the module power on and the last time
 * the input button was pressed (the input contact transitioned from open to closed).
 *
 * @return an integer corresponding to the number of elapsed milliseconds between the module power on
 * and the last time
 *         the input button was pressed (the input contact transitioned from open to closed)
 *
 * On failure, throws an exception or returns Y_LASTTIMEPRESSED_INVALID.
 */
s64 YAnButton::get_lastTimePressed(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YAnButton::LASTTIMEPRESSED_INVALID;
        }
    }
    return _lastTimePressed;
}

/**
 * Returns the number of elapsed milliseconds between the module power on and the last time
 * the input button was released (the input contact transitioned from closed to open).
 *
 * @return an integer corresponding to the number of elapsed milliseconds between the module power on
 * and the last time
 *         the input button was released (the input contact transitioned from closed to open)
 *
 * On failure, throws an exception or returns Y_LASTTIMERELEASED_INVALID.
 */
s64 YAnButton::get_lastTimeReleased(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YAnButton::LASTTIMERELEASED_INVALID;
        }
    }
    return _lastTimeReleased;
}

/**
 * Returns the pulse counter value
 *
 * @return an integer corresponding to the pulse counter value
 *
 * On failure, throws an exception or returns Y_PULSECOUNTER_INVALID.
 */
s64 YAnButton::get_pulseCounter(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YAnButton::PULSECOUNTER_INVALID;
        }
    }
    return _pulseCounter;
}

int YAnButton::set_pulseCounter(s64 newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", (u32)newval); rest_val = string(buf);
    return _setAttr("pulseCounter", rest_val);
}

/**
 * Returns the timer of the pulses counter (ms)
 *
 * @return an integer corresponding to the timer of the pulses counter (ms)
 *
 * On failure, throws an exception or returns Y_PULSETIMER_INVALID.
 */
s64 YAnButton::get_pulseTimer(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YAnButton::PULSETIMER_INVALID;
        }
    }
    return _pulseTimer;
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
 * Use the method YAnButton.isOnline() to test if $THEFUNCTION$ is
 * indeed online at a given time. In case of ambiguity when looking for
 * $AFUNCTION$ by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * @param func : a string that uniquely characterizes $THEFUNCTION$
 *
 * @return a YAnButton object allowing you to drive $THEFUNCTION$.
 */
YAnButton* YAnButton::FindAnButton(string func)
{
    YAnButton* obj = NULL;
    obj = (YAnButton*) YFunction::_FindFromCache("AnButton", func);
    if (obj == NULL) {
        obj = new YAnButton(func);
        YFunction::_AddToCache("AnButton", func, obj);
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
int YAnButton::registerValueCallback(YAnButtonValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackAnButton = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YAnButton::_invokeValueCallback(string value)
{
    if (_valueCallbackAnButton != NULL) {
        _valueCallbackAnButton(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

/**
 * Returns the pulse counter value as well as its timer.
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YAnButton::resetCounter(void)
{
    return this->set_pulseCounter(0);
}

YAnButton *YAnButton::nextAnButton(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YAnButton::FindAnButton(hwid);
}

YAnButton* YAnButton::FirstAnButton(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("AnButton", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YAnButton::FindAnButton(serial+"."+funcId);
}

//--- (end of YAnButton implementation)

//--- (AnButton functions)
//--- (end of AnButton functions)
