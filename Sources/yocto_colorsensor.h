/*********************************************************************
 *
 *  $Id: svn_id $
 *
 *  Declares yFindColorSensor(), the high-level API for ColorSensor functions
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


#ifndef YOCTO_COLORSENSOR_H
#define YOCTO_COLORSENSOR_H

#include <cfloat>
#include <cmath>

#include "yocto_api.h"

#ifdef YOCTOLIB_NAMESPACE
namespace YOCTOLIB_NAMESPACE
{
#endif

//--- (YColorSensor return codes)
//--- (end of YColorSensor return codes)
//--- (YColorSensor yapiwrapper)
//--- (end of YColorSensor yapiwrapper)
//--- (YColorSensor definitions)
class YColorSensor; // forward declaration

typedef void (*YColorSensorValueCallback)(YColorSensor *func, const string& functionValue);
#ifndef _Y_ESTIMATIONMODEL_ENUM
#define _Y_ESTIMATIONMODEL_ENUM
typedef enum {
    Y_ESTIMATIONMODEL_REFLECTION = 0,
    Y_ESTIMATIONMODEL_EMISSION = 1,
    Y_ESTIMATIONMODEL_INVALID = -1,
} Y_ESTIMATIONMODEL_enum;
#endif
#ifndef _Y_WORKINGMODE_ENUM
#define _Y_WORKINGMODE_ENUM
typedef enum {
    Y_WORKINGMODE_AUTO = 0,
    Y_WORKINGMODE_EXPERT = 1,
    Y_WORKINGMODE_INVALID = -1,
} Y_WORKINGMODE_enum;
#endif
#ifndef _Y_NEARSIMPLECOLORINDEX_ENUM
#define _Y_NEARSIMPLECOLORINDEX_ENUM
typedef enum {
    Y_NEARSIMPLECOLORINDEX_BROWN = 0,
    Y_NEARSIMPLECOLORINDEX_RED = 1,
    Y_NEARSIMPLECOLORINDEX_ORANGE = 2,
    Y_NEARSIMPLECOLORINDEX_YELLOW = 3,
    Y_NEARSIMPLECOLORINDEX_WHITE = 4,
    Y_NEARSIMPLECOLORINDEX_GRAY = 5,
    Y_NEARSIMPLECOLORINDEX_BLACK = 6,
    Y_NEARSIMPLECOLORINDEX_GREEN = 7,
    Y_NEARSIMPLECOLORINDEX_BLUE = 8,
    Y_NEARSIMPLECOLORINDEX_PURPLE = 9,
    Y_NEARSIMPLECOLORINDEX_PINK = 10,
    Y_NEARSIMPLECOLORINDEX_INVALID = -1,
} Y_NEARSIMPLECOLORINDEX_enum;
#endif
#define Y_LEDCURRENT_INVALID            (YAPI_INVALID_UINT)
#define Y_LEDCALIBRATION_INVALID        (YAPI_INVALID_UINT)
#define Y_INTEGRATIONTIME_INVALID       (YAPI_INVALID_UINT)
#define Y_GAIN_INVALID                  (YAPI_INVALID_UINT)
#define Y_SATURATION_INVALID            (YAPI_INVALID_UINT)
#define Y_ESTIMATEDRGB_INVALID          (YAPI_INVALID_UINT)
#define Y_ESTIMATEDHSL_INVALID          (YAPI_INVALID_UINT)
#define Y_ESTIMATEDXYZ_INVALID          (YAPI_INVALID_STRING)
#define Y_ESTIMATEDOKLAB_INVALID        (YAPI_INVALID_STRING)
#define Y_NEARRAL1_INVALID              (YAPI_INVALID_STRING)
#define Y_NEARRAL2_INVALID              (YAPI_INVALID_STRING)
#define Y_NEARRAL3_INVALID              (YAPI_INVALID_STRING)
#define Y_NEARHTMLCOLOR_INVALID         (YAPI_INVALID_STRING)
#define Y_NEARSIMPLECOLOR_INVALID       (YAPI_INVALID_STRING)
//--- (end of YColorSensor definitions)

//--- (YColorSensor declaration)
/**
 * YColorSensor Class: color sensor control interface
 *
 * The YColorSensor class allows you to read and configure Yoctopuce color sensors.
 */
