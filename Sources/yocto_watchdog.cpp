/*********************************************************************
 *
 * $Id: yocto_watchdog.cpp 22191 2015-12-02 06:49:31Z mvuilleu $
 *
 * Implements yFindWatchdog(), the high-level API for Watchdog functions
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
#include "yocto_watchdog.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

YWatchdog::YWatchdog(const string& func): YFunction(func)
//--- (Watchdog initialization)
    ,_state(STATE_INVALID)
    ,_stateAtPowerOn(STATEATPOWERON_INVALID)
    ,_maxTimeOnStateA(MAXTIMEONSTATEA_INVALID)
    ,_maxTimeOnStateB(MAXTIMEONSTATEB_INVALID)
    ,_output(OUTPUT_INVALID)
    ,_pulseTimer(PULSETIMER_INVALID)
    ,_delayedPulseTimer(DELAYEDPULSETIMER_INVALID)
    ,_countdown(COUNTDOWN_INVALID)
    ,_autoStart(AUTOSTART_INVALID)
    ,_running(RUNNING_INVALID)
    ,_triggerDelay(TRIGGERDELAY_INVALID)
    ,_triggerDuration(TRIGGERDURATION_INVALID)
    ,_valueCallbackWatchdog(NULL)
//--- (end of Watchdog initialization)
{
    _className="Watchdog";
}

YWatchdog::~YWatchdog()
{
//--- (YWatchdog cleanup)
//--- (end of YWatchdog cleanup)
}
//--- (YWatchdog implementation)
// static attributes
const YDelayedPulse YWatchdog::DELAYEDPULSETIMER_INVALID = YDelayedPulse();

int YWatchdog::_parseAttr(yJsonStateMachine& j)
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
    if(!strcmp(j.token, "autoStart")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _autoStart =  (Y_AUTOSTART_enum)atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "running")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _running =  (Y_RUNNING_enum)atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "triggerDelay")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _triggerDelay =  atol(j.token);
        return 1;
    }
    if(!strcmp(j.token, "triggerDuration")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _triggerDuration =  atol(j.token);
        return 1;
    }
    failed:
    return YFunction::_parseAttr(j);
}


/**
 * Returns the state of the watchdog (A for the idle position, B for the active position).
 *
 * @return either Y_STATE_A or Y_STATE_B, according to the state of the watchdog (A for the idle
 * position, B for the active position)
 *
 * On failure, throws an exception or returns Y_STATE_INVALID.
 */
Y_STATE_enum YWatchdog::get_state(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YWatchdog::STATE_INVALID;
        }
    }
    return _state;
}

/**
 * Changes the state of the watchdog (A for the idle position, B for the active position).
 *
 * @param newval : either Y_STATE_A or Y_STATE_B, according to the state of the watchdog (A for the
 * idle position, B for the active position)
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YWatchdog::set_state(Y_STATE_enum newval)
{
    string rest_val;
    rest_val = (newval>0 ? "1" : "0");
    return _setAttr("state", rest_val);
}

/**
 * Returns the state of the watchdog at device startup (A for the idle position, B for the active
 * position, UNCHANGED for no change).
 *
 * @return a value among Y_STATEATPOWERON_UNCHANGED, Y_STATEATPOWERON_A and Y_STATEATPOWERON_B
 * corresponding to the state of the watchdog at device startup (A for the idle position, B for the
 * active position, UNCHANGED for no change)
 *
 * On failure, throws an exception or returns Y_STATEATPOWERON_INVALID.
 */
Y_STATEATPOWERON_enum YWatchdog::get_stateAtPowerOn(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YWatchdog::STATEATPOWERON_INVALID;
        }
    }
    return _stateAtPowerOn;
}

