/*********************************************************************
 *
 * $Id: yocto_dualpower.cpp 22191 2015-12-02 06:49:31Z mvuilleu $
 *
 * Implements yFindDualPower(), the high-level API for DualPower functions
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
#include "yocto_dualpower.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

YDualPower::YDualPower(const string& func): YFunction(func)
//--- (DualPower initialization)
    ,_powerState(POWERSTATE_INVALID)
    ,_powerControl(POWERCONTROL_INVALID)
    ,_extVoltage(EXTVOLTAGE_INVALID)
    ,_valueCallbackDualPower(NULL)
//--- (end of DualPower initialization)
{
    _className="DualPower";
}

YDualPower::~YDualPower()
{
//--- (YDualPower cleanup)
//--- (end of YDualPower cleanup)
}
//--- (YDualPower implementation)
// static attributes

int YDualPower::_parseAttr(yJsonStateMachine& j)
{
    if(!strcmp(j.token, "powerState")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _powerState =  (Y_POWERSTATE_enum)atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "powerControl")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _powerControl =  (Y_POWERCONTROL_enum)atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "extVoltage")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _extVoltage =  atoi(j.token);
        return 1;
    }
    failed:
    return YFunction::_parseAttr(j);
}


/**
 * Returns the current power source for module functions that require lots of current.
 *
 * @return a value among Y_POWERSTATE_OFF, Y_POWERSTATE_FROM_USB and Y_POWERSTATE_FROM_EXT
 * corresponding to the current power source for module functions that require lots of current
 *
 * On failure, throws an exception or returns Y_POWERSTATE_INVALID.
 */
Y_POWERSTATE_enum YDualPower::get_powerState(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YDualPower::POWERSTATE_INVALID;
        }
    }
    return _powerState;
}

/**
 * Returns the selected power source for module functions that require lots of current.
 *
 * @return a value among Y_POWERCONTROL_AUTO, Y_POWERCONTROL_FROM_USB, Y_POWERCONTROL_FROM_EXT and
 * Y_POWERCONTROL_OFF corresponding to the selected power source for module functions that require lots of current
 *
 * On failure, throws an exception or returns Y_POWERCONTROL_INVALID.
 */
Y_POWERCONTROL_enum YDualPower::get_powerControl(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YDualPower::POWERCONTROL_INVALID;
        }
    }
    return _powerControl;
}

/**
 * Changes the selected power source for module functions that require lots of current.
 *
 * @param newval : a value among Y_POWERCONTROL_AUTO, Y_POWERCONTROL_FROM_USB, Y_POWERCONTROL_FROM_EXT
 * and Y_POWERCONTROL_OFF corresponding to the selected power source for module functions that require
 * lots of current
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDualPower::set_powerControl(Y_POWERCONTROL_enum newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("powerControl", rest_val);
}

/**
 * Returns the measured voltage on the external power source, in millivolts.
 *
 * @return an integer corresponding to the measured voltage on the external power source, in millivolts
 *
 * On failure, throws an exception or returns Y_EXTVOLTAGE_INVALID.
 */
int YDualPower::get_extVoltage(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YDualPower::EXTVOLTAGE_INVALID;
        }
    }
    return _extVoltage;
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
 * Use the method YDualPower.isOnline() to test if $THEFUNCTION$ is
 * indeed online at a given time. In case of ambiguity when looking for
 * $AFUNCTION$ by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * @param func : a string that uniquely characterizes $THEFUNCTION$
 *
 * @return a YDualPower object allowing you to drive $THEFUNCTION$.
 */
YDualPower* YDualPower::FindDualPower(string func)
{
    YDualPower* obj = NULL;
    obj = (YDualPower*) YFunction::_FindFromCache("DualPower", func);
    if (obj == NULL) {
        obj = new YDualPower(func);
        YFunction::_AddToCache("DualPower", func, obj);
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
int YDualPower::registerValueCallback(YDualPowerValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackDualPower = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YDualPower::_invokeValueCallback(string value)
{
    if (_valueCallbackDualPower != NULL) {
        _valueCallbackDualPower(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

YDualPower *YDualPower::nextDualPower(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YDualPower::FindDualPower(hwid);
}

YDualPower* YDualPower::FirstDualPower(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("DualPower", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YDualPower::FindDualPower(serial+"."+funcId);
}

//--- (end of YDualPower implementation)

//--- (DualPower functions)
//--- (end of DualPower functions)
