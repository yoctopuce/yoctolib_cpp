/*********************************************************************
 *
 *  $Id: svn_id $
 *
 *  Declares yFindServo(), the high-level API for Servo functions
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


#ifndef YOCTO_SERVO_H
#define YOCTO_SERVO_H

#include <cfloat>
#include <cmath>

#include "yocto_api.h"

#ifdef YOCTOLIB_NAMESPACE
namespace YOCTOLIB_NAMESPACE
{
#endif

//--- (YServo return codes)
//--- (end of YServo return codes)
//--- (YServo yapiwrapper)
//--- (end of YServo yapiwrapper)
//--- (YServo definitions)
class YServo; // forward declaration

typedef void (*YServoValueCallback)(YServo *func, const string& functionValue);
#ifndef _Y_ENABLED_ENUM
#define _Y_ENABLED_ENUM
typedef enum {
    Y_ENABLED_FALSE = 0,
    Y_ENABLED_TRUE = 1,
    Y_ENABLED_INVALID = -1,
} Y_ENABLED_enum;
#endif
#ifndef _CLASS_YMOVE
#define _CLASS_YMOVE
class YOCTO_CLASS_EXPORT YMove {
public:
    int             target;
    int             ms;
    int             moving;

    YMove()
        :target(YAPI_INVALID_INT), ms(YAPI_INVALID_INT), moving(YAPI_INVALID_UINT)
    {}

    bool operator==(const YMove& o) const {
         return (target == o.target) && (ms == o.ms) && (moving == o.moving);
    }
};
#endif
#ifndef _Y_ENABLEDATPOWERON_ENUM
#define _Y_ENABLEDATPOWERON_ENUM
typedef enum {
    Y_ENABLEDATPOWERON_FALSE = 0,
    Y_ENABLEDATPOWERON_TRUE = 1,
    Y_ENABLEDATPOWERON_INVALID = -1,
} Y_ENABLEDATPOWERON_enum;
#endif
#define Y_POSITION_INVALID              (YAPI_INVALID_INT)
#define Y_RANGE_INVALID                 (YAPI_INVALID_UINT)
#define Y_NEUTRAL_INVALID               (YAPI_INVALID_UINT)
#define Y_POSITIONATPOWERON_INVALID     (YAPI_INVALID_INT)
//--- (end of YServo definitions)

//--- (YServo declaration)
/**
 * YServo Class: RC servo motor control interface, available for instance in the Yocto-Servo
 *
 * The YServo class is designed to drive remote-control servo motors
 * outputs. This class allows you not only to move
 * a servo to a given position, but also to specify the time interval
 * in which the move should be performed. This makes it possible to
 * synchronize two servos involved in a same move.
 */
class YOCTO_CLASS_EXPORT YServo: public YFunction {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of YServo declaration)
protected:
    //--- (YServo attributes)
    // Attributes (function value cache)
    int             _position;
    Y_ENABLED_enum  _enabled;
    int             _range;
    int             _neutral;
    YMove           _move;
    int             _positionAtPowerOn;
    Y_ENABLEDATPOWERON_enum _enabledAtPowerOn;
    YServoValueCallback _valueCallbackServo;

    friend YServo *yFindServo(const string& func);
    friend YServo *yFirstServo(void);

    // Function-specific method for parsing of JSON output and caching result
    virtual int     _parseAttr(YJSONObject *json_val);

    // Constructor is protected, use yFindServo factory function to instantiate
    YServo(const string& func);
    //--- (end of YServo attributes)

