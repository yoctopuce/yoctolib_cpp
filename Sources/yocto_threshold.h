/*********************************************************************
 *
 *  $Id: svn_id $
 *
 *  Declares yFindThreshold(), the high-level API for Threshold functions
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


#ifndef YOCTO_THRESHOLD_H
#define YOCTO_THRESHOLD_H

#include <cfloat>
#include <cmath>

#include "yocto_api.h"

#ifdef YOCTOLIB_NAMESPACE
namespace YOCTOLIB_NAMESPACE
{
#endif

//--- (YThreshold return codes)
//--- (end of YThreshold return codes)
//--- (YThreshold yapiwrapper)
//--- (end of YThreshold yapiwrapper)
//--- (YThreshold definitions)
class YThreshold; // forward declaration

typedef void (*YThresholdValueCallback)(YThreshold *func, const string& functionValue);
#ifndef _Y_THRESHOLDSTATE_ENUM
#define _Y_THRESHOLDSTATE_ENUM
typedef enum {
    Y_THRESHOLDSTATE_SAFE = 0,
    Y_THRESHOLDSTATE_ALERT = 1,
    Y_THRESHOLDSTATE_INVALID = -1,
} Y_THRESHOLDSTATE_enum;
#endif
#define Y_TARGETSENSOR_INVALID          (YAPI_INVALID_STRING)
#define Y_ALERTLEVEL_INVALID            (YAPI_INVALID_DOUBLE)
#define Y_SAFELEVEL_INVALID             (YAPI_INVALID_DOUBLE)
//--- (end of YThreshold definitions)

//--- (YThreshold declaration)
/**
 * YThreshold Class: Control interface to define a threshold
 *
 * The Threshold class allows you define a threshold on a Yoctopuce sensor
 * to trigger a predefined action, on specific devices where this is implemented.
 */
class YOCTO_CLASS_EXPORT YThreshold: public YFunction {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of YThreshold declaration)
protected:
    //--- (YThreshold attributes)
    // Attributes (function value cache)
    Y_THRESHOLDSTATE_enum _thresholdState;
    string          _targetSensor;
    double          _alertLevel;
    double          _safeLevel;
    YThresholdValueCallback _valueCallbackThreshold;

    friend YThreshold *yFindThreshold(const string& func);
    friend YThreshold *yFirstThreshold(void);

    // Function-specific method for parsing of JSON output and caching result
    virtual int     _parseAttr(YJSONObject *json_val);

    // Constructor is protected, use yFindThreshold factory function to instantiate
    YThreshold(const string& func);
    //--- (end of YThreshold attributes)

