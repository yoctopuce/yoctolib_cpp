/*********************************************************************
 *
 * $Id: yocto_currentloopoutput.cpp 22182 2015-11-30 21:48:04Z mvuilleu $
 *
 * Implements yFindCurrentLoopOutput(), the high-level API for CurrentLoopOutput functions
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
#include "yocto_currentloopoutput.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

YCurrentLoopOutput::YCurrentLoopOutput(const string& func): YFunction(func)
//--- (CurrentLoopOutput initialization)
    ,_current(CURRENT_INVALID)
    ,_currentTransition(CURRENTTRANSITION_INVALID)
    ,_currentAtStartUp(CURRENTATSTARTUP_INVALID)
    ,_loopPower(LOOPPOWER_INVALID)
    ,_valueCallbackCurrentLoopOutput(NULL)
//--- (end of CurrentLoopOutput initialization)
{
    _className="CurrentLoopOutput";
}

YCurrentLoopOutput::~YCurrentLoopOutput()
{
//--- (YCurrentLoopOutput cleanup)
//--- (end of YCurrentLoopOutput cleanup)
}
//--- (YCurrentLoopOutput implementation)
// static attributes
const double YCurrentLoopOutput::CURRENT_INVALID = YAPI_INVALID_DOUBLE;
const string YCurrentLoopOutput::CURRENTTRANSITION_INVALID = YAPI_INVALID_STRING;
const double YCurrentLoopOutput::CURRENTATSTARTUP_INVALID = YAPI_INVALID_DOUBLE;

int YCurrentLoopOutput::_parseAttr(yJsonStateMachine& j)
{
    if(!strcmp(j.token, "current")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _current =  floor(atof(j.token) * 1000.0 / 65536.0 + 0.5) / 1000.0;
        return 1;
    }
    if(!strcmp(j.token, "currentTransition")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _currentTransition =  _parseString(j);
        return 1;
    }
    if(!strcmp(j.token, "currentAtStartUp")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _currentAtStartUp =  floor(atof(j.token) * 1000.0 / 65536.0 + 0.5) / 1000.0;
        return 1;
    }
    if(!strcmp(j.token, "loopPower")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _loopPower =  (Y_LOOPPOWER_enum)atoi(j.token);
        return 1;
    }
    failed:
    return YFunction::_parseAttr(j);
}


/**
 * Changes the current loop, the valid range is from 3 to 21mA. If the loop is
 * not propely powered, the  target current is not reached and
 * loopPower is set to LOWPWR.
 *
 * @param newval : a floating point number corresponding to the current loop, the valid range is from 3 to 21mA
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YCurrentLoopOutput::set_current(double newval)
{
    string rest_val;
    char buf[32]; sprintf(buf,"%d", (int)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
    return _setAttr("current", rest_val);
}

/**
 * Returns the loop current set point in mA.
 *
 * @return a floating point number corresponding to the loop current set point in mA
 *
 * On failure, throws an exception or returns Y_CURRENT_INVALID.
 */
double YCurrentLoopOutput::get_current(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YCurrentLoopOutput::CURRENT_INVALID;
        }
    }
    return _current;
}

string YCurrentLoopOutput::get_currentTransition(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YCurrentLoopOutput::CURRENTTRANSITION_INVALID;
        }
    }
    return _currentTransition;
}

int YCurrentLoopOutput::set_currentTransition(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("currentTransition", rest_val);
}

/**
 * Changes the loop current at device start up. Remember to call the matching
 * module saveToFlash() method, otherwise this call has no effect.
 *
 * @param newval : a floating point number corresponding to the loop current at device start up
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YCurrentLoopOutput::set_currentAtStartUp(double newval)
{
    string rest_val;
    char buf[32]; sprintf(buf,"%d", (int)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
    return _setAttr("currentAtStartUp", rest_val);
}

/**
 * Returns the current in the loop at device startup, in mA
 *
 * @return a floating point number corresponding to the current in the loop at device startup, in mA
 *
 * On failure, throws an exception or returns Y_CURRENTATSTARTUP_INVALID.
 */
double YCurrentLoopOutput::get_currentAtStartUp(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YCurrentLoopOutput::CURRENTATSTARTUP_INVALID;
        }
    }
    return _currentAtStartUp;
}

/**
 * Returns the loop powerstate.  POWEROK: the loop
 * is powered. NOPWR: the loop in not powered. LOWPWR: the loop is not
 * powered enough to maintain the current required (insufficient voltage).
 *
 * @return a value among Y_LOOPPOWER_NOPWR, Y_LOOPPOWER_LOWPWR and Y_LOOPPOWER_POWEROK corresponding
 * to the loop powerstate
 *
 * On failure, throws an exception or returns Y_LOOPPOWER_INVALID.
 */
Y_LOOPPOWER_enum YCurrentLoopOutput::get_loopPower(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YCurrentLoopOutput::LOOPPOWER_INVALID;
        }
    }
    return _loopPower;
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
 * Use the method YCurrentLoopOutput.isOnline() to test if $THEFUNCTION$ is
 * indeed online at a given time. In case of ambiguity when looking for
 * $AFUNCTION$ by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * @param func : a string that uniquely characterizes $THEFUNCTION$
 *
 * @return a YCurrentLoopOutput object allowing you to drive $THEFUNCTION$.
 */
YCurrentLoopOutput* YCurrentLoopOutput::FindCurrentLoopOutput(string func)
{
    YCurrentLoopOutput* obj = NULL;
    obj = (YCurrentLoopOutput*) YFunction::_FindFromCache("CurrentLoopOutput", func);
    if (obj == NULL) {
        obj = new YCurrentLoopOutput(func);
        YFunction::_AddToCache("CurrentLoopOutput", func, obj);
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
int YCurrentLoopOutput::registerValueCallback(YCurrentLoopOutputValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackCurrentLoopOutput = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YCurrentLoopOutput::_invokeValueCallback(string value)
{
    if (_valueCallbackCurrentLoopOutput != NULL) {
        _valueCallbackCurrentLoopOutput(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

/**
 * Performs a smooth transistion of current flowing in the loop. Any current explicit
 * change cancels any ongoing transition process.
 *
 * @param mA_target   : new current value at the end of the transition
 *         (floating-point number, representing the transition duration in mA)
 * @param ms_duration : total duration of the transition, in milliseconds
 *
 * @return YAPI_SUCCESS when the call succeeds.
 */
int YCurrentLoopOutput::currentMove(double mA_target,int ms_duration)
{
    string newval;
    if (mA_target < 3.0) {
        mA_target  = 3.0;
    }
    if (mA_target > 21.0) {
        mA_target = 21.0;
    }
    newval = YapiWrapper::ysprintf("%d:%d", (int) floor(mA_target*1000+0.5),ms_duration);
    // may throw an exception
    return this->set_currentTransition(newval);
}

YCurrentLoopOutput *YCurrentLoopOutput::nextCurrentLoopOutput(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YCurrentLoopOutput::FindCurrentLoopOutput(hwid);
}

YCurrentLoopOutput* YCurrentLoopOutput::FirstCurrentLoopOutput(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("CurrentLoopOutput", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YCurrentLoopOutput::FindCurrentLoopOutput(serial+"."+funcId);
}

//--- (end of YCurrentLoopOutput implementation)

//--- (CurrentLoopOutput functions)
//--- (end of CurrentLoopOutput functions)