public:
    virtual ~YServo();
    //--- (YServo accessors declaration)

    static const int POSITION_INVALID = YAPI_INVALID_INT;
    static const Y_ENABLED_enum ENABLED_FALSE = Y_ENABLED_FALSE;
    static const Y_ENABLED_enum ENABLED_TRUE = Y_ENABLED_TRUE;
    static const Y_ENABLED_enum ENABLED_INVALID = Y_ENABLED_INVALID;
    static const int RANGE_INVALID = YAPI_INVALID_UINT;
    static const int NEUTRAL_INVALID = YAPI_INVALID_UINT;
    static const YMove MOVE_INVALID;
    static const int POSITIONATPOWERON_INVALID = YAPI_INVALID_INT;
    static const Y_ENABLEDATPOWERON_enum ENABLEDATPOWERON_FALSE = Y_ENABLEDATPOWERON_FALSE;
    static const Y_ENABLEDATPOWERON_enum ENABLEDATPOWERON_TRUE = Y_ENABLEDATPOWERON_TRUE;
    static const Y_ENABLEDATPOWERON_enum ENABLEDATPOWERON_INVALID = Y_ENABLEDATPOWERON_INVALID;

    /**
     * Returns the current servo position.
     *
     * @return an integer corresponding to the current servo position
     *
     * On failure, throws an exception or returns YServo::POSITION_INVALID.
     */
    int                 get_position(void);

    inline int          position(void)
    { return this->get_position(); }

    /**
     * Changes immediately the servo driving position.
     *
     * @param newval : an integer corresponding to immediately the servo driving position
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_position(int newval);
    inline int      setPosition(int newval)
    { return this->set_position(newval); }

    /**
     * Returns the state of the RC servo motors.
     *
     * @return either YServo::ENABLED_FALSE or YServo::ENABLED_TRUE, according to the state of the RC servo motors
     *
     * On failure, throws an exception or returns YServo::ENABLED_INVALID.
     */
    Y_ENABLED_enum      get_enabled(void);

    inline Y_ENABLED_enum enabled(void)
    { return this->get_enabled(); }

    /**
     * Stops or starts the RC servo motor.
     *
     * @param newval : either YServo::ENABLED_FALSE or YServo::ENABLED_TRUE
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_enabled(Y_ENABLED_enum newval);
    inline int      setEnabled(Y_ENABLED_enum newval)
    { return this->set_enabled(newval); }

    /**
     * Returns the current range of use of the servo.
     *
     * @return an integer corresponding to the current range of use of the servo
     *
     * On failure, throws an exception or returns YServo::RANGE_INVALID.
     */
    int                 get_range(void);

    inline int          range(void)
    { return this->get_range(); }

    /**
     * Changes the range of use of the servo, specified in per cents.
     * A range of 100% corresponds to a standard control signal, that varies
     * from 1 [ms] to 2 [ms], When using a servo that supports a double range,
     * from 0.5 [ms] to 2.5 [ms], you can select a range of 200%.
     * Be aware that using a range higher than what is supported by the servo
     * is likely to damage the servo. Remember to call the matching module
     * saveToFlash() method, otherwise this call will have no effect.
     *
     * @param newval : an integer corresponding to the range of use of the servo, specified in per cents
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_range(int newval);
    inline int      setRange(int newval)
    { return this->set_range(newval); }

    /**
     * Returns the duration in microseconds of a neutral pulse for the servo.
     *
     * @return an integer corresponding to the duration in microseconds of a neutral pulse for the servo
     *
     * On failure, throws an exception or returns YServo::NEUTRAL_INVALID.
     */
    int                 get_neutral(void);

    inline int          neutral(void)
    { return this->get_neutral(); }

    /**
     * Changes the duration of the pulse corresponding to the neutral position of the servo.
     * The duration is specified in microseconds, and the standard value is 1500 [us].
     * This setting makes it possible to shift the range of use of the servo.
     * Be aware that using a range higher than what is supported by the servo is
     * likely to damage the servo. Remember to call the matching module
     * saveToFlash() method, otherwise this call will have no effect.
     *
     * @param newval : an integer corresponding to the duration of the pulse corresponding to the neutral
     * position of the servo
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_neutral(int newval);
    inline int      setNeutral(int newval)
    { return this->set_neutral(newval); }

    YMove               get_move(void);

    inline YMove        move(void)
    { return this->get_move(); }

    int             set_move(YMove newval);
    inline int      setMove(YMove newval)
    { return this->set_move(newval); }

    /**
     * Performs a smooth move at constant speed toward a given position.
     *
     * @param target      : new position at the end of the move
     * @param ms_duration : total duration of the move, in milliseconds
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             move(int target,int ms_duration);

    /**
     * Returns the servo position at device power up.
     *
     * @return an integer corresponding to the servo position at device power up
     *
     * On failure, throws an exception or returns YServo::POSITIONATPOWERON_INVALID.
     */
    int                 get_positionAtPowerOn(void);

    inline int          positionAtPowerOn(void)
    { return this->get_positionAtPowerOn(); }

    /**
     * Configure the servo position at device power up. Remember to call the matching
     * module saveToFlash() method, otherwise this call will have no effect.
     *
     * @param newval : an integer
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_positionAtPowerOn(int newval);
    inline int      setPositionAtPowerOn(int newval)
    { return this->set_positionAtPowerOn(newval); }

    /**
     * Returns the servo signal generator state at power up.
     *
     * @return either YServo::ENABLEDATPOWERON_FALSE or YServo::ENABLEDATPOWERON_TRUE, according to the
     * servo signal generator state at power up
     *
     * On failure, throws an exception or returns YServo::ENABLEDATPOWERON_INVALID.
     */
    Y_ENABLEDATPOWERON_enum get_enabledAtPowerOn(void);

    inline Y_ENABLEDATPOWERON_enum enabledAtPowerOn(void)
    { return this->get_enabledAtPowerOn(); }

    /**
     * Configure the servo signal generator state at power up. Remember to call the matching module saveToFlash()
     * method, otherwise this call will have no effect.
     *
     * @param newval : either YServo::ENABLEDATPOWERON_FALSE or YServo::ENABLEDATPOWERON_TRUE
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_enabledAtPowerOn(Y_ENABLEDATPOWERON_enum newval);
    inline int      setEnabledAtPowerOn(Y_ENABLEDATPOWERON_enum newval)
    { return this->set_enabledAtPowerOn(newval); }

    /**
     * Retrieves a RC servo motor for a given identifier.
     * The identifier can be specified using several formats:
     *
     * - FunctionLogicalName
     * - ModuleSerialNumber.FunctionIdentifier
     * - ModuleSerialNumber.FunctionLogicalName
     * - ModuleLogicalName.FunctionIdentifier
     * - ModuleLogicalName.FunctionLogicalName
     *
     *
     * This function does not require that the RC servo motor is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method isOnline() to test if the RC servo motor is
     * indeed online at a given time. In case of ambiguity when looking for
     * a RC servo motor by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     *
     * If a call to this object's is_online() method returns FALSE although
     * you are certain that the matching device is plugged, make sure that you did
     * call registerHub() at application initialization time.
     *
     * @param func : a string that uniquely characterizes the RC servo motor, for instance
     *         SERVORC1.servo1.
     *
     * @return a YServo object allowing you to drive the RC servo motor.
     */
    static YServo*      FindServo(string func);

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
    virtual int         registerValueCallback(YServoValueCallback callback);
    using YFunction::registerValueCallback;

    virtual int         _invokeValueCallback(string value);


    inline static YServo *Find(string func)
    { return YServo::FindServo(func); }

    /**
     * Continues the enumeration of RC servo motors started using yFirstServo().
     * Caution: You can't make any assumption about the returned RC servo motors order.
     * If you want to find a specific a RC servo motor, use Servo.findServo()
     * and a hardwareID or a logical name.
     *
     * @return a pointer to a YServo object, corresponding to
     *         a RC servo motor currently online, or a NULL pointer
     *         if there are no more RC servo motors to enumerate.
     */
           YServo          *nextServo(void);
    inline YServo          *next(void)
    { return this->nextServo();}

    /**
     * Starts the enumeration of RC servo motors currently accessible.
     * Use the method YServo::nextServo() to iterate on
     * next RC servo motors.
     *
     * @return a pointer to a YServo object, corresponding to
     *         the first RC servo motor currently online, or a NULL pointer
     *         if there are none.
     */
           static YServo *FirstServo(void);
    inline static YServo *First(void)
    { return YServo::FirstServo();}
