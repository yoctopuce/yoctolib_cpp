/*********************************************************************
 *
 * $Id: yocto_carbondioxide.h 12324 2013-08-13 15:10:31Z mvuilleu $
 *
 * Declares yFindCarbonDioxide(), the high-level API for CarbonDioxide functions
 *
 * - - - - - - - - - License information: - - - - - - - - - 
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


#ifndef YOCTO_CARBONDIOXIDE_H
#define YOCTO_CARBONDIOXIDE_H

#include "yocto_api.h"
#include <cfloat>
#include <cmath>
#include <map>

//--- (return codes)
//--- (end of return codes)
//--- (YCarbonDioxide definitions)
class YCarbonDioxide; //forward declaration

typedef void (*YCarbonDioxideUpdateCallback)(YCarbonDioxide *func, const string& functionValue);
#define Y_LOGICALNAME_INVALID           (YAPI::INVALID_STRING)
#define Y_ADVERTISEDVALUE_INVALID       (YAPI::INVALID_STRING)
#define Y_UNIT_INVALID                  (YAPI::INVALID_STRING)
#define Y_CURRENTVALUE_INVALID          (-DBL_MAX)
#define Y_LOWESTVALUE_INVALID           (-DBL_MAX)
#define Y_HIGHESTVALUE_INVALID          (-DBL_MAX)
#define Y_CURRENTRAWVALUE_INVALID       (-DBL_MAX)
#define Y_CALIBRATIONPARAM_INVALID      (YAPI::INVALID_STRING)
#define Y_RESOLUTION_INVALID            (-DBL_MAX)
//--- (end of YCarbonDioxide definitions)

//--- (YCarbonDioxide declaration)
/**
 * YCarbonDioxide Class: CarbonDioxide function interface
 * 
 * The Yoctopuce application programming interface allows you to read an instant
 * measure of the sensor, as well as the minimal and maximal values observed.
 */
class YCarbonDioxide: public YFunction {
protected:
    // Attributes (function value cache)
    YCarbonDioxideUpdateCallback _callback;
    string          _logicalName;
    string          _advertisedValue;
    string          _unit;
    double          _currentValue;
    double          _lowestValue;
    double          _highestValue;
    double          _currentRawValue;
    string          _calibrationParam;
    double          _resolution;
    int             _calibrationOffset;

    friend YCarbonDioxide *yFindCarbonDioxide(const string& func);
    friend YCarbonDioxide *yFirstCarbonDioxide(void);

    // Function-specific method for parsing of JSON output and caching result
    int             _parse(yJsonStateMachine& j);
    //--- (end of YCarbonDioxide declaration)

    //--- (YCarbonDioxide constructor)
    // Constructor is protected, use yFindCarbonDioxide factory function to instantiate
    YCarbonDioxide(const string& func);
    //--- (end of YCarbonDioxide constructor)
    //--- (CarbonDioxide initialization)
    //--- (end of CarbonDioxide initialization)

public:
    ~YCarbonDioxide();
    //--- (YCarbonDioxide accessors declaration)

    static const string LOGICALNAME_INVALID;
    static const string ADVERTISEDVALUE_INVALID;
    static const string UNIT_INVALID;
    static const double CURRENTVALUE_INVALID;
    static const double LOWESTVALUE_INVALID;
    static const double HIGHESTVALUE_INVALID;
    static const double CURRENTRAWVALUE_INVALID;
    static const string CALIBRATIONPARAM_INVALID;
    static const double RESOLUTION_INVALID;

    /**
     * Returns the logical name of the CO2 sensor.
     * 
     * @return a string corresponding to the logical name of the CO2 sensor
     * 
     * On failure, throws an exception or returns Y_LOGICALNAME_INVALID.
     */
           string          get_logicalName(void);
    inline string          logicalName(void)
    { return this->get_logicalName(); }

