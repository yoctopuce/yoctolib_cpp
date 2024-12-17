/*********************************************************************
 *
 *  $Id: svn_id $
 *
 *  Declares yFindSpectralSensor(), the high-level API for SpectralSensor functions
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


#ifndef YOCTO_SPECTRALSENSOR_H
#define YOCTO_SPECTRALSENSOR_H

#include <cfloat>
#include <cmath>

#include "yocto_api.h"

#ifdef YOCTOLIB_NAMESPACE
namespace YOCTOLIB_NAMESPACE
{
#endif

//--- (YSpectralSensor return codes)
//--- (end of YSpectralSensor return codes)
//--- (YSpectralSensor yapiwrapper)
//--- (end of YSpectralSensor yapiwrapper)
//--- (YSpectralSensor definitions)
class YSpectralSensor; // forward declaration

typedef void (*YSpectralSensorValueCallback)(YSpectralSensor *func, const string& functionValue);
#ifndef _Y_ESTIMATIONMODEL_ENUM
#define _Y_ESTIMATIONMODEL_ENUM
typedef enum {
    Y_ESTIMATIONMODEL_REFLECTION = 0,
    Y_ESTIMATIONMODEL_EMISSION = 1,
    Y_ESTIMATIONMODEL_INVALID = -1,
} Y_ESTIMATIONMODEL_enum;
#endif
#define Y_LEDCURRENT_INVALID            (YAPI_INVALID_INT)
#define Y_RESOLUTION_INVALID            (YAPI_INVALID_DOUBLE)
#define Y_INTEGRATIONTIME_INVALID       (YAPI_INVALID_INT)
#define Y_GAIN_INVALID                  (YAPI_INVALID_INT)
#define Y_SATURATION_INVALID            (YAPI_INVALID_UINT)
#define Y_ESTIMATEDRGB_INVALID          (YAPI_INVALID_UINT)
#define Y_ESTIMATEDHSL_INVALID          (YAPI_INVALID_UINT)
#define Y_ESTIMATEDXYZ_INVALID          (YAPI_INVALID_STRING)
#define Y_ESTIMATEDOKLAB_INVALID        (YAPI_INVALID_STRING)
#define Y_NEARRAL1_INVALID              (YAPI_INVALID_STRING)
#define Y_NEARRAL2_INVALID              (YAPI_INVALID_STRING)
#define Y_NEARRAL3_INVALID              (YAPI_INVALID_STRING)
#define Y_NEARHTMLCOLOR_INVALID         (YAPI_INVALID_STRING)
#define Y_LEDCURRENTATPOWERON_INVALID   (YAPI_INVALID_INT)
#define Y_INTEGRATIONTIMEATPOWERON_INVALID (YAPI_INVALID_INT)
#define Y_GAINATPOWERON_INVALID         (YAPI_INVALID_INT)
//--- (end of YSpectralSensor definitions)

//--- (YSpectralSensor declaration)
/**
 * YSpectralSensor Class: spectral sensor control interface
 *
 * The YSpectralSensor class allows you to read and configure Yoctopuce spectral sensors.
 * It inherits from YSensor class the core functions to read measurements,
 * to register callback functions, and to access the autonomous datalogger.
 */
class YOCTO_CLASS_EXPORT YSpectralSensor: public YFunction {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of YSpectralSensor declaration)
protected:
    //--- (YSpectralSensor attributes)
    // Attributes (function value cache)
    int             _ledCurrent;
    double          _resolution;
    int             _integrationTime;
    int             _gain;
    Y_ESTIMATIONMODEL_enum _estimationModel;
    int             _saturation;
    int             _estimatedRGB;
    int             _estimatedHSL;
    string          _estimatedXYZ;
    string          _estimatedOkLab;
    string          _nearRAL1;
    string          _nearRAL2;
    string          _nearRAL3;
    string          _nearHTMLColor;
    int             _ledCurrentAtPowerOn;
    int             _integrationTimeAtPowerOn;
    int             _gainAtPowerOn;
    YSpectralSensorValueCallback _valueCallbackSpectralSensor;

    friend YSpectralSensor *yFindSpectralSensor(const string& func);
    friend YSpectralSensor *yFirstSpectralSensor(void);

    // Function-specific method for parsing of JSON output and caching result
    virtual int     _parseAttr(YJSONObject *json_val);

    // Constructor is protected, use yFindSpectralSensor factory function to instantiate
    YSpectralSensor(const string& func);
    //--- (end of YSpectralSensor attributes)

