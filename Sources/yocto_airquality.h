/*********************************************************************
 *
 *  $Id: svn_id $
 *
 *  Declares yFindAirQuality(), the high-level API for AirQuality functions
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


#ifndef YOCTO_AIRQUALITY_H
#define YOCTO_AIRQUALITY_H

#include <cfloat>
#include <cmath>

#include "yocto_api.h"

#ifdef YOCTOLIB_NAMESPACE
namespace YOCTOLIB_NAMESPACE
{
#endif

//--- (YAirQuality return codes)
//--- (end of YAirQuality return codes)
//--- (YAirQuality yapiwrapper)
//--- (end of YAirQuality yapiwrapper)
//--- (YAirQuality definitions)
class YAirQuality; // forward declaration

typedef void (*YAirQualityValueCallback)(YAirQuality *func, const string& functionValue);
class YMeasure; // forward declaration
typedef void (*YAirQualityTimedReportCallback)(YAirQuality *func, YMeasure measure);
#ifndef _Y_AQIMODE_ENUM
#define _Y_AQIMODE_ENUM
typedef enum {
    Y_AQIMODE_RELATIVE = 0,
    Y_AQIMODE_UBA = 1,
    Y_AQIMODE_INVALID = -1,
} Y_AQIMODE_enum;
#endif
#define Y_UBAINDEX_INVALID              (YAPI_INVALID_DOUBLE)
#define Y_RELATIVEINDEX_INVALID         (YAPI_INVALID_DOUBLE)
//--- (end of YAirQuality definitions)

//--- (YAirQuality declaration)
/**
 * YAirQuality Class: air quality sensor control interface
 *
 * The YAirQuality class allows you to read and configure Yoctopuce air quality sensors.
 * It inherits from YSensor class the core functions to read measurements,
 * to register callback functions, and to access the autonomous datalogger.
 */
class YOCTO_CLASS_EXPORT YAirQuality: public YSensor {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of YAirQuality declaration)
protected:
    //--- (YAirQuality attributes)
    // Attributes (function value cache)
    double          _ubaIndex;
    double          _relativeIndex;
    Y_AQIMODE_enum  _aqiMode;
    YAirQualityValueCallback _valueCallbackAirQuality;
    YAirQualityTimedReportCallback _timedReportCallbackAirQuality;

    friend YAirQuality *yFindAirQuality(const string& func);
    friend YAirQuality *yFirstAirQuality(void);

    // Function-specific method for parsing of JSON output and caching result
    virtual int     _parseAttr(YJSONObject *json_val);

    // Constructor is protected, use yFindAirQuality factory function to instantiate
    YAirQuality(const string& func);
    //--- (end of YAirQuality attributes)

