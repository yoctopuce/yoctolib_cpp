/*********************************************************************
 *
 *  $Id: svn_id $
 *
 *  Declares yFindSoundLevel(), the high-level API for SoundLevel functions
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


#ifndef YOCTO_SOUNDLEVEL_H
#define YOCTO_SOUNDLEVEL_H

#include <cfloat>
#include <cmath>

#include "yocto_api.h"

#ifdef YOCTOLIB_NAMESPACE
namespace YOCTOLIB_NAMESPACE
{
#endif

//--- (YSoundLevel return codes)
//--- (end of YSoundLevel return codes)
//--- (YSoundLevel yapiwrapper)
//--- (end of YSoundLevel yapiwrapper)
//--- (YSoundLevel definitions)
class YSoundLevel; // forward declaration

typedef void (*YSoundLevelValueCallback)(YSoundLevel *func, const string& functionValue);
class YMeasure; // forward declaration
typedef void (*YSoundLevelTimedReportCallback)(YSoundLevel *func, YMeasure measure);
#define Y_LABEL_INVALID                 (YAPI_INVALID_STRING)
#define Y_INTEGRATIONTIME_INVALID       (YAPI_INVALID_UINT)
//--- (end of YSoundLevel definitions)

//--- (YSoundLevel declaration)
/**
 * YSoundLevel Class: sound pressure level meter control interface
 *
 * The YSoundLevel class allows you to read and configure Yoctopuce sound pressure level meters.
 * It inherits from YSensor class the core functions to read measurements,
 * to register callback functions, and to access the autonomous datalogger.
 */
class YOCTO_CLASS_EXPORT YSoundLevel: public YSensor {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of YSoundLevel declaration)
protected:
    //--- (YSoundLevel attributes)
    // Attributes (function value cache)
    string          _label;
    int             _integrationTime;
    YSoundLevelValueCallback _valueCallbackSoundLevel;
    YSoundLevelTimedReportCallback _timedReportCallbackSoundLevel;

    friend YSoundLevel *yFindSoundLevel(const string& func);
    friend YSoundLevel *yFirstSoundLevel(void);

    // Function-specific method for parsing of JSON output and caching result
    virtual int     _parseAttr(YJSONObject *json_val);

    // Constructor is protected, use yFindSoundLevel factory function to instantiate
    YSoundLevel(const string& func);
    //--- (end of YSoundLevel attributes)

