/*********************************************************************
 *
 *  $Id: yocto_multisenscontroller.cpp 56106 2023-08-16 09:14:54Z seb $
 *
 *  Implements yFindMultiSensController(), the high-level API for MultiSensController functions
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

#include "yocto_multisenscontroller.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#define  __FILE_ID__  "multisenscontroller"

#ifdef YOCTOLIB_NAMESPACE
using namespace YOCTOLIB_NAMESPACE;
#endif

YMultiSensController::YMultiSensController(const string& func): YFunction(func)
//--- (YMultiSensController initialization)
    ,_nSensors(NSENSORS_INVALID)
    ,_maxSensors(MAXSENSORS_INVALID)
    ,_maintenanceMode(MAINTENANCEMODE_INVALID)
    ,_lastAddressDetected(LASTADDRESSDETECTED_INVALID)
    ,_command(COMMAND_INVALID)
    ,_valueCallbackMultiSensController(NULL)
//--- (end of YMultiSensController initialization)
{
    _className="MultiSensController";
}

YMultiSensController::~YMultiSensController()
{
//--- (YMultiSensController cleanup)
//--- (end of YMultiSensController cleanup)
}
//--- (YMultiSensController implementation)
// static attributes
const string YMultiSensController::COMMAND_INVALID = YAPI_INVALID_STRING;

int YMultiSensController::_parseAttr(YJSONObject *json_val)
{
    if(json_val->has("nSensors")) {
        _nSensors =  json_val->getInt("nSensors");
    }
    if(json_val->has("maxSensors")) {
        _maxSensors =  json_val->getInt("maxSensors");
    }
    if(json_val->has("maintenanceMode")) {
        _maintenanceMode =  (Y_MAINTENANCEMODE_enum)json_val->getInt("maintenanceMode");
    }
    if(json_val->has("lastAddressDetected")) {
        _lastAddressDetected =  json_val->getInt("lastAddressDetected");
    }
    if(json_val->has("command")) {
        _command =  json_val->getString("command");
    }
    return YFunction::_parseAttr(json_val);
}


/**
 * Returns the number of sensors to poll.
 *
 * @return an integer corresponding to the number of sensors to poll
 *
 * On failure, throws an exception or returns YMultiSensController::NSENSORS_INVALID.
 */
int YMultiSensController::get_nSensors(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YMultiSensController::NSENSORS_INVALID;
                }
            }
        }
        res = _nSensors;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the number of sensors to poll. Remember to call the
 * saveToFlash() method of the module if the
 * modification must be kept. It is recommended to restart the
 * device with  module->reboot() after modifying
 * (and saving) this settings.
 *
 * @param newval : an integer corresponding to the number of sensors to poll
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YMultiSensController::set_nSensors(int newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("nSensors", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the maximum configurable sensor count allowed on this device.
 *
 * @return an integer corresponding to the maximum configurable sensor count allowed on this device
 *
 * On failure, throws an exception or returns YMultiSensController::MAXSENSORS_INVALID.
 */
int YMultiSensController::get_maxSensors(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YMultiSensController::MAXSENSORS_INVALID;
                }
            }
        }
        res = _maxSensors;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns true when the device is in maintenance mode.
 *
 * @return either YMultiSensController::MAINTENANCEMODE_FALSE or
 * YMultiSensController::MAINTENANCEMODE_TRUE, according to true when the device is in maintenance mode
 *
 * On failure, throws an exception or returns YMultiSensController::MAINTENANCEMODE_INVALID.
 */
Y_MAINTENANCEMODE_enum YMultiSensController::get_maintenanceMode(void)
{
    Y_MAINTENANCEMODE_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YMultiSensController::MAINTENANCEMODE_INVALID;
                }
            }
        }
        res = _maintenanceMode;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the device mode to enable maintenance and to stop sensor polling.
 * This way, the device does not automatically restart when it cannot
 * communicate with one of the sensors.
 *
 * @param newval : either YMultiSensController::MAINTENANCEMODE_FALSE or
 * YMultiSensController::MAINTENANCEMODE_TRUE, according to the device mode to enable maintenance and
 * to stop sensor polling
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YMultiSensController::set_maintenanceMode(Y_MAINTENANCEMODE_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = (newval>0 ? "1" : "0");
        res = _setAttr("maintenanceMode", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the I2C address of the most recently detected sensor. This method can
 * be used to in case of I2C communication error to determine what is the
 * last sensor that can be reached, or after a call to setupAddress
 * to make sure that the address change was properly processed.
 *
 * @return an integer corresponding to the I2C address of the most recently detected sensor
 *
 * On failure, throws an exception or returns YMultiSensController::LASTADDRESSDETECTED_INVALID.
 */
int YMultiSensController::get_lastAddressDetected(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YMultiSensController::LASTADDRESSDETECTED_INVALID;
                }
            }
        }
        res = _lastAddressDetected;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

