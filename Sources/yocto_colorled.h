/*********************************************************************
 *
 * $Id: yocto_colorled.h 12324 2013-08-13 15:10:31Z mvuilleu $
 *
 * Declares yFindColorLed(), the high-level API for ColorLed functions
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


#ifndef YOCTO_COLORLED_H
#define YOCTO_COLORLED_H

#include "yocto_api.h"
#include <cfloat>
#include <cmath>
#include <map>

//--- (return codes)
//--- (end of return codes)
//--- (YColorLed definitions)
class YColorLed; //forward declaration

typedef void (*YColorLedUpdateCallback)(YColorLed *func, const string& functionValue);
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
extern YMove YCOLORLED_INVALID_MOVE;
#define Y_LOGICALNAME_INVALID           (YAPI::INVALID_STRING)
#define Y_ADVERTISEDVALUE_INVALID       (YAPI::INVALID_STRING)
#define Y_RGBCOLOR_INVALID              (0xffffffff)
#define Y_HSLCOLOR_INVALID              (0xffffffff)
#define Y_RGBMOVE_INVALID               (&YCOLORLED_INVALID_MOVE)
#define Y_HSLMOVE_INVALID               (&YCOLORLED_INVALID_MOVE)
#define Y_RGBCOLORATPOWERON_INVALID     (0xffffffff)
//--- (end of YColorLed definitions)

//--- (YColorLed declaration)
/**
 * YColorLed Class: ColorLed function interface
 * 
 * Yoctopuce application programming interface
 * allows you to drive a color led using RGB coordinates as well as HSL coordinates.
 * The module performs all conversions form RGB to HSL automatically. It is then
 * self-evident to turn on a led with a given hue and to progressively vary its
 * saturation or lightness. If needed, you can find more information on the
 * difference between RGB and HSL in the section following this one.
 */
class YColorLed: public YFunction {
protected:
    // Attributes (function value cache)
    YColorLedUpdateCallback _callback;
    string          _logicalName;
    string          _advertisedValue;
    unsigned        _rgbColor;
    unsigned        _hslColor;
    YMove           _rgbMove;
    YMove           _hslMove;
    unsigned        _rgbColorAtPowerOn;

    friend YColorLed *yFindColorLed(const string& func);
    friend YColorLed *yFirstColorLed(void);

    // Function-specific method for parsing of JSON output and caching result
    int             _parse(yJsonStateMachine& j);
    //--- (end of YColorLed declaration)

    //--- (YColorLed constructor)
    // Constructor is protected, use yFindColorLed factory function to instantiate
    YColorLed(const string& func);
    //--- (end of YColorLed constructor)
    //--- (ColorLed initialization)
    //--- (end of ColorLed initialization)

public:
    ~YColorLed();
    //--- (YColorLed accessors declaration)

    static const string LOGICALNAME_INVALID;
    static const string ADVERTISEDVALUE_INVALID;
    static const unsigned RGBCOLOR_INVALID = 0xffffffff;
    static const unsigned HSLCOLOR_INVALID = 0xffffffff;
    static const unsigned RGBCOLORATPOWERON_INVALID = 0xffffffff;

    /**
     * Returns the logical name of the RGB led.
     * 
     * @return a string corresponding to the logical name of the RGB led
     * 
     * On failure, throws an exception or returns Y_LOGICALNAME_INVALID.
     */
           string          get_logicalName(void);
    inline string          logicalName(void)
    { return this->get_logicalName(); }

    /**
     * Changes the logical name of the RGB led. You can use yCheckLogicalName()
     * prior to this call to make sure that your parameter is valid.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     * 
     * @param newval : a string corresponding to the logical name of the RGB led
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_logicalName(const string& newval);
    inline int      setLogicalName(const string& newval)
    { return this->set_logicalName(newval); }

    /**
     * Returns the current value of the RGB led (no more than 6 characters).
     * 
     * @return a string corresponding to the current value of the RGB led (no more than 6 characters)
     * 
     * On failure, throws an exception or returns Y_ADVERTISEDVALUE_INVALID.
     */
           string          get_advertisedValue(void);
    inline string          advertisedValue(void)
    { return this->get_advertisedValue(); }

    /**
     * Returns the current RGB color of the led.
     * 
     * @return an integer corresponding to the current RGB color of the led
     * 
     * On failure, throws an exception or returns Y_RGBCOLOR_INVALID.
     */
           unsigned        get_rgbColor(void);
    inline unsigned        rgbColor(void)
    { return this->get_rgbColor(); }

    /**
     * Changes the current color of the led, using a RGB color. Encoding is done as follows: 0xRRGGBB.
     * 
     * @param newval : an integer corresponding to the current color of the led, using a RGB color
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_rgbColor(unsigned newval);
    inline int      setRgbColor(unsigned newval)
    { return this->set_rgbColor(newval); }

    /**
     * Returns the current HSL color of the led.
     * 
     * @return an integer corresponding to the current HSL color of the led
     * 
     * On failure, throws an exception or returns Y_HSLCOLOR_INVALID.
     */
           unsigned        get_hslColor(void);
    inline unsigned        hslColor(void)
    { return this->get_hslColor(); }

