/*********************************************************************
 *
 * $Id: yocto_led.h 9921 2013-02-20 09:39:16Z seb $
 *
 * Declares yFindLed(), the high-level API for Led functions
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


#ifndef YOCTO_LED_H
#define YOCTO_LED_H

#include "yocto_api.h"
#include <cfloat>
#include <cmath>
#include <map>

//--- (return codes)
//--- (end of return codes)
//--- (YLed definitions)
class YLed; //forward declaration

typedef void (*YLedUpdateCallback)(YLed *func, const string& functionValue);
typedef enum {
    Y_POWER_OFF = 0,
    Y_POWER_ON = 1,
    Y_POWER_INVALID = -1,
} Y_POWER_enum;

typedef enum {
    Y_BLINKING_STILL = 0,
    Y_BLINKING_RELAX = 1,
    Y_BLINKING_AWARE = 2,
    Y_BLINKING_RUN = 3,
    Y_BLINKING_CALL = 4,
    Y_BLINKING_PANIC = 5,
    Y_BLINKING_INVALID = -1,
} Y_BLINKING_enum;

#define Y_LOGICALNAME_INVALID           (YAPI::INVALID_STRING)
#define Y_ADVERTISEDVALUE_INVALID       (YAPI::INVALID_STRING)
#define Y_LUMINOSITY_INVALID            (-1)
//--- (end of YLed definitions)

//--- (YLed declaration)
/**
 * YLed Class: Led function interface
 * 
 * Yoctopuce application programming interface
 * allows you not only to drive the intensity of the led, but also to
 * have it blink at various preset frequencies.
 */
class YLed: public YFunction {
protected:
    // Attributes (function value cache)
    YLedUpdateCallback _callback;
    string          _logicalName;
    string          _advertisedValue;
    Y_POWER_enum    _power;
    int             _luminosity;
    Y_BLINKING_enum _blinking;
    // Static function object cache
    static std::map<string,YLed*> _LedCache;

    friend YLed *yFindLed(const string& func);
    friend YLed *yFirstLed(void);

    // Function-specific method for parsing of JSON output and caching result
    int             _parse(yJsonStateMachine& j);
    //--- (end of YLed declaration)

    //--- (YLed constructor)
    // Constructor is protected, use yFindLed factory function to instantiate
    YLed(const string& func): YFunction("Led", func)
    //--- (end of YLed constructor)
    //--- (Led initialization)
            ,_callback(NULL)
            ,_logicalName(Y_LOGICALNAME_INVALID)
            ,_advertisedValue(Y_ADVERTISEDVALUE_INVALID)
            ,_power(Y_POWER_INVALID)
            ,_luminosity(Y_LUMINOSITY_INVALID)
            ,_blinking(Y_BLINKING_INVALID)
    //--- (end of Led initialization)
    {};

public:
    //--- (YLed accessors declaration)

    static const string LOGICALNAME_INVALID;
    static const string ADVERTISEDVALUE_INVALID;
    static const Y_POWER_enum POWER_OFF = Y_POWER_OFF;
    static const Y_POWER_enum POWER_ON = Y_POWER_ON;
    static const Y_POWER_enum POWER_INVALID = Y_POWER_INVALID;
    static const int      LUMINOSITY_INVALID = -1;
    static const Y_BLINKING_enum BLINKING_STILL = Y_BLINKING_STILL;
    static const Y_BLINKING_enum BLINKING_RELAX = Y_BLINKING_RELAX;
    static const Y_BLINKING_enum BLINKING_AWARE = Y_BLINKING_AWARE;
    static const Y_BLINKING_enum BLINKING_RUN = Y_BLINKING_RUN;
    static const Y_BLINKING_enum BLINKING_CALL = Y_BLINKING_CALL;
    static const Y_BLINKING_enum BLINKING_PANIC = Y_BLINKING_PANIC;
    static const Y_BLINKING_enum BLINKING_INVALID = Y_BLINKING_INVALID;

    /**
     * Returns the logical name of the led.
     * 
     * @return a string corresponding to the logical name of the led
     * 
     * On failure, throws an exception or returns Y_LOGICALNAME_INVALID.
     */
           string          get_logicalName(void);
    inline string          logicalName(void)
    { return this->get_logicalName(); }

    /**
     * Changes the logical name of the led. You can use yCheckLogicalName()
     * prior to this call to make sure that your parameter is valid.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     * 
     * @param newval : a string corresponding to the logical name of the led
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_logicalName(const string& newval);
    inline int      setLogicalName(const string& newval)
    { return this->set_logicalName(newval); }

    /**
     * Returns the current value of the led (no more than 6 characters).
     * 
     * @return a string corresponding to the current value of the led (no more than 6 characters)
     * 
     * On failure, throws an exception or returns Y_ADVERTISEDVALUE_INVALID.
     */
           string          get_advertisedValue(void);
    inline string          advertisedValue(void)
    { return this->get_advertisedValue(); }

