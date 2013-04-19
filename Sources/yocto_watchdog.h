/*********************************************************************
 *
 * $Id: yocto_watchdog.h 9921 2013-02-20 09:39:16Z seb $
 *
 * Declares yFindWatchdog(), the high-level API for Watchdog functions
 *
 * - - - - - - - - - License information: - - - - - - - - - 
 *
 * Copyright (C) 2011 and beyond by Yoctopuce Sarl, Switzerland.
 *
 * 1) If you have obtained this file from www.yoctopuce.com,
 *    Yoctopuce Sarl licenses to you (hereafter Licensee) the
 *    right to use, modify, copy, and integrate this source file
 *    into your own solution for the sole purpose of interfacing
 *    a Yoctopuce product with Licensee's solution.
 *
 *    The use of this file and all relationship between Yoctopuce 
 *    and Licensee are governed by Yoctopuce General Terms and 
 *    Conditions.
 *
 *    THE SOFTWARE AND DOCUMENTATION ARE PROVIDED 'AS IS' WITHOUT
 *    WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING 
 *    WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS 
 *    FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO
 *    EVENT SHALL LICENSOR BE LIABLE FOR ANY INCIDENTAL, SPECIAL,
 *    INDIRECT OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, 
 *    COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR 
 *    SERVICES, ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT 
 *    LIMITED TO ANY DEFENSE THEREOF), ANY CLAIMS FOR INDEMNITY OR
 *    CONTRIBUTION, OR OTHER SIMILAR COSTS, WHETHER ASSERTED ON THE
 *    BASIS OF CONTRACT, TORT (INCLUDING NEGLIGENCE), BREACH OF
 *    WARRANTY, OR OTHERWISE.
 *
 * 2) If your intent is not to interface with Yoctopuce products,
 *    you are not entitled to use, read or create any derived
 *    material from this source file.
 *
 *********************************************************************/


#ifndef YOCTO_WATCHDOG_H
#define YOCTO_WATCHDOG_H

#include "yocto_api.h"
#include <cfloat>
#include <cmath>
#include <map>

//--- (return codes)
//--- (end of return codes)
//--- (YWatchdog definitions)
class YWatchdog; //forward declaration

typedef void (*YWatchdogUpdateCallback)(YWatchdog *func, const string& functionValue);
typedef enum {
    Y_STATE_A = 0,
    Y_STATE_B = 1,
    Y_STATE_INVALID = -1,
} Y_STATE_enum;

typedef enum {
    Y_OUTPUT_OFF = 0,
    Y_OUTPUT_ON = 1,
    Y_OUTPUT_INVALID = -1,
} Y_OUTPUT_enum;

#ifndef _CLASS_YDELAYEDPULSE
#define _CLASS_YDELAYEDPULSE
class YDelayedPulse {
public:
    s32             target;
    s32             ms;
    u8              moving;
    YDelayedPulse() {}
};
#endif
extern YDelayedPulse YWATCHDOG_INVALID_DELAYEDPULSE;
typedef enum {
    Y_AUTOSTART_OFF = 0,
    Y_AUTOSTART_ON = 1,
    Y_AUTOSTART_INVALID = -1,
} Y_AUTOSTART_enum;

typedef enum {
    Y_RUNNING_OFF = 0,
    Y_RUNNING_ON = 1,
    Y_RUNNING_INVALID = -1,
} Y_RUNNING_enum;

#define Y_LOGICALNAME_INVALID           (YAPI::INVALID_STRING)
#define Y_ADVERTISEDVALUE_INVALID       (YAPI::INVALID_STRING)
#define Y_PULSETIMER_INVALID            (0xffffffff)
#define Y_DELAYEDPULSETIMER_INVALID     (&YWATCHDOG_INVALID_DELAYEDPULSE)
#define Y_COUNTDOWN_INVALID             (0xffffffff)
#define Y_TRIGGERDELAY_INVALID          (0xffffffff)
#define Y_TRIGGERDURATION_INVALID       (0xffffffff)
//--- (end of YWatchdog definitions)

//--- (YWatchdog declaration)
/**
 * YWatchdog Class: Watchdog function interface
 * 
 * The watchog function works like a relay and can cause a brief power cut
 * to an appliance after a preset delay to force this appliance to
 * reset. The Watchdog must be called from time to time to reset the
 * timer and prevent the appliance reset.
 * The watchdog can be driven direcly with <i>pulse</i> and <i>delayedpulse</i> methods to switch
 * off an appliance for a given duration.
 */
