/*********************************************************************
 *
 * $Id: yocto_anbutton.cpp 9425 2013-01-11 15:50:01Z seb $
 *
 * Implements yFindAnButton(), the high-level API for AnButton functions
 *
 * - - - - - - - - - License information: - - - - - - - - - 
 *
 * Copyright (C) 2011 and beyond by Yoctopuce Sarl, Switzerland.
 *
 * 1) If you have obtained this file from www.yoctopuce.com,
 *    Yoctopuce Sarl licenses to you (hereafter Licensee) the
 *    right to use, modify, copy, and integrate this source file
 *    into your own solution for the sole purpose of interfacing
 *    a Yoctopuce product with Licensee's solution.
 *
 *    The use of this file and all relationship between Yoctopuce 
 *    and Licensee are governed by Yoctopuce General Terms and 
 *    Conditions.
 *
 *    THE SOFTWARE AND DOCUMENTATION ARE PROVIDED 'AS IS' WITHOUT
 *    WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING 
 *    WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS 
 *    FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO
 *    EVENT SHALL LICENSOR BE LIABLE FOR ANY INCIDENTAL, SPECIAL,
 *    INDIRECT OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, 
 *    COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR 
 *    SERVICES, ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT 
 *    LIMITED TO ANY DEFENSE THEREOF), ANY CLAIMS FOR INDEMNITY OR
 *    CONTRIBUTION, OR OTHER SIMILAR COSTS, WHETHER ASSERTED ON THE
 *    BASIS OF CONTRACT, TORT (INCLUDING NEGLIGENCE), BREACH OF
 *    WARRANTY, OR OTHERWISE.
 *
 * 2) If your intent is not to interface with Yoctopuce products,
 *    you are not entitled to use, read or create any derived
 *    material from this source file.
 *
 *********************************************************************/


#define _CRT_SECURE_NO_DEPRECATE //do not use windows secure crt
#include "yocto_anbutton.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//--- (YAnButton implementation)

const string YAnButton::LOGICALNAME_INVALID = "!INVALID!";
const string YAnButton::ADVERTISEDVALUE_INVALID = "!INVALID!";

std::map<string,YAnButton*> YAnButton::_AnButtonCache;

int YAnButton::_parse(yJsonStateMachine& j)
{
    if(yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_STRUCT) {
    failed:
        return -1;
    }
    while(yJsonParse(&j) == YJSON_PARSE_AVAIL && j.st == YJSON_PARSE_MEMBNAME) {
        if(!strcmp(j.token, "logicalName")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _logicalName =  _parseString(j);
        } else if(!strcmp(j.token, "advertisedValue")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _advertisedValue =  _parseString(j);
        } else if(!strcmp(j.token, "calibratedValue")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _calibratedValue =  atoi(j.token);
        } else if(!strcmp(j.token, "rawValue")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _rawValue =  atoi(j.token);
        } else if(!strcmp(j.token, "analogCalibration")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _analogCalibration =  (Y_ANALOGCALIBRATION_enum)atoi(j.token);
        } else if(!strcmp(j.token, "calibrationMax")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _calibrationMax =  atoi(j.token);
        } else if(!strcmp(j.token, "calibrationMin")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _calibrationMin =  atoi(j.token);
        } else if(!strcmp(j.token, "sensitivity")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _sensitivity =  atoi(j.token);
        } else if(!strcmp(j.token, "isPressed")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _isPressed =  (Y_ISPRESSED_enum)atoi(j.token);
        } else if(!strcmp(j.token, "lastTimePressed")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _lastTimePressed =  atoi(j.token);
        } else if(!strcmp(j.token, "lastTimeReleased")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _lastTimeReleased =  atoi(j.token);
        } else if(!strcmp(j.token, "pulseCounter")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _pulseCounter =  atoi(j.token);
        } else if(!strcmp(j.token, "pulseTimer")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _pulseTimer =  atoi(j.token);
        } else {
            // ignore unknown field
            yJsonSkip(&j, 1);
        }
    }
    if(j.st != YJSON_PARSE_STRUCT) goto failed;
    return 0;
}

/**
 * Returns the logical name of the analog input.
 * 
 * @return a string corresponding to the logical name of the analog input
 * 
 * On failure, throws an exception or returns Y_LOGICALNAME_INVALID.
 */
string YAnButton::get_logicalName(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_LOGICALNAME_INVALID;
    }
    return _logicalName;
}

/**
 * Changes the logical name of the analog input. You can use yCheckLogicalName()
 * prior to this call to make sure that your parameter is valid.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 * 
 * @param newval : a string corresponding to the logical name of the analog input
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YAnButton::set_logicalName(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("logicalName", rest_val);
}

/**
 * Returns the current value of the analog input (no more than 6 characters).
 * 
 * @return a string corresponding to the current value of the analog input (no more than 6 characters)
 * 
 * On failure, throws an exception or returns Y_ADVERTISEDVALUE_INVALID.
 */
