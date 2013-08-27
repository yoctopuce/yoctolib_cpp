/*********************************************************************
 *
 * $Id: yocto_wakeupmonitor.h 12324 2013-08-13 15:10:31Z mvuilleu $
 *
 * Declares yFindWakeUpMonitor(), the high-level API for WakeUpMonitor functions
 *
 * - - - - - - - - - License information: - - - - - - - - - 
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

#include "yocto_api.h"
#include <cfloat>
#include <cmath>
#include <map>

//--- (return codes)
//--- (end of return codes)
//--- (YWakeUpMonitor definitions)
class YWakeUpMonitor; //forward declaration

typedef void (*YWakeUpMonitorUpdateCallback)(YWakeUpMonitor *func, const string& functionValue);
typedef enum {
    Y_WAKEUPREASON_USBPOWER = 0,
    Y_WAKEUPREASON_EXTPOWER = 1,
    Y_WAKEUPREASON_ENDOFSLEEP = 2,
    Y_WAKEUPREASON_EXTSIG1 = 3,
    Y_WAKEUPREASON_EXTSIG2 = 4,
    Y_WAKEUPREASON_EXTSIG3 = 5,
    Y_WAKEUPREASON_EXTSIG4 = 6,
    Y_WAKEUPREASON_SCHEDULE1 = 7,
    Y_WAKEUPREASON_SCHEDULE2 = 8,
    Y_WAKEUPREASON_SCHEDULE3 = 9,
    Y_WAKEUPREASON_SCHEDULE4 = 10,
    Y_WAKEUPREASON_SCHEDULE5 = 11,
    Y_WAKEUPREASON_SCHEDULE6 = 12,
    Y_WAKEUPREASON_INVALID = -1,
} Y_WAKEUPREASON_enum;

typedef enum {
    Y_WAKEUPSTATE_SLEEPING = 0,
    Y_WAKEUPSTATE_AWAKE = 1,
    Y_WAKEUPSTATE_INVALID = -1,
} Y_WAKEUPSTATE_enum;

#define Y_LOGICALNAME_INVALID           (YAPI::INVALID_STRING)
#define Y_ADVERTISEDVALUE_INVALID       (YAPI::INVALID_STRING)
#define Y_POWERDURATION_INVALID         (0x80000000)
#define Y_SLEEPCOUNTDOWN_INVALID        (0x80000000)
#define Y_NEXTWAKEUP_INVALID            (0xffffffff)
#define Y_RTCTIME_INVALID               (0xffffffff)
//--- (end of YWakeUpMonitor definitions)

//--- (YWakeUpMonitor declaration)
/**
 * YWakeUpMonitor Class: WakeUpMonitor function interface
 * 
 * 
 */
class YWakeUpMonitor: public YFunction {
protected:
    // Attributes (function value cache)
    YWakeUpMonitorUpdateCallback _callback;
    string          _logicalName;
    string          _advertisedValue;
    int             _powerDuration;
    int             _sleepCountdown;
    unsigned        _nextWakeUp;
    Y_WAKEUPREASON_enum _wakeUpReason;
    Y_WAKEUPSTATE_enum _wakeUpState;
    unsigned        _rtcTime;
    unsigned        _endOfTime;

    friend YWakeUpMonitor *yFindWakeUpMonitor(const string& func);
    friend YWakeUpMonitor *yFirstWakeUpMonitor(void);

    // Function-specific method for parsing of JSON output and caching result
    int             _parse(yJsonStateMachine& j);
    //--- (end of YWakeUpMonitor declaration)

    //--- (YWakeUpMonitor constructor)
    // Constructor is protected, use yFindWakeUpMonitor factory function to instantiate
    YWakeUpMonitor(const string& func);
    //--- (end of YWakeUpMonitor constructor)
    //--- (WakeUpMonitor initialization)
    //--- (end of WakeUpMonitor initialization)

public:
    ~YWakeUpMonitor();
    //--- (YWakeUpMonitor accessors declaration)

