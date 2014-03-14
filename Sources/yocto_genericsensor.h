/*********************************************************************
 *
 * $Id: yocto_genericsensor.h 15253 2014-03-06 10:15:50Z seb $
 *
 * Declares yFindGenericSensor(), the high-level API for GenericSensor functions
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


#ifndef YOCTO_GENERICSENSOR_H
#define YOCTO_GENERICSENSOR_H

#include "yocto_api.h"
#include <cfloat>
#include <cmath>
#include <map>

//--- (YGenericSensor return codes)
//--- (end of YGenericSensor return codes)
//--- (YGenericSensor definitions)
class YGenericSensor; // forward declaration

typedef void (*YGenericSensorValueCallback)(YGenericSensor *func, const string& functionValue);
class YMeasure; // forward declaration
typedef void (*YGenericSensorTimedReportCallback)(YGenericSensor *func, YMeasure measure);
#define Y_SIGNALVALUE_INVALID           (YAPI_INVALID_DOUBLE)
#define Y_SIGNALUNIT_INVALID            (YAPI_INVALID_STRING)
#define Y_SIGNALRANGE_INVALID           (YAPI_INVALID_STRING)
#define Y_VALUERANGE_INVALID            (YAPI_INVALID_STRING)
//--- (end of YGenericSensor definitions)

//--- (YGenericSensor declaration)
/**
 * YGenericSensor Class: GenericSensor function interface
 * 
 * The Yoctopuce application programming interface allows you to read an instant
 * measure of the sensor, as well as the minimal and maximal values observed.
 */
class YOCTO_CLASS_EXPORT YGenericSensor: public YSensor {
//--- (end of YGenericSensor declaration)
protected:
    //--- (YGenericSensor attributes)
    // Attributes (function value cache)
    double          _signalValue;
    string          _signalUnit;
    string          _signalRange;
    string          _valueRange;
    YGenericSensorValueCallback _valueCallbackGenericSensor;
    YGenericSensorTimedReportCallback _timedReportCallbackGenericSensor;

    friend YGenericSensor *yFindGenericSensor(const string& func);
    friend YGenericSensor *yFirstGenericSensor(void);

    // Function-specific method for parsing of JSON output and caching result
    virtual int     _parseAttr(yJsonStateMachine& j);

    // Constructor is protected, use yFindGenericSensor factory function to instantiate
    YGenericSensor(const string& func);
    //--- (end of YGenericSensor attributes)

public:
    ~YGenericSensor();
    //--- (YGenericSensor accessors declaration)

    static const double SIGNALVALUE_INVALID;
    static const string SIGNALUNIT_INVALID;
    static const string SIGNALRANGE_INVALID;
    static const string VALUERANGE_INVALID;

    /**
     * Changes the measuring unit for the measured value.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     * 
     * @param newval : a string corresponding to the measuring unit for the measured value
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_unit(const string& newval);
    inline int      setUnit(const string& newval)
    { return this->set_unit(newval); }

    /**
     * Returns the measured value of the electrical signal used by the sensor.
     * 
     * @return a floating point number corresponding to the measured value of the electrical signal used by the sensor
     * 
     * On failure, throws an exception or returns Y_SIGNALVALUE_INVALID.
     */
    double              get_signalValue(void);

    inline double       signalValue(void)
    { return this->get_signalValue(); }

    /**
     * Returns the measuring unit of the electrical signal used by the sensor.
     * 
     * @return a string corresponding to the measuring unit of the electrical signal used by the sensor
     * 
     * On failure, throws an exception or returns Y_SIGNALUNIT_INVALID.
     */
    string              get_signalUnit(void);

    inline string       signalUnit(void)
    { return this->get_signalUnit(); }

    /**
     * Returns the electric signal range used by the sensor.
     * 
     * @return a string corresponding to the electric signal range used by the sensor
     * 
     * On failure, throws an exception or returns Y_SIGNALRANGE_INVALID.
     */
    string              get_signalRange(void);

    inline string       signalRange(void)
    { return this->get_signalRange(); }

