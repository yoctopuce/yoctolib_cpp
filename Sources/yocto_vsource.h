/*********************************************************************
 *
 * $Id: yocto_vsource.h 10263 2013-03-11 17:25:38Z seb $
 *
 * Declares yFindVSource(), the high-level API for VSource functions
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


#ifndef YOCTO_VSOURCE_H
#define YOCTO_VSOURCE_H

#include "yocto_api.h"
#include <cfloat>
#include <cmath>
#include <map>

//--- (return codes)
//--- (end of return codes)
//--- (YVSource definitions)
class YVSource; //forward declaration

typedef void (*YVSourceUpdateCallback)(YVSource *func, const string& functionValue);
typedef enum {
    Y_FAILURE_FALSE = 0,
    Y_FAILURE_TRUE = 1,
    Y_FAILURE_INVALID = -1,
} Y_FAILURE_enum;

typedef enum {
    Y_OVERHEAT_FALSE = 0,
    Y_OVERHEAT_TRUE = 1,
    Y_OVERHEAT_INVALID = -1,
} Y_OVERHEAT_enum;

typedef enum {
    Y_OVERCURRENT_FALSE = 0,
    Y_OVERCURRENT_TRUE = 1,
    Y_OVERCURRENT_INVALID = -1,
} Y_OVERCURRENT_enum;

typedef enum {
    Y_OVERLOAD_FALSE = 0,
    Y_OVERLOAD_TRUE = 1,
    Y_OVERLOAD_INVALID = -1,
} Y_OVERLOAD_enum;

typedef enum {
    Y_REGULATIONFAILURE_FALSE = 0,
    Y_REGULATIONFAILURE_TRUE = 1,
    Y_REGULATIONFAILURE_INVALID = -1,
} Y_REGULATIONFAILURE_enum;

typedef enum {
    Y_EXTPOWERFAILURE_FALSE = 0,
    Y_EXTPOWERFAILURE_TRUE = 1,
    Y_EXTPOWERFAILURE_INVALID = -1,
} Y_EXTPOWERFAILURE_enum;

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
extern YMove YVSOURCE_INVALID_MOVE;
#ifndef _CLASS_YPULSE
#define _CLASS_YPULSE
class YPulse {
public:
    s16             target;
    s32             ms;
    u8              moving;
    YPulse() {}
};
#endif
extern YPulse YVSOURCE_INVALID_PULSE;
#define Y_LOGICALNAME_INVALID           (YAPI::INVALID_STRING)
#define Y_ADVERTISEDVALUE_INVALID       (YAPI::INVALID_STRING)
#define Y_UNIT_INVALID                  (YAPI::INVALID_STRING)
#define Y_VOLTAGE_INVALID               (0x80000000)
#define Y_MOVE_INVALID                  (&YVSOURCE_INVALID_MOVE)
#define Y_PULSETIMER_INVALID            (&YVSOURCE_INVALID_PULSE)
//--- (end of YVSource definitions)

//--- (YVSource declaration)
/**
 * YVSource Class: Voltage source function interface
 * 
 * Yoctopuce application programming interface allows you to control
 * the module voltage output. You affect absolute output values or make
 * transitions
 */
class YVSource: public YFunction {
protected:
    // Attributes (function value cache)
    YVSourceUpdateCallback _callback;
    string          _logicalName;
    string          _advertisedValue;
    string          _unit;
    int             _voltage;
    Y_FAILURE_enum  _failure;
    Y_OVERHEAT_enum _overHeat;
    Y_OVERCURRENT_enum _overCurrent;
    Y_OVERLOAD_enum _overLoad;
    Y_REGULATIONFAILURE_enum _regulationFailure;
    Y_EXTPOWERFAILURE_enum _extPowerFailure;
    YMove           _move;
    YPulse          _pulseTimer;
    // Static function object cache
    static std::map<string,YVSource*> _VSourceCache;

    friend YVSource *yFindVSource(const string& func);
    friend YVSource *yFirstVSource(void);

    // Function-specific method for parsing of JSON output and caching result
    int             _parse(yJsonStateMachine& j);
    //--- (end of YVSource declaration)