    static const string LOGICALNAME_INVALID;
    static const string ADVERTISEDVALUE_INVALID;
    static const int      POWERDURATION_INVALID = 0x80000000;
    static const int      SLEEPCOUNTDOWN_INVALID = 0x80000000;
    static const unsigned NEXTWAKEUP_INVALID = 0xffffffff;
    static const Y_WAKEUPREASON_enum WAKEUPREASON_USBPOWER = Y_WAKEUPREASON_USBPOWER;
    static const Y_WAKEUPREASON_enum WAKEUPREASON_EXTPOWER = Y_WAKEUPREASON_EXTPOWER;
    static const Y_WAKEUPREASON_enum WAKEUPREASON_ENDOFSLEEP = Y_WAKEUPREASON_ENDOFSLEEP;
    static const Y_WAKEUPREASON_enum WAKEUPREASON_EXTSIG1 = Y_WAKEUPREASON_EXTSIG1;
    static const Y_WAKEUPREASON_enum WAKEUPREASON_EXTSIG2 = Y_WAKEUPREASON_EXTSIG2;
    static const Y_WAKEUPREASON_enum WAKEUPREASON_EXTSIG3 = Y_WAKEUPREASON_EXTSIG3;
    static const Y_WAKEUPREASON_enum WAKEUPREASON_EXTSIG4 = Y_WAKEUPREASON_EXTSIG4;
    static const Y_WAKEUPREASON_enum WAKEUPREASON_SCHEDULE1 = Y_WAKEUPREASON_SCHEDULE1;
    static const Y_WAKEUPREASON_enum WAKEUPREASON_SCHEDULE2 = Y_WAKEUPREASON_SCHEDULE2;
    static const Y_WAKEUPREASON_enum WAKEUPREASON_SCHEDULE3 = Y_WAKEUPREASON_SCHEDULE3;
    static const Y_WAKEUPREASON_enum WAKEUPREASON_SCHEDULE4 = Y_WAKEUPREASON_SCHEDULE4;
    static const Y_WAKEUPREASON_enum WAKEUPREASON_SCHEDULE5 = Y_WAKEUPREASON_SCHEDULE5;
    static const Y_WAKEUPREASON_enum WAKEUPREASON_SCHEDULE6 = Y_WAKEUPREASON_SCHEDULE6;
    static const Y_WAKEUPREASON_enum WAKEUPREASON_INVALID = Y_WAKEUPREASON_INVALID;
    static const Y_WAKEUPSTATE_enum WAKEUPSTATE_SLEEPING = Y_WAKEUPSTATE_SLEEPING;
    static const Y_WAKEUPSTATE_enum WAKEUPSTATE_AWAKE = Y_WAKEUPSTATE_AWAKE;
    static const Y_WAKEUPSTATE_enum WAKEUPSTATE_INVALID = Y_WAKEUPSTATE_INVALID;
    static const unsigned RTCTIME_INVALID = 0xffffffff;

    /**
     * Returns the logical name of the monitor.
     * 
     * @return a string corresponding to the logical name of the monitor
     * 
     * On failure, throws an exception or returns Y_LOGICALNAME_INVALID.
     */
           string          get_logicalName(void);
    inline string          logicalName(void)
    { return this->get_logicalName(); }

    /**
     * Changes the logical name of the monitor. You can use yCheckLogicalName()
     * prior to this call to make sure that your parameter is valid.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     * 
     * @param newval : a string corresponding to the logical name of the monitor
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_logicalName(const string& newval);
    inline int      setLogicalName(const string& newval)
    { return this->set_logicalName(newval); }

    /**
     * Returns the current value of the monitor (no more than 6 characters).
     * 
     * @return a string corresponding to the current value of the monitor (no more than 6 characters)
     * 
     * On failure, throws an exception or returns Y_ADVERTISEDVALUE_INVALID.
     */
           string          get_advertisedValue(void);
    inline string          advertisedValue(void)
    { return this->get_advertisedValue(); }

    /**
     * Returns the maximal wake up time (seconds) before going to sleep automatically.
     * 
     * @return an integer corresponding to the maximal wake up time (seconds) before going to sleep automatically
     * 
     * On failure, throws an exception or returns Y_POWERDURATION_INVALID.
     */
           int             get_powerDuration(void);
    inline int             powerDuration(void)
    { return this->get_powerDuration(); }

