/*********************************************************************
 *
 *  $Id: svn_id $
 *
 *  Declares yFindWakeUpMonitor(), the high-level API for WakeUpMonitor functions
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


#ifndef YOCTO_WAKEUPMONITOR_H
#define YOCTO_WAKEUPMONITOR_H

#include <cfloat>
#include <cmath>

#include "yocto_api.h"

#ifdef YOCTOLIB_NAMESPACE
namespace YOCTOLIB_NAMESPACE
{
#endif

//--- (YWakeUpMonitor return codes)
//--- (end of YWakeUpMonitor return codes)
//--- (YWakeUpMonitor yapiwrapper)
//--- (end of YWakeUpMonitor yapiwrapper)
//--- (YWakeUpMonitor definitions)
class YWakeUpMonitor; // forward declaration

typedef void (*YWakeUpMonitorValueCallback)(YWakeUpMonitor *func, const string& functionValue);
#ifndef _Y_WAKEUPREASON_ENUM
#define _Y_WAKEUPREASON_ENUM
typedef enum {
    Y_WAKEUPREASON_USBPOWER = 0,
    Y_WAKEUPREASON_EXTPOWER = 1,
    Y_WAKEUPREASON_ENDOFSLEEP = 2,
    Y_WAKEUPREASON_EXTSIG1 = 3,
    Y_WAKEUPREASON_SCHEDULE1 = 4,
    Y_WAKEUPREASON_SCHEDULE2 = 5,
    Y_WAKEUPREASON_SCHEDULE3 = 6,
    Y_WAKEUPREASON_INVALID = -1,
} Y_WAKEUPREASON_enum;
#endif
#ifndef _Y_WAKEUPSTATE_ENUM
#define _Y_WAKEUPSTATE_ENUM
typedef enum {
    Y_WAKEUPSTATE_SLEEPING = 0,
    Y_WAKEUPSTATE_AWAKE = 1,
    Y_WAKEUPSTATE_INVALID = -1,
} Y_WAKEUPSTATE_enum;
#endif
#define Y_POWERDURATION_INVALID         (YAPI_INVALID_UINT)
#define Y_SLEEPCOUNTDOWN_INVALID        (YAPI_INVALID_UINT)
#define Y_NEXTWAKEUP_INVALID            (YAPI_INVALID_LONG)
#define Y_RTCTIME_INVALID               (YAPI_INVALID_LONG)
//--- (end of YWakeUpMonitor definitions)

//--- (YWakeUpMonitor declaration)
/**
 * YWakeUpMonitor Class: wake-up monitor control interface, available for instance in the
 * YoctoHub-GSM-4G, the YoctoHub-Wireless-SR, the YoctoHub-Wireless-g or the YoctoHub-Wireless-n
 *
 * The YWakeUpMonitor class handles globally all wake-up sources, as well
 * as automated sleep mode.
 */
class YOCTO_CLASS_EXPORT YWakeUpMonitor: public YFunction {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of YWakeUpMonitor declaration)
protected:
    //--- (YWakeUpMonitor attributes)
    // Attributes (function value cache)
    int             _powerDuration;
    int             _sleepCountdown;
    s64             _nextWakeUp;
    Y_WAKEUPREASON_enum _wakeUpReason;
    Y_WAKEUPSTATE_enum _wakeUpState;
    s64             _rtcTime;
    int             _endOfTime;
    YWakeUpMonitorValueCallback _valueCallbackWakeUpMonitor;

    friend YWakeUpMonitor *yFindWakeUpMonitor(const string& func);
    friend YWakeUpMonitor *yFirstWakeUpMonitor(void);

    // Function-specific method for parsing of JSON output and caching result
    virtual int     _parseAttr(YJSONObject *json_val);

    // Constructor is protected, use yFindWakeUpMonitor factory function to instantiate
    YWakeUpMonitor(const string& func);
    //--- (end of YWakeUpMonitor attributes)