class YOCTO_CLASS_EXPORT YColorSensor: public YFunction {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of YColorSensor declaration)
protected:
    //--- (YColorSensor attributes)
    // Attributes (function value cache)
    Y_ESTIMATIONMODEL_enum _estimationModel;
    Y_WORKINGMODE_enum _workingMode;
    int             _ledCurrent;
    int             _ledCalibration;
    int             _integrationTime;
    int             _gain;
    int             _saturation;
    int             _estimatedRGB;
    int             _estimatedHSL;
    string          _estimatedXYZ;
    string          _estimatedOkLab;
    string          _nearRAL1;
    string          _nearRAL2;
    string          _nearRAL3;
    string          _nearHTMLColor;
    Y_NEARSIMPLECOLORINDEX_enum _nearSimpleColorIndex;
    string          _nearSimpleColor;
    YColorSensorValueCallback _valueCallbackColorSensor;

    friend YColorSensor *yFindColorSensor(const string& func);
    friend YColorSensor *yFirstColorSensor(void);

    // Function-specific method for parsing of JSON output and caching result
    virtual int     _parseAttr(YJSONObject *json_val);

    // Constructor is protected, use yFindColorSensor factory function to instantiate
    YColorSensor(const string& func);
    //--- (end of YColorSensor attributes)