class YWatchdog: public YFunction {
protected:
    // Attributes (function value cache)
    YWatchdogUpdateCallback _callback;
    string          _logicalName;
    string          _advertisedValue;
    Y_STATE_enum    _state;
    Y_OUTPUT_enum   _output;
    unsigned        _pulseTimer;
    YDelayedPulse   _delayedPulseTimer;
    unsigned        _countdown;
    Y_AUTOSTART_enum _autoStart;
    Y_RUNNING_enum  _running;
    unsigned        _triggerDelay;
    unsigned        _triggerDuration;
    // Static function object cache
    static std::map<string,YWatchdog*> _WatchdogCache;

    friend YWatchdog *yFindWatchdog(const string& func);
    friend YWatchdog *yFirstWatchdog(void);

    // Function-specific method for parsing of JSON output and caching result
    int             _parse(yJsonStateMachine& j);
    //--- (end of YWatchdog declaration)

    //--- (YWatchdog constructor)
    // Constructor is protected, use yFindWatchdog factory function to instantiate
    YWatchdog(const string& func): YFunction("Watchdog", func)
    //--- (end of YWatchdog constructor)
    //--- (Watchdog initialization)
            ,_callback(NULL)
            ,_logicalName(Y_LOGICALNAME_INVALID)
            ,_advertisedValue(Y_ADVERTISEDVALUE_INVALID)
            ,_state(Y_STATE_INVALID)
            ,_output(Y_OUTPUT_INVALID)
            ,_pulseTimer(Y_PULSETIMER_INVALID)
            ,_delayedPulseTimer()
            ,_countdown(Y_COUNTDOWN_INVALID)
            ,_autoStart(Y_AUTOSTART_INVALID)
            ,_running(Y_RUNNING_INVALID)
            ,_triggerDelay(Y_TRIGGERDELAY_INVALID)
            ,_triggerDuration(Y_TRIGGERDURATION_INVALID)
    //--- (end of Watchdog initialization)
    {};

public:
    //--- (YWatchdog accessors declaration)

    static const string LOGICALNAME_INVALID;
    static const string ADVERTISEDVALUE_INVALID;
    static const Y_STATE_enum STATE_A = Y_STATE_A;
    static const Y_STATE_enum STATE_B = Y_STATE_B;
    static const Y_STATE_enum STATE_INVALID = Y_STATE_INVALID;
    static const Y_OUTPUT_enum OUTPUT_OFF = Y_OUTPUT_OFF;
    static const Y_OUTPUT_enum OUTPUT_ON = Y_OUTPUT_ON;
    static const Y_OUTPUT_enum OUTPUT_INVALID = Y_OUTPUT_INVALID;
    static const unsigned PULSETIMER_INVALID = 0xffffffff;
    static const unsigned COUNTDOWN_INVALID = 0xffffffff;
    static const Y_AUTOSTART_enum AUTOSTART_OFF = Y_AUTOSTART_OFF;
    static const Y_AUTOSTART_enum AUTOSTART_ON = Y_AUTOSTART_ON;
    static const Y_AUTOSTART_enum AUTOSTART_INVALID = Y_AUTOSTART_INVALID;
    static const Y_RUNNING_enum RUNNING_OFF = Y_RUNNING_OFF;
    static const Y_RUNNING_enum RUNNING_ON = Y_RUNNING_ON;
    static const Y_RUNNING_enum RUNNING_INVALID = Y_RUNNING_INVALID;
    static const unsigned TRIGGERDELAY_INVALID = 0xffffffff;
    static const unsigned TRIGGERDURATION_INVALID = 0xffffffff;

    /**
     * Returns the logical name of the watchdog.
     * 
     * @return a string corresponding to the logical name of the watchdog
     * 
     * On failure, throws an exception or returns Y_LOGICALNAME_INVALID.
     */
           string          get_logicalName(void);
    inline string          logicalName(void)
    { return this->get_logicalName(); }

    /**
     * Changes the logical name of the watchdog. You can use yCheckLogicalName()
     * prior to this call to make sure that your parameter is valid.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     * 
     * @param newval : a string corresponding to the logical name of the watchdog
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_logicalName(const string& newval);
    inline int      setLogicalName(const string& newval)
    { return this->set_logicalName(newval); }

    /**
     * Returns the current value of the watchdog (no more than 6 characters).
     * 
     * @return a string corresponding to the current value of the watchdog (no more than 6 characters)
     * 
     * On failure, throws an exception or returns Y_ADVERTISEDVALUE_INVALID.
     */
           string          get_advertisedValue(void);
    inline string          advertisedValue(void)
    { return this->get_advertisedValue(); }

    /**
     * Returns the state of the watchdog (A for the idle position, B for the active position).
     * 
     * @return either Y_STATE_A or Y_STATE_B, according to the state of the watchdog (A for the idle
     * position, B for the active position)
     * 
     * On failure, throws an exception or returns Y_STATE_INVALID.
     */
           Y_STATE_enum    get_state(void);
    inline Y_STATE_enum    state(void)
    { return this->get_state(); }

