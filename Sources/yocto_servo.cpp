/*********************************************************************
 *
 * $Id: yocto_servo.cpp 22191 2015-12-02 06:49:31Z mvuilleu $
 *
 * Implements yFindServo(), the high-level API for Servo functions
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
#include "yocto_servo.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

YServo::YServo(const string& func): YFunction(func)
//--- (Servo initialization)
    ,_position(POSITION_INVALID)
    ,_enabled(ENABLED_INVALID)
    ,_range(RANGE_INVALID)
    ,_neutral(NEUTRAL_INVALID)
    ,_move(MOVE_INVALID)
    ,_positionAtPowerOn(POSITIONATPOWERON_INVALID)
    ,_enabledAtPowerOn(ENABLEDATPOWERON_INVALID)
    ,_valueCallbackServo(NULL)
//--- (end of Servo initialization)
{
    _className="Servo";
}

YServo::~YServo()
{
//--- (YServo cleanup)
//--- (end of YServo cleanup)
}
//--- (YServo implementation)
// static attributes
const YMove YServo::MOVE_INVALID = YMove();

int YServo::_parseAttr(yJsonStateMachine& j)
{
    if(!strcmp(j.token, "position")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _position =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "enabled")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _enabled =  (Y_ENABLED_enum)atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "range")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _range =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "neutral")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _neutral =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "move")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        if(j.st != YJSON_PARSE_STRUCT) goto failed;
        while(yJsonParse(&j) == YJSON_PARSE_AVAIL && j.st == YJSON_PARSE_MEMBNAME) {
            if(!strcmp(j.token, "moving")) {
                if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
                _move.moving = atoi(j.token);
            } else if(!strcmp(j.token, "target")) {
                if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
                _move.target = atoi(j.token);
            } else if(!strcmp(j.token, "ms")) {
                if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
                _move.ms = atoi(j.token);
            }
        }
        if(j.st != YJSON_PARSE_STRUCT) goto failed;
        return 1;
    }
    if(!strcmp(j.token, "positionAtPowerOn")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _positionAtPowerOn =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "enabledAtPowerOn")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _enabledAtPowerOn =  (Y_ENABLEDATPOWERON_enum)atoi(j.token);
        return 1;
    }
    failed:
    return YFunction::_parseAttr(j);
}


/**
 * Returns the current servo position.
 *
 * @return an integer corresponding to the current servo position
 *
 * On failure, throws an exception or returns Y_POSITION_INVALID.
 */
int YServo::get_position(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YServo::POSITION_INVALID;
        }
    }
    return _position;
}

/**
 * Changes immediately the servo driving position.
 *
 * @param newval : an integer corresponding to immediately the servo driving position
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YServo::set_position(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("position", rest_val);
}

/**
 * Returns the state of the servos.
 *
 * @return either Y_ENABLED_FALSE or Y_ENABLED_TRUE, according to the state of the servos
 *
 * On failure, throws an exception or returns Y_ENABLED_INVALID.
 */
Y_ENABLED_enum YServo::get_enabled(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YServo::ENABLED_INVALID;
        }
    }
    return _enabled;
}

/**
 * Stops or starts the servo.
 *
 * @param newval : either Y_ENABLED_FALSE or Y_ENABLED_TRUE
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YServo::set_enabled(Y_ENABLED_enum newval)
{
    string rest_val;
    rest_val = (newval>0 ? "1" : "0");
    return _setAttr("enabled", rest_val);
}

/**
 * Returns the current range of use of the servo.
 *
 * @return an integer corresponding to the current range of use of the servo
 *
 * On failure, throws an exception or returns Y_RANGE_INVALID.
 */
int YServo::get_range(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YServo::RANGE_INVALID;
        }
    }
    return _range;
}

/**
 * Changes the range of use of the servo, specified in per cents.
 * A range of 100% corresponds to a standard control signal, that varies
 * from 1 [ms] to 2 [ms], When using a servo that supports a double range,
 * from 0.5 [ms] to 2.5 [ms], you can select a range of 200%.
 * Be aware that using a range higher than what is supported by the servo
 * is likely to damage the servo. Remember to call the matching module
 * saveToFlash() method, otherwise this call will have no effect.
 *
 * @param newval : an integer corresponding to the range of use of the servo, specified in per cents
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YServo::set_range(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("range", rest_val);
}

/**
 * Returns the duration in microseconds of a neutral pulse for the servo.
 *
 * @return an integer corresponding to the duration in microseconds of a neutral pulse for the servo
 *
 * On failure, throws an exception or returns Y_NEUTRAL_INVALID.
 */
