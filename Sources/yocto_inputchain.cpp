/*********************************************************************
 *
 *  $Id: svn_id $
 *
 *  Implements yFindInputChain(), the high-level API for InputChain functions
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

#include "yocto_inputchain.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#define  __FILE_ID__  "inputchain"

#ifdef YOCTOLIB_NAMESPACE
using namespace YOCTOLIB_NAMESPACE;
#endif

YInputChain::YInputChain(const string& func): YFunction(func)
//--- (YInputChain initialization)
    ,_expectedNodes(EXPECTEDNODES_INVALID)
    ,_detectedNodes(DETECTEDNODES_INVALID)
    ,_loopbackTest(LOOPBACKTEST_INVALID)
    ,_refreshRate(REFRESHRATE_INVALID)
    ,_bitChain1(BITCHAIN1_INVALID)
    ,_bitChain2(BITCHAIN2_INVALID)
    ,_bitChain3(BITCHAIN3_INVALID)
    ,_bitChain4(BITCHAIN4_INVALID)
    ,_bitChain5(BITCHAIN5_INVALID)
    ,_bitChain6(BITCHAIN6_INVALID)
    ,_bitChain7(BITCHAIN7_INVALID)
    ,_watchdogPeriod(WATCHDOGPERIOD_INVALID)
    ,_chainDiags(CHAINDIAGS_INVALID)
    ,_valueCallbackInputChain(NULL)
    ,_stateChangeCallback(NULL)
    ,_prevPos(0)
    ,_eventPos(0)
    ,_eventStamp(0)
//--- (end of YInputChain initialization)
{
    _className="InputChain";
}

YInputChain::~YInputChain()
{
//--- (YInputChain cleanup)
//--- (end of YInputChain cleanup)
}
//--- (YInputChain implementation)
void YInputChain::yInternalEventCallback(YInputChain *obj, const string& value)
{
    obj->_internalEventHandler(value);
}

// static attributes
const string YInputChain::BITCHAIN1_INVALID = YAPI_INVALID_STRING;
const string YInputChain::BITCHAIN2_INVALID = YAPI_INVALID_STRING;
const string YInputChain::BITCHAIN3_INVALID = YAPI_INVALID_STRING;
const string YInputChain::BITCHAIN4_INVALID = YAPI_INVALID_STRING;
const string YInputChain::BITCHAIN5_INVALID = YAPI_INVALID_STRING;
const string YInputChain::BITCHAIN6_INVALID = YAPI_INVALID_STRING;
const string YInputChain::BITCHAIN7_INVALID = YAPI_INVALID_STRING;

int YInputChain::_parseAttr(YJSONObject *json_val)
{
    if(json_val->has("expectedNodes")) {
        _expectedNodes =  json_val->getInt("expectedNodes");
    }
    if(json_val->has("detectedNodes")) {
        _detectedNodes =  json_val->getInt("detectedNodes");
    }
    if(json_val->has("loopbackTest")) {
        _loopbackTest =  (Y_LOOPBACKTEST_enum)json_val->getInt("loopbackTest");
    }
    if(json_val->has("refreshRate")) {
        _refreshRate =  json_val->getInt("refreshRate");
    }
    if(json_val->has("bitChain1")) {
        _bitChain1 =  json_val->getString("bitChain1");
    }
    if(json_val->has("bitChain2")) {
        _bitChain2 =  json_val->getString("bitChain2");
    }
    if(json_val->has("bitChain3")) {
        _bitChain3 =  json_val->getString("bitChain3");
    }
    if(json_val->has("bitChain4")) {
        _bitChain4 =  json_val->getString("bitChain4");
    }
    if(json_val->has("bitChain5")) {
        _bitChain5 =  json_val->getString("bitChain5");
    }
    if(json_val->has("bitChain6")) {
        _bitChain6 =  json_val->getString("bitChain6");
    }
    if(json_val->has("bitChain7")) {
        _bitChain7 =  json_val->getString("bitChain7");
    }
    if(json_val->has("watchdogPeriod")) {
        _watchdogPeriod =  json_val->getInt("watchdogPeriod");
    }
    if(json_val->has("chainDiags")) {
        _chainDiags =  json_val->getInt("chainDiags");
    }
    return YFunction::_parseAttr(json_val);
}


/**
 * Returns the number of nodes expected in the chain.
 *
 * @return an integer corresponding to the number of nodes expected in the chain
 *
 * On failure, throws an exception or returns YInputChain::EXPECTEDNODES_INVALID.
 */
