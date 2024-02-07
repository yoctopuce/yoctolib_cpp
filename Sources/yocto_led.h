/*********************************************************************
 *
 *  $Id: yocto_led.h 44049 2021-02-26 10:57:40Z web $
 *
 *  Declares yFindLed(), the high-level API for Led functions
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


#ifndef YOCTO_LED_H
#define YOCTO_LED_H

#include <cfloat>
#include <cmath>

#include "yocto_api.h"

#ifdef YOCTOLIB_NAMESPACE
namespace YOCTOLIB_NAMESPACE
{
#endif

//--- (YLed return codes)
//--- (end of YLed return codes)
//--- (YLed yapiwrapper)
//--- (end of YLed yapiwrapper)
//--- (YLed definitions)
class YLed; // forward declaration

typedef void (*YLedValueCallback)(YLed *func, const string& functionValue);
#ifndef _Y_POWER_ENUM
#define _Y_POWER_ENUM
typedef enum {
    Y_POWER_OFF = 0,
    Y_POWER_ON = 1,
    Y_POWER_INVALID = -1,
} Y_POWER_enum;
#endif
#ifndef _Y_BLINKING_ENUM
#define _Y_BLINKING_ENUM
typedef enum {
    Y_BLINKING_STILL = 0,
    Y_BLINKING_RELAX = 1,
    Y_BLINKING_AWARE = 2,
    Y_BLINKING_RUN = 3,
    Y_BLINKING_CALL = 4,
    Y_BLINKING_PANIC = 5,
    Y_BLINKING_INVALID = -1,
} Y_BLINKING_enum;
#endif
#define Y_LUMINOSITY_INVALID            (YAPI_INVALID_UINT)
//--- (end of YLed definitions)

//--- (YLed declaration)
/**
 * YLed Class: monochrome LED control interface, available for instance in the Yocto-Buzzer, the
 * YoctoBox-Short-Thin-Black-Prox, the YoctoBox-Short-Thin-Transp or the YoctoBox-Short-Thin-Transp-Prox
 *
 * The YLed class allows you to drive a monocolor LED.
 * You can not only to drive the intensity of the LED, but also to
 * have it blink at various preset frequencies.
 */
class YOCTO_CLASS_EXPORT YLed: public YFunction {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of YLed declaration)
protected:
    //--- (YLed attributes)
    // Attributes (function value cache)
    Y_POWER_enum    _power;
    int             _luminosity;
    Y_BLINKING_enum _blinking;
    YLedValueCallback _valueCallbackLed;

    friend YLed *yFindLed(const string& func);
    friend YLed *yFirstLed(void);

    // Function-specific method for parsing of JSON output and caching result
    virtual int     _parseAttr(YJSONObject *json_val);

    // Constructor is protected, use yFindLed factory function to instantiate
    YLed(const string& func);
    //--- (end of YLed attributes)