#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of YServo accessors declaration)
};

//--- (YServo functions declaration)

/**
 * Retrieves a RC servo motor for a given identifier.
 * The identifier can be specified using several formats:
 *
 * - FunctionLogicalName
 * - ModuleSerialNumber.FunctionIdentifier
 * - ModuleSerialNumber.FunctionLogicalName
 * - ModuleLogicalName.FunctionIdentifier
 * - ModuleLogicalName.FunctionLogicalName
 *
 *
 * This function does not require that the RC servo motor is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the RC servo motor is
 * indeed online at a given time. In case of ambiguity when looking for
 * a RC servo motor by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the RC servo motor, for instance
 *         SERVORC1.servo1.
 *
 * @return a YServo object allowing you to drive the RC servo motor.
 */
inline YServo *yFindServo(const string& func)
{ return YServo::FindServo(func);}
/**
 * Starts the enumeration of RC servo motors currently accessible.
 * Use the method YServo::nextServo() to iterate on
 * next RC servo motors.
 *
 * @return a pointer to a YServo object, corresponding to
 *         the first RC servo motor currently online, or a NULL pointer
 *         if there are none.
 */
inline YServo *yFirstServo(void)
{ return YServo::FirstServo();}

//--- (end of YServo functions declaration)

#ifdef YOCTOLIB_NAMESPACE
// end of namespace definition
}
#endif

#endif
