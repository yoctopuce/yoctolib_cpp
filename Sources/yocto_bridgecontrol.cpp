/*********************************************************************
 *
 * $Id: yocto_bridgecontrol.cpp 27180 2017-04-20 13:46:43Z seb $
 *
 * Implements yFindBridgeControl(), the high-level API for BridgeControl functions
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
#include "yocto_bridgecontrol.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#define  __FILE_ID__  "bridgecontrol"

YBridgeControl::YBridgeControl(const string& func): YFunction(func)
//--- (BridgeControl initialization)
    ,_excitationMode(EXCITATIONMODE_INVALID)
    ,_bridgeLatency(BRIDGELATENCY_INVALID)
    ,_adValue(ADVALUE_INVALID)
    ,_adGain(ADGAIN_INVALID)
    ,_valueCallbackBridgeControl(NULL)
//--- (end of BridgeControl initialization)
{
    _className="BridgeControl";
}

YBridgeControl::~YBridgeControl()
{
//--- (YBridgeControl cleanup)
//--- (end of YBridgeControl cleanup)
}
//--- (YBridgeControl implementation)
// static attributes

int YBridgeControl::_parseAttr(YJSONObject* json_val)
{
    if(json_val->has("excitationMode")) {
        _excitationMode =  (Y_EXCITATIONMODE_enum)json_val->getInt("excitationMode");
    }
    if(json_val->has("bridgeLatency")) {
        _bridgeLatency =  json_val->getInt("bridgeLatency");
    }
    if(json_val->has("adValue")) {
        _adValue =  json_val->getInt("adValue");
    }
    if(json_val->has("adGain")) {
        _adGain =  json_val->getInt("adGain");
    }
    return YFunction::_parseAttr(json_val);
}


/**
 * Returns the current Wheatstone bridge excitation method.
 *
 * @return a value among Y_EXCITATIONMODE_INTERNAL_AC, Y_EXCITATIONMODE_INTERNAL_DC and
 * Y_EXCITATIONMODE_EXTERNAL_DC corresponding to the current Wheatstone bridge excitation method
 *
 * On failure, throws an exception or returns Y_EXCITATIONMODE_INVALID.
 */
Y_EXCITATIONMODE_enum YBridgeControl::get_excitationMode(void)
{
    Y_EXCITATIONMODE_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YBridgeControl::EXCITATIONMODE_INVALID;
                }
            }
        }
        res = _excitationMode;
    } catch (std::exception) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the current Wheatstone bridge excitation method.
 *
 * @param newval : a value among Y_EXCITATIONMODE_INTERNAL_AC, Y_EXCITATIONMODE_INTERNAL_DC and
 * Y_EXCITATIONMODE_EXTERNAL_DC corresponding to the current Wheatstone bridge excitation method
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YBridgeControl::set_excitationMode(Y_EXCITATIONMODE_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
        res = _setAttr("excitationMode", rest_val);
    } catch (std::exception) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the current Wheatstone bridge excitation method.
 *
 * @return an integer corresponding to the current Wheatstone bridge excitation method
 *
 * On failure, throws an exception or returns Y_BRIDGELATENCY_INVALID.
 */
int YBridgeControl::get_bridgeLatency(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YBridgeControl::BRIDGELATENCY_INVALID;
                }
            }
        }
        res = _bridgeLatency;
    } catch (std::exception) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the current Wheatstone bridge excitation method.
 *
 * @param newval : an integer corresponding to the current Wheatstone bridge excitation method
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YBridgeControl::set_bridgeLatency(int newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
        res = _setAttr("bridgeLatency", rest_val);
    } catch (std::exception) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the raw value returned by the ratiometric A/D converter
 * during last read.
 *
 * @return an integer corresponding to the raw value returned by the ratiometric A/D converter
 *         during last read
 *
 * On failure, throws an exception or returns Y_ADVALUE_INVALID.
 */
int YBridgeControl::get_adValue(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YBridgeControl::ADVALUE_INVALID;
                }
            }
        }
        res = _adValue;
    } catch (std::exception) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the current ratiometric A/D converter gain. The gain is automatically
 * configured according to the signalRange set in the corresponding genericSensor.
 *
 * @return an integer corresponding to the current ratiometric A/D converter gain
 *
 * On failure, throws an exception or returns Y_ADGAIN_INVALID.
 */
int YBridgeControl::get_adGain(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YBridgeControl::ADGAIN_INVALID;
                }
            }
        }
        res = _adGain;
    } catch (std::exception) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Retrieves a Wheatstone bridge controller for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that the Wheatstone bridge controller is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method YBridgeControl.isOnline() to test if the Wheatstone bridge controller is
 * indeed online at a given time. In case of ambiguity when looking for
 * a Wheatstone bridge controller by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * @param func : a string that uniquely characterizes the Wheatstone bridge controller
 *
 * @return a YBridgeControl object allowing you to drive the Wheatstone bridge controller.
 */
YBridgeControl* YBridgeControl::FindBridgeControl(string func)
{
    YBridgeControl* obj = NULL;
    int taken = 0;
    if (YAPI::_apiInitialized) {
        yEnterCriticalSection(&YAPI::_global_cs);
        taken = 1;
    }try {
        obj = (YBridgeControl*) YFunction::_FindFromCache("BridgeControl", func);
        if (obj == NULL) {
            obj = new YBridgeControl(func);
            YFunction::_AddToCache("BridgeControl", func, obj);
        }
    } catch (std::exception) {
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
int YBridgeControl::registerValueCallback(YBridgeControlValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackBridgeControl = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YBridgeControl::_invokeValueCallback(string value)
{
    if (_valueCallbackBridgeControl != NULL) {
        _valueCallbackBridgeControl(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

YBridgeControl *YBridgeControl::nextBridgeControl(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YBridgeControl::FindBridgeControl(hwid);
}

YBridgeControl* YBridgeControl::FirstBridgeControl(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("BridgeControl", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YBridgeControl::FindBridgeControl(serial+"."+funcId);
}

//--- (end of YBridgeControl implementation)

//--- (BridgeControl functions)
//--- (end of BridgeControl functions)
