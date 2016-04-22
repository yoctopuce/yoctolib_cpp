/*********************************************************************
 *
 * $Id: yocto_colorled.h 23577 2016-03-22 22:59:53Z mvuilleu $
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

//--- (YColorLed return codes)
//--- (end of YColorLed return codes)
//--- (YColorLed definitions)
class YColorLed; // forward declaration

typedef void (*YColorLedValueCallback)(YColorLed *func, const string& functionValue);
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
#define Y_RGBCOLOR_INVALID              (YAPI_INVALID_UINT)
#define Y_HSLCOLOR_INVALID              (YAPI_INVALID_UINT)
#define Y_RGBCOLORATPOWERON_INVALID     (YAPI_INVALID_UINT)
#define Y_BLINKSEQSIZE_INVALID          (YAPI_INVALID_UINT)
#define Y_BLINKSEQMAXSIZE_INVALID       (YAPI_INVALID_UINT)
#define Y_BLINKSEQSIGNATURE_INVALID     (YAPI_INVALID_UINT)
#define Y_COMMAND_INVALID               (YAPI_INVALID_STRING)
//--- (end of YColorLed definitions)

//--- (YColorLed declaration)
/**
 * YColorLed Class: ColorLed function interface
 *
 * The Yoctopuce application programming interface
 * allows you to drive a color LED using RGB coordinates as well as HSL coordinates.
 * The module performs all conversions form RGB to HSL automatically. It is then
 * self-evident to turn on a LED with a given hue and to progressively vary its
 * saturation or lightness. If needed, you can find more information on the
 * difference between RGB and HSL in the section following this one.
 */
class YOCTO_CLASS_EXPORT YColorLed: public YFunction {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of YColorLed declaration)
protected:
    //--- (YColorLed attributes)
    // Attributes (function value cache)
    int             _rgbColor;
    int             _hslColor;
    YMove           _rgbMove;
    YMove           _hslMove;
    int             _rgbColorAtPowerOn;
    int             _blinkSeqSize;
    int             _blinkSeqMaxSize;
    int             _blinkSeqSignature;
    string          _command;
    YColorLedValueCallback _valueCallbackColorLed;

    friend YColorLed *yFindColorLed(const string& func);
    friend YColorLed *yFirstColorLed(void);

    // Function-specific method for parsing of JSON output and caching result
    virtual int     _parseAttr(yJsonStateMachine& j);

    // Constructor is protected, use yFindColorLed factory function to instantiate
    YColorLed(const string& func);
    //--- (end of YColorLed attributes)