/**
 * Preset the state of the watchdog at device startup (A for the idle position,
 * B for the active position, UNCHANGED for no modification). Remember to call the matching module saveToFlash()
 * method, otherwise this call will have no effect.
 *
 * @param newval : a value among Y_STATEATPOWERON_UNCHANGED, Y_STATEATPOWERON_A and Y_STATEATPOWERON_B
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YWatchdog::set_stateAtPowerOn(Y_STATEATPOWERON_enum newval)
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
s64 YWatchdog::get_maxTimeOnStateA(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YWatchdog::MAXTIMEONSTATEA_INVALID;
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
int YWatchdog::set_maxTimeOnStateA(s64 newval)
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
s64 YWatchdog::get_maxTimeOnStateB(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YWatchdog::MAXTIMEONSTATEB_INVALID;
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
int YWatchdog::set_maxTimeOnStateB(s64 newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", (u32)newval); rest_val = string(buf);
    return _setAttr("maxTimeOnStateB", rest_val);
}

/**
 * Returns the output state of the watchdog, when used as a simple switch (single throw).
 *
 * @return either Y_OUTPUT_OFF or Y_OUTPUT_ON, according to the output state of the watchdog, when
 * used as a simple switch (single throw)
 *
 * On failure, throws an exception or returns Y_OUTPUT_INVALID.
 */
Y_OUTPUT_enum YWatchdog::get_output(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YWatchdog::OUTPUT_INVALID;
        }
    }
    return _output;
}

/**
 * Changes the output state of the watchdog, when used as a simple switch (single throw).
 *
 * @param newval : either Y_OUTPUT_OFF or Y_OUTPUT_ON, according to the output state of the watchdog,
 * when used as a simple switch (single throw)
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YWatchdog::set_output(Y_OUTPUT_enum newval)
{
    string rest_val;
    rest_val = (newval>0 ? "1" : "0");
    return _setAttr("output", rest_val);
}

/**
 * Returns the number of milliseconds remaining before the watchdog is returned to idle position
 * (state A), during a measured pulse generation. When there is no ongoing pulse, returns zero.
 *
 * @return an integer corresponding to the number of milliseconds remaining before the watchdog is
 * returned to idle position
 *         (state A), during a measured pulse generation
 *
 * On failure, throws an exception or returns Y_PULSETIMER_INVALID.
 */
s64 YWatchdog::get_pulseTimer(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YWatchdog::PULSETIMER_INVALID;
        }
    }
    return _pulseTimer;
}

int YWatchdog::set_pulseTimer(s64 newval)
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
int YWatchdog::pulse(int ms_duration)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", (u32)ms_duration); rest_val = string(buf);
    return _setAttr("pulseTimer", rest_val);
}

YDelayedPulse YWatchdog::get_delayedPulseTimer(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YWatchdog::DELAYEDPULSETIMER_INVALID;
        }
    }
    return _delayedPulseTimer;
}

int YWatchdog::set_delayedPulseTimer(YDelayedPulse newval)
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
int YWatchdog::delayedPulse(int ms_delay,int ms_duration)
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
s64 YWatchdog::get_countdown(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YWatchdog::COUNTDOWN_INVALID;
        }
    }
    return _countdown;
}

/**
 * Returns the watchdog runing state at module power on.
 *
 * @return either Y_AUTOSTART_OFF or Y_AUTOSTART_ON, according to the watchdog runing state at module power on
 *
 * On failure, throws an exception or returns Y_AUTOSTART_INVALID.
 */
Y_AUTOSTART_enum YWatchdog::get_autoStart(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YWatchdog::AUTOSTART_INVALID;
        }
    }
    return _autoStart;
}

/**
 * Changes the watchdog runningsttae at module power on. Remember to call the
 * saveToFlash() method and then to reboot the module to apply this setting.
 *
 * @param newval : either Y_AUTOSTART_OFF or Y_AUTOSTART_ON, according to the watchdog runningsttae at
 * module power on
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YWatchdog::set_autoStart(Y_AUTOSTART_enum newval)
{
    string rest_val;
    rest_val = (newval>0 ? "1" : "0");
    return _setAttr("autoStart", rest_val);
}

/**
 * Returns the watchdog running state.
 *
 * @return either Y_RUNNING_OFF or Y_RUNNING_ON, according to the watchdog running state
 *
 * On failure, throws an exception or returns Y_RUNNING_INVALID.
 */