    /**
     * Changes the state of the watchdog (A for the idle position, B for the active position).
     * 
     * @param newval : either Y_STATE_A or Y_STATE_B, according to the state of the watchdog (A for the
     * idle position, B for the active position)
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_state(Y_STATE_enum newval);
    inline int      setState(Y_STATE_enum newval)
    { return this->set_state(newval); }

    /**
     * Returns the output state of the watchdog, when used as a simple switch (single throw).
     * 
     * @return either Y_OUTPUT_OFF or Y_OUTPUT_ON, according to the output state of the watchdog, when
     * used as a simple switch (single throw)
     * 
     * On failure, throws an exception or returns Y_OUTPUT_INVALID.
     */
           Y_OUTPUT_enum   get_output(void);
    inline Y_OUTPUT_enum   output(void)
    { return this->get_output(); }

    /**
     * Changes the output state of the watchdog, when used as a simple switch (single throw).
     * 
     * @param newval : either Y_OUTPUT_OFF or Y_OUTPUT_ON, according to the output state of the watchdog,
     * when used as a simple switch (single throw)
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_output(Y_OUTPUT_enum newval);
    inline int      setOutput(Y_OUTPUT_enum newval)
    { return this->set_output(newval); }

    /**
     * Returns the number of milliseconds remaining before the watchdog is returned to idle position
     * (state A), during a measured pulse generation. When there is no ongoing pulse, returns zero.
     * 
     * @return an integer corresponding to the number of milliseconds remaining before the watchdog is
     * returned to idle position
     *         (state A), during a measured pulse generation
     * 
     * On failure, throws an exception or returns Y_PULSETIMER_INVALID.
     */
           unsigned        get_pulseTimer(void);
    inline unsigned        pulseTimer(void)
    { return this->get_pulseTimer(); }

    int             set_pulseTimer(unsigned newval);
    inline int      setPulseTimer(unsigned newval)
    { return this->set_pulseTimer(newval); }

    /**
     * Sets the relay to output B (active) for a specified duration, then brings it
     * automatically back to output A (idle state).
     * 
     * @param ms_duration : pulse duration, in millisecondes
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             pulse(int ms_duration);

           const YDelayedPulse *get_delayedPulseTimer(void);
    inline const YDelayedPulse *delayedPulseTimer(void)
    { return this->get_delayedPulseTimer(); }

    int             set_delayedPulseTimer(const YDelayedPulse * newval);
    inline int      setDelayedPulseTimer(const YDelayedPulse * newval)
    { return this->set_delayedPulseTimer(newval); }

    /**
     * Schedules a pulse.
     * 
     * @param ms_delay : waiting time before the pulse, in millisecondes
     * @param ms_duration : pulse duration, in millisecondes
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             delayedPulse(int ms_delay,int ms_duration);

    /**
     * Returns the number of milliseconds remaining before a pulse (delayedPulse() call)
     * When there is no scheduled pulse, returns zero.
     * 
     * @return an integer corresponding to the number of milliseconds remaining before a pulse (delayedPulse() call)
     *         When there is no scheduled pulse, returns zero
     * 
     * On failure, throws an exception or returns Y_COUNTDOWN_INVALID.
     */
           unsigned        get_countdown(void);
    inline unsigned        countdown(void)
    { return this->get_countdown(); }

    /**
     * Returns the watchdog runing state at module power up.
     * 
     * @return either Y_AUTOSTART_OFF or Y_AUTOSTART_ON, according to the watchdog runing state at module power up
     * 
     * On failure, throws an exception or returns Y_AUTOSTART_INVALID.
     */
           Y_AUTOSTART_enum get_autoStart(void);
    inline Y_AUTOSTART_enum autoStart(void)
    { return this->get_autoStart(); }

    /**
     * Changes the watchdog runningsttae at module power up. Remember to call the
     * saveToFlash() method and then to reboot the module to apply this setting.
     * 
     * @param newval : either Y_AUTOSTART_OFF or Y_AUTOSTART_ON, according to the watchdog runningsttae at
     * module power up
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_autoStart(Y_AUTOSTART_enum newval);
    inline int      setAutoStart(Y_AUTOSTART_enum newval)
    { return this->set_autoStart(newval); }

    /**
     * Returns the watchdog running state.
     * 
     * @return either Y_RUNNING_OFF or Y_RUNNING_ON, according to the watchdog running state
     * 
     * On failure, throws an exception or returns Y_RUNNING_INVALID.
     */
           Y_RUNNING_enum  get_running(void);
    inline Y_RUNNING_enum  running(void)
    { return this->get_running(); }