    /**
     * Changes the logical name of the CO2 sensor. You can use yCheckLogicalName()
     * prior to this call to make sure that your parameter is valid.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     * 
     * @param newval : a string corresponding to the logical name of the CO2 sensor
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_logicalName(const string& newval);
    inline int      setLogicalName(const string& newval)
    { return this->set_logicalName(newval); }

    /**
     * Returns the current value of the CO2 sensor (no more than 6 characters).
     * 
     * @return a string corresponding to the current value of the CO2 sensor (no more than 6 characters)
     * 
     * On failure, throws an exception or returns Y_ADVERTISEDVALUE_INVALID.
     */
           string          get_advertisedValue(void);
    inline string          advertisedValue(void)
    { return this->get_advertisedValue(); }

    /**
     * Returns the measuring unit for the measured value.
     * 
     * @return a string corresponding to the measuring unit for the measured value
     * 
     * On failure, throws an exception or returns Y_UNIT_INVALID.
     */
           string          get_unit(void);
    inline string          unit(void)
    { return this->get_unit(); }

    /**
     * Returns the current measured value.
     * 
     * @return a floating point number corresponding to the current measured value
     * 
     * On failure, throws an exception or returns Y_CURRENTVALUE_INVALID.
     */
           double          get_currentValue(void);
    inline double          currentValue(void)
    { return this->get_currentValue(); }

    /**
     * Changes the recorded minimal value observed.
     * 
     * @param newval : a floating point number corresponding to the recorded minimal value observed
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_lowestValue(double newval);
    inline int      setLowestValue(double newval)
    { return this->set_lowestValue(newval); }

    /**
     * Returns the minimal value observed.
     * 
     * @return a floating point number corresponding to the minimal value observed
     * 
     * On failure, throws an exception or returns Y_LOWESTVALUE_INVALID.
     */
           double          get_lowestValue(void);
    inline double          lowestValue(void)
    { return this->get_lowestValue(); }

    /**
     * Changes the recorded maximal value observed.
     * 
     * @param newval : a floating point number corresponding to the recorded maximal value observed
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_highestValue(double newval);
    inline int      setHighestValue(double newval)
    { return this->set_highestValue(newval); }

    /**
     * Returns the maximal value observed.
     * 
     * @return a floating point number corresponding to the maximal value observed
     * 
     * On failure, throws an exception or returns Y_HIGHESTVALUE_INVALID.
     */
           double          get_highestValue(void);
    inline double          highestValue(void)
    { return this->get_highestValue(); }

    /**
     * Returns the uncalibrated, unrounded raw value returned by the sensor.
     * 
     * @return a floating point number corresponding to the uncalibrated, unrounded raw value returned by the sensor
     * 
     * On failure, throws an exception or returns Y_CURRENTRAWVALUE_INVALID.
     */
           double          get_currentRawValue(void);
    inline double          currentRawValue(void)
    { return this->get_currentRawValue(); }

           string          get_calibrationParam(void);
    inline string          calibrationParam(void)
    { return this->get_calibrationParam(); }

    int             set_calibrationParam(const string& newval);
    inline int      setCalibrationParam(const string& newval)
    { return this->set_calibrationParam(newval); }

    /**
     * Configures error correction data points, in particular to compensate for
     * a possible perturbation of the measure caused by an enclosure. It is possible
     * to configure up to five correction points. Correction points must be provided
     * in ascending order, and be in the range of the sensor. The device will automatically
     * perform a linear interpolation of the error correction between specified
     * points. Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     * 
     * For more information on advanced capabilities to refine the calibration of
     * sensors, please contact support@yoctopuce.com.
     * 
     * @param rawValues : array of floating point numbers, corresponding to the raw
     *         values returned by the sensor for the correction points.
     * @param refValues : array of floating point numbers, corresponding to the corrected
     *         values for the correction points.
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             calibrateFromPoints(vector<double> rawValues,vector<double> refValues);

    int             loadCalibrationPoints(vector<double> rawValues,vector<double> refValues);

    /**
     * Returns the resolution of the measured values. The resolution corresponds to the numerical precision
     * of the values, which is not always the same as the actual precision of the sensor.
     * 
     * @return a floating point number corresponding to the resolution of the measured values
     * 
     * On failure, throws an exception or returns Y_RESOLUTION_INVALID.
     */
           double          get_resolution(void);
    inline double          resolution(void)
    { return this->get_resolution(); }


