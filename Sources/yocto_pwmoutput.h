/*********************************************************************
 *
 * $Id: pic24config.php 15411 2014-03-13 12:08:37Z mvuilleu $
 *
 * Declares yFindPwmOutput(), the high-level API for PwmOutput functions
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


#ifndef YOCTO_PWMOUTPUT_H
#define YOCTO_PWMOUTPUT_H

#include "yocto_api.h"
#include <cfloat>
#include <cmath>
#include <map>

//--- (YPwmOutput return codes)
//--- (end of YPwmOutput return codes)
//--- (YPwmOutput definitions)
class YPwmOutput; // forward declaration

typedef void (*YPwmOutputValueCallback)(YPwmOutput *func, const string& functionValue);
#ifndef _Y_ENABLED_ENUM
#define _Y_ENABLED_ENUM
typedef enum {
    Y_ENABLED_FALSE = 0,
    Y_ENABLED_TRUE = 1,
    Y_ENABLED_INVALID = -1,
} Y_ENABLED_enum;
#endif

#ifndef _Y_ENABLEDATPOWERON_ENUM
#define _Y_ENABLEDATPOWERON_ENUM
typedef enum {
    Y_ENABLEDATPOWERON_FALSE = 0,
    Y_ENABLEDATPOWERON_TRUE = 1,
    Y_ENABLEDATPOWERON_INVALID = -1,
} Y_ENABLEDATPOWERON_enum;
#endif

#define Y_DUTYCYCLE_INVALID             (YAPI_INVALID_DOUBLE)
#define Y_PULSEDURATION_INVALID         (YAPI_INVALID_DOUBLE)
#define Y_FREQUENCY_INVALID             (YAPI_INVALID_UINT)
#define Y_PERIOD_INVALID                (YAPI_INVALID_DOUBLE)
#define Y_PWMTRANSITION_INVALID         (YAPI_INVALID_STRING)
#define Y_DUTYCYCLEATPOWERON_INVALID    (YAPI_INVALID_DOUBLE)
//--- (end of YPwmOutput definitions)

//--- (YPwmOutput declaration)
/**
 * YPwmOutput Class: Pwm function interface
 * 
 * The Yoctopuce application programming interface allows you to configure, start, and stop the PWM.
 */
class YOCTO_CLASS_EXPORT YPwmOutput: public YFunction {
//--- (end of YPwmOutput declaration)
protected:
    //--- (YPwmOutput attributes)
    // Attributes (function value cache)
    Y_ENABLED_enum  _enabled;
    double          _dutyCycle;
    double          _pulseDuration;
    int             _frequency;
    double          _period;
    string          _pwmTransition;
    Y_ENABLEDATPOWERON_enum _enabledAtPowerOn;
    double          _dutyCycleAtPowerOn;
    YPwmOutputValueCallback _valueCallbackPwmOutput;

    friend YPwmOutput *yFindPwmOutput(const string& func);
    friend YPwmOutput *yFirstPwmOutput(void);

    // Function-specific method for parsing of JSON output and caching result
    virtual int     _parseAttr(yJsonStateMachine& j);

    // Constructor is protected, use yFindPwmOutput factory function to instantiate
    YPwmOutput(const string& func);
    //--- (end of YPwmOutput attributes)

public:
    ~YPwmOutput();
    //--- (YPwmOutput accessors declaration)

    static const Y_ENABLED_enum ENABLED_FALSE = Y_ENABLED_FALSE;
    static const Y_ENABLED_enum ENABLED_TRUE = Y_ENABLED_TRUE;
    static const Y_ENABLED_enum ENABLED_INVALID = Y_ENABLED_INVALID;
    static const double DUTYCYCLE_INVALID;
    static const double PULSEDURATION_INVALID;
    static const int FREQUENCY_INVALID = YAPI_INVALID_UINT;
    static const double PERIOD_INVALID;
    static const string PWMTRANSITION_INVALID;
    static const Y_ENABLEDATPOWERON_enum ENABLEDATPOWERON_FALSE = Y_ENABLEDATPOWERON_FALSE;
    static const Y_ENABLEDATPOWERON_enum ENABLEDATPOWERON_TRUE = Y_ENABLEDATPOWERON_TRUE;
    static const Y_ENABLEDATPOWERON_enum ENABLEDATPOWERON_INVALID = Y_ENABLEDATPOWERON_INVALID;
    static const double DUTYCYCLEATPOWERON_INVALID;