    //--- (YVSource constructor)
    // Constructor is protected, use yFindVSource factory function to instantiate
    YVSource(const string& func): YFunction("VSource", func)
    //--- (end of YVSource constructor)
    //--- (VSource initialization)
            ,_callback(NULL)
            ,_logicalName(Y_LOGICALNAME_INVALID)
            ,_advertisedValue(Y_ADVERTISEDVALUE_INVALID)
            ,_unit(Y_UNIT_INVALID)
            ,_voltage(Y_VOLTAGE_INVALID)
            ,_failure(Y_FAILURE_INVALID)
            ,_overHeat(Y_OVERHEAT_INVALID)
            ,_overCurrent(Y_OVERCURRENT_INVALID)
            ,_overLoad(Y_OVERLOAD_INVALID)
            ,_regulationFailure(Y_REGULATIONFAILURE_INVALID)
            ,_extPowerFailure(Y_EXTPOWERFAILURE_INVALID)
            ,_move()
            ,_pulseTimer()
    //--- (end of VSource initialization)
    {};

public:
    //--- (YVSource accessors declaration)

    static const string LOGICALNAME_INVALID;
    static const string ADVERTISEDVALUE_INVALID;
    static const string UNIT_INVALID;
    static const int      VOLTAGE_INVALID = 0x80000000;
    static const Y_FAILURE_enum FAILURE_FALSE = Y_FAILURE_FALSE;
    static const Y_FAILURE_enum FAILURE_TRUE = Y_FAILURE_TRUE;
    static const Y_FAILURE_enum FAILURE_INVALID = Y_FAILURE_INVALID;
    static const Y_OVERHEAT_enum OVERHEAT_FALSE = Y_OVERHEAT_FALSE;
    static const Y_OVERHEAT_enum OVERHEAT_TRUE = Y_OVERHEAT_TRUE;
    static const Y_OVERHEAT_enum OVERHEAT_INVALID = Y_OVERHEAT_INVALID;
    static const Y_OVERCURRENT_enum OVERCURRENT_FALSE = Y_OVERCURRENT_FALSE;
    static const Y_OVERCURRENT_enum OVERCURRENT_TRUE = Y_OVERCURRENT_TRUE;
    static const Y_OVERCURRENT_enum OVERCURRENT_INVALID = Y_OVERCURRENT_INVALID;
    static const Y_OVERLOAD_enum OVERLOAD_FALSE = Y_OVERLOAD_FALSE;
    static const Y_OVERLOAD_enum OVERLOAD_TRUE = Y_OVERLOAD_TRUE;
    static const Y_OVERLOAD_enum OVERLOAD_INVALID = Y_OVERLOAD_INVALID;
    static const Y_REGULATIONFAILURE_enum REGULATIONFAILURE_FALSE = Y_REGULATIONFAILURE_FALSE;
    static const Y_REGULATIONFAILURE_enum REGULATIONFAILURE_TRUE = Y_REGULATIONFAILURE_TRUE;
    static const Y_REGULATIONFAILURE_enum REGULATIONFAILURE_INVALID = Y_REGULATIONFAILURE_INVALID;
    static const Y_EXTPOWERFAILURE_enum EXTPOWERFAILURE_FALSE = Y_EXTPOWERFAILURE_FALSE;
    static const Y_EXTPOWERFAILURE_enum EXTPOWERFAILURE_TRUE = Y_EXTPOWERFAILURE_TRUE;
    static const Y_EXTPOWERFAILURE_enum EXTPOWERFAILURE_INVALID = Y_EXTPOWERFAILURE_INVALID;

    /**
     * Returns the logical name of the voltage source.
     * 
     * @return a string corresponding to the logical name of the voltage source
     * 
     * On failure, throws an exception or returns Y_LOGICALNAME_INVALID.
     */
           string          get_logicalName(void);
    inline string          logicalName(void)
    { return this->get_logicalName(); }

