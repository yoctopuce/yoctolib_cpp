/*********************************************************************
 *
 *  $Id: yocto_compass.h 44049 2021-02-26 10:57:40Z web $
 *
 *  Declares yFindCompass(), the high-level API for Compass functions
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


#ifndef YOCTO_COMPASS_H
#define YOCTO_COMPASS_H

#include <cfloat>
#include <cmath>

#include "yocto_api.h"

#ifdef YOCTOLIB_NAMESPACE
namespace YOCTOLIB_NAMESPACE
{
#endif

//--- (YCompass return codes)
//--- (end of YCompass return codes)
//--- (YCompass yapiwrapper)
//--- (end of YCompass yapiwrapper)
//--- (YCompass definitions)
class YCompass; // forward declaration

typedef void (*YCompassValueCallback)(YCompass *func, const string& functionValue);
class YMeasure; // forward declaration
typedef void (*YCompassTimedReportCallback)(YCompass *func, YMeasure measure);
#ifndef _Y_AXIS_ENUM
#define _Y_AXIS_ENUM
typedef enum {
    Y_AXIS_X = 0,
    Y_AXIS_Y = 1,
    Y_AXIS_Z = 2,
    Y_AXIS_INVALID = -1,
} Y_AXIS_enum;
#endif
#define Y_BANDWIDTH_INVALID             (YAPI_INVALID_UINT)
#define Y_MAGNETICHEADING_INVALID       (YAPI_INVALID_DOUBLE)
//--- (end of YCompass definitions)

//--- (YCompass declaration)
/**
 * YCompass Class: compass function control interface, available for instance in the Yocto-3D-V2
 *
 * The YCompass class allows you to read and configure Yoctopuce compass functions.
 * It inherits from YSensor class the core functions to read measurements,
 * to register callback functions, and to access the autonomous datalogger.
 */
class YOCTO_CLASS_EXPORT YCompass: public YSensor {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of YCompass declaration)
protected:
    //--- (YCompass attributes)
    // Attributes (function value cache)
    int             _bandwidth;
    Y_AXIS_enum     _axis;
    double          _magneticHeading;
    YCompassValueCallback _valueCallbackCompass;
    YCompassTimedReportCallback _timedReportCallbackCompass;

    friend YCompass *yFindCompass(const string& func);
    friend YCompass *yFirstCompass(void);

    // Function-specific method for parsing of JSON output and caching result
    virtual int     _parseAttr(YJSONObject *json_val);

    // Constructor is protected, use yFindCompass factory function to instantiate
    YCompass(const string& func);
    //--- (end of YCompass attributes)

public:
    virtual ~YCompass();
    //--- (YCompass accessors declaration)

    static const int BANDWIDTH_INVALID = YAPI_INVALID_UINT;
    static const Y_AXIS_enum AXIS_X = Y_AXIS_X;
    static const Y_AXIS_enum AXIS_Y = Y_AXIS_Y;
    static const Y_AXIS_enum AXIS_Z = Y_AXIS_Z;
    static const Y_AXIS_enum AXIS_INVALID = Y_AXIS_INVALID;
    static const double MAGNETICHEADING_INVALID;

    /**
     * Returns the measure update frequency, measured in Hz.
     *
     * @return an integer corresponding to the measure update frequency, measured in Hz
     *
     * On failure, throws an exception or returns YCompass::BANDWIDTH_INVALID.
     */
    int                 get_bandwidth(void);

    inline int          bandwidth(void)
    { return this->get_bandwidth(); }

    /**
     * Changes the measure update frequency, measured in Hz. When the
     * frequency is lower, the device performs averaging.
     * Remember to call the saveToFlash()
     * method of the module if the modification must be kept.
     *
     * @param newval : an integer corresponding to the measure update frequency, measured in Hz
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_bandwidth(int newval);
    inline int      setBandwidth(int newval)
    { return this->set_bandwidth(newval); }

    Y_AXIS_enum         get_axis(void);

    inline Y_AXIS_enum  axis(void)
    { return this->get_axis(); }

    /**
     * Returns the magnetic heading, regardless of the configured bearing.
     *
     * @return a floating point number corresponding to the magnetic heading, regardless of the configured bearing
     *
     * On failure, throws an exception or returns YCompass::MAGNETICHEADING_INVALID.
     */
    double              get_magneticHeading(void);

    inline double       magneticHeading(void)
    { return this->get_magneticHeading(); }

    /**
     * Retrieves a compass function for a given identifier.
     * The identifier can be specified using several formats:
     * <ul>
     * <li>FunctionLogicalName</li>
     * <li>ModuleSerialNumber.FunctionIdentifier</li>
     * <li>ModuleSerialNumber.FunctionLogicalName</li>
     * <li>ModuleLogicalName.FunctionIdentifier</li>
     * <li>ModuleLogicalName.FunctionLogicalName</li>
     * </ul>
     *
     * This function does not require that the compass function is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method isOnline() to test if the compass function is
     * indeed online at a given time. In case of ambiguity when looking for
     * a compass function by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     *
     * If a call to this object's is_online() method returns FALSE although
     * you are certain that the matching device is plugged, make sure that you did
     * call registerHub() at application initialization time.
     *
     * @param func : a string that uniquely characterizes the compass function, for instance
     *         Y3DMK002.compass.
     *
     * @return a YCompass object allowing you to drive the compass function.
     */
    static YCompass*    FindCompass(string func);

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
    virtual int         registerValueCallback(YCompassValueCallback callback);
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
    virtual int         registerTimedReportCallback(YCompassTimedReportCallback callback);
    using YSensor::registerTimedReportCallback;

    virtual int         _invokeTimedReportCallback(YMeasure value);


    inline static YCompass *Find(string func)
    { return YCompass::FindCompass(func); }

    /**
     * Continues the enumeration of compass functions started using yFirstCompass().
     * Caution: You can't make any assumption about the returned compass functions order.
     * If you want to find a specific a compass function, use Compass.findCompass()
     * and a hardwareID or a logical name.
     *
     * @return a pointer to a YCompass object, corresponding to
     *         a compass function currently online, or a NULL pointer
     *         if there are no more compass functions to enumerate.
     */
           YCompass        *nextCompass(void);
    inline YCompass        *next(void)
    { return this->nextCompass();}

    /**
     * Starts the enumeration of compass functions currently accessible.
     * Use the method YCompass::nextCompass() to iterate on
     * next compass functions.
     *
     * @return a pointer to a YCompass object, corresponding to
     *         the first compass function currently online, or a NULL pointer
     *         if there are none.
     */
           static YCompass *FirstCompass(void);
    inline static YCompass *First(void)
    { return YCompass::FirstCompass();}
#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of YCompass accessors declaration)
};

//--- (YCompass functions declaration)

/**
 * Retrieves a compass function for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that the compass function is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the compass function is
 * indeed online at a given time. In case of ambiguity when looking for
 * a compass function by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the compass function, for instance
 *         Y3DMK002.compass.
 *
 * @return a YCompass object allowing you to drive the compass function.
 */
inline YCompass *yFindCompass(const string& func)
{ return YCompass::FindCompass(func);}
/**
 * Starts the enumeration of compass functions currently accessible.
 * Use the method YCompass::nextCompass() to iterate on
 * next compass functions.
 *
 * @return a pointer to a YCompass object, corresponding to
 *         the first compass function currently online, or a NULL pointer
 *         if there are none.
 */
inline YCompass *yFirstCompass(void)
{ return YCompass::FirstCompass();}

//--- (end of YCompass functions declaration)

#ifdef YOCTOLIB_NAMESPACE
// end of namespace definition
}
#endif

#endif
