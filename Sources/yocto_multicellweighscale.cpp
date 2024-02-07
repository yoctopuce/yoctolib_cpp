/*********************************************************************
 *
 *  $Id: yocto_multicellweighscale.cpp 52570 2022-12-26 09:27:54Z seb $
 *
 *  Implements yFindMultiCellWeighScale(), the high-level API for MultiCellWeighScale functions
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

#include "yocto_multicellweighscale.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#define  __FILE_ID__  "multicellweighscale"

#ifdef YOCTOLIB_NAMESPACE
using namespace YOCTOLIB_NAMESPACE;
#endif

YMultiCellWeighScale::YMultiCellWeighScale(const string& func): YSensor(func)
//--- (YMultiCellWeighScale initialization)
    ,_cellCount(CELLCOUNT_INVALID)
    ,_externalSense(EXTERNALSENSE_INVALID)
    ,_excitation(EXCITATION_INVALID)
    ,_tempAvgAdaptRatio(TEMPAVGADAPTRATIO_INVALID)
    ,_tempChgAdaptRatio(TEMPCHGADAPTRATIO_INVALID)
    ,_compTempAvg(COMPTEMPAVG_INVALID)
    ,_compTempChg(COMPTEMPCHG_INVALID)
    ,_compensation(COMPENSATION_INVALID)
    ,_zeroTracking(ZEROTRACKING_INVALID)
    ,_command(COMMAND_INVALID)
    ,_valueCallbackMultiCellWeighScale(NULL)
    ,_timedReportCallbackMultiCellWeighScale(NULL)
//--- (end of YMultiCellWeighScale initialization)
{
    _className="MultiCellWeighScale";
}

YMultiCellWeighScale::~YMultiCellWeighScale()
{
//--- (YMultiCellWeighScale cleanup)
//--- (end of YMultiCellWeighScale cleanup)
}
//--- (YMultiCellWeighScale implementation)
// static attributes
const double YMultiCellWeighScale::TEMPAVGADAPTRATIO_INVALID = YAPI_INVALID_DOUBLE;
const double YMultiCellWeighScale::TEMPCHGADAPTRATIO_INVALID = YAPI_INVALID_DOUBLE;
const double YMultiCellWeighScale::COMPTEMPAVG_INVALID = YAPI_INVALID_DOUBLE;
const double YMultiCellWeighScale::COMPTEMPCHG_INVALID = YAPI_INVALID_DOUBLE;
const double YMultiCellWeighScale::COMPENSATION_INVALID = YAPI_INVALID_DOUBLE;
const double YMultiCellWeighScale::ZEROTRACKING_INVALID = YAPI_INVALID_DOUBLE;
const string YMultiCellWeighScale::COMMAND_INVALID = YAPI_INVALID_STRING;

int YMultiCellWeighScale::_parseAttr(YJSONObject *json_val)
{
    if(json_val->has("cellCount")) {
        _cellCount =  json_val->getInt("cellCount");
    }
    if(json_val->has("externalSense")) {
        _externalSense =  (Y_EXTERNALSENSE_enum)json_val->getInt("externalSense");
    }
    if(json_val->has("excitation")) {
        _excitation =  (Y_EXCITATION_enum)json_val->getInt("excitation");
    }
    if(json_val->has("tempAvgAdaptRatio")) {
        _tempAvgAdaptRatio =  floor(json_val->getDouble("tempAvgAdaptRatio") / 65.536 + 0.5) / 1000.0;
    }
    if(json_val->has("tempChgAdaptRatio")) {
        _tempChgAdaptRatio =  floor(json_val->getDouble("tempChgAdaptRatio") / 65.536 + 0.5) / 1000.0;
    }
    if(json_val->has("compTempAvg")) {
        _compTempAvg =  floor(json_val->getDouble("compTempAvg") / 65.536 + 0.5) / 1000.0;
    }
    if(json_val->has("compTempChg")) {
        _compTempChg =  floor(json_val->getDouble("compTempChg") / 65.536 + 0.5) / 1000.0;
    }
    if(json_val->has("compensation")) {
        _compensation =  floor(json_val->getDouble("compensation") / 65.536 + 0.5) / 1000.0;
    }
    if(json_val->has("zeroTracking")) {
        _zeroTracking =  floor(json_val->getDouble("zeroTracking") / 65.536 + 0.5) / 1000.0;
    }
    if(json_val->has("command")) {
        _command =  json_val->getString("command");
    }
    return YSensor::_parseAttr(json_val);
}


/**
 * Changes the measuring unit for the weight.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : a string corresponding to the measuring unit for the weight
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YMultiCellWeighScale::set_unit(const string& newval)
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
 * Returns the number of load cells in use.
 *
 * @return an integer corresponding to the number of load cells in use
 *
 * On failure, throws an exception or returns YMultiCellWeighScale::CELLCOUNT_INVALID.
 */
