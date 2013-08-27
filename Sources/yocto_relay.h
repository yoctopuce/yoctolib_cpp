/*********************************************************************
 *
 * $Id: yocto_relay.h 12324 2013-08-13 15:10:31Z mvuilleu $
 *
 * Declares yFindRelay(), the high-level API for Relay functions
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


#ifndef YOCTO_RELAY_H
#define YOCTO_RELAY_H

#include "yocto_api.h"
#include <cfloat>
#include <cmath>
#include <map>

//--- (return codes)
//--- (end of return codes)
//--- (YRelay definitions)
class YRelay; //forward declaration

typedef void (*YRelayUpdateCallback)(YRelay *func, const string& functionValue);
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
extern YDelayedPulse YRELAY_INVALID_DELAYEDPULSE;
#define Y_LOGICALNAME_INVALID           (YAPI::INVALID_STRING)
#define Y_ADVERTISEDVALUE_INVALID       (YAPI::INVALID_STRING)
#define Y_PULSETIMER_INVALID            (0xffffffff)
#define Y_DELAYEDPULSETIMER_INVALID     (&YRELAY_INVALID_DELAYEDPULSE)
#define Y_COUNTDOWN_INVALID             (0xffffffff)
//--- (end of YRelay definitions)

//--- (YRelay declaration)
/**
 * YRelay Class: Relay function interface
 * 
 * The Yoctopuce application programming interface allows you to switch the relay state.
 * This change is not persistent: the relay will automatically return to its idle position
 * whenever power is lost or if the module is restarted.
 * The library can also generate automatically short pulses of determined duration.
 * On devices with two output for each relay (double throw), the two outputs are named A and B,
 * with output A corresponding to the idle position (at power off) and the output B corresponding to the
 * active state. If you prefer the alternate default state, simply switch your cables on the board.
 */
class YRelay: public YFunction {
protected:
    // Attributes (function value cache)
    YRelayUpdateCallback _callback;
    string          _logicalName;
    string          _advertisedValue;
    Y_STATE_enum    _state;
    Y_OUTPUT_enum   _output;
    unsigned        _pulseTimer;
    YDelayedPulse   _delayedPulseTimer;
    unsigned        _countdown;

    friend YRelay *yFindRelay(const string& func);
    friend YRelay *yFirstRelay(void);

    // Function-specific method for parsing of JSON output and caching result
    int             _parse(yJsonStateMachine& j);
    //--- (end of YRelay declaration)

    //--- (YRelay constructor)
    // Constructor is protected, use yFindRelay factory function to instantiate
    YRelay(const string& func);
    //--- (end of YRelay constructor)
    //--- (Relay initialization)
    //--- (end of Relay initialization)

public:
    ~YRelay();
    //--- (YRelay accessors declaration)

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

    /**
     * Returns the logical name of the relay.
     * 
     * @return a string corresponding to the logical name of the relay
     * 
     * On failure, throws an exception or returns Y_LOGICALNAME_INVALID.
     */
           string          get_logicalName(void);
    inline string          logicalName(void)
    { return this->get_logicalName(); }

    /**
     * Changes the logical name of the relay. You can use yCheckLogicalName()
     * prior to this call to make sure that your parameter is valid.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     * 
     * @param newval : a string corresponding to the logical name of the relay
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_logicalName(const string& newval);
    inline int      setLogicalName(const string& newval)
    { return this->set_logicalName(newval); }

    /**
     * Returns the current value of the relay (no more than 6 characters).
     * 
     * @return a string corresponding to the current value of the relay (no more than 6 characters)
     * 
     * On failure, throws an exception or returns Y_ADVERTISEDVALUE_INVALID.
     */
           string          get_advertisedValue(void);
    inline string          advertisedValue(void)
    { return this->get_advertisedValue(); }

    /**
     * Returns the state of the relays (A for the idle position, B for the active position).
     * 
     * @return either Y_STATE_A or Y_STATE_B, according to the state of the relays (A for the idle
     * position, B for the active position)
     * 
     * On failure, throws an exception or returns Y_STATE_INVALID.
     */
           Y_STATE_enum    get_state(void);
    inline Y_STATE_enum    state(void)
    { return this->get_state(); }

