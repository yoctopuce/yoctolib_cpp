/*********************************************************************
 *
 *  $Id: yocto_tvoc.h 44049 2021-02-26 10:57:40Z web $
 *
 *  Declares yFindTvoc(), the high-level API for Tvoc functions
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


#ifndef YOCTO_TVOC_H
#define YOCTO_TVOC_H

#include <cfloat>
#include <cmath>

#include "yocto_api.h"

#ifdef YOCTOLIB_NAMESPACE
namespace YOCTOLIB_NAMESPACE
{
#endif

//--- (YTvoc return codes)
//--- (end of YTvoc return codes)
//--- (YTvoc yapiwrapper)
//--- (end of YTvoc yapiwrapper)
//--- (YTvoc definitions)
class YTvoc; // forward declaration

typedef void (*YTvocValueCallback)(YTvoc *func, const string& functionValue);
class YMeasure; // forward declaration
typedef void (*YTvocTimedReportCallback)(YTvoc *func, YMeasure measure);
//--- (end of YTvoc definitions)

//--- (YTvoc declaration)
/**
 * YTvoc Class: Total Volatile Organic Compound sensor control interface, available for instance in
 * the Yocto-VOC-V3
 *
 * The YTvoc class allows you to read and configure Yoctopuce Total Volatile Organic Compound sensors.
 * It inherits from YSensor class the core functions to read measurements,
 * to register callback functions, and to access the autonomous datalogger.
 */
class YOCTO_CLASS_EXPORT YTvoc: public YSensor {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of YTvoc declaration)
protected:
    //--- (YTvoc attributes)
    // Attributes (function value cache)
    YTvocValueCallback _valueCallbackTvoc;
    YTvocTimedReportCallback _timedReportCallbackTvoc;

    friend YTvoc *yFindTvoc(const string& func);
    friend YTvoc *yFirstTvoc(void);

    // Constructor is protected, use yFindTvoc factory function to instantiate
    YTvoc(const string& func);
    //--- (end of YTvoc attributes)

public:
    virtual ~YTvoc();
    //--- (YTvoc accessors declaration)


    /**
     * Retrieves a Total  Volatile Organic Compound sensor for a given identifier.
     * The identifier can be specified using several formats:
     * <ul>
     * <li>FunctionLogicalName</li>
     * <li>ModuleSerialNumber.FunctionIdentifier</li>
     * <li>ModuleSerialNumber.FunctionLogicalName</li>
     * <li>ModuleLogicalName.FunctionIdentifier</li>
     * <li>ModuleLogicalName.FunctionLogicalName</li>
     * </ul>
     *
     * This function does not require that the Total  Volatile Organic Compound sensor is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method isOnline() to test if the Total  Volatile Organic Compound sensor is
     * indeed online at a given time. In case of ambiguity when looking for
     * a Total  Volatile Organic Compound sensor by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     *
     * If a call to this object's is_online() method returns FALSE although
     * you are certain that the matching device is plugged, make sure that you did
     * call registerHub() at application initialization time.
     *
     * @param func : a string that uniquely characterizes the Total  Volatile Organic Compound sensor, for instance
     *         YVOCMK03.tvoc.
     *
     * @return a YTvoc object allowing you to drive the Total  Volatile Organic Compound sensor.
     */
    static YTvoc*       FindTvoc(string func);

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
    virtual int         registerValueCallback(YTvocValueCallback callback);
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
    virtual int         registerTimedReportCallback(YTvocTimedReportCallback callback);
    using YSensor::registerTimedReportCallback;

    virtual int         _invokeTimedReportCallback(YMeasure value);


    inline static YTvoc *Find(string func)
    { return YTvoc::FindTvoc(func); }

    /**
     * Continues the enumeration of Total Volatile Organic Compound sensors started using yFirstTvoc().
     * Caution: You can't make any assumption about the returned Total Volatile Organic Compound sensors order.
     * If you want to find a specific a Total  Volatile Organic Compound sensor, use Tvoc.findTvoc()
     * and a hardwareID or a logical name.
     *
     * @return a pointer to a YTvoc object, corresponding to
     *         a Total  Volatile Organic Compound sensor currently online, or a NULL pointer
     *         if there are no more Total Volatile Organic Compound sensors to enumerate.
     */
           YTvoc           *nextTvoc(void);
    inline YTvoc           *next(void)
    { return this->nextTvoc();}

    /**
     * Starts the enumeration of Total Volatile Organic Compound sensors currently accessible.
     * Use the method YTvoc::nextTvoc() to iterate on
     * next Total Volatile Organic Compound sensors.
     *
     * @return a pointer to a YTvoc object, corresponding to
     *         the first Total Volatile Organic Compound sensor currently online, or a NULL pointer
     *         if there are none.
     */
           static YTvoc *FirstTvoc(void);
    inline static YTvoc *First(void)
    { return YTvoc::FirstTvoc();}
#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of YTvoc accessors declaration)
};

//--- (YTvoc functions declaration)

/**
 * Retrieves a Total  Volatile Organic Compound sensor for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that the Total  Volatile Organic Compound sensor is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the Total  Volatile Organic Compound sensor is
 * indeed online at a given time. In case of ambiguity when looking for
 * a Total  Volatile Organic Compound sensor by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the Total  Volatile Organic Compound sensor, for instance
 *         YVOCMK03.tvoc.
 *
 * @return a YTvoc object allowing you to drive the Total  Volatile Organic Compound sensor.
 */
inline YTvoc *yFindTvoc(const string& func)
{ return YTvoc::FindTvoc(func);}
/**
 * Starts the enumeration of Total Volatile Organic Compound sensors currently accessible.
 * Use the method YTvoc::nextTvoc() to iterate on
 * next Total Volatile Organic Compound sensors.
 *
 * @return a pointer to a YTvoc object, corresponding to
 *         the first Total Volatile Organic Compound sensor currently online, or a NULL pointer
 *         if there are none.
 */
inline YTvoc *yFirstTvoc(void)
{ return YTvoc::FirstTvoc();}

//--- (end of YTvoc functions declaration)

#ifdef YOCTOLIB_NAMESPACE
// end of namespace definition
}
#endif

#endif