public:
    virtual ~YLed();
    //--- (YLed accessors declaration)

    static const Y_POWER_enum POWER_OFF = Y_POWER_OFF;
    static const Y_POWER_enum POWER_ON = Y_POWER_ON;
    static const Y_POWER_enum POWER_INVALID = Y_POWER_INVALID;
    static const int LUMINOSITY_INVALID = YAPI_INVALID_UINT;
    static const Y_BLINKING_enum BLINKING_STILL = Y_BLINKING_STILL;
    static const Y_BLINKING_enum BLINKING_RELAX = Y_BLINKING_RELAX;
    static const Y_BLINKING_enum BLINKING_AWARE = Y_BLINKING_AWARE;
    static const Y_BLINKING_enum BLINKING_RUN = Y_BLINKING_RUN;
    static const Y_BLINKING_enum BLINKING_CALL = Y_BLINKING_CALL;
    static const Y_BLINKING_enum BLINKING_PANIC = Y_BLINKING_PANIC;
    static const Y_BLINKING_enum BLINKING_INVALID = Y_BLINKING_INVALID;

    /**
     * Returns the current LED state.
     *
     * @return either YLed::POWER_OFF or YLed::POWER_ON, according to the current LED state
     *
     * On failure, throws an exception or returns YLed::POWER_INVALID.
     */
    Y_POWER_enum        get_power(void);

    inline Y_POWER_enum power(void)
    { return this->get_power(); }

    /**
     * Changes the state of the LED.
     *
     * @param newval : either YLed::POWER_OFF or YLed::POWER_ON, according to the state of the LED
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_power(Y_POWER_enum newval);
    inline int      setPower(Y_POWER_enum newval)
    { return this->set_power(newval); }

    /**
     * Returns the current LED intensity (in per cent).
     *
     * @return an integer corresponding to the current LED intensity (in per cent)
     *
     * On failure, throws an exception or returns YLed::LUMINOSITY_INVALID.
     */
    int                 get_luminosity(void);

    inline int          luminosity(void)
    { return this->get_luminosity(); }

    /**
     * Changes the current LED intensity (in per cent). Remember to call the
     * saveToFlash() method of the module if the modification must be kept.
     *
     * @param newval : an integer corresponding to the current LED intensity (in per cent)
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_luminosity(int newval);
    inline int      setLuminosity(int newval)
    { return this->set_luminosity(newval); }

    /**
     * Returns the current LED signaling mode.
     *
     * @return a value among YLed::BLINKING_STILL, YLed::BLINKING_RELAX, YLed::BLINKING_AWARE,
     * YLed::BLINKING_RUN, YLed::BLINKING_CALL and YLed::BLINKING_PANIC corresponding to the current LED signaling mode
     *
     * On failure, throws an exception or returns YLed::BLINKING_INVALID.
     */
    Y_BLINKING_enum     get_blinking(void);

    inline Y_BLINKING_enum blinking(void)
    { return this->get_blinking(); }

    /**
     * Changes the current LED signaling mode.
     *
     * @param newval : a value among YLed::BLINKING_STILL, YLed::BLINKING_RELAX, YLed::BLINKING_AWARE,
     * YLed::BLINKING_RUN, YLed::BLINKING_CALL and YLed::BLINKING_PANIC corresponding to the current LED signaling mode
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_blinking(Y_BLINKING_enum newval);
    inline int      setBlinking(Y_BLINKING_enum newval)
    { return this->set_blinking(newval); }

    /**
     * Retrieves a monochrome LED for a given identifier.
     * The identifier can be specified using several formats:
     * <ul>
     * <li>FunctionLogicalName</li>
     * <li>ModuleSerialNumber.FunctionIdentifier</li>
     * <li>ModuleSerialNumber.FunctionLogicalName</li>
     * <li>ModuleLogicalName.FunctionIdentifier</li>
     * <li>ModuleLogicalName.FunctionLogicalName</li>
     * </ul>
     *
     * This function does not require that the monochrome LED is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method isOnline() to test if the monochrome LED is
     * indeed online at a given time. In case of ambiguity when looking for
     * a monochrome LED by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     *
     * If a call to this object's is_online() method returns FALSE although
     * you are certain that the matching device is plugged, make sure that you did
     * call registerHub() at application initialization time.
     *
     * @param func : a string that uniquely characterizes the monochrome LED, for instance
     *         YBUZZER2.led1.
     *
     * @return a YLed object allowing you to drive the monochrome LED.
     */
    static YLed*        FindLed(string func);

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
    virtual int         registerValueCallback(YLedValueCallback callback);
    using YFunction::registerValueCallback;

    virtual int         _invokeValueCallback(string value);


    inline static YLed *Find(string func)
    { return YLed::FindLed(func); }

    /**
     * Continues the enumeration of monochrome LEDs started using yFirstLed().
     * Caution: You can't make any assumption about the returned monochrome LEDs order.
     * If you want to find a specific a monochrome LED, use Led.findLed()
     * and a hardwareID or a logical name.
     *
     * @return a pointer to a YLed object, corresponding to
     *         a monochrome LED currently online, or a NULL pointer
     *         if there are no more monochrome LEDs to enumerate.
     */
           YLed            *nextLed(void);
    inline YLed            *next(void)
    { return this->nextLed();}

    /**
     * Starts the enumeration of monochrome LEDs currently accessible.
     * Use the method YLed::nextLed() to iterate on
     * next monochrome LEDs.
     *
     * @return a pointer to a YLed object, corresponding to
     *         the first monochrome LED currently online, or a NULL pointer
     *         if there are none.
     */
           static YLed *FirstLed(void);
    inline static YLed *First(void)
    { return YLed::FirstLed();}
#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of YLed accessors declaration)
};

//--- (YLed functions declaration)

/**
 * Retrieves a monochrome LED for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that the monochrome LED is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the monochrome LED is
 * indeed online at a given time. In case of ambiguity when looking for
 * a monochrome LED by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the monochrome LED, for instance
 *         YBUZZER2.led1.
 *
 * @return a YLed object allowing you to drive the monochrome LED.
 */
inline YLed *yFindLed(const string& func)
{ return YLed::FindLed(func);}
/**
 * Starts the enumeration of monochrome LEDs currently accessible.
 * Use the method YLed::nextLed() to iterate on
 * next monochrome LEDs.
 *
 * @return a pointer to a YLed object, corresponding to
 *         the first monochrome LED currently online, or a NULL pointer
 *         if there are none.
 */
inline YLed *yFirstLed(void)
{ return YLed::FirstLed();}

//--- (end of YLed functions declaration)

#ifdef YOCTOLIB_NAMESPACE
// end of namespace definition
}
#endif

#endif
