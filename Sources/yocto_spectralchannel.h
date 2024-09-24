/*********************************************************************
 *
 *  $Id: svn_id $
 *
 *  Declares yFindSpectralChannel(), the high-level API for SpectralChannel functions
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


#ifndef YOCTO_SPECTRALCHANNEL_H
#define YOCTO_SPECTRALCHANNEL_H

#include <cfloat>
#include <cmath>

#include "yocto_api.h"

#ifdef YOCTOLIB_NAMESPACE
namespace YOCTOLIB_NAMESPACE
{
#endif

//--- (YSpectralChannel return codes)
//--- (end of YSpectralChannel return codes)
//--- (YSpectralChannel yapiwrapper)
//--- (end of YSpectralChannel yapiwrapper)
//--- (YSpectralChannel definitions)
class YSpectralChannel; // forward declaration

typedef void (*YSpectralChannelValueCallback)(YSpectralChannel *func, const string& functionValue);
class YMeasure; // forward declaration
typedef void (*YSpectralChannelTimedReportCallback)(YSpectralChannel *func, YMeasure measure);
#define Y_RAWCOUNT_INVALID              (YAPI_INVALID_INT)
//--- (end of YSpectralChannel definitions)

//--- (YSpectralChannel declaration)
/**
 * YSpectralChannel Class: spectral analysis channel control interface
 *
 * The YSpectralChannel class allows you to read and configure Yoctopuce spectral analysis channels.
 * It inherits from YSensor class the core functions to read measurements,
 * to register callback functions, and to access the autonomous datalogger.
 */
class YOCTO_CLASS_EXPORT YSpectralChannel: public YSensor {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of YSpectralChannel declaration)
protected:
    //--- (YSpectralChannel attributes)
    // Attributes (function value cache)
    int             _rawCount;
    YSpectralChannelValueCallback _valueCallbackSpectralChannel;
    YSpectralChannelTimedReportCallback _timedReportCallbackSpectralChannel;

    friend YSpectralChannel *yFindSpectralChannel(const string& func);
    friend YSpectralChannel *yFirstSpectralChannel(void);

    // Function-specific method for parsing of JSON output and caching result
    virtual int     _parseAttr(YJSONObject *json_val);

    // Constructor is protected, use yFindSpectralChannel factory function to instantiate
    YSpectralChannel(const string& func);
    //--- (end of YSpectralChannel attributes)

public:
    virtual ~YSpectralChannel();
    //--- (YSpectralChannel accessors declaration)

    static const int RAWCOUNT_INVALID = YAPI_INVALID_INT;

    /**
     *
     * @return an integer
     *
     * On failure, throws an exception or returns YSpectralChannel::RAWCOUNT_INVALID.
     */
    int                 get_rawCount(void);

    inline int          rawCount(void)
    { return this->get_rawCount(); }

    /**
     * Retrieves a spectral analysis channel for a given identifier.
     * The identifier can be specified using several formats:
     *
     * - FunctionLogicalName
     * - ModuleSerialNumber.FunctionIdentifier
     * - ModuleSerialNumber.FunctionLogicalName
     * - ModuleLogicalName.FunctionIdentifier
     * - ModuleLogicalName.FunctionLogicalName
     *
     *
     * This function does not require that the spectral analysis channel is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method isOnline() to test if the spectral analysis channel is
     * indeed online at a given time. In case of ambiguity when looking for
     * a spectral analysis channel by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     *
     * If a call to this object's is_online() method returns FALSE although
     * you are certain that the matching device is plugged, make sure that you did
     * call registerHub() at application initialization time.
     *
     * @param func : a string that uniquely characterizes the spectral analysis channel, for instance
     *         MyDevice.spectralChannel1.
     *
     * @return a YSpectralChannel object allowing you to drive the spectral analysis channel.
     */
    static YSpectralChannel* FindSpectralChannel(string func);

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
    virtual int         registerValueCallback(YSpectralChannelValueCallback callback);
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
    virtual int         registerTimedReportCallback(YSpectralChannelTimedReportCallback callback);
    using YSensor::registerTimedReportCallback;

    virtual int         _invokeTimedReportCallback(YMeasure value);


    inline static YSpectralChannel *Find(string func)
    { return YSpectralChannel::FindSpectralChannel(func); }

    /**
     * Continues the enumeration of spectral analysis channels started using yFirstSpectralChannel().
     * Caution: You can't make any assumption about the returned spectral analysis channels order.
     * If you want to find a specific a spectral analysis channel, use SpectralChannel.findSpectralChannel()
     * and a hardwareID or a logical name.
     *
     * @return a pointer to a YSpectralChannel object, corresponding to
     *         a spectral analysis channel currently online, or a NULL pointer
     *         if there are no more spectral analysis channels to enumerate.
     */
           YSpectralChannel *nextSpectralChannel(void);
    inline YSpectralChannel *next(void)
    { return this->nextSpectralChannel();}

    /**
     * Starts the enumeration of spectral analysis channels currently accessible.
     * Use the method YSpectralChannel::nextSpectralChannel() to iterate on
     * next spectral analysis channels.
     *
     * @return a pointer to a YSpectralChannel object, corresponding to
     *         the first spectral analysis channel currently online, or a NULL pointer
     *         if there are none.
     */
           static YSpectralChannel *FirstSpectralChannel(void);
    inline static YSpectralChannel *First(void)
    { return YSpectralChannel::FirstSpectralChannel();}
#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of YSpectralChannel accessors declaration)
};

//--- (YSpectralChannel functions declaration)

/**
 * Retrieves a spectral analysis channel for a given identifier.
 * The identifier can be specified using several formats:
 *
 * - FunctionLogicalName
 * - ModuleSerialNumber.FunctionIdentifier
 * - ModuleSerialNumber.FunctionLogicalName
 * - ModuleLogicalName.FunctionIdentifier
 * - ModuleLogicalName.FunctionLogicalName
 *
 *
 * This function does not require that the spectral analysis channel is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the spectral analysis channel is
 * indeed online at a given time. In case of ambiguity when looking for
 * a spectral analysis channel by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the spectral analysis channel, for instance
 *         MyDevice.spectralChannel1.
 *
 * @return a YSpectralChannel object allowing you to drive the spectral analysis channel.
 */
inline YSpectralChannel *yFindSpectralChannel(const string& func)
{ return YSpectralChannel::FindSpectralChannel(func);}
/**
 * Starts the enumeration of spectral analysis channels currently accessible.
 * Use the method YSpectralChannel::nextSpectralChannel() to iterate on
 * next spectral analysis channels.
 *
 * @return a pointer to a YSpectralChannel object, corresponding to
 *         the first spectral analysis channel currently online, or a NULL pointer
 *         if there are none.
 */
inline YSpectralChannel *yFirstSpectralChannel(void)
{ return YSpectralChannel::FirstSpectralChannel();}

//--- (end of YSpectralChannel functions declaration)

#ifdef YOCTOLIB_NAMESPACE
// end of namespace definition
}
#endif

#endif
