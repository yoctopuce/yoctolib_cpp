/*********************************************************************
 *
 *  $Id: yocto_watchdog.cpp 52570 2022-12-26 09:27:54Z seb $
 *
 *  Implements yFindWatchdog(), the high-level API for Watchdog functions
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

#include "yocto_watchdog.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#define  __FILE_ID__  "watchdog"

#ifdef YOCTOLIB_NAMESPACE
using namespace YOCTOLIB_NAMESPACE;
#endif

YWatchdog::YWatchdog(const string& func): YFunction(func)
//--- (YWatchdog initialization)
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
    ,_lastTrigger(LASTTRIGGER_INVALID)
    ,_valueCallbackWatchdog(NULL)
    ,_firm(0)
//--- (end of YWatchdog initialization)
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

int YWatchdog::_parseAttr(YJSONObject *json_val)
{
    if(json_val->has("state")) {
        _state =  (Y_STATE_enum)json_val->getInt("state");
    }
    if(json_val->has("stateAtPowerOn")) {
        _stateAtPowerOn =  (Y_STATEATPOWERON_enum)json_val->getInt("stateAtPowerOn");
    }
    if(json_val->has("maxTimeOnStateA")) {
        _maxTimeOnStateA =  json_val->getLong("maxTimeOnStateA");
    }
    if(json_val->has("maxTimeOnStateB")) {
        _maxTimeOnStateB =  json_val->getLong("maxTimeOnStateB");
    }
    if(json_val->has("output")) {
        _output =  (Y_OUTPUT_enum)json_val->getInt("output");
    }
    if(json_val->has("pulseTimer")) {
        _pulseTimer =  json_val->getLong("pulseTimer");
    }
    if(json_val->has("delayedPulseTimer")) {
        YJSONObject* subjson = json_val->getYJSONObject("delayedPulseTimer");
        if (subjson->has("moving")) {
            _delayedPulseTimer.moving = subjson->getInt("moving");
        }
        if (subjson->has("target")) {
            _delayedPulseTimer.target = subjson->getInt("target");
        }
        if (subjson->has("ms")) {
            _delayedPulseTimer.ms = subjson->getInt("ms");
        }
    }
    if(json_val->has("countdown")) {
        _countdown =  json_val->getLong("countdown");
    }
    if(json_val->has("autoStart")) {
        _autoStart =  (Y_AUTOSTART_enum)json_val->getInt("autoStart");
    }
    if(json_val->has("running")) {
        _running =  (Y_RUNNING_enum)json_val->getInt("running");
    }
    if(json_val->has("triggerDelay")) {
        _triggerDelay =  json_val->getLong("triggerDelay");
    }
    if(json_val->has("triggerDuration")) {
        _triggerDuration =  json_val->getLong("triggerDuration");
    }
    if(json_val->has("lastTrigger")) {
        _lastTrigger =  json_val->getInt("lastTrigger");
    }
    return YFunction::_parseAttr(json_val);
}


/**
 * Returns the state of the watchdog (A for the idle position, B for the active position).
 *
 * @return either YWatchdog::STATE_A or YWatchdog::STATE_B, according to the state of the watchdog (A
 * for the idle position, B for the active position)
 *
 * On failure, throws an exception or returns YWatchdog::STATE_INVALID.
 */