public:
    virtual ~YColorSensor();
    //--- (YColorSensor accessors declaration)

    static const Y_ESTIMATIONMODEL_enum ESTIMATIONMODEL_REFLECTION = Y_ESTIMATIONMODEL_REFLECTION;
    static const Y_ESTIMATIONMODEL_enum ESTIMATIONMODEL_EMISSION = Y_ESTIMATIONMODEL_EMISSION;
    static const Y_ESTIMATIONMODEL_enum ESTIMATIONMODEL_INVALID = Y_ESTIMATIONMODEL_INVALID;
    static const Y_WORKINGMODE_enum WORKINGMODE_AUTO = Y_WORKINGMODE_AUTO;
    static const Y_WORKINGMODE_enum WORKINGMODE_EXPERT = Y_WORKINGMODE_EXPERT;
    static const Y_WORKINGMODE_enum WORKINGMODE_INVALID = Y_WORKINGMODE_INVALID;
    static const int LEDCURRENT_INVALID = YAPI_INVALID_UINT;
    static const int LEDCALIBRATION_INVALID = YAPI_INVALID_UINT;
    static const int INTEGRATIONTIME_INVALID = YAPI_INVALID_UINT;
    static const int GAIN_INVALID = YAPI_INVALID_UINT;
    static const int SATURATION_INVALID = YAPI_INVALID_UINT;
    static const int ESTIMATEDRGB_INVALID = YAPI_INVALID_UINT;
    static const int ESTIMATEDHSL_INVALID = YAPI_INVALID_UINT;
    static const string ESTIMATEDXYZ_INVALID;
    static const string ESTIMATEDOKLAB_INVALID;
    static const string NEARRAL1_INVALID;
    static const string NEARRAL2_INVALID;
    static const string NEARRAL3_INVALID;
    static const string NEARHTMLCOLOR_INVALID;
    static const Y_NEARSIMPLECOLORINDEX_enum NEARSIMPLECOLORINDEX_BROWN = Y_NEARSIMPLECOLORINDEX_BROWN;
    static const Y_NEARSIMPLECOLORINDEX_enum NEARSIMPLECOLORINDEX_RED = Y_NEARSIMPLECOLORINDEX_RED;
    static const Y_NEARSIMPLECOLORINDEX_enum NEARSIMPLECOLORINDEX_ORANGE = Y_NEARSIMPLECOLORINDEX_ORANGE;
    static const Y_NEARSIMPLECOLORINDEX_enum NEARSIMPLECOLORINDEX_YELLOW = Y_NEARSIMPLECOLORINDEX_YELLOW;
    static const Y_NEARSIMPLECOLORINDEX_enum NEARSIMPLECOLORINDEX_WHITE = Y_NEARSIMPLECOLORINDEX_WHITE;
    static const Y_NEARSIMPLECOLORINDEX_enum NEARSIMPLECOLORINDEX_GRAY = Y_NEARSIMPLECOLORINDEX_GRAY;
    static const Y_NEARSIMPLECOLORINDEX_enum NEARSIMPLECOLORINDEX_BLACK = Y_NEARSIMPLECOLORINDEX_BLACK;
    static const Y_NEARSIMPLECOLORINDEX_enum NEARSIMPLECOLORINDEX_GREEN = Y_NEARSIMPLECOLORINDEX_GREEN;
    static const Y_NEARSIMPLECOLORINDEX_enum NEARSIMPLECOLORINDEX_BLUE = Y_NEARSIMPLECOLORINDEX_BLUE;
    static const Y_NEARSIMPLECOLORINDEX_enum NEARSIMPLECOLORINDEX_PURPLE = Y_NEARSIMPLECOLORINDEX_PURPLE;
    static const Y_NEARSIMPLECOLORINDEX_enum NEARSIMPLECOLORINDEX_PINK = Y_NEARSIMPLECOLORINDEX_PINK;
    static const Y_NEARSIMPLECOLORINDEX_enum NEARSIMPLECOLORINDEX_INVALID = Y_NEARSIMPLECOLORINDEX_INVALID;
    static const string NEARSIMPLECOLOR_INVALID;

    /**
     * Returns the predictive model used for color estimation (reflective or emissive).
     *
     * @return either YColorSensor::ESTIMATIONMODEL_REFLECTION or YColorSensor::ESTIMATIONMODEL_EMISSION,
     * according to the predictive model used for color estimation (reflective or emissive)
     *
     * On failure, throws an exception or returns YColorSensor::ESTIMATIONMODEL_INVALID.
     */
    Y_ESTIMATIONMODEL_enum get_estimationModel(void);

    inline Y_ESTIMATIONMODEL_enum estimationModel(void)
    { return this->get_estimationModel(); }

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
    int             set_estimationModel(Y_ESTIMATIONMODEL_enum newval);
    inline int      setEstimationModel(Y_ESTIMATIONMODEL_enum newval)
    { return this->set_estimationModel(newval); }

    /**
     * Returns the sensor working mode.
     * In Auto mode, sensor parameters are automatically set based on the selected estimation model.
     * In Expert mode, sensor parameters such as gain and integration time are configured manually.
     *
     * @return either YColorSensor::WORKINGMODE_AUTO or YColorSensor::WORKINGMODE_EXPERT, according to the
     * sensor working mode
     *
     * On failure, throws an exception or returns YColorSensor::WORKINGMODE_INVALID.
     */
    Y_WORKINGMODE_enum  get_workingMode(void);

    inline Y_WORKINGMODE_enum workingMode(void)
    { return this->get_workingMode(); }

    /**
     * Changes the sensor working mode.
     * In Auto mode, sensor parameters are automatically set based on the selected estimation model.
     * In Expert mode, sensor parameters such as gain and integration time are configured manually.
     * Remember to call the saveToFlash() method of the module if the modification must be kept.
     *
     * @param newval : either YColorSensor::WORKINGMODE_AUTO or YColorSensor::WORKINGMODE_EXPERT, according
     * to the sensor working mode
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_workingMode(Y_WORKINGMODE_enum newval);
    inline int      setWorkingMode(Y_WORKINGMODE_enum newval)
    { return this->set_workingMode(newval); }

    /**
     * Returns the amount of current sent to the illumination LEDs, for reflection measures.
     * The value is an integer ranging from 0 (LEDs off) to 254 (LEDs at maximum intensity).
     *
     * @return an integer corresponding to the amount of current sent to the illumination LEDs, for reflection measures
     *
     * On failure, throws an exception or returns YColorSensor::LEDCURRENT_INVALID.
     */
    int                 get_ledCurrent(void);

    inline int          ledCurrent(void)
    { return this->get_ledCurrent(); }

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
    int             set_ledCurrent(int newval);
    inline int      setLedCurrent(int newval)
    { return this->set_ledCurrent(newval); }

    /**
     * Returns the current sent to the illumination LEDs during the latest calibration.
     *
     * @return an integer corresponding to the current sent to the illumination LEDs during the latest calibration
     *
     * On failure, throws an exception or returns YColorSensor::LEDCALIBRATION_INVALID.
     */
    int                 get_ledCalibration(void);

    inline int          ledCalibration(void)
    { return this->get_ledCalibration(); }

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
    int             set_ledCalibration(int newval);
    inline int      setLedCalibration(int newval)
    { return this->set_ledCalibration(newval); }

    /**
     * Returns the current integration time for spectral measure, in milliseconds.
     * A longer integration time increase the sensitivity for low light conditions,
     * but reduces the measure taking rate and may lead to saturation for lighter colors.
     *
     * @return an integer corresponding to the current integration time for spectral measure, in milliseconds
     *
     * On failure, throws an exception or returns YColorSensor::INTEGRATIONTIME_INVALID.
     */
    int                 get_integrationTime(void);

    inline int          integrationTime(void)
    { return this->get_integrationTime(); }

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
    int             set_integrationTime(int newval);
    inline int      setIntegrationTime(int newval)
    { return this->set_integrationTime(newval); }

    /**
     * Returns the current spectral channel detector gain exponent.
     * For a value n ranging from 0 to 12, the applied gain is 2^(n-1).
     * 0 corresponds to a gain of 0.5, and 12 corresponds to a gain of 2048.
     *
     * @return an integer corresponding to the current spectral channel detector gain exponent
     *
     * On failure, throws an exception or returns YColorSensor::GAIN_INVALID.
     */
    int                 get_gain(void);

    inline int          gain(void)
    { return this->get_gain(); }

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
    int             set_gain(int newval);
    inline int      setGain(int newval)
    { return this->set_gain(newval); }

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
    int                 get_saturation(void);

    inline int          saturation(void)
    { return this->get_saturation(); }

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
    int                 get_estimatedRGB(void);

    inline int          estimatedRGB(void)
    { return this->get_estimatedRGB(); }

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
    int                 get_estimatedHSL(void);

    inline int          estimatedHSL(void)
    { return this->get_estimatedHSL(); }

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
    string              get_estimatedXYZ(void);

    inline string       estimatedXYZ(void)
    { return this->get_estimatedXYZ(); }

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
    string              get_estimatedOkLab(void);

    inline string       estimatedOkLab(void)
    { return this->get_estimatedOkLab(); }

    /**
     * Returns the RAL Classic color closest to the estimated color, with a similarity ratio.
     *
     * @return a string corresponding to the RAL Classic color closest to the estimated color, with a similarity ratio
     *
     * On failure, throws an exception or returns YColorSensor::NEARRAL1_INVALID.
     */
    string              get_nearRAL1(void);

    inline string       nearRAL1(void)
    { return this->get_nearRAL1(); }

    /**
     * Returns the second closest RAL Classic color to the estimated color, with a similarity ratio.
     *
     * @return a string corresponding to the second closest RAL Classic color to the estimated color, with
     * a similarity ratio
     *
     * On failure, throws an exception or returns YColorSensor::NEARRAL2_INVALID.
     */
    string              get_nearRAL2(void);

    inline string       nearRAL2(void)
    { return this->get_nearRAL2(); }

    /**
     * Returns the third closest RAL Classic color to the estimated color, with a similarity ratio.
     *
     * @return a string corresponding to the third closest RAL Classic color to the estimated color, with
     * a similarity ratio
     *
     * On failure, throws an exception or returns YColorSensor::NEARRAL3_INVALID.
     */
    string              get_nearRAL3(void);

    inline string       nearRAL3(void)
    { return this->get_nearRAL3(); }

    /**
     * Returns the name of the HTML color closest to the estimated color.
     *
     * @return a string corresponding to the name of the HTML color closest to the estimated color
     *
     * On failure, throws an exception or returns YColorSensor::NEARHTMLCOLOR_INVALID.
     */
    string              get_nearHTMLColor(void);

    inline string       nearHTMLColor(void)
    { return this->get_nearHTMLColor(); }

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
    Y_NEARSIMPLECOLORINDEX_enum get_nearSimpleColorIndex(void);

    inline Y_NEARSIMPLECOLORINDEX_enum nearSimpleColorIndex(void)
    { return this->get_nearSimpleColorIndex(); }

    /**
     * Returns the name of the basic color typically used to refer to the estimated color.
     *
     * @return a string corresponding to the name of the basic color typically used to refer to the estimated color
     *
     * On failure, throws an exception or returns YColorSensor::NEARSIMPLECOLOR_INVALID.
     */
    string              get_nearSimpleColor(void);

    inline string       nearSimpleColor(void)
    { return this->get_nearSimpleColor(); }

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
    static YColorSensor* FindColorSensor(string func);

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
    virtual int         registerValueCallback(YColorSensorValueCallback callback);
    using YFunction::registerValueCallback;

    virtual int         _invokeValueCallback(string value);

    /**
     * Turns on the built-in illumination LEDs using the same current as used during the latest calibration.
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         turnLedOn(void);

    /**
     * Turns off the built-in illumination LEDs.
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         turnLedOff(void);


    inline static YColorSensor *Find(string func)
    { return YColorSensor::FindColorSensor(func); }

    /**
     * Continues the enumeration of color sensors started using yFirstColorSensor().
     * Caution: You can't make any assumption about the returned color sensors order.
     * If you want to find a specific a color sensor, use ColorSensor.findColorSensor()
     * and a hardwareID or a logical name.
     *
     * @return a pointer to a YColorSensor object, corresponding to
     *         a color sensor currently online, or a NULL pointer
     *         if there are no more color sensors to enumerate.
     */
           YColorSensor    *nextColorSensor(void);
    inline YColorSensor    *next(void)
    { return this->nextColorSensor();}

    /**
     * Starts the enumeration of color sensors currently accessible.
     * Use the method YColorSensor::nextColorSensor() to iterate on
     * next color sensors.
     *
     * @return a pointer to a YColorSensor object, corresponding to
     *         the first color sensor currently online, or a NULL pointer
     *         if there are none.
     */
           static YColorSensor *FirstColorSensor(void);
    inline static YColorSensor *First(void)
    { return YColorSensor::FirstColorSensor();}
#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of YColorSensor accessors declaration)
};

//--- (YColorSensor functions declaration)

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
inline YColorSensor *yFindColorSensor(const string& func)
{ return YColorSensor::FindColorSensor(func);}
/**
 * Starts the enumeration of color sensors currently accessible.
 * Use the method YColorSensor::nextColorSensor() to iterate on
 * next color sensors.
 *
 * @return a pointer to a YColorSensor object, corresponding to
 *         the first color sensor currently online, or a NULL pointer
 *         if there are none.
 */
inline YColorSensor *yFirstColorSensor(void)
{ return YColorSensor::FirstColorSensor();}

//--- (end of YColorSensor functions declaration)

#ifdef YOCTOLIB_NAMESPACE
// end of namespace definition
}
#endif

#endif
