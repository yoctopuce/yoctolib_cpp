/*********************************************************************
 *
 * $Id: yocto_wakeupmonitor.cpp 12324 2013-08-13 15:10:31Z mvuilleu $
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

//--- (YWakeUpMonitor constructor)
// Constructor is protected, use yFindWakeUpMonitor factory function to instantiate
YWakeUpMonitor::YWakeUpMonitor(const string& func): YFunction("WakeUpMonitor", func)
//--- (end of YWakeUpMonitor constructor)
//--- (WakeUpMonitor initialization)
            ,_callback(NULL)
            ,_logicalName(Y_LOGICALNAME_INVALID)
            ,_advertisedValue(Y_ADVERTISEDVALUE_INVALID)
            ,_powerDuration(Y_POWERDURATION_INVALID)
            ,_sleepCountdown(Y_SLEEPCOUNTDOWN_INVALID)
            ,_nextWakeUp(Y_NEXTWAKEUP_INVALID)
            ,_wakeUpReason(Y_WAKEUPREASON_INVALID)
            ,_wakeUpState(Y_WAKEUPSTATE_INVALID)
            ,_rtcTime(Y_RTCTIME_INVALID)
            ,_endOfTime(2145960000)
//--- (end of WakeUpMonitor initialization)
{}

YWakeUpMonitor::~YWakeUpMonitor() 
{
//--- (YWakeUpMonitor cleanup)
//--- (end of YWakeUpMonitor cleanup)
}
//--- (YWakeUpMonitor implementation)

const string YWakeUpMonitor::LOGICALNAME_INVALID = "!INVALID!";
const string YWakeUpMonitor::ADVERTISEDVALUE_INVALID = "!INVALID!";



int YWakeUpMonitor::_parse(yJsonStateMachine& j)
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
        } else if(!strcmp(j.token, "powerDuration")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _powerDuration =  atoi(j.token);
        } else if(!strcmp(j.token, "sleepCountdown")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _sleepCountdown =  atoi(j.token);
        } else if(!strcmp(j.token, "nextWakeUp")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _nextWakeUp =  atoi(j.token);
        } else if(!strcmp(j.token, "wakeUpReason")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _wakeUpReason =  (Y_WAKEUPREASON_enum)atoi(j.token);
        } else if(!strcmp(j.token, "wakeUpState")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _wakeUpState =  (Y_WAKEUPSTATE_enum)atoi(j.token);
        } else if(!strcmp(j.token, "rtcTime")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _rtcTime =  atoi(j.token);
        } else {
            // ignore unknown field
            yJsonSkip(&j, 1);
        }
    }
    if(j.st != YJSON_PARSE_STRUCT) goto failed;
    return 0;
}

/**
 * Returns the logical name of the monitor.
 * 
 * @return a string corresponding to the logical name of the monitor
 * 
 * On failure, throws an exception or returns Y_LOGICALNAME_INVALID.
 */
string YWakeUpMonitor::get_logicalName(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_LOGICALNAME_INVALID;
    }
    return _logicalName;
}

/**
 * Changes the logical name of the monitor. You can use yCheckLogicalName()
 * prior to this call to make sure that your parameter is valid.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 * 
 * @param newval : a string corresponding to the logical name of the monitor
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YWakeUpMonitor::set_logicalName(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("logicalName", rest_val);
}

/**
 * Returns the current value of the monitor (no more than 6 characters).
 * 
 * @return a string corresponding to the current value of the monitor (no more than 6 characters)
 * 
 * On failure, throws an exception or returns Y_ADVERTISEDVALUE_INVALID.
 */
string YWakeUpMonitor::get_advertisedValue(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_ADVERTISEDVALUE_INVALID;
    }
    return _advertisedValue;
}

/**
 * Returns the maximal wake up time (seconds) before going to sleep automatically.
 * 
 * @return an integer corresponding to the maximal wake up time (seconds) before going to sleep automatically
 * 
 * On failure, throws an exception or returns Y_POWERDURATION_INVALID.
 */
int YWakeUpMonitor::get_powerDuration(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_POWERDURATION_INVALID;
    }
    return _powerDuration;
}

/**
 * Changes the maximal wake up time (seconds) before going to sleep automatically.
 * 
 * @param newval : an integer corresponding to the maximal wake up time (seconds) before going to
 * sleep automatically
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
 * Returns the delay before next sleep.
 * 
 * @return an integer corresponding to the delay before next sleep
 * 
 * On failure, throws an exception or returns Y_SLEEPCOUNTDOWN_INVALID.
 */
int YWakeUpMonitor::get_sleepCountdown(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_SLEEPCOUNTDOWN_INVALID;
    }
    return _sleepCountdown;
}

/**
 * Changes the delay before next sleep.
 * 
 * @param newval : an integer corresponding to the delay before next sleep
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
 * Returns the next scheduled wake-up date/time (UNIX format)
 * 
 * @return an integer corresponding to the next scheduled wake-up date/time (UNIX format)
 * 
 * On failure, throws an exception or returns Y_NEXTWAKEUP_INVALID.
 */