public:
    virtual ~YAirQuality();
    //--- (YAirQuality accessors declaration)

    static const double UBAINDEX_INVALID;
    static const double RELATIVEINDEX_INVALID;
    static const Y_AQIMODE_enum AQIMODE_RELATIVE = Y_AQIMODE_RELATIVE;
    static const Y_AQIMODE_enum AQIMODE_UBA = Y_AQIMODE_UBA;
    static const Y_AQIMODE_enum AQIMODE_INVALID = Y_AQIMODE_INVALID;

    /**
     * Returns the current air quality index, according to UBA (from 1 to 5).
     *
     * @return a floating point number corresponding to the current air quality index, according to UBA (from 1 to 5)
     *
     * On failure, throws an exception or returns YAirQuality::UBAINDEX_INVALID.
     */
    double              get_ubaIndex(void);

    inline double       ubaIndex(void)
    { return this->get_ubaIndex(); }

    /**
     * Returns the relative air quality index, according to ScioSense (from 0 to 500).
     * A value below 100 indicates better-than-average air quality compared to the past 24 hours,
     * while a value above 100 indicates poorer-than-average air quality compared to the past 24 hours.
     *
     * @return a floating point number corresponding to the relative air quality index, according to
     * ScioSense (from 0 to 500)
     *
     * On failure, throws an exception or returns YAirQuality::RELATIVEINDEX_INVALID.
     */
    double              get_relativeIndex(void);

    inline double       relativeIndex(void)
    { return this->get_relativeIndex(); }

    /**
     * Returns the type of index reported by the get_currentValue function and callbacks (UBA index or relative index).
     *
     * @return either YAirQuality::AQIMODE_RELATIVE or YAirQuality::AQIMODE_UBA, according to the type of
     * index reported by the get_currentValue function and callbacks (UBA index or relative index)
     *
     * On failure, throws an exception or returns YAirQuality::AQIMODE_INVALID.
     */
    Y_AQIMODE_enum      get_aqiMode(void);

    inline Y_AQIMODE_enum aqiMode(void)
    { return this->get_aqiMode(); }

    /**
     * Changes the the type of index reported by the get_currentValue function and callbacks (UBA index or
     * relative index).
     * Remember to call the saveToFlash() method of the module if the modification must be kept.
     *
     * @param newval : either YAirQuality::AQIMODE_RELATIVE or YAirQuality::AQIMODE_UBA, according to the
     * the type of index reported by the get_currentValue function and callbacks (UBA index or relative index)
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_aqiMode(Y_AQIMODE_enum newval);
    inline int      setAqiMode(Y_AQIMODE_enum newval)
    { return this->set_aqiMode(newval); }

    /**
     * Retrieves a air quality sensor for a given identifier.
     * The identifier can be specified using several formats:
     *
     * - FunctionLogicalName
     * - ModuleSerialNumber.FunctionIdentifier
     * - ModuleSerialNumber.FunctionLogicalName
     * - ModuleLogicalName.FunctionIdentifier
     * - ModuleLogicalName.FunctionLogicalName
     *
     *
     * This function does not require that the air quality sensor is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method isOnline() to test if the air quality sensor is
     * indeed online at a given time. In case of ambiguity when looking for
     * a air quality sensor by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     *
     * If a call to this object's is_online() method returns FALSE although
     * you are certain that the matching device is plugged, make sure that you did
     * call registerHub() at application initialization time.
     *
     * @param func : a string that uniquely characterizes the air quality sensor, for instance
     *         MyDevice.airQuality.
     *
     * @return a YAirQuality object allowing you to drive the air quality sensor.
     */
    static YAirQuality* FindAirQuality(string func);

    /**
     * Registers the callback function that is invoked on every change of advertised value.
     * The callback is then invoked only during the execution of ySleep or yHandleEvents.
     * This provides control over the time when the callback is triggered. For good responsiveness,
     * remember to call one of these two functions periodically. The callback is called once juste after beeing
     * registered, passing the current advertised value  of the function, provided that it is not an empty string.
     * To unregister a callback, pass a NULL pointer as argument.
     *
     * @param callback : the callback function to call, or a NULL pointer. The callback function should take two
     *         arguments: the function object of which the value has changed, and the character string describing
     *         the new advertised value.
     * @noreturn
     */
    virtual int         registerValueCallback(YAirQualityValueCallback callback);
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
    virtual int         registerTimedReportCallback(YAirQualityTimedReportCallback callback);
    using YSensor::registerTimedReportCallback;

    virtual int         _invokeTimedReportCallback(YMeasure value);


    inline static YAirQuality *Find(string func)
    { return YAirQuality::FindAirQuality(func); }

    /**
     * Continues the enumeration of air quality sensors started using yFirstAirQuality().
     * Caution: You can't make any assumption about the returned air quality sensors order.
     * If you want to find a specific a air quality sensor, use AirQuality.findAirQuality()
     * and a hardwareID or a logical name.
     *
     * @return a pointer to a YAirQuality object, corresponding to
     *         a air quality sensor currently online, or a NULL pointer
     *         if there are no more air quality sensors to enumerate.
     */
           YAirQuality     *nextAirQuality(void);
    inline YAirQuality     *next(void)
    { return this->nextAirQuality();}

    /**
     * Starts the enumeration of air quality sensors currently accessible.
     * Use the method YAirQuality::nextAirQuality() to iterate on
     * next air quality sensors.
     *
     * @return a pointer to a YAirQuality object, corresponding to
     *         the first air quality sensor currently online, or a NULL pointer
     *         if there are none.
     */
           static YAirQuality *FirstAirQuality(void);
    inline static YAirQuality *First(void)
    { return YAirQuality::FirstAirQuality();}
#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of YAirQuality accessors declaration)
};

//--- (YAirQuality functions declaration)

/**
 * Retrieves a air quality sensor for a given identifier.
 * The identifier can be specified using several formats:
 *
 * - FunctionLogicalName
 * - ModuleSerialNumber.FunctionIdentifier
 * - ModuleSerialNumber.FunctionLogicalName
 * - ModuleLogicalName.FunctionIdentifier
 * - ModuleLogicalName.FunctionLogicalName
 *
 *
 * This function does not require that the air quality sensor is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the air quality sensor is
 * indeed online at a given time. In case of ambiguity when looking for
 * a air quality sensor by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the air quality sensor, for instance
 *         MyDevice.airQuality.
 *
 * @return a YAirQuality object allowing you to drive the air quality sensor.
 */
inline YAirQuality *yFindAirQuality(const string& func)
{ return YAirQuality::FindAirQuality(func);}
/**
 * Starts the enumeration of air quality sensors currently accessible.
 * Use the method YAirQuality::nextAirQuality() to iterate on
 * next air quality sensors.
 *
 * @return a pointer to a YAirQuality object, corresponding to
 *         the first air quality sensor currently online, or a NULL pointer
 *         if there are none.
 */
inline YAirQuality *yFirstAirQuality(void)
{ return YAirQuality::FirstAirQuality();}

//--- (end of YAirQuality functions declaration)

#ifdef YOCTOLIB_NAMESPACE
// end of namespace definition
}
#endif

#endif