    /**
     * Changes the logical name of the voltage source. You can use yCheckLogicalName()
     * prior to this call to make sure that your parameter is valid.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     * 
     * @param newval : a string corresponding to the logical name of the voltage source
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_logicalName(const string& newval);
    inline int      setLogicalName(const string& newval)
    { return this->set_logicalName(newval); }

    /**
     * Returns the current value of the voltage source (no more than 6 characters).
     * 
     * @return a string corresponding to the current value of the voltage source (no more than 6 characters)
     * 
     * On failure, throws an exception or returns Y_ADVERTISEDVALUE_INVALID.
     */
           string          get_advertisedValue(void);
    inline string          advertisedValue(void)
    { return this->get_advertisedValue(); }

    /**
     * Returns the measuring unit for the voltage.
     * 
     * @return a string corresponding to the measuring unit for the voltage
     * 
     * On failure, throws an exception or returns Y_UNIT_INVALID.
     */
           string          get_unit(void);
    inline string          unit(void)
    { return this->get_unit(); }

    /**
     * Returns the voltage output command (mV)
     * 
     * @return an integer corresponding to the voltage output command (mV)
     * 
     * On failure, throws an exception or returns Y_VOLTAGE_INVALID.
     */
           int             get_voltage(void);
    inline int             voltage(void)
    { return this->get_voltage(); }

    /**
     * Tunes the device output voltage (milliVolts).
     * 
     * @param newval : an integer
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_voltage(int newval);
    inline int      setVoltage(int newval)
    { return this->set_voltage(newval); }

    /**
     * Returns true if the  module is in failure mode. More information can be obtained by testing
     * get_overheat, get_overcurrent etc... When a error condition is met, the output voltage is
     * set to zéro and cannot be changed until the reset() function is called.
     * 
     * @return either Y_FAILURE_FALSE or Y_FAILURE_TRUE, according to true if the  module is in failure mode
     * 
     * On failure, throws an exception or returns Y_FAILURE_INVALID.
     */
           Y_FAILURE_enum  get_failure(void);
    inline Y_FAILURE_enum  failure(void)
    { return this->get_failure(); }

    int             set_failure(Y_FAILURE_enum newval);
    inline int      setFailure(Y_FAILURE_enum newval)
    { return this->set_failure(newval); }

    /**
     * Returns TRUE if the  module is overheating.
     * 
     * @return either Y_OVERHEAT_FALSE or Y_OVERHEAT_TRUE, according to TRUE if the  module is overheating
     * 
     * On failure, throws an exception or returns Y_OVERHEAT_INVALID.
     */
           Y_OVERHEAT_enum get_overHeat(void);
    inline Y_OVERHEAT_enum overHeat(void)
    { return this->get_overHeat(); }

    /**
     * Returns true if the appliance connected to the device is too greedy .
     * 
     * @return either Y_OVERCURRENT_FALSE or Y_OVERCURRENT_TRUE, according to true if the appliance
     * connected to the device is too greedy
     * 
     * On failure, throws an exception or returns Y_OVERCURRENT_INVALID.
     */
           Y_OVERCURRENT_enum get_overCurrent(void);
    inline Y_OVERCURRENT_enum overCurrent(void)
    { return this->get_overCurrent(); }

    /**
     * Returns true if the device is not able to maintaint the requested voltage output  .
     * 
     * @return either Y_OVERLOAD_FALSE or Y_OVERLOAD_TRUE, according to true if the device is not able to
     * maintaint the requested voltage output
     * 
     * On failure, throws an exception or returns Y_OVERLOAD_INVALID.
     */
           Y_OVERLOAD_enum get_overLoad(void);
    inline Y_OVERLOAD_enum overLoad(void)
    { return this->get_overLoad(); }

    /**
     * Returns true if the voltage output is too high regarding the requested voltage  .
     * 
     * @return either Y_REGULATIONFAILURE_FALSE or Y_REGULATIONFAILURE_TRUE, according to true if the
     * voltage output is too high regarding the requested voltage
     * 
     * On failure, throws an exception or returns Y_REGULATIONFAILURE_INVALID.
     */
           Y_REGULATIONFAILURE_enum get_regulationFailure(void);
    inline Y_REGULATIONFAILURE_enum regulationFailure(void)
    { return this->get_regulationFailure(); }

