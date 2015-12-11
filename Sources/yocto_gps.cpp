/*********************************************************************
 *
 * $Id: yocto_gps.cpp 22191 2015-12-02 06:49:31Z mvuilleu $
 *
 * Implements yFindGps(), the high-level API for Gps functions
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
#include "yocto_gps.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

YGps::YGps(const string& func): YFunction(func)
//--- (Gps initialization)
    ,_isFixed(ISFIXED_INVALID)
    ,_satCount(SATCOUNT_INVALID)
    ,_coordSystem(COORDSYSTEM_INVALID)
    ,_latitude(LATITUDE_INVALID)
    ,_longitude(LONGITUDE_INVALID)
    ,_dilution(DILUTION_INVALID)
    ,_altitude(ALTITUDE_INVALID)
    ,_groundSpeed(GROUNDSPEED_INVALID)
    ,_direction(DIRECTION_INVALID)
    ,_unixTime(UNIXTIME_INVALID)
    ,_dateTime(DATETIME_INVALID)
    ,_utcOffset(UTCOFFSET_INVALID)
    ,_command(COMMAND_INVALID)
    ,_valueCallbackGps(NULL)
//--- (end of Gps initialization)
{
    _className="Gps";
}

YGps::~YGps()
{
//--- (YGps cleanup)
//--- (end of YGps cleanup)
}
//--- (YGps implementation)
// static attributes
const string YGps::LATITUDE_INVALID = YAPI_INVALID_STRING;
const string YGps::LONGITUDE_INVALID = YAPI_INVALID_STRING;
const double YGps::DILUTION_INVALID = YAPI_INVALID_DOUBLE;
const double YGps::ALTITUDE_INVALID = YAPI_INVALID_DOUBLE;
const double YGps::GROUNDSPEED_INVALID = YAPI_INVALID_DOUBLE;
const double YGps::DIRECTION_INVALID = YAPI_INVALID_DOUBLE;
const string YGps::DATETIME_INVALID = YAPI_INVALID_STRING;
const string YGps::COMMAND_INVALID = YAPI_INVALID_STRING;

int YGps::_parseAttr(yJsonStateMachine& j)
{
    if(!strcmp(j.token, "isFixed")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _isFixed =  (Y_ISFIXED_enum)atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "satCount")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _satCount =  atol(j.token);
        return 1;
    }
    if(!strcmp(j.token, "coordSystem")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _coordSystem =  (Y_COORDSYSTEM_enum)atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "latitude")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _latitude =  _parseString(j);
        return 1;
    }
    if(!strcmp(j.token, "longitude")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _longitude =  _parseString(j);
        return 1;
    }
    if(!strcmp(j.token, "dilution")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _dilution =  floor(atof(j.token) * 1000.0 / 65536.0 + 0.5) / 1000.0;
        return 1;
    }
    if(!strcmp(j.token, "altitude")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _altitude =  floor(atof(j.token) * 1000.0 / 65536.0 + 0.5) / 1000.0;
        return 1;
    }
    if(!strcmp(j.token, "groundSpeed")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _groundSpeed =  floor(atof(j.token) * 1000.0 / 65536.0 + 0.5) / 1000.0;
        return 1;
    }
    if(!strcmp(j.token, "direction")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _direction =  floor(atof(j.token) * 1000.0 / 65536.0 + 0.5) / 1000.0;
        return 1;
    }
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
    if(!strcmp(j.token, "command")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _command =  _parseString(j);
        return 1;
    }
    failed:
    return YFunction::_parseAttr(j);
}


/**
 * Returns TRUE if the receiver has found enough satellites to work
 *
 * @return either Y_ISFIXED_FALSE or Y_ISFIXED_TRUE, according to TRUE if the receiver has found
 * enough satellites to work
 *
 * On failure, throws an exception or returns Y_ISFIXED_INVALID.
 */
Y_ISFIXED_enum YGps::get_isFixed(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YGps::ISFIXED_INVALID;
        }
    }
    return _isFixed;
}

/**
 * Returns the count of visible satellites.
 *
 * @return an integer corresponding to the count of visible satellites
 *
 * On failure, throws an exception or returns Y_SATCOUNT_INVALID.
 */
s64 YGps::get_satCount(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YGps::SATCOUNT_INVALID;
        }
    }
    return _satCount;
}

/**
 * Returns the representation system used for positioning data.
 *
 * @return a value among Y_COORDSYSTEM_GPS_DMS, Y_COORDSYSTEM_GPS_DM and Y_COORDSYSTEM_GPS_D
 * corresponding to the representation system used for positioning data
 *
 * On failure, throws an exception or returns Y_COORDSYSTEM_INVALID.
 */
Y_COORDSYSTEM_enum YGps::get_coordSystem(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YGps::COORDSYSTEM_INVALID;
        }
    }
    return _coordSystem;
}