Y_STATE_enum YWatchdog::get_state(void)
{
    Y_STATE_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YWatchdog::STATE_INVALID;
                }
            }
        }
        res = _state;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the state of the watchdog (A for the idle position, B for the active position).
 *
 * @param newval : either YWatchdog::STATE_A or YWatchdog::STATE_B, according to the state of the
 * watchdog (A for the idle position, B for the active position)
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YWatchdog::set_state(Y_STATE_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = (newval>0 ? "1" : "0");
        res = _setAttr("state", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the state of the watchdog at device startup (A for the idle position,
 * B for the active position, UNCHANGED to leave the relay state as is).
 *
 * @return a value among YWatchdog::STATEATPOWERON_UNCHANGED, YWatchdog::STATEATPOWERON_A and
 * YWatchdog::STATEATPOWERON_B corresponding to the state of the watchdog at device startup (A for the
 * idle position,
 *         B for the active position, UNCHANGED to leave the relay state as is)
 *
 * On failure, throws an exception or returns YWatchdog::STATEATPOWERON_INVALID.
 */
Y_STATEATPOWERON_enum YWatchdog::get_stateAtPowerOn(void)
{
    Y_STATEATPOWERON_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YWatchdog::STATEATPOWERON_INVALID;
                }
            }
        }
        res = _stateAtPowerOn;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the state of the watchdog at device startup (A for the idle position,
 * B for the active position, UNCHANGED to leave the relay state as is).
 * Remember to call the matching module saveToFlash()
 * method, otherwise this call will have no effect.
 *
 * @param newval : a value among YWatchdog::STATEATPOWERON_UNCHANGED, YWatchdog::STATEATPOWERON_A and
 * YWatchdog::STATEATPOWERON_B corresponding to the state of the watchdog at device startup (A for the
 * idle position,
 *         B for the active position, UNCHANGED to leave the relay state as is)
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YWatchdog::set_stateAtPowerOn(Y_STATEATPOWERON_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("stateAtPowerOn", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the maximum time (ms) allowed for the watchdog to stay in state
 * A before automatically switching back in to B state. Zero means no time limit.
 *
 * @return an integer corresponding to the maximum time (ms) allowed for the watchdog to stay in state
 *         A before automatically switching back in to B state
 *
 * On failure, throws an exception or returns YWatchdog::MAXTIMEONSTATEA_INVALID.
 */
s64 YWatchdog::get_maxTimeOnStateA(void)
{
    s64 res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YWatchdog::MAXTIMEONSTATEA_INVALID;
                }
            }
        }
        res = _maxTimeOnStateA;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the maximum time (ms) allowed for the watchdog to stay in state A
 * before automatically switching back in to B state. Use zero for no time limit.
 * Remember to call the saveToFlash()
 * method of the module if the modification must be kept.
 *
 * @param newval : an integer corresponding to the maximum time (ms) allowed for the watchdog to stay in state A
 *         before automatically switching back in to B state
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YWatchdog::set_maxTimeOnStateA(s64 newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%u", (u32)newval); rest_val = string(buf);
        res = _setAttr("maxTimeOnStateA", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Retourne the maximum time (ms) allowed for the watchdog to stay in state B
 * before automatically switching back in to A state. Zero means no time limit.
 *
 * @return an integer
 *
 * On failure, throws an exception or returns YWatchdog::MAXTIMEONSTATEB_INVALID.
 */
s64 YWatchdog::get_maxTimeOnStateB(void)
{
    s64 res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YWatchdog::MAXTIMEONSTATEB_INVALID;
                }
            }
        }
        res = _maxTimeOnStateB;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the maximum time (ms) allowed for the watchdog to stay in state B before
 * automatically switching back in to A state. Use zero for no time limit.
 * Remember to call the saveToFlash()
 * method of the module if the modification must be kept.
 *
 * @param newval : an integer corresponding to the maximum time (ms) allowed for the watchdog to stay
 * in state B before
 *         automatically switching back in to A state
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YWatchdog::set_maxTimeOnStateB(s64 newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%u", (u32)newval); rest_val = string(buf);
        res = _setAttr("maxTimeOnStateB", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the output state of the watchdog, when used as a simple switch (single throw).
 *
 * @return either YWatchdog::OUTPUT_OFF or YWatchdog::OUTPUT_ON, according to the output state of the
 * watchdog, when used as a simple switch (single throw)
 *
 * On failure, throws an exception or returns YWatchdog::OUTPUT_INVALID.
 */
Y_OUTPUT_enum YWatchdog::get_output(void)
{
    Y_OUTPUT_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YWatchdog::OUTPUT_INVALID;
                }
            }
        }
        res = _output;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the output state of the watchdog, when used as a simple switch (single throw).
 *
 * @param newval : either YWatchdog::OUTPUT_OFF or YWatchdog::OUTPUT_ON, according to the output state
 * of the watchdog, when used as a simple switch (single throw)
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YWatchdog::set_output(Y_OUTPUT_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = (newval>0 ? "1" : "0");
        res = _setAttr("output", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the number of milliseconds remaining before the watchdog is returned to idle position
 * (state A), during a measured pulse generation. When there is no ongoing pulse, returns zero.
 *
 * @return an integer corresponding to the number of milliseconds remaining before the watchdog is
 * returned to idle position
 *         (state A), during a measured pulse generation
 *
 * On failure, throws an exception or returns YWatchdog::PULSETIMER_INVALID.
 */
s64 YWatchdog::get_pulseTimer(void)
{
    s64 res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YWatchdog::PULSETIMER_INVALID;
                }
            }
        }
        res = _pulseTimer;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

