/*********************************************************************
 *
 *  $Id: svn_id $
 *
 *  Implements yFindColorSensor(), the high-level API for ColorSensor functions
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

#include "yocto_colorsensor.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#define  __FILE_ID__  "colorsensor"

#ifdef YOCTOLIB_NAMESPACE
using namespace YOCTOLIB_NAMESPACE;
#endif

YColorSensor::YColorSensor(const string& func): YFunction(func)
//--- (YColorSensor initialization)
    ,_estimationModel(ESTIMATIONMODEL_INVALID)
    ,_workingMode(WORKINGMODE_INVALID)
    ,_ledCurrent(LEDCURRENT_INVALID)
    ,_ledCalibration(LEDCALIBRATION_INVALID)
    ,_integrationTime(INTEGRATIONTIME_INVALID)
    ,_gain(GAIN_INVALID)
    ,_autoGain(AUTOGAIN_INVALID)
    ,_saturation(SATURATION_INVALID)
    ,_estimatedRGB(ESTIMATEDRGB_INVALID)
    ,_estimatedHSL(ESTIMATEDHSL_INVALID)
    ,_estimatedXYZ(ESTIMATEDXYZ_INVALID)
    ,_estimatedOkLab(ESTIMATEDOKLAB_INVALID)
    ,_nearRAL1(NEARRAL1_INVALID)
    ,_nearRAL2(NEARRAL2_INVALID)
    ,_nearRAL3(NEARRAL3_INVALID)
    ,_nearHTMLColor(NEARHTMLCOLOR_INVALID)
    ,_nearSimpleColorIndex(NEARSIMPLECOLORINDEX_INVALID)
    ,_nearSimpleColor(NEARSIMPLECOLOR_INVALID)
    ,_valueCallbackColorSensor(NULL)
//--- (end of YColorSensor initialization)
{
    _className="ColorSensor";
}

YColorSensor::~YColorSensor()
{
//--- (YColorSensor cleanup)
//--- (end of YColorSensor cleanup)
}
//--- (YColorSensor implementation)
// static attributes
const string YColorSensor::AUTOGAIN_INVALID = YAPI_INVALID_STRING;
const string YColorSensor::ESTIMATEDXYZ_INVALID = YAPI_INVALID_STRING;
const string YColorSensor::ESTIMATEDOKLAB_INVALID = YAPI_INVALID_STRING;
const string YColorSensor::NEARRAL1_INVALID = YAPI_INVALID_STRING;
const string YColorSensor::NEARRAL2_INVALID = YAPI_INVALID_STRING;
const string YColorSensor::NEARRAL3_INVALID = YAPI_INVALID_STRING;
const string YColorSensor::NEARHTMLCOLOR_INVALID = YAPI_INVALID_STRING;
const string YColorSensor::NEARSIMPLECOLOR_INVALID = YAPI_INVALID_STRING;

int YColorSensor::_parseAttr(YJSONObject *json_val)
{
    if(json_val->has("estimationModel")) {
        _estimationModel =  (Y_ESTIMATIONMODEL_enum)json_val->getInt("estimationModel");
    }
    if(json_val->has("workingMode")) {
        _workingMode =  (Y_WORKINGMODE_enum)json_val->getInt("workingMode");
    }
    if(json_val->has("ledCurrent")) {
        _ledCurrent =  json_val->getInt("ledCurrent");
    }
    if(json_val->has("ledCalibration")) {
        _ledCalibration =  json_val->getInt("ledCalibration");
    }
    if(json_val->has("integrationTime")) {
        _integrationTime =  json_val->getInt("integrationTime");
    }
    if(json_val->has("gain")) {
        _gain =  json_val->getInt("gain");
    }
    if(json_val->has("autoGain")) {
        _autoGain =  json_val->getString("autoGain");
    }
    if(json_val->has("saturation")) {
        _saturation =  json_val->getInt("saturation");
    }
    if(json_val->has("estimatedRGB")) {
        _estimatedRGB =  json_val->getInt("estimatedRGB");
    }
    if(json_val->has("estimatedHSL")) {
        _estimatedHSL =  json_val->getInt("estimatedHSL");
    }
    if(json_val->has("estimatedXYZ")) {
        _estimatedXYZ =  json_val->getString("estimatedXYZ");
    }
    if(json_val->has("estimatedOkLab")) {
        _estimatedOkLab =  json_val->getString("estimatedOkLab");
    }
    if(json_val->has("nearRAL1")) {
        _nearRAL1 =  json_val->getString("nearRAL1");
    }
    if(json_val->has("nearRAL2")) {
        _nearRAL2 =  json_val->getString("nearRAL2");
    }
    if(json_val->has("nearRAL3")) {
        _nearRAL3 =  json_val->getString("nearRAL3");
    }
    if(json_val->has("nearHTMLColor")) {
        _nearHTMLColor =  json_val->getString("nearHTMLColor");
    }
    if(json_val->has("nearSimpleColorIndex")) {
        _nearSimpleColorIndex =  (Y_NEARSIMPLECOLORINDEX_enum)json_val->getInt("nearSimpleColorIndex");
    }
    if(json_val->has("nearSimpleColor")) {
        _nearSimpleColor =  json_val->getString("nearSimpleColor");
    }
    return YFunction::_parseAttr(json_val);
}


/**
 * Returns the predictive model used for color estimation (reflective or emissive).
 *
 * @return either YColorSensor::ESTIMATIONMODEL_REFLECTION or YColorSensor::ESTIMATIONMODEL_EMISSION,
 * according to the predictive model used for color estimation (reflective or emissive)
 *
 * On failure, throws an exception or returns YColorSensor::ESTIMATIONMODEL_INVALID.
 */
