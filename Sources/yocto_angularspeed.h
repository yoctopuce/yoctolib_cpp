/*********************************************************************
 *
 *  $Id: svn_id $
 *
 *  Declares yFindAngularSpeed(), the high-level API for AngularSpeed functions
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


#ifndef YOCTO_ANGULARSPEED_H
#define YOCTO_ANGULARSPEED_H

#include <cfloat>
#include <cmath>

#include "yocto_api.h"

#ifdef YOCTOLIB_NAMESPACE
namespace YOCTOLIB_NAMESPACE
{
#endif

//--- (YAngularSpeed return codes)
//--- (end of YAngularSpeed return codes)
//--- (YAngularSpeed yapiwrapper)
//--- (end of YAngularSpeed yapiwrapper)
//--- (YAngularSpeed definitions)
class YAngularSpeed; // forward declaration

typedef void (*YAngularSpeedValueCallback)(YAngularSpeed *func, const string& functionValue);
class YMeasure; // forward declaration
typedef void (*YAngularSpeedTimedReportCallback)(YAngularSpeed *func, YMeasure measure);
//--- (end of YAngularSpeed definitions)

//--- (YAngularSpeed declaration)
/**
 * YAngularSpeed Class: tachometer control interface
 *
 * The YAngularSpeed class allows you to read and configure Yoctopuce tachometers.
 * It inherits from YSensor class the core functions to read measurements,
 * to register callback functions, and to access the autonomous datalogger.
 */
class YOCTO_CLASS_EXPORT YAngularSpeed: public YSensor {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of YAngularSpeed declaration)
protected:
    //--- (YAngularSpeed attributes)
    // Attributes (function value cache)
    YAngularSpeedValueCallback _valueCallbackAngularSpeed;
    YAngularSpeedTimedReportCallback _timedReportCallbackAngularSpeed;

    friend YAngularSpeed *yFindAngularSpeed(const string& func);
    friend YAngularSpeed *yFirstAngularSpeed(void);

    // Constructor is protected, use yFindAngularSpeed factory function to instantiate
    YAngularSpeed(const string& func);
    //--- (end of YAngularSpeed attributes)

public:
    virtual ~YAngularSpeed();
    //--- (YAngularSpeed accessors declaration)


    /**
     * Retrieves a tachometer for a given identifier.
     * The identifier can be specified using several formats:
     *
     * - FunctionLogicalName
     * - ModuleSerialNumber.FunctionIdentifier
     * - ModuleSerialNumber.FunctionLogicalName
     * - ModuleLogicalName.FunctionIdentifier
     * - ModuleLogicalName.FunctionLogicalName
     *
     *
     * This function does not require that the rtachometer is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method isOnline() to test if the rtachometer is
     * indeed online at a given time. In case of ambiguity when looking for
     * a tachometer by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     *
     * If a call to this object's is_online() method returns FALSE although
     * you are certain that the matching device is plugged, make sure that you did
     * call registerHub() at application initialization time.
     *
     * @param func : a string that uniquely characterizes the rtachometer, for instance
     *         MyDevice.angularSpeed.
     *
     * @return a YAngularSpeed object allowing you to drive the rtachometer.
     */
    static YAngularSpeed* FindAngularSpeed(string func);

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
    virtual int         registerValueCallback(YAngularSpeedValueCallback callback);
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
    virtual int         registerTimedReportCallback(YAngularSpeedTimedReportCallback callback);
    using YSensor::registerTimedReportCallback;

    virtual int         _invokeTimedReportCallback(YMeasure value);


    inline static YAngularSpeed *Find(string func)
    { return YAngularSpeed::FindAngularSpeed(func); }

    /**
     * Continues the enumeration of tachometers started using yFirstAngularSpeed().
     * Caution: You can't make any assumption about the returned tachometers order.
     * If you want to find a specific a tachometer, use AngularSpeed.findAngularSpeed()
     * and a hardwareID or a logical name.
     *
     * @return a pointer to a YAngularSpeed object, corresponding to
     *         a tachometer currently online, or a NULL pointer
     *         if there are no more tachometers to enumerate.
     */
           YAngularSpeed   *nextAngularSpeed(void);
    inline YAngularSpeed   *next(void)
    { return this->nextAngularSpeed();}

    /**
     * Starts the enumeration of tachometers currently accessible.
     * Use the method YAngularSpeed::nextAngularSpeed() to iterate on
     * next tachometers.
     *
     * @return a pointer to a YAngularSpeed object, corresponding to
     *         the first tachometer currently online, or a NULL pointer
     *         if there are none.
     */
           static YAngularSpeed *FirstAngularSpeed(void);
    inline static YAngularSpeed *First(void)
    { return YAngularSpeed::FirstAngularSpeed();}
#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of YAngularSpeed accessors declaration)
};

//--- (YAngularSpeed functions declaration)

/**
 * Retrieves a tachometer for a given identifier.
 * The identifier can be specified using several formats:
 *
 * - FunctionLogicalName
 * - ModuleSerialNumber.FunctionIdentifier
 * - ModuleSerialNumber.FunctionLogicalName
 * - ModuleLogicalName.FunctionIdentifier
 * - ModuleLogicalName.FunctionLogicalName
 *
 *
 * This function does not require that the rtachometer is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the rtachometer is
 * indeed online at a given time. In case of ambiguity when looking for
 * a tachometer by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the rtachometer, for instance
 *         MyDevice.angularSpeed.
 *
 * @return a YAngularSpeed object allowing you to drive the rtachometer.
 */
inline YAngularSpeed *yFindAngularSpeed(const string& func)
{ return YAngularSpeed::FindAngularSpeed(func);}
/**
 * Starts the enumeration of tachometers currently accessible.
 * Use the method YAngularSpeed::nextAngularSpeed() to iterate on
 * next tachometers.
 *
 * @return a pointer to a YAngularSpeed object, corresponding to
 *         the first tachometer currently online, or a NULL pointer
 *         if there are none.
 */
inline YAngularSpeed *yFirstAngularSpeed(void)
{ return YAngularSpeed::FirstAngularSpeed();}

//--- (end of YAngularSpeed functions declaration)

#ifdef YOCTOLIB_NAMESPACE
// end of namespace definition
}
#endif

#endif
