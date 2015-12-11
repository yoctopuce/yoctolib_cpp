/*********************************************************************
 *
 * $Id: yocto_poweroutput.cpp 22191 2015-12-02 06:49:31Z mvuilleu $
 *
 * Implements yFindPowerOutput(), the high-level API for PowerOutput functions
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
#include "yocto_poweroutput.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

YPowerOutput::YPowerOutput(const string& func): YFunction(func)
//--- (PowerOutput initialization)
    ,_voltage(VOLTAGE_INVALID)
    ,_valueCallbackPowerOutput(NULL)
//--- (end of PowerOutput initialization)
{
    _className="PowerOutput";
}

YPowerOutput::~YPowerOutput()
{
//--- (YPowerOutput cleanup)
//--- (end of YPowerOutput cleanup)
}
//--- (YPowerOutput implementation)
// static attributes

int YPowerOutput::_parseAttr(yJsonStateMachine& j)
{
    if(!strcmp(j.token, "voltage")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _voltage =  (Y_VOLTAGE_enum)atoi(j.token);
        return 1;
    }
    failed:
    return YFunction::_parseAttr(j);
}


/**
 * Returns the voltage on the power ouput featured by
 * the module.
 *
 * @return a value among Y_VOLTAGE_OFF, Y_VOLTAGE_OUT3V3 and Y_VOLTAGE_OUT5V corresponding to the
 * voltage on the power ouput featured by
 *         the module
 *
 * On failure, throws an exception or returns Y_VOLTAGE_INVALID.
 */
Y_VOLTAGE_enum YPowerOutput::get_voltage(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YPowerOutput::VOLTAGE_INVALID;
        }
    }
    return _voltage;
}

/**
 * Changes the voltage on the power output provided by the
 * module. Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : a value among Y_VOLTAGE_OFF, Y_VOLTAGE_OUT3V3 and Y_VOLTAGE_OUT5V corresponding to
 * the voltage on the power output provided by the
 *         module
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YPowerOutput::set_voltage(Y_VOLTAGE_enum newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("voltage", rest_val);
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
 * Use the method YPowerOutput.isOnline() to test if $THEFUNCTION$ is
 * indeed online at a given time. In case of ambiguity when looking for
 * $AFUNCTION$ by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * @param func : a string that uniquely characterizes $THEFUNCTION$
 *
 * @return a YPowerOutput object allowing you to drive $THEFUNCTION$.
 */
YPowerOutput* YPowerOutput::FindPowerOutput(string func)
{
    YPowerOutput* obj = NULL;
    obj = (YPowerOutput*) YFunction::_FindFromCache("PowerOutput", func);
    if (obj == NULL) {
        obj = new YPowerOutput(func);
        YFunction::_AddToCache("PowerOutput", func, obj);
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
int YPowerOutput::registerValueCallback(YPowerOutputValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackPowerOutput = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YPowerOutput::_invokeValueCallback(string value)
{
    if (_valueCallbackPowerOutput != NULL) {
        _valueCallbackPowerOutput(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

YPowerOutput *YPowerOutput::nextPowerOutput(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YPowerOutput::FindPowerOutput(hwid);
}

YPowerOutput* YPowerOutput::FirstPowerOutput(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("PowerOutput", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YPowerOutput::FindPowerOutput(serial+"."+funcId);
}

//--- (end of YPowerOutput implementation)

//--- (PowerOutput functions)
//--- (end of PowerOutput functions)
