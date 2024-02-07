/*********************************************************************
 *
 *  $Id: yocto_arithmeticsensor.cpp 56106 2023-08-16 09:14:54Z seb $
 *
 *  Implements yFindArithmeticSensor(), the high-level API for ArithmeticSensor functions
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

#include "yocto_arithmeticsensor.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#define  __FILE_ID__  "arithmeticsensor"

#ifdef YOCTOLIB_NAMESPACE
using namespace YOCTOLIB_NAMESPACE;
#endif

YArithmeticSensor::YArithmeticSensor(const string& func): YSensor(func)
//--- (YArithmeticSensor initialization)
    ,_description(DESCRIPTION_INVALID)
    ,_command(COMMAND_INVALID)
    ,_valueCallbackArithmeticSensor(NULL)
    ,_timedReportCallbackArithmeticSensor(NULL)
//--- (end of YArithmeticSensor initialization)
{
    _className="ArithmeticSensor";
}

YArithmeticSensor::~YArithmeticSensor()
{
//--- (YArithmeticSensor cleanup)
//--- (end of YArithmeticSensor cleanup)
}
//--- (YArithmeticSensor implementation)
// static attributes
const string YArithmeticSensor::DESCRIPTION_INVALID = YAPI_INVALID_STRING;
const string YArithmeticSensor::COMMAND_INVALID = YAPI_INVALID_STRING;

int YArithmeticSensor::_parseAttr(YJSONObject *json_val)
{
    if(json_val->has("description")) {
        _description =  json_val->getString("description");
    }
    if(json_val->has("command")) {
        _command =  json_val->getString("command");
    }
    return YSensor::_parseAttr(json_val);
}


/**
 * Changes the measuring unit for the arithmetic sensor.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : a string corresponding to the measuring unit for the arithmetic sensor
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YArithmeticSensor::set_unit(const string& newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = newval;
        res = _setAttr("unit", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns a short informative description of the formula.
 *
 * @return a string corresponding to a short informative description of the formula
 *
 * On failure, throws an exception or returns YArithmeticSensor::DESCRIPTION_INVALID.
 */
string YArithmeticSensor::get_description(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YArithmeticSensor::DESCRIPTION_INVALID;
                }
            }
        }
        res = _description;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

string YArithmeticSensor::get_command(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YArithmeticSensor::COMMAND_INVALID;
                }
            }
        }
        res = _command;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

