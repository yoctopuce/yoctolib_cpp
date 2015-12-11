/*********************************************************************
 *
 * $Id: yocto_segmenteddisplay.cpp 22191 2015-12-02 06:49:31Z mvuilleu $
 *
 * Implements yFindSegmentedDisplay(), the high-level API for SegmentedDisplay functions
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
#include "yocto_segmenteddisplay.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

YSegmentedDisplay::YSegmentedDisplay(const string& func): YFunction(func)
//--- (SegmentedDisplay initialization)
    ,_displayedText(DISPLAYEDTEXT_INVALID)
    ,_displayMode(DISPLAYMODE_INVALID)
    ,_valueCallbackSegmentedDisplay(NULL)
//--- (end of SegmentedDisplay initialization)
{
    _className="SegmentedDisplay";
}

YSegmentedDisplay::~YSegmentedDisplay()
{
//--- (YSegmentedDisplay cleanup)
//--- (end of YSegmentedDisplay cleanup)
}
//--- (YSegmentedDisplay implementation)
// static attributes
const string YSegmentedDisplay::DISPLAYEDTEXT_INVALID = YAPI_INVALID_STRING;

int YSegmentedDisplay::_parseAttr(yJsonStateMachine& j)
{
    if(!strcmp(j.token, "displayedText")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _displayedText =  _parseString(j);
        return 1;
    }
    if(!strcmp(j.token, "displayMode")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _displayMode =  (Y_DISPLAYMODE_enum)atoi(j.token);
        return 1;
    }
    failed:
    return YFunction::_parseAttr(j);
}


/**
 * Returns the text currently displayed on the screen.
 *
 * @return a string corresponding to the text currently displayed on the screen
 *
 * On failure, throws an exception or returns Y_DISPLAYEDTEXT_INVALID.
 */
string YSegmentedDisplay::get_displayedText(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YSegmentedDisplay::DISPLAYEDTEXT_INVALID;
        }
    }
    return _displayedText;
}

/**
 * Changes the text currently displayed on the screen.
 *
 * @param newval : a string corresponding to the text currently displayed on the screen
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YSegmentedDisplay::set_displayedText(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("displayedText", rest_val);
}

Y_DISPLAYMODE_enum YSegmentedDisplay::get_displayMode(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YSegmentedDisplay::DISPLAYMODE_INVALID;
        }
    }
    return _displayMode;
}

int YSegmentedDisplay::set_displayMode(Y_DISPLAYMODE_enum newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("displayMode", rest_val);
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
 * Use the method YSegmentedDisplay.isOnline() to test if $THEFUNCTION$ is
 * indeed online at a given time. In case of ambiguity when looking for
 * $AFUNCTION$ by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * @param func : a string that uniquely characterizes $THEFUNCTION$
 *
 * @return a YSegmentedDisplay object allowing you to drive $THEFUNCTION$.
 */
YSegmentedDisplay* YSegmentedDisplay::FindSegmentedDisplay(string func)
{
    YSegmentedDisplay* obj = NULL;
    obj = (YSegmentedDisplay*) YFunction::_FindFromCache("SegmentedDisplay", func);
    if (obj == NULL) {
        obj = new YSegmentedDisplay(func);
        YFunction::_AddToCache("SegmentedDisplay", func, obj);
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
int YSegmentedDisplay::registerValueCallback(YSegmentedDisplayValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackSegmentedDisplay = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YSegmentedDisplay::_invokeValueCallback(string value)
{
    if (_valueCallbackSegmentedDisplay != NULL) {
        _valueCallbackSegmentedDisplay(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

YSegmentedDisplay *YSegmentedDisplay::nextSegmentedDisplay(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YSegmentedDisplay::FindSegmentedDisplay(hwid);
}

YSegmentedDisplay* YSegmentedDisplay::FirstSegmentedDisplay(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("SegmentedDisplay", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YSegmentedDisplay::FindSegmentedDisplay(serial+"."+funcId);
}

//--- (end of YSegmentedDisplay implementation)

//--- (SegmentedDisplay functions)
//--- (end of SegmentedDisplay functions)