int YWatchdog::set_pulseTimer(s64 newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%u", (u32)newval); rest_val = string(buf);
        res = _setAttr("pulseTimer", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Sets the relay to output B (active) for a specified duration, then brings it
 * automatically back to output A (idle state).
 *
 * @param ms_duration : pulse duration, in milliseconds
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YWatchdog::pulse(int ms_duration)
{
    string rest_val;
    char buf[32]; SAFE_SPRINTF(buf, 32, "%u", (u32)ms_duration); rest_val = string(buf);
    return _setAttr("pulseTimer", rest_val);
}

YDelayedPulse YWatchdog::get_delayedPulseTimer(void)
{
    YDelayedPulse res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YWatchdog::DELAYEDPULSETIMER_INVALID;
                }
            }
        }
        res = _delayedPulseTimer;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

int YWatchdog::set_delayedPulseTimer(YDelayedPulse newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buff[64]; SAFE_SPRINTF(buff, 64, "%d:%d",newval.target,newval.ms); rest_val = string(buff);
        res = _setAttr("delayedPulseTimer", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Schedules a pulse.
 *
 * @param ms_delay : waiting time before the pulse, in milliseconds
 * @param ms_duration : pulse duration, in milliseconds
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YWatchdog::delayedPulse(int ms_delay,int ms_duration)
{
    string rest_val;
    char buff[64]; SAFE_SPRINTF(buff, 64, "%d:%d",ms_delay,ms_duration); rest_val = string(buff);
    return _setAttr("delayedPulseTimer", rest_val);
}

/**
 * Returns the number of milliseconds remaining before a pulse (delayedPulse() call)
 * When there is no scheduled pulse, returns zero.
 *
 * @return an integer corresponding to the number of milliseconds remaining before a pulse (delayedPulse() call)
 *         When there is no scheduled pulse, returns zero
 *
 * On failure, throws an exception or returns YWatchdog::COUNTDOWN_INVALID.
 */
s64 YWatchdog::get_countdown(void)
{
    s64 res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YWatchdog::COUNTDOWN_INVALID;
                }
            }
        }
        res = _countdown;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the watchdog running state at module power on.
 *
 * @return either YWatchdog::AUTOSTART_OFF or YWatchdog::AUTOSTART_ON, according to the watchdog running
 * state at module power on
 *
 * On failure, throws an exception or returns YWatchdog::AUTOSTART_INVALID.
 */