    /**
     * Changes the state of the relays (A for the idle position, B for the active position).
     * 
     * @param newval : either Y_STATE_A or Y_STATE_B, according to the state of the relays (A for the idle
     * position, B for the active position)
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_state(Y_STATE_enum newval);
    inline int      setState(Y_STATE_enum newval)
    { return this->set_state(newval); }

    /**
     * Returns the output state of the relays, when used as a simple switch (single throw).
     * 
     * @return either Y_OUTPUT_OFF or Y_OUTPUT_ON, according to the output state of the relays, when used
     * as a simple switch (single throw)
     * 
     * On failure, throws an exception or returns Y_OUTPUT_INVALID.
     */
           Y_OUTPUT_enum   get_output(void);
    inline Y_OUTPUT_enum   output(void)
    { return this->get_output(); }

    /**
     * Changes the output state of the relays, when used as a simple switch (single throw).
     * 
     * @param newval : either Y_OUTPUT_OFF or Y_OUTPUT_ON, according to the output state of the relays,
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
     * Returns the number of milliseconds remaining before the relays is returned to idle position
     * (state A), during a measured pulse generation. When there is no ongoing pulse, returns zero.
     * 
     * @return an integer corresponding to the number of milliseconds remaining before the relays is
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
    void registerValueCallback(YRelayUpdateCallback callback);

    void advertiseValue(const string& value);

    /**
     * Continues the enumeration of relays started using yFirstRelay().
     * 
     * @return a pointer to a YRelay object, corresponding to
     *         a relay currently online, or a null pointer
     *         if there are no more relays to enumerate.
     */
           YRelay          *nextRelay(void);
    inline YRelay          *next(void)
    { return this->nextRelay();}

    /**
     * Retrieves a relay for a given identifier.
     * The identifier can be specified using several formats:
     * <ul>
     * <li>FunctionLogicalName</li>
     * <li>ModuleSerialNumber.FunctionIdentifier</li>
     * <li>ModuleSerialNumber.FunctionLogicalName</li>
     * <li>ModuleLogicalName.FunctionIdentifier</li>
     * <li>ModuleLogicalName.FunctionLogicalName</li>
     * </ul>
     * 
     * This function does not require that the relay is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method YRelay.isOnline() to test if the relay is
     * indeed online at a given time. In case of ambiguity when looking for
     * a relay by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     * 
     * @param func : a string that uniquely characterizes the relay
     * 
     * @return a YRelay object allowing you to drive the relay.
     */
           static YRelay* FindRelay(const string& func);
    inline static YRelay* Find(const string& func)
    { return YRelay::FindRelay(func);}
    /**
     * Starts the enumeration of relays currently accessible.
     * Use the method YRelay.nextRelay() to iterate on
     * next relays.
     * 
     * @return a pointer to a YRelay object, corresponding to
     *         the first relay currently online, or a null pointer
     *         if there are none.
     */
           static YRelay* FirstRelay(void);
    inline static YRelay* First(void)
    { return YRelay::FirstRelay();}
    //--- (end of YRelay accessors declaration)
};

//--- (Relay functions declaration)

/**
 * Retrieves a relay for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 * 
 * This function does not require that the relay is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method YRelay.isOnline() to test if the relay is
 * indeed online at a given time. In case of ambiguity when looking for
 * a relay by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 * 
 * @param func : a string that uniquely characterizes the relay
 * 
 * @return a YRelay object allowing you to drive the relay.
 */
inline YRelay* yFindRelay(const string& func)
{ return YRelay::FindRelay(func);}
/**
 * Starts the enumeration of relays currently accessible.
 * Use the method YRelay.nextRelay() to iterate on
 * next relays.
 * 
 * @return a pointer to a YRelay object, corresponding to
 *         the first relay currently online, or a null pointer
 *         if there are none.
 */
inline YRelay* yFirstRelay(void)
{ return YRelay::FirstRelay();}

//--- (end of Relay functions declaration)

#endif