    /**
     * Returns the state of the PWMs.
     * 
     * @return either Y_ENABLED_FALSE or Y_ENABLED_TRUE, according to the state of the PWMs
     * 
     * On failure, throws an exception or returns Y_ENABLED_INVALID.
     */
    Y_ENABLED_enum      get_enabled(void);

    inline Y_ENABLED_enum enabled(void)
    { return this->get_enabled(); }

    /**
     * Stops or starts the PWM.
     * 
     * @param newval : either Y_ENABLED_FALSE or Y_ENABLED_TRUE
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_enabled(Y_ENABLED_enum newval);
    inline int      setEnabled(Y_ENABLED_enum newval)
    { return this->set_enabled(newval); }

    /**
     * Configures the PWMs duty cyle.
     * 
     * @param newval : a floating point number
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_dutyCycle(double newval);
    inline int      setDutyCycle(double newval)
    { return this->set_dutyCycle(newval); }

    /**
     * Returns the PWMs dutty cyle as a floating point number between 0 an 1.
     * 
     * @return a floating point number corresponding to the PWMs dutty cyle as a floating point number between 0 an 1
     * 
     * On failure, throws an exception or returns Y_DUTYCYCLE_INVALID.
     */
    double              get_dutyCycle(void);

    inline double       dutyCycle(void)
    { return this->get_dutyCycle(); }

    /**
     * Configures the PWM pluses length. A pulse length cannot be longer than period, otherwise it is truncated.
     * 
     * @param newval : a floating point number
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_pulseDuration(double newval);
    inline int      setPulseDuration(double newval)
    { return this->set_pulseDuration(newval); }

    /**
     * Returns the PWM pulse length in milliseconds.
     * 
     * @return a floating point number corresponding to the PWM pulse length in milliseconds
     * 
     * On failure, throws an exception or returns Y_PULSEDURATION_INVALID.
     */
    double              get_pulseDuration(void);

    inline double       pulseDuration(void)
    { return this->get_pulseDuration(); }

    /**
     * Returns the PWM frequency in Hz.
     * 
     * @return an integer corresponding to the PWM frequency in Hz
     * 
     * On failure, throws an exception or returns Y_FREQUENCY_INVALID.
     */
    int                 get_frequency(void);

    inline int          frequency(void)
    { return this->get_frequency(); }

    /**
     * Configures the PWM frequency. The duty cycle is kept unchanged thanks to an
     * automatic pulse width change.
     * 
     * @param newval : an integer
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_frequency(int newval);
    inline int      setFrequency(int newval)
    { return this->set_frequency(newval); }

    /**
     * Configures the PWM period.
     * 
     * @param newval : a floating point number
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_period(double newval);
    inline int      setPeriod(double newval)
    { return this->set_period(newval); }

    /**
     * Returns the PWM period in nonaseconde.
     * 
     * @return a floating point number corresponding to the PWM period in nonaseconde
     * 
     * On failure, throws an exception or returns Y_PERIOD_INVALID.
     */
    double              get_period(void);

    inline double       period(void)
    { return this->get_period(); }

    string              get_pwmTransition(void);

    inline string       pwmTransition(void)
    { return this->get_pwmTransition(); }

    int             set_pwmTransition(const string& newval);
    inline int      setPwmTransition(const string& newval)
    { return this->set_pwmTransition(newval); }

    /**
     * Returns the state of the PWMs at device power on.
     * 
     * @return either Y_ENABLEDATPOWERON_FALSE or Y_ENABLEDATPOWERON_TRUE, according to the state of the
     * PWMs at device power on
     * 
     * On failure, throws an exception or returns Y_ENABLEDATPOWERON_INVALID.
     */
    Y_ENABLEDATPOWERON_enum get_enabledAtPowerOn(void);

    inline Y_ENABLEDATPOWERON_enum enabledAtPowerOn(void)
    { return this->get_enabledAtPowerOn(); }

    /**
     * Configures the state of PWM at device power on. Remember to call the matching module saveToFlash()
     * method, otherwise this call will have no effect.
     * 
     * @param newval : either Y_ENABLEDATPOWERON_FALSE or Y_ENABLEDATPOWERON_TRUE
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_enabledAtPowerOn(Y_ENABLEDATPOWERON_enum newval);
    inline int      setEnabledAtPowerOn(Y_ENABLEDATPOWERON_enum newval)
    { return this->set_enabledAtPowerOn(newval); }

    /**
     * Configures the PWMs duty cycle at device power on. Remember to call the matching
     * module saveToFlash() method, otherwise this call will have no effect.
     * 
     * @param newval : a floating point number
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_dutyCycleAtPowerOn(double newval);
    inline int      setDutyCycleAtPowerOn(double newval)
    { return this->set_dutyCycleAtPowerOn(newval); }

    /**
     * Returns the PWMs duty cycle at device power on as a floating point number between 0.0 and 100.0%
     * 
     * @return a floating point number corresponding to the PWMs duty cycle at device power on as a
     * floating point number between 0.0 and 100
     * 
     * On failure, throws an exception or returns Y_DUTYCYCLEATPOWERON_INVALID.
     */
    double              get_dutyCycleAtPowerOn(void);

