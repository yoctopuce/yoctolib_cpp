/*********************************************************************
 *
 *  $Id: yocto_power.h 53431 2023-03-06 14:19:35Z seb $
 *
 *  Declares yFindPower(), the high-level API for Power functions
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


#ifndef YOCTO_POWER_H
#define YOCTO_POWER_H

#include <cfloat>
#include <cmath>

#include "yocto_api.h"

#ifdef YOCTOLIB_NAMESPACE
namespace YOCTOLIB_NAMESPACE
{
#endif

//--- (YPower return codes)
//--- (end of YPower return codes)
//--- (YPower yapiwrapper)
//--- (end of YPower yapiwrapper)
//--- (YPower definitions)
class YPower; // forward declaration

typedef void (*YPowerValueCallback)(YPower *func, const string& functionValue);
class YMeasure; // forward declaration
typedef void (*YPowerTimedReportCallback)(YPower *func, YMeasure measure);
#define Y_POWERFACTOR_INVALID           (YAPI_INVALID_DOUBLE)
#define Y_COSPHI_INVALID                (YAPI_INVALID_DOUBLE)
#define Y_METER_INVALID                 (YAPI_INVALID_DOUBLE)
#define Y_DELIVEREDENERGYMETER_INVALID  (YAPI_INVALID_DOUBLE)
#define Y_RECEIVEDENERGYMETER_INVALID   (YAPI_INVALID_DOUBLE)
#define Y_METERTIMER_INVALID            (YAPI_INVALID_UINT)
//--- (end of YPower definitions)

//--- (YPower declaration)
/**
 * YPower Class: electrical power sensor control interface, available for instance in the Yocto-Watt
 *
 * The YPower class allows you to read and configure Yoctopuce electrical power sensors.
 * It inherits from YSensor class the core functions to read measurements,
 * to register callback functions, and to access the autonomous datalogger.
 * This class adds the ability to access the energy counter and the power factor.
 */
class YOCTO_CLASS_EXPORT YPower: public YSensor {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of YPower declaration)
protected:
    //--- (YPower attributes)
    // Attributes (function value cache)
    double          _powerFactor;
    double          _cosPhi;
    double          _meter;
    double          _deliveredEnergyMeter;
    double          _receivedEnergyMeter;
    int             _meterTimer;
    YPowerValueCallback _valueCallbackPower;
    YPowerTimedReportCallback _timedReportCallbackPower;

    friend YPower *yFindPower(const string& func);
    friend YPower *yFirstPower(void);

    // Function-specific method for parsing of JSON output and caching result
    virtual int     _parseAttr(YJSONObject *json_val);

    // Constructor is protected, use yFindPower factory function to instantiate
    YPower(const string& func);
    //--- (end of YPower attributes)

