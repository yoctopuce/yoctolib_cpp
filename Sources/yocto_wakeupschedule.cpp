/*********************************************************************
 *
 * $Id: yocto_wakeupschedule.cpp 22191 2015-12-02 06:49:31Z mvuilleu $
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

YWakeUpSchedule::YWakeUpSchedule(const string& func): YFunction(func)
//--- (WakeUpSchedule initialization)
    ,_minutesA(MINUTESA_INVALID)
    ,_minutesB(MINUTESB_INVALID)
    ,_hours(HOURS_INVALID)
    ,_weekDays(WEEKDAYS_INVALID)
    ,_monthDays(MONTHDAYS_INVALID)
    ,_months(MONTHS_INVALID)
    ,_nextOccurence(NEXTOCCURENCE_INVALID)
    ,_valueCallbackWakeUpSchedule(NULL)
//--- (end of WakeUpSchedule initialization)
{
    _className="WakeUpSchedule";
}

YWakeUpSchedule::~YWakeUpSchedule()
{
//--- (YWakeUpSchedule cleanup)
//--- (end of YWakeUpSchedule cleanup)
}
//--- (YWakeUpSchedule implementation)
// static attributes

int YWakeUpSchedule::_parseAttr(yJsonStateMachine& j)
{
    if(!strcmp(j.token, "minutesA")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _minutesA =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "minutesB")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _minutesB =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "hours")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _hours =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "weekDays")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _weekDays =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "monthDays")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _monthDays =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "months")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _months =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "nextOccurence")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _nextOccurence =  atol(j.token);
        return 1;
    }
    failed:
    return YFunction::_parseAttr(j);
}


/**
 * Returns the minutes in the 00-29 interval of each hour scheduled for wake up.
 *
 * @return an integer corresponding to the minutes in the 00-29 interval of each hour scheduled for wake up
 *
 * On failure, throws an exception or returns Y_MINUTESA_INVALID.
 */
int YWakeUpSchedule::get_minutesA(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YWakeUpSchedule::MINUTESA_INVALID;
        }
    }
    return _minutesA;
}

/**
 * Changes the minutes in the 00-29 interval when a wake up must take place.
 *
 * @param newval : an integer corresponding to the minutes in the 00-29 interval when a wake up must take place
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YWakeUpSchedule::set_minutesA(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("minutesA", rest_val);
}

/**
 * Returns the minutes in the 30-59 intervalof each hour scheduled for wake up.
 *
 * @return an integer corresponding to the minutes in the 30-59 intervalof each hour scheduled for wake up
 *
 * On failure, throws an exception or returns Y_MINUTESB_INVALID.
 */
int YWakeUpSchedule::get_minutesB(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YWakeUpSchedule::MINUTESB_INVALID;
        }
    }
    return _minutesB;
}

/**
 * Changes the minutes in the 30-59 interval when a wake up must take place.
 *
 * @param newval : an integer corresponding to the minutes in the 30-59 interval when a wake up must take place
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YWakeUpSchedule::set_minutesB(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("minutesB", rest_val);
}

/**
 * Returns the hours scheduled for wake up.
 *
 * @return an integer corresponding to the hours scheduled for wake up
 *
 * On failure, throws an exception or returns Y_HOURS_INVALID.
 */
int YWakeUpSchedule::get_hours(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YWakeUpSchedule::HOURS_INVALID;
        }
    }
    return _hours;
}

/**
 * Changes the hours when a wake up must take place.
 *
 * @param newval : an integer corresponding to the hours when a wake up must take place
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YWakeUpSchedule::set_hours(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("hours", rest_val);
}

/**
 * Returns the days of the week scheduled for wake up.
 *
 * @return an integer corresponding to the days of the week scheduled for wake up
 *
 * On failure, throws an exception or returns Y_WEEKDAYS_INVALID.
 */
int YWakeUpSchedule::get_weekDays(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YWakeUpSchedule::WEEKDAYS_INVALID;
        }
    }
    return _weekDays;
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
int YWakeUpSchedule::set_weekDays(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("weekDays", rest_val);
}

/**
 * Returns the days of the month scheduled for wake up.
 *
 * @return an integer corresponding to the days of the month scheduled for wake up
 *
 * On failure, throws an exception or returns Y_MONTHDAYS_INVALID.
 */
int YWakeUpSchedule::get_monthDays(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YWakeUpSchedule::MONTHDAYS_INVALID;
        }
    }
    return _monthDays;
}

/**
 * Changes the days of the month when a wake up must take place.
 *
 * @param newval : an integer corresponding to the days of the month when a wake up must take place
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YWakeUpSchedule::set_monthDays(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("monthDays", rest_val);
}

/**
 * Returns the months scheduled for wake up.
 *
 * @return an integer corresponding to the months scheduled for wake up
 *
 * On failure, throws an exception or returns Y_MONTHS_INVALID.
 */
int YWakeUpSchedule::get_months(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YWakeUpSchedule::MONTHS_INVALID;
        }
    }
    return _months;
}

/**
 * Changes the months when a wake up must take place.
 *
 * @param newval : an integer corresponding to the months when a wake up must take place
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YWakeUpSchedule::set_months(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("months", rest_val);
}

/**
 * Returns the date/time (seconds) of the next wake up occurence
 *
 * @return an integer corresponding to the date/time (seconds) of the next wake up occurence
 *
 * On failure, throws an exception or returns Y_NEXTOCCURENCE_INVALID.
 */
s64 YWakeUpSchedule::get_nextOccurence(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YWakeUpSchedule::NEXTOCCURENCE_INVALID;
        }
    }
    return _nextOccurence;
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
 * Use the method YWakeUpSchedule.isOnline() to test if $THEFUNCTION$ is
 * indeed online at a given time. In case of ambiguity when looking for
 * $AFUNCTION$ by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * @param func : a string that uniquely characterizes $THEFUNCTION$
 *
 * @return a YWakeUpSchedule object allowing you to drive $THEFUNCTION$.
 */
YWakeUpSchedule* YWakeUpSchedule::FindWakeUpSchedule(string func)
{
    YWakeUpSchedule* obj = NULL;
    obj = (YWakeUpSchedule*) YFunction::_FindFromCache("WakeUpSchedule", func);
    if (obj == NULL) {
        obj = new YWakeUpSchedule(func);
        YFunction::_AddToCache("WakeUpSchedule", func, obj);
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
int YWakeUpSchedule::registerValueCallback(YWakeUpScheduleValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackWakeUpSchedule = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YWakeUpSchedule::_invokeValueCallback(string value)
{
    if (_valueCallbackWakeUpSchedule != NULL) {
        _valueCallbackWakeUpSchedule(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

/**
 * Returns all the minutes of each hour that are scheduled for wake up.
 */
s64 YWakeUpSchedule::get_minutes(void)
{
    s64 res = 0;
    // may throw an exception
    res = this->get_minutesB();
    res = ((res) << (30));
    res = res + this->get_minutesA();
    return res;
}

/**
 * Changes all the minutes where a wake up must take place.
 *
 * @param bitmap : Minutes 00-59 of each hour scheduled for wake up.
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YWakeUpSchedule::set_minutes(s64 bitmap)
{
    this->set_minutesA((int)(((bitmap) & (0x3fffffff))));
    bitmap = ((bitmap) >> (30));
    return this->set_minutesB((int)(((bitmap) & (0x3fffffff))));
}

YWakeUpSchedule *YWakeUpSchedule::nextWakeUpSchedule(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YWakeUpSchedule::FindWakeUpSchedule(hwid);
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
