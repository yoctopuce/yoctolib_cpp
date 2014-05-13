/*********************************************************************
 *
 * $Id: pic24config.php 15635 2014-03-28 21:04:00Z mvuilleu $
 *
 * Declares yFindMotor(), the high-level API for Motor functions
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


#ifndef YOCTO_MOTOR_H
#define YOCTO_MOTOR_H

#include "yocto_api.h"
#include <cfloat>
#include <cmath>
#include <map>

//--- (YMotor return codes)
//--- (end of YMotor return codes)
//--- (YMotor definitions)
class YMotor; // forward declaration

typedef void (*YMotorValueCallback)(YMotor *func, const string& functionValue);
#ifndef _Y_MOTORSTATUS_ENUM
#define _Y_MOTORSTATUS_ENUM
typedef enum {
    Y_MOTORSTATUS_IDLE = 0,
    Y_MOTORSTATUS_BREAK = 1,
    Y_MOTORSTATUS_FORWD = 2,
    Y_MOTORSTATUS_BACKWD = 3,
    Y_MOTORSTATUS_LOVOLT = 4,
    Y_MOTORSTATUS_HICURR = 5,
    Y_MOTORSTATUS_HIHEAT = 6,
    Y_MOTORSTATUS_FAILSF = 7,
    Y_MOTORSTATUS_INVALID = -1,
} Y_MOTORSTATUS_enum;
#endif

#define Y_DRIVINGFORCE_INVALID          (YAPI_INVALID_DOUBLE)
#define Y_BREAKINGFORCE_INVALID         (YAPI_INVALID_DOUBLE)
#define Y_CUTOFFVOLTAGE_INVALID         (YAPI_INVALID_DOUBLE)
#define Y_OVERCURRENTLIMIT_INVALID      (YAPI_INVALID_INT)
#define Y_FREQUENCY_INVALID             (YAPI_INVALID_UINT)
#define Y_STARTERTIME_INVALID           (YAPI_INVALID_INT)
#define Y_FAILSAFETIMEOUT_INVALID       (YAPI_INVALID_UINT)
#define Y_COMMAND_INVALID               (YAPI_INVALID_STRING)
//--- (end of YMotor definitions)

//--- (YMotor declaration)
/**
 * YMotor Class: Motor function interface
 * 
 * Yoctopuce application programming interface allows you to drive the
 * power sent to motor to make it turn both ways, but also to drive accelerations
 * and decelerations. The motor will then accelerate automatically: you will not
 * have to monitor it. The API also allows to slow down the motor by shortening
 * its terminals: the motor will then act as an electromagnetic break.
 */
class YOCTO_CLASS_EXPORT YMotor: public YFunction {
//--- (end of YMotor declaration)
protected:
    //--- (YMotor attributes)
    // Attributes (function value cache)
    Y_MOTORSTATUS_enum _motorStatus;
    double          _drivingForce;
    double          _breakingForce;
    double          _cutOffVoltage;
    int             _overCurrentLimit;
    int             _frequency;
    int             _starterTime;
    int             _failSafeTimeout;
    string          _command;
    YMotorValueCallback _valueCallbackMotor;

    friend YMotor *yFindMotor(const string& func);
    friend YMotor *yFirstMotor(void);

    // Function-specific method for parsing of JSON output and caching result
    virtual int     _parseAttr(yJsonStateMachine& j);

    // Constructor is protected, use yFindMotor factory function to instantiate
    YMotor(const string& func);
    //--- (end of YMotor attributes)

public:
    ~YMotor();
    //--- (YMotor accessors declaration)