int YInputChain::get_expectedNodes(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YInputChain::EXPECTEDNODES_INVALID;
                }
            }
        }
        res = _expectedNodes;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the number of nodes expected in the chain.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : an integer corresponding to the number of nodes expected in the chain
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YInputChain::set_expectedNodes(int newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("expectedNodes", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the number of nodes detected in the chain.
 *
 * @return an integer corresponding to the number of nodes detected in the chain
 *
 * On failure, throws an exception or returns YInputChain::DETECTEDNODES_INVALID.
 */
int YInputChain::get_detectedNodes(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YInputChain::DETECTEDNODES_INVALID;
                }
            }
        }
        res = _detectedNodes;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the activation state of the exhaustive chain connectivity test.
 * The connectivity test requires a cable connecting the end of the chain
 * to the loopback test connector.
 *
 * @return either YInputChain::LOOPBACKTEST_OFF or YInputChain::LOOPBACKTEST_ON, according to the
 * activation state of the exhaustive chain connectivity test
 *
 * On failure, throws an exception or returns YInputChain::LOOPBACKTEST_INVALID.
 */
Y_LOOPBACKTEST_enum YInputChain::get_loopbackTest(void)
{
    Y_LOOPBACKTEST_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YInputChain::LOOPBACKTEST_INVALID;
                }
            }
        }
        res = _loopbackTest;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the activation state of the exhaustive chain connectivity test.
 * The connectivity test requires a cable connecting the end of the chain
 * to the loopback test connector.
 *
 * If you want the change to be kept after a device reboot,
 * make sure  to call the matching module saveToFlash().
 *
 * @param newval : either YInputChain::LOOPBACKTEST_OFF or YInputChain::LOOPBACKTEST_ON, according to
 * the activation state of the exhaustive chain connectivity test
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YInputChain::set_loopbackTest(Y_LOOPBACKTEST_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = (newval>0 ? "1" : "0");
        res = _setAttr("loopbackTest", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the desired refresh rate, measured in Hz.
 * The higher the refresh rate is set, the higher the
 * communication speed on the chain will be.
 *
 * @return an integer corresponding to the desired refresh rate, measured in Hz
 *
 * On failure, throws an exception or returns YInputChain::REFRESHRATE_INVALID.
 */
int YInputChain::get_refreshRate(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YInputChain::REFRESHRATE_INVALID;
                }
            }
        }
        res = _refreshRate;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the desired refresh rate, measured in Hz.
 * The higher the refresh rate is set, the higher the
 * communication speed on the chain will be.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : an integer corresponding to the desired refresh rate, measured in Hz
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YInputChain::set_refreshRate(int newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("refreshRate", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the state of input 1 for all nodes of the input chain,
 * as a hexadecimal string. The node nearest to the controller
 * is the lowest bit of the result.
 *
 * @return a string corresponding to the state of input 1 for all nodes of the input chain,
 *         as a hexadecimal string
 *
 * On failure, throws an exception or returns YInputChain::BITCHAIN1_INVALID.
 */
string YInputChain::get_bitChain1(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YInputChain::BITCHAIN1_INVALID;
                }
            }
        }
        res = _bitChain1;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the state of input 2 for all nodes of the input chain,
 * as a hexadecimal string. The node nearest to the controller
 * is the lowest bit of the result.
 *
 * @return a string corresponding to the state of input 2 for all nodes of the input chain,
 *         as a hexadecimal string
 *
 * On failure, throws an exception or returns YInputChain::BITCHAIN2_INVALID.
 */