Y_ESTIMATIONMODEL_enum YColorSensor::get_estimationModel(void)
{
    Y_ESTIMATIONMODEL_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YColorSensor::ESTIMATIONMODEL_INVALID;
                }
            }
        }
        res = _estimationModel;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the predictive model to be used for color estimation (reflective or emissive).
 * Remember to call the saveToFlash() method of the module if the modification must be kept.
 *
 * @param newval : either YColorSensor::ESTIMATIONMODEL_REFLECTION or
 * YColorSensor::ESTIMATIONMODEL_EMISSION, according to the predictive model to be used for color
 * estimation (reflective or emissive)
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YColorSensor::set_estimationModel(Y_ESTIMATIONMODEL_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("estimationModel", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the sensor working mode.
 * In Auto mode, sensor parameters are automatically set based on the selected estimation model.
 * In Expert mode, sensor parameters such as gain and integration time are configured manually.
 *
 * @return a value among YColorSensor::WORKINGMODE_AUTO, YColorSensor::WORKINGMODE_EXPERT and
 * YColorSensor::WORKINGMODE_AUTOGAIN corresponding to the sensor working mode
 *
 * On failure, throws an exception or returns YColorSensor::WORKINGMODE_INVALID.
 */
Y_WORKINGMODE_enum YColorSensor::get_workingMode(void)
{
    Y_WORKINGMODE_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YColorSensor::WORKINGMODE_INVALID;
                }
            }
        }
        res = _workingMode;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the sensor working mode.
 * In Auto mode, sensor parameters are automatically set based on the selected estimation model.
 * In Expert mode, sensor parameters such as gain and integration time are configured manually.
 * Remember to call the saveToFlash() method of the module if the modification must be kept.
 *
 * @param newval : a value among YColorSensor::WORKINGMODE_AUTO, YColorSensor::WORKINGMODE_EXPERT and
 * YColorSensor::WORKINGMODE_AUTOGAIN corresponding to the sensor working mode
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YColorSensor::set_workingMode(Y_WORKINGMODE_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("workingMode", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the amount of current sent to the illumination LEDs, for reflection measures.
 * The value is an integer ranging from 0 (LEDs off) to 254 (LEDs at maximum intensity).
 *
 * @return an integer corresponding to the amount of current sent to the illumination LEDs, for reflection measures
 *
 * On failure, throws an exception or returns YColorSensor::LEDCURRENT_INVALID.
 */
int YColorSensor::get_ledCurrent(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YColorSensor::LEDCURRENT_INVALID;
                }
            }
        }
        res = _ledCurrent;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the amount of current sent to the illumination LEDs, for reflection measures.
 * The value is an integer ranging from 0 (LEDs off) to 254 (LEDs at maximum intensity).
 *
 * @param newval : an integer corresponding to the amount of current sent to the illumination LEDs,
 * for reflection measures
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YColorSensor::set_ledCurrent(int newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("ledCurrent", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the current sent to the illumination LEDs during the latest calibration.
 *
 * @return an integer corresponding to the current sent to the illumination LEDs during the latest calibration
 *
 * On failure, throws an exception or returns YColorSensor::LEDCALIBRATION_INVALID.
 */
int YColorSensor::get_ledCalibration(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YColorSensor::LEDCALIBRATION_INVALID;
                }
            }
        }
        res = _ledCalibration;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Remember the LED current sent to the illumination LEDs during a calibration.
 * Thanks to this, the device is able to use the same current when taking measures.
 * Remember to call the saveToFlash() method of the module if the modification must be kept.
 *
 * @param newval : an integer
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YColorSensor::set_ledCalibration(int newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("ledCalibration", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the current integration time for spectral measure, in milliseconds.
 * A longer integration time increase the sensitivity for low light conditions,
 * but reduces the measure taking rate and may lead to saturation for lighter colors.
 *
 * @return an integer corresponding to the current integration time for spectral measure, in milliseconds
 *
 * On failure, throws an exception or returns YColorSensor::INTEGRATIONTIME_INVALID.
 */
int YColorSensor::get_integrationTime(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YColorSensor::INTEGRATIONTIME_INVALID;
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
 * Changes the integration time for spectral measure, in milliseconds.
 * A longer integration time increase the sensitivity for low light conditions,
 * but reduces the measure taking rate and may lead to saturation for lighter colors.
 * This method can only be used when the sensor is configured in expert mode;
 * when running in auto mode, the change is ignored.
 * Remember to call the saveToFlash() method of the module if the modification must be kept.
 *
 * @param newval : an integer corresponding to the integration time for spectral measure, in milliseconds
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YColorSensor::set_integrationTime(int newval)
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

/**
 * Returns the current spectral channel detector gain exponent.
 * For a value n ranging from 0 to 12, the applied gain is 2^(n-1).
 * 0 corresponds to a gain of 0.5, and 12 corresponds to a gain of 2048.
 *
 * @return an integer corresponding to the current spectral channel detector gain exponent
 *
 * On failure, throws an exception or returns YColorSensor::GAIN_INVALID.
 */
int YColorSensor::get_gain(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YColorSensor::GAIN_INVALID;
                }
            }
        }
        res = _gain;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the spectral channel detector gain exponent.
 * For a value n ranging from 0 to 12, the applied gain is 2^(n-1).
 * 0 corresponds to a gain of 0.5, and 12 corresponds to a gain of 2048.
 * This method can only be used when the sensor is configured in expert mode;
 * when running in auto mode, the change is ignored.
 * Remember to call the saveToFlash() method of the module if the modification must be kept.
 *
 * @param newval : an integer corresponding to the spectral channel detector gain exponent
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YColorSensor::set_gain(int newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("gain", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the current autogain parameters of the sensor as a character string.
 * The returned parameter format is: "Min &lt; Channel &lt; Max:Saturation".
 *
 * @return a string corresponding to the current autogain parameters of the sensor as a character string
 *
 * On failure, throws an exception or returns YColorSensor::AUTOGAIN_INVALID.
 */
string YColorSensor::get_autoGain(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YColorSensor::AUTOGAIN_INVALID;
                }
            }
        }
        res = _autoGain;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Remember to call the saveToFlash() method of the module if the modification must be kept.
 *
 * @param newval : a string
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YColorSensor::set_autoGain(const string& newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = newval;
        res = _setAttr("autoGain", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the current saturation state of the sensor, as an integer.
 * Bit 0 indicates saturation of the analog sensor, which can only
 * be corrected by reducing the gain parameters or the luminosity.
 * Bit 1 indicates saturation of the digital interface, which can
 * be corrected by reducing the integration time or the gain.
 *
 * @return an integer corresponding to the current saturation state of the sensor, as an integer
 *
 * On failure, throws an exception or returns YColorSensor::SATURATION_INVALID.
 */
int YColorSensor::get_saturation(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YColorSensor::SATURATION_INVALID;
                }
            }
        }
        res = _saturation;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the estimated color in RGB color model (0xRRGGBB).
 * The RGB color model describes each color using a combination of 3 components:
 * - Red (R): the intensity of red, in the 0...255 range
 * - Green (G): the intensity of green, in the 0...255 range
 * - Blue (B): the intensity of blue, in the 0...255 range
 *
 * @return an integer corresponding to the estimated color in RGB color model (0xRRGGBB)
 *
 * On failure, throws an exception or returns YColorSensor::ESTIMATEDRGB_INVALID.
 */
int YColorSensor::get_estimatedRGB(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YColorSensor::ESTIMATEDRGB_INVALID;
                }
            }
        }
        res = _estimatedRGB;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the estimated color in HSL color model (0xHHSSLL).
 * The HSL color model describes each color using a combination of 3 components:
 * - Hue (H): the angle on the color wheel (0-360 degrees), mapped to 0...255
 * - Saturation (S): the intensity of the color (0-100%), mapped to 0...255
 * - Lightness (L): the brightness of the color (0-100%), mapped to 0...255
 *
 * @return an integer corresponding to the estimated color in HSL color model (0xHHSSLL)
 *
 * On failure, throws an exception or returns YColorSensor::ESTIMATEDHSL_INVALID.
 */