public:
    virtual ~YPower();
    //--- (YPower accessors declaration)

    static const double POWERFACTOR_INVALID;
    static const double COSPHI_INVALID;
    static const double METER_INVALID;
    static const double DELIVEREDENERGYMETER_INVALID;
    static const double RECEIVEDENERGYMETER_INVALID;
    static const int METERTIMER_INVALID = YAPI_INVALID_UINT;

    /**
     * Returns the power factor (PF), i.e. ratio between the active power consumed (in W)
     * and the apparent power provided (VA).
     *
     * @return a floating point number corresponding to the power factor (PF), i.e
     *
     * On failure, throws an exception or returns YPower::POWERFACTOR_INVALID.
     */
    double              get_powerFactor(void);

    inline double       powerFactor(void)
    { return this->get_powerFactor(); }

    /**
     * Returns the Displacement Power factor (DPF), i.e. cosine of the phase shift between
     * the voltage and current fundamentals.
     * On the Yocto-Watt (V1), the value returned by this method correponds to the
     * power factor as this device is cannot estimate the true DPF.
     *
     * @return a floating point number corresponding to the Displacement Power factor (DPF), i.e
     *
     * On failure, throws an exception or returns YPower::COSPHI_INVALID.
     */
    double              get_cosPhi(void);

    inline double       cosPhi(void)
    { return this->get_cosPhi(); }

    int             set_meter(double newval);
    inline int      setMeter(double newval)
    { return this->set_meter(newval); }

    /**
     * Returns the energy counter, maintained by the wattmeter by integrating the
     * power consumption over time. This is the sum of forward and backwad energy transfers,
     * if you are insterested in only one direction, use  get_receivedEnergyMeter() or
     * get_deliveredEnergyMeter(). Note that this counter is reset at each start of the device.
     *
     * @return a floating point number corresponding to the energy counter, maintained by the wattmeter by
     * integrating the
     *         power consumption over time
     *
     * On failure, throws an exception or returns YPower::METER_INVALID.
     */
    double              get_meter(void);

    inline double       meter(void)
    { return this->get_meter(); }

    /**
     * Returns the energy counter, maintained by the wattmeter by integrating the power consumption over time,
     * but only when positive. Note that this counter is reset at each start of the device.
     *
     * @return a floating point number corresponding to the energy counter, maintained by the wattmeter by
     * integrating the power consumption over time,
     *         but only when positive
     *
     * On failure, throws an exception or returns YPower::DELIVEREDENERGYMETER_INVALID.
     */
    double              get_deliveredEnergyMeter(void);

    inline double       deliveredEnergyMeter(void)
    { return this->get_deliveredEnergyMeter(); }

    /**
     * Returns the energy counter, maintained by the wattmeter by integrating the power consumption over time,
     * but only when negative. Note that this counter is reset at each start of the device.
     *
     * @return a floating point number corresponding to the energy counter, maintained by the wattmeter by
     * integrating the power consumption over time,
     *         but only when negative
     *
     * On failure, throws an exception or returns YPower::RECEIVEDENERGYMETER_INVALID.
     */
    double              get_receivedEnergyMeter(void);

    inline double       receivedEnergyMeter(void)
    { return this->get_receivedEnergyMeter(); }

    /**
     * Returns the elapsed time since last energy counter reset, in seconds.
     *
     * @return an integer corresponding to the elapsed time since last energy counter reset, in seconds
     *
     * On failure, throws an exception or returns YPower::METERTIMER_INVALID.
     */
    int                 get_meterTimer(void);

    inline int          meterTimer(void)
    { return this->get_meterTimer(); }

    /**
     * Retrieves a electrical power sensor for a given identifier.
     * The identifier can be specified using several formats:
     * <ul>
     * <li>FunctionLogicalName</li>
     * <li>ModuleSerialNumber.FunctionIdentifier</li>
     * <li>ModuleSerialNumber.FunctionLogicalName</li>
     * <li>ModuleLogicalName.FunctionIdentifier</li>
     * <li>ModuleLogicalName.FunctionLogicalName</li>
     * </ul>
     *
     * This function does not require that the electrical power sensor is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method isOnline() to test if the electrical power sensor is
     * indeed online at a given time. In case of ambiguity when looking for
     * a electrical power sensor by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     *
     * If a call to this object's is_online() method returns FALSE although
     * you are certain that the matching device is plugged, make sure that you did
     * call registerHub() at application initialization time.
     *
     * @param func : a string that uniquely characterizes the electrical power sensor, for instance
     *         YWATTMK1.power.
     *
     * @return a YPower object allowing you to drive the electrical power sensor.
     */
    static YPower*      FindPower(string func);

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
    virtual int         registerValueCallback(YPowerValueCallback callback);
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
    virtual int         registerTimedReportCallback(YPowerTimedReportCallback callback);
    using YSensor::registerTimedReportCallback;

    virtual int         _invokeTimedReportCallback(YMeasure value);

    /**
     * Resets the energy counters.
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         reset(void);


    inline static YPower *Find(string func)
    { return YPower::FindPower(func); }

    /**
     * Continues the enumeration of electrical power sensors started using yFirstPower().
     * Caution: You can't make any assumption about the returned electrical power sensors order.
     * If you want to find a specific a electrical power sensor, use Power.findPower()
     * and a hardwareID or a logical name.
     *
     * @return a pointer to a YPower object, corresponding to
     *         a electrical power sensor currently online, or a NULL pointer
     *         if there are no more electrical power sensors to enumerate.
     */
           YPower          *nextPower(void);
    inline YPower          *next(void)
    { return this->nextPower();}

    /**
     * Starts the enumeration of electrical power sensors currently accessible.
     * Use the method YPower::nextPower() to iterate on
     * next electrical power sensors.
     *
     * @return a pointer to a YPower object, corresponding to
     *         the first electrical power sensor currently online, or a NULL pointer
     *         if there are none.
     */
           static YPower *FirstPower(void);
    inline static YPower *First(void)
    { return YPower::FirstPower();}
#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of YPower accessors declaration)
};

//--- (YPower functions declaration)

/**
 * Retrieves a electrical power sensor for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that the electrical power sensor is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the electrical power sensor is
 * indeed online at a given time. In case of ambiguity when looking for
 * a electrical power sensor by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the electrical power sensor, for instance
 *         YWATTMK1.power.
 *
 * @return a YPower object allowing you to drive the electrical power sensor.
 */
inline YPower *yFindPower(const string& func)
{ return YPower::FindPower(func);}
/**
 * Starts the enumeration of electrical power sensors currently accessible.
 * Use the method YPower::nextPower() to iterate on
 * next electrical power sensors.
 *
 * @return a pointer to a YPower object, corresponding to
 *         the first electrical power sensor currently online, or a NULL pointer
 *         if there are none.
 */
inline YPower *yFirstPower(void)
{ return YPower::FirstPower();}

//--- (end of YPower functions declaration)

#ifdef YOCTOLIB_NAMESPACE
// end of namespace definition
}
#endif

#endif
