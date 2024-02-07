/*********************************************************************
 *
 *  $Id: yocto_watchdog.h 44567 2021-04-14 08:57:09Z seb $
 *
 *  Declares yFindWatchdog(), the high-level API for Watchdog functions
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


#ifndef YOCTO_WATCHDOG_H
#define YOCTO_WATCHDOG_H

#include <cfloat>
#include <cmath>

#include "yocto_api.h"

#ifdef YOCTOLIB_NAMESPACE
namespace YOCTOLIB_NAMESPACE
{
#endif

//--- (YWatchdog return codes)
//--- (end of YWatchdog return codes)
//--- (YWatchdog yapiwrapper)
//--- (end of YWatchdog yapiwrapper)
//--- (YWatchdog definitions)
class YWatchdog; // forward declaration

typedef void (*YWatchdogValueCallback)(YWatchdog *func, const string& functionValue);
#ifndef _Y_STATE_ENUM
#define _Y_STATE_ENUM
typedef enum {
    Y_STATE_A = 0,
    Y_STATE_B = 1,
    Y_STATE_INVALID = -1,
} Y_STATE_enum;
#endif
#ifndef _Y_STATEATPOWERON_ENUM
#define _Y_STATEATPOWERON_ENUM
typedef enum {
    Y_STATEATPOWERON_UNCHANGED = 0,
    Y_STATEATPOWERON_A = 1,
    Y_STATEATPOWERON_B = 2,
    Y_STATEATPOWERON_INVALID = -1,
} Y_STATEATPOWERON_enum;
#endif
#ifndef _Y_OUTPUT_ENUM
#define _Y_OUTPUT_ENUM
typedef enum {
    Y_OUTPUT_OFF = 0,
    Y_OUTPUT_ON = 1,
    Y_OUTPUT_INVALID = -1,
} Y_OUTPUT_enum;
#endif
#ifndef _CLASS_YDELAYEDPULSE
#define _CLASS_YDELAYEDPULSE
class YOCTO_CLASS_EXPORT YDelayedPulse {
public:
    int             target;
    int             ms;
    int             moving;

    YDelayedPulse()
        :target(YAPI_INVALID_INT), ms(YAPI_INVALID_INT), moving(YAPI_INVALID_UINT)
    {}

    bool operator==(const YDelayedPulse& o) const {
         return (target == o.target) && (ms == o.ms) && (moving == o.moving);
    }
};
#endif
#ifndef _Y_AUTOSTART_ENUM
#define _Y_AUTOSTART_ENUM
typedef enum {
    Y_AUTOSTART_OFF = 0,
    Y_AUTOSTART_ON = 1,
    Y_AUTOSTART_INVALID = -1,
} Y_AUTOSTART_enum;
#endif
#ifndef _Y_RUNNING_ENUM
#define _Y_RUNNING_ENUM
typedef enum {
    Y_RUNNING_OFF = 0,
    Y_RUNNING_ON = 1,
    Y_RUNNING_INVALID = -1,
} Y_RUNNING_enum;
#endif
#define Y_MAXTIMEONSTATEA_INVALID       (YAPI_INVALID_LONG)
#define Y_MAXTIMEONSTATEB_INVALID       (YAPI_INVALID_LONG)
#define Y_PULSETIMER_INVALID            (YAPI_INVALID_LONG)
#define Y_COUNTDOWN_INVALID             (YAPI_INVALID_LONG)
#define Y_TRIGGERDELAY_INVALID          (YAPI_INVALID_LONG)
#define Y_TRIGGERDURATION_INVALID       (YAPI_INVALID_LONG)
#define Y_LASTTRIGGER_INVALID           (YAPI_INVALID_UINT)
//--- (end of YWatchdog definitions)

//--- (YWatchdog declaration)
/**
 * YWatchdog Class: watchdog control interface, available for instance in the Yocto-WatchdogDC
 *
 * The YWatchdog class allows you to drive a Yoctopuce watchdog.
 * A watchdog works like a relay, with an extra timer that can automatically
 * trigger a brief power cycle to an appliance after a preset delay, to force this
 * appliance to reset if a problem occurs. During normal use, the watchdog timer
 * is reset periodically by the application to prevent the automated power cycle.
 * Whenever the application dies, the watchdog will automatically trigger the power cycle.
 * The watchdog can also be driven directly with pulse and delayedPulse
 * methods to switch off an appliance for a given duration.
 */