public:
    virtual ~YWakeUpMonitor();
    //--- (YWakeUpMonitor accessors declaration)

    static const int POWERDURATION_INVALID = YAPI_INVALID_UINT;
    static const int SLEEPCOUNTDOWN_INVALID = YAPI_INVALID_UINT;
    static const s64 NEXTWAKEUP_INVALID = YAPI_INVALID_LONG;
    static const Y_WAKEUPREASON_enum WAKEUPREASON_USBPOWER = Y_WAKEUPREASON_USBPOWER;
    static const Y_WAKEUPREASON_enum WAKEUPREASON_EXTPOWER = Y_WAKEUPREASON_EXTPOWER;
    static const Y_WAKEUPREASON_enum WAKEUPREASON_ENDOFSLEEP = Y_WAKEUPREASON_ENDOFSLEEP;
    static const Y_WAKEUPREASON_enum WAKEUPREASON_EXTSIG1 = Y_WAKEUPREASON_EXTSIG1;
    static const Y_WAKEUPREASON_enum WAKEUPREASON_SCHEDULE1 = Y_WAKEUPREASON_SCHEDULE1;
    static const Y_WAKEUPREASON_enum WAKEUPREASON_SCHEDULE2 = Y_WAKEUPREASON_SCHEDULE2;
    static const Y_WAKEUPREASON_enum WAKEUPREASON_SCHEDULE3 = Y_WAKEUPREASON_SCHEDULE3;
    static const Y_WAKEUPREASON_enum WAKEUPREASON_INVALID = Y_WAKEUPREASON_INVALID;
    static const Y_WAKEUPSTATE_enum WAKEUPSTATE_SLEEPING = Y_WAKEUPSTATE_SLEEPING;
    static const Y_WAKEUPSTATE_enum WAKEUPSTATE_AWAKE = Y_WAKEUPSTATE_AWAKE;
    static const Y_WAKEUPSTATE_enum WAKEUPSTATE_INVALID = Y_WAKEUPSTATE_INVALID;
    static const s64 RTCTIME_INVALID = YAPI_INVALID_LONG;

    /**
     * Returns the maximal wake up time (in seconds) before automatically going to sleep.
     *
     * @return an integer corresponding to the maximal wake up time (in seconds) before automatically going to sleep
     *
     * On failure, throws an exception or returns YWakeUpMonitor::POWERDURATION_INVALID.
     */
    int                 get_powerDuration(void);

    inline int          powerDuration(void)
    { return this->get_powerDuration(); }

    /**
     * Changes the maximal wake up time (seconds) before automatically going to sleep.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     *
     * @param newval : an integer corresponding to the maximal wake up time (seconds) before automatically
     * going to sleep
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_powerDuration(int newval);
    inline int      setPowerDuration(int newval)
    { return this->set_powerDuration(newval); }

    /**
     * Returns the delay before the  next sleep period.
     *
     * @return an integer corresponding to the delay before the  next sleep period
     *
     * On failure, throws an exception or returns YWakeUpMonitor::SLEEPCOUNTDOWN_INVALID.
     */
    int                 get_sleepCountdown(void);

    inline int          sleepCountdown(void)
    { return this->get_sleepCountdown(); }

    /**
     * Changes the delay before the next sleep period.
     *
     * @param newval : an integer corresponding to the delay before the next sleep period
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_sleepCountdown(int newval);
    inline int      setSleepCountdown(int newval)
    { return this->set_sleepCountdown(newval); }

    /**
     * Returns the next scheduled wake up date/time (UNIX format).
     *
     * @return an integer corresponding to the next scheduled wake up date/time (UNIX format)
     *
     * On failure, throws an exception or returns YWakeUpMonitor::NEXTWAKEUP_INVALID.
     */
    s64                 get_nextWakeUp(void);

    inline s64          nextWakeUp(void)
    { return this->get_nextWakeUp(); }

    /**
     * Changes the days of the week when a wake up must take place.
     *
     * @param newval : an integer corresponding to the days of the week when a wake up must take place
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_nextWakeUp(s64 newval);
    inline int      setNextWakeUp(s64 newval)
    { return this->set_nextWakeUp(newval); }

    /**
     * Returns the latest wake up reason.
     *
     * @return a value among YWakeUpMonitor::WAKEUPREASON_USBPOWER, YWakeUpMonitor::WAKEUPREASON_EXTPOWER,
     * YWakeUpMonitor::WAKEUPREASON_ENDOFSLEEP, YWakeUpMonitor::WAKEUPREASON_EXTSIG1,
     * YWakeUpMonitor::WAKEUPREASON_SCHEDULE1, YWakeUpMonitor::WAKEUPREASON_SCHEDULE2 and
     * YWakeUpMonitor::WAKEUPREASON_SCHEDULE3 corresponding to the latest wake up reason
     *
     * On failure, throws an exception or returns YWakeUpMonitor::WAKEUPREASON_INVALID.
     */
    Y_WAKEUPREASON_enum get_wakeUpReason(void);

    inline Y_WAKEUPREASON_enum wakeUpReason(void)
    { return this->get_wakeUpReason(); }

    /**
     * Returns  the current state of the monitor.
     *
     * @return either YWakeUpMonitor::WAKEUPSTATE_SLEEPING or YWakeUpMonitor::WAKEUPSTATE_AWAKE, according
     * to  the current state of the monitor
     *
     * On failure, throws an exception or returns YWakeUpMonitor::WAKEUPSTATE_INVALID.
     */
    Y_WAKEUPSTATE_enum  get_wakeUpState(void);

    inline Y_WAKEUPSTATE_enum wakeUpState(void)
    { return this->get_wakeUpState(); }

    int             set_wakeUpState(Y_WAKEUPSTATE_enum newval);
    inline int      setWakeUpState(Y_WAKEUPSTATE_enum newval)
    { return this->set_wakeUpState(newval); }

    s64                 get_rtcTime(void);

    inline s64          rtcTime(void)
    { return this->get_rtcTime(); }

    /**
     * Retrieves a wake-up monitor for a given identifier.
     * The identifier can be specified using several formats:
     *
     * - FunctionLogicalName
     * - ModuleSerialNumber.FunctionIdentifier
     * - ModuleSerialNumber.FunctionLogicalName
     * - ModuleLogicalName.FunctionIdentifier
     * - ModuleLogicalName.FunctionLogicalName
     *
     *
     * This function does not require that the wake-up monitor is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method isOnline() to test if the wake-up monitor is
     * indeed online at a given time. In case of ambiguity when looking for
     * a wake-up monitor by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     *
     * If a call to this object's is_online() method returns FALSE although
     * you are certain that the matching device is plugged, make sure that you did
     * call registerHub() at application initialization time.
     *
     * @param func : a string that uniquely characterizes the wake-up monitor, for instance
     *         YHUBGSM5.wakeUpMonitor.
     *
     * @return a YWakeUpMonitor object allowing you to drive the wake-up monitor.
     */
    static YWakeUpMonitor* FindWakeUpMonitor(string func);

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
    virtual int         registerValueCallback(YWakeUpMonitorValueCallback callback);
    using YFunction::registerValueCallback;

    virtual int         _invokeValueCallback(string value);

    /**
     * Forces a wake up.
     */
    virtual int         wakeUp(void);

    /**
     * Goes to sleep until the next wake up condition is met,  the
     * RTC time must have been set before calling this function.
     *
     * @param secBeforeSleep : number of seconds before going into sleep mode,
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         sleep(int secBeforeSleep);

    /**
     * Goes to sleep for a specific duration or until the next wake up condition is met, the
     * RTC time must have been set before calling this function. The count down before sleep
     * can be canceled with resetSleepCountDown.
     *
     * @param secUntilWakeUp : number of seconds before next wake up
     * @param secBeforeSleep : number of seconds before going into sleep mode
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         sleepFor(int secUntilWakeUp,int secBeforeSleep);

    /**
     * Go to sleep until a specific date is reached or until the next wake up condition is met, the
     * RTC time must have been set before calling this function. The count down before sleep
     * can be canceled with resetSleepCountDown.
     *
     * @param wakeUpTime : wake-up datetime (UNIX format)
     * @param secBeforeSleep : number of seconds before going into sleep mode
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         sleepUntil(int wakeUpTime,int secBeforeSleep);

    /**
     * Resets the sleep countdown.
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *         On failure, throws an exception or returns a negative error code.
     */
    virtual int         resetSleepCountDown(void);


    inline static YWakeUpMonitor *Find(string func)
    { return YWakeUpMonitor::FindWakeUpMonitor(func); }

    /**
     * Continues the enumeration of wake-up monitors started using yFirstWakeUpMonitor().
     * Caution: You can't make any assumption about the returned wake-up monitors order.
     * If you want to find a specific a wake-up monitor, use WakeUpMonitor.findWakeUpMonitor()
     * and a hardwareID or a logical name.
     *
     * @return a pointer to a YWakeUpMonitor object, corresponding to
     *         a wake-up monitor currently online, or a NULL pointer
     *         if there are no more wake-up monitors to enumerate.
     */
           YWakeUpMonitor  *nextWakeUpMonitor(void);
    inline YWakeUpMonitor  *next(void)
    { return this->nextWakeUpMonitor();}

    /**
     * Starts the enumeration of wake-up monitors currently accessible.
     * Use the method YWakeUpMonitor::nextWakeUpMonitor() to iterate on
     * next wake-up monitors.
     *
     * @return a pointer to a YWakeUpMonitor object, corresponding to
     *         the first wake-up monitor currently online, or a NULL pointer
     *         if there are none.
     */
           static YWakeUpMonitor *FirstWakeUpMonitor(void);
    inline static YWakeUpMonitor *First(void)
    { return YWakeUpMonitor::FirstWakeUpMonitor();}