string YMultiSensController::get_command(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YMultiSensController::COMMAND_INVALID;
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

int YMultiSensController::set_command(const string& newval)
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
 * Retrieves a multi-sensor controller for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that the multi-sensor controller is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the multi-sensor controller is
 * indeed online at a given time. In case of ambiguity when looking for
 * a multi-sensor controller by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the multi-sensor controller, for instance
 *         YTEMPIR1.multiSensController.
 *
 * @return a YMultiSensController object allowing you to drive the multi-sensor controller.
 */
YMultiSensController* YMultiSensController::FindMultiSensController(string func)
{
    YMultiSensController* obj = NULL;
    int taken = 0;
    if (YAPI::_apiInitialized) {
        yEnterCriticalSection(&YAPI::_global_cs);
        taken = 1;
    }try {
        obj = (YMultiSensController*) YFunction::_FindFromCache("MultiSensController", func);
        if (obj == NULL) {
            obj = new YMultiSensController(func);
            YFunction::_AddToCache("MultiSensController", func, obj);
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
int YMultiSensController::registerValueCallback(YMultiSensControllerValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackMultiSensController = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YMultiSensController::_invokeValueCallback(string value)
{
    if (_valueCallbackMultiSensController != NULL) {
        _valueCallbackMultiSensController(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

/**
 * Configures the I2C address of the only sensor connected to the device.
 * It is recommended to put the the device in maintenance mode before
 * changing sensor addresses.  This method is only intended to work with a single
 * sensor connected to the device. If several sensors are connected, the result
 * is unpredictable.
 *
 * Note that the device is expecting to find a sensor or a string of sensors with specific
 * addresses. Check the device documentation to find out which addresses should be used.
 *
 * @param addr : new address of the connected sensor
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *         On failure, throws an exception or returns a negative error code.
 */
int YMultiSensController::setupAddress(int addr)
{
    string cmd;
    int res = 0;
    cmd = YapiWrapper::ysprintf("A%d",addr);
    res = this->set_command(cmd);
    if (!(res == YAPI_SUCCESS)) {
        _throw((YRETCODE)(YAPI_IO_ERROR), "unable to trigger address change");
        return YAPI_IO_ERROR;
    }
    {string ignore_error; YAPI::Sleep(1500, ignore_error);};
    res = this->get_lastAddressDetected();
    if (!(res > 0)) {
        _throw((YRETCODE)(YAPI_IO_ERROR), "IR sensor not found");
        return YAPI_IO_ERROR;
    }
    if (!(res == addr)) {
        _throw((YRETCODE)(YAPI_IO_ERROR), "address change failed");
        return YAPI_IO_ERROR;
    }
    return YAPI_SUCCESS;
}

/**
 * Triggers the I2C address detection procedure for the only sensor connected to the device.
 * This method is only intended to work with a single sensor connected to the device.
 * If several sensors are connected, the result is unpredictable.
 *
 * @return the I2C address of the detected sensor, or 0 if none is found
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YMultiSensController::get_sensorAddress(void)
{
    int res = 0;
    res = this->set_command("a");
    if (!(res == YAPI_SUCCESS)) {
        _throw((YRETCODE)(YAPI_IO_ERROR), "unable to trigger address detection");
        return res;
    }
    {string ignore_error; YAPI::Sleep(1000, ignore_error);};
    res = this->get_lastAddressDetected();
    return res;
}

YMultiSensController *YMultiSensController::nextMultiSensController(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YMultiSensController::FindMultiSensController(hwid);
}

YMultiSensController *YMultiSensController::FirstMultiSensController(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("MultiSensController", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YMultiSensController::FindMultiSensController(serial+"."+funcId);
}

//--- (end of YMultiSensController implementation)

//--- (YMultiSensController functions)
//--- (end of YMultiSensController functions)