public:
    virtual ~YSpectralSensor();
    //--- (YSpectralSensor accessors declaration)

    static const int LEDCURRENT_INVALID = YAPI_INVALID_INT;
    static const double RESOLUTION_INVALID;
    static const int INTEGRATIONTIME_INVALID = YAPI_INVALID_INT;
    static const int GAIN_INVALID = YAPI_INVALID_INT;
    static const Y_ESTIMATIONMODEL_enum ESTIMATIONMODEL_REFLECTION = Y_ESTIMATIONMODEL_REFLECTION;
    static const Y_ESTIMATIONMODEL_enum ESTIMATIONMODEL_EMISSION = Y_ESTIMATIONMODEL_EMISSION;
    static const Y_ESTIMATIONMODEL_enum ESTIMATIONMODEL_INVALID = Y_ESTIMATIONMODEL_INVALID;
    static const int SATURATION_INVALID = YAPI_INVALID_UINT;
    static const int ESTIMATEDRGB_INVALID = YAPI_INVALID_UINT;
    static const int ESTIMATEDHSL_INVALID = YAPI_INVALID_UINT;
    static const string ESTIMATEDXYZ_INVALID;
    static const string ESTIMATEDOKLAB_INVALID;
    static const string NEARRAL1_INVALID;
    static const string NEARRAL2_INVALID;
    static const string NEARRAL3_INVALID;
    static const string NEARHTMLCOLOR_INVALID;
    static const int LEDCURRENTATPOWERON_INVALID = YAPI_INVALID_INT;
    static const int INTEGRATIONTIMEATPOWERON_INVALID = YAPI_INVALID_INT;
    static const int GAINATPOWERON_INVALID = YAPI_INVALID_INT;

    /**
     * Returns the current value of the LED.
     * This method retrieves the current flowing through the LED
     * and returns it as an integer or an object.
     *
     * @return an integer corresponding to the current value of the LED
     *
     * On failure, throws an exception or returns YSpectralSensor::LEDCURRENT_INVALID.
     */
    int                 get_ledCurrent(void);

    inline int          ledCurrent(void)
    { return this->get_ledCurrent(); }

    /**
     * Changes the luminosity of the module leds. The parameter is a
     * value between 0 and 100.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
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
     * Changes the resolution of the measured physical values. The resolution corresponds to the numerical precision
     * when displaying value. It does not change the precision of the measure itself.
     * Remember to call the saveToFlash() method of the module if the modification must be kept.
     *
     * @param newval : a floating point number corresponding to the resolution of the measured physical values
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_resolution(double newval);
    inline int      setResolution(double newval)
    { return this->set_resolution(newval); }

    /**
     * Returns the resolution of the measured values. The resolution corresponds to the numerical precision
     * of the measures, which is not always the same as the actual precision of the sensor.
     * Remember to call the saveToFlash() method of the module if the modification must be kept.
     *
     * @return a floating point number corresponding to the resolution of the measured values
     *
     * On failure, throws an exception or returns YSpectralSensor::RESOLUTION_INVALID.
     */
    double              get_resolution(void);

    inline double       resolution(void)
    { return this->get_resolution(); }

    /**
     * Returns the current integration time.
     * This method retrieves the integration time value
     * used for data processing and returns it as an integer or an object.
     *
     * @return an integer corresponding to the current integration time
     *
     * On failure, throws an exception or returns YSpectralSensor::INTEGRATIONTIME_INVALID.
     */
    int                 get_integrationTime(void);

    inline int          integrationTime(void)
    { return this->get_integrationTime(); }

    /**
     * Sets the integration time for data processing.
     * This method takes a parameter `val` and assigns it
     * as the new integration time. This affects the duration
     * for which data is integrated before being processed.
     *
     * @param newval : an integer
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_integrationTime(int newval);
    inline int      setIntegrationTime(int newval)
    { return this->set_integrationTime(newval); }

    /**
     * Retrieves the current gain.
     * This method updates the gain value.
     *
     * @return an integer
     *
     * On failure, throws an exception or returns YSpectralSensor::GAIN_INVALID.
     */
    int                 get_gain(void);

    inline int          gain(void)
    { return this->get_gain(); }

    /**
     * Sets the gain for signal processing.
     * This method takes a parameter `val` and assigns it
     * as the new gain. This affects the sensitivity and
     * intensity of the processed signal.
     *
     * @param newval : an integer
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_gain(int newval);
    inline int      setGain(int newval)
    { return this->set_gain(newval); }

    /**
     * Return the model for the estimation colors.
     *
     * @return either YSpectralSensor::ESTIMATIONMODEL_REFLECTION or YSpectralSensor::ESTIMATIONMODEL_EMISSION
     *
     * On failure, throws an exception or returns YSpectralSensor::ESTIMATIONMODEL_INVALID.
     */
    Y_ESTIMATIONMODEL_enum get_estimationModel(void);

    inline Y_ESTIMATIONMODEL_enum estimationModel(void)
    { return this->get_estimationModel(); }

    /**
     * Change the model for the estimation colors.
     *
     * @param newval : either YSpectralSensor::ESTIMATIONMODEL_REFLECTION or YSpectralSensor::ESTIMATIONMODEL_EMISSION
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_estimationModel(Y_ESTIMATIONMODEL_enum newval);
    inline int      setEstimationModel(Y_ESTIMATIONMODEL_enum newval)
    { return this->set_estimationModel(newval); }

    /**
     * Returns the current saturation of the sensor.
     * This function updates the sensor's saturation value.
     *
     * @return an integer corresponding to the current saturation of the sensor
     *
     * On failure, throws an exception or returns YSpectralSensor::SATURATION_INVALID.
     */
    int                 get_saturation(void);

    inline int          saturation(void)
    { return this->get_saturation(); }

    /**
     * Returns the estimated color in RGB format.
     * This method retrieves the estimated color values
     * and returns them as an RGB object or structure.
     *
     * @return an integer corresponding to the estimated color in RGB format
     *
     * On failure, throws an exception or returns YSpectralSensor::ESTIMATEDRGB_INVALID.
     */
    int                 get_estimatedRGB(void);

    inline int          estimatedRGB(void)
    { return this->get_estimatedRGB(); }

    /**
     * Returns the estimated color in HSL format.
     * This method retrieves the estimated color values
     * and returns them as an HSL object or structure.
     *
     * @return an integer corresponding to the estimated color in HSL format
     *
     * On failure, throws an exception or returns YSpectralSensor::ESTIMATEDHSL_INVALID.
     */
    int                 get_estimatedHSL(void);

    inline int          estimatedHSL(void)
    { return this->get_estimatedHSL(); }

    /**
     * Returns the estimated color in XYZ format.
     * This method retrieves the estimated color values
     * and returns them as an XYZ object or structure.
     *
     * @return a string corresponding to the estimated color in XYZ format
     *
     * On failure, throws an exception or returns YSpectralSensor::ESTIMATEDXYZ_INVALID.
     */
    string              get_estimatedXYZ(void);

    inline string       estimatedXYZ(void)
    { return this->get_estimatedXYZ(); }

    /**
     * Returns the estimated color in OkLab format.
     * This method retrieves the estimated color values
     * and returns them as an OkLab object or structure.
     *
     * @return a string corresponding to the estimated color in OkLab format
     *
     * On failure, throws an exception or returns YSpectralSensor::ESTIMATEDOKLAB_INVALID.
     */
    string              get_estimatedOkLab(void);

    inline string       estimatedOkLab(void)
    { return this->get_estimatedOkLab(); }

    string              get_nearRAL1(void);

    inline string       nearRAL1(void)
    { return this->get_nearRAL1(); }

    string              get_nearRAL2(void);

    inline string       nearRAL2(void)
    { return this->get_nearRAL2(); }

    string              get_nearRAL3(void);

    inline string       nearRAL3(void)
    { return this->get_nearRAL3(); }

    string              get_nearHTMLColor(void);

    inline string       nearHTMLColor(void)
    { return this->get_nearHTMLColor(); }

    int                 get_ledCurrentAtPowerOn(void);

    inline int          ledCurrentAtPowerOn(void)
    { return this->get_ledCurrentAtPowerOn(); }

    /**
     * Sets the LED current at power-on.
     * This method takes a parameter `val` and assigns it to startupLumin, representing the LED current defined
     * at startup.
     * Remember to call the saveToFlash() method of the module if the modification must be kept.
     *
     * @param newval : an integer
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_ledCurrentAtPowerOn(int newval);
    inline int      setLedCurrentAtPowerOn(int newval)
    { return this->set_ledCurrentAtPowerOn(newval); }

    /**
     * Retrieves the integration time at power-on.
     * This method updates the power-on integration time value.
     *
     * @return an integer
     *
     * On failure, throws an exception or returns YSpectralSensor::INTEGRATIONTIMEATPOWERON_INVALID.
     */
    int                 get_integrationTimeAtPowerOn(void);

    inline int          integrationTimeAtPowerOn(void)
    { return this->get_integrationTimeAtPowerOn(); }

    /**
     * Sets the integration time at power-on.
     * This method takes a parameter `val` and assigns it to startupIntegTime, representing the integration time
     * defined at startup.
     * Remember to call the saveToFlash() method of the module if the modification must be kept.
     *
     * @param newval : an integer
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_integrationTimeAtPowerOn(int newval);
    inline int      setIntegrationTimeAtPowerOn(int newval)
    { return this->set_integrationTimeAtPowerOn(newval); }

    int                 get_gainAtPowerOn(void);

    inline int          gainAtPowerOn(void)
    { return this->get_gainAtPowerOn(); }

    /**
     * Sets the gain at power-on.
     * This method takes a parameter `val` and assigns it to startupGain, representing the gain defined at startup.
     * Remember to call the saveToFlash() method of the module if the modification must be kept.
     *
     * @param newval : an integer
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_gainAtPowerOn(int newval);
    inline int      setGainAtPowerOn(int newval)
    { return this->set_gainAtPowerOn(newval); }

    /**
     * Retrieves a spectral sensor for a given identifier.
     * The identifier can be specified using several formats:
     *
     * - FunctionLogicalName
     * - ModuleSerialNumber.FunctionIdentifier
     * - ModuleSerialNumber.FunctionLogicalName
     * - ModuleLogicalName.FunctionIdentifier
     * - ModuleLogicalName.FunctionLogicalName
     *
     *
     * This function does not require that the spectral sensor is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method isOnline() to test if the spectral sensor is
     * indeed online at a given time. In case of ambiguity when looking for
     * a spectral sensor by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     *
     * If a call to this object's is_online() method returns FALSE although
     * you are certain that the matching device is plugged, make sure that you did
     * call registerHub() at application initialization time.
     *
     * @param func : a string that uniquely characterizes the spectral sensor, for instance
     *         MyDevice.spectralSensor.
     *
     * @return a YSpectralSensor object allowing you to drive the spectral sensor.
     */
    static YSpectralSensor* FindSpectralSensor(string func);

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
    virtual int         registerValueCallback(YSpectralSensorValueCallback callback);
    using YFunction::registerValueCallback;

    virtual int         _invokeValueCallback(string value);


    inline static YSpectralSensor *Find(string func)
    { return YSpectralSensor::FindSpectralSensor(func); }

    /**
     * Continues the enumeration of spectral sensors started using yFirstSpectralSensor().
     * Caution: You can't make any assumption about the returned spectral sensors order.
     * If you want to find a specific a spectral sensor, use SpectralSensor.findSpectralSensor()
     * and a hardwareID or a logical name.
     *
     * @return a pointer to a YSpectralSensor object, corresponding to
     *         a spectral sensor currently online, or a NULL pointer
     *         if there are no more spectral sensors to enumerate.
     */
           YSpectralSensor *nextSpectralSensor(void);
    inline YSpectralSensor *next(void)
    { return this->nextSpectralSensor();}

    /**
     * Starts the enumeration of spectral sensors currently accessible.
     * Use the method YSpectralSensor::nextSpectralSensor() to iterate on
     * next spectral sensors.
     *
     * @return a pointer to a YSpectralSensor object, corresponding to
     *         the first spectral sensor currently online, or a NULL pointer
     *         if there are none.
     */
           static YSpectralSensor *FirstSpectralSensor(void);
    inline static YSpectralSensor *First(void)
    { return YSpectralSensor::FirstSpectralSensor();}