class YOCTO_CLASS_EXPORT YWatchdog: public YFunction {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of YWatchdog declaration)
protected:
    //--- (YWatchdog attributes)
    // Attributes (function value cache)
    Y_STATE_enum    _state;
    Y_STATEATPOWERON_enum _stateAtPowerOn;
    s64             _maxTimeOnStateA;
    s64             _maxTimeOnStateB;
    Y_OUTPUT_enum   _output;
    s64             _pulseTimer;
    YDelayedPulse   _delayedPulseTimer;
    s64             _countdown;
    Y_AUTOSTART_enum _autoStart;
    Y_RUNNING_enum  _running;
    s64             _triggerDelay;
    s64             _triggerDuration;
    int             _lastTrigger;
    YWatchdogValueCallback _valueCallbackWatchdog;
    int             _firm;

    friend YWatchdog *yFindWatchdog(const string& func);
    friend YWatchdog *yFirstWatchdog(void);

    // Function-specific method for parsing of JSON output and caching result
    virtual int     _parseAttr(YJSONObject *json_val);

    // Constructor is protected, use yFindWatchdog factory function to instantiate
    YWatchdog(const string& func);
    //--- (end of YWatchdog attributes)

public:
    virtual ~YWatchdog();
    //--- (YWatchdog accessors declaration)

    static const Y_STATE_enum STATE_A = Y_STATE_A;
    static const Y_STATE_enum STATE_B = Y_STATE_B;
    static const Y_STATE_enum STATE_INVALID = Y_STATE_INVALID;
    static const Y_STATEATPOWERON_enum STATEATPOWERON_UNCHANGED = Y_STATEATPOWERON_UNCHANGED;
    static const Y_STATEATPOWERON_enum STATEATPOWERON_A = Y_STATEATPOWERON_A;
    static const Y_STATEATPOWERON_enum STATEATPOWERON_B = Y_STATEATPOWERON_B;
    static const Y_STATEATPOWERON_enum STATEATPOWERON_INVALID = Y_STATEATPOWERON_INVALID;
    static const s64 MAXTIMEONSTATEA_INVALID = YAPI_INVALID_LONG;
    static const s64 MAXTIMEONSTATEB_INVALID = YAPI_INVALID_LONG;
    static const Y_OUTPUT_enum OUTPUT_OFF = Y_OUTPUT_OFF;
    static const Y_OUTPUT_enum OUTPUT_ON = Y_OUTPUT_ON;
    static const Y_OUTPUT_enum OUTPUT_INVALID = Y_OUTPUT_INVALID;
    static const s64 PULSETIMER_INVALID = YAPI_INVALID_LONG;
    static const YDelayedPulse DELAYEDPULSETIMER_INVALID;
    static const s64 COUNTDOWN_INVALID = YAPI_INVALID_LONG;
    static const Y_AUTOSTART_enum AUTOSTART_OFF = Y_AUTOSTART_OFF;
    static const Y_AUTOSTART_enum AUTOSTART_ON = Y_AUTOSTART_ON;
    static const Y_AUTOSTART_enum AUTOSTART_INVALID = Y_AUTOSTART_INVALID;
    static const Y_RUNNING_enum RUNNING_OFF = Y_RUNNING_OFF;
    static const Y_RUNNING_enum RUNNING_ON = Y_RUNNING_ON;
    static const Y_RUNNING_enum RUNNING_INVALID = Y_RUNNING_INVALID;
    static const s64 TRIGGERDELAY_INVALID = YAPI_INVALID_LONG;
    static const s64 TRIGGERDURATION_INVALID = YAPI_INVALID_LONG;
    static const int LASTTRIGGER_INVALID = YAPI_INVALID_UINT;

    /**
     * Returns the state of the watchdog (A for the idle position, B for the active position).
     *
     * @return either YWatchdog::STATE_A or YWatchdog::STATE_B, according to the state of the watchdog (A
     * for the idle position, B for the active position)
     *
     * On failure, throws an exception or returns YWatchdog::STATE_INVALID.
     */
    Y_STATE_enum        get_state(void);

    inline Y_STATE_enum state(void)
    { return this->get_state(); }

    /**
     * Changes the state of the watchdog (A for the idle position, B for the active position).
     *
     * @param newval : either YWatchdog::STATE_A or YWatchdog::STATE_B, according to the state of the
     * watchdog (A for the idle position, B for the active position)
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_state(Y_STATE_enum newval);
    inline int      setState(Y_STATE_enum newval)
    { return this->set_state(newval); }

    /**
     * Returns the state of the watchdog at device startup (A for the idle position,
     * B for the active position, UNCHANGED to leave the relay state as is).
     *
     * @return a value among YWatchdog::STATEATPOWERON_UNCHANGED, YWatchdog::STATEATPOWERON_A and
     * YWatchdog::STATEATPOWERON_B corresponding to the state of the watchdog at device startup (A for the
     * idle position,
     *         B for the active position, UNCHANGED to leave the relay state as is)
     *
     * On failure, throws an exception or returns YWatchdog::STATEATPOWERON_INVALID.
     */
    Y_STATEATPOWERON_enum get_stateAtPowerOn(void);

    inline Y_STATEATPOWERON_enum stateAtPowerOn(void)
    { return this->get_stateAtPowerOn(); }

    /**
     * Changes the state of the watchdog at device startup (A for the idle position,
     * B for the active position, UNCHANGED to leave the relay state as is).
     * Remember to call the matching module saveToFlash()
     * method, otherwise this call will have no effect.
     *
     * @param newval : a value among YWatchdog::STATEATPOWERON_UNCHANGED, YWatchdog::STATEATPOWERON_A and
     * YWatchdog::STATEATPOWERON_B corresponding to the state of the watchdog at device startup (A for the
     * idle position,
     *         B for the active position, UNCHANGED to leave the relay state as is)
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_stateAtPowerOn(Y_STATEATPOWERON_enum newval);
    inline int      setStateAtPowerOn(Y_STATEATPOWERON_enum newval)
    { return this->set_stateAtPowerOn(newval); }

    /**
     * Returns the maximum time (ms) allowed for the watchdog to stay in state
     * A before automatically switching back in to B state. Zero means no time limit.
     *
     * @return an integer corresponding to the maximum time (ms) allowed for the watchdog to stay in state
     *         A before automatically switching back in to B state
     *
     * On failure, throws an exception or returns YWatchdog::MAXTIMEONSTATEA_INVALID.
     */
    s64                 get_maxTimeOnStateA(void);

    inline s64          maxTimeOnStateA(void)
    { return this->get_maxTimeOnStateA(); }

    /**
     * Changes the maximum time (ms) allowed for the watchdog to stay in state A
     * before automatically switching back in to B state. Use zero for no time limit.
     * Remember to call the saveToFlash()
     * method of the module if the modification must be kept.
     *
     * @param newval : an integer corresponding to the maximum time (ms) allowed for the watchdog to stay in state A
     *         before automatically switching back in to B state
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_maxTimeOnStateA(s64 newval);
    inline int      setMaxTimeOnStateA(s64 newval)
    { return this->set_maxTimeOnStateA(newval); }

    /**
     * Retourne the maximum time (ms) allowed for the watchdog to stay in state B
     * before automatically switching back in to A state. Zero means no time limit.
     *
     * @return an integer
     *
     * On failure, throws an exception or returns YWatchdog::MAXTIMEONSTATEB_INVALID.
     */
    s64                 get_maxTimeOnStateB(void);

    inline s64          maxTimeOnStateB(void)
    { return this->get_maxTimeOnStateB(); }

    /**
     * Changes the maximum time (ms) allowed for the watchdog to stay in state B before
     * automatically switching back in to A state. Use zero for no time limit.
     * Remember to call the saveToFlash()
     * method of the module if the modification must be kept.
     *
     * @param newval : an integer corresponding to the maximum time (ms) allowed for the watchdog to stay
     * in state B before
     *         automatically switching back in to A state
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_maxTimeOnStateB(s64 newval);
    inline int      setMaxTimeOnStateB(s64 newval)
    { return this->set_maxTimeOnStateB(newval); }

    /**
     * Returns the output state of the watchdog, when used as a simple switch (single throw).
     *
     * @return either YWatchdog::OUTPUT_OFF or YWatchdog::OUTPUT_ON, according to the output state of the
     * watchdog, when used as a simple switch (single throw)
     *
     * On failure, throws an exception or returns YWatchdog::OUTPUT_INVALID.
     */
    Y_OUTPUT_enum       get_output(void);

    inline Y_OUTPUT_enum output(void)
    { return this->get_output(); }

    /**
     * Changes the output state of the watchdog, when used as a simple switch (single throw).
     *
     * @param newval : either YWatchdog::OUTPUT_OFF or YWatchdog::OUTPUT_ON, according to the output state
     * of the watchdog, when used as a simple switch (single throw)
     *
     * @return YAPI::SUCCESS if the call succeeds.
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
     * On failure, throws an exception or returns YWatchdog::PULSETIMER_INVALID.
     */
    s64                 get_pulseTimer(void);

    inline s64          pulseTimer(void)
    { return this->get_pulseTimer(); }

    int             set_pulseTimer(s64 newval);
    inline int      setPulseTimer(s64 newval)
    { return this->set_pulseTimer(newval); }

    /**
     * Sets the relay to output B (active) for a specified duration, then brings it
     * automatically back to output A (idle state).
     *
     * @param ms_duration : pulse duration, in milliseconds
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             pulse(int ms_duration);

    YDelayedPulse       get_delayedPulseTimer(void);

    inline YDelayedPulse delayedPulseTimer(void)
    { return this->get_delayedPulseTimer(); }

    int             set_delayedPulseTimer(YDelayedPulse newval);
    inline int      setDelayedPulseTimer(YDelayedPulse newval)
    { return this->set_delayedPulseTimer(newval); }

    /**
     * Schedules a pulse.
     *
     * @param ms_delay : waiting time before the pulse, in milliseconds
     * @param ms_duration : pulse duration, in milliseconds
     *
     * @return YAPI::SUCCESS if the call succeeds.
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
     * On failure, throws an exception or returns YWatchdog::COUNTDOWN_INVALID.
     */
    s64                 get_countdown(void);

    inline s64          countdown(void)
    { return this->get_countdown(); }

    /**
     * Returns the watchdog running state at module power on.
     *
     * @return either YWatchdog::AUTOSTART_OFF or YWatchdog::AUTOSTART_ON, according to the watchdog running
     * state at module power on
     *
     * On failure, throws an exception or returns YWatchdog::AUTOSTART_INVALID.
     */
    Y_AUTOSTART_enum    get_autoStart(void);

    inline Y_AUTOSTART_enum autoStart(void)
    { return this->get_autoStart(); }

    /**
     * Changes the watchdog running state at module power on. Remember to call the
     * saveToFlash() method and then to reboot the module to apply this setting.
     *
     * @param newval : either YWatchdog::AUTOSTART_OFF or YWatchdog::AUTOSTART_ON, according to the watchdog
     * running state at module power on
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_autoStart(Y_AUTOSTART_enum newval);
    inline int      setAutoStart(Y_AUTOSTART_enum newval)
    { return this->set_autoStart(newval); }

    /**
     * Returns the watchdog running state.
     *
     * @return either YWatchdog::RUNNING_OFF or YWatchdog::RUNNING_ON, according to the watchdog running state
     *
     * On failure, throws an exception or returns YWatchdog::RUNNING_INVALID.
     */
    Y_RUNNING_enum      get_running(void);

    inline Y_RUNNING_enum running(void)
    { return this->get_running(); }

    /**
     * Changes the running state of the watchdog.
     *
     * @param newval : either YWatchdog::RUNNING_OFF or YWatchdog::RUNNING_ON, according to the running
     * state of the watchdog
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_running(Y_RUNNING_enum newval);
    inline int      setRunning(Y_RUNNING_enum newval)
    { return this->set_running(newval); }

    /**
     * Resets the watchdog. When the watchdog is running, this function
     * must be called on a regular basis to prevent the watchdog to
     * trigger
     *
     * @return YAPI::SUCCESS if the call succeeds.
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
     * On failure, throws an exception or returns YWatchdog::TRIGGERDELAY_INVALID.
     */
    s64                 get_triggerDelay(void);

    inline s64          triggerDelay(void)
    { return this->get_triggerDelay(); }

    /**
     * Changes the waiting delay before a reset is triggered by the watchdog,
     * in milliseconds. Remember to call the saveToFlash()
     * method of the module if the modification must be kept.
     *
     * @param newval : an integer corresponding to the waiting delay before a reset is triggered by the watchdog,
     *         in milliseconds
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_triggerDelay(s64 newval);
    inline int      setTriggerDelay(s64 newval)
    { return this->set_triggerDelay(newval); }

    /**
     * Returns the duration of resets caused by the watchdog, in milliseconds.
     *
     * @return an integer corresponding to the duration of resets caused by the watchdog, in milliseconds
     *
     * On failure, throws an exception or returns YWatchdog::TRIGGERDURATION_INVALID.
     */
    s64                 get_triggerDuration(void);

    inline s64          triggerDuration(void)
    { return this->get_triggerDuration(); }

    /**
     * Changes the duration of resets caused by the watchdog, in milliseconds.
     * Remember to call the saveToFlash()
     * method of the module if the modification must be kept.
     *
     * @param newval : an integer corresponding to the duration of resets caused by the watchdog, in milliseconds
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_triggerDuration(s64 newval);
    inline int      setTriggerDuration(s64 newval)
    { return this->set_triggerDuration(newval); }

    /**
     * Returns the number of seconds spent since the last output power-up event.
     *
     * @return an integer corresponding to the number of seconds spent since the last output power-up event
     *
     * On failure, throws an exception or returns YWatchdog::LASTTRIGGER_INVALID.
     */
    int                 get_lastTrigger(void);

    inline int          lastTrigger(void)
    { return this->get_lastTrigger(); }

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
     * Use the method isOnline() to test if the watchdog is
     * indeed online at a given time. In case of ambiguity when looking for
     * a watchdog by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     *
     * If a call to this object's is_online() method returns FALSE although
     * you are certain that the matching device is plugged, make sure that you did
     * call registerHub() at application initialization time.
     *
     * @param func : a string that uniquely characterizes the watchdog, for instance
     *         WDOGDC01.watchdog1.
     *
     * @return a YWatchdog object allowing you to drive the watchdog.
     */
    static YWatchdog*   FindWatchdog(string func);

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
    virtual int         registerValueCallback(YWatchdogValueCallback callback);
    using YFunction::registerValueCallback;

    virtual int         _invokeValueCallback(string value);

    /**
     * Switch the relay to the opposite state.
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         toggle(void);


    inline static YWatchdog *Find(string func)
    { return YWatchdog::FindWatchdog(func); }

    /**
     * Continues the enumeration of watchdog started using yFirstWatchdog().
     * Caution: You can't make any assumption about the returned watchdog order.
     * If you want to find a specific a watchdog, use Watchdog.findWatchdog()
     * and a hardwareID or a logical name.
     *
     * @return a pointer to a YWatchdog object, corresponding to
     *         a watchdog currently online, or a NULL pointer
     *         if there are no more watchdog to enumerate.
     */
           YWatchdog       *nextWatchdog(void);
    inline YWatchdog       *next(void)
    { return this->nextWatchdog();}

    /**
     * Starts the enumeration of watchdog currently accessible.
     * Use the method YWatchdog::nextWatchdog() to iterate on
     * next watchdog.
     *
     * @return a pointer to a YWatchdog object, corresponding to
     *         the first watchdog currently online, or a NULL pointer
     *         if there are none.
     */
           static YWatchdog *FirstWatchdog(void);
    inline static YWatchdog *First(void)
    { return YWatchdog::FirstWatchdog();}
#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of YWatchdog accessors declaration)
};

//--- (YWatchdog functions declaration)

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
 * Use the method isOnline() to test if the watchdog is
 * indeed online at a given time. In case of ambiguity when looking for
 * a watchdog by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the watchdog, for instance
 *         WDOGDC01.watchdog1.
 *
 * @return a YWatchdog object allowing you to drive the watchdog.
 */
inline YWatchdog *yFindWatchdog(const string& func)
{ return YWatchdog::FindWatchdog(func);}
/**
 * Starts the enumeration of watchdog currently accessible.
 * Use the method YWatchdog::nextWatchdog() to iterate on
 * next watchdog.
 *
 * @return a pointer to a YWatchdog object, corresponding to
 *         the first watchdog currently online, or a NULL pointer
 *         if there are none.
 */
inline YWatchdog *yFirstWatchdog(void)
{ return YWatchdog::FirstWatchdog();}

//--- (end of YWatchdog functions declaration)

#ifdef YOCTOLIB_NAMESPACE
// end of namespace definition
}
#endif

#endif
