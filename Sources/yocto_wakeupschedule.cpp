/*********************************************************************
 *
 * $Id: yocto_wakeupschedule.cpp 12469 2013-08-22 10:11:58Z seb $
 *
 * Implements yFindWakeUpSchedule(), the high-level API for WakeUpSchedule functions
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
#include "yocto_wakeupschedule.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

//--- (YWakeUpSchedule constructor)
// Constructor is protected, use yFindWakeUpSchedule factory function to instantiate
YWakeUpSchedule::YWakeUpSchedule(const string& func): YFunction("WakeUpSchedule", func)
//--- (end of YWakeUpSchedule constructor)
//--- (WakeUpSchedule initialization)
            ,_callback(NULL)
            ,_logicalName(Y_LOGICALNAME_INVALID)
            ,_advertisedValue(Y_ADVERTISEDVALUE_INVALID)
            ,_minutesA(Y_MINUTESA_INVALID)
            ,_minutesB(Y_MINUTESB_INVALID)
            ,_hours(Y_HOURS_INVALID)
            ,_weekDays(Y_WEEKDAYS_INVALID)
            ,_monthDays(Y_MONTHDAYS_INVALID)
            ,_months(Y_MONTHS_INVALID)
            ,_nextOccurence(Y_NEXTOCCURENCE_INVALID)
//--- (end of WakeUpSchedule initialization)
{}

YWakeUpSchedule::~YWakeUpSchedule() 
{
//--- (YWakeUpSchedule cleanup)
//--- (end of YWakeUpSchedule cleanup)
}
//--- (YWakeUpSchedule implementation)

const string YWakeUpSchedule::LOGICALNAME_INVALID = "!INVALID!";
const string YWakeUpSchedule::ADVERTISEDVALUE_INVALID = "!INVALID!";



int YWakeUpSchedule::_parse(yJsonStateMachine& j)
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
        } else if(!strcmp(j.token, "minutesA")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _minutesA =  atoi(j.token);
        } else if(!strcmp(j.token, "minutesB")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _minutesB =  atoi(j.token);
        } else if(!strcmp(j.token, "hours")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _hours =  atoi(j.token);
        } else if(!strcmp(j.token, "weekDays")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _weekDays =  atoi(j.token);
        } else if(!strcmp(j.token, "monthDays")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _monthDays =  atoi(j.token);
        } else if(!strcmp(j.token, "months")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _months =  atoi(j.token);
        } else if(!strcmp(j.token, "nextOccurence")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _nextOccurence =  atoi(j.token);
        } else {
            // ignore unknown field
            yJsonSkip(&j, 1);
        }
    }
    if(j.st != YJSON_PARSE_STRUCT) goto failed;
    return 0;
}

/**
 * Returns the logical name of the wake-up schedule.
 * 
 * @return a string corresponding to the logical name of the wake-up schedule
 * 
 * On failure, throws an exception or returns Y_LOGICALNAME_INVALID.
 */
string YWakeUpSchedule::get_logicalName(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_LOGICALNAME_INVALID;
    }
    return _logicalName;
}

/**
 * Changes the logical name of the wake-up schedule. You can use yCheckLogicalName()
 * prior to this call to make sure that your parameter is valid.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 * 
 * @param newval : a string corresponding to the logical name of the wake-up schedule
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YWakeUpSchedule::set_logicalName(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("logicalName", rest_val);
}

/**
 * Returns the current value of the wake-up schedule (no more than 6 characters).
 * 
 * @return a string corresponding to the current value of the wake-up schedule (no more than 6 characters)
 * 
 * On failure, throws an exception or returns Y_ADVERTISEDVALUE_INVALID.
 */
string YWakeUpSchedule::get_advertisedValue(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_ADVERTISEDVALUE_INVALID;
    }
    return _advertisedValue;
}

/**
 * Returns the minutes 00-29 of each hour scheduled for wake-up.
 * 
 * @return an integer corresponding to the minutes 00-29 of each hour scheduled for wake-up
 * 
 * On failure, throws an exception or returns Y_MINUTESA_INVALID.
 */
unsigned YWakeUpSchedule::get_minutesA(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_MINUTESA_INVALID;
    }
    return _minutesA;
}

/**
 * Changes the minutes 00-29 where a wake up must take place.
 * 
 * @param newval : an integer corresponding to the minutes 00-29 where a wake up must take place
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YWakeUpSchedule::set_minutesA(unsigned newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
    return _setAttr("minutesA", rest_val);
}

/**
 * Returns the minutes 30-59 of each hour scheduled for wake-up.
 * 
 * @return an integer corresponding to the minutes 30-59 of each hour scheduled for wake-up
 * 
 * On failure, throws an exception or returns Y_MINUTESB_INVALID.
 */
unsigned YWakeUpSchedule::get_minutesB(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_MINUTESB_INVALID;
    }
    return _minutesB;
}