#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of YSpectralSensor accessors declaration)
};

//--- (YSpectralSensor functions declaration)

/**
 * Retrieves a spectral sensor for a given identifier.
 * The identifier can be specified using several formats:
 *
 * - FunctionLogicalName
 * - ModuleSerialNumber.FunctionIdentifier
 * - ModuleSerialNumber.FunctionLogicalName
 * - ModuleLogicalName.FunctionIdentifier
 * - ModuleLogicalName.FunctionLogicalName
 *
 *
 * This function does not require that the spectral sensor is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the spectral sensor is
 * indeed online at a given time. In case of ambiguity when looking for
 * a spectral sensor by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the spectral sensor, for instance
 *         MyDevice.spectralSensor.
 *
 * @return a YSpectralSensor object allowing you to drive the spectral sensor.
 */
inline YSpectralSensor *yFindSpectralSensor(const string& func)
{ return YSpectralSensor::FindSpectralSensor(func);}
/**
 * Starts the enumeration of spectral sensors currently accessible.
 * Use the method YSpectralSensor::nextSpectralSensor() to iterate on
 * next spectral sensors.
 *
 * @return a pointer to a YSpectralSensor object, corresponding to
 *         the first spectral sensor currently online, or a NULL pointer
 *         if there are none.
 */
inline YSpectralSensor *yFirstSpectralSensor(void)
{ return YSpectralSensor::FirstSpectralSensor();}

//--- (end of YSpectralSensor functions declaration)

#ifdef YOCTOLIB_NAMESPACE
// end of namespace definition
}
#endif

#endif