string YInputChain::get_bitChain2(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YInputChain::BITCHAIN2_INVALID;
                }
            }
        }
        res = _bitChain2;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the state of input 3 for all nodes of the input chain,
 * as a hexadecimal string. The node nearest to the controller
 * is the lowest bit of the result.
 *
 * @return a string corresponding to the state of input 3 for all nodes of the input chain,
 *         as a hexadecimal string
 *
 * On failure, throws an exception or returns YInputChain::BITCHAIN3_INVALID.
 */
string YInputChain::get_bitChain3(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YInputChain::BITCHAIN3_INVALID;
                }
            }
        }
        res = _bitChain3;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the state of input 4 for all nodes of the input chain,
 * as a hexadecimal string. The node nearest to the controller
 * is the lowest bit of the result.
 *
 * @return a string corresponding to the state of input 4 for all nodes of the input chain,
 *         as a hexadecimal string
 *
 * On failure, throws an exception or returns YInputChain::BITCHAIN4_INVALID.
 */
string YInputChain::get_bitChain4(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YInputChain::BITCHAIN4_INVALID;
                }
            }
        }
        res = _bitChain4;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the state of input 5 for all nodes of the input chain,
 * as a hexadecimal string. The node nearest to the controller
 * is the lowest bit of the result.
 *
 * @return a string corresponding to the state of input 5 for all nodes of the input chain,
 *         as a hexadecimal string
 *
 * On failure, throws an exception or returns YInputChain::BITCHAIN5_INVALID.
 */
string YInputChain::get_bitChain5(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YInputChain::BITCHAIN5_INVALID;
                }
            }
        }
        res = _bitChain5;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the state of input 6 for all nodes of the input chain,
 * as a hexadecimal string. The node nearest to the controller
 * is the lowest bit of the result.
 *
 * @return a string corresponding to the state of input 6 for all nodes of the input chain,
 *         as a hexadecimal string
 *
 * On failure, throws an exception or returns YInputChain::BITCHAIN6_INVALID.
 */
string YInputChain::get_bitChain6(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YInputChain::BITCHAIN6_INVALID;
                }
            }
        }
        res = _bitChain6;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the state of input 7 for all nodes of the input chain,
 * as a hexadecimal string. The node nearest to the controller
 * is the lowest bit of the result.
 *
 * @return a string corresponding to the state of input 7 for all nodes of the input chain,
 *         as a hexadecimal string
 *
 * On failure, throws an exception or returns YInputChain::BITCHAIN7_INVALID.
 */
string YInputChain::get_bitChain7(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YInputChain::BITCHAIN7_INVALID;
                }
            }
        }
        res = _bitChain7;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the wait time in seconds before triggering an inactivity
 * timeout error.
 *
 * @return an integer corresponding to the wait time in seconds before triggering an inactivity
 *         timeout error
 *
 * On failure, throws an exception or returns YInputChain::WATCHDOGPERIOD_INVALID.
 */
int YInputChain::get_watchdogPeriod(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YInputChain::WATCHDOGPERIOD_INVALID;
                }
            }
        }
        res = _watchdogPeriod;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the wait time in seconds before triggering an inactivity
 * timeout error. Remember to call the saveToFlash() method
 * of the module if the modification must be kept.
 *
 * @param newval : an integer corresponding to the wait time in seconds before triggering an inactivity
 *         timeout error
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YInputChain::set_watchdogPeriod(int newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("watchdogPeriod", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the controller state diagnostics. Bit 0 indicates a chain length
 * error, bit 1 indicates an inactivity timeout and bit 2 indicates
 * a loopback test failure.
 *
 * @return an integer corresponding to the controller state diagnostics
 *
 * On failure, throws an exception or returns YInputChain::CHAINDIAGS_INVALID.
 */
int YInputChain::get_chainDiags(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YInputChain::CHAINDIAGS_INVALID;
                }
            }
        }
        res = _chainDiags;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Retrieves a digital input chain for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that the digital input chain is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the digital input chain is
 * indeed online at a given time. In case of ambiguity when looking for
 * a digital input chain by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the digital input chain, for instance
 *         MyDevice.inputChain.
 *
 * @return a YInputChain object allowing you to drive the digital input chain.
 */
