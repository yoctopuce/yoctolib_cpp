/*********************************************************************
 *
 *  $Id: svn_id $
 *
 *  Declares yFindVirtualSensor(), the high-level API for VirtualSensor functions
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


#ifndef YOCTO_VIRTUALSENSOR_H
#define YOCTO_VIRTUALSENSOR_H

#include <cfloat>
#include <cmath>

#include "yocto_api.h"

#ifdef YOCTOLIB_NAMESPACE
namespace YOCTOLIB_NAMESPACE
{
#endif

//--- (YVirtualSensor return codes)
//--- (end of YVirtualSensor return codes)
//--- (YVirtualSensor yapiwrapper)
//--- (end of YVirtualSensor yapiwrapper)
//--- (YVirtualSensor definitions)
class YVirtualSensor; // forward declaration

typedef void (*YVirtualSensorValueCallback)(YVirtualSensor *func, const string& functionValue);
class YMeasure; // forward declaration
typedef void (*YVirtualSensorTimedReportCallback)(YVirtualSensor *func, YMeasure measure);
#define Y_INVALIDVALUE_INVALID          (YAPI_INVALID_DOUBLE)
//--- (end of YVirtualSensor definitions)

//--- (YVirtualSensor declaration)
/**
 * YVirtualSensor Class: virtual sensor control interface
 *
 * The YVirtualSensor class allows you to use Yoctopuce virtual sensors.
 * These sensors make it possible to show external data collected by the user
 * as a Yoctopuce Sensor. This class inherits from YSensor class the core
 * functions to read measurements, to register callback functions, and to access
 * the autonomous datalogger. It adds the ability to change the sensor value as
 * needed, or to mark current value as invalid.
 */
class YOCTO_CLASS_EXPORT YVirtualSensor: public YSensor {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of YVirtualSensor declaration)
protected:
    //--- (YVirtualSensor attributes)
    // Attributes (function value cache)
    double          _invalidValue;
    YVirtualSensorValueCallback _valueCallbackVirtualSensor;
    YVirtualSensorTimedReportCallback _timedReportCallbackVirtualSensor;

    friend YVirtualSensor *yFindVirtualSensor(const string& func);
    friend YVirtualSensor *yFirstVirtualSensor(void);

    // Function-specific method for parsing of JSON output and caching result
    virtual int     _parseAttr(YJSONObject *json_val);

    // Constructor is protected, use yFindVirtualSensor factory function to instantiate
    YVirtualSensor(const string& func);
    //--- (end of YVirtualSensor attributes)