    /**
     * Changes the maximal wake up time (seconds) before going to sleep automatically.
     * 
     * @param newval : an integer corresponding to the maximal wake up time (seconds) before going to
     * sleep automatically
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_powerDuration(int newval);
    inline int      setPowerDuration(int newval)
    { return this->set_powerDuration(newval); }

    /**
     * Returns the delay before next sleep.
     * 
     * @return an integer corresponding to the delay before next sleep
     * 
     * On failure, throws an exception or returns Y_SLEEPCOUNTDOWN_INVALID.
     */
           int             get_sleepCountdown(void);
    inline int             sleepCountdown(void)
    { return this->get_sleepCountdown(); }

    /**
     * Changes the delay before next sleep.
     * 
     * @param newval : an integer corresponding to the delay before next sleep
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_sleepCountdown(int newval);
    inline int      setSleepCountdown(int newval)
    { return this->set_sleepCountdown(newval); }

    /**
     * Returns the next scheduled wake-up date/time (UNIX format)
     * 
     * @return an integer corresponding to the next scheduled wake-up date/time (UNIX format)
     * 
     * On failure, throws an exception or returns Y_NEXTWAKEUP_INVALID.
     */
           unsigned        get_nextWakeUp(void);
    inline unsigned        nextWakeUp(void)
    { return this->get_nextWakeUp(); }

    /**
     * Changes the days of the week where a wake up must take place.
     * 
     * @param newval : an integer corresponding to the days of the week where a wake up must take place
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_nextWakeUp(unsigned newval);
    inline int      setNextWakeUp(unsigned newval)
    { return this->set_nextWakeUp(newval); }

    /**
     * Return the last wake up reason.
     * 
     * @return a value among Y_WAKEUPREASON_USBPOWER, Y_WAKEUPREASON_EXTPOWER, Y_WAKEUPREASON_ENDOFSLEEP,
     * Y_WAKEUPREASON_EXTSIG1, Y_WAKEUPREASON_EXTSIG2, Y_WAKEUPREASON_EXTSIG3, Y_WAKEUPREASON_EXTSIG4,
     * Y_WAKEUPREASON_SCHEDULE1, Y_WAKEUPREASON_SCHEDULE2, Y_WAKEUPREASON_SCHEDULE3,
     * Y_WAKEUPREASON_SCHEDULE4, Y_WAKEUPREASON_SCHEDULE5 and Y_WAKEUPREASON_SCHEDULE6
     * 
     * On failure, throws an exception or returns Y_WAKEUPREASON_INVALID.
     */
           Y_WAKEUPREASON_enum get_wakeUpReason(void);
    inline Y_WAKEUPREASON_enum wakeUpReason(void)
    { return this->get_wakeUpReason(); }

    /**
     * Returns  the current state of the monitor
     * 
     * @return either Y_WAKEUPSTATE_SLEEPING or Y_WAKEUPSTATE_AWAKE, according to  the current state of the monitor
     * 
     * On failure, throws an exception or returns Y_WAKEUPSTATE_INVALID.
     */
           Y_WAKEUPSTATE_enum get_wakeUpState(void);
    inline Y_WAKEUPSTATE_enum wakeUpState(void)
    { return this->get_wakeUpState(); }

    int             set_wakeUpState(Y_WAKEUPSTATE_enum newval);
    inline int      setWakeUpState(Y_WAKEUPSTATE_enum newval)
    { return this->set_wakeUpState(newval); }

           unsigned        get_rtcTime(void);
    inline unsigned        rtcTime(void)
    { return this->get_rtcTime(); }

    /**
     * Forces a wakeup.
     */
    int             wakeUp();

    /**
     * Go to sleep until the next wakeup condition is met,  the
     * RTC time must have been set before calling this function.
     * 
     * @param secBeforeSleep : number of seconds before going into sleep mode,
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             sleep(int secBeforeSleep);

    /**
     * Go to sleep for a specific time or until the next wakeup condition is met, the
     * RTC time must have been set before calling this function. The count down before sleep
     * can be canceled with resetSleepCountDown.
     * 
     * @param secUntilWakeUp : sleep duration, in secondes
     * @param secBeforeSleep : number of seconds before going into sleep mode
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             sleepFor(int secUntilWakeUp,int secBeforeSleep);

    /**
     * Go to sleep until a specific date is reached or until the next wakeup condition is met, the
     * RTC time must have been set before calling this function. The count down before sleep
     * can be canceled with resetSleepCountDown.
     * 
     * @param wakeUpTime : wake-up datetime (UNIX format)
     * @param secBeforeSleep : number of seconds before going into sleep mode
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             sleepUntil(int wakeUpTime,int secBeforeSleep);

    /**
     * Reset the sleep countdown.
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     *         On failure, throws an exception or returns a negative error code.
     */
    int             resetSleepCountDown();