    /**
     * Registers the callback function that is invoked on every change of advertised value.
     * The callback is invoked only during the execution of ySleep or yHandleEvents.
     * This provides control over the time when the callback is triggered. For good responsiveness, remember to call
     * one of these two functions periodically. To unregister a callback, pass a null pointer as argument.
     * 
     * @param callback : the callback function to call, or a null pointer. The callback function should take two
     *         arguments: the function object of which the value has changed, and the character string describing
     *         the new advertised value.
     * @noreturn
     */
    void registerValueCallback(YCarbonDioxideUpdateCallback callback);

    void advertiseValue(const string& value);

    /**
     * Continues the enumeration of CO2 sensors started using yFirstCarbonDioxide().
     * 
     * @return a pointer to a YCarbonDioxide object, corresponding to
     *         a CO2 sensor currently online, or a null pointer
     *         if there are no more CO2 sensors to enumerate.
     */
           YCarbonDioxide  *nextCarbonDioxide(void);
    inline YCarbonDioxide  *next(void)
    { return this->nextCarbonDioxide();}

    /**
     * Retrieves a CO2 sensor for a given identifier.
     * The identifier can be specified using several formats:
     * <ul>
     * <li>FunctionLogicalName</li>
     * <li>ModuleSerialNumber.FunctionIdentifier</li>
     * <li>ModuleSerialNumber.FunctionLogicalName</li>
     * <li>ModuleLogicalName.FunctionIdentifier</li>
     * <li>ModuleLogicalName.FunctionLogicalName</li>
     * </ul>
     * 
     * This function does not require that the CO2 sensor is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method YCarbonDioxide.isOnline() to test if the CO2 sensor is
     * indeed online at a given time. In case of ambiguity when looking for
     * a CO2 sensor by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     * 
     * @param func : a string that uniquely characterizes the CO2 sensor
     * 
     * @return a YCarbonDioxide object allowing you to drive the CO2 sensor.
     */
           static YCarbonDioxide* FindCarbonDioxide(const string& func);
    inline static YCarbonDioxide* Find(const string& func)
    { return YCarbonDioxide::FindCarbonDioxide(func);}
    /**
     * Starts the enumeration of CO2 sensors currently accessible.
     * Use the method YCarbonDioxide.nextCarbonDioxide() to iterate on
     * next CO2 sensors.
     * 
     * @return a pointer to a YCarbonDioxide object, corresponding to
     *         the first CO2 sensor currently online, or a null pointer
     *         if there are none.
     */
           static YCarbonDioxide* FirstCarbonDioxide(void);
    inline static YCarbonDioxide* First(void)
    { return YCarbonDioxide::FirstCarbonDioxide();}
    //--- (end of YCarbonDioxide accessors declaration)
};

//--- (CarbonDioxide functions declaration)

/**
 * Retrieves a CO2 sensor for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 * 
 * This function does not require that the CO2 sensor is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method YCarbonDioxide.isOnline() to test if the CO2 sensor is
 * indeed online at a given time. In case of ambiguity when looking for
 * a CO2 sensor by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 * 
 * @param func : a string that uniquely characterizes the CO2 sensor
 * 
 * @return a YCarbonDioxide object allowing you to drive the CO2 sensor.
 */
inline YCarbonDioxide* yFindCarbonDioxide(const string& func)
{ return YCarbonDioxide::FindCarbonDioxide(func);}
/**
 * Starts the enumeration of CO2 sensors currently accessible.
 * Use the method YCarbonDioxide.nextCarbonDioxide() to iterate on
 * next CO2 sensors.
 * 
 * @return a pointer to a YCarbonDioxide object, corresponding to
 *         the first CO2 sensor currently online, or a null pointer
 *         if there are none.
 */
inline YCarbonDioxide* yFirstCarbonDioxide(void)
{ return YCarbonDioxide::FirstCarbonDioxide();}

//--- (end of CarbonDioxide functions declaration)

#endif
