/*********************************************************************
 *
 * $Id: yocto_realtimeclock.cpp 22191 2015-12-02 06:49:31Z mvuilleu $
 *
 * Implements yFindRealTimeClock(), the high-level API for RealTimeClock functions
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
#include "yocto_realtimeclock.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

YRealTimeClock::YRealTimeClock(const string& func): YFunction(func)
//--- (RealTimeClock initialization)
    ,_unixTime(UNIXTIME_INVALID)
    ,_dateTime(DATETIME_INVALID)
    ,_utcOffset(UTCOFFSET_INVALID)
    ,_timeSet(TIMESET_INVALID)
    ,_valueCallbackRealTimeClock(NULL)
//--- (end of RealTimeClock initialization)
{
    _className="RealTimeClock";
}

YRealTimeClock::~YRealTimeClock()
{
//--- (YRealTimeClock cleanup)
//--- (end of YRealTimeClock cleanup)
}
//--- (YRealTimeClock implementation)
// static attributes
const string YRealTimeClock::DATETIME_INVALID = YAPI_INVALID_STRING;

int YRealTimeClock::_parseAttr(yJsonStateMachine& j)
{
    if(!strcmp(j.token, "unixTime")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _unixTime =  atol(j.token);
        return 1;
    }
    if(!strcmp(j.token, "dateTime")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _dateTime =  _parseString(j);
        return 1;
    }
    if(!strcmp(j.token, "utcOffset")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _utcOffset =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "timeSet")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _timeSet =  (Y_TIMESET_enum)atoi(j.token);
        return 1;
    }
    failed:
    return YFunction::_parseAttr(j);
}


/**
 * Returns the current time in Unix format (number of elapsed seconds since Jan 1st, 1970).
 *
 * @return an integer corresponding to the current time in Unix format (number of elapsed seconds
 * since Jan 1st, 1970)
 *
 * On failure, throws an exception or returns Y_UNIXTIME_INVALID.
 */
s64 YRealTimeClock::get_unixTime(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YRealTimeClock::UNIXTIME_INVALID;
        }
    }
    return _unixTime;
}

/**
 * Changes the current time. Time is specifid in Unix format (number of elapsed seconds since Jan 1st, 1970).
 *
 * @param newval : an integer corresponding to the current time
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YRealTimeClock::set_unixTime(s64 newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", (u32)newval); rest_val = string(buf);
    return _setAttr("unixTime", rest_val);
}

/**
 * Returns the current time in the form "YYYY/MM/DD hh:mm:ss"
 *
 * @return a string corresponding to the current time in the form "YYYY/MM/DD hh:mm:ss"
 *
 * On failure, throws an exception or returns Y_DATETIME_INVALID.
 */
string YRealTimeClock::get_dateTime(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YRealTimeClock::DATETIME_INVALID;
        }
    }
    return _dateTime;
}

/**
 * Returns the number of seconds between current time and UTC time (time zone).
 *
 * @return an integer corresponding to the number of seconds between current time and UTC time (time zone)
 *
 * On failure, throws an exception or returns Y_UTCOFFSET_INVALID.
 */
int YRealTimeClock::get_utcOffset(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YRealTimeClock::UTCOFFSET_INVALID;
        }
    }
    return _utcOffset;
}

/**
 * Changes the number of seconds between current time and UTC time (time zone).
 * The timezone is automatically rounded to the nearest multiple of 15 minutes.
 *
 * @param newval : an integer corresponding to the number of seconds between current time and UTC time (time zone)
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YRealTimeClock::set_utcOffset(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("utcOffset", rest_val);
}

/**
 * Returns true if the clock has been set, and false otherwise.
 *
 * @return either Y_TIMESET_FALSE or Y_TIMESET_TRUE, according to true if the clock has been set, and
 * false otherwise
 *
 * On failure, throws an exception or returns Y_TIMESET_INVALID.
 */
Y_TIMESET_enum YRealTimeClock::get_timeSet(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YRealTimeClock::TIMESET_INVALID;
        }
    }
    return _timeSet;
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
 * Use the method YRealTimeClock.isOnline() to test if $THEFUNCTION$ is
 * indeed online at a given time. In case of ambiguity when looking for
 * $AFUNCTION$ by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * @param func : a string that uniquely characterizes $THEFUNCTION$
 *
 * @return a YRealTimeClock object allowing you to drive $THEFUNCTION$.
 */
YRealTimeClock* YRealTimeClock::FindRealTimeClock(string func)
{
    YRealTimeClock* obj = NULL;
    obj = (YRealTimeClock*) YFunction::_FindFromCache("RealTimeClock", func);
    if (obj == NULL) {
        obj = new YRealTimeClock(func);
        YFunction::_AddToCache("RealTimeClock", func, obj);
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
int YRealTimeClock::registerValueCallback(YRealTimeClockValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackRealTimeClock = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YRealTimeClock::_invokeValueCallback(string value)
{
    if (_valueCallbackRealTimeClock != NULL) {
        _valueCallbackRealTimeClock(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

YRealTimeClock *YRealTimeClock::nextRealTimeClock(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YRealTimeClock::FindRealTimeClock(hwid);
}

YRealTimeClock* YRealTimeClock::FirstRealTimeClock(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("RealTimeClock", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YRealTimeClock::FindRealTimeClock(serial+"."+funcId);
}

//--- (end of YRealTimeClock implementation)

//--- (RealTimeClock functions)
//--- (end of RealTimeClock functions)