    static const Y_MOTORSTATUS_enum MOTORSTATUS_IDLE = Y_MOTORSTATUS_IDLE;
    static const Y_MOTORSTATUS_enum MOTORSTATUS_BREAK = Y_MOTORSTATUS_BREAK;
    static const Y_MOTORSTATUS_enum MOTORSTATUS_FORWD = Y_MOTORSTATUS_FORWD;
    static const Y_MOTORSTATUS_enum MOTORSTATUS_BACKWD = Y_MOTORSTATUS_BACKWD;
    static const Y_MOTORSTATUS_enum MOTORSTATUS_LOVOLT = Y_MOTORSTATUS_LOVOLT;
    static const Y_MOTORSTATUS_enum MOTORSTATUS_HICURR = Y_MOTORSTATUS_HICURR;
    static const Y_MOTORSTATUS_enum MOTORSTATUS_HIHEAT = Y_MOTORSTATUS_HIHEAT;
    static const Y_MOTORSTATUS_enum MOTORSTATUS_FAILSF = Y_MOTORSTATUS_FAILSF;
    static const Y_MOTORSTATUS_enum MOTORSTATUS_INVALID = Y_MOTORSTATUS_INVALID;
    static const double DRIVINGFORCE_INVALID;
    static const double BREAKINGFORCE_INVALID;
    static const double CUTOFFVOLTAGE_INVALID;
    static const int OVERCURRENTLIMIT_INVALID = YAPI_INVALID_INT;
    static const int FREQUENCY_INVALID = YAPI_INVALID_UINT;
    static const int STARTERTIME_INVALID = YAPI_INVALID_INT;
    static const int FAILSAFETIMEOUT_INVALID = YAPI_INVALID_UINT;
    static const string COMMAND_INVALID;

    /**
     * Return the controller state. Possible states are:
     * IDLE   when the motor is stopped/in free wheel, ready to start;
     * FORWD  when the controller is driving the motor forward;
     * BACKWD when the controller is driving the motor backward;
     * BREAK  when the controller is breaking;
     * LOVOLT when the controller has detected a low voltage condition;
     * HICURR when the controller has detected an overcurrent condition;
     * HIHEAT when the controller detected an overheat condition;
     * FAILSF when the controller switched on the failsafe security.
     * 
     * When an error condition occurred (LOVOLT, HICURR, HIHEAT, FAILSF), the controller
     * status must be explicitly reset using the resetStatus function.
     * 
     * @return a value among Y_MOTORSTATUS_IDLE, Y_MOTORSTATUS_BREAK, Y_MOTORSTATUS_FORWD,
     * Y_MOTORSTATUS_BACKWD, Y_MOTORSTATUS_LOVOLT, Y_MOTORSTATUS_HICURR, Y_MOTORSTATUS_HIHEAT and Y_MOTORSTATUS_FAILSF
     * 
     * On failure, throws an exception or returns Y_MOTORSTATUS_INVALID.
     */
    Y_MOTORSTATUS_enum  get_motorStatus(void);

    inline Y_MOTORSTATUS_enum motorStatus(void)
    { return this->get_motorStatus(); }

    int             set_motorStatus(Y_MOTORSTATUS_enum newval);
    inline int      setMotorStatus(Y_MOTORSTATUS_enum newval)
    { return this->set_motorStatus(newval); }

    /**
     * Changes immediately the power sent to the motor. The value is a percentage between -100%
     * to 100%. If you want go easy on your mechanics and avoid excessive current consumption,
     * try to avoid brutal power changes. For example, immediate transition from forward full power
     * to reverse full power is a very bad idea. Each time the driving power is modified, the
     * breaking power is set to zero.
     * 
     * @param newval : a floating point number corresponding to immediately the power sent to the motor
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_drivingForce(double newval);
    inline int      setDrivingForce(double newval)
    { return this->set_drivingForce(newval); }

    /**
     * Returns the power sent to the motor, as a percentage between -100% and +100%.
     * 
     * @return a floating point number corresponding to the power sent to the motor, as a percentage
     * between -100% and +100%
     * 
     * On failure, throws an exception or returns Y_DRIVINGFORCE_INVALID.
     */
    double              get_drivingForce(void);

    inline double       drivingForce(void)
    { return this->get_drivingForce(); }

    /**
     * Changes immediately the breaking force applied to the motor (in per cents).
     * The value 0 corresponds to no breaking (free wheel). When the breaking force
     * is changed, the driving power is set to zero. The value is a percentage.
     * 
     * @param newval : a floating point number corresponding to immediately the breaking force applied to
     * the motor (in per cents)
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_breakingForce(double newval);
    inline int      setBreakingForce(double newval)
    { return this->set_breakingForce(newval); }

    /**
     * Returns the breaking force applied to the motor, as a percentage.
     * The value 0 corresponds to no breaking (free wheel).
     * 
     * @return a floating point number corresponding to the breaking force applied to the motor, as a percentage
     * 
     * On failure, throws an exception or returns Y_BREAKINGFORCE_INVALID.
     */
    double              get_breakingForce(void);

    inline double       breakingForce(void)
    { return this->get_breakingForce(); }