    inline double       dutyCycleAtPowerOn(void)
    { return this->get_dutyCycleAtPowerOn(); }

    /**
     * Retrieves a PWM for a given identifier.
     * The identifier can be specified using several formats:
     * <ul>
     * <li>FunctionLogicalName</li>
     * <li>ModuleSerialNumber.FunctionIdentifier</li>
     * <li>ModuleSerialNumber.FunctionLogicalName</li>
     * <li>ModuleLogicalName.FunctionIdentifier</li>
     * <li>ModuleLogicalName.FunctionLogicalName</li>
     * </ul>
     * 
     * This function does not require that the PWM is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method YPwmOutput.isOnline() to test if the PWM is
     * indeed online at a given time. In case of ambiguity when looking for
     * a PWM by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     * 
     * @param func : a string that uniquely characterizes the PWM
     * 
     * @return a YPwmOutput object allowing you to drive the PWM.
     */
    static YPwmOutput*  FindPwmOutput(string func);

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
    virtual int         registerValueCallback(YPwmOutputValueCallback callback);
    using YFunction::registerValueCallback;

    virtual int         _invokeValueCallback(string value);

    /**
     * Performs a smooth change of the pulse duration toward a given value.
     * 
     * @param ms_target   : new pulse duration at the end of the transition
     *         (floating-point number, representing the pulse duration in milliseconds)
     * @param ms_duration : total duration of the transition, in milliseconds
     * 
     * @return YAPI_SUCCESS when the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         pulseDurationMove(double ms_target,int ms_duration);

    /**
     * Performs a smooth change of the pulse duration toward a given value.
     * 
     * @param target      : new duty cycle at the end of the transition
     *         (floating-point number, between 0 and 1)
     * @param ms_duration : total duration of the transition, in milliseconds
     * 
     * @return YAPI_SUCCESS when the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         dutyCycleMove(double target,int ms_duration);


    inline static YPwmOutput* Find(string func)
    { return YPwmOutput::FindPwmOutput(func); }

    /**
     * Continues the enumeration of PWMs started using yFirstPwmOutput().
     * 
     * @return a pointer to a YPwmOutput object, corresponding to
     *         a PWM currently online, or a null pointer
     *         if there are no more PWMs to enumerate.
     */
           YPwmOutput      *nextPwmOutput(void);
    inline YPwmOutput      *next(void)
    { return this->nextPwmOutput();}

    /**
     * Starts the enumeration of PWMs currently accessible.
     * Use the method YPwmOutput.nextPwmOutput() to iterate on
     * next PWMs.
     * 
     * @return a pointer to a YPwmOutput object, corresponding to
     *         the first PWM currently online, or a null pointer
     *         if there are none.
     */
           static YPwmOutput* FirstPwmOutput(void);
    inline static YPwmOutput* First(void)
    { return YPwmOutput::FirstPwmOutput();}
    //--- (end of YPwmOutput accessors declaration)
};

//--- (PwmOutput functions declaration)

/**
 * Retrieves a PWM for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 * 
 * This function does not require that the PWM is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method YPwmOutput.isOnline() to test if the PWM is
 * indeed online at a given time. In case of ambiguity when looking for
 * a PWM by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 * 
 * @param func : a string that uniquely characterizes the PWM
 * 
 * @return a YPwmOutput object allowing you to drive the PWM.
 */
inline YPwmOutput* yFindPwmOutput(const string& func)
{ return YPwmOutput::FindPwmOutput(func);}
/**
 * Starts the enumeration of PWMs currently accessible.
 * Use the method YPwmOutput.nextPwmOutput() to iterate on
 * next PWMs.
 * 
 * @return a pointer to a YPwmOutput object, corresponding to
 *         the first PWM currently online, or a null pointer
 *         if there are none.
 */
inline YPwmOutput* yFirstPwmOutput(void)
{ return YPwmOutput::FirstPwmOutput();}

//--- (end of PwmOutput functions declaration)

#endif