int YServo::get_neutral(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YServo::NEUTRAL_INVALID;
        }
    }
    return _neutral;
}

/**
 * Changes the duration of the pulse corresponding to the neutral position of the servo.
 * The duration is specified in microseconds, and the standard value is 1500 [us].
 * This setting makes it possible to shift the range of use of the servo.
 * Be aware that using a range higher than what is supported by the servo is
 * likely to damage the servo. Remember to call the matching module
 * saveToFlash() method, otherwise this call will have no effect.
 *
 * @param newval : an integer corresponding to the duration of the pulse corresponding to the neutral
 * position of the servo
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YServo::set_neutral(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("neutral", rest_val);
}

YMove YServo::get_move(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YServo::MOVE_INVALID;
        }
    }
    return _move;
}

int YServo::set_move(YMove newval)
{
    string rest_val;
    char buff[64]; sprintf(buff,"%d:%d",newval.target,newval.ms); rest_val = string(buff);
    return _setAttr("move", rest_val);
}

/**
 * Performs a smooth move at constant speed toward a given position.
 *
 * @param target      : new position at the end of the move
 * @param ms_duration : total duration of the move, in milliseconds
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YServo::move(int target,int ms_duration)
{
    string rest_val;
    char buff[64]; sprintf(buff,"%d:%d",target,ms_duration); rest_val = string(buff);
    return _setAttr("move", rest_val);
}

/**
 * Returns the servo position at device power up.
 *
 * @return an integer corresponding to the servo position at device power up
 *
 * On failure, throws an exception or returns Y_POSITIONATPOWERON_INVALID.
 */
int YServo::get_positionAtPowerOn(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YServo::POSITIONATPOWERON_INVALID;
        }
    }
    return _positionAtPowerOn;
}

/**
 * Configure the servo position at device power up. Remember to call the matching
 * module saveToFlash() method, otherwise this call will have no effect.
 *
 * @param newval : an integer
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YServo::set_positionAtPowerOn(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("positionAtPowerOn", rest_val);
}

/**
 * Returns the servo signal generator state at power up.
 *
 * @return either Y_ENABLEDATPOWERON_FALSE or Y_ENABLEDATPOWERON_TRUE, according to the servo signal
 * generator state at power up
 *
 * On failure, throws an exception or returns Y_ENABLEDATPOWERON_INVALID.
 */
Y_ENABLEDATPOWERON_enum YServo::get_enabledAtPowerOn(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YServo::ENABLEDATPOWERON_INVALID;
        }
    }
    return _enabledAtPowerOn;
}

/**
 * Configure the servo signal generator state at power up. Remember to call the matching module saveToFlash()
 * method, otherwise this call will have no effect.
 *
 * @param newval : either Y_ENABLEDATPOWERON_FALSE or Y_ENABLEDATPOWERON_TRUE
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YServo::set_enabledAtPowerOn(Y_ENABLEDATPOWERON_enum newval)
{
    string rest_val;
    rest_val = (newval>0 ? "1" : "0");
    return _setAttr("enabledAtPowerOn", rest_val);
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
 * Use the method YServo.isOnline() to test if $THEFUNCTION$ is
 * indeed online at a given time. In case of ambiguity when looking for
 * $AFUNCTION$ by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * @param func : a string that uniquely characterizes $THEFUNCTION$
 *
 * @return a YServo object allowing you to drive $THEFUNCTION$.
 */
YServo* YServo::FindServo(string func)
{
    YServo* obj = NULL;
    obj = (YServo*) YFunction::_FindFromCache("Servo", func);
    if (obj == NULL) {
        obj = new YServo(func);
        YFunction::_AddToCache("Servo", func, obj);
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
int YServo::registerValueCallback(YServoValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackServo = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YServo::_invokeValueCallback(string value)
{
    if (_valueCallbackServo != NULL) {
        _valueCallbackServo(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

YServo *YServo::nextServo(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YServo::FindServo(hwid);
}

YServo* YServo::FirstServo(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("Servo", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YServo::FindServo(serial+"."+funcId);
}

//--- (end of YServo implementation)

//--- (Servo functions)
//--- (end of Servo functions)