    /**
     * Changes the threshold voltage under which the controller will automatically switch to error state
     * and prevent further current draw. This setting prevent damage to a battery that can
     * occur when drawing current from an "empty" battery.
     * Note that whatever the cutoff threshold, the controller will switch to undervoltage
     * error state if the power supply goes under 3V, even for a very brief time.
     * 
     * @param newval : a floating point number corresponding to the threshold voltage under which the
     * controller will automatically switch to error state
     *         and prevent further current draw
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_cutOffVoltage(double newval);
    inline int      setCutOffVoltage(double newval)
    { return this->set_cutOffVoltage(newval); }

    /**
     * Returns the threshold voltage under which the controller will automatically switch to error state
     * and prevent further current draw. This setting prevent damage to a battery that can
     * occur when drawing current from an "empty" battery.
     * 
     * @return a floating point number corresponding to the threshold voltage under which the controller
     * will automatically switch to error state
     *         and prevent further current draw
     * 
     * On failure, throws an exception or returns Y_CUTOFFVOLTAGE_INVALID.
     */
    double              get_cutOffVoltage(void);

    inline double       cutOffVoltage(void)
    { return this->get_cutOffVoltage(); }

    /**
     * Returns the current threshold (in mA) above which the controller will automatically
     * switch to error state. A zero value means that there is no limit.
     * 
     * @return an integer corresponding to the current threshold (in mA) above which the controller will automatically
     *         switch to error state
     * 
     * On failure, throws an exception or returns Y_OVERCURRENTLIMIT_INVALID.
     */
    int                 get_overCurrentLimit(void);

    inline int          overCurrentLimit(void)
    { return this->get_overCurrentLimit(); }

    /**
     * Changes tthe current threshold (in mA) above which the controller will automatically
     * switch to error state. A zero value means that there is no limit. Note that whatever the
     * current limit is, the controller will switch to OVERCURRENT status if the current
     * goes above 32A, even for a very brief time.
     * 
     * @param newval : an integer corresponding to tthe current threshold (in mA) above which the
     * controller will automatically
     *         switch to error state
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_overCurrentLimit(int newval);
    inline int      setOverCurrentLimit(int newval)
    { return this->set_overCurrentLimit(newval); }

    /**
     * Returns the PWM frequency used to control the motor.
     * 
     * @return an integer corresponding to the PWM frequency used to control the motor
     * 
     * On failure, throws an exception or returns Y_FREQUENCY_INVALID.
     */
    int                 get_frequency(void);

    inline int          frequency(void)
    { return this->get_frequency(); }

    /**
     * Changes the PWM frequency used to control the motor. Low frequency is usually
     * more efficient and may help the motor to start, but an audible noise might be
     * generated. A higher frequency reduces the noise, but more energy is converted
     * into heat.
     * 
     * @param newval : an integer corresponding to the PWM frequency used to control the motor
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_frequency(int newval);
    inline int      setFrequency(int newval)
    { return this->set_frequency(newval); }

    /**
     * Returns the duration (in ms) during which the motor is driven at low frequency to help
     * it start up.
     * 
     * @return an integer corresponding to the duration (in ms) during which the motor is driven at low
     * frequency to help
     *         it start up
     * 
     * On failure, throws an exception or returns Y_STARTERTIME_INVALID.
     */
    int                 get_starterTime(void);

    inline int          starterTime(void)
    { return this->get_starterTime(); }

    /**
     * Changes the duration (in ms) during which the motor is driven at low frequency to help
     * it start up.
     * 
     * @param newval : an integer corresponding to the duration (in ms) during which the motor is driven
     * at low frequency to help
     *         it start up
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_starterTime(int newval);
    inline int      setStarterTime(int newval)
    { return this->set_starterTime(newval); }

    /**
     * Returns the delay in milliseconds allowed for the controller to run autonomously without
     * receiving any instruction from the control process. Once this delay is elapsed,
     * the controller will automatically stop the motor and switch to FAILSAFE error.
     * Failsafe security is disabled when the value is zero.
     * 
     * @return an integer corresponding to the delay in milliseconds allowed for the controller to run
     * autonomously without
     *         receiving any instruction from the control process
     * 
     * On failure, throws an exception or returns Y_FAILSAFETIMEOUT_INVALID.
     */
    int                 get_failSafeTimeout(void);

    inline int          failSafeTimeout(void)
    { return this->get_failSafeTimeout(); }

