/*********************************************************************
 *
 * $Id: yocto_servo.h 12324 2013-08-13 15:10:31Z mvuilleu $
 *
 * Declares yFindServo(), the high-level API for Servo functions
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


#ifndef YOCTO_SERVO_H
#define YOCTO_SERVO_H

#include "yocto_api.h"
#include <cfloat>
#include <cmath>
#include <map>

//--- (return codes)
//--- (end of return codes)
//--- (YServo definitions)
class YServo; //forward declaration

typedef void (*YServoUpdateCallback)(YServo *func, const string& functionValue);
#ifndef _CLASS_YMOVE
#define _CLASS_YMOVE
class YMove {
public:
    s32             target;
    s16             ms;
    u8              moving;
    YMove() {}
};
#endif
extern YMove YSERVO_INVALID_MOVE;
#define Y_LOGICALNAME_INVALID           (YAPI::INVALID_STRING)
#define Y_ADVERTISEDVALUE_INVALID       (YAPI::INVALID_STRING)
#define Y_POSITION_INVALID              (0x80000000)
#define Y_RANGE_INVALID                 (-1)
#define Y_NEUTRAL_INVALID               (-1)
#define Y_MOVE_INVALID                  (&YSERVO_INVALID_MOVE)
//--- (end of YServo definitions)

//--- (YServo declaration)
/**
 * YServo Class: Servo function interface
 * 
 * Yoctopuce application programming interface allows you not only to move
 * a servo to a given position, but also to specify the time interval
 * in which the move should be performed. This makes it possible to
 * synchronize two servos involved in a same move.
 */
class YServo: public YFunction {
protected:
    // Attributes (function value cache)
    YServoUpdateCallback _callback;
    string          _logicalName;
    string          _advertisedValue;
    int             _position;
    int             _range;
    int             _neutral;
    YMove           _move;

    friend YServo *yFindServo(const string& func);
    friend YServo *yFirstServo(void);

    // Function-specific method for parsing of JSON output and caching result
    int             _parse(yJsonStateMachine& j);
    //--- (end of YServo declaration)

    //--- (YServo constructor)
    // Constructor is protected, use yFindServo factory function to instantiate
    YServo(const string& func);
    //--- (end of YServo constructor)
    //--- (Servo initialization)
    //--- (end of Servo initialization)

public:
    ~YServo();
    //--- (YServo accessors declaration)

    static const string LOGICALNAME_INVALID;
    static const string ADVERTISEDVALUE_INVALID;
    static const int      POSITION_INVALID = 0x80000000;
    static const int      RANGE_INVALID = -1;
    static const int      NEUTRAL_INVALID = -1;

    /**
     * Returns the logical name of the servo.
     * 
     * @return a string corresponding to the logical name of the servo
     * 
     * On failure, throws an exception or returns Y_LOGICALNAME_INVALID.
     */
           string          get_logicalName(void);
    inline string          logicalName(void)
    { return this->get_logicalName(); }

    /**
     * Changes the logical name of the servo. You can use yCheckLogicalName()
     * prior to this call to make sure that your parameter is valid.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     * 
     * @param newval : a string corresponding to the logical name of the servo
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_logicalName(const string& newval);
    inline int      setLogicalName(const string& newval)
    { return this->set_logicalName(newval); }

    /**
     * Returns the current value of the servo (no more than 6 characters).
     * 
     * @return a string corresponding to the current value of the servo (no more than 6 characters)
     * 
     * On failure, throws an exception or returns Y_ADVERTISEDVALUE_INVALID.
     */
           string          get_advertisedValue(void);
    inline string          advertisedValue(void)
    { return this->get_advertisedValue(); }

    /**
     * Returns the current servo position.
     * 
     * @return an integer corresponding to the current servo position
     * 
     * On failure, throws an exception or returns Y_POSITION_INVALID.
     */
           int             get_position(void);
    inline int             position(void)
    { return this->get_position(); }

    /**
     * Changes immediately the servo driving position.
     * 
     * @param newval : an integer corresponding to immediately the servo driving position
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_position(int newval);
    inline int      setPosition(int newval)
    { return this->set_position(newval); }

    /**
     * Returns the current range of use of the servo.
     * 
     * @return an integer corresponding to the current range of use of the servo
     * 
     * On failure, throws an exception or returns Y_RANGE_INVALID.
     */
           int             get_range(void);
    inline int             range(void)
    { return this->get_range(); }

