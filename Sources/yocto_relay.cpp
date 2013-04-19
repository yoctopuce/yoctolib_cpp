/*********************************************************************
 *
 * $Id: yocto_relay.cpp 9425 2013-01-11 15:50:01Z seb $
 *
 * Implements yFindRelay(), the high-level API for Relay functions
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
#include "yocto_relay.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//--- (YRelay implementation)
YDelayedPulse YRELAY_INVALID_DELAYEDPULSE;

const string YRelay::LOGICALNAME_INVALID = "!INVALID!";
const string YRelay::ADVERTISEDVALUE_INVALID = "!INVALID!";

std::map<string,YRelay*> YRelay::_RelayCache;

int YRelay::_parse(yJsonStateMachine& j)
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
        } else if(!strcmp(j.token, "state")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _state =  (Y_STATE_enum)atoi(j.token);
        } else if(!strcmp(j.token, "output")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _output =  (Y_OUTPUT_enum)atoi(j.token);
        } else if(!strcmp(j.token, "pulseTimer")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _pulseTimer =  atoi(j.token);
        } else if(!strcmp(j.token, "delayedPulseTimer")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            if(j.st != YJSON_PARSE_STRUCT) goto failed;
            while(yJsonParse(&j) == YJSON_PARSE_AVAIL && j.st == YJSON_PARSE_MEMBNAME) {
                if(!strcmp(j.token, "moving")) {
                    if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
                    _delayedPulseTimer.moving = atoi(j.token);
                } else if(!strcmp(j.token, "target")) {
                    if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
                    _delayedPulseTimer.target = atoi(j.token);
                } else if(!strcmp(j.token, "ms")) {
                    if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
                    _delayedPulseTimer.ms = atoi(j.token);
                }
            }
            if(j.st != YJSON_PARSE_STRUCT) goto failed; 
            
        } else if(!strcmp(j.token, "countdown")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _countdown =  atoi(j.token);
        } else {
            // ignore unknown field
            yJsonSkip(&j, 1);
        }
    }
    if(j.st != YJSON_PARSE_STRUCT) goto failed;
    return 0;
}

/**
 * Returns the logical name of the relay.
 * 
 * @return a string corresponding to the logical name of the relay
 * 
 * On failure, throws an exception or returns Y_LOGICALNAME_INVALID.
 */
string YRelay::get_logicalName(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_LOGICALNAME_INVALID;
    }
    return _logicalName;
}

/**
 * Changes the logical name of the relay. You can use yCheckLogicalName()
 * prior to this call to make sure that your parameter is valid.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 * 
 * @param newval : a string corresponding to the logical name of the relay
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YRelay::set_logicalName(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("logicalName", rest_val);
}

/**
 * Returns the current value of the relay (no more than 6 characters).
 * 
 * @return a string corresponding to the current value of the relay (no more than 6 characters)
 * 
 * On failure, throws an exception or returns Y_ADVERTISEDVALUE_INVALID.
 */
string YRelay::get_advertisedValue(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_ADVERTISEDVALUE_INVALID;
    }
    return _advertisedValue;
}

/**
 * Returns the state of the relays (A for the idle position, B for the active position).
 * 
 * @return either Y_STATE_A or Y_STATE_B, according to the state of the relays (A for the idle
 * position, B for the active position)
 * 
 * On failure, throws an exception or returns Y_STATE_INVALID.
 */
Y_STATE_enum YRelay::get_state(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_STATE_INVALID;
    }
    return _state;
}

/**
 * Changes the state of the relays (A for the idle position, B for the active position).
 * 
 * @param newval : either Y_STATE_A or Y_STATE_B, according to the state of the relays (A for the idle
 * position, B for the active position)
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YRelay::set_state(Y_STATE_enum newval)
{
    string rest_val;
    rest_val = (newval>0 ? "1" : "0");
    return _setAttr("state", rest_val);
}

/**
 * Returns the output state of the relays, when used as a simple switch (single throw).
 * 
 * @return either Y_OUTPUT_OFF or Y_OUTPUT_ON, according to the output state of the relays, when used
 * as a simple switch (single throw)
 * 
 * On failure, throws an exception or returns Y_OUTPUT_INVALID.
 */
