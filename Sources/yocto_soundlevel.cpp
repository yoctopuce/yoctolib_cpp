/*********************************************************************
 *
 *  $Id: svn_id $
 *
 *  Implements yFindSoundLevel(), the high-level API for SoundLevel functions
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

#include "yocto_soundlevel.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#define  __FILE_ID__  "soundlevel"

#ifdef YOCTOLIB_NAMESPACE
using namespace YOCTOLIB_NAMESPACE;
#endif

YSoundLevel::YSoundLevel(const string& func): YSensor(func)
//--- (YSoundLevel initialization)
    ,_label(LABEL_INVALID)
    ,_integrationTime(INTEGRATIONTIME_INVALID)
    ,_valueCallbackSoundLevel(NULL)
    ,_timedReportCallbackSoundLevel(NULL)
//--- (end of YSoundLevel initialization)
{
    _className="SoundLevel";
}

YSoundLevel::~YSoundLevel()
{
//--- (YSoundLevel cleanup)
//--- (end of YSoundLevel cleanup)
}
//--- (YSoundLevel implementation)
// static attributes
const string YSoundLevel::LABEL_INVALID = YAPI_INVALID_STRING;

int YSoundLevel::_parseAttr(YJSONObject *json_val)
{
    if(json_val->has("label")) {
        _label =  json_val->getString("label");
    }
    if(json_val->has("integrationTime")) {
        _integrationTime =  json_val->getInt("integrationTime");
    }
    return YSensor::_parseAttr(json_val);
}


/**
 * Changes the measuring unit for the sound pressure level (dBA, dBC or dBZ).
 * That unit will directly determine frequency weighting to be used to compute
 * the measured value. Remember to call the saveToFlash() method of the
 * module if the modification must be kept.
 *
 * @param newval : a string corresponding to the measuring unit for the sound pressure level (dBA, dBC or dBZ)
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YSoundLevel::set_unit(const string& newval)
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
 * Returns the label for the sound pressure level measurement, as per
 * IEC standard 61672-1:2013.
 *
 * @return a string corresponding to the label for the sound pressure level measurement, as per
 *         IEC standard 61672-1:2013
 *
 * On failure, throws an exception or returns YSoundLevel::LABEL_INVALID.
 */
string YSoundLevel::get_label(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration == 0) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSoundLevel::LABEL_INVALID;
                }
            }
        }
        res = _label;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the integration time in milliseconds for measuring the sound pressure level.
 *
 * @return an integer corresponding to the integration time in milliseconds for measuring the sound pressure level
 *
 * On failure, throws an exception or returns YSoundLevel::INTEGRATIONTIME_INVALID.
 */
int YSoundLevel::get_integrationTime(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSoundLevel::INTEGRATIONTIME_INVALID;
                }
            }
        }
        res = _integrationTime;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Retrieves a sound pressure level meter for a given identifier.
 * The identifier can be specified using several formats:
 *
 * - FunctionLogicalName
 * - ModuleSerialNumber.FunctionIdentifier
 * - ModuleSerialNumber.FunctionLogicalName
 * - ModuleLogicalName.FunctionIdentifier
 * - ModuleLogicalName.FunctionLogicalName
 *
 *
 * This function does not require that the sound pressure level meter is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the sound pressure level meter is
 * indeed online at a given time. In case of ambiguity when looking for
 * a sound pressure level meter by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the sound pressure level meter, for instance
 *         MyDevice.soundLevel1.
 *
 * @return a YSoundLevel object allowing you to drive the sound pressure level meter.
 */
YSoundLevel* YSoundLevel::FindSoundLevel(string func)
{
    YSoundLevel* obj = NULL;
    int taken = 0;
    if (YAPI::_apiInitialized) {
        yEnterCriticalSection(&YAPI::_global_cs);
        taken = 1;
    }try {
        obj = (YSoundLevel*) YFunction::_FindFromCache("SoundLevel", func);
        if (obj == NULL) {
            obj = new YSoundLevel(func);
            YFunction::_AddToCache("SoundLevel", func, obj);
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
int YSoundLevel::registerValueCallback(YSoundLevelValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackSoundLevel = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YSoundLevel::_invokeValueCallback(string value)
{
    if (_valueCallbackSoundLevel != NULL) {
        _valueCallbackSoundLevel(this, value);
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
int YSoundLevel::registerTimedReportCallback(YSoundLevelTimedReportCallback callback)
{
    YSensor* sensor = NULL;
    sensor = this;
    if (callback != NULL) {
        YFunction::_UpdateTimedReportCallbackList(sensor, true);
    } else {
        YFunction::_UpdateTimedReportCallbackList(sensor, false);
    }
    _timedReportCallbackSoundLevel = callback;
    return 0;
}

int YSoundLevel::_invokeTimedReportCallback(YMeasure value)
{
    if (_timedReportCallbackSoundLevel != NULL) {
        _timedReportCallbackSoundLevel(this, value);
    } else {
        YSensor::_invokeTimedReportCallback(value);
    }
    return 0;
}

YSoundLevel *YSoundLevel::nextSoundLevel(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YSoundLevel::FindSoundLevel(hwid);
}

YSoundLevel *YSoundLevel::FirstSoundLevel(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("SoundLevel", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YSoundLevel::FindSoundLevel(serial+"."+funcId);
}

//--- (end of YSoundLevel implementation)

//--- (YSoundLevel functions)
//--- (end of YSoundLevel functions)