public:
    virtual ~YSoundLevel();
    //--- (YSoundLevel accessors declaration)

    static const string LABEL_INVALID;
    static const int INTEGRATIONTIME_INVALID = YAPI_INVALID_UINT;

    /**
     * Changes the measuring unit for the sound pressure level (dBA, dBC or dBZ).
     * That unit will directly determine frequency weighting to be used to compute
     * the measured value. Remember to call the saveToFlash() method of the
     * module if the modification must be kept.
     *
     * @param newval : a string corresponding to the measuring unit for the sound pressure level (dBA, dBC or dBZ)
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_unit(const string& newval);
    inline int      setUnit(const string& newval)
    { return this->set_unit(newval); }

    /**
     * Returns the label for the sound pressure level measurement, as per
     * IEC standard 61672-1:2013.
     *
     * @return a string corresponding to the label for the sound pressure level measurement, as per
     *         IEC standard 61672-1:2013
     *
     * On failure, throws an exception or returns YSoundLevel::LABEL_INVALID.
     */
    string              get_label(void);

    inline string       label(void)
    { return this->get_label(); }

    /**
     * Returns the integration time in milliseconds for measuring the sound pressure level.
     *
     * @return an integer corresponding to the integration time in milliseconds for measuring the sound pressure level
     *
     * On failure, throws an exception or returns YSoundLevel::INTEGRATIONTIME_INVALID.
     */
    int                 get_integrationTime(void);

    inline int          integrationTime(void)
    { return this->get_integrationTime(); }

    /**
     * Retrieves a sound pressure level meter for a given identifier.
     * The identifier can be specified using several formats:
     *
     * - FunctionLogicalName
     * - ModuleSerialNumber.FunctionIdentifier
     * - ModuleSerialNumber.FunctionLogicalName
     * - ModuleLogicalName.FunctionIdentifier
     * - ModuleLogicalName.FunctionLogicalName
     *
     *
     * This function does not require that the sound pressure level meter is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method isOnline() to test if the sound pressure level meter is
     * indeed online at a given time. In case of ambiguity when looking for
     * a sound pressure level meter by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     *
     * If a call to this object's is_online() method returns FALSE although
     * you are certain that the matching device is plugged, make sure that you did
     * call registerHub() at application initialization time.
     *
     * @param func : a string that uniquely characterizes the sound pressure level meter, for instance
     *         MyDevice.soundLevel1.
     *
     * @return a YSoundLevel object allowing you to drive the sound pressure level meter.
     */
    static YSoundLevel* FindSoundLevel(string func);

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
    virtual int         registerValueCallback(YSoundLevelValueCallback callback);
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
    virtual int         registerTimedReportCallback(YSoundLevelTimedReportCallback callback);
    using YSensor::registerTimedReportCallback;

    virtual int         _invokeTimedReportCallback(YMeasure value);


    inline static YSoundLevel *Find(string func)
    { return YSoundLevel::FindSoundLevel(func); }

    /**
     * Continues the enumeration of sound pressure level meters started using yFirstSoundLevel().
     * Caution: You can't make any assumption about the returned sound pressure level meters order.
     * If you want to find a specific a sound pressure level meter, use SoundLevel.findSoundLevel()
     * and a hardwareID or a logical name.
     *
     * @return a pointer to a YSoundLevel object, corresponding to
     *         a sound pressure level meter currently online, or a NULL pointer
     *         if there are no more sound pressure level meters to enumerate.
     */
           YSoundLevel     *nextSoundLevel(void);
    inline YSoundLevel     *next(void)
    { return this->nextSoundLevel();}

    /**
     * Starts the enumeration of sound pressure level meters currently accessible.
     * Use the method YSoundLevel::nextSoundLevel() to iterate on
     * next sound pressure level meters.
     *
     * @return a pointer to a YSoundLevel object, corresponding to
     *         the first sound pressure level meter currently online, or a NULL pointer
     *         if there are none.
     */
           static YSoundLevel *FirstSoundLevel(void);
    inline static YSoundLevel *First(void)
    { return YSoundLevel::FirstSoundLevel();}
#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of YSoundLevel accessors declaration)
};

//--- (YSoundLevel functions declaration)

/**
 * Retrieves a sound pressure level meter for a given identifier.
 * The identifier can be specified using several formats:
 *
 * - FunctionLogicalName
 * - ModuleSerialNumber.FunctionIdentifier
 * - ModuleSerialNumber.FunctionLogicalName
 * - ModuleLogicalName.FunctionIdentifier
 * - ModuleLogicalName.FunctionLogicalName
 *
 *
 * This function does not require that the sound pressure level meter is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the sound pressure level meter is
 * indeed online at a given time. In case of ambiguity when looking for
 * a sound pressure level meter by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the sound pressure level meter, for instance
 *         MyDevice.soundLevel1.
 *
 * @return a YSoundLevel object allowing you to drive the sound pressure level meter.
 */
inline YSoundLevel *yFindSoundLevel(const string& func)
{ return YSoundLevel::FindSoundLevel(func);}
/**
 * Starts the enumeration of sound pressure level meters currently accessible.
 * Use the method YSoundLevel::nextSoundLevel() to iterate on
 * next sound pressure level meters.
 *
 * @return a pointer to a YSoundLevel object, corresponding to
 *         the first sound pressure level meter currently online, or a NULL pointer
 *         if there are none.
 */
inline YSoundLevel *yFirstSoundLevel(void)
{ return YSoundLevel::FirstSoundLevel();}

//--- (end of YSoundLevel functions declaration)

#ifdef YOCTOLIB_NAMESPACE
// end of namespace definition
}
#endif

#endif
