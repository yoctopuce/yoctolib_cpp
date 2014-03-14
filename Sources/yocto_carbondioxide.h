/*********************************************************************
 *
 * $Id: yocto_carbondioxide.h 15253 2014-03-06 10:15:50Z seb $
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

//--- (YCarbonDioxide return codes)
//--- (end of YCarbonDioxide return codes)
//--- (YCarbonDioxide definitions)
class YCarbonDioxide; // forward declaration

typedef void (*YCarbonDioxideValueCallback)(YCarbonDioxide *func, const string& functionValue);
class YMeasure; // forward declaration
typedef void (*YCarbonDioxideTimedReportCallback)(YCarbonDioxide *func, YMeasure measure);
//--- (end of YCarbonDioxide definitions)

//--- (YCarbonDioxide declaration)
/**
 * YCarbonDioxide Class: CarbonDioxide function interface
 * 
 * The Yoctopuce application programming interface allows you to read an instant
 * measure of the sensor, as well as the minimal and maximal values observed.
 */
class YOCTO_CLASS_EXPORT YCarbonDioxide: public YSensor {
//--- (end of YCarbonDioxide declaration)
protected:
    //--- (YCarbonDioxide attributes)
    // Attributes (function value cache)
    YCarbonDioxideValueCallback _valueCallbackCarbonDioxide;
    YCarbonDioxideTimedReportCallback _timedReportCallbackCarbonDioxide;

    friend YCarbonDioxide *yFindCarbonDioxide(const string& func);
    friend YCarbonDioxide *yFirstCarbonDioxide(void);

    // Constructor is protected, use yFindCarbonDioxide factory function to instantiate
    YCarbonDioxide(const string& func);
    //--- (end of YCarbonDioxide attributes)

public:
    ~YCarbonDioxide();
    //--- (YCarbonDioxide accessors declaration)


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
    static YCarbonDioxide* FindCarbonDioxide(string func);

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
    virtual int         registerValueCallback(YCarbonDioxideValueCallback callback);
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
    virtual int         registerTimedReportCallback(YCarbonDioxideTimedReportCallback callback);
    using YSensor::registerTimedReportCallback;

    virtual int         _invokeTimedReportCallback(YMeasure value);


    inline static YCarbonDioxide* Find(string func)
    { return YCarbonDioxide::FindCarbonDioxide(func); }

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