int YMultiCellWeighScale::get_cellCount(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YMultiCellWeighScale::CELLCOUNT_INVALID;
                }
            }
        }
        res = _cellCount;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the number of load cells in use. Remember to call the saveToFlash()
 * method of the module if the modification must be kept.
 *
 * @param newval : an integer corresponding to the number of load cells in use
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YMultiCellWeighScale::set_cellCount(int newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("cellCount", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns true if entry 4 is used as external sense for 6-wires load cells.
 *
 * @return either YMultiCellWeighScale::EXTERNALSENSE_FALSE or YMultiCellWeighScale::EXTERNALSENSE_TRUE,
 * according to true if entry 4 is used as external sense for 6-wires load cells
 *
 * On failure, throws an exception or returns YMultiCellWeighScale::EXTERNALSENSE_INVALID.
 */
Y_EXTERNALSENSE_enum YMultiCellWeighScale::get_externalSense(void)
{
    Y_EXTERNALSENSE_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YMultiCellWeighScale::EXTERNALSENSE_INVALID;
                }
            }
        }
        res = _externalSense;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the configuration to tell if entry 4 is used as external sense for
 * 6-wires load cells. Remember to call the saveToFlash() method of the
 * module if the modification must be kept.
 *
 * @param newval : either YMultiCellWeighScale::EXTERNALSENSE_FALSE or
 * YMultiCellWeighScale::EXTERNALSENSE_TRUE, according to the configuration to tell if entry 4 is used
 * as external sense for
 *         6-wires load cells
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YMultiCellWeighScale::set_externalSense(Y_EXTERNALSENSE_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = (newval>0 ? "1" : "0");
        res = _setAttr("externalSense", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the current load cell bridge excitation method.
 *
 * @return a value among YMultiCellWeighScale::EXCITATION_OFF, YMultiCellWeighScale::EXCITATION_DC and
 * YMultiCellWeighScale::EXCITATION_AC corresponding to the current load cell bridge excitation method
 *
 * On failure, throws an exception or returns YMultiCellWeighScale::EXCITATION_INVALID.
 */
Y_EXCITATION_enum YMultiCellWeighScale::get_excitation(void)
{
    Y_EXCITATION_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YMultiCellWeighScale::EXCITATION_INVALID;
                }
            }
        }
        res = _excitation;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the current load cell bridge excitation method.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : a value among YMultiCellWeighScale::EXCITATION_OFF,
 * YMultiCellWeighScale::EXCITATION_DC and YMultiCellWeighScale::EXCITATION_AC corresponding to the
 * current load cell bridge excitation method
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YMultiCellWeighScale::set_excitation(Y_EXCITATION_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("excitation", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the averaged temperature update rate, in per mille.
 * The purpose of this adaptation ratio is to model the thermal inertia of the load cell.
 * The averaged temperature is updated every 10 seconds, by applying this adaptation rate
 * to the difference between the measures ambient temperature and the current compensation
 * temperature. The standard rate is 0.2 per mille, and the maximal rate is 65 per mille.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : a floating point number corresponding to the averaged temperature update rate, in per mille
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YMultiCellWeighScale::set_tempAvgAdaptRatio(double newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%" FMTs64, (s64)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
        res = _setAttr("tempAvgAdaptRatio", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the averaged temperature update rate, in per mille.
 * The purpose of this adaptation ratio is to model the thermal inertia of the load cell.
 * The averaged temperature is updated every 10 seconds, by applying this adaptation rate
 * to the difference between the measures ambient temperature and the current compensation
 * temperature. The standard rate is 0.2 per mille, and the maximal rate is 65 per mille.
 *
 * @return a floating point number corresponding to the averaged temperature update rate, in per mille
 *
 * On failure, throws an exception or returns YMultiCellWeighScale::TEMPAVGADAPTRATIO_INVALID.
 */
double YMultiCellWeighScale::get_tempAvgAdaptRatio(void)
{
    double res = 0.0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YMultiCellWeighScale::TEMPAVGADAPTRATIO_INVALID;
                }
            }
        }
        res = _tempAvgAdaptRatio;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the temperature change update rate, in per mille.
 * The temperature change is updated every 10 seconds, by applying this adaptation rate
 * to the difference between the measures ambient temperature and the current temperature used for
 * change compensation. The standard rate is 0.6 per mille, and the maximal rate is 65 per mille.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : a floating point number corresponding to the temperature change update rate, in per mille
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YMultiCellWeighScale::set_tempChgAdaptRatio(double newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%" FMTs64, (s64)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
        res = _setAttr("tempChgAdaptRatio", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the temperature change update rate, in per mille.
 * The temperature change is updated every 10 seconds, by applying this adaptation rate
 * to the difference between the measures ambient temperature and the current temperature used for
 * change compensation. The standard rate is 0.6 per mille, and the maximal rate is 65 per mille.
 *
 * @return a floating point number corresponding to the temperature change update rate, in per mille
 *
 * On failure, throws an exception or returns YMultiCellWeighScale::TEMPCHGADAPTRATIO_INVALID.
 */
double YMultiCellWeighScale::get_tempChgAdaptRatio(void)
{
    double res = 0.0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YMultiCellWeighScale::TEMPCHGADAPTRATIO_INVALID;
                }
            }
        }
        res = _tempChgAdaptRatio;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the current averaged temperature, used for thermal compensation.
 *
 * @return a floating point number corresponding to the current averaged temperature, used for thermal compensation
 *
 * On failure, throws an exception or returns YMultiCellWeighScale::COMPTEMPAVG_INVALID.
 */
double YMultiCellWeighScale::get_compTempAvg(void)
{
    double res = 0.0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YMultiCellWeighScale::COMPTEMPAVG_INVALID;
                }
            }
        }
        res = _compTempAvg;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the current temperature variation, used for thermal compensation.
 *
 * @return a floating point number corresponding to the current temperature variation, used for
 * thermal compensation
 *
 * On failure, throws an exception or returns YMultiCellWeighScale::COMPTEMPCHG_INVALID.
 */