    /**
     * Changes the range of use of the servo, specified in per cents.
     * A range of 100% corresponds to a standard control signal, that varies
     * from 1 [ms] to 2 [ms], When using a servo that supports a double range,
     * from 0.5 [ms] to 2.5 [ms], you can select a range of 200%.
     * Be aware that using a range higher than what is supported by the servo
     * is likely to damage the servo.
     * 
     * @param newval : an integer corresponding to the range of use of the servo, specified in per cents
     * 
     * @return YAPI_SUCCESS if the call succeeds.
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
     * On failure, throws an exception or returns Y_NEUTRAL_INVALID.
     */
           int             get_neutral(void);
    inline int             neutral(void)
    { return this->get_neutral(); }

    /**
     * Changes the duration of the pulse corresponding to the neutral position of the servo.
     * The duration is specified in microseconds, and the standard value is 1500 [us].
     * This setting makes it possible to shift the range of use of the servo.
     * Be aware that using a range higher than what is supported by the servo is
     * likely to damage the servo.
     * 
     * @param newval : an integer corresponding to the duration of the pulse corresponding to the neutral
     * position of the servo
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_neutral(int newval);
    inline int      setNeutral(int newval)
    { return this->set_neutral(newval); }

           const YMove     *get_move(void);
    inline const YMove     *move(void)
    { return this->get_move(); }

    int             set_move(const YMove * newval);
    inline int      setMove(const YMove * newval)
    { return this->set_move(newval); }

    /**
     * Performs a smooth move at constant speed toward a given position.
     * 
     * @param target      : new position at the end of the move
     * @param ms_duration : total duration of the move, in milliseconds
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             move(int target,int ms_duration);


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
    void registerValueCallback(YServoUpdateCallback callback);

    void advertiseValue(const string& value);

    /**
     * Continues the enumeration of servos started using yFirstServo().
     * 
     * @return a pointer to a YServo object, corresponding to
     *         a servo currently online, or a null pointer
     *         if there are no more servos to enumerate.
     */
           YServo          *nextServo(void);
    inline YServo          *next(void)
    { return this->nextServo();}

    /**
     * Retrieves a servo for a given identifier.
     * The identifier can be specified using several formats:
     * <ul>
     * <li>FunctionLogicalName</li>
     * <li>ModuleSerialNumber.FunctionIdentifier</li>
     * <li>ModuleSerialNumber.FunctionLogicalName</li>
     * <li>ModuleLogicalName.FunctionIdentifier</li>
     * <li>ModuleLogicalName.FunctionLogicalName</li>
     * </ul>
     * 
     * This function does not require that the servo is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method YServo.isOnline() to test if the servo is
     * indeed online at a given time. In case of ambiguity when looking for
     * a servo by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     * 
     * @param func : a string that uniquely characterizes the servo
     * 
     * @return a YServo object allowing you to drive the servo.
     */
           static YServo* FindServo(const string& func);
    inline static YServo* Find(const string& func)
    { return YServo::FindServo(func);}
    /**
     * Starts the enumeration of servos currently accessible.
     * Use the method YServo.nextServo() to iterate on
     * next servos.
     * 
     * @return a pointer to a YServo object, corresponding to
     *         the first servo currently online, or a null pointer
     *         if there are none.
     */
           static YServo* FirstServo(void);
    inline static YServo* First(void)
    { return YServo::FirstServo();}
    //--- (end of YServo accessors declaration)
};

//--- (Servo functions declaration)

/**
 * Retrieves a servo for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 * 
 * This function does not require that the servo is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method YServo.isOnline() to test if the servo is
 * indeed online at a given time. In case of ambiguity when looking for
 * a servo by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 * 
 * @param func : a string that uniquely characterizes the servo
 * 
 * @return a YServo object allowing you to drive the servo.
 */
inline YServo* yFindServo(const string& func)
{ return YServo::FindServo(func);}
/**
 * Starts the enumeration of servos currently accessible.
 * Use the method YServo.nextServo() to iterate on
 * next servos.
 * 
 * @return a pointer to a YServo object, corresponding to
 *         the first servo currently online, or a null pointer
 *         if there are none.
 */
inline YServo* yFirstServo(void)
{ return YServo::FirstServo();}

//--- (end of Servo functions declaration)

#endif