YInputChain* YInputChain::FindInputChain(string func)
{
    YInputChain* obj = NULL;
    int taken = 0;
    if (YAPI::_apiInitialized) {
        yEnterCriticalSection(&YAPI::_global_cs);
        taken = 1;
    }try {
        obj = (YInputChain*) YFunction::_FindFromCache("InputChain", func);
        if (obj == NULL) {
            obj = new YInputChain(func);
            YFunction::_AddToCache("InputChain", func, obj);
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
int YInputChain::registerValueCallback(YInputChainValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackInputChain = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YInputChain::_invokeValueCallback(string value)
{
    if (_valueCallbackInputChain != NULL) {
        _valueCallbackInputChain(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

/**
 * Resets the application watchdog countdown.
 * If you have setup a non-zero watchdogPeriod, you should
 * call this function on a regular basis to prevent the application
 * inactivity error to be triggered.
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YInputChain::resetWatchdog(void)
{
    return this->set_watchdogPeriod(-1);
}

/**
 * Returns a string with last events observed on the digital input chain.
 * This method return only events that are still buffered in the device memory.
 *
 * @return a string with last events observed (one per line).
 *
 * On failure, throws an exception or returns  YAPI::INVALID_STRING.
 */
string YInputChain::get_lastEvents(void)
{
    string content;

    content = this->_download("events.txt");
    return content;
}

/**
 * Registers a callback function to be called each time that an event is detected on the
 * input chain.The callback is invoked only during the execution of
 * ySleep or yHandleEvents. This provides control over the time when
 * the callback is triggered. For good responsiveness, remember to call one of these
 * two functions periodically. To unregister a callback, pass a NULL pointer as argument.
 *
 * @param callback : the callback function to call, or a NULL pointer.
 *         The callback function should take four arguments:
 *         the YInputChain object that emitted the event, the
 *         UTC timestamp of the event, a character string describing
 *         the type of event and a character string with the event data.
 *         On failure, throws an exception or returns a negative error code.
 */
int YInputChain::registerStateChangeCallback(YStateChangeCallback callback)
{
    if (callback != NULL) {
        this->registerValueCallback(yInternalEventCallback);
    } else {
        this->registerValueCallback((YInputChainValueCallback) NULL);
    }
    // register user callback AFTER the internal pseudo-event,
    // to make sure we start with future events only
    _stateChangeCallback = callback;
    return 0;
}

int YInputChain::_internalEventHandler(string cbpos)
{
    int newPos = 0;
    string url;
    string content;
    string contentStr;
    vector<string> eventArr;
    int arrLen = 0;
    string lenStr;
    int arrPos = 0;
    string eventStr;
    int eventLen = 0;
    string hexStamp;
    int typePos = 0;
    int dataPos = 0;
    int evtStamp = 0;
    string evtType;
    string evtData;
    string evtChange;
    int chainIdx = 0;
    newPos = atoi((cbpos).c_str());
    if (newPos < _prevPos) {
        _eventPos = 0;
    }
    _prevPos = newPos;
    if (newPos < _eventPos) {
        return YAPI_SUCCESS;
    }
    if (!(_stateChangeCallback != NULL)) {
        // first simulated event, use it to initialize reference values
        _eventPos = newPos;
        _eventChains.clear();
        _eventChains.push_back(this->get_bitChain1());
        _eventChains.push_back(this->get_bitChain2());
        _eventChains.push_back(this->get_bitChain3());
        _eventChains.push_back(this->get_bitChain4());
        _eventChains.push_back(this->get_bitChain5());
        _eventChains.push_back(this->get_bitChain6());
        _eventChains.push_back(this->get_bitChain7());
        return YAPI_SUCCESS;
    }
    url = YapiWrapper::ysprintf("events.txt?pos=%d",_eventPos);

    content = this->_download(url);
    contentStr = content;
    eventArr = _strsplit(contentStr,'\n');
    arrLen = (int)eventArr.size();
    if (!(arrLen > 0)) {
        _throw((YRETCODE)(YAPI_IO_ERROR), "fail to download events");
        return YAPI_IO_ERROR;
    }
    // last element of array is the new position preceeded by '@'
    arrLen = arrLen - 1;
    lenStr = eventArr[arrLen];
    lenStr = (lenStr).substr(1, (int)(lenStr).length()-1);
    // update processed event position pointer
    _eventPos = atoi((lenStr).c_str());
    // now generate callbacks for each event received
    arrPos = 0;
    while (arrPos < arrLen) {
        eventStr = eventArr[arrPos];
        eventLen = (int)(eventStr).length();
        if (eventLen >= 1) {
            hexStamp = (eventStr).substr(0, 8);
            evtStamp = (int)YAPI::_hexStr2Long(hexStamp);
            typePos = _ystrpos(eventStr, ":")+1;
            if ((evtStamp >= _eventStamp) && (typePos > 8)) {
                _eventStamp = evtStamp;
                dataPos = _ystrpos(eventStr, "=")+1;
                evtType = (eventStr).substr(typePos, 1);
                evtData = "";
                evtChange = "";
                if (dataPos > 10) {
                    evtData = (eventStr).substr(dataPos, (int)(eventStr).length()-dataPos);
                    if (_ystrpos("1234567", evtType) >= 0) {
                        chainIdx = atoi((evtType).c_str()) - 1;
                        evtChange = this->_strXor(evtData, _eventChains[chainIdx]);
                        _eventChains[chainIdx] = evtData;
                    }
                }
                _stateChangeCallback(this, evtStamp, evtType, evtData, evtChange);
            }
        }
        arrPos = arrPos + 1;
    }
    return YAPI_SUCCESS;
}

string YInputChain::_strXor(string a,string b)
{
    int lenA = 0;
    int lenB = 0;
    string res;
    int idx = 0;
    int digitA = 0;
    int digitB = 0;
    // make sure the result has the same length as first argument
    lenA = (int)(a).length();
    lenB = (int)(b).length();
    if (lenA > lenB) {
        res = (a).substr(0, lenA-lenB);
        a = (a).substr(lenA-lenB, lenB);
        lenA = lenB;
    } else {
        res = "";
        b = (b).substr(lenA-lenB, lenA);
    }
    // scan strings and compare digit by digit
    idx = 0;
    while (idx < lenA) {
        digitA = (int)YAPI::_hexStr2Long((a).substr(idx, 1));
        digitB = (int)YAPI::_hexStr2Long((b).substr(idx, 1));
        res = YapiWrapper::ysprintf("%s%x", res.c_str(),((digitA) ^ (digitB)));
        idx = idx + 1;
    }
    return res;
}

vector<int> YInputChain::hex2array(string hexstr)
{
    int hexlen = 0;
    vector<int> res;
    int idx = 0;
    int digit = 0;
    hexlen = (int)(hexstr).length();
    res.clear();
    idx = hexlen;
    while (idx > 0) {
        idx = idx - 1;
        digit = (int)YAPI::_hexStr2Long((hexstr).substr(idx, 1));
        res.push_back(((digit) & (1)));
        res.push_back(((((digit) >> (1))) & (1)));
        res.push_back(((((digit) >> (2))) & (1)));
        res.push_back(((((digit) >> (3))) & (1)));
    }
    return res;
}

YInputChain *YInputChain::nextInputChain(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YInputChain::FindInputChain(hwid);
}

YInputChain *YInputChain::FirstInputChain(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("InputChain", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YInputChain::FindInputChain(serial+"."+funcId);
}

//--- (end of YInputChain implementation)

//--- (YInputChain functions)
//--- (end of YInputChain functions)