double YMultiCellWeighScale::get_compTempChg(void)
{
    double res = 0.0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YMultiCellWeighScale::COMPTEMPCHG_INVALID;
                }
            }
        }
        res = _compTempChg;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the current current thermal compensation value.
 *
 * @return a floating point number corresponding to the current current thermal compensation value
 *
 * On failure, throws an exception or returns YMultiCellWeighScale::COMPENSATION_INVALID.
 */
double YMultiCellWeighScale::get_compensation(void)
{
    double res = 0.0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YMultiCellWeighScale::COMPENSATION_INVALID;
                }
            }
        }
        res = _compensation;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the zero tracking threshold value. When this threshold is larger than
 * zero, any measure under the threshold will automatically be ignored and the
 * zero compensation will be updated.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : a floating point number corresponding to the zero tracking threshold value
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YMultiCellWeighScale::set_zeroTracking(double newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%" FMTs64, (s64)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
        res = _setAttr("zeroTracking", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the zero tracking threshold value. When this threshold is larger than
 * zero, any measure under the threshold will automatically be ignored and the
 * zero compensation will be updated.
 *
 * @return a floating point number corresponding to the zero tracking threshold value
 *
 * On failure, throws an exception or returns YMultiCellWeighScale::ZEROTRACKING_INVALID.
 */
double YMultiCellWeighScale::get_zeroTracking(void)
{
    double res = 0.0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YMultiCellWeighScale::ZEROTRACKING_INVALID;
                }
            }
        }
        res = _zeroTracking;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