    /**
     * Returns the current led state.
     * 
     * @return either Y_POWER_OFF or Y_POWER_ON, according to the current led state
     * 
     * On failure, throws an exception or returns Y_POWER_INVALID.
     */
           Y_POWER_enum    get_power(void);
    inline Y_POWER_enum    power(void)
    { return this->get_power(); }

    /**
     * Changes the state of the led.
     * 
     * @param newval : either Y_POWER_OFF or Y_POWER_ON, according to the state of the led
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_power(Y_POWER_enum newval);
    inline int      setPower(Y_POWER_enum newval)
    { return this->set_power(newval); }

    /**
     * Returns the current led intensity (in per cent).
     * 
     * @return an integer corresponding to the current led intensity (in per cent)
     * 
     * On failure, throws an exception or returns Y_LUMINOSITY_INVALID.
     */
           int             get_luminosity(void);
    inline int             luminosity(void)
    { return this->get_luminosity(); }

    /**
     * Changes the current led intensity (in per cent).
     * 
     * @param newval : an integer corresponding to the current led intensity (in per cent)
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_luminosity(int newval);
    inline int      setLuminosity(int newval)
    { return this->set_luminosity(newval); }

    /**
     * Returns the current led signaling mode.
     * 
     * @return a value among Y_BLINKING_STILL, Y_BLINKING_RELAX, Y_BLINKING_AWARE, Y_BLINKING_RUN,
     * Y_BLINKING_CALL and Y_BLINKING_PANIC corresponding to the current led signaling mode
     * 
     * On failure, throws an exception or returns Y_BLINKING_INVALID.
     */
           Y_BLINKING_enum get_blinking(void);
    inline Y_BLINKING_enum blinking(void)
    { return this->get_blinking(); }

    /**
     * Changes the current led signaling mode.
     * 
     * @param newval : a value among Y_BLINKING_STILL, Y_BLINKING_RELAX, Y_BLINKING_AWARE, Y_BLINKING_RUN,
     * Y_BLINKING_CALL and Y_BLINKING_PANIC corresponding to the current led signaling mode
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_blinking(Y_BLINKING_enum newval);
    inline int      setBlinking(Y_BLINKING_enum newval)
    { return this->set_blinking(newval); }


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
    void registerValueCallback(YLedUpdateCallback callback);

    void advertiseValue(const string& value);

    /**
     * Continues the enumeration of leds started using yFirstLed().
     * 
     * @return a pointer to a YLed object, corresponding to
     *         a led currently online, or a null pointer
     *         if there are no more leds to enumerate.
     */
           YLed            *nextLed(void);
    inline YLed            *next(void)
    { return this->nextLed();}

    /**
     * Retrieves a led for a given identifier.
     * The identifier can be specified using several formats:
     * <ul>
     * <li>FunctionLogicalName</li>
     * <li>ModuleSerialNumber.FunctionIdentifier</li>
     * <li>ModuleSerialNumber.FunctionLogicalName</li>
     * <li>ModuleLogicalName.FunctionIdentifier</li>
     * <li>ModuleLogicalName.FunctionLogicalName</li>
     * </ul>
     * 
     * This function does not require that the led is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method YLed.isOnline() to test if the led is
     * indeed online at a given time. In case of ambiguity when looking for
     * a led by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     * 
     * @param func : a string that uniquely characterizes the led
     * 
     * @return a YLed object allowing you to drive the led.
     */
           static YLed* FindLed(const string& func);
    inline static YLed* Find(const string& func)
    { return YLed::FindLed(func);}
    /**
     * Starts the enumeration of leds currently accessible.
     * Use the method YLed.nextLed() to iterate on
     * next leds.
     * 
     * @return a pointer to a YLed object, corresponding to
     *         the first led currently online, or a null pointer
     *         if there are none.
     */
           static YLed* FirstLed(void);
    inline static YLed* First(void)
    { return YLed::FirstLed();}
    //--- (end of YLed accessors declaration)
};

//--- (Led functions declaration)

/**
 * Retrieves a led for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 * 
 * This function does not require that the led is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method YLed.isOnline() to test if the led is
 * indeed online at a given time. In case of ambiguity when looking for
 * a led by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 * 
 * @param func : a string that uniquely characterizes the led
 * 
 * @return a YLed object allowing you to drive the led.
 */
inline YLed* yFindLed(const string& func)
{ return YLed::FindLed(func);}
/**
 * Starts the enumeration of leds currently accessible.
 * Use the method YLed.nextLed() to iterate on
 * next leds.
 * 
 * @return a pointer to a YLed object, corresponding to
 *         the first led currently online, or a null pointer
 *         if there are none.
 */
inline YLed* yFirstLed(void)
{ return YLed::FirstLed();}

//--- (end of Led functions declaration)

#endif