    /**
     * Changes the delay in milliseconds allowed for the controller to run autonomously without
     * receiving any instruction from the control process. Once this delay is elapsed,
     * the controller will automatically stop the motor and switch to FAILSAFE error.
     * Failsafe security is disabled when the value is zero.
     * 
     * @param newval : an integer corresponding to the delay in milliseconds allowed for the controller to
     * run autonomously without
     *         receiving any instruction from the control process
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_failSafeTimeout(int newval);
    inline int      setFailSafeTimeout(int newval)
    { return this->set_failSafeTimeout(newval); }

    string              get_command(void);

    inline string       command(void)
    { return this->get_command(); }

    int             set_command(const string& newval);
    inline int      setCommand(const string& newval)
    { return this->set_command(newval); }

    /**
     * Retrieves a motor for a given identifier.
     * The identifier can be specified using several formats:
     * <ul>
     * <li>FunctionLogicalName</li>
     * <li>ModuleSerialNumber.FunctionIdentifier</li>
     * <li>ModuleSerialNumber.FunctionLogicalName</li>
     * <li>ModuleLogicalName.FunctionIdentifier</li>
     * <li>ModuleLogicalName.FunctionLogicalName</li>
     * </ul>
     * 
     * This function does not require that the motor is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method YMotor.isOnline() to test if the motor is
     * indeed online at a given time. In case of ambiguity when looking for
     * a motor by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     * 
     * @param func : a string that uniquely characterizes the motor
     * 
     * @return a YMotor object allowing you to drive the motor.
     */
    static YMotor*      FindMotor(string func);

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
    virtual int         registerValueCallback(YMotorValueCallback callback);
    using YFunction::registerValueCallback;

    virtual int         _invokeValueCallback(string value);

    /**
     * Rearms the controller failsafe timer. When the motor is running and the failsafe feature
     * is active, this function should be called periodically to prove that the control process
     * is running properly. Otherwise, the motor will be automatically stopped after the specified
     * timeout. Calling a motor <i>set</i> function implicitely rearms the failsafe timer.
     */
    virtual int         keepALive(void);

    /**
     * Reset the controller state to IDLE. This function must be invoked explicitely
     * after any error condition is signaled.
     */
    virtual int         resetStatus(void);

    /**
     * Changes progressively the power sent to the moteur for a specific duration.
     * 
     * @param targetPower : desired motor power, in per cents (between -100% and +100%)
     * @param delay : duration (in ms) of the transition
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         drivingForceMove(double targetPower,int delay);

    /**
     * Changes progressively the breaking force applied to the motor for a specific duration.
     * 
     * @param targetPower : desired breaking force, in per cents
     * @param delay : duration (in ms) of the transition
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         breakingForceMove(double targetPower,int delay);


    inline static YMotor* Find(string func)
    { return YMotor::FindMotor(func); }

    /**
     * Continues the enumeration of motors started using yFirstMotor().
     * 
     * @return a pointer to a YMotor object, corresponding to
     *         a motor currently online, or a null pointer
     *         if there are no more motors to enumerate.
     */
           YMotor          *nextMotor(void);
    inline YMotor          *next(void)
    { return this->nextMotor();}

    /**
     * Starts the enumeration of motors currently accessible.
     * Use the method YMotor.nextMotor() to iterate on
     * next motors.
     * 
     * @return a pointer to a YMotor object, corresponding to
     *         the first motor currently online, or a null pointer
     *         if there are none.
     */
           static YMotor* FirstMotor(void);
    inline static YMotor* First(void)
    { return YMotor::FirstMotor();}
    //--- (end of YMotor accessors declaration)
};

//--- (Motor functions declaration)

/**
 * Retrieves a motor for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 * 
 * This function does not require that the motor is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method YMotor.isOnline() to test if the motor is
 * indeed online at a given time. In case of ambiguity when looking for
 * a motor by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 * 
 * @param func : a string that uniquely characterizes the motor
 * 
 * @return a YMotor object allowing you to drive the motor.
 */
inline YMotor* yFindMotor(const string& func)
{ return YMotor::FindMotor(func);}
/**
 * Starts the enumeration of motors currently accessible.
 * Use the method YMotor.nextMotor() to iterate on
 * next motors.
 * 
 * @return a pointer to a YMotor object, corresponding to
 *         the first motor currently online, or a null pointer
 *         if there are none.
 */
inline YMotor* yFirstMotor(void)
{ return YMotor::FirstMotor();}

//--- (end of Motor functions declaration)

#endif