    /**
     * Changes the running state of the watchdog.
     * 
     * @param newval : either Y_RUNNING_OFF or Y_RUNNING_ON, according to the running state of the watchdog
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_running(Y_RUNNING_enum newval);
    inline int      setRunning(Y_RUNNING_enum newval)
    { return this->set_running(newval); }

    /**
     * Resets the watchdog. When the watchdog is running, this function
     * must be called on a regular basis to prevent the watchog to
     * trigger
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             resetWatchdog(void);

    /**
     * Returns  the waiting duration before a reset is automatically triggered by the watchdog, in milliseconds.
     * 
     * @return an integer corresponding to  the waiting duration before a reset is automatically triggered
     * by the watchdog, in milliseconds
     * 
     * On failure, throws an exception or returns Y_TRIGGERDELAY_INVALID.
     */
           unsigned        get_triggerDelay(void);
    inline unsigned        triggerDelay(void)
    { return this->get_triggerDelay(); }

    /**
     * Changes the waiting delay before a reset is triggered by the watchdog, in milliseconds.
     * 
     * @param newval : an integer corresponding to the waiting delay before a reset is triggered by the
     * watchdog, in milliseconds
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_triggerDelay(unsigned newval);
    inline int      setTriggerDelay(unsigned newval)
    { return this->set_triggerDelay(newval); }

    /**
     * Returns the duration of resets caused by the watchdog, in milliseconds.
     * 
     * @return an integer corresponding to the duration of resets caused by the watchdog, in milliseconds
     * 
     * On failure, throws an exception or returns Y_TRIGGERDURATION_INVALID.
     */
           unsigned        get_triggerDuration(void);
    inline unsigned        triggerDuration(void)
    { return this->get_triggerDuration(); }

    /**
     * Changes the duration of resets caused by the watchdog, in milliseconds.
     * 
     * @param newval : an integer corresponding to the duration of resets caused by the watchdog, in milliseconds
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_triggerDuration(unsigned newval);
    inline int      setTriggerDuration(unsigned newval)
    { return this->set_triggerDuration(newval); }


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
    void registerValueCallback(YWatchdogUpdateCallback callback);

    void advertiseValue(const string& value);

    /**
     * Continues the enumeration of watchdog started using yFirstWatchdog().
     * 
     * @return a pointer to a YWatchdog object, corresponding to
     *         a watchdog currently online, or a null pointer
     *         if there are no more watchdog to enumerate.
     */
           YWatchdog       *nextWatchdog(void);
    inline YWatchdog       *next(void)
    { return this->nextWatchdog();}

    /**
     * Retrieves a watchdog for a given identifier.
     * The identifier can be specified using several formats:
     * <ul>
     * <li>FunctionLogicalName</li>
     * <li>ModuleSerialNumber.FunctionIdentifier</li>
     * <li>ModuleSerialNumber.FunctionLogicalName</li>
     * <li>ModuleLogicalName.FunctionIdentifier</li>
     * <li>ModuleLogicalName.FunctionLogicalName</li>
     * </ul>
     * 
     * This function does not require that the watchdog is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method YWatchdog.isOnline() to test if the watchdog is
     * indeed online at a given time. In case of ambiguity when looking for
     * a watchdog by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     * 
     * @param func : a string that uniquely characterizes the watchdog
     * 
     * @return a YWatchdog object allowing you to drive the watchdog.
     */
           static YWatchdog* FindWatchdog(const string& func);
    inline static YWatchdog* Find(const string& func)
    { return YWatchdog::FindWatchdog(func);}
    /**
     * Starts the enumeration of watchdog currently accessible.
     * Use the method YWatchdog.nextWatchdog() to iterate on
     * next watchdog.
     * 
     * @return a pointer to a YWatchdog object, corresponding to
     *         the first watchdog currently online, or a null pointer
     *         if there are none.
     */
           static YWatchdog* FirstWatchdog(void);
    inline static YWatchdog* First(void)
    { return YWatchdog::FirstWatchdog();}
    //--- (end of YWatchdog accessors declaration)
};

//--- (Watchdog functions declaration)

/**
 * Retrieves a watchdog for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 * 
 * This function does not require that the watchdog is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method YWatchdog.isOnline() to test if the watchdog is
 * indeed online at a given time. In case of ambiguity when looking for
 * a watchdog by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 * 
 * @param func : a string that uniquely characterizes the watchdog
 * 
 * @return a YWatchdog object allowing you to drive the watchdog.
 */
inline YWatchdog* yFindWatchdog(const string& func)
{ return YWatchdog::FindWatchdog(func);}
/**
 * Starts the enumeration of watchdog currently accessible.
 * Use the method YWatchdog.nextWatchdog() to iterate on
 * next watchdog.
 * 
 * @return a pointer to a YWatchdog object, corresponding to
 *         the first watchdog currently online, or a null pointer
 *         if there are none.
 */
inline YWatchdog* yFirstWatchdog(void)
{ return YWatchdog::FirstWatchdog();}

//--- (end of Watchdog functions declaration)

#endif