int YColorSensor::get_estimatedHSL(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YColorSensor::ESTIMATEDHSL_INVALID;
                }
            }
        }
        res = _estimatedHSL;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the estimated color according to the CIE XYZ color model.
 * This color model is based on human vision and light perception, with three components
 * represented by real numbers between 0 and 1:
 * - X: corresponds to a component mixing sensitivity to red and green
 * - Y: represents luminance (perceived brightness)
 * - Z: corresponds to sensitivity to blue
 *
 * @return a string corresponding to the estimated color according to the CIE XYZ color model
 *
 * On failure, throws an exception or returns YColorSensor::ESTIMATEDXYZ_INVALID.
 */
string YColorSensor::get_estimatedXYZ(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YColorSensor::ESTIMATEDXYZ_INVALID;
                }
            }
        }
        res = _estimatedXYZ;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the estimated color according to the OkLab color model.
 * OkLab is a perceptual color model that aims to align human color perception with numerical
 * values, so that colors that are visually near are also numerically near. Colors are represented
 * using three components:
 * - L: lightness, a real number between 0 and 1
 * - a: color variations between green and red, between -0.5 and 0.5
 * - b: color variations between blue and yellow, between -0.5 and 0.5.
 *
 * @return a string corresponding to the estimated color according to the OkLab color model
 *
 * On failure, throws an exception or returns YColorSensor::ESTIMATEDOKLAB_INVALID.
 */