public:
    virtual ~YVirtualSensor();
    //--- (YVirtualSensor accessors declaration)

    static const double INVALIDVALUE_INVALID;

    /**
     * Changes the measuring unit for the measured value.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     *
     * @param newval : a string corresponding to the measuring unit for the measured value
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_unit(const string& newval);
    inline int      setUnit(const string& newval)
    { return this->set_unit(newval); }

    /**
     * Changes the current value of the sensor (raw value, before calibration).
     *
     * @param newval : a floating point number corresponding to the current value of the sensor (raw
     * value, before calibration)
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_currentRawValue(double newval);
    inline int      setCurrentRawValue(double newval)
    { return this->set_currentRawValue(newval); }

    int             set_sensorState(int newval);
    inline int      setSensorState(int newval)
    { return this->set_sensorState(newval); }

    /**
     * Changes the invalid value of the sensor, returned if the sensor is read when in invalid state
     * (for instance before having been set). Remember to call the saveToFlash()
     * method of the module if the modification must be kept.
     *
     * @param newval : a floating point number corresponding to the invalid value of the sensor, returned
     * if the sensor is read when in invalid state
     *         (for instance before having been set)
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_invalidValue(double newval);
    inline int      setInvalidValue(double newval)
    { return this->set_invalidValue(newval); }

    /**
     * Returns the invalid value of the sensor, returned if the sensor is read when in invalid state
     * (for instance before having been set).
     *
     * @return a floating point number corresponding to the invalid value of the sensor, returned if the
     * sensor is read when in invalid state
     *         (for instance before having been set)
     *
     * On failure, throws an exception or returns YVirtualSensor::INVALIDVALUE_INVALID.
     */
    double              get_invalidValue(void);

    inline double       invalidValue(void)
    { return this->get_invalidValue(); }

    /**
     * Retrieves a virtual sensor for a given identifier.
     * The identifier can be specified using several formats:
     *
     * - FunctionLogicalName
     * - ModuleSerialNumber.FunctionIdentifier
     * - ModuleSerialNumber.FunctionLogicalName
     * - ModuleLogicalName.FunctionIdentifier
     * - ModuleLogicalName.FunctionLogicalName
     *
     *
     * This function does not require that the virtual sensor is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method isOnline() to test if the virtual sensor is
     * indeed online at a given time. In case of ambiguity when looking for
     * a virtual sensor by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     *
     * If a call to this object's is_online() method returns FALSE although
     * you are certain that the matching device is plugged, make sure that you did
     * call registerHub() at application initialization time.
     *
     * @param func : a string that uniquely characterizes the virtual sensor, for instance
     *         MyDevice.virtualSensor1.
     *
     * @return a YVirtualSensor object allowing you to drive the virtual sensor.
     */
    static YVirtualSensor* FindVirtualSensor(string func);

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
    virtual int         registerValueCallback(YVirtualSensorValueCallback callback);
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
    virtual int         registerTimedReportCallback(YVirtualSensorTimedReportCallback callback);
    using YSensor::registerTimedReportCallback;

    virtual int         _invokeTimedReportCallback(YMeasure value);

    /**
     * Changes the current sensor state to invalid (as if no value would have been ever set).
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         set_sensorAsInvalid(void);


    inline static YVirtualSensor *Find(string func)
    { return YVirtualSensor::FindVirtualSensor(func); }

    /**
     * Continues the enumeration of virtual sensors started using yFirstVirtualSensor().
     * Caution: You can't make any assumption about the returned virtual sensors order.
     * If you want to find a specific a virtual sensor, use VirtualSensor.findVirtualSensor()
     * and a hardwareID or a logical name.
     *
     * @return a pointer to a YVirtualSensor object, corresponding to
     *         a virtual sensor currently online, or a NULL pointer
     *         if there are no more virtual sensors to enumerate.
     */
           YVirtualSensor  *nextVirtualSensor(void);
    inline YVirtualSensor  *next(void)
    { return this->nextVirtualSensor();}

    /**
     * Starts the enumeration of virtual sensors currently accessible.
     * Use the method YVirtualSensor::nextVirtualSensor() to iterate on
     * next virtual sensors.
     *
     * @return a pointer to a YVirtualSensor object, corresponding to
     *         the first virtual sensor currently online, or a NULL pointer
     *         if there are none.
     */
           static YVirtualSensor *FirstVirtualSensor(void);
    inline static YVirtualSensor *First(void)
    { return YVirtualSensor::FirstVirtualSensor();}
#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of YVirtualSensor accessors declaration)
};

//--- (YVirtualSensor functions declaration)

/**
 * Retrieves a virtual sensor for a given identifier.
 * The identifier can be specified using several formats:
 *
 * - FunctionLogicalName
 * - ModuleSerialNumber.FunctionIdentifier
 * - ModuleSerialNumber.FunctionLogicalName
 * - ModuleLogicalName.FunctionIdentifier
 * - ModuleLogicalName.FunctionLogicalName
 *
 *
 * This function does not require that the virtual sensor is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the virtual sensor is
 * indeed online at a given time. In case of ambiguity when looking for
 * a virtual sensor by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the virtual sensor, for instance
 *         MyDevice.virtualSensor1.
 *
 * @return a YVirtualSensor object allowing you to drive the virtual sensor.
 */
inline YVirtualSensor *yFindVirtualSensor(const string& func)
{ return YVirtualSensor::FindVirtualSensor(func);}
/**
 * Starts the enumeration of virtual sensors currently accessible.
 * Use the method YVirtualSensor::nextVirtualSensor() to iterate on
 * next virtual sensors.
 *
 * @return a pointer to a YVirtualSensor object, corresponding to
 *         the first virtual sensor currently online, or a NULL pointer
 *         if there are none.
 */
inline YVirtualSensor *yFirstVirtualSensor(void)
{ return YVirtualSensor::FirstVirtualSensor();}

//--- (end of YVirtualSensor functions declaration)

#ifdef YOCTOLIB_NAMESPACE
// end of namespace definition
}
#endif

#endif
