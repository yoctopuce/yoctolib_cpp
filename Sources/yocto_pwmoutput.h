/*********************************************************************
 *
 *  $Id: yocto_pwmoutput.h 58920 2024-01-12 09:39:03Z seb $
 *
 *  Declares yFindPwmOutput(), the high-level API for PwmOutput functions
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


#ifndef YOCTO_PWMOUTPUT_H
#define YOCTO_PWMOUTPUT_H

#include <cfloat>
#include <cmath>

#include "yocto_api.h"

#ifdef YOCTOLIB_NAMESPACE
namespace YOCTOLIB_NAMESPACE
{
#endif

//--- (YPwmOutput return codes)
//--- (end of YPwmOutput return codes)
//--- (YPwmOutput yapiwrapper)
//--- (end of YPwmOutput yapiwrapper)
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
#ifndef _Y_INVERTEDOUTPUT_ENUM
#define _Y_INVERTEDOUTPUT_ENUM
typedef enum {
    Y_INVERTEDOUTPUT_FALSE = 0,
    Y_INVERTEDOUTPUT_TRUE = 1,
    Y_INVERTEDOUTPUT_INVALID = -1,
} Y_INVERTEDOUTPUT_enum;
#endif
#ifndef _Y_ENABLEDATPOWERON_ENUM
#define _Y_ENABLEDATPOWERON_ENUM
typedef enum {
    Y_ENABLEDATPOWERON_FALSE = 0,
    Y_ENABLEDATPOWERON_TRUE = 1,
    Y_ENABLEDATPOWERON_INVALID = -1,
} Y_ENABLEDATPOWERON_enum;
#endif
#define Y_FREQUENCY_INVALID             (YAPI_INVALID_DOUBLE)
#define Y_PERIOD_INVALID                (YAPI_INVALID_DOUBLE)
#define Y_DUTYCYCLE_INVALID             (YAPI_INVALID_DOUBLE)
#define Y_PULSEDURATION_INVALID         (YAPI_INVALID_DOUBLE)
#define Y_PWMTRANSITION_INVALID         (YAPI_INVALID_STRING)
#define Y_DUTYCYCLEATPOWERON_INVALID    (YAPI_INVALID_DOUBLE)
//--- (end of YPwmOutput definitions)

//--- (YPwmOutput declaration)
/**
 * YPwmOutput Class: PWM generator control interface, available for instance in the Yocto-PWM-Tx
 *
 * The YPwmOutput class allows you to drive a pulse-width modulated output (PWM).
 * You can configure the frequency as well as the duty cycle, and setup progressive
 * transitions.
 */
class YOCTO_CLASS_EXPORT YPwmOutput: public YFunction {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of YPwmOutput declaration)
protected:
    //--- (YPwmOutput attributes)
    // Attributes (function value cache)
    Y_ENABLED_enum  _enabled;
    double          _frequency;
    double          _period;
    double          _dutyCycle;
    double          _pulseDuration;
    string          _pwmTransition;
    Y_INVERTEDOUTPUT_enum _invertedOutput;
    Y_ENABLEDATPOWERON_enum _enabledAtPowerOn;
    double          _dutyCycleAtPowerOn;
    YPwmOutputValueCallback _valueCallbackPwmOutput;

    friend YPwmOutput *yFindPwmOutput(const string& func);
    friend YPwmOutput *yFirstPwmOutput(void);

    // Function-specific method for parsing of JSON output and caching result
    virtual int     _parseAttr(YJSONObject *json_val);

    // Constructor is protected, use yFindPwmOutput factory function to instantiate
    YPwmOutput(const string& func);
    //--- (end of YPwmOutput attributes)