string YColorSensor::get_estimatedOkLab(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YColorSensor::ESTIMATEDOKLAB_INVALID;
                }
            }
        }
        res = _estimatedOkLab;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the RAL Classic color closest to the estimated color, with a similarity ratio.
 *
 * @return a string corresponding to the RAL Classic color closest to the estimated color, with a similarity ratio
 *
 * On failure, throws an exception or returns YColorSensor::NEARRAL1_INVALID.
 */
string YColorSensor::get_nearRAL1(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YColorSensor::NEARRAL1_INVALID;
                }
            }
        }
        res = _nearRAL1;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the second closest RAL Classic color to the estimated color, with a similarity ratio.
 *
 * @return a string corresponding to the second closest RAL Classic color to the estimated color, with
 * a similarity ratio
 *
 * On failure, throws an exception or returns YColorSensor::NEARRAL2_INVALID.
 */
string YColorSensor::get_nearRAL2(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YColorSensor::NEARRAL2_INVALID;
                }
            }
        }
        res = _nearRAL2;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the third closest RAL Classic color to the estimated color, with a similarity ratio.
 *
 * @return a string corresponding to the third closest RAL Classic color to the estimated color, with
 * a similarity ratio
 *
 * On failure, throws an exception or returns YColorSensor::NEARRAL3_INVALID.
 */