unsigned YWakeUpMonitor::get_nextWakeUp(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_NEXTWAKEUP_INVALID;
    }
    return _nextWakeUp;
}

/**
 * Changes the days of the week where a wake up must take place.
 * 
 * @param newval : an integer corresponding to the days of the week where a wake up must take place
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YWakeUpMonitor::set_nextWakeUp(unsigned newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
    return _setAttr("nextWakeUp", rest_val);
}

/**
 * Return the last wake up reason.
 * 
 * @return a value among Y_WAKEUPREASON_USBPOWER, Y_WAKEUPREASON_EXTPOWER, Y_WAKEUPREASON_ENDOFSLEEP,
 * Y_WAKEUPREASON_EXTSIG1, Y_WAKEUPREASON_EXTSIG2, Y_WAKEUPREASON_EXTSIG3, Y_WAKEUPREASON_EXTSIG4,
 * Y_WAKEUPREASON_SCHEDULE1, Y_WAKEUPREASON_SCHEDULE2, Y_WAKEUPREASON_SCHEDULE3,
 * Y_WAKEUPREASON_SCHEDULE4, Y_WAKEUPREASON_SCHEDULE5 and Y_WAKEUPREASON_SCHEDULE6
 * 
 * On failure, throws an exception or returns Y_WAKEUPREASON_INVALID.
 */
Y_WAKEUPREASON_enum YWakeUpMonitor::get_wakeUpReason(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_WAKEUPREASON_INVALID;
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
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_WAKEUPSTATE_INVALID;
    }
    return _wakeUpState;
}

int YWakeUpMonitor::set_wakeUpState(Y_WAKEUPSTATE_enum newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
    return _setAttr("wakeUpState", rest_val);
}

unsigned YWakeUpMonitor::get_rtcTime(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_RTCTIME_INVALID;
    }
    return _rtcTime;
}

/**
 * Forces a wakeup.
 */
int YWakeUpMonitor::wakeUp()
{
    return this->set_wakeUpState(Y_WAKEUPSTATE_AWAKE);
    
}

/**
 * Go to sleep until the next wakeup condition is met,  the
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
    int currTime;
    currTime = this->get_rtcTime();
    if (!(currTime != 0)) {this->_throw( YAPI_RTC_NOT_READY, "RTC time not set"); return  YAPI_RTC_NOT_READY;};
    this->set_nextWakeUp(this->_endOfTime);
    this->set_sleepCountdown(secBeforeSleep);
    return YAPI_SUCCESS; 
    
}

/**
 * Go to sleep for a specific time or until the next wakeup condition is met, the
 * RTC time must have been set before calling this function. The count down before sleep
 * can be canceled with resetSleepCountDown.
 * 
 * @param secUntilWakeUp : sleep duration, in secondes
 * @param secBeforeSleep : number of seconds before going into sleep mode
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YWakeUpMonitor::sleepFor(int secUntilWakeUp,int secBeforeSleep)
{
    int currTime;
    currTime = this->get_rtcTime();
    if (!(currTime != 0)) {this->_throw( YAPI_RTC_NOT_READY, "RTC time not set"); return  YAPI_RTC_NOT_READY;};
    this->set_nextWakeUp(currTime+secUntilWakeUp);
    this->set_sleepCountdown(secBeforeSleep);
    return YAPI_SUCCESS; 
    
}

/**
 * Go to sleep until a specific date is reached or until the next wakeup condition is met, the
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
    int currTime;
    currTime = this->get_rtcTime();
    if (!(currTime != 0)) {this->_throw( YAPI_RTC_NOT_READY, "RTC time not set"); return  YAPI_RTC_NOT_READY;};
    this->set_nextWakeUp(wakeUpTime);
    this->set_sleepCountdown(secBeforeSleep);
    return YAPI_SUCCESS; 
    
}

/**
 * Reset the sleep countdown.
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 *         On failure, throws an exception or returns a negative error code.
 */
int YWakeUpMonitor::resetSleepCountDown()
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
    return yFindWakeUpMonitor(hwid);
}

void YWakeUpMonitor::registerValueCallback(YWakeUpMonitorUpdateCallback callback)
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

void YWakeUpMonitor::advertiseValue(const string& value)
{
    if (_callback != NULL) {
        _callback(this, value);
    }
}


YWakeUpMonitor* YWakeUpMonitor::FindWakeUpMonitor(const string& func)
{
    if(YAPI::_YFunctionsCaches["YWakeUpMonitor"].find(func) != YAPI::_YFunctionsCaches["YWakeUpMonitor"].end())
        return (YWakeUpMonitor*) YAPI::_YFunctionsCaches["YWakeUpMonitor"][func];
    
    YWakeUpMonitor *newWakeUpMonitor = new YWakeUpMonitor(func);
    YAPI::_YFunctionsCaches["YWakeUpMonitor"][func] = newWakeUpMonitor ;
    return newWakeUpMonitor;
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