int YArithmeticSensor::set_command(const string& newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = newval;
        res = _setAttr("command", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Retrieves an arithmetic sensor for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that the arithmetic sensor is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the arithmetic sensor is
 * indeed online at a given time. In case of ambiguity when looking for
 * an arithmetic sensor by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the arithmetic sensor, for instance
 *         RXUVOLT1.arithmeticSensor1.
 *
 * @return a YArithmeticSensor object allowing you to drive the arithmetic sensor.
 */
YArithmeticSensor* YArithmeticSensor::FindArithmeticSensor(string func)
{
    YArithmeticSensor* obj = NULL;
    int taken = 0;
    if (YAPI::_apiInitialized) {
        yEnterCriticalSection(&YAPI::_global_cs);
        taken = 1;
    }try {
        obj = (YArithmeticSensor*) YFunction::_FindFromCache("ArithmeticSensor", func);
        if (obj == NULL) {
            obj = new YArithmeticSensor(func);
            YFunction::_AddToCache("ArithmeticSensor", func, obj);
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
int YArithmeticSensor::registerValueCallback(YArithmeticSensorValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackArithmeticSensor = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YArithmeticSensor::_invokeValueCallback(string value)
{
    if (_valueCallbackArithmeticSensor != NULL) {
        _valueCallbackArithmeticSensor(this, value);
    } else {
        YSensor::_invokeValueCallback(value);
    }
    return 0;
}

/**
 * Registers the callback function that is invoked on every periodic timed notification.
 * The callback is invoked only during the execution of ySleep or yHandleEvents.
 * This provides control over the time when the callback is triggered. For good responsiveness, remember to call
 * one of these two functions periodically. To unregister a callback, pass a NULL pointer as argument.
 *
 * @param callback : the callback function to call, or a NULL pointer. The callback function should take two
 *         arguments: the function object of which the value has changed, and an YMeasure object describing
 *         the new advertised value.
 * @noreturn
 */
int YArithmeticSensor::registerTimedReportCallback(YArithmeticSensorTimedReportCallback callback)
{
    YSensor* sensor = NULL;
    sensor = this;
    if (callback != NULL) {
        YFunction::_UpdateTimedReportCallbackList(sensor, true);
    } else {
        YFunction::_UpdateTimedReportCallbackList(sensor, false);
    }
    _timedReportCallbackArithmeticSensor = callback;
    return 0;
}

int YArithmeticSensor::_invokeTimedReportCallback(YMeasure value)
{
    if (_timedReportCallbackArithmeticSensor != NULL) {
        _timedReportCallbackArithmeticSensor(this, value);
    } else {
        YSensor::_invokeTimedReportCallback(value);
    }
    return 0;
}

/**
 * Defines the arithmetic function by means of an algebraic expression. The expression
 * may include references to device sensors, by their physical or logical name, to
 * usual math functions and to auxiliary functions defined separately.
 *
 * @param expr : the algebraic expression defining the function.
 * @param descr : short informative description of the expression.
 *
 * @return the current expression value if the call succeeds.
 *
 * On failure, throws an exception or returns YAPI::INVALID_DOUBLE.
 */
double YArithmeticSensor::defineExpression(string expr,string descr)
{
    string id;
    string fname;
    string content;
    string data;
    string diags;
    double resval = 0.0;
    id = this->get_functionId();
    id = (id).substr(16, (int)(id).length() - 16);
    fname = YapiWrapper::ysprintf("arithmExpr%s.txt",id.c_str());

    content = YapiWrapper::ysprintf("// %s\n%s", descr.c_str(),expr.c_str());
    data = this->_uploadEx(fname, content);
    diags = data;
    if (!((diags).substr(0, 8) == "Result: ")) {
        _throw((YRETCODE)(YAPI_INVALID_ARGUMENT), diags);
        return YAPI_INVALID_DOUBLE;
    }
    resval = atof(((diags).substr(8, (int)(diags).length()-8)).c_str());
    return resval;
}

/**
 * Retrieves the algebraic expression defining the arithmetic function, as previously
 * configured using the defineExpression function.
 *
 * @return a string containing the mathematical expression.
 *
 * On failure, throws an exception or returns a negative error code.
 */
string YArithmeticSensor::loadExpression(void)
{
    string id;
    string fname;
    string content;
    int idx = 0;
    id = this->get_functionId();
    id = (id).substr(16, (int)(id).length() - 16);
    fname = YapiWrapper::ysprintf("arithmExpr%s.txt",id.c_str());

    content = this->_download(fname);
    idx = _ystrpos(content, "\n");
    if (idx > 0) {
        content = (content).substr(idx+1, (int)(content).length()-(idx+1));
    }
    return content;
}

/**
 * Defines a auxiliary function by means of a table of reference points. Intermediate values
 * will be interpolated between specified reference points. The reference points are given
 * as pairs of floating point numbers.
 * The auxiliary function will be available for use by all ArithmeticSensor objects of the
 * device. Up to nine auxiliary function can be defined in a device, each containing up to
 * 96 reference points.
 *
 * @param name : auxiliary function name, up to 16 characters.
 * @param inputValues : array of floating point numbers, corresponding to the function input value.
 * @param outputValues : array of floating point numbers, corresponding to the output value
 *         desired for each of the input value, index by index.
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YArithmeticSensor::defineAuxiliaryFunction(string name,vector<double> inputValues,vector<double> outputValues)
{
    int siz = 0;
    string defstr;
    int idx = 0;
    double inputVal = 0.0;
    double outputVal = 0.0;
    string fname;
    siz = (int)inputValues.size();
    if (!(siz > 1)) {
        _throw((YRETCODE)(YAPI_INVALID_ARGUMENT), "auxiliary function must be defined by at least two points");
        return YAPI_INVALID_ARGUMENT;
    }
    if (!(siz == (int)outputValues.size())) {
        _throw((YRETCODE)(YAPI_INVALID_ARGUMENT), "table sizes mismatch");
        return YAPI_INVALID_ARGUMENT;
    }
    defstr = "";
    idx = 0;
    while (idx < siz) {
        inputVal = inputValues[idx];
        outputVal = outputValues[idx];
        defstr = YapiWrapper::ysprintf("%s%g:%g\n", defstr.c_str(), inputVal,outputVal);
        idx = idx + 1;
    }
    fname = YapiWrapper::ysprintf("userMap%s.txt",name.c_str());

    return this->_upload(fname, defstr);
}

/**
 * Retrieves the reference points table defining an auxiliary function previously
 * configured using the defineAuxiliaryFunction function.
 *
 * @param name : auxiliary function name, up to 16 characters.
 * @param inputValues : array of floating point numbers, that is filled by the function
 *         with all the function reference input value.
 * @param outputValues : array of floating point numbers, that is filled by the function
 *         output value for each of the input value, index by index.
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YArithmeticSensor::loadAuxiliaryFunction(string name,vector<double>& inputValues,vector<double>& outputValues)
{
    string fname;
    string defbin;
    int siz = 0;

    fname = YapiWrapper::ysprintf("userMap%s.txt",name.c_str());
    defbin = this->_download(fname);
    siz = (int)(defbin).size();
    if (!(siz > 0)) {
        _throw((YRETCODE)(YAPI_INVALID_ARGUMENT), "auxiliary function does not exist");
        return YAPI_INVALID_ARGUMENT;
    }
    inputValues.clear();
    outputValues.clear();
    // FIXME: decode line by line
    return YAPI_SUCCESS;
}

YArithmeticSensor *YArithmeticSensor::nextArithmeticSensor(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YArithmeticSensor::FindArithmeticSensor(hwid);
}

YArithmeticSensor *YArithmeticSensor::FirstArithmeticSensor(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("ArithmeticSensor", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YArithmeticSensor::FindArithmeticSensor(serial+"."+funcId);
}

//--- (end of YArithmeticSensor implementation)

//--- (YArithmeticSensor functions)
//--- (end of YArithmeticSensor functions)