string YMultiCellWeighScale::get_command(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YMultiCellWeighScale::COMMAND_INVALID;
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

int YMultiCellWeighScale::set_command(const string& newval)
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
 * Retrieves a multi-cell weighing scale sensor for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that the multi-cell weighing scale sensor is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the multi-cell weighing scale sensor is
 * indeed online at a given time. In case of ambiguity when looking for
 * a multi-cell weighing scale sensor by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the multi-cell weighing scale sensor, for instance
 *         YWMBRDG1.multiCellWeighScale.
 *
 * @return a YMultiCellWeighScale object allowing you to drive the multi-cell weighing scale sensor.
 */
YMultiCellWeighScale* YMultiCellWeighScale::FindMultiCellWeighScale(string func)
{
    YMultiCellWeighScale* obj = NULL;
    int taken = 0;
    if (YAPI::_apiInitialized) {
        yEnterCriticalSection(&YAPI::_global_cs);
        taken = 1;
    }try {
        obj = (YMultiCellWeighScale*) YFunction::_FindFromCache("MultiCellWeighScale", func);
        if (obj == NULL) {
            obj = new YMultiCellWeighScale(func);
            YFunction::_AddToCache("MultiCellWeighScale", func, obj);
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
int YMultiCellWeighScale::registerValueCallback(YMultiCellWeighScaleValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackMultiCellWeighScale = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YMultiCellWeighScale::_invokeValueCallback(string value)
{
    if (_valueCallbackMultiCellWeighScale != NULL) {
        _valueCallbackMultiCellWeighScale(this, value);
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
int YMultiCellWeighScale::registerTimedReportCallback(YMultiCellWeighScaleTimedReportCallback callback)
{
    YSensor* sensor = NULL;
    sensor = this;
    if (callback != NULL) {
        YFunction::_UpdateTimedReportCallbackList(sensor, true);
    } else {
        YFunction::_UpdateTimedReportCallbackList(sensor, false);
    }
    _timedReportCallbackMultiCellWeighScale = callback;
    return 0;
}

int YMultiCellWeighScale::_invokeTimedReportCallback(YMeasure value)
{
    if (_timedReportCallbackMultiCellWeighScale != NULL) {
        _timedReportCallbackMultiCellWeighScale(this, value);
    } else {
        YSensor::_invokeTimedReportCallback(value);
    }
    return 0;
}

/**
 * Adapts the load cell signal bias (stored in the corresponding genericSensor)
 * so that the current signal corresponds to a zero weight. Remember to call the
 * saveToFlash() method of the module if the modification must be kept.
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YMultiCellWeighScale::tare(void)
{
    return this->set_command("T");
}

/**
 * Configures the load cells span parameters (stored in the corresponding genericSensors)
 * so that the current signal corresponds to the specified reference weight.
 *
 * @param currWeight : reference weight presently on the load cell.
 * @param maxWeight : maximum weight to be expected on the load cell.
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YMultiCellWeighScale::setupSpan(double currWeight,double maxWeight)
{
    return this->set_command(YapiWrapper::ysprintf("S%d:%d", (int) floor(1000*currWeight+0.5),(int) floor(1000*maxWeight+0.5)));
}

YMultiCellWeighScale *YMultiCellWeighScale::nextMultiCellWeighScale(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YMultiCellWeighScale::FindMultiCellWeighScale(hwid);
}

YMultiCellWeighScale *YMultiCellWeighScale::FirstMultiCellWeighScale(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("MultiCellWeighScale", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YMultiCellWeighScale::FindMultiCellWeighScale(serial+"."+funcId);
}

//--- (end of YMultiCellWeighScale implementation)

//--- (YMultiCellWeighScale functions)
//--- (end of YMultiCellWeighScale functions)