public:
    virtual ~YThreshold();
    //--- (YThreshold accessors declaration)

    static const Y_THRESHOLDSTATE_enum THRESHOLDSTATE_SAFE = Y_THRESHOLDSTATE_SAFE;
    static const Y_THRESHOLDSTATE_enum THRESHOLDSTATE_ALERT = Y_THRESHOLDSTATE_ALERT;
    static const Y_THRESHOLDSTATE_enum THRESHOLDSTATE_INVALID = Y_THRESHOLDSTATE_INVALID;
    static const string TARGETSENSOR_INVALID;
    static const double ALERTLEVEL_INVALID;
    static const double SAFELEVEL_INVALID;

    /**
     * Returns current state of the threshold function.
     *
     * @return either YThreshold::THRESHOLDSTATE_SAFE or YThreshold::THRESHOLDSTATE_ALERT, according to
     * current state of the threshold function
     *
     * On failure, throws an exception or returns YThreshold::THRESHOLDSTATE_INVALID.
     */
    Y_THRESHOLDSTATE_enum get_thresholdState(void);

    inline Y_THRESHOLDSTATE_enum thresholdState(void)
    { return this->get_thresholdState(); }

    /**
     * Returns the name of the sensor monitored by the threshold function.
     *
     * @return a string corresponding to the name of the sensor monitored by the threshold function
     *
     * On failure, throws an exception or returns YThreshold::TARGETSENSOR_INVALID.
     */
    string              get_targetSensor(void);

    inline string       targetSensor(void)
    { return this->get_targetSensor(); }

    /**
     * Changes the sensor alert level triggering the threshold function.
     * Remember to call the matching module saveToFlash()
     * method if you want to preserve the setting after reboot.
     *
     * @param newval : a floating point number corresponding to the sensor alert level triggering the
     * threshold function
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_alertLevel(double newval);
    inline int      setAlertLevel(double newval)
    { return this->set_alertLevel(newval); }

    /**
     * Returns the sensor alert level, triggering the threshold function.
     *
     * @return a floating point number corresponding to the sensor alert level, triggering the threshold function
     *
     * On failure, throws an exception or returns YThreshold::ALERTLEVEL_INVALID.
     */
    double              get_alertLevel(void);

    inline double       alertLevel(void)
    { return this->get_alertLevel(); }

    /**
     * Changes the sensor acceptable level for disabling the threshold function.
     * Remember to call the matching module saveToFlash()
     * method if you want to preserve the setting after reboot.
     *
     * @param newval : a floating point number corresponding to the sensor acceptable level for disabling
     * the threshold function
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_safeLevel(double newval);
    inline int      setSafeLevel(double newval)
    { return this->set_safeLevel(newval); }

    /**
     * Returns the sensor acceptable level for disabling the threshold function.
     *
     * @return a floating point number corresponding to the sensor acceptable level for disabling the
     * threshold function
     *
     * On failure, throws an exception or returns YThreshold::SAFELEVEL_INVALID.
     */
    double              get_safeLevel(void);

    inline double       safeLevel(void)
    { return this->get_safeLevel(); }

    /**
     * Retrieves a threshold function for a given identifier.
     * The identifier can be specified using several formats:
     *
     * - FunctionLogicalName
     * - ModuleSerialNumber.FunctionIdentifier
     * - ModuleSerialNumber.FunctionLogicalName
     * - ModuleLogicalName.FunctionIdentifier
     * - ModuleLogicalName.FunctionLogicalName
     *
     *
     * This function does not require that the threshold function is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method isOnline() to test if the threshold function is
     * indeed online at a given time. In case of ambiguity when looking for
     * a threshold function by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     *
     * If a call to this object's is_online() method returns FALSE although
     * you are certain that the matching device is plugged, make sure that you did
     * call registerHub() at application initialization time.
     *
     * @param func : a string that uniquely characterizes the threshold function, for instance
     *         MyDevice.threshold1.
     *
     * @return a YThreshold object allowing you to drive the threshold function.
     */
    static YThreshold*  FindThreshold(string func);

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
    virtual int         registerValueCallback(YThresholdValueCallback callback);
    using YFunction::registerValueCallback;

    virtual int         _invokeValueCallback(string value);


    inline static YThreshold *Find(string func)
    { return YThreshold::FindThreshold(func); }

    /**
     * Continues the enumeration of threshold functions started using yFirstThreshold().
     * Caution: You can't make any assumption about the returned threshold functions order.
     * If you want to find a specific a threshold function, use Threshold.findThreshold()
     * and a hardwareID or a logical name.
     *
     * @return a pointer to a YThreshold object, corresponding to
     *         a threshold function currently online, or a NULL pointer
     *         if there are no more threshold functions to enumerate.
     */
           YThreshold      *nextThreshold(void);
    inline YThreshold      *next(void)
    { return this->nextThreshold();}

    /**
     * Starts the enumeration of threshold functions currently accessible.
     * Use the method YThreshold::nextThreshold() to iterate on
     * next threshold functions.
     *
     * @return a pointer to a YThreshold object, corresponding to
     *         the first threshold function currently online, or a NULL pointer
     *         if there are none.
     */
           static YThreshold *FirstThreshold(void);
    inline static YThreshold *First(void)
    { return YThreshold::FirstThreshold();}
#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of YThreshold accessors declaration)
};

//--- (YThreshold functions declaration)

/**
 * Retrieves a threshold function for a given identifier.
 * The identifier can be specified using several formats:
 *
 * - FunctionLogicalName
 * - ModuleSerialNumber.FunctionIdentifier
 * - ModuleSerialNumber.FunctionLogicalName
 * - ModuleLogicalName.FunctionIdentifier
 * - ModuleLogicalName.FunctionLogicalName
 *
 *
 * This function does not require that the threshold function is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the threshold function is
 * indeed online at a given time. In case of ambiguity when looking for
 * a threshold function by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the threshold function, for instance
 *         MyDevice.threshold1.
 *
 * @return a YThreshold object allowing you to drive the threshold function.
 */
inline YThreshold *yFindThreshold(const string& func)
{ return YThreshold::FindThreshold(func);}
/**
 * Starts the enumeration of threshold functions currently accessible.
 * Use the method YThreshold::nextThreshold() to iterate on
 * next threshold functions.
 *
 * @return a pointer to a YThreshold object, corresponding to
 *         the first threshold function currently online, or a NULL pointer
 *         if there are none.
 */
inline YThreshold *yFirstThreshold(void)
{ return YThreshold::FirstThreshold();}

//--- (end of YThreshold functions declaration)

#ifdef YOCTOLIB_NAMESPACE
// end of namespace definition
}
#endif

#endif