string YColorSensor::get_nearRAL3(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YColorSensor::NEARRAL3_INVALID;
                }
            }
        }
        res = _nearRAL3;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the name of the HTML color closest to the estimated color.
 *
 * @return a string corresponding to the name of the HTML color closest to the estimated color
 *
 * On failure, throws an exception or returns YColorSensor::NEARHTMLCOLOR_INVALID.
 */
string YColorSensor::get_nearHTMLColor(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YColorSensor::NEARHTMLCOLOR_INVALID;
                }
            }
        }
        res = _nearHTMLColor;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the index of the basic color typically used to refer to the estimated color (enumerated value).
 * The list of basic colors recognized is:
 * - 0 - Brown
 * - 1 - Red
 * - 2 - Orange
 * - 3 - Yellow
 * - 4 - White
 * - 5 - Gray
 * - 6 - Black
 * - 7 - Green
 * - 8 - Blue
 * - 9 - Purple
 * - 10 - Pink
 *
 * @return a value among YColorSensor::NEARSIMPLECOLORINDEX_BROWN,
 * YColorSensor::NEARSIMPLECOLORINDEX_RED, YColorSensor::NEARSIMPLECOLORINDEX_ORANGE,
 * YColorSensor::NEARSIMPLECOLORINDEX_YELLOW, YColorSensor::NEARSIMPLECOLORINDEX_WHITE,
 * YColorSensor::NEARSIMPLECOLORINDEX_GRAY, YColorSensor::NEARSIMPLECOLORINDEX_BLACK,
 * YColorSensor::NEARSIMPLECOLORINDEX_GREEN, YColorSensor::NEARSIMPLECOLORINDEX_BLUE,
 * YColorSensor::NEARSIMPLECOLORINDEX_PURPLE and YColorSensor::NEARSIMPLECOLORINDEX_PINK corresponding
 * to the index of the basic color typically used to refer to the estimated color (enumerated value)
 *
 * On failure, throws an exception or returns YColorSensor::NEARSIMPLECOLORINDEX_INVALID.
 */
Y_NEARSIMPLECOLORINDEX_enum YColorSensor::get_nearSimpleColorIndex(void)
{
    Y_NEARSIMPLECOLORINDEX_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YColorSensor::NEARSIMPLECOLORINDEX_INVALID;
                }
            }
        }
        res = _nearSimpleColorIndex;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the name of the basic color typically used to refer to the estimated color.
 *
 * @return a string corresponding to the name of the basic color typically used to refer to the estimated color
 *
 * On failure, throws an exception or returns YColorSensor::NEARSIMPLECOLOR_INVALID.
 */
