/*********************************************************************
 *
 * $Id: yocto_led.cpp 22191 2015-12-02 06:49:31Z mvuilleu $
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

YLed::YLed(const string& func): YFunction(func)
//--- (Led initialization)
    ,_power(POWER_INVALID)
    ,_luminosity(LUMINOSITY_INVALID)
    ,_blinking(BLINKING_INVALID)
    ,_valueCallbackLed(NULL)
//--- (end of Led initialization)
{
    _className="Led";
}

YLed::~YLed()
{
//--- (YLed cleanup)
//--- (end of YLed cleanup)
}
//--- (YLed implementation)
// static attributes

int YLed::_parseAttr(yJsonStateMachine& j)
{
    if(!strcmp(j.token, "power")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _power =  (Y_POWER_enum)atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "luminosity")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _luminosity =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "blinking")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _blinking =  (Y_BLINKING_enum)atoi(j.token);
        return 1;
    }
    failed:
    return YFunction::_parseAttr(j);
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
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YLed::POWER_INVALID;
        }
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
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YLed::LUMINOSITY_INVALID;
        }
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
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
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
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YLed::BLINKING_INVALID;
        }
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
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("blinking", rest_val);
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
 * Use the method YLed.isOnline() to test if $THEFUNCTION$ is
 * indeed online at a given time. In case of ambiguity when looking for
 * $AFUNCTION$ by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * @param func : a string that uniquely characterizes $THEFUNCTION$
 *
 * @return a YLed object allowing you to drive $THEFUNCTION$.
 */
YLed* YLed::FindLed(string func)
{
    YLed* obj = NULL;
    obj = (YLed*) YFunction::_FindFromCache("Led", func);
    if (obj == NULL) {
        obj = new YLed(func);
        YFunction::_AddToCache("Led", func, obj);
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
int YLed::registerValueCallback(YLedValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackLed = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YLed::_invokeValueCallback(string value)
{
    if (_valueCallbackLed != NULL) {
        _valueCallbackLed(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

YLed *YLed::nextLed(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YLed::FindLed(hwid);
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