    /**
     * Changes the electric signal range used by the sensor.
     * 
     * @param newval : a string corresponding to the electric signal range used by the sensor
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_signalRange(const string& newval);
    inline int      setSignalRange(const string& newval)
    { return this->set_signalRange(newval); }

    /**
     * Returns the physical value range measured by the sensor.
     * 
     * @return a string corresponding to the physical value range measured by the sensor
     * 
     * On failure, throws an exception or returns Y_VALUERANGE_INVALID.
     */
    string              get_valueRange(void);

    inline string       valueRange(void)
    { return this->get_valueRange(); }

    /**
     * Changes the physical value range measured by the sensor. The range change may have a side effect
     * on the display resolution, as it may be adapted automatically.
     * 
     * @param newval : a string corresponding to the physical value range measured by the sensor
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_valueRange(const string& newval);
    inline int      setValueRange(const string& newval)
    { return this->set_valueRange(newval); }

    /**
     * Retrieves a generic sensor for a given identifier.
     * The identifier can be specified using several formats:
     * <ul>
     * <li>FunctionLogicalName</li>
     * <li>ModuleSerialNumber.FunctionIdentifier</li>
     * <li>ModuleSerialNumber.FunctionLogicalName</li>
     * <li>ModuleLogicalName.FunctionIdentifier</li>
     * <li>ModuleLogicalName.FunctionLogicalName</li>
     * </ul>
     * 
     * This function does not require that the generic sensor is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method YGenericSensor.isOnline() to test if the generic sensor is
     * indeed online at a given time. In case of ambiguity when looking for
     * a generic sensor by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     * 
     * @param func : a string that uniquely characterizes the generic sensor
     * 
     * @return a YGenericSensor object allowing you to drive the generic sensor.
     */
    static YGenericSensor* FindGenericSensor(string func);

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
    virtual int         registerValueCallback(YGenericSensorValueCallback callback);
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
    virtual int         registerTimedReportCallback(YGenericSensorTimedReportCallback callback);
    using YSensor::registerTimedReportCallback;

    virtual int         _invokeTimedReportCallback(YMeasure value);


    inline static YGenericSensor* Find(string func)
    { return YGenericSensor::FindGenericSensor(func); }

    /**
     * Continues the enumeration of generic sensors started using yFirstGenericSensor().
     * 
     * @return a pointer to a YGenericSensor object, corresponding to
     *         a generic sensor currently online, or a null pointer
     *         if there are no more generic sensors to enumerate.
     */
           YGenericSensor  *nextGenericSensor(void);
    inline YGenericSensor  *next(void)
    { return this->nextGenericSensor();}

    /**
     * Starts the enumeration of generic sensors currently accessible.
     * Use the method YGenericSensor.nextGenericSensor() to iterate on
     * next generic sensors.
     * 
     * @return a pointer to a YGenericSensor object, corresponding to
     *         the first generic sensor currently online, or a null pointer
     *         if there are none.
     */
           static YGenericSensor* FirstGenericSensor(void);
    inline static YGenericSensor* First(void)
    { return YGenericSensor::FirstGenericSensor();}
    //--- (end of YGenericSensor accessors declaration)
};

//--- (GenericSensor functions declaration)

/**
 * Retrieves a generic sensor for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 * 
 * This function does not require that the generic sensor is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method YGenericSensor.isOnline() to test if the generic sensor is
 * indeed online at a given time. In case of ambiguity when looking for
 * a generic sensor by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 * 
 * @param func : a string that uniquely characterizes the generic sensor
 * 
 * @return a YGenericSensor object allowing you to drive the generic sensor.
 */
inline YGenericSensor* yFindGenericSensor(const string& func)
{ return YGenericSensor::FindGenericSensor(func);}
/**
 * Starts the enumeration of generic sensors currently accessible.
 * Use the method YGenericSensor.nextGenericSensor() to iterate on
 * next generic sensors.
 * 
 * @return a pointer to a YGenericSensor object, corresponding to
 *         the first generic sensor currently online, or a null pointer
 *         if there are none.
 */
inline YGenericSensor* yFirstGenericSensor(void)
{ return YGenericSensor::FirstGenericSensor();}

//--- (end of GenericSensor functions declaration)

#endif
