/*********************************************************************
 *
 * $Id: yocto_wakeupmonitor.cpp 22191 2015-12-02 06:49:31Z mvuilleu $
 *
 * Implements yFindWakeUpMonitor(), the high-level API for WakeUpMonitor functions
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
#include "yocto_wakeupmonitor.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

YWakeUpMonitor::YWakeUpMonitor(const string& func): YFunction(func)
//--- (WakeUpMonitor initialization)
    ,_powerDuration(POWERDURATION_INVALID)
    ,_sleepCountdown(SLEEPCOUNTDOWN_INVALID)
    ,_nextWakeUp(NEXTWAKEUP_INVALID)
    ,_wakeUpReason(WAKEUPREASON_INVALID)
    ,_wakeUpState(WAKEUPSTATE_INVALID)
    ,_rtcTime(RTCTIME_INVALID)
    ,_endOfTime(2145960000)
    ,_valueCallbackWakeUpMonitor(NULL)
//--- (end of WakeUpMonitor initialization)
{
    _className="WakeUpMonitor";
}

YWakeUpMonitor::~YWakeUpMonitor()
{
//--- (YWakeUpMonitor cleanup)
//--- (end of YWakeUpMonitor cleanup)
}
//--- (YWakeUpMonitor implementation)
// static attributes

int YWakeUpMonitor::_parseAttr(yJsonStateMachine& j)
{
    if(!strcmp(j.token, "powerDuration")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _powerDuration =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "sleepCountdown")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _sleepCountdown =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "nextWakeUp")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _nextWakeUp =  atol(j.token);
        return 1;
    }
    if(!strcmp(j.token, "wakeUpReason")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _wakeUpReason =  (Y_WAKEUPREASON_enum)atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "wakeUpState")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _wakeUpState =  (Y_WAKEUPSTATE_enum)atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "rtcTime")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _rtcTime =  atol(j.token);
        return 1;
    }
    failed:
    return YFunction::_parseAttr(j);
}


/**
 * Returns the maximal wake up time (in seconds) before automatically going to sleep.
 *
 * @return an integer corresponding to the maximal wake up time (in seconds) before automatically going to sleep
 *
 * On failure, throws an exception or returns Y_POWERDURATION_INVALID.
 */
int YWakeUpMonitor::get_powerDuration(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YWakeUpMonitor::POWERDURATION_INVALID;
        }
    }
    return _powerDuration;
}

/**
 * Changes the maximal wake up time (seconds) before automatically going to sleep.
 *
 * @param newval : an integer corresponding to the maximal wake up time (seconds) before automatically
 * going to sleep
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YWakeUpMonitor::set_powerDuration(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("powerDuration", rest_val);
}

/**
 * Returns the delay before the  next sleep period.
 *
 * @return an integer corresponding to the delay before the  next sleep period
 *
 * On failure, throws an exception or returns Y_SLEEPCOUNTDOWN_INVALID.
 */
int YWakeUpMonitor::get_sleepCountdown(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YWakeUpMonitor::SLEEPCOUNTDOWN_INVALID;
        }
    }
    return _sleepCountdown;
}

/**
 * Changes the delay before the next sleep period.
 *
 * @param newval : an integer corresponding to the delay before the next sleep period
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YWakeUpMonitor::set_sleepCountdown(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("sleepCountdown", rest_val);
}

/**
 * Returns the next scheduled wake up date/time (UNIX format)
 *
 * @return an integer corresponding to the next scheduled wake up date/time (UNIX format)
 *
 * On failure, throws an exception or returns Y_NEXTWAKEUP_INVALID.
 */
s64 YWakeUpMonitor::get_nextWakeUp(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YWakeUpMonitor::NEXTWAKEUP_INVALID;
        }
    }
    return _nextWakeUp;
}

/**
 * Changes the days of the week when a wake up must take place.
 *
 * @param newval : an integer corresponding to the days of the week when a wake up must take place
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YWakeUpMonitor::set_nextWakeUp(s64 newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", (u32)newval); rest_val = string(buf);
    return _setAttr("nextWakeUp", rest_val);
}

/**
 * Returns the latest wake up reason.
 *
 * @return a value among Y_WAKEUPREASON_USBPOWER, Y_WAKEUPREASON_EXTPOWER, Y_WAKEUPREASON_ENDOFSLEEP,
 * Y_WAKEUPREASON_EXTSIG1, Y_WAKEUPREASON_SCHEDULE1 and Y_WAKEUPREASON_SCHEDULE2 corresponding to the
 * latest wake up reason
 *
 * On failure, throws an exception or returns Y_WAKEUPREASON_INVALID.
 */
Y_WAKEUPREASON_enum YWakeUpMonitor::get_wakeUpReason(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YWakeUpMonitor::WAKEUPREASON_INVALID;
        }
    }
    return _wakeUpReason;
}

/**
 * Returns  the current state of the monitor
 *
 * @return either Y_WAKEUPSTATE_SLEEPING or Y_WAKEUPSTATE_AWAKE, according to  the current state of the monitor
 *
 * On failure, throws an exception or returns Y_WAKEUPSTATE_INVALID.
 */
