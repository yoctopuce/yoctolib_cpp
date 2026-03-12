/*********************************************************************
 *
 *  $Id: svn_id $
 *
 *  Implements yFindOrientation(), the high-level API for Orientation functions
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

#include "yocto_orientation.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#define  __FILE_ID__  "orientation"

#ifdef YOCTOLIB_NAMESPACE
using namespace YOCTOLIB_NAMESPACE;
#endif

YOrientation::YOrientation(const string& func): YSensor(func)
//--- (YOrientation initialization)
    ,_command(COMMAND_INVALID)
    ,_zeroOffset(ZEROOFFSET_INVALID)
    ,_valueCallbackOrientation(NULL)
    ,_timedReportCallbackOrientation(NULL)
//--- (end of YOrientation initialization)
{
    _className="Orientation";
}

YOrientation::~YOrientation()
{
//--- (YOrientation cleanup)
//--- (end of YOrientation cleanup)
}
//--- (YOrientation implementation)
// static attributes
const string YOrientation::COMMAND_INVALID = YAPI_INVALID_STRING;
const double YOrientation::ZEROOFFSET_INVALID = YAPI_INVALID_DOUBLE;

int YOrientation::_parseAttr(YJSONObject *json_val)
{
    if(json_val->has("command")) {
        _command =  json_val->getString("command");
    }
    if(json_val->has("zeroOffset")) {
        _zeroOffset =  floor(json_val->getDouble("zeroOffset") / 65.536 + 0.5) / 1000.0;
    }
    return YSensor::_parseAttr(json_val);
}


string YOrientation::get_command(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YOrientation::COMMAND_INVALID;
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

int YOrientation::set_command(const string& newval)
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
 * Sets an offset between the orientation reported by the sensor and the actual orientation. This
 * can typically be used  to compensate for mechanical offset. This offset can also be set
 * automatically using the zero() method.
 * Remember to call the saveToFlash() method of the module if the modification must be kept.
 * On failure, throws an exception or returns a negative error code.
 *
 * @param newval : a floating point number
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YOrientation::set_zeroOffset(double newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%" FMTs64, (s64)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
        res = _setAttr("zeroOffset", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the Offset between the orientation reported by the sensor and the actual orientation.
 *
 * @return a floating point number corresponding to the Offset between the orientation reported by the
 * sensor and the actual orientation
 *
 * On failure, throws an exception or returns YOrientation::ZEROOFFSET_INVALID.
 */
double YOrientation::get_zeroOffset(void)
{
    double res = 0.0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YOrientation::ZEROOFFSET_INVALID;
                }
            }
        }
        res = _zeroOffset;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Retrieves an orientation sensor for a given identifier.
 * The identifier can be specified using several formats:
 *
 * - FunctionLogicalName
 * - ModuleSerialNumber.FunctionIdentifier
 * - ModuleSerialNumber.FunctionLogicalName
 * - ModuleLogicalName.FunctionIdentifier
 * - ModuleLogicalName.FunctionLogicalName
 *
 *
 * This function does not require that the orientation sensor is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the orientation sensor is
 * indeed online at a given time. In case of ambiguity when looking for
 * an orientation sensor by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the orientation sensor, for instance
 *         MyDevice.orientation.
 *
 * @return a YOrientation object allowing you to drive the orientation sensor.
 */