Y_RUNNING_enum YWatchdog::get_running(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YWatchdog::RUNNING_INVALID;
        }
    }
    return _running;
}

/**
 * Changes the running state of the watchdog.
 *
 * @param newval : either Y_RUNNING_OFF or Y_RUNNING_ON, according to the running state of the watchdog
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YWatchdog::set_running(Y_RUNNING_enum newval)
{
    string rest_val;
    rest_val = (newval>0 ? "1" : "0");
    return _setAttr("running", rest_val);
}

/**
 * Resets the watchdog. When the watchdog is running, this function
 * must be called on a regular basis to prevent the watchog to
 * trigger
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YWatchdog::resetWatchdog(void)
{
    string rest_val;
    rest_val = "1";
    return _setAttr("running", rest_val);
}

/**
 * Returns  the waiting duration before a reset is automatically triggered by the watchdog, in milliseconds.
 *
 * @return an integer corresponding to  the waiting duration before a reset is automatically triggered
 * by the watchdog, in milliseconds
 *
 * On failure, throws an exception or returns Y_TRIGGERDELAY_INVALID.
 */
s64 YWatchdog::get_triggerDelay(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YWatchdog::TRIGGERDELAY_INVALID;
        }
    }
    return _triggerDelay;
}

/**
 * Changes the waiting delay before a reset is triggered by the watchdog, in milliseconds.
 *
 * @param newval : an integer corresponding to the waiting delay before a reset is triggered by the
 * watchdog, in milliseconds
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YWatchdog::set_triggerDelay(s64 newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", (u32)newval); rest_val = string(buf);
    return _setAttr("triggerDelay", rest_val);
}

/**
 * Returns the duration of resets caused by the watchdog, in milliseconds.
 *
 * @return an integer corresponding to the duration of resets caused by the watchdog, in milliseconds
 *
 * On failure, throws an exception or returns Y_TRIGGERDURATION_INVALID.
 */
s64 YWatchdog::get_triggerDuration(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YWatchdog::TRIGGERDURATION_INVALID;
        }
    }
    return _triggerDuration;
}

/**
 * Changes the duration of resets caused by the watchdog, in milliseconds.
 *
 * @param newval : an integer corresponding to the duration of resets caused by the watchdog, in milliseconds
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YWatchdog::set_triggerDuration(s64 newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", (u32)newval); rest_val = string(buf);
    return _setAttr("triggerDuration", rest_val);
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
 * Use the method YWatchdog.isOnline() to test if $THEFUNCTION$ is
 * indeed online at a given time. In case of ambiguity when looking for
 * $AFUNCTION$ by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * @param func : a string that uniquely characterizes $THEFUNCTION$
 *
 * @return a YWatchdog object allowing you to drive $THEFUNCTION$.
 */
YWatchdog* YWatchdog::FindWatchdog(string func)
{
    YWatchdog* obj = NULL;
    obj = (YWatchdog*) YFunction::_FindFromCache("Watchdog", func);
    if (obj == NULL) {
        obj = new YWatchdog(func);
        YFunction::_AddToCache("Watchdog", func, obj);
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
int YWatchdog::registerValueCallback(YWatchdogValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackWatchdog = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YWatchdog::_invokeValueCallback(string value)
{
    if (_valueCallbackWatchdog != NULL) {
        _valueCallbackWatchdog(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

YWatchdog *YWatchdog::nextWatchdog(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YWatchdog::FindWatchdog(hwid);
}

YWatchdog* YWatchdog::FirstWatchdog(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("Watchdog", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YWatchdog::FindWatchdog(serial+"."+funcId);
}

//--- (end of YWatchdog implementation)

//--- (Watchdog functions)
//--- (end of Watchdog functions)