Y_OUTPUT_enum YRelay::get_output(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_OUTPUT_INVALID;
    }
    return _output;
}

/**
 * Changes the output state of the relays, when used as a simple switch (single throw).
 * 
 * @param newval : either Y_OUTPUT_OFF or Y_OUTPUT_ON, according to the output state of the relays,
 * when used as a simple switch (single throw)
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YRelay::set_output(Y_OUTPUT_enum newval)
{
    string rest_val;
    rest_val = (newval>0 ? "1" : "0");
    return _setAttr("output", rest_val);
}

/**
 * Returns the number of milliseconds remaining before the relays is returned to idle position
 * (state A), during a measured pulse generation. When there is no ongoing pulse, returns zero.
 * 
 * @return an integer corresponding to the number of milliseconds remaining before the relays is
 * returned to idle position
 *         (state A), during a measured pulse generation
 * 
 * On failure, throws an exception or returns Y_PULSETIMER_INVALID.
 */
unsigned YRelay::get_pulseTimer(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_PULSETIMER_INVALID;
    }
    return _pulseTimer;
}

int YRelay::set_pulseTimer(unsigned newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
    return _setAttr("pulseTimer", rest_val);
}

/**
 * Sets the relay to output B (active) for a specified duration, then brings it
 * automatically back to output A (idle state).
 * 
 * @param ms_duration : pulse duration, in millisecondes
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YRelay::pulse(int ms_duration)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", ms_duration); rest_val = string(buf);
    return _setAttr("pulseTimer", rest_val);
}

const YDelayedPulse * YRelay::get_delayedPulseTimer(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_DELAYEDPULSETIMER_INVALID;
    }
    return &_delayedPulseTimer;
}

int YRelay::set_delayedPulseTimer(const YDelayedPulse * newval)
{
    string rest_val;
    char buff[64]; sprintf(buff,"%d:%d",newval->target,newval->ms); rest_val = string(buff);
    return _setAttr("delayedPulseTimer", rest_val);
}

/**
 * Schedules a pulse.
 * 
 * @param ms_delay : waiting time before the pulse, in millisecondes
 * @param ms_duration : pulse duration, in millisecondes
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YRelay::delayedPulse(int ms_delay,int ms_duration)
{
    string rest_val;
    char buff[64]; sprintf(buff,"%d:%d",ms_delay,ms_duration); rest_val = string(buff);
    return _setAttr("delayedPulseTimer", rest_val);
}

/**
 * Returns the number of milliseconds remaining before a pulse (delayedPulse() call)
 * When there is no scheduled pulse, returns zero.
 * 
 * @return an integer corresponding to the number of milliseconds remaining before a pulse (delayedPulse() call)
 *         When there is no scheduled pulse, returns zero
 * 
 * On failure, throws an exception or returns Y_COUNTDOWN_INVALID.
 */
unsigned YRelay::get_countdown(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_COUNTDOWN_INVALID;
    }
    return _countdown;
}

YRelay *YRelay::nextRelay(void)
{
    string  hwid;
    
    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return yFindRelay(hwid);
}

void YRelay::registerValueCallback(YRelayUpdateCallback callback)
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

void YRelay::advertiseValue(const string& value)
{
    if (_callback != NULL) {
        _callback(this, value);
    }
}


YRelay* YRelay::FindRelay(const string& func)
{
    if(YRelay::_RelayCache.find(func) != YRelay::_RelayCache.end())
        return YRelay::_RelayCache[func];
    
    YRelay *newRelay = new YRelay(func);
    YRelay::_RelayCache[func] = newRelay;
    
    return newRelay;
}

YRelay* YRelay::FirstRelay(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;
    
    if(YISERR(YapiWrapper::getFunctionsByClass("Relay", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YRelay::FindRelay(serial+"."+funcId);
}

//--- (end of YRelay implementation)

//--- (Relay functions)
//--- (end of Relay functions)
