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
#define Y_SATURATION_INVALID            (YAPI_INVALID_UINT)
#define Y_LEDCURRENT_INVALID            (YAPI_INVALID_UINT)
#define Y_LEDCALIBRATION_INVALID        (YAPI_INVALID_UINT)
#define Y_INTEGRATIONTIME_INVALID       (YAPI_INVALID_UINT)
#define Y_GAIN_INVALID                  (YAPI_INVALID_UINT)
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
 * It inherits from YSensor class the core functions to read measurements,
 * to register callback functions, and to access the autonomous datalogger.
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
    int             _saturation;
    int             _ledCurrent;
    int             _ledCalibration;
    int             _integrationTime;
    int             _gain;
    int             _estimatedRGB;
    int             _estimatedHSL;
    string          _estimatedXYZ;
    string          _estimatedOkLab;
    string          _nearRAL1;
    string          _nearRAL2;
    string          _nearRAL3;
    string          _nearHTMLColor;
    string          _nearSimpleColor;
    Y_NEARSIMPLECOLORINDEX_enum _nearSimpleColorIndex;
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
    static const int SATURATION_INVALID = YAPI_INVALID_UINT;
    static const int LEDCURRENT_INVALID = YAPI_INVALID_UINT;
    static const int LEDCALIBRATION_INVALID = YAPI_INVALID_UINT;
    static const int INTEGRATIONTIME_INVALID = YAPI_INVALID_UINT;
    static const int GAIN_INVALID = YAPI_INVALID_UINT;
    static const int ESTIMATEDRGB_INVALID = YAPI_INVALID_UINT;
    static const int ESTIMATEDHSL_INVALID = YAPI_INVALID_UINT;
    static const string ESTIMATEDXYZ_INVALID;
    static const string ESTIMATEDOKLAB_INVALID;
    static const string NEARRAL1_INVALID;
    static const string NEARRAL2_INVALID;
    static const string NEARRAL3_INVALID;
    static const string NEARHTMLCOLOR_INVALID;
    static const string NEARSIMPLECOLOR_INVALID;
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

    /**
     * Returns the model for color estimation.
     *
     * @return either YColorSensor::ESTIMATIONMODEL_REFLECTION or YColorSensor::ESTIMATIONMODEL_EMISSION,
     * according to the model for color estimation
     *
     * On failure, throws an exception or returns YColorSensor::ESTIMATIONMODEL_INVALID.
     */
    Y_ESTIMATIONMODEL_enum get_estimationModel(void);

    inline Y_ESTIMATIONMODEL_enum estimationModel(void)
    { return this->get_estimationModel(); }

    /**
     * Changes the model for color estimation.
     * Remember to call the saveToFlash() method of the module if the modification must be kept.
     *
     * @param newval : either YColorSensor::ESTIMATIONMODEL_REFLECTION or
     * YColorSensor::ESTIMATIONMODEL_EMISSION, according to the model for color estimation
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_estimationModel(Y_ESTIMATIONMODEL_enum newval);
    inline int      setEstimationModel(Y_ESTIMATIONMODEL_enum newval)
    { return this->set_estimationModel(newval); }

    /**
     * Returns the active working mode.
     *
     * @return either YColorSensor::WORKINGMODE_AUTO or YColorSensor::WORKINGMODE_EXPERT, according to the
     * active working mode
     *
     * On failure, throws an exception or returns YColorSensor::WORKINGMODE_INVALID.
     */
    Y_WORKINGMODE_enum  get_workingMode(void);

    inline Y_WORKINGMODE_enum workingMode(void)
    { return this->get_workingMode(); }

    /**
     * Changes the operating mode.
     * Remember to call the saveToFlash() method of the module if the modification must be kept.
     *
     * @param newval : either YColorSensor::WORKINGMODE_AUTO or YColorSensor::WORKINGMODE_EXPERT, according
     * to the operating mode
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_workingMode(Y_WORKINGMODE_enum newval);
    inline int      setWorkingMode(Y_WORKINGMODE_enum newval)
    { return this->set_workingMode(newval); }

    /**
     * Returns the current saturation of the sensor.
     * This function updates the sensor's saturation value.
     *
     * @return an integer corresponding to the current saturation of the sensor
     *
     * On failure, throws an exception or returns YColorSensor::SATURATION_INVALID.
     */
    int                 get_saturation(void);

    inline int          saturation(void)
    { return this->get_saturation(); }

    /**
     * Returns the current value of the LED.
     *
     * @return an integer corresponding to the current value of the LED
     *
     * On failure, throws an exception or returns YColorSensor::LEDCURRENT_INVALID.
     */
    int                 get_ledCurrent(void);

    inline int          ledCurrent(void)
    { return this->get_ledCurrent(); }

    /**
     * Changes the luminosity of the module leds. The parameter is a
     * value between 0 and 254.
     *
     * @param newval : an integer corresponding to the luminosity of the module leds
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_ledCurrent(int newval);
    inline int      setLedCurrent(int newval)
    { return this->set_ledCurrent(newval); }

    /**
     * Returns the LED current at calibration.
     *
     * @return an integer corresponding to the LED current at calibration
     *
     * On failure, throws an exception or returns YColorSensor::LEDCALIBRATION_INVALID.
     */
    int                 get_ledCalibration(void);

    inline int          ledCalibration(void)
    { return this->get_ledCalibration(); }

    /**
     * Sets the LED current for calibration.
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
     * Returns the current integration time.
     * This method retrieves the integration time value
     * used for data processing and returns it as an integer or an object.
     *
     * @return an integer corresponding to the current integration time
     *
     * On failure, throws an exception or returns YColorSensor::INTEGRATIONTIME_INVALID.
     */
    int                 get_integrationTime(void);

    inline int          integrationTime(void)
    { return this->get_integrationTime(); }

    /**
     * Changes the integration time for data processing.
     * This method takes a parameter and assigns it
     * as the new integration time. This affects the duration
     * for which data is integrated before being processed.
     *
     * @param newval : an integer corresponding to the integration time for data processing
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_integrationTime(int newval);
    inline int      setIntegrationTime(int newval)
    { return this->set_integrationTime(newval); }

    /**
     * Returns the current gain.
     * This method updates the gain value.
     *
     * @return an integer corresponding to the current gain
     *
     * On failure, throws an exception or returns YColorSensor::GAIN_INVALID.
     */
    int                 get_gain(void);

    inline int          gain(void)
    { return this->get_gain(); }

    /**
     * Changes the gain for signal processing.
     * This method takes a parameter and assigns it
     * as the new gain. This affects the sensitivity and
     * intensity of the processed signal.
     *
     * @param newval : an integer corresponding to the gain for signal processing
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_gain(int newval);
    inline int      setGain(int newval)
    { return this->set_gain(newval); }

    /**
     * Returns the estimated color in RGB format (0xRRGGBB).
     *
     * @return an integer corresponding to the estimated color in RGB format (0xRRGGBB)
     *
     * On failure, throws an exception or returns YColorSensor::ESTIMATEDRGB_INVALID.
     */
    int                 get_estimatedRGB(void);

    inline int          estimatedRGB(void)
    { return this->get_estimatedRGB(); }

    /**
     * Returns the estimated color in HSL (Hue, Saturation, Lightness) format.
     *
     * @return an integer corresponding to the estimated color in HSL (Hue, Saturation, Lightness) format
     *
     * On failure, throws an exception or returns YColorSensor::ESTIMATEDHSL_INVALID.
     */
    int                 get_estimatedHSL(void);

    inline int          estimatedHSL(void)
    { return this->get_estimatedHSL(); }

    /**
     * Returns the estimated color in XYZ format.
     *
     * @return a string corresponding to the estimated color in XYZ format
     *
     * On failure, throws an exception or returns YColorSensor::ESTIMATEDXYZ_INVALID.
     */
    string              get_estimatedXYZ(void);

    inline string       estimatedXYZ(void)
    { return this->get_estimatedXYZ(); }

    /**
     * Returns the estimated color in OkLab format.
     *
     * @return a string corresponding to the estimated color in OkLab format
     *
     * On failure, throws an exception or returns YColorSensor::ESTIMATEDOKLAB_INVALID.
     */
    string              get_estimatedOkLab(void);

    inline string       estimatedOkLab(void)
    { return this->get_estimatedOkLab(); }

    /**
     * Returns the estimated color in RAL format.
     *
     * @return a string corresponding to the estimated color in RAL format
     *
     * On failure, throws an exception or returns YColorSensor::NEARRAL1_INVALID.
     */
    string              get_nearRAL1(void);

    inline string       nearRAL1(void)
    { return this->get_nearRAL1(); }

    /**
     * Returns the estimated color in RAL format.
     *
     * @return a string corresponding to the estimated color in RAL format
     *
     * On failure, throws an exception or returns YColorSensor::NEARRAL2_INVALID.
     */
    string              get_nearRAL2(void);

    inline string       nearRAL2(void)
    { return this->get_nearRAL2(); }

    /**
     * Returns the estimated color in RAL format.
     *
     * @return a string corresponding to the estimated color in RAL format
     *
     * On failure, throws an exception or returns YColorSensor::NEARRAL3_INVALID.
     */
    string              get_nearRAL3(void);

    inline string       nearRAL3(void)
    { return this->get_nearRAL3(); }

    /**
     * Returns the estimated HTML color .
     *
     * @return a string corresponding to the estimated HTML color
     *
     * On failure, throws an exception or returns YColorSensor::NEARHTMLCOLOR_INVALID.
     */
    string              get_nearHTMLColor(void);

    inline string       nearHTMLColor(void)
    { return this->get_nearHTMLColor(); }

    /**
     * Returns the estimated color .
     *
     * @return a string corresponding to the estimated color
     *
     * On failure, throws an exception or returns YColorSensor::NEARSIMPLECOLOR_INVALID.
     */
    string              get_nearSimpleColor(void);

    inline string       nearSimpleColor(void)
    { return this->get_nearSimpleColor(); }

    /**
     * Returns the estimated color as an index.
     *
     * @return a value among YColorSensor::NEARSIMPLECOLORINDEX_BROWN,
     * YColorSensor::NEARSIMPLECOLORINDEX_RED, YColorSensor::NEARSIMPLECOLORINDEX_ORANGE,
     * YColorSensor::NEARSIMPLECOLORINDEX_YELLOW, YColorSensor::NEARSIMPLECOLORINDEX_WHITE,
     * YColorSensor::NEARSIMPLECOLORINDEX_GRAY, YColorSensor::NEARSIMPLECOLORINDEX_BLACK,
     * YColorSensor::NEARSIMPLECOLORINDEX_GREEN, YColorSensor::NEARSIMPLECOLORINDEX_BLUE,
     * YColorSensor::NEARSIMPLECOLORINDEX_PURPLE and YColorSensor::NEARSIMPLECOLORINDEX_PINK corresponding
     * to the estimated color as an index
     *
     * On failure, throws an exception or returns YColorSensor::NEARSIMPLECOLORINDEX_INVALID.
     */
    Y_NEARSIMPLECOLORINDEX_enum get_nearSimpleColorIndex(void);

    inline Y_NEARSIMPLECOLORINDEX_enum nearSimpleColorIndex(void)
    { return this->get_nearSimpleColorIndex(); }

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
     * Turns on the LEDs at the current used during calibration.
     * On failure, throws an exception or returns YColorSensor::DATA_INVALID.
     */
    virtual int         turnLedOn(void);

    /**
     * Turns off the LEDs.
     * On failure, throws an exception or returns YColorSensor::DATA_INVALID.
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