public:
    ~YColorLed();
    //--- (YColorLed accessors declaration)

    static const int RGBCOLOR_INVALID = YAPI_INVALID_UINT;
    static const int HSLCOLOR_INVALID = YAPI_INVALID_UINT;
    static const YMove RGBMOVE_INVALID;
    static const YMove HSLMOVE_INVALID;
    static const int RGBCOLORATPOWERON_INVALID = YAPI_INVALID_UINT;
    static const int BLINKSEQSIZE_INVALID = YAPI_INVALID_UINT;
    static const int BLINKSEQMAXSIZE_INVALID = YAPI_INVALID_UINT;
    static const int BLINKSEQSIGNATURE_INVALID = YAPI_INVALID_UINT;
    static const string COMMAND_INVALID;

    /**
     * Returns the current RGB color of the LED.
     *
     * @return an integer corresponding to the current RGB color of the LED
     *
     * On failure, throws an exception or returns Y_RGBCOLOR_INVALID.
     */
    int                 get_rgbColor(void);

    inline int          rgbColor(void)
    { return this->get_rgbColor(); }

    /**
     * Changes the current color of the LED, using a RGB color. Encoding is done as follows: 0xRRGGBB.
     *
     * @param newval : an integer corresponding to the current color of the LED, using a RGB color
     *
     * @return YAPI_SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_rgbColor(int newval);
    inline int      setRgbColor(int newval)
    { return this->set_rgbColor(newval); }

    /**
     * Returns the current HSL color of the LED.
     *
     * @return an integer corresponding to the current HSL color of the LED
     *
     * On failure, throws an exception or returns Y_HSLCOLOR_INVALID.
     */
    int                 get_hslColor(void);

    inline int          hslColor(void)
    { return this->get_hslColor(); }

    /**
     * Changes the current color of the LED, using a color HSL. Encoding is done as follows: 0xHHSSLL.
     *
     * @param newval : an integer corresponding to the current color of the LED, using a color HSL
     *
     * @return YAPI_SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_hslColor(int newval);
    inline int      setHslColor(int newval)
    { return this->set_hslColor(newval); }

    YMove               get_rgbMove(void);

    inline YMove        rgbMove(void)
    { return this->get_rgbMove(); }

    int             set_rgbMove(YMove newval);
    inline int      setRgbMove(YMove newval)
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

    YMove               get_hslMove(void);

    inline YMove        hslMove(void)
    { return this->get_hslMove(); }

    int             set_hslMove(YMove newval);
    inline int      setHslMove(YMove newval)
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
    int                 get_rgbColorAtPowerOn(void);

    inline int          rgbColorAtPowerOn(void)
    { return this->get_rgbColorAtPowerOn(); }

    /**
     * Changes the color that the LED will display by default when the module is turned on.
     *
     * @param newval : an integer corresponding to the color that the LED will display by default when the
     * module is turned on
     *
     * @return YAPI_SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_rgbColorAtPowerOn(int newval);
    inline int      setRgbColorAtPowerOn(int newval)
    { return this->set_rgbColorAtPowerOn(newval); }

    /**
     * Returns the current length of the blinking sequence.
     *
     * @return an integer corresponding to the current length of the blinking sequence
     *
     * On failure, throws an exception or returns Y_BLINKSEQSIZE_INVALID.
     */
    int                 get_blinkSeqSize(void);

    inline int          blinkSeqSize(void)
    { return this->get_blinkSeqSize(); }

    /**
     * Returns the maximum length of the blinking sequence.
     *
     * @return an integer corresponding to the maximum length of the blinking sequence
     *
     * On failure, throws an exception or returns Y_BLINKSEQMAXSIZE_INVALID.
     */
    int                 get_blinkSeqMaxSize(void);

    inline int          blinkSeqMaxSize(void)
    { return this->get_blinkSeqMaxSize(); }

    /**
     * Return the blinking sequence signature. Since blinking
     * sequences cannot be read from the device, this can be used
     * to detect if a specific blinking sequence is already
     * programmed.
     *
     * @return an integer
     *
     * On failure, throws an exception or returns Y_BLINKSEQSIGNATURE_INVALID.
     */
    int                 get_blinkSeqSignature(void);

    inline int          blinkSeqSignature(void)
    { return this->get_blinkSeqSignature(); }

    string              get_command(void);

    inline string       command(void)
    { return this->get_command(); }

    int             set_command(const string& newval);
    inline int      setCommand(const string& newval)
    { return this->set_command(newval); }

    /**
     * Retrieves an RGB LED for a given identifier.
     * The identifier can be specified using several formats:
     * <ul>
     * <li>FunctionLogicalName</li>
     * <li>ModuleSerialNumber.FunctionIdentifier</li>
     * <li>ModuleSerialNumber.FunctionLogicalName</li>
     * <li>ModuleLogicalName.FunctionIdentifier</li>
     * <li>ModuleLogicalName.FunctionLogicalName</li>
     * </ul>
     *
     * This function does not require that the RGB LED is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method YColorLed.isOnline() to test if the RGB LED is
     * indeed online at a given time. In case of ambiguity when looking for
     * an RGB LED by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     *
     * @param func : a string that uniquely characterizes the RGB LED
     *
     * @return a YColorLed object allowing you to drive the RGB LED.
     */
    static YColorLed*   FindColorLed(string func);

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
    virtual int         registerValueCallback(YColorLedValueCallback callback);
    using YFunction::registerValueCallback;

    virtual int         _invokeValueCallback(string value);

    virtual int         sendCommand(string command);

    /**
     * Add a new transition to the blinking sequence, the move will
     * be performed in the HSL space.
     *
     * @param HSLcolor : desired HSL color when the traisntion is completed
     * @param msDelay : duration of the color transition, in milliseconds.
     *
     * @return YAPI_SUCCESS if the call succeeds.
     *         On failure, throws an exception or returns a negative error code.
     */
    virtual int         addHslMoveToBlinkSeq(int HSLcolor,int msDelay);

    /**
     * Add a new transition to the blinking sequence, the move will
     * be performed in the RGB space.
     *
     * @param RGBcolor : desired RGB color when the transition is completed
     * @param msDelay : duration of the color transition, in milliseconds.
     *
     * @return YAPI_SUCCESS if the call succeeds.
     *         On failure, throws an exception or returns a negative error code.
     */
    virtual int         addRgbMoveToBlinkSeq(int RGBcolor,int msDelay);

    /**
     * Starts the preprogrammed blinking sequence. The sequence will
     * run in loop until it is stopped by stopBlinkSeq or an explicit
     * change.
     *
     * @return YAPI_SUCCESS if the call succeeds.
     *         On failure, throws an exception or returns a negative error code.
     */
    virtual int         startBlinkSeq(void);

    /**
     * Stops the preprogrammed blinking sequence.
     *
     * @return YAPI_SUCCESS if the call succeeds.
     *         On failure, throws an exception or returns a negative error code.
     */
    virtual int         stopBlinkSeq(void);

    /**
     * Resets the preprogrammed blinking sequence.
     *
     * @return YAPI_SUCCESS if the call succeeds.
     *         On failure, throws an exception or returns a negative error code.
     */
    virtual int         resetBlinkSeq(void);


    inline static YColorLed* Find(string func)
    { return YColorLed::FindColorLed(func); }

    /**
     * Continues the enumeration of RGB LEDs started using yFirstColorLed().
     *
     * @return a pointer to a YColorLed object, corresponding to
     *         an RGB LED currently online, or a null pointer
     *         if there are no more RGB LEDs to enumerate.
     */
           YColorLed       *nextColorLed(void);
    inline YColorLed       *next(void)
    { return this->nextColorLed();}

    /**
     * Starts the enumeration of RGB LEDs currently accessible.
     * Use the method YColorLed.nextColorLed() to iterate on
     * next RGB LEDs.
     *
     * @return a pointer to a YColorLed object, corresponding to
     *         the first RGB LED currently online, or a null pointer
     *         if there are none.
     */
           static YColorLed* FirstColorLed(void);
    inline static YColorLed* First(void)
    { return YColorLed::FirstColorLed();}
#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of YColorLed accessors declaration)
};

//--- (ColorLed functions declaration)

/**
 * Retrieves an RGB LED for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that the RGB LED is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method YColorLed.isOnline() to test if the RGB LED is
 * indeed online at a given time. In case of ambiguity when looking for
 * an RGB LED by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * @param func : a string that uniquely characterizes the RGB LED
 *
 * @return a YColorLed object allowing you to drive the RGB LED.
 */
inline YColorLed* yFindColorLed(const string& func)
{ return YColorLed::FindColorLed(func);}
/**
 * Starts the enumeration of RGB LEDs currently accessible.
 * Use the method YColorLed.nextColorLed() to iterate on
 * next RGB LEDs.
 *
 * @return a pointer to a YColorLed object, corresponding to
 *         the first RGB LED currently online, or a null pointer
 *         if there are none.
 */
inline YColorLed* yFirstColorLed(void)
{ return YColorLed::FirstColorLed();}

//--- (end of ColorLed functions declaration)

#endif
