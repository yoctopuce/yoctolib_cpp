/*********************************************************************
 *
 * $Id: yocto_led.cpp 12324 2013-08-13 15:10:31Z mvuilleu $
 *
 * Implements yFindLed(), the high-level API for Led functions
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
#include "yocto_led.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

//--- (YLed constructor)
// Constructor is protected, use yFindLed factory function to instantiate
YLed::YLed(const string& func): YFunction("Led", func)
//--- (end of YLed constructor)
//--- (Led initialization)
            ,_callback(NULL)
            ,_logicalName(Y_LOGICALNAME_INVALID)
            ,_advertisedValue(Y_ADVERTISEDVALUE_INVALID)
            ,_power(Y_POWER_INVALID)
            ,_luminosity(Y_LUMINOSITY_INVALID)
            ,_blinking(Y_BLINKING_INVALID)
//--- (end of Led initialization)
{}

YLed::~YLed() 
{
//--- (YLed cleanup)
//--- (end of YLed cleanup)
}
//--- (YLed implementation)

const string YLed::LOGICALNAME_INVALID = "!INVALID!";
const string YLed::ADVERTISEDVALUE_INVALID = "!INVALID!";



int YLed::_parse(yJsonStateMachine& j)
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
        } else if(!strcmp(j.token, "power")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _power =  (Y_POWER_enum)atoi(j.token);
        } else if(!strcmp(j.token, "luminosity")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _luminosity =  atoi(j.token);
        } else if(!strcmp(j.token, "blinking")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _blinking =  (Y_BLINKING_enum)atoi(j.token);
        } else {
            // ignore unknown field
            yJsonSkip(&j, 1);
        }
    }
    if(j.st != YJSON_PARSE_STRUCT) goto failed;
    return 0;
}

/**
 * Returns the logical name of the led.
 * 
 * @return a string corresponding to the logical name of the led
 * 
 * On failure, throws an exception or returns Y_LOGICALNAME_INVALID.
 */
string YLed::get_logicalName(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_LOGICALNAME_INVALID;
    }
    return _logicalName;
}

/**
 * Changes the logical name of the led. You can use yCheckLogicalName()
 * prior to this call to make sure that your parameter is valid.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 * 
 * @param newval : a string corresponding to the logical name of the led
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YLed::set_logicalName(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("logicalName", rest_val);
}

/**
 * Returns the current value of the led (no more than 6 characters).
 * 
 * @return a string corresponding to the current value of the led (no more than 6 characters)
 * 
 * On failure, throws an exception or returns Y_ADVERTISEDVALUE_INVALID.
 */
string YLed::get_advertisedValue(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_ADVERTISEDVALUE_INVALID;
    }
    return _advertisedValue;
}

/**
 * Returns the current led state.
 * 
 * @return either Y_POWER_OFF or Y_POWER_ON, according to the current led state
 * 
 * On failure, throws an exception or returns Y_POWER_INVALID.
 */
Y_POWER_enum YLed::get_power(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_POWER_INVALID;
    }
    return _power;
}

/**
 * Changes the state of the led.
 * 
 * @param newval : either Y_POWER_OFF or Y_POWER_ON, according to the state of the led
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YLed::set_power(Y_POWER_enum newval)
{
    string rest_val;
    rest_val = (newval>0 ? "1" : "0");
    return _setAttr("power", rest_val);
}

/**
 * Returns the current led intensity (in per cent).
 * 
 * @return an integer corresponding to the current led intensity (in per cent)
 * 
 * On failure, throws an exception or returns Y_LUMINOSITY_INVALID.
 */
int YLed::get_luminosity(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_LUMINOSITY_INVALID;
    }
    return _luminosity;
}

/**
 * Changes the current led intensity (in per cent).
 * 
 * @param newval : an integer corresponding to the current led intensity (in per cent)
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YLed::set_luminosity(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
    return _setAttr("luminosity", rest_val);
}

/**
 * Returns the current led signaling mode.
 * 
 * @return a value among Y_BLINKING_STILL, Y_BLINKING_RELAX, Y_BLINKING_AWARE, Y_BLINKING_RUN,
 * Y_BLINKING_CALL and Y_BLINKING_PANIC corresponding to the current led signaling mode
 * 
 * On failure, throws an exception or returns Y_BLINKING_INVALID.
 */
Y_BLINKING_enum YLed::get_blinking(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_BLINKING_INVALID;
    }
    return _blinking;
}

/**
 * Changes the current led signaling mode.
 * 
 * @param newval : a value among Y_BLINKING_STILL, Y_BLINKING_RELAX, Y_BLINKING_AWARE, Y_BLINKING_RUN,
 * Y_BLINKING_CALL and Y_BLINKING_PANIC corresponding to the current led signaling mode
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YLed::set_blinking(Y_BLINKING_enum newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
    return _setAttr("blinking", rest_val);
}

YLed *YLed::nextLed(void)
{
    string  hwid;
    
    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return yFindLed(hwid);
}

void YLed::registerValueCallback(YLedUpdateCallback callback)
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

void YLed::advertiseValue(const string& value)
{
    if (_callback != NULL) {
        _callback(this, value);
    }
}


YLed* YLed::FindLed(const string& func)
{
    if(YAPI::_YFunctionsCaches["YLed"].find(func) != YAPI::_YFunctionsCaches["YLed"].end())
        return (YLed*) YAPI::_YFunctionsCaches["YLed"][func];
    
    YLed *newLed = new YLed(func);
    YAPI::_YFunctionsCaches["YLed"][func] = newLed ;
    return newLed;
}

YLed* YLed::FirstLed(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;
    
    if(YISERR(YapiWrapper::getFunctionsByClass("Led", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YLed::FindLed(serial+"."+funcId);
}

//--- (end of YLed implementation)

//--- (Led functions)
//--- (end of Led functions)