Y_AUTOSTART_enum YWatchdog::get_autoStart(void)
{
    Y_AUTOSTART_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YWatchdog::AUTOSTART_INVALID;
                }
            }
        }
        res = _autoStart;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the watchdog running state at module power on. Remember to call the
 * saveToFlash() method and then to reboot the module to apply this setting.
 *
 * @param newval : either YWatchdog::AUTOSTART_OFF or YWatchdog::AUTOSTART_ON, according to the watchdog
 * running state at module power on
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YWatchdog::set_autoStart(Y_AUTOSTART_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = (newval>0 ? "1" : "0");
        res = _setAttr("autoStart", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the watchdog running state.
 *
 * @return either YWatchdog::RUNNING_OFF or YWatchdog::RUNNING_ON, according to the watchdog running state
 *
 * On failure, throws an exception or returns YWatchdog::RUNNING_INVALID.
 */
Y_RUNNING_enum YWatchdog::get_running(void)
{
    Y_RUNNING_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YWatchdog::RUNNING_INVALID;
                }
            }
        }
        res = _running;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the running state of the watchdog.
 *
 * @param newval : either YWatchdog::RUNNING_OFF or YWatchdog::RUNNING_ON, according to the running
 * state of the watchdog
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YWatchdog::set_running(Y_RUNNING_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = (newval>0 ? "1" : "0");
        res = _setAttr("running", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Resets the watchdog. When the watchdog is running, this function
 * must be called on a regular basis to prevent the watchdog to
 * trigger
 *
 * @return YAPI::SUCCESS if the call succeeds.
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
 * On failure, throws an exception or returns YWatchdog::TRIGGERDELAY_INVALID.
 */
s64 YWatchdog::get_triggerDelay(void)
{
    s64 res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YWatchdog::TRIGGERDELAY_INVALID;
                }
            }
        }
        res = _triggerDelay;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the waiting delay before a reset is triggered by the watchdog,
 * in milliseconds. Remember to call the saveToFlash()
 * method of the module if the modification must be kept.
 *
 * @param newval : an integer corresponding to the waiting delay before a reset is triggered by the watchdog,
 *         in milliseconds
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YWatchdog::set_triggerDelay(s64 newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%u", (u32)newval); rest_val = string(buf);
        res = _setAttr("triggerDelay", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the duration of resets caused by the watchdog, in milliseconds.
 *
 * @return an integer corresponding to the duration of resets caused by the watchdog, in milliseconds
 *
 * On failure, throws an exception or returns YWatchdog::TRIGGERDURATION_INVALID.
 */
s64 YWatchdog::get_triggerDuration(void)
{
    s64 res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YWatchdog::TRIGGERDURATION_INVALID;
                }
            }
        }
        res = _triggerDuration;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the duration of resets caused by the watchdog, in milliseconds.
 * Remember to call the saveToFlash()
 * method of the module if the modification must be kept.
 *
 * @param newval : an integer corresponding to the duration of resets caused by the watchdog, in milliseconds
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YWatchdog::set_triggerDuration(s64 newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%u", (u32)newval); rest_val = string(buf);
        res = _setAttr("triggerDuration", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the number of seconds spent since the last output power-up event.
 *
 * @return an integer corresponding to the number of seconds spent since the last output power-up event
 *
 * On failure, throws an exception or returns YWatchdog::LASTTRIGGER_INVALID.
 */
int YWatchdog::get_lastTrigger(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YWatchdog::LASTTRIGGER_INVALID;
                }
            }
        }
        res = _lastTrigger;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Retrieves a watchdog for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that the watchdog is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the watchdog is
 * indeed online at a given time. In case of ambiguity when looking for
 * a watchdog by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the watchdog, for instance
 *         WDOGDC01.watchdog1.
 *
 * @return a YWatchdog object allowing you to drive the watchdog.
 */
YWatchdog* YWatchdog::FindWatchdog(string func)
{
    YWatchdog* obj = NULL;
    int taken = 0;
    if (YAPI::_apiInitialized) {
        yEnterCriticalSection(&YAPI::_global_cs);
        taken = 1;
    }try {
        obj = (YWatchdog*) YFunction::_FindFromCache("Watchdog", func);
        if (obj == NULL) {
            obj = new YWatchdog(func);
            YFunction::_AddToCache("Watchdog", func, obj);
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

/**
 * Switch the relay to the opposite state.
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YWatchdog::toggle(void)
{
    int sta = 0;
    string fw;
    YModule* mo = NULL;
    if (_firm == 0) {
        mo = this->get_module();
        fw = mo->get_firmwareRelease();
        if (fw == YModule::FIRMWARERELEASE_INVALID) {
            return Y_STATE_INVALID;
        }
        _firm = atoi((fw).c_str());
    }
    if (_firm < 34921) {
        sta = this->get_state();
        if (sta == Y_STATE_INVALID) {
            return Y_STATE_INVALID;
        }
        if (sta == Y_STATE_B) {
            this->set_state(Y_STATE_A);
        } else {
            this->set_state(Y_STATE_B);
        }
        return YAPI_SUCCESS;
    } else {
        return this->_setAttr("state","X");
    }
}

YWatchdog *YWatchdog::nextWatchdog(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YWatchdog::FindWatchdog(hwid);
}

YWatchdog *YWatchdog::FirstWatchdog(void)
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

//--- (YWatchdog functions)
//--- (end of YWatchdog functions)
