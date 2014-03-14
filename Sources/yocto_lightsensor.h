/*********************************************************************
 *
 * $Id: yocto_lightsensor.h 15253 2014-03-06 10:15:50Z seb $
 *
 * Declares yFindLightSensor(), the high-level API for LightSensor functions
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


#ifndef YOCTO_LIGHTSENSOR_H
#define YOCTO_LIGHTSENSOR_H

#include "yocto_api.h"
#include <cfloat>
#include <cmath>
#include <map>

//--- (YLightSensor return codes)
//--- (end of YLightSensor return codes)
//--- (YLightSensor definitions)
class YLightSensor; // forward declaration

typedef void (*YLightSensorValueCallback)(YLightSensor *func, const string& functionValue);
class YMeasure; // forward declaration
typedef void (*YLightSensorTimedReportCallback)(YLightSensor *func, YMeasure measure);
//--- (end of YLightSensor definitions)

//--- (YLightSensor declaration)
/**
 * YLightSensor Class: LightSensor function interface
 * 
 * The Yoctopuce application programming interface allows you to read an instant
 * measure of the sensor, as well as the minimal and maximal values observed.
 */
class YOCTO_CLASS_EXPORT YLightSensor: public YSensor {
//--- (end of YLightSensor declaration)
protected:
    //--- (YLightSensor attributes)
    // Attributes (function value cache)
    YLightSensorValueCallback _valueCallbackLightSensor;
    YLightSensorTimedReportCallback _timedReportCallbackLightSensor;

    friend YLightSensor *yFindLightSensor(const string& func);
    friend YLightSensor *yFirstLightSensor(void);

    // Constructor is protected, use yFindLightSensor factory function to instantiate
    YLightSensor(const string& func);
    //--- (end of YLightSensor attributes)

public:
    ~YLightSensor();
    //--- (YLightSensor accessors declaration)


    int             set_currentValue(double newval);
    inline int      setCurrentValue(double newval)
    { return this->set_currentValue(newval); }

    /**
     * Changes the sensor-specific calibration parameter so that the current value
     * matches a desired target (linear scaling).
     * 
     * @param calibratedVal : the desired target value.
     * 
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             calibrate(double calibratedVal);

    /**
     * Retrieves a light sensor for a given identifier.
     * The identifier can be specified using several formats:
     * <ul>
     * <li>FunctionLogicalName</li>
     * <li>ModuleSerialNumber.FunctionIdentifier</li>
     * <li>ModuleSerialNumber.FunctionLogicalName</li>
     * <li>ModuleLogicalName.FunctionIdentifier</li>
     * <li>ModuleLogicalName.FunctionLogicalName</li>
     * </ul>
     * 
     * This function does not require that the light sensor is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method YLightSensor.isOnline() to test if the light sensor is
     * indeed online at a given time. In case of ambiguity when looking for
     * a light sensor by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     * 
     * @param func : a string that uniquely characterizes the light sensor
     * 
     * @return a YLightSensor object allowing you to drive the light sensor.
     */
    static YLightSensor* FindLightSensor(string func);

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
    virtual int         registerValueCallback(YLightSensorValueCallback callback);
    using YSensor::registerValueCallback;

    virtual int         _invokeValueCallback(string value);

    /**
     * Registers the callback function that is invoked on every periodic timed notification.
     * The callback is invoked only during the execution of ySleep or yHandleEvents.
     * This provides control over the time when the callback is triggered. For good responsiveness, remember to call
     * one of these two functions periodically. To unregister a callback, pass a null pointer as argument.
     * 
     * @param callback : the callback function to call, or a null pointer. The callback function should take two
     *         arguments: the function object of which the value has changed, and an YMeasure object describing
     *         the new advertised value.
     * @noreturn
     */
    virtual int         registerTimedReportCallback(YLightSensorTimedReportCallback callback);
    using YSensor::registerTimedReportCallback;

    virtual int         _invokeTimedReportCallback(YMeasure value);


    inline static YLightSensor* Find(string func)
    { return YLightSensor::FindLightSensor(func); }

    /**
     * Continues the enumeration of light sensors started using yFirstLightSensor().
     * 
     * @return a pointer to a YLightSensor object, corresponding to
     *         a light sensor currently online, or a null pointer
     *         if there are no more light sensors to enumerate.
     */
           YLightSensor    *nextLightSensor(void);
    inline YLightSensor    *next(void)
    { return this->nextLightSensor();}

    /**
     * Starts the enumeration of light sensors currently accessible.
     * Use the method YLightSensor.nextLightSensor() to iterate on
     * next light sensors.
     * 
     * @return a pointer to a YLightSensor object, corresponding to
     *         the first light sensor currently online, or a null pointer
     *         if there are none.
     */
           static YLightSensor* FirstLightSensor(void);
    inline static YLightSensor* First(void)
    { return YLightSensor::FirstLightSensor();}
    //--- (end of YLightSensor accessors declaration)
};

//--- (LightSensor functions declaration)

/**
 * Retrieves a light sensor for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 * 
 * This function does not require that the light sensor is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method YLightSensor.isOnline() to test if the light sensor is
 * indeed online at a given time. In case of ambiguity when looking for
 * a light sensor by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 * 
 * @param func : a string that uniquely characterizes the light sensor
 * 
 * @return a YLightSensor object allowing you to drive the light sensor.
 */
inline YLightSensor* yFindLightSensor(const string& func)
{ return YLightSensor::FindLightSensor(func);}
/**
 * Starts the enumeration of light sensors currently accessible.
 * Use the method YLightSensor.nextLightSensor() to iterate on
 * next light sensors.
 * 
 * @return a pointer to a YLightSensor object, corresponding to
 *         the first light sensor currently online, or a null pointer
 *         if there are none.
 */
inline YLightSensor* yFirstLightSensor(void)
{ return YLightSensor::FirstLightSensor();}

//--- (end of LightSensor functions declaration)

#endif
