/*********************************************************************
 *
 * $Id: yocto_hubport.cpp 22191 2015-12-02 06:49:31Z mvuilleu $
 *
 * Implements yFindHubPort(), the high-level API for HubPort functions
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
#include "yocto_hubport.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

YHubPort::YHubPort(const string& func): YFunction(func)
//--- (HubPort initialization)
    ,_enabled(ENABLED_INVALID)
    ,_portState(PORTSTATE_INVALID)
    ,_baudRate(BAUDRATE_INVALID)
    ,_valueCallbackHubPort(NULL)
//--- (end of HubPort initialization)
{
    _className="HubPort";
}

YHubPort::~YHubPort()
{
//--- (YHubPort cleanup)
//--- (end of YHubPort cleanup)
}
//--- (YHubPort implementation)
// static attributes

int YHubPort::_parseAttr(yJsonStateMachine& j)
{
    if(!strcmp(j.token, "enabled")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _enabled =  (Y_ENABLED_enum)atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "portState")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _portState =  (Y_PORTSTATE_enum)atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "baudRate")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _baudRate =  atoi(j.token);
        return 1;
    }
    failed:
    return YFunction::_parseAttr(j);
}


/**
 * Returns true if the Yocto-hub port is powered, false otherwise.
 *
 * @return either Y_ENABLED_FALSE or Y_ENABLED_TRUE, according to true if the Yocto-hub port is
 * powered, false otherwise
 *
 * On failure, throws an exception or returns Y_ENABLED_INVALID.
 */
Y_ENABLED_enum YHubPort::get_enabled(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YHubPort::ENABLED_INVALID;
        }
    }
    return _enabled;
}

/**
 * Changes the activation of the Yocto-hub port. If the port is enabled, the
 * connected module is powered. Otherwise, port power is shut down.
 *
 * @param newval : either Y_ENABLED_FALSE or Y_ENABLED_TRUE, according to the activation of the Yocto-hub port
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YHubPort::set_enabled(Y_ENABLED_enum newval)
{
    string rest_val;
    rest_val = (newval>0 ? "1" : "0");
    return _setAttr("enabled", rest_val);
}

/**
 * Returns the current state of the Yocto-hub port.
 *
 * @return a value among Y_PORTSTATE_OFF, Y_PORTSTATE_OVRLD, Y_PORTSTATE_ON, Y_PORTSTATE_RUN and
 * Y_PORTSTATE_PROG corresponding to the current state of the Yocto-hub port
 *
 * On failure, throws an exception or returns Y_PORTSTATE_INVALID.
 */
Y_PORTSTATE_enum YHubPort::get_portState(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YHubPort::PORTSTATE_INVALID;
        }
    }
    return _portState;
}

/**
 * Returns the current baud rate used by this Yocto-hub port, in kbps.
 * The default value is 1000 kbps, but a slower rate may be used if communication
 * problems are encountered.
 *
 * @return an integer corresponding to the current baud rate used by this Yocto-hub port, in kbps
 *
 * On failure, throws an exception or returns Y_BAUDRATE_INVALID.
 */
int YHubPort::get_baudRate(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YHubPort::BAUDRATE_INVALID;
        }
    }
    return _baudRate;
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
 * Use the method YHubPort.isOnline() to test if $THEFUNCTION$ is
 * indeed online at a given time. In case of ambiguity when looking for
 * $AFUNCTION$ by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * @param func : a string that uniquely characterizes $THEFUNCTION$
 *
 * @return a YHubPort object allowing you to drive $THEFUNCTION$.
 */
YHubPort* YHubPort::FindHubPort(string func)
{
    YHubPort* obj = NULL;
    obj = (YHubPort*) YFunction::_FindFromCache("HubPort", func);
    if (obj == NULL) {
        obj = new YHubPort(func);
        YFunction::_AddToCache("HubPort", func, obj);
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
int YHubPort::registerValueCallback(YHubPortValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackHubPort = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YHubPort::_invokeValueCallback(string value)
{
    if (_valueCallbackHubPort != NULL) {
        _valueCallbackHubPort(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

YHubPort *YHubPort::nextHubPort(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YHubPort::FindHubPort(hwid);
}

YHubPort* YHubPort::FirstHubPort(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("HubPort", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YHubPort::FindHubPort(serial+"."+funcId);
}

//--- (end of YHubPort implementation)

//--- (HubPort functions)
//--- (end of HubPort functions)
