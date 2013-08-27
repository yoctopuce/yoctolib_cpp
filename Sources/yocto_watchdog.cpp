/*********************************************************************
 *
 * $Id: yocto_watchdog.cpp 12324 2013-08-13 15:10:31Z mvuilleu $
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

//--- (YWatchdog constructor)
// Constructor is protected, use yFindWatchdog factory function to instantiate
YWatchdog::YWatchdog(const string& func): YFunction("Watchdog", func)
//--- (end of YWatchdog constructor)
//--- (Watchdog initialization)
            ,_callback(NULL)
            ,_logicalName(Y_LOGICALNAME_INVALID)
            ,_advertisedValue(Y_ADVERTISEDVALUE_INVALID)
            ,_state(Y_STATE_INVALID)
            ,_output(Y_OUTPUT_INVALID)
            ,_pulseTimer(Y_PULSETIMER_INVALID)
            ,_delayedPulseTimer()
            ,_countdown(Y_COUNTDOWN_INVALID)
            ,_autoStart(Y_AUTOSTART_INVALID)
            ,_running(Y_RUNNING_INVALID)
            ,_triggerDelay(Y_TRIGGERDELAY_INVALID)
            ,_triggerDuration(Y_TRIGGERDURATION_INVALID)
//--- (end of Watchdog initialization)
{}

YWatchdog::~YWatchdog() 
{
//--- (YWatchdog cleanup)
//--- (end of YWatchdog cleanup)
}
//--- (YWatchdog implementation)
YDelayedPulse YWATCHDOG_INVALID_DELAYEDPULSE;

const string YWatchdog::LOGICALNAME_INVALID = "!INVALID!";
const string YWatchdog::ADVERTISEDVALUE_INVALID = "!INVALID!";



int YWatchdog::_parse(yJsonStateMachine& j)
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
        } else if(!strcmp(j.token, "autoStart")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _autoStart =  (Y_AUTOSTART_enum)atoi(j.token);
        } else if(!strcmp(j.token, "running")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _running =  (Y_RUNNING_enum)atoi(j.token);
        } else if(!strcmp(j.token, "triggerDelay")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _triggerDelay =  atoi(j.token);
        } else if(!strcmp(j.token, "triggerDuration")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _triggerDuration =  atoi(j.token);
        } else {
            // ignore unknown field
            yJsonSkip(&j, 1);
        }
    }
    if(j.st != YJSON_PARSE_STRUCT) goto failed;
    return 0;
}

/**
 * Returns the logical name of the watchdog.
 * 
 * @return a string corresponding to the logical name of the watchdog
 * 
 * On failure, throws an exception or returns Y_LOGICALNAME_INVALID.
 */
string YWatchdog::get_logicalName(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_LOGICALNAME_INVALID;
    }
    return _logicalName;
}

/**
 * Changes the logical name of the watchdog. You can use yCheckLogicalName()
 * prior to this call to make sure that your parameter is valid.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 * 
 * @param newval : a string corresponding to the logical name of the watchdog
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YWatchdog::set_logicalName(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("logicalName", rest_val);
}

/**
 * Returns the current value of the watchdog (no more than 6 characters).
 * 
 * @return a string corresponding to the current value of the watchdog (no more than 6 characters)
 * 
 * On failure, throws an exception or returns Y_ADVERTISEDVALUE_INVALID.
 */
string YWatchdog::get_advertisedValue(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_ADVERTISEDVALUE_INVALID;
    }
    return _advertisedValue;
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
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_STATE_INVALID;
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
 * Returns the output state of the watchdog, when used as a simple switch (single throw).
 * 
 * @return either Y_OUTPUT_OFF or Y_OUTPUT_ON, according to the output state of the watchdog, when
 * used as a simple switch (single throw)
 * 
 * On failure, throws an exception or returns Y_OUTPUT_INVALID.
 */
Y_OUTPUT_enum YWatchdog::get_output(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_OUTPUT_INVALID;
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
unsigned YWatchdog::get_pulseTimer(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_PULSETIMER_INVALID;
    }
    return _pulseTimer;
}

int YWatchdog::set_pulseTimer(unsigned newval)
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
int YWatchdog::pulse(int ms_duration)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", ms_duration); rest_val = string(buf);
    return _setAttr("pulseTimer", rest_val);
}

const YDelayedPulse * YWatchdog::get_delayedPulseTimer(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_DELAYEDPULSETIMER_INVALID;
    }
    return &_delayedPulseTimer;
}

int YWatchdog::set_delayedPulseTimer(const YDelayedPulse * newval)
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
unsigned YWatchdog::get_countdown(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_COUNTDOWN_INVALID;
    }
    return _countdown;
}

/**
 * Returns the watchdog runing state at module power up.
 * 
 * @return either Y_AUTOSTART_OFF or Y_AUTOSTART_ON, according to the watchdog runing state at module power up
 * 
 * On failure, throws an exception or returns Y_AUTOSTART_INVALID.
 */
Y_AUTOSTART_enum YWatchdog::get_autoStart(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_AUTOSTART_INVALID;
    }
    return _autoStart;
}

/**
 * Changes the watchdog runningsttae at module power up. Remember to call the
 * saveToFlash() method and then to reboot the module to apply this setting.
 * 
 * @param newval : either Y_AUTOSTART_OFF or Y_AUTOSTART_ON, according to the watchdog runningsttae at
 * module power up
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
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_RUNNING_INVALID;
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
unsigned YWatchdog::get_triggerDelay(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_TRIGGERDELAY_INVALID;
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
int YWatchdog::set_triggerDelay(unsigned newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
    return _setAttr("triggerDelay", rest_val);
}

/**
 * Returns the duration of resets caused by the watchdog, in milliseconds.
 * 
 * @return an integer corresponding to the duration of resets caused by the watchdog, in milliseconds
 * 
 * On failure, throws an exception or returns Y_TRIGGERDURATION_INVALID.
 */
unsigned YWatchdog::get_triggerDuration(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_TRIGGERDURATION_INVALID;
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
int YWatchdog::set_triggerDuration(unsigned newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
    return _setAttr("triggerDuration", rest_val);
}

YWatchdog *YWatchdog::nextWatchdog(void)
{
    string  hwid;
    
    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return yFindWatchdog(hwid);
}

void YWatchdog::registerValueCallback(YWatchdogUpdateCallback callback)
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

void YWatchdog::advertiseValue(const string& value)
{
    if (_callback != NULL) {
        _callback(this, value);
    }
}


YWatchdog* YWatchdog::FindWatchdog(const string& func)
{
    if(YAPI::_YFunctionsCaches["YWatchdog"].find(func) != YAPI::_YFunctionsCaches["YWatchdog"].end())
        return (YWatchdog*) YAPI::_YFunctionsCaches["YWatchdog"][func];
    
    YWatchdog *newWatchdog = new YWatchdog(func);
    YAPI::_YFunctionsCaches["YWatchdog"][func] = newWatchdog ;
    return newWatchdog;
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
