/*********************************************************************
 *
 *  $Id: yocto_realtimeclock.cpp 54332 2023-05-02 08:35:37Z seb $
 *
 *  Implements yFindRealTimeClock(), the high-level API for RealTimeClock functions
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

#include "yocto_realtimeclock.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#define  __FILE_ID__  "realtimeclock"

#ifdef YOCTOLIB_NAMESPACE
using namespace YOCTOLIB_NAMESPACE;
#endif

YRealTimeClock::YRealTimeClock(const string& func): YFunction(func)
//--- (YRealTimeClock initialization)
    ,_unixTime(UNIXTIME_INVALID)
    ,_dateTime(DATETIME_INVALID)
    ,_utcOffset(UTCOFFSET_INVALID)
    ,_timeSet(TIMESET_INVALID)
    ,_disableHostSync(DISABLEHOSTSYNC_INVALID)
    ,_valueCallbackRealTimeClock(NULL)
//--- (end of YRealTimeClock initialization)
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

int YRealTimeClock::_parseAttr(YJSONObject *json_val)
{
    if(json_val->has("unixTime")) {
        _unixTime =  json_val->getLong("unixTime");
    }
    if(json_val->has("dateTime")) {
        _dateTime =  json_val->getString("dateTime");
    }
    if(json_val->has("utcOffset")) {
        _utcOffset =  json_val->getInt("utcOffset");
    }
    if(json_val->has("timeSet")) {
        _timeSet =  (Y_TIMESET_enum)json_val->getInt("timeSet");
    }
    if(json_val->has("disableHostSync")) {
        _disableHostSync =  (Y_DISABLEHOSTSYNC_enum)json_val->getInt("disableHostSync");
    }
    return YFunction::_parseAttr(json_val);
}


/**
 * Returns the current time in Unix format (number of elapsed seconds since Jan 1st, 1970).
 *
 * @return an integer corresponding to the current time in Unix format (number of elapsed seconds
 * since Jan 1st, 1970)
 *
 * On failure, throws an exception or returns YRealTimeClock::UNIXTIME_INVALID.
 */
s64 YRealTimeClock::get_unixTime(void)
{
    s64 res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YRealTimeClock::UNIXTIME_INVALID;
                }
            }
        }
        res = _unixTime;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the current time. Time is specifid in Unix format (number of elapsed seconds since Jan 1st, 1970).
 *
 * @param newval : an integer corresponding to the current time
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YRealTimeClock::set_unixTime(s64 newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%u", (u32)newval); rest_val = string(buf);
        res = _setAttr("unixTime", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the current time in the form "YYYY/MM/DD hh:mm:ss".
 *
 * @return a string corresponding to the current time in the form "YYYY/MM/DD hh:mm:ss"
 *
 * On failure, throws an exception or returns YRealTimeClock::DATETIME_INVALID.
 */
string YRealTimeClock::get_dateTime(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YRealTimeClock::DATETIME_INVALID;
                }
            }
        }
        res = _dateTime;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the number of seconds between current time and UTC time (time zone).
 *
 * @return an integer corresponding to the number of seconds between current time and UTC time (time zone)
 *
 * On failure, throws an exception or returns YRealTimeClock::UTCOFFSET_INVALID.
 */
int YRealTimeClock::get_utcOffset(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YRealTimeClock::UTCOFFSET_INVALID;
                }
            }
        }
        res = _utcOffset;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the number of seconds between current time and UTC time (time zone).
 * The timezone is automatically rounded to the nearest multiple of 15 minutes.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : an integer corresponding to the number of seconds between current time and UTC time (time zone)
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YRealTimeClock::set_utcOffset(int newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("utcOffset", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns true if the clock has been set, and false otherwise.
 *
 * @return either YRealTimeClock::TIMESET_FALSE or YRealTimeClock::TIMESET_TRUE, according to true if
 * the clock has been set, and false otherwise
 *
 * On failure, throws an exception or returns YRealTimeClock::TIMESET_INVALID.
 */
Y_TIMESET_enum YRealTimeClock::get_timeSet(void)
{
    Y_TIMESET_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YRealTimeClock::TIMESET_INVALID;
                }
            }
        }
        res = _timeSet;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns true if the automatic clock synchronization with host has been disabled,
 * and false otherwise.
 *
 * @return either YRealTimeClock::DISABLEHOSTSYNC_FALSE or YRealTimeClock::DISABLEHOSTSYNC_TRUE,
 * according to true if the automatic clock synchronization with host has been disabled,
 *         and false otherwise
 *
 * On failure, throws an exception or returns YRealTimeClock::DISABLEHOSTSYNC_INVALID.
 */
Y_DISABLEHOSTSYNC_enum YRealTimeClock::get_disableHostSync(void)
{
    Y_DISABLEHOSTSYNC_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YRealTimeClock::DISABLEHOSTSYNC_INVALID;
                }
            }
        }
        res = _disableHostSync;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the automatic clock synchronization with host working state.
 * To disable automatic synchronization, set the value to true.
 * To enable automatic synchronization (default), set the value to false.
 *
 * If you want the change to be kept after a device reboot,
 * make sure  to call the matching module saveToFlash().
 *
 * @param newval : either YRealTimeClock::DISABLEHOSTSYNC_FALSE or YRealTimeClock::DISABLEHOSTSYNC_TRUE,
 * according to the automatic clock synchronization with host working state
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YRealTimeClock::set_disableHostSync(Y_DISABLEHOSTSYNC_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = (newval>0 ? "1" : "0");
        res = _setAttr("disableHostSync", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Retrieves a real-time clock for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that the real-time clock is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the real-time clock is
 * indeed online at a given time. In case of ambiguity when looking for
 * a real-time clock by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the real-time clock, for instance
 *         YHUBGSM5.realTimeClock.
 *
 * @return a YRealTimeClock object allowing you to drive the real-time clock.
 */
YRealTimeClock* YRealTimeClock::FindRealTimeClock(string func)
{
    YRealTimeClock* obj = NULL;
    int taken = 0;
    if (YAPI::_apiInitialized) {
        yEnterCriticalSection(&YAPI::_global_cs);
        taken = 1;
    }try {
        obj = (YRealTimeClock*) YFunction::_FindFromCache("RealTimeClock", func);
        if (obj == NULL) {
            obj = new YRealTimeClock(func);
            YFunction::_AddToCache("RealTimeClock", func, obj);
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

YRealTimeClock *YRealTimeClock::FirstRealTimeClock(void)
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

//--- (YRealTimeClock functions)
//--- (end of YRealTimeClock functions)
