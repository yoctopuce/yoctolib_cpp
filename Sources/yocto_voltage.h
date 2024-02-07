/*********************************************************************
 *
 *  $Id: yocto_voltage.h 44049 2021-02-26 10:57:40Z web $
 *
 *  Declares yFindVoltage(), the high-level API for Voltage functions
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


#ifndef YOCTO_VOLTAGE_H
#define YOCTO_VOLTAGE_H

#include <cfloat>
#include <cmath>

#include "yocto_api.h"

#ifdef YOCTOLIB_NAMESPACE
namespace YOCTOLIB_NAMESPACE
{
#endif

//--- (YVoltage return codes)
//--- (end of YVoltage return codes)
//--- (YVoltage yapiwrapper)
//--- (end of YVoltage yapiwrapper)
//--- (YVoltage definitions)
class YVoltage; // forward declaration

typedef void (*YVoltageValueCallback)(YVoltage *func, const string& functionValue);
class YMeasure; // forward declaration
typedef void (*YVoltageTimedReportCallback)(YVoltage *func, YMeasure measure);
#ifndef _Y_ENABLED_ENUM
#define _Y_ENABLED_ENUM
typedef enum {
    Y_ENABLED_FALSE = 0,
    Y_ENABLED_TRUE = 1,
    Y_ENABLED_INVALID = -1,
} Y_ENABLED_enum;
#endif
//--- (end of YVoltage definitions)

//--- (YVoltage declaration)
/**
 * YVoltage Class: voltage sensor control interface, available for instance in the Yocto-Motor-DC, the
 * Yocto-Volt or the Yocto-Watt
 *
 * The YVoltage class allows you to read and configure Yoctopuce voltage sensors.
 * It inherits from YSensor class the core functions to read measurements,
 * to register callback functions, and to access the autonomous datalogger.
 */
class YOCTO_CLASS_EXPORT YVoltage: public YSensor {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of YVoltage declaration)
protected:
    //--- (YVoltage attributes)
    // Attributes (function value cache)
    Y_ENABLED_enum  _enabled;
    YVoltageValueCallback _valueCallbackVoltage;
    YVoltageTimedReportCallback _timedReportCallbackVoltage;

    friend YVoltage *yFindVoltage(const string& func);
    friend YVoltage *yFirstVoltage(void);

    // Function-specific method for parsing of JSON output and caching result
    virtual int     _parseAttr(YJSONObject *json_val);

    // Constructor is protected, use yFindVoltage factory function to instantiate
    YVoltage(const string& func);
    //--- (end of YVoltage attributes)

public:
    virtual ~YVoltage();
    //--- (YVoltage accessors declaration)

    static const Y_ENABLED_enum ENABLED_FALSE = Y_ENABLED_FALSE;
    static const Y_ENABLED_enum ENABLED_TRUE = Y_ENABLED_TRUE;
    static const Y_ENABLED_enum ENABLED_INVALID = Y_ENABLED_INVALID;

    /**
     * Returns the activation state of this input.
     *
     * @return either YVoltage::ENABLED_FALSE or YVoltage::ENABLED_TRUE, according to the activation state of this input
     *
     * On failure, throws an exception or returns YVoltage::ENABLED_INVALID.
     */
    Y_ENABLED_enum      get_enabled(void);

    inline Y_ENABLED_enum enabled(void)
    { return this->get_enabled(); }

    /**
     * Changes the activation state of this voltage input. When AC measurements are disabled,
     * the device will always assume a DC signal, and vice-versa. When both AC and DC measurements
     * are active, the device switches between AC and DC mode based on the relative amplitude
     * of variations compared to the average value.
     * Remember to call the saveToFlash()
     * method of the module if the modification must be kept.
     *
     * @param newval : either YVoltage::ENABLED_FALSE or YVoltage::ENABLED_TRUE, according to the activation
     * state of this voltage input
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_enabled(Y_ENABLED_enum newval);
    inline int      setEnabled(Y_ENABLED_enum newval)
    { return this->set_enabled(newval); }

    /**
     * Retrieves a voltage sensor for a given identifier.
     * The identifier can be specified using several formats:
     * <ul>
     * <li>FunctionLogicalName</li>
     * <li>ModuleSerialNumber.FunctionIdentifier</li>
     * <li>ModuleSerialNumber.FunctionLogicalName</li>
     * <li>ModuleLogicalName.FunctionIdentifier</li>
     * <li>ModuleLogicalName.FunctionLogicalName</li>
     * </ul>
     *
     * This function does not require that the voltage sensor is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method isOnline() to test if the voltage sensor is
     * indeed online at a given time. In case of ambiguity when looking for
     * a voltage sensor by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     *
     * If a call to this object's is_online() method returns FALSE although
     * you are certain that the matching device is plugged, make sure that you did
     * call registerHub() at application initialization time.
     *
     * @param func : a string that uniquely characterizes the voltage sensor, for instance
     *         MOTORCTL.voltage.
     *
     * @return a YVoltage object allowing you to drive the voltage sensor.
     */
    static YVoltage*    FindVoltage(string func);

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
    virtual int         registerValueCallback(YVoltageValueCallback callback);
    using YSensor::registerValueCallback;

    virtual int         _invokeValueCallback(string value);

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
    virtual int         registerTimedReportCallback(YVoltageTimedReportCallback callback);
    using YSensor::registerTimedReportCallback;

    virtual int         _invokeTimedReportCallback(YMeasure value);


    inline static YVoltage *Find(string func)
    { return YVoltage::FindVoltage(func); }

    /**
     * Continues the enumeration of voltage sensors started using yFirstVoltage().
     * Caution: You can't make any assumption about the returned voltage sensors order.
     * If you want to find a specific a voltage sensor, use Voltage.findVoltage()
     * and a hardwareID or a logical name.
     *
     * @return a pointer to a YVoltage object, corresponding to
     *         a voltage sensor currently online, or a NULL pointer
     *         if there are no more voltage sensors to enumerate.
     */
           YVoltage        *nextVoltage(void);
    inline YVoltage        *next(void)
    { return this->nextVoltage();}

    /**
     * Starts the enumeration of voltage sensors currently accessible.
     * Use the method YVoltage::nextVoltage() to iterate on
     * next voltage sensors.
     *
     * @return a pointer to a YVoltage object, corresponding to
     *         the first voltage sensor currently online, or a NULL pointer
     *         if there are none.
     */
           static YVoltage *FirstVoltage(void);
    inline static YVoltage *First(void)
    { return YVoltage::FirstVoltage();}
#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of YVoltage accessors declaration)
};

//--- (YVoltage functions declaration)

/**
 * Retrieves a voltage sensor for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that the voltage sensor is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the voltage sensor is
 * indeed online at a given time. In case of ambiguity when looking for
 * a voltage sensor by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the voltage sensor, for instance
 *         MOTORCTL.voltage.
 *
 * @return a YVoltage object allowing you to drive the voltage sensor.
 */
inline YVoltage *yFindVoltage(const string& func)
{ return YVoltage::FindVoltage(func);}
/**
 * Starts the enumeration of voltage sensors currently accessible.
 * Use the method YVoltage::nextVoltage() to iterate on
 * next voltage sensors.
 *
 * @return a pointer to a YVoltage object, corresponding to
 *         the first voltage sensor currently online, or a NULL pointer
 *         if there are none.
 */
inline YVoltage *yFirstVoltage(void)
{ return YVoltage::FirstVoltage();}

//--- (end of YVoltage functions declaration)

#ifdef YOCTOLIB_NAMESPACE
// end of namespace definition
}
#endif

#endif
