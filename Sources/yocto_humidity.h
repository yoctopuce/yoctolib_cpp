/*********************************************************************
 *
 * $Id: yocto_humidity.h 14314 2014-01-10 18:12:33Z seb $
 *
 * Declares yFindHumidity(), the high-level API for Humidity functions
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


#ifndef YOCTO_HUMIDITY_H
#define YOCTO_HUMIDITY_H

#include "yocto_api.h"
#include <cfloat>
#include <cmath>
#include <map>

//--- (YHumidity return codes)
//--- (end of YHumidity return codes)
//--- (YHumidity definitions)
class YHumidity; // forward declaration

typedef void (*YHumidityValueCallback)(YHumidity *func, const string& functionValue);
class YMeasure; // forward declaration
typedef void (*YHumidityTimedReportCallback)(YHumidity *func, YMeasure measure);
//--- (end of YHumidity definitions)

//--- (YHumidity declaration)
/**
 * YHumidity Class: Humidity function interface
 * 
 * The Yoctopuce application programming interface allows you to read an instant
 * measure of the sensor, as well as the minimal and maximal values observed.
 */
class YOCTO_CLASS_EXPORT YHumidity: public YSensor {
//--- (end of YHumidity declaration)
protected:
    //--- (YHumidity attributes)
    // Attributes (function value cache)
    YHumidityValueCallback _valueCallbackHumidity;
    YHumidityTimedReportCallback _timedReportCallbackHumidity;

    friend YHumidity *yFindHumidity(const string& func);
    friend YHumidity *yFirstHumidity(void);

    // Constructor is protected, use yFindHumidity factory function to instantiate
    YHumidity(const string& func);
    //--- (end of YHumidity attributes)

public:
    ~YHumidity();
    //--- (YHumidity accessors declaration)


    /**
     * Retrieves a humidity sensor for a given identifier.
     * The identifier can be specified using several formats:
     * <ul>
     * <li>FunctionLogicalName</li>
     * <li>ModuleSerialNumber.FunctionIdentifier</li>
     * <li>ModuleSerialNumber.FunctionLogicalName</li>
     * <li>ModuleLogicalName.FunctionIdentifier</li>
     * <li>ModuleLogicalName.FunctionLogicalName</li>
     * </ul>
     * 
     * This function does not require that the humidity sensor is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method YHumidity.isOnline() to test if the humidity sensor is
     * indeed online at a given time. In case of ambiguity when looking for
     * a humidity sensor by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     * 
     * @param func : a string that uniquely characterizes the humidity sensor
     * 
     * @return a YHumidity object allowing you to drive the humidity sensor.
     */
    static YHumidity*   FindHumidity(string func);

    using YSensor::registerValueCallback;

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
    virtual int         registerValueCallback(YHumidityValueCallback callback);

    virtual int         _invokeValueCallback(string value);

    using YSensor::registerTimedReportCallback;

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
    virtual int         registerTimedReportCallback(YHumidityTimedReportCallback callback);

    virtual int         _invokeTimedReportCallback(YMeasure value);


    inline static YHumidity* Find(string func)
    { return YHumidity::FindHumidity(func); }

    /**
     * Continues the enumeration of humidity sensors started using yFirstHumidity().
     * 
     * @return a pointer to a YHumidity object, corresponding to
     *         a humidity sensor currently online, or a null pointer
     *         if there are no more humidity sensors to enumerate.
     */
           YHumidity       *nextHumidity(void);
    inline YHumidity       *next(void)
    { return this->nextHumidity();}

           static YHumidity* FirstHumidity(void);
    inline static YHumidity* First(void)
    { return YHumidity::FirstHumidity();}
    //--- (end of YHumidity accessors declaration)
};

//--- (Humidity functions declaration)

/**
 * Retrieves a humidity sensor for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 * 
 * This function does not require that the humidity sensor is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method YHumidity.isOnline() to test if the humidity sensor is
 * indeed online at a given time. In case of ambiguity when looking for
 * a humidity sensor by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 * 
 * @param func : a string that uniquely characterizes the humidity sensor
 * 
 * @return a YHumidity object allowing you to drive the humidity sensor.
 */
inline YHumidity* yFindHumidity(const string& func)
{ return YHumidity::FindHumidity(func);}
/**
 * Starts the enumeration of humidity sensors currently accessible.
 * Use the method YHumidity.nextHumidity() to iterate on
 * next humidity sensors.
 * 
 * @return a pointer to a YHumidity object, corresponding to
 *         the first humidity sensor currently online, or a null pointer
 *         if there are none.
 */
inline YHumidity* yFirstHumidity(void)
{ return YHumidity::FirstHumidity();}

//--- (end of Humidity functions declaration)

#endif
