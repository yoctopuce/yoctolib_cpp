/*********************************************************************
 *
 * $Id: yocto_relay.cpp 22191 2015-12-02 06:49:31Z mvuilleu $
 *
 * Implements yFindRelay(), the high-level API for Relay functions
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
#include "yocto_relay.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

YRelay::YRelay(const string& func): YFunction(func)
//--- (Relay initialization)
    ,_state(STATE_INVALID)
    ,_stateAtPowerOn(STATEATPOWERON_INVALID)
    ,_maxTimeOnStateA(MAXTIMEONSTATEA_INVALID)
    ,_maxTimeOnStateB(MAXTIMEONSTATEB_INVALID)
    ,_output(OUTPUT_INVALID)
    ,_pulseTimer(PULSETIMER_INVALID)
    ,_delayedPulseTimer(DELAYEDPULSETIMER_INVALID)
    ,_countdown(COUNTDOWN_INVALID)
    ,_valueCallbackRelay(NULL)
//--- (end of Relay initialization)
{
    _className="Relay";
}

YRelay::~YRelay()
{
//--- (YRelay cleanup)
//--- (end of YRelay cleanup)
}
//--- (YRelay implementation)
// static attributes
const YDelayedPulse YRelay::DELAYEDPULSETIMER_INVALID = YDelayedPulse();

int YRelay::_parseAttr(yJsonStateMachine& j)
{
    if(!strcmp(j.token, "state")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _state =  (Y_STATE_enum)atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "stateAtPowerOn")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _stateAtPowerOn =  (Y_STATEATPOWERON_enum)atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "maxTimeOnStateA")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _maxTimeOnStateA =  atol(j.token);
        return 1;
    }
    if(!strcmp(j.token, "maxTimeOnStateB")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _maxTimeOnStateB =  atol(j.token);
        return 1;
    }
    if(!strcmp(j.token, "output")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _output =  (Y_OUTPUT_enum)atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "pulseTimer")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _pulseTimer =  atol(j.token);
        return 1;
    }
    if(!strcmp(j.token, "delayedPulseTimer")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
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
        return 1;
    }
    if(!strcmp(j.token, "countdown")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _countdown =  atol(j.token);
        return 1;
    }
    failed:
    return YFunction::_parseAttr(j);
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
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YRelay::STATE_INVALID;
        }
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
 * Returns the state of the relays at device startup (A for the idle position, B for the active
 * position, UNCHANGED for no change).
 *
 * @return a value among Y_STATEATPOWERON_UNCHANGED, Y_STATEATPOWERON_A and Y_STATEATPOWERON_B
 * corresponding to the state of the relays at device startup (A for the idle position, B for the
 * active position, UNCHANGED for no change)
 *
 * On failure, throws an exception or returns Y_STATEATPOWERON_INVALID.
 */
Y_STATEATPOWERON_enum YRelay::get_stateAtPowerOn(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YRelay::STATEATPOWERON_INVALID;
        }
    }
    return _stateAtPowerOn;
}

/**
 * Preset the state of the relays at device startup (A for the idle position,
 * B for the active position, UNCHANGED for no modification). Remember to call the matching module saveToFlash()
 * method, otherwise this call will have no effect.
 *
 * @param newval : a value among Y_STATEATPOWERON_UNCHANGED, Y_STATEATPOWERON_A and Y_STATEATPOWERON_B
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YRelay::set_stateAtPowerOn(Y_STATEATPOWERON_enum newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("stateAtPowerOn", rest_val);
}

/**
 * Retourne the maximum time (ms) allowed for $THEFUNCTIONS$ to stay in state A before automatically
 * switching back in to B state. Zero means no maximum time.
 *
 * @return an integer
 *
 * On failure, throws an exception or returns Y_MAXTIMEONSTATEA_INVALID.
 */
s64 YRelay::get_maxTimeOnStateA(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YRelay::MAXTIMEONSTATEA_INVALID;
        }
    }
    return _maxTimeOnStateA;
}

/**
 * Sets the maximum time (ms) allowed for $THEFUNCTIONS$ to stay in state A before automatically
 * switching back in to B state. Use zero for no maximum time.
 *
 * @param newval : an integer
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YRelay::set_maxTimeOnStateA(s64 newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", (u32)newval); rest_val = string(buf);
    return _setAttr("maxTimeOnStateA", rest_val);
}

/**
 * Retourne the maximum time (ms) allowed for $THEFUNCTIONS$ to stay in state B before automatically
 * switching back in to A state. Zero means no maximum time.
 *
 * @return an integer
 *
 * On failure, throws an exception or returns Y_MAXTIMEONSTATEB_INVALID.
 */
s64 YRelay::get_maxTimeOnStateB(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YRelay::MAXTIMEONSTATEB_INVALID;
        }
    }
    return _maxTimeOnStateB;
}

/**
 * Sets the maximum time (ms) allowed for $THEFUNCTIONS$ to stay in state B before automatically
 * switching back in to A state. Use zero for no maximum time.
 *
 * @param newval : an integer
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YRelay::set_maxTimeOnStateB(s64 newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", (u32)newval); rest_val = string(buf);
    return _setAttr("maxTimeOnStateB", rest_val);
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
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YRelay::OUTPUT_INVALID;
        }
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
s64 YRelay::get_pulseTimer(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YRelay::PULSETIMER_INVALID;
        }
    }
    return _pulseTimer;
}

int YRelay::set_pulseTimer(s64 newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", (u32)newval); rest_val = string(buf);
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
    char buf[32]; sprintf(buf, "%u", (u32)ms_duration); rest_val = string(buf);
    return _setAttr("pulseTimer", rest_val);
}

YDelayedPulse YRelay::get_delayedPulseTimer(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YRelay::DELAYEDPULSETIMER_INVALID;
        }
    }
    return _delayedPulseTimer;
}

int YRelay::set_delayedPulseTimer(YDelayedPulse newval)
{
    string rest_val;
    char buff[64]; sprintf(buff,"%d:%d",newval.target,newval.ms); rest_val = string(buff);
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
s64 YRelay::get_countdown(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YRelay::COUNTDOWN_INVALID;
        }
    }
    return _countdown;
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
 * Use the method YRelay.isOnline() to test if $THEFUNCTION$ is
 * indeed online at a given time. In case of ambiguity when looking for
 * $AFUNCTION$ by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * @param func : a string that uniquely characterizes $THEFUNCTION$
 *
 * @return a YRelay object allowing you to drive $THEFUNCTION$.
 */
YRelay* YRelay::FindRelay(string func)
{
    YRelay* obj = NULL;
    obj = (YRelay*) YFunction::_FindFromCache("Relay", func);
    if (obj == NULL) {
        obj = new YRelay(func);
        YFunction::_AddToCache("Relay", func, obj);
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
int YRelay::registerValueCallback(YRelayValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackRelay = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YRelay::_invokeValueCallback(string value)
{
    if (_valueCallbackRelay != NULL) {
        _valueCallbackRelay(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

YRelay *YRelay::nextRelay(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YRelay::FindRelay(hwid);
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