#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of YWakeUpMonitor accessors declaration)
};

//--- (YWakeUpMonitor functions declaration)

/**
 * Retrieves a wake-up monitor for a given identifier.
 * The identifier can be specified using several formats:
 *
 * - FunctionLogicalName
 * - ModuleSerialNumber.FunctionIdentifier
 * - ModuleSerialNumber.FunctionLogicalName
 * - ModuleLogicalName.FunctionIdentifier
 * - ModuleLogicalName.FunctionLogicalName
 *
 *
 * This function does not require that the wake-up monitor is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the wake-up monitor is
 * indeed online at a given time. In case of ambiguity when looking for
 * a wake-up monitor by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the wake-up monitor, for instance
 *         YHUBGSM5.wakeUpMonitor.
 *
 * @return a YWakeUpMonitor object allowing you to drive the wake-up monitor.
 */
inline YWakeUpMonitor *yFindWakeUpMonitor(const string& func)
{ return YWakeUpMonitor::FindWakeUpMonitor(func);}
/**
 * Starts the enumeration of wake-up monitors currently accessible.
 * Use the method YWakeUpMonitor::nextWakeUpMonitor() to iterate on
 * next wake-up monitors.
 *
 * @return a pointer to a YWakeUpMonitor object, corresponding to
 *         the first wake-up monitor currently online, or a NULL pointer
 *         if there are none.
 */
inline YWakeUpMonitor *yFirstWakeUpMonitor(void)
{ return YWakeUpMonitor::FirstWakeUpMonitor();}

//--- (end of YWakeUpMonitor functions declaration)

#ifdef YOCTOLIB_NAMESPACE
// end of namespace definition
}
#endif

#endif