Y_WAKEUPSTATE_enum YWakeUpMonitor::get_wakeUpState(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YWakeUpMonitor::WAKEUPSTATE_INVALID;
        }
    }
    return _wakeUpState;
}

int YWakeUpMonitor::set_wakeUpState(Y_WAKEUPSTATE_enum newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("wakeUpState", rest_val);
}

s64 YWakeUpMonitor::get_rtcTime(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YWakeUpMonitor::RTCTIME_INVALID;
        }
    }
    return _rtcTime;
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
 * Use the method YWakeUpMonitor.isOnline() to test if $THEFUNCTION$ is
 * indeed online at a given time. In case of ambiguity when looking for
 * $AFUNCTION$ by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * @param func : a string that uniquely characterizes $THEFUNCTION$
 *
 * @return a YWakeUpMonitor object allowing you to drive $THEFUNCTION$.
 */
YWakeUpMonitor* YWakeUpMonitor::FindWakeUpMonitor(string func)
{
    YWakeUpMonitor* obj = NULL;
    obj = (YWakeUpMonitor*) YFunction::_FindFromCache("WakeUpMonitor", func);
    if (obj == NULL) {
        obj = new YWakeUpMonitor(func);
        YFunction::_AddToCache("WakeUpMonitor", func, obj);
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
int YWakeUpMonitor::registerValueCallback(YWakeUpMonitorValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackWakeUpMonitor = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YWakeUpMonitor::_invokeValueCallback(string value)
{
    if (_valueCallbackWakeUpMonitor != NULL) {
        _valueCallbackWakeUpMonitor(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

/**
 * Forces a wake up.
 */
int YWakeUpMonitor::wakeUp(void)
{
    return this->set_wakeUpState(Y_WAKEUPSTATE_AWAKE);
}

/**
 * Goes to sleep until the next wake up condition is met,  the
 * RTC time must have been set before calling this function.
 *
 * @param secBeforeSleep : number of seconds before going into sleep mode,
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YWakeUpMonitor::sleep(int secBeforeSleep)
{
    int currTime = 0;
    currTime = (int)(this->get_rtcTime());
    if (!(currTime != 0)) {
        _throw(YAPI_RTC_NOT_READY,"RTC time not set");
        return YAPI_RTC_NOT_READY;
    }
    this->set_nextWakeUp(_endOfTime);
    this->set_sleepCountdown(secBeforeSleep);
    return YAPI_SUCCESS;
}

/**
 * Goes to sleep for a specific duration or until the next wake up condition is met, the
 * RTC time must have been set before calling this function. The count down before sleep
 * can be canceled with resetSleepCountDown.
 *
 * @param secUntilWakeUp : number of seconds before next wake up
 * @param secBeforeSleep : number of seconds before going into sleep mode
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YWakeUpMonitor::sleepFor(int secUntilWakeUp,int secBeforeSleep)
{
    int currTime = 0;
    currTime = (int)(this->get_rtcTime());
    if (!(currTime != 0)) {
        _throw(YAPI_RTC_NOT_READY,"RTC time not set");
        return YAPI_RTC_NOT_READY;
    }
    this->set_nextWakeUp(currTime+secUntilWakeUp);
    this->set_sleepCountdown(secBeforeSleep);
    return YAPI_SUCCESS;
}

/**
 * Go to sleep until a specific date is reached or until the next wake up condition is met, the
 * RTC time must have been set before calling this function. The count down before sleep
 * can be canceled with resetSleepCountDown.
 *
 * @param wakeUpTime : wake-up datetime (UNIX format)
 * @param secBeforeSleep : number of seconds before going into sleep mode
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YWakeUpMonitor::sleepUntil(int wakeUpTime,int secBeforeSleep)
{
    int currTime = 0;
    currTime = (int)(this->get_rtcTime());
    if (!(currTime != 0)) {
        _throw(YAPI_RTC_NOT_READY,"RTC time not set");
        return YAPI_RTC_NOT_READY;
    }
    this->set_nextWakeUp(wakeUpTime);
    this->set_sleepCountdown(secBeforeSleep);
    return YAPI_SUCCESS;
}

/**
 * Resets the sleep countdown.
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *         On failure, throws an exception or returns a negative error code.
 */
int YWakeUpMonitor::resetSleepCountDown(void)
{
    this->set_sleepCountdown(0);
    this->set_nextWakeUp(0);
    return YAPI_SUCCESS;
}

YWakeUpMonitor *YWakeUpMonitor::nextWakeUpMonitor(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YWakeUpMonitor::FindWakeUpMonitor(hwid);
}

YWakeUpMonitor* YWakeUpMonitor::FirstWakeUpMonitor(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("WakeUpMonitor", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YWakeUpMonitor::FindWakeUpMonitor(serial+"."+funcId);
}

//--- (end of YWakeUpMonitor implementation)

//--- (WakeUpMonitor functions)
//--- (end of WakeUpMonitor functions)