/**
 * Changes the minutes 30-59 where a wake up must take place.
 * 
 * @param newval : an integer corresponding to the minutes 30-59 where a wake up must take place
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YWakeUpSchedule::set_minutesB(unsigned newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
    return _setAttr("minutesB", rest_val);
}

/**
 * Returns the hours  scheduled for wake-up.
 * 
 * @return an integer corresponding to the hours  scheduled for wake-up
 * 
 * On failure, throws an exception or returns Y_HOURS_INVALID.
 */
int YWakeUpSchedule::get_hours(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_HOURS_INVALID;
    }
    return _hours;
}

/**
 * Changes the hours where a wake up must take place.
 * 
 * @param newval : an integer corresponding to the hours where a wake up must take place
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YWakeUpSchedule::set_hours(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
    return _setAttr("hours", rest_val);
}

/**
 * Returns the days of week scheduled for wake-up.
 * 
 * @return an integer corresponding to the days of week scheduled for wake-up
 * 
 * On failure, throws an exception or returns Y_WEEKDAYS_INVALID.
 */
int YWakeUpSchedule::get_weekDays(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_WEEKDAYS_INVALID;
    }
    return _weekDays;
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
int YWakeUpSchedule::set_weekDays(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
    return _setAttr("weekDays", rest_val);
}

/**
 * Returns the days of week scheduled for wake-up.
 * 
 * @return an integer corresponding to the days of week scheduled for wake-up
 * 
 * On failure, throws an exception or returns Y_MONTHDAYS_INVALID.
 */
unsigned YWakeUpSchedule::get_monthDays(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_MONTHDAYS_INVALID;
    }
    return _monthDays;
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
int YWakeUpSchedule::set_monthDays(unsigned newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
    return _setAttr("monthDays", rest_val);
}

/**
 * Returns the days of week scheduled for wake-up.
 * 
 * @return an integer corresponding to the days of week scheduled for wake-up
 * 
 * On failure, throws an exception or returns Y_MONTHS_INVALID.
 */
int YWakeUpSchedule::get_months(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_MONTHS_INVALID;
    }
    return _months;
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
int YWakeUpSchedule::set_months(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
    return _setAttr("months", rest_val);
}

/**
 * Returns the  nextwake up date/time (seconds) wake up occurence
 * 
 * @return an integer corresponding to the  nextwake up date/time (seconds) wake up occurence
 * 
 * On failure, throws an exception or returns Y_NEXTOCCURENCE_INVALID.
 */
unsigned YWakeUpSchedule::get_nextOccurence(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_NEXTOCCURENCE_INVALID;
    }
    return _nextOccurence;
}

/**
 * Returns every the minutes of each hour scheduled for wake-up.
 */
long YWakeUpSchedule::get_minutes()
{
    long res;
    res = this->get_minutesB();
    res = res << 30;
    res = res + this->get_minutesA();
    return res;
    
}

/**
 * Changes all the minutes where a wake up must take place.
 * 
 * @param bitmap : Minutes 00-59 of each hour scheduled for wake-up.,
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YWakeUpSchedule::set_minutes(long bitmap)
{
    this->set_minutesA(bitmap & 0x3fffffff);
    bitmap = bitmap >> 30;
    return this->set_minutesB(bitmap & 0x3fffffff);
    
}

YWakeUpSchedule *YWakeUpSchedule::nextWakeUpSchedule(void)
{
    string  hwid;
    
    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return yFindWakeUpSchedule(hwid);
}

void YWakeUpSchedule::registerValueCallback(YWakeUpScheduleUpdateCallback callback)
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

void YWakeUpSchedule::advertiseValue(const string& value)
{
    if (_callback != NULL) {
        _callback(this, value);
    }
}


YWakeUpSchedule* YWakeUpSchedule::FindWakeUpSchedule(const string& func)
{
    if(YAPI::_YFunctionsCaches["YWakeUpSchedule"].find(func) != YAPI::_YFunctionsCaches["YWakeUpSchedule"].end())
        return (YWakeUpSchedule*) YAPI::_YFunctionsCaches["YWakeUpSchedule"][func];
    
    YWakeUpSchedule *newWakeUpSchedule = new YWakeUpSchedule(func);
    YAPI::_YFunctionsCaches["YWakeUpSchedule"][func] = newWakeUpSchedule ;
    return newWakeUpSchedule;
}

YWakeUpSchedule* YWakeUpSchedule::FirstWakeUpSchedule(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;
    
    if(YISERR(YapiWrapper::getFunctionsByClass("WakeUpSchedule", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YWakeUpSchedule::FindWakeUpSchedule(serial+"."+funcId);
}

//--- (end of YWakeUpSchedule implementation)

//--- (WakeUpSchedule functions)
//--- (end of WakeUpSchedule functions)
