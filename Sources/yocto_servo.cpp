/*********************************************************************
 *
 * $Id: yocto_servo.cpp 9425 2013-01-11 15:50:01Z seb $
 *
 * Implements yFindServo(), the high-level API for Servo functions
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
#include "yocto_servo.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//--- (YServo implementation)
YMove YSERVO_INVALID_MOVE;

const string YServo::LOGICALNAME_INVALID = "!INVALID!";
const string YServo::ADVERTISEDVALUE_INVALID = "!INVALID!";

std::map<string,YServo*> YServo::_ServoCache;

int YServo::_parse(yJsonStateMachine& j)
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
        } else if(!strcmp(j.token, "position")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _position =  atoi(j.token);
        } else if(!strcmp(j.token, "range")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _range =  atoi(j.token);
        } else if(!strcmp(j.token, "neutral")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _neutral =  atoi(j.token);
        } else if(!strcmp(j.token, "move")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
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
            
        } else {
            // ignore unknown field
            yJsonSkip(&j, 1);
        }
    }
    if(j.st != YJSON_PARSE_STRUCT) goto failed;
    return 0;
}

/**
 * Returns the logical name of the servo.
 * 
 * @return a string corresponding to the logical name of the servo
 * 
 * On failure, throws an exception or returns Y_LOGICALNAME_INVALID.
 */
string YServo::get_logicalName(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_LOGICALNAME_INVALID;
    }
    return _logicalName;
}

/**
 * Changes the logical name of the servo. You can use yCheckLogicalName()
 * prior to this call to make sure that your parameter is valid.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 * 
 * @param newval : a string corresponding to the logical name of the servo
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YServo::set_logicalName(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("logicalName", rest_val);
}

/**
 * Returns the current value of the servo (no more than 6 characters).
 * 
 * @return a string corresponding to the current value of the servo (no more than 6 characters)
 * 
 * On failure, throws an exception or returns Y_ADVERTISEDVALUE_INVALID.
 */
string YServo::get_advertisedValue(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_ADVERTISEDVALUE_INVALID;
    }
    return _advertisedValue;
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
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_POSITION_INVALID;
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
 * Returns the current range of use of the servo.
 * 
 * @return an integer corresponding to the current range of use of the servo
 * 
 * On failure, throws an exception or returns Y_RANGE_INVALID.
 */
int YServo::get_range(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_RANGE_INVALID;
    }
    return _range;
}

/**
 * Changes the range of use of the servo, specified in per cents.
 * A range of 100% corresponds to a standard control signal, that varies
 * from 1 [ms] to 2 [ms], When using a servo that supports a double range,
 * from 0.5 [ms] to 2.5 [ms], you can select a range of 200%.
 * Be aware that using a range higher than what is supported by the servo
 * is likely to damage the servo.
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
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
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
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_NEUTRAL_INVALID;
    }
    return _neutral;
}

/**
 * Changes the duration of the pulse corresponding to the neutral position of the servo.
 * The duration is specified in microseconds, and the standard value is 1500 [us].
 * This setting makes it possible to shift the range of use of the servo.
 * Be aware that using a range higher than what is supported by the servo is
 * likely to damage the servo.
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
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
    return _setAttr("neutral", rest_val);
}

const YMove * YServo::get_move(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_MOVE_INVALID;
    }
    return &_move;
}

int YServo::set_move(const YMove * newval)
{
    string rest_val;
    char buff[64]; sprintf(buff,"%d:%d",newval->target,newval->ms); rest_val = string(buff);
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

YServo *YServo::nextServo(void)
{
    string  hwid;
    
    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return yFindServo(hwid);
}

void YServo::registerValueCallback(YServoUpdateCallback callback)
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

void YServo::advertiseValue(const string& value)
{
    if (_callback != NULL) {
        _callback(this, value);
    }
}


YServo* YServo::FindServo(const string& func)
{
    if(YServo::_ServoCache.find(func) != YServo::_ServoCache.end())
        return YServo::_ServoCache[func];
    
    YServo *newServo = new YServo(func);
    YServo::_ServoCache[func] = newServo;
    
    return newServo;
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