    /**
     * Returns true if external power supply voltage is too low.
     * 
     * @return either Y_EXTPOWERFAILURE_FALSE or Y_EXTPOWERFAILURE_TRUE, according to true if external
     * power supply voltage is too low
     * 
     * On failure, throws an exception or returns Y_EXTPOWERFAILURE_INVALID.
     */
           Y_EXTPOWERFAILURE_enum get_extPowerFailure(void);
    inline Y_EXTPOWERFAILURE_enum extPowerFailure(void)
    { return this->get_extPowerFailure(); }

           const YMove     *get_move(void);
    inline const YMove     *move(void)
    { return this->get_move(); }

    int             set_move(const YMove * newval);
    inline int      setMove(const YMove * newval)
    { return this->set_move(newval); }

    /**
     * Performs a smooth move at constant speed toward a given value.
     * 
     * @param target      : new output value at end of transition, in milliVolts.
     * @param ms_duration : transition duration, in milliseconds
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             voltageMove(int target,int ms_duration);

           const YPulse    *get_pulseTimer(void);
    inline const YPulse    *pulseTimer(void)
    { return this->get_pulseTimer(); }

    int             set_pulseTimer(const YPulse * newval);
    inline int      setPulseTimer(const YPulse * newval)
    { return this->set_pulseTimer(newval); }

    /**
     * Sets device output to a specific volatage, for a specified duration, then brings it
     * automatically to 0V.
     * 
     * @param voltage : pulse voltage, in millivolts
     * @param ms_duration : pulse duration, in millisecondes
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             pulse(int voltage,int ms_duration);


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
    void registerValueCallback(YVSourceUpdateCallback callback);

    void advertiseValue(const string& value);

    /**
     * Continues the enumeration of voltage sources started using yFirstVSource().
     * 
     * @return a pointer to a YVSource object, corresponding to
     *         a voltage source currently online, or a null pointer
     *         if there are no more voltage sources to enumerate.
     */
           YVSource        *nextVSource(void);
    inline YVSource        *next(void)
    { return this->nextVSource();}

    /**
     * Retrieves a voltage source for a given identifier.
     * The identifier can be specified using several formats:
     * <ul>
     * <li>FunctionLogicalName</li>
     * <li>ModuleSerialNumber.FunctionIdentifier</li>
     * <li>ModuleSerialNumber.FunctionLogicalName</li>
     * <li>ModuleLogicalName.FunctionIdentifier</li>
     * <li>ModuleLogicalName.FunctionLogicalName</li>
     * </ul>
     * 
     * This function does not require that the voltage source is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method YVSource.isOnline() to test if the voltage source is
     * indeed online at a given time. In case of ambiguity when looking for
     * a voltage source by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     * 
     * @param func : a string that uniquely characterizes the voltage source
     * 
     * @return a YVSource object allowing you to drive the voltage source.
     */
           static YVSource* FindVSource(const string& func);
    inline static YVSource* Find(const string& func)
    { return YVSource::FindVSource(func);}
    /**
     * Starts the enumeration of voltage sources currently accessible.
     * Use the method YVSource.nextVSource() to iterate on
     * next voltage sources.
     * 
     * @return a pointer to a YVSource object, corresponding to
     *         the first voltage source currently online, or a null pointer
     *         if there are none.
     */
           static YVSource* FirstVSource(void);
    inline static YVSource* First(void)
    { return YVSource::FirstVSource();}
    //--- (end of YVSource accessors declaration)
};

//--- (VSource functions declaration)

/**
 * Retrieves a voltage source for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 * 
 * This function does not require that the voltage source is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method YVSource.isOnline() to test if the voltage source is
 * indeed online at a given time. In case of ambiguity when looking for
 * a voltage source by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 * 
 * @param func : a string that uniquely characterizes the voltage source
 * 
 * @return a YVSource object allowing you to drive the voltage source.
 */
inline YVSource* yFindVSource(const string& func)
{ return YVSource::FindVSource(func);}
/**
 * Starts the enumeration of voltage sources currently accessible.
 * Use the method YVSource.nextVSource() to iterate on
 * next voltage sources.
 * 
 * @return a pointer to a YVSource object, corresponding to
 *         the first voltage source currently online, or a null pointer
 *         if there are none.
 */
inline YVSource* yFirstVSource(void)
{ return YVSource::FirstVSource();}

//--- (end of VSource functions declaration)

#endif