/**
 * Changes the representation system used for positioning data.
 *
 * @param newval : a value among Y_COORDSYSTEM_GPS_DMS, Y_COORDSYSTEM_GPS_DM and Y_COORDSYSTEM_GPS_D
 * corresponding to the representation system used for positioning data
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YGps::set_coordSystem(Y_COORDSYSTEM_enum newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("coordSystem", rest_val);
}

/**
 * Returns the current latitude.
 *
 * @return a string corresponding to the current latitude
 *
 * On failure, throws an exception or returns Y_LATITUDE_INVALID.
 */
string YGps::get_latitude(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YGps::LATITUDE_INVALID;
        }
    }
    return _latitude;
}

/**
 * Returns the current longitude.
 *
 * @return a string corresponding to the current longitude
 *
 * On failure, throws an exception or returns Y_LONGITUDE_INVALID.
 */
string YGps::get_longitude(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YGps::LONGITUDE_INVALID;
        }
    }
    return _longitude;
}

/**
 * Returns the current horizontal dilution of precision,
 * the smaller that number is, the better .
 *
 * @return a floating point number corresponding to the current horizontal dilution of precision,
 *         the smaller that number is, the better
 *
 * On failure, throws an exception or returns Y_DILUTION_INVALID.
 */
double YGps::get_dilution(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YGps::DILUTION_INVALID;
        }
    }
    return _dilution;
}

/**
 * Returns the current altitude. Beware:  GPS technology
 * is very inaccurate regarding altitude.
 *
 * @return a floating point number corresponding to the current altitude
 *
 * On failure, throws an exception or returns Y_ALTITUDE_INVALID.
 */
double YGps::get_altitude(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YGps::ALTITUDE_INVALID;
        }
    }
    return _altitude;
}

/**
 * Returns the current ground speed in Km/h.
 *
 * @return a floating point number corresponding to the current ground speed in Km/h
 *
 * On failure, throws an exception or returns Y_GROUNDSPEED_INVALID.
 */
double YGps::get_groundSpeed(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YGps::GROUNDSPEED_INVALID;
        }
    }
    return _groundSpeed;
}

/**
 * Returns the current move bearing in degrees, zero
 * is the true (geographic) north.
 *
 * @return a floating point number corresponding to the current move bearing in degrees, zero
 *         is the true (geographic) north
 *
 * On failure, throws an exception or returns Y_DIRECTION_INVALID.
 */
double YGps::get_direction(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YGps::DIRECTION_INVALID;
        }
    }
    return _direction;
}

/**
 * Returns the current time in Unix format (number of
 * seconds elapsed since Jan 1st, 1970).
 *
 * @return an integer corresponding to the current time in Unix format (number of
 *         seconds elapsed since Jan 1st, 1970)
 *
 * On failure, throws an exception or returns Y_UNIXTIME_INVALID.
 */
s64 YGps::get_unixTime(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YGps::UNIXTIME_INVALID;
        }
    }
    return _unixTime;
}

/**
 * Returns the current time in the form "YYYY/MM/DD hh:mm:ss"
 *
 * @return a string corresponding to the current time in the form "YYYY/MM/DD hh:mm:ss"
 *
 * On failure, throws an exception or returns Y_DATETIME_INVALID.
 */
string YGps::get_dateTime(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YGps::DATETIME_INVALID;
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
int YGps::get_utcOffset(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YGps::UTCOFFSET_INVALID;
        }
    }
    return _utcOffset;
}

/**
 * Changes the number of seconds between current time and UTC time (time zone).
 * The timezone is automatically rounded to the nearest multiple of 15 minutes.
 * If current UTC time is known, the current time is automatically be updated according to the selected time zone.
 *
 * @param newval : an integer corresponding to the number of seconds between current time and UTC time (time zone)
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YGps::set_utcOffset(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("utcOffset", rest_val);
}

string YGps::get_command(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YGps::COMMAND_INVALID;
        }
    }
    return _command;
}

int YGps::set_command(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("command", rest_val);
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
 * Use the method YGps.isOnline() to test if $THEFUNCTION$ is
 * indeed online at a given time. In case of ambiguity when looking for
 * $AFUNCTION$ by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * @param func : a string that uniquely characterizes $THEFUNCTION$
 *
 * @return a YGps object allowing you to drive $THEFUNCTION$.
 */
YGps* YGps::FindGps(string func)
{
    YGps* obj = NULL;
    obj = (YGps*) YFunction::_FindFromCache("Gps", func);
    if (obj == NULL) {
        obj = new YGps(func);
        YFunction::_AddToCache("Gps", func, obj);
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
int YGps::registerValueCallback(YGpsValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackGps = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YGps::_invokeValueCallback(string value)
{
    if (_valueCallbackGps != NULL) {
        _valueCallbackGps(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

YGps *YGps::nextGps(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YGps::FindGps(hwid);
}

YGps* YGps::FirstGps(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("Gps", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YGps::FindGps(serial+"."+funcId);
}

//--- (end of YGps implementation)

//--- (Gps functions)
//--- (end of Gps functions)