YOrientation* YOrientation::FindOrientation(string func)
{
    YOrientation* obj = NULL;
    int taken = 0;
    if (YAPI::_apiInitialized) {
        yEnterCriticalSection(&YAPI::_global_cs);
        taken = 1;
    }try {
        obj = (YOrientation*) YFunction::_FindFromCache("Orientation", func);
        if (obj == NULL) {
            obj = new YOrientation(func);
            YFunction::_AddToCache("Orientation", func, obj);
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
 * The callback is then invoked only during the execution of ySleep or yHandleEvents.
 * This provides control over the time when the callback is triggered. For good responsiveness,
 * remember to call one of these two functions periodically. The callback is called once juste after beeing
 * registered, passing the current advertised value  of the function, provided that it is not an empty string.
 * To unregister a callback, pass a NULL pointer as argument.
 *
 * @param callback : the callback function to call, or a NULL pointer. The callback function should take two
 *         arguments: the function object of which the value has changed, and the character string describing
 *         the new advertised value.
 * @noreturn
 */
int YOrientation::registerValueCallback(YOrientationValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackOrientation = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YOrientation::_invokeValueCallback(string value)
{
    if (_valueCallbackOrientation != NULL) {
        _valueCallbackOrientation(this, value);
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
int YOrientation::registerTimedReportCallback(YOrientationTimedReportCallback callback)
{
    YSensor* sensor = NULL;
    sensor = this;
    if (callback != NULL) {
        YFunction::_UpdateTimedReportCallbackList(sensor, true);
    } else {
        YFunction::_UpdateTimedReportCallbackList(sensor, false);
    }
    _timedReportCallbackOrientation = callback;
    return 0;
}

int YOrientation::_invokeTimedReportCallback(YMeasure value)
{
    if (_timedReportCallbackOrientation != NULL) {
        _timedReportCallbackOrientation(this, value);
    } else {
        YSensor::_invokeTimedReportCallback(value);
    }
    return 0;
}

int YOrientation::sendCommand(string command)
{
    return this->set_command(command);
}

/**
 * Reset the sensor's zero to current position by automatically setting a new offset.
 * Remember to call the saveToFlash() method of the module if the modification must be kept.
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YOrientation::zero(void)
{
    return this->sendCommand("Z");
}

/**
 * Modifies the calibration of the MA600A sensor using an array of 32
 * values representing the offset in degrees between the true values and
 * those measured regularly every 11.25 degrees starting from zero. The calibration
 * is applied immediately and is stored permanently in the MA600A sensor.
 * Before calculating the offset values, remember to clear any previous
 * calibration using the clearCalibration function and set
 * the zero offset  to 0. After a calibration change, the sensor will stop
 * measurements for about one second.
 * Do not confuse this function with the generic calibrateFromPoints function,
 * which works at the YSensor level and is not necessarily well suited to
 * a sensor returning circular values.
 *
 * @param offsetValues : array of 32 floating point values in the [-11.25..+11.25] range
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YOrientation::set_calibration(vector<double>& offsetValues)
{
    string res;
    int npt = 0;
    int idx = 0;
    int corr = 0;
    npt = (int)offsetValues.size();
    if (npt != 32) {
        this->_throw(YAPI_INVALID_ARGUMENT, "Invalid calibration parameters (32 expected)");
        return YAPI_INVALID_ARGUMENT;
    }
    res = "C";
    idx = 0;
    while (idx < npt) {
        corr = (int) floor(offsetValues[idx] * 128 / 11.25+0.5);
        if ((corr < -128) || (corr > 127)) {
            this->_throw(YAPI_INVALID_ARGUMENT, "Calibration parameter exceeds permitted range (+/-11.25)");
            return YAPI_INVALID_ARGUMENT;
        }
        if (corr < 0) {
            corr = corr + 256;
        }
        res = YapiWrapper::ysprintf("%s%02x",res.c_str(),corr);
        idx = idx + 1;
    }
    return this->sendCommand(res);
}

/**
 * Retrieves offset correction data points previously entered using the method
 * set_calibration.
 *
 * @param offsetValues : array of 32 floating point numbers, that will be filled by the
 *         function with the offset values for the correction points.
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YOrientation::get_Calibration(vector<double>& offsetValues)
{
    return 0;
}

/**
 * Cancels any calibration set with set_calibration. This function
 * is equivalent to calling set_calibration with only zeros.
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YOrientation::clearCalibration(void)
{
    return this->sendCommand("-");
}

YOrientation *YOrientation::nextOrientation(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YOrientation::FindOrientation(hwid);
}

YOrientation *YOrientation::FirstOrientation(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("Orientation", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YOrientation::FindOrientation(serial+"."+funcId);
}

//--- (end of YOrientation implementation)

//--- (YOrientation functions)
//--- (end of YOrientation functions)
