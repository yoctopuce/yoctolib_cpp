/*********************************************************************
 *
 * $Id: yocto_realtimeclock.cpp 12324 2013-08-13 15:10:31Z mvuilleu $
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

//--- (YRealTimeClock constructor)
// Constructor is protected, use yFindRealTimeClock factory function to instantiate
YRealTimeClock::YRealTimeClock(const string& func): YFunction("RealTimeClock", func)
//--- (end of YRealTimeClock constructor)
//--- (RealTimeClock initialization)
            ,_callback(NULL)
            ,_logicalName(Y_LOGICALNAME_INVALID)
            ,_advertisedValue(Y_ADVERTISEDVALUE_INVALID)
            ,_unixTime(Y_UNIXTIME_INVALID)
            ,_dateTime(Y_DATETIME_INVALID)
            ,_utcOffset(Y_UTCOFFSET_INVALID)
            ,_timeSet(Y_TIMESET_INVALID)
//--- (end of RealTimeClock initialization)
{}

YRealTimeClock::~YRealTimeClock() 
{
//--- (YRealTimeClock cleanup)
//--- (end of YRealTimeClock cleanup)
}
//--- (YRealTimeClock implementation)

const string YRealTimeClock::LOGICALNAME_INVALID = "!INVALID!";
const string YRealTimeClock::ADVERTISEDVALUE_INVALID = "!INVALID!";
const string YRealTimeClock::DATETIME_INVALID = "!INVALID!";



int YRealTimeClock::_parse(yJsonStateMachine& j)
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
        } else if(!strcmp(j.token, "unixTime")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _unixTime =  atoi(j.token);
        } else if(!strcmp(j.token, "dateTime")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _dateTime =  _parseString(j);
        } else if(!strcmp(j.token, "utcOffset")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _utcOffset =  atoi(j.token);
        } else if(!strcmp(j.token, "timeSet")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _timeSet =  (Y_TIMESET_enum)atoi(j.token);
        } else {
            // ignore unknown field
            yJsonSkip(&j, 1);
        }
    }
    if(j.st != YJSON_PARSE_STRUCT) goto failed;
    return 0;
}

/**
 * Returns the logical name of the clock.
 * 
 * @return a string corresponding to the logical name of the clock
 * 
 * On failure, throws an exception or returns Y_LOGICALNAME_INVALID.
 */
string YRealTimeClock::get_logicalName(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_LOGICALNAME_INVALID;
    }
    return _logicalName;
}

/**
 * Changes the logical name of the clock. You can use yCheckLogicalName()
 * prior to this call to make sure that your parameter is valid.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 * 
 * @param newval : a string corresponding to the logical name of the clock
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YRealTimeClock::set_logicalName(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("logicalName", rest_val);
}

/**
 * Returns the current value of the clock (no more than 6 characters).
 * 
 * @return a string corresponding to the current value of the clock (no more than 6 characters)
 * 
 * On failure, throws an exception or returns Y_ADVERTISEDVALUE_INVALID.
 */
string YRealTimeClock::get_advertisedValue(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_ADVERTISEDVALUE_INVALID;
    }
    return _advertisedValue;
}

/**
 * Returns the current time in Unix format (number of elapsed seconds since Jan 1st, 1970).
 * 
 * @return an integer corresponding to the current time in Unix format (number of elapsed seconds
 * since Jan 1st, 1970)
 * 
 * On failure, throws an exception or returns Y_UNIXTIME_INVALID.
 */
unsigned YRealTimeClock::get_unixTime(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_UNIXTIME_INVALID;
    }
    return _unixTime;
}

/**
 * Changes the current time. Time is specifid in Unix format (number of elapsed seconds since Jan 1st, 1970).
 * If current UTC time is known, utcOffset will be automatically adjusted for the new specified time.
 * 
 * @param newval : an integer corresponding to the current time
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YRealTimeClock::set_unixTime(unsigned newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
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
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_DATETIME_INVALID;
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
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_UTCOFFSET_INVALID;
    }
    return _utcOffset;
}

/**
 * Changes the number of seconds between current time and UTC time (time zone).
 * The timezone is automatically rounded to the nearest multiple of 15 minutes.
 * If current UTC time is known, the current time will automatically be updated according to the
 * selected time zone.
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
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_TIMESET_INVALID;
    }
    return _timeSet;
}

YRealTimeClock *YRealTimeClock::nextRealTimeClock(void)
{
    string  hwid;
    
    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return yFindRealTimeClock(hwid);
}

void YRealTimeClock::registerValueCallback(YRealTimeClockUpdateCallback callback)
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

void YRealTimeClock::advertiseValue(const string& value)
{
    if (_callback != NULL) {
        _callback(this, value);
    }
}


YRealTimeClock* YRealTimeClock::FindRealTimeClock(const string& func)
{
    if(YAPI::_YFunctionsCaches["YRealTimeClock"].find(func) != YAPI::_YFunctionsCaches["YRealTimeClock"].end())
        return (YRealTimeClock*) YAPI::_YFunctionsCaches["YRealTimeClock"][func];
    
    YRealTimeClock *newRealTimeClock = new YRealTimeClock(func);
    YAPI::_YFunctionsCaches["YRealTimeClock"][func] = newRealTimeClock ;
    return newRealTimeClock;
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