public:
    virtual ~YPwmOutput();
    //--- (YPwmOutput accessors declaration)

    static const Y_ENABLED_enum ENABLED_FALSE = Y_ENABLED_FALSE;
    static const Y_ENABLED_enum ENABLED_TRUE = Y_ENABLED_TRUE;
    static const Y_ENABLED_enum ENABLED_INVALID = Y_ENABLED_INVALID;
    static const double FREQUENCY_INVALID;
    static const double PERIOD_INVALID;
    static const double DUTYCYCLE_INVALID;
    static const double PULSEDURATION_INVALID;
    static const string PWMTRANSITION_INVALID;
    static const Y_INVERTEDOUTPUT_enum INVERTEDOUTPUT_FALSE = Y_INVERTEDOUTPUT_FALSE;
    static const Y_INVERTEDOUTPUT_enum INVERTEDOUTPUT_TRUE = Y_INVERTEDOUTPUT_TRUE;
    static const Y_INVERTEDOUTPUT_enum INVERTEDOUTPUT_INVALID = Y_INVERTEDOUTPUT_INVALID;
    static const Y_ENABLEDATPOWERON_enum ENABLEDATPOWERON_FALSE = Y_ENABLEDATPOWERON_FALSE;
    static const Y_ENABLEDATPOWERON_enum ENABLEDATPOWERON_TRUE = Y_ENABLEDATPOWERON_TRUE;
    static const Y_ENABLEDATPOWERON_enum ENABLEDATPOWERON_INVALID = Y_ENABLEDATPOWERON_INVALID;
    static const double DUTYCYCLEATPOWERON_INVALID;

    /**
     * Returns the state of the PWM generators.
     *
     * @return either YPwmOutput::ENABLED_FALSE or YPwmOutput::ENABLED_TRUE, according to the state of the PWM generators
     *
     * On failure, throws an exception or returns YPwmOutput::ENABLED_INVALID.
     */
    Y_ENABLED_enum      get_enabled(void);

    inline Y_ENABLED_enum enabled(void)
    { return this->get_enabled(); }

    /**
     * Stops or starts the PWM.
     *
     * @param newval : either YPwmOutput::ENABLED_FALSE or YPwmOutput::ENABLED_TRUE
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_enabled(Y_ENABLED_enum newval);
    inline int      setEnabled(Y_ENABLED_enum newval)
    { return this->set_enabled(newval); }

    /**
     * Changes the PWM frequency. The duty cycle is kept unchanged thanks to an
     * automatic pulse width change, in other words, the change will not be applied
     * before the end of the current period. This can significantly affect reaction
     * time at low frequencies. If you call the matching module saveToFlash()
     * method, the frequency will be kept after a device power cycle.
     * To stop the PWM signal, do not set the frequency to zero, use the set_enabled()
     * method instead.
     *
     * @param newval : a floating point number corresponding to the PWM frequency
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_frequency(double newval);
    inline int      setFrequency(double newval)
    { return this->set_frequency(newval); }

    /**
     * Returns the PWM frequency in Hz.
     *
     * @return a floating point number corresponding to the PWM frequency in Hz
     *
     * On failure, throws an exception or returns YPwmOutput::FREQUENCY_INVALID.
     */
    double              get_frequency(void);

    inline double       frequency(void)
    { return this->get_frequency(); }

    /**
     * Changes the PWM period in milliseconds. Caution: in order to avoid  random truncation of
     * the current pulse, the change will not be applied
     * before the end of the current period. This can significantly affect reaction
     * time at low frequencies. If you call the matching module saveToFlash()
     * method, the frequency will be kept after a device power cycle.
     *
     * @param newval : a floating point number corresponding to the PWM period in milliseconds
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_period(double newval);
    inline int      setPeriod(double newval)
    { return this->set_period(newval); }

    /**
     * Returns the PWM period in milliseconds.
     *
     * @return a floating point number corresponding to the PWM period in milliseconds
     *
     * On failure, throws an exception or returns YPwmOutput::PERIOD_INVALID.
     */
    double              get_period(void);

    inline double       period(void)
    { return this->get_period(); }

    /**
     * Changes the PWM duty cycle, in per cents.
     *
     * @param newval : a floating point number corresponding to the PWM duty cycle, in per cents
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_dutyCycle(double newval);
    inline int      setDutyCycle(double newval)
    { return this->set_dutyCycle(newval); }

    /**
     * Returns the PWM duty cycle, in per cents.
     *
     * @return a floating point number corresponding to the PWM duty cycle, in per cents
     *
     * On failure, throws an exception or returns YPwmOutput::DUTYCYCLE_INVALID.
     */
    double              get_dutyCycle(void);

    inline double       dutyCycle(void)
    { return this->get_dutyCycle(); }

    /**
     * Changes the PWM pulse length, in milliseconds. A pulse length cannot be longer than period,
     * otherwise it is truncated.
     *
     * @param newval : a floating point number corresponding to the PWM pulse length, in milliseconds
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_pulseDuration(double newval);
    inline int      setPulseDuration(double newval)
    { return this->set_pulseDuration(newval); }

    /**
     * Returns the PWM pulse length in milliseconds, as a floating point number.
     *
     * @return a floating point number corresponding to the PWM pulse length in milliseconds, as a
     * floating point number
     *
     * On failure, throws an exception or returns YPwmOutput::PULSEDURATION_INVALID.
     */
    double              get_pulseDuration(void);

    inline double       pulseDuration(void)
    { return this->get_pulseDuration(); }

    string              get_pwmTransition(void);

    inline string       pwmTransition(void)
    { return this->get_pwmTransition(); }

    int             set_pwmTransition(const string& newval);
    inline int      setPwmTransition(const string& newval)
    { return this->set_pwmTransition(newval); }

    /**
     * Returns true if the output signal is configured as inverted, and false otherwise.
     *
     * @return either YPwmOutput::INVERTEDOUTPUT_FALSE or YPwmOutput::INVERTEDOUTPUT_TRUE, according to true
     * if the output signal is configured as inverted, and false otherwise
     *
     * On failure, throws an exception or returns YPwmOutput::INVERTEDOUTPUT_INVALID.
     */
    Y_INVERTEDOUTPUT_enum get_invertedOutput(void);

    inline Y_INVERTEDOUTPUT_enum invertedOutput(void)
    { return this->get_invertedOutput(); }

    /**
     * Changes the inversion mode of the output signal.
     * Remember to call the matching module saveToFlash() method if you want
     * the change to be kept after power cycle.
     *
     * @param newval : either YPwmOutput::INVERTEDOUTPUT_FALSE or YPwmOutput::INVERTEDOUTPUT_TRUE, according
     * to the inversion mode of the output signal
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_invertedOutput(Y_INVERTEDOUTPUT_enum newval);
    inline int      setInvertedOutput(Y_INVERTEDOUTPUT_enum newval)
    { return this->set_invertedOutput(newval); }

    /**
     * Returns the state of the PWM at device power on.
     *
     * @return either YPwmOutput::ENABLEDATPOWERON_FALSE or YPwmOutput::ENABLEDATPOWERON_TRUE, according to
     * the state of the PWM at device power on
     *
     * On failure, throws an exception or returns YPwmOutput::ENABLEDATPOWERON_INVALID.
     */
    Y_ENABLEDATPOWERON_enum get_enabledAtPowerOn(void);

    inline Y_ENABLEDATPOWERON_enum enabledAtPowerOn(void)
    { return this->get_enabledAtPowerOn(); }

    /**
     * Changes the state of the PWM at device power on. Remember to call the matching module saveToFlash()
     * method, otherwise this call will have no effect.
     *
     * @param newval : either YPwmOutput::ENABLEDATPOWERON_FALSE or YPwmOutput::ENABLEDATPOWERON_TRUE,
     * according to the state of the PWM at device power on
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_enabledAtPowerOn(Y_ENABLEDATPOWERON_enum newval);
    inline int      setEnabledAtPowerOn(Y_ENABLEDATPOWERON_enum newval)
    { return this->set_enabledAtPowerOn(newval); }

    /**
     * Changes the PWM duty cycle at device power on. Remember to call the matching
     * module saveToFlash() method, otherwise this call will have no effect.
     *
     * @param newval : a floating point number corresponding to the PWM duty cycle at device power on
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_dutyCycleAtPowerOn(double newval);
    inline int      setDutyCycleAtPowerOn(double newval)
    { return this->set_dutyCycleAtPowerOn(newval); }

    /**
     * Returns the PWM generators duty cycle at device power on as a floating point number between 0 and 100.
     *
     * @return a floating point number corresponding to the PWM generators duty cycle at device power on
     * as a floating point number between 0 and 100
     *
     * On failure, throws an exception or returns YPwmOutput::DUTYCYCLEATPOWERON_INVALID.
     */
    double              get_dutyCycleAtPowerOn(void);

    inline double       dutyCycleAtPowerOn(void)
    { return this->get_dutyCycleAtPowerOn(); }

    /**
     * Retrieves a PWM generator for a given identifier.
     * The identifier can be specified using several formats:
     * <ul>
     * <li>FunctionLogicalName</li>
     * <li>ModuleSerialNumber.FunctionIdentifier</li>
     * <li>ModuleSerialNumber.FunctionLogicalName</li>
     * <li>ModuleLogicalName.FunctionIdentifier</li>
     * <li>ModuleLogicalName.FunctionLogicalName</li>
     * </ul>
     *
     * This function does not require that the PWM generator is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method isOnline() to test if the PWM generator is
     * indeed online at a given time. In case of ambiguity when looking for
     * a PWM generator by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     *
     * If a call to this object's is_online() method returns FALSE although
     * you are certain that the matching device is plugged, make sure that you did
     * call registerHub() at application initialization time.
     *
     * @param func : a string that uniquely characterizes the PWM generator, for instance
     *         YPWMTX01.pwmOutput1.
     *
     * @return a YPwmOutput object allowing you to drive the PWM generator.
     */
    static YPwmOutput*  FindPwmOutput(string func);

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
    virtual int         registerValueCallback(YPwmOutputValueCallback callback);
    using YFunction::registerValueCallback;

    virtual int         _invokeValueCallback(string value);

    /**
     * Performs a smooth transition of the pulse duration toward a given value.
     * Any period, frequency, duty cycle or pulse width change will cancel any ongoing transition process.
     *
     * @param ms_target   : new pulse duration at the end of the transition
     *         (floating-point number, representing the pulse duration in milliseconds)
     * @param ms_duration : total duration of the transition, in milliseconds
     *
     * @return YAPI::SUCCESS when the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         pulseDurationMove(double ms_target,int ms_duration);

    /**
     * Performs a smooth change of the duty cycle toward a given value.
     * Any period, frequency, duty cycle or pulse width change will cancel any ongoing transition process.
     *
     * @param target      : new duty cycle at the end of the transition
     *         (percentage, floating-point number between 0 and 100)
     * @param ms_duration : total duration of the transition, in milliseconds
     *
     * @return YAPI::SUCCESS when the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         dutyCycleMove(double target,int ms_duration);

    /**
     * Performs a smooth frequency change toward a given value.
     * Any period, frequency, duty cycle or pulse width change will cancel any ongoing transition process.
     *
     * @param target      : new frequency at the end of the transition (floating-point number)
     * @param ms_duration : total duration of the transition, in milliseconds
     *
     * @return YAPI::SUCCESS when the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         frequencyMove(double target,int ms_duration);

    /**
     * Performs a smooth transition toward a specified value of the phase shift between this channel
     * and the other channel. The phase shift is executed by slightly changing the frequency
     * temporarily during the specified duration. This function only makes sense when both channels
     * are running, either at the same frequency, or at a multiple of the channel frequency.
     * Any period, frequency, duty cycle or pulse width change will cancel any ongoing transition process.
     *
     * @param target      : phase shift at the end of the transition, in milliseconds (floating-point number)
     * @param ms_duration : total duration of the transition, in milliseconds
     *
     * @return YAPI::SUCCESS when the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         phaseMove(double target,int ms_duration);

    /**
     * Trigger a given number of pulses of specified duration, at current frequency.
     * At the end of the pulse train, revert to the original state of the PWM generator.
     *
     * @param ms_target : desired pulse duration
     *         (floating-point number, representing the pulse duration in milliseconds)
     * @param n_pulses  : desired pulse count
     *
     * @return YAPI::SUCCESS when the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         triggerPulsesByDuration(double ms_target,int n_pulses);

    /**
     * Trigger a given number of pulses of specified duration, at current frequency.
     * At the end of the pulse train, revert to the original state of the PWM generator.
     *
     * @param target   : desired duty cycle for the generated pulses
     *         (percentage, floating-point number between 0 and 100)
     * @param n_pulses : desired pulse count
     *
     * @return YAPI::SUCCESS when the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         triggerPulsesByDutyCycle(double target,int n_pulses);

    /**
     * Trigger a given number of pulses at the specified frequency, using current duty cycle.
     * At the end of the pulse train, revert to the original state of the PWM generator.
     *
     * @param target   : desired frequency for the generated pulses (floating-point number)
     * @param n_pulses : desired pulse count
     *
     * @return YAPI::SUCCESS when the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         triggerPulsesByFrequency(double target,int n_pulses);

    virtual int         markForRepeat(void);

    virtual int         repeatFromMark(void);


    inline static YPwmOutput *Find(string func)
    { return YPwmOutput::FindPwmOutput(func); }

    /**
     * Continues the enumeration of PWM generators started using yFirstPwmOutput().
     * Caution: You can't make any assumption about the returned PWM generators order.
     * If you want to find a specific a PWM generator, use PwmOutput.findPwmOutput()
     * and a hardwareID or a logical name.
     *
     * @return a pointer to a YPwmOutput object, corresponding to
     *         a PWM generator currently online, or a NULL pointer
     *         if there are no more PWM generators to enumerate.
     */
           YPwmOutput      *nextPwmOutput(void);
    inline YPwmOutput      *next(void)
    { return this->nextPwmOutput();}

    /**
     * Starts the enumeration of PWM generators currently accessible.
     * Use the method YPwmOutput::nextPwmOutput() to iterate on
     * next PWM generators.
     *
     * @return a pointer to a YPwmOutput object, corresponding to
     *         the first PWM generator currently online, or a NULL pointer
     *         if there are none.
     */
           static YPwmOutput *FirstPwmOutput(void);
    inline static YPwmOutput *First(void)
    { return YPwmOutput::FirstPwmOutput();}
#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of YPwmOutput accessors declaration)
};

//--- (YPwmOutput functions declaration)

/**
 * Retrieves a PWM generator for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that the PWM generator is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the PWM generator is
 * indeed online at a given time. In case of ambiguity when looking for
 * a PWM generator by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the PWM generator, for instance
 *         YPWMTX01.pwmOutput1.
 *
 * @return a YPwmOutput object allowing you to drive the PWM generator.
 */
inline YPwmOutput *yFindPwmOutput(const string& func)
{ return YPwmOutput::FindPwmOutput(func);}
/**
 * Starts the enumeration of PWM generators currently accessible.
 * Use the method YPwmOutput::nextPwmOutput() to iterate on
 * next PWM generators.
 *
 * @return a pointer to a YPwmOutput object, corresponding to
 *         the first PWM generator currently online, or a NULL pointer
 *         if there are none.
 */
inline YPwmOutput *yFirstPwmOutput(void)
{ return YPwmOutput::FirstPwmOutput();}

//--- (end of YPwmOutput functions declaration)

#ifdef YOCTOLIB_NAMESPACE
// end of namespace definition
}
#endif

#endif