    /**
     * Registers the callback function that is invoked on every change of advertised value.
     * The callback is invoked only during the execution of ySleep or yHandleEvents.
     * This provides control over the time when the callback is triggered. For good responsiveness, remember to call
     * one of these two functions periodically. To unregister a callback, pass a null pointer as argument.
     * 
     * @param callback : the callback function to call, or a null pointer. The callback function should take two
     *         arguments: the function object of which the value has changed, and the character string describing
     *         the new advertised value.
     * @noreturn
     */
    void registerValueCallback(YWakeUpMonitorUpdateCallback callback);

    void advertiseValue(const string& value);

    /**
     * Continues the enumeration of monitors started using yFirstWakeUpMonitor().
     * 
     * @return a pointer to a YWakeUpMonitor object, corresponding to
     *         a monitor currently online, or a null pointer
     *         if there are no more monitors to enumerate.
     */
           YWakeUpMonitor  *nextWakeUpMonitor(void);
    inline YWakeUpMonitor  *next(void)
    { return this->nextWakeUpMonitor();}

    /**
     * Retrieves a monitor for a given identifier.
     * The identifier can be specified using several formats:
     * <ul>
     * <li>FunctionLogicalName</li>
     * <li>ModuleSerialNumber.FunctionIdentifier</li>
     * <li>ModuleSerialNumber.FunctionLogicalName</li>
     * <li>ModuleLogicalName.FunctionIdentifier</li>
     * <li>ModuleLogicalName.FunctionLogicalName</li>
     * </ul>
     * 
     * This function does not require that the monitor is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method YWakeUpMonitor.isOnline() to test if the monitor is
     * indeed online at a given time. In case of ambiguity when looking for
     * a monitor by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     * 
     * @param func : a string that uniquely characterizes the monitor
     * 
     * @return a YWakeUpMonitor object allowing you to drive the monitor.
     */
           static YWakeUpMonitor* FindWakeUpMonitor(const string& func);
    inline static YWakeUpMonitor* Find(const string& func)
    { return YWakeUpMonitor::FindWakeUpMonitor(func);}
    /**
     * Starts the enumeration of monitors currently accessible.
     * Use the method YWakeUpMonitor.nextWakeUpMonitor() to iterate on
     * next monitors.
     * 
     * @return a pointer to a YWakeUpMonitor object, corresponding to
     *         the first monitor currently online, or a null pointer
     *         if there are none.
     */
           static YWakeUpMonitor* FirstWakeUpMonitor(void);
    inline static YWakeUpMonitor* First(void)
    { return YWakeUpMonitor::FirstWakeUpMonitor();}
    //--- (end of YWakeUpMonitor accessors declaration)
};

//--- (WakeUpMonitor functions declaration)

/**
 * Retrieves a monitor for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 * 
 * This function does not require that the monitor is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method YWakeUpMonitor.isOnline() to test if the monitor is
 * indeed online at a given time. In case of ambiguity when looking for
 * a monitor by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 * 
 * @param func : a string that uniquely characterizes the monitor
 * 
 * @return a YWakeUpMonitor object allowing you to drive the monitor.
 */
inline YWakeUpMonitor* yFindWakeUpMonitor(const string& func)
{ return YWakeUpMonitor::FindWakeUpMonitor(func);}
/**
 * Starts the enumeration of monitors currently accessible.
 * Use the method YWakeUpMonitor.nextWakeUpMonitor() to iterate on
 * next monitors.
 * 
 * @return a pointer to a YWakeUpMonitor object, corresponding to
 *         the first monitor currently online, or a null pointer
 *         if there are none.
 */
inline YWakeUpMonitor* yFirstWakeUpMonitor(void)
{ return YWakeUpMonitor::FirstWakeUpMonitor();}

//--- (end of WakeUpMonitor functions declaration)

#endif