string YAnButton::get_advertisedValue(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_ADVERTISEDVALUE_INVALID;
    }
    return _advertisedValue;
}

/**
 * Returns the current calibrated input value (between 0 and 1000, included).
 * 
 * @return an integer corresponding to the current calibrated input value (between 0 and 1000, included)
 * 
 * On failure, throws an exception or returns Y_CALIBRATEDVALUE_INVALID.
 */
unsigned YAnButton::get_calibratedValue(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_CALIBRATEDVALUE_INVALID;
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
unsigned YAnButton::get_rawValue(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_RAWVALUE_INVALID;
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
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_ANALOGCALIBRATION_INVALID;
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
unsigned YAnButton::get_calibrationMax(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_CALIBRATIONMAX_INVALID;
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
int YAnButton::set_calibrationMax(unsigned newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
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
unsigned YAnButton::get_calibrationMin(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_CALIBRATIONMIN_INVALID;
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
int YAnButton::set_calibrationMin(unsigned newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
    return _setAttr("calibrationMin", rest_val);
}

/**
 * Returns the sensibility for the input (between 1 and 255, included) for triggering user callbacks.
 * 
 * @return an integer corresponding to the sensibility for the input (between 1 and 255, included) for
 * triggering user callbacks
 * 
 * On failure, throws an exception or returns Y_SENSITIVITY_INVALID.
 */
unsigned YAnButton::get_sensitivity(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_SENSITIVITY_INVALID;
    }
    return _sensitivity;
}

/**
 * Changes the sensibility for the input (between 1 and 255, included) for triggering user callbacks.
 * Remember to call the saveToFlash() method of the module if the modification must be kept.
 * 
 * @param newval : an integer corresponding to the sensibility for the input (between 1 and 255,
 * included) for triggering user callbacks
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YAnButton::set_sensitivity(unsigned newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
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
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_ISPRESSED_INVALID;
    }
    return _isPressed;
}

/**
 * Returns the number of elapsed milliseconds between the module power on and the last time
 * the input button was pressed (the input contact transitionned from open to closed).
 * 
 * @return an integer corresponding to the number of elapsed milliseconds between the module power on
 * and the last time
 *         the input button was pressed (the input contact transitionned from open to closed)
 * 
 * On failure, throws an exception or returns Y_LASTTIMEPRESSED_INVALID.
 */
unsigned YAnButton::get_lastTimePressed(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_LASTTIMEPRESSED_INVALID;
    }
    return _lastTimePressed;
}

/**
 * Returns the number of elapsed milliseconds between the module power on and the last time
 * the input button was released (the input contact transitionned from closed to open).
 * 
 * @return an integer corresponding to the number of elapsed milliseconds between the module power on
 * and the last time
 *         the input button was released (the input contact transitionned from closed to open)
 * 
 * On failure, throws an exception or returns Y_LASTTIMERELEASED_INVALID.
 */
unsigned YAnButton::get_lastTimeReleased(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_LASTTIMERELEASED_INVALID;
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
unsigned YAnButton::get_pulseCounter(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_PULSECOUNTER_INVALID;
    }
    return _pulseCounter;
}

int YAnButton::set_pulseCounter(unsigned newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
    return _setAttr("pulseCounter", rest_val);
}

/**
 * Returns the pulse counter value as well as his timer
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YAnButton::resetCounter(void)
{
    string rest_val;
    rest_val = "0";
    return _setAttr("pulseCounter", rest_val);
}

/**
 * Returns the timer of the pulses counter (ms)
 * 
 * @return an integer corresponding to the timer of the pulses counter (ms)
 * 
 * On failure, throws an exception or returns Y_PULSETIMER_INVALID.
 */
unsigned YAnButton::get_pulseTimer(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_PULSETIMER_INVALID;
    }
    return _pulseTimer;
}

YAnButton *YAnButton::nextAnButton(void)
{
    string  hwid;
    
    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return yFindAnButton(hwid);
}

void YAnButton::registerValueCallback(YAnButtonUpdateCallback callback)
{
    if (callback != NULL) {
        _registerFuncCallback(this);
        yapiLockFunctionCallBack(NULL);
        YAPI::_yapiFunctionUpdateCallbackFwd(this->functionDescriptor(), this->get_advertisedValue().c_str());
        yapiUnlockFunctionCallBack(NULL);
    } else {
        _unregisterFuncCallback(this);
    }
    _callback = callback;
}

void YAnButton::advertiseValue(const string& value)
{
    if (_callback != NULL) {
        _callback(this, value);
    }
}


YAnButton* YAnButton::FindAnButton(const string& func)
{
    if(YAnButton::_AnButtonCache.find(func) != YAnButton::_AnButtonCache.end())
        return YAnButton::_AnButtonCache[func];
    
    YAnButton *newAnButton = new YAnButton(func);
    YAnButton::_AnButtonCache[func] = newAnButton;
    
    return newAnButton;
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
