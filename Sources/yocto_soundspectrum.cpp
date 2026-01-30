/*********************************************************************
 *
 *  $Id: svn_id $
 *
 *  Implements yFindSoundSpectrum(), the high-level API for SoundSpectrum functions
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

#include "yocto_soundspectrum.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#define  __FILE_ID__  "soundspectrum"

#ifdef YOCTOLIB_NAMESPACE
using namespace YOCTOLIB_NAMESPACE;
#endif

YSoundSpectrum::YSoundSpectrum(const string& func): YFunction(func)
//--- (YSoundSpectrum initialization)
    ,_integrationTime(INTEGRATIONTIME_INVALID)
    ,_spectrumData(SPECTRUMDATA_INVALID)
    ,_valueCallbackSoundSpectrum(NULL)
//--- (end of YSoundSpectrum initialization)
{
    _className="SoundSpectrum";
}

YSoundSpectrum::~YSoundSpectrum()
{
//--- (YSoundSpectrum cleanup)
//--- (end of YSoundSpectrum cleanup)
}
//--- (YSoundSpectrum implementation)
// static attributes
const string YSoundSpectrum::SPECTRUMDATA_INVALID = YAPI_INVALID_STRING;

int YSoundSpectrum::_parseAttr(YJSONObject *json_val)
{
    if(json_val->has("integrationTime")) {
        _integrationTime =  json_val->getInt("integrationTime");
    }
    if(json_val->has("spectrumData")) {
        _spectrumData =  json_val->getString("spectrumData");
    }
    return YFunction::_parseAttr(json_val);
}


/**
 * Returns the integration time in milliseconds for calculating time
 * weighted spectrum data.
 *
 * @return an integer corresponding to the integration time in milliseconds for calculating time
 *         weighted spectrum data
 *
 * On failure, throws an exception or returns YSoundSpectrum::INTEGRATIONTIME_INVALID.
 */
int YSoundSpectrum::get_integrationTime(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSoundSpectrum::INTEGRATIONTIME_INVALID;
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
 * Changes the integration time in milliseconds for computing time weighted
 * spectrum data. Be aware that on some devices, changing the integration
 * time for time-weighted spectrum data may also affect the integration
 * period for one or more sound pressure level measurements.
 * Remember to call the saveToFlash() method of the
 * module if the modification must be kept.
 *
 * @param newval : an integer corresponding to the integration time in milliseconds for computing time weighted
 *         spectrum data
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YSoundSpectrum::set_integrationTime(int newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("integrationTime", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

string YSoundSpectrum::get_spectrumData(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSoundSpectrum::SPECTRUMDATA_INVALID;
                }
            }
        }
        res = _spectrumData;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Retrieves a sound spectrum analyzer for a given identifier.
 * The identifier can be specified using several formats:
 *
 * - FunctionLogicalName
 * - ModuleSerialNumber.FunctionIdentifier
 * - ModuleSerialNumber.FunctionLogicalName
 * - ModuleLogicalName.FunctionIdentifier
 * - ModuleLogicalName.FunctionLogicalName
 *
 *
 * This function does not require that the sound spectrum analyzer is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the sound spectrum analyzer is
 * indeed online at a given time. In case of ambiguity when looking for
 * a sound spectrum analyzer by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the sound spectrum analyzer, for instance
 *         MyDevice.soundSpectrum.
 *
 * @return a YSoundSpectrum object allowing you to drive the sound spectrum analyzer.
 */
YSoundSpectrum* YSoundSpectrum::FindSoundSpectrum(string func)
{
    YSoundSpectrum* obj = NULL;
    int taken = 0;
    if (YAPI::_apiInitialized) {
        yEnterCriticalSection(&YAPI::_global_cs);
        taken = 1;
    }try {
        obj = (YSoundSpectrum*) YFunction::_FindFromCache("SoundSpectrum", func);
        if (obj == NULL) {
            obj = new YSoundSpectrum(func);
            YFunction::_AddToCache("SoundSpectrum", func, obj);
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
int YSoundSpectrum::registerValueCallback(YSoundSpectrumValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackSoundSpectrum = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YSoundSpectrum::_invokeValueCallback(string value)
{
    if (_valueCallbackSoundSpectrum != NULL) {
        _valueCallbackSoundSpectrum(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

YSoundSpectrum *YSoundSpectrum::nextSoundSpectrum(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YSoundSpectrum::FindSoundSpectrum(hwid);
}

YSoundSpectrum *YSoundSpectrum::FirstSoundSpectrum(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("SoundSpectrum", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YSoundSpectrum::FindSoundSpectrum(serial+"."+funcId);
}

//--- (end of YSoundSpectrum implementation)

//--- (YSoundSpectrum functions)
//--- (end of YSoundSpectrum functions)