    /**
     * Changes the current color of the led, using a color HSL. Encoding is done as follows: 0xHHSSLL.
     * 
     * @param newval : an integer corresponding to the current color of the led, using a color HSL
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_hslColor(unsigned newval);
    inline int      setHslColor(unsigned newval)
    { return this->set_hslColor(newval); }

           const YMove     *get_rgbMove(void);
    inline const YMove     *rgbMove(void)
    { return this->get_rgbMove(); }

    int             set_rgbMove(const YMove * newval);
    inline int      setRgbMove(const YMove * newval)
    { return this->set_rgbMove(newval); }

    /**
     * Performs a smooth transition in the RGB color space between the current color and a target color.
     * 
     * @param rgb_target  : desired RGB color at the end of the transition
     * @param ms_duration : duration of the transition, in millisecond
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             rgbMove(int rgb_target,int ms_duration);

           const YMove     *get_hslMove(void);
    inline const YMove     *hslMove(void)
    { return this->get_hslMove(); }

    int             set_hslMove(const YMove * newval);
    inline int      setHslMove(const YMove * newval)
    { return this->set_hslMove(newval); }

    /**
     * Performs a smooth transition in the HSL color space between the current color and a target color.
     * 
     * @param hsl_target  : desired HSL color at the end of the transition
     * @param ms_duration : duration of the transition, in millisecond
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             hslMove(int hsl_target,int ms_duration);

    /**
     * Returns the configured color to be displayed when the module is turned on.
     * 
     * @return an integer corresponding to the configured color to be displayed when the module is turned on
     * 
     * On failure, throws an exception or returns Y_RGBCOLORATPOWERON_INVALID.
     */
           unsigned        get_rgbColorAtPowerOn(void);
    inline unsigned        rgbColorAtPowerOn(void)
    { return this->get_rgbColorAtPowerOn(); }

    /**
     * Changes the color that the led will display by default when the module is turned on.
     * This color will be displayed as soon as the module is powered on.
     * Remember to call the saveToFlash() method of the module if the
     * change should be kept.
     * 
     * @param newval : an integer corresponding to the color that the led will display by default when the
     * module is turned on
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_rgbColorAtPowerOn(unsigned newval);
    inline int      setRgbColorAtPowerOn(unsigned newval)
    { return this->set_rgbColorAtPowerOn(newval); }


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
    void registerValueCallback(YColorLedUpdateCallback callback);

    void advertiseValue(const string& value);

    /**
     * Continues the enumeration of RGB leds started using yFirstColorLed().
     * 
     * @return a pointer to a YColorLed object, corresponding to
     *         an RGB led currently online, or a null pointer
     *         if there are no more RGB leds to enumerate.
     */
           YColorLed       *nextColorLed(void);
    inline YColorLed       *next(void)
    { return this->nextColorLed();}

    /**
     * Retrieves an RGB led for a given identifier.
     * The identifier can be specified using several formats:
     * <ul>
     * <li>FunctionLogicalName</li>
     * <li>ModuleSerialNumber.FunctionIdentifier</li>
     * <li>ModuleSerialNumber.FunctionLogicalName</li>
     * <li>ModuleLogicalName.FunctionIdentifier</li>
     * <li>ModuleLogicalName.FunctionLogicalName</li>
     * </ul>
     * 
     * This function does not require that the RGB led is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method YColorLed.isOnline() to test if the RGB led is
     * indeed online at a given time. In case of ambiguity when looking for
     * an RGB led by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     * 
     * @param func : a string that uniquely characterizes the RGB led
     * 
     * @return a YColorLed object allowing you to drive the RGB led.
     */
           static YColorLed* FindColorLed(const string& func);
    inline static YColorLed* Find(const string& func)
    { return YColorLed::FindColorLed(func);}
    /**
     * Starts the enumeration of RGB leds currently accessible.
     * Use the method YColorLed.nextColorLed() to iterate on
     * next RGB leds.
     * 
     * @return a pointer to a YColorLed object, corresponding to
     *         the first RGB led currently online, or a null pointer
     *         if there are none.
     */
           static YColorLed* FirstColorLed(void);
    inline static YColorLed* First(void)
    { return YColorLed::FirstColorLed();}
    //--- (end of YColorLed accessors declaration)
};

//--- (ColorLed functions declaration)

/**
 * Retrieves an RGB led for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 * 
 * This function does not require that the RGB led is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method YColorLed.isOnline() to test if the RGB led is
 * indeed online at a given time. In case of ambiguity when looking for
 * an RGB led by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 * 
 * @param func : a string that uniquely characterizes the RGB led
 * 
 * @return a YColorLed object allowing you to drive the RGB led.
 */
inline YColorLed* yFindColorLed(const string& func)
{ return YColorLed::FindColorLed(func);}
/**
 * Starts the enumeration of RGB leds currently accessible.
 * Use the method YColorLed.nextColorLed() to iterate on
 * next RGB leds.
 * 
 * @return a pointer to a YColorLed object, corresponding to
 *         the first RGB led currently online, or a null pointer
 *         if there are none.
 */
inline YColorLed* yFirstColorLed(void)
{ return YColorLed::FirstColorLed();}

//--- (end of ColorLed functions declaration)

#endif