string YColorSensor::get_nearSimpleColor(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YColorSensor::NEARSIMPLECOLOR_INVALID;
                }
            }
        }
        res = _nearSimpleColor;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Retrieves a color sensor for a given identifier.
 * The identifier can be specified using several formats:
 *
 * - FunctionLogicalName
 * - ModuleSerialNumber.FunctionIdentifier
 * - ModuleSerialNumber.FunctionLogicalName
 * - ModuleLogicalName.FunctionIdentifier
 * - ModuleLogicalName.FunctionLogicalName
 *
 *
 * This function does not require that the color sensor is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the color sensor is
 * indeed online at a given time. In case of ambiguity when looking for
 * a color sensor by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the color sensor, for instance
 *         MyDevice.colorSensor.
 *
 * @return a YColorSensor object allowing you to drive the color sensor.
 */
YColorSensor* YColorSensor::FindColorSensor(string func)
{
    YColorSensor* obj = NULL;
    int taken = 0;
    if (YAPI::_apiInitialized) {
        yEnterCriticalSection(&YAPI::_global_cs);
        taken = 1;
    }try {
        obj = (YColorSensor*) YFunction::_FindFromCache("ColorSensor", func);
        if (obj == NULL) {
            obj = new YColorSensor(func);
            YFunction::_AddToCache("ColorSensor", func, obj);
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
 * The callback is called once when it is registered, passing the current advertised value
 * of the function, provided that it is not an empty string.
 * The callback is then invoked only during the execution of ySleep or yHandleEvents.
 * This provides control over the time when the callback is triggered. For good responsiveness, remember to call
 * one of these two functions periodically. To unregister a callback, pass a NULL pointer as argument.
 *
 * @param callback : the callback function to call, or a NULL pointer. The callback function should take two
 *         arguments: the function object of which the value has changed, and the character string describing
 *         the new advertised value.
 * @noreturn
 */
int YColorSensor::registerValueCallback(YColorSensorValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackColorSensor = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YColorSensor::_invokeValueCallback(string value)
{
    if (_valueCallbackColorSensor != NULL) {
        _valueCallbackColorSensor(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

/**
 * Changes the sensor automatic gain control settings.
 * Remember to call the saveToFlash() method of the module if the modification must be kept.
 *
 * @param channel : reference channel to use for automated gain control.
 * @param minRaw : lower threshold for the measured raw value, below which the gain is
 *         automatically increased as long as possible.
 * @param maxRaw : high threshold for the measured raw value, over which the gain is
 *         automatically decreased as long as possible.
 * @param noSatur : enables gain reduction in case of sensor saturation.
 *
 * @return YAPI::SUCCESS if the operation completes successfully.
 *         On failure, throws an exception or returns a negative error code.
 */
int YColorSensor::configureAutoGain(string channel,int minRaw,int maxRaw,bool noSatur)
{
    string opt;
    if (noSatur) {
        opt = "nosat";
    } else {
        opt = "";
    }

    return this->set_autoGain(YapiWrapper::ysprintf("%d < %s < %d:%s",minRaw,channel.c_str(),maxRaw,opt.c_str()));
}

/**
 * Turns on the built-in illumination LEDs using the same current as used during the latest calibration.
 * On failure, throws an exception or returns a negative error code.
 */
int YColorSensor::turnLedOn(void)
{
    return this->set_ledCurrent(this->get_ledCalibration());
}

/**
 * Turns off the built-in illumination LEDs.
 * On failure, throws an exception or returns a negative error code.
 */
int YColorSensor::turnLedOff(void)
{
    return this->set_ledCurrent(0);
}

YColorSensor *YColorSensor::nextColorSensor(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YColorSensor::FindColorSensor(hwid);
}

YColorSensor *YColorSensor::FirstColorSensor(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("ColorSensor", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YColorSensor::FindColorSensor(serial+"."+funcId);
}

//--- (end of YColorSensor implementation)

//--- (YColorSensor functions)
//--- (end of YColorSensor functions)
