/*********************************************************************
 *
 * $Id: yocto_bridgecontrol.h 27704 2017-06-01 12:32:11Z seb $
 *
 * Declares yFindBridgeControl(), the high-level API for BridgeControl functions
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


#ifndef YOCTO_BRIDGECONTROL_H
#define YOCTO_BRIDGECONTROL_H

#include "yocto_api.h"
#include <cfloat>
#include <cmath>
#include <map>

//--- (YBridgeControl return codes)
//--- (end of YBridgeControl return codes)
//--- (YBridgeControl definitions)
class YBridgeControl; // forward declaration

typedef void (*YBridgeControlValueCallback)(YBridgeControl *func, const string& functionValue);
#ifndef _Y_EXCITATIONMODE_ENUM
#define _Y_EXCITATIONMODE_ENUM
typedef enum {
    Y_EXCITATIONMODE_INTERNAL_AC = 0,
    Y_EXCITATIONMODE_INTERNAL_DC = 1,
    Y_EXCITATIONMODE_EXTERNAL_DC = 2,
    Y_EXCITATIONMODE_INVALID = -1,
} Y_EXCITATIONMODE_enum;
#endif
#define Y_BRIDGELATENCY_INVALID         (YAPI_INVALID_UINT)
#define Y_ADVALUE_INVALID               (YAPI_INVALID_INT)
#define Y_ADGAIN_INVALID                (YAPI_INVALID_UINT)
//--- (end of YBridgeControl definitions)

//--- (YBridgeControl declaration)
/**
 * YBridgeControl Class: BridgeControl function interface
 *
 * The Yoctopuce class YBridgeControl allows you to control bridge excitation parameters
 * and measure parameters for a Wheatstone bridge sensor. To read the measurements, it
 * is best to use the GenericSensor calss, which will compute the measured value
 * in the optimal way.
 */
class YOCTO_CLASS_EXPORT YBridgeControl: public YFunction {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of YBridgeControl declaration)
protected:
    //--- (YBridgeControl attributes)
    // Attributes (function value cache)
    Y_EXCITATIONMODE_enum _excitationMode;
    int             _bridgeLatency;
    int             _adValue;
    int             _adGain;
    YBridgeControlValueCallback _valueCallbackBridgeControl;

    friend YBridgeControl *yFindBridgeControl(const string& func);
    friend YBridgeControl *yFirstBridgeControl(void);

    // Function-specific method for parsing of JSON output and caching result
    virtual int     _parseAttr(YJSONObject* json_val);

    // Constructor is protected, use yFindBridgeControl factory function to instantiate
    YBridgeControl(const string& func);
    //--- (end of YBridgeControl attributes)

public:
    ~YBridgeControl();
    //--- (YBridgeControl accessors declaration)

    static const Y_EXCITATIONMODE_enum EXCITATIONMODE_INTERNAL_AC = Y_EXCITATIONMODE_INTERNAL_AC;
    static const Y_EXCITATIONMODE_enum EXCITATIONMODE_INTERNAL_DC = Y_EXCITATIONMODE_INTERNAL_DC;
    static const Y_EXCITATIONMODE_enum EXCITATIONMODE_EXTERNAL_DC = Y_EXCITATIONMODE_EXTERNAL_DC;
    static const Y_EXCITATIONMODE_enum EXCITATIONMODE_INVALID = Y_EXCITATIONMODE_INVALID;
    static const int BRIDGELATENCY_INVALID = YAPI_INVALID_UINT;
    static const int ADVALUE_INVALID = YAPI_INVALID_INT;
    static const int ADGAIN_INVALID = YAPI_INVALID_UINT;

    /**
     * Returns the current Wheatstone bridge excitation method.
     *
     * @return a value among Y_EXCITATIONMODE_INTERNAL_AC, Y_EXCITATIONMODE_INTERNAL_DC and
     * Y_EXCITATIONMODE_EXTERNAL_DC corresponding to the current Wheatstone bridge excitation method
     *
     * On failure, throws an exception or returns Y_EXCITATIONMODE_INVALID.
     */
    Y_EXCITATIONMODE_enum get_excitationMode(void);

    inline Y_EXCITATIONMODE_enum excitationMode(void)
    { return this->get_excitationMode(); }

    /**
     * Changes the current Wheatstone bridge excitation method.
     *
     * @param newval : a value among Y_EXCITATIONMODE_INTERNAL_AC, Y_EXCITATIONMODE_INTERNAL_DC and
     * Y_EXCITATIONMODE_EXTERNAL_DC corresponding to the current Wheatstone bridge excitation method
     *
     * @return YAPI_SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_excitationMode(Y_EXCITATIONMODE_enum newval);
    inline int      setExcitationMode(Y_EXCITATIONMODE_enum newval)
    { return this->set_excitationMode(newval); }

    /**
     * Returns the current Wheatstone bridge excitation method.
     *
     * @return an integer corresponding to the current Wheatstone bridge excitation method
     *
     * On failure, throws an exception or returns Y_BRIDGELATENCY_INVALID.
     */
    int                 get_bridgeLatency(void);

    inline int          bridgeLatency(void)
    { return this->get_bridgeLatency(); }

    /**
     * Changes the current Wheatstone bridge excitation method.
     *
     * @param newval : an integer corresponding to the current Wheatstone bridge excitation method
     *
     * @return YAPI_SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_bridgeLatency(int newval);
    inline int      setBridgeLatency(int newval)
    { return this->set_bridgeLatency(newval); }

    /**
     * Returns the raw value returned by the ratiometric A/D converter
     * during last read.
     *
     * @return an integer corresponding to the raw value returned by the ratiometric A/D converter
     *         during last read
     *
     * On failure, throws an exception or returns Y_ADVALUE_INVALID.
     */
    int                 get_adValue(void);

    inline int          adValue(void)
    { return this->get_adValue(); }

    /**
     * Returns the current ratiometric A/D converter gain. The gain is automatically
     * configured according to the signalRange set in the corresponding genericSensor.
     *
     * @return an integer corresponding to the current ratiometric A/D converter gain
     *
     * On failure, throws an exception or returns Y_ADGAIN_INVALID.
     */
    int                 get_adGain(void);

    inline int          adGain(void)
    { return this->get_adGain(); }

    /**
     * Retrieves a Wheatstone bridge controller for a given identifier.
     * The identifier can be specified using several formats:
     * <ul>
     * <li>FunctionLogicalName</li>
     * <li>ModuleSerialNumber.FunctionIdentifier</li>
     * <li>ModuleSerialNumber.FunctionLogicalName</li>
     * <li>ModuleLogicalName.FunctionIdentifier</li>
     * <li>ModuleLogicalName.FunctionLogicalName</li>
     * </ul>
     *
     * This function does not require that the Wheatstone bridge controller is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method YBridgeControl.isOnline() to test if the Wheatstone bridge controller is
     * indeed online at a given time. In case of ambiguity when looking for
     * a Wheatstone bridge controller by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     *
     * If a call to this object's is_online() method returns FALSE although
     * you are certain that the matching device is plugged, make sure that you did
     * call registerHub() at application initialization time.
     *
     * @param func : a string that uniquely characterizes the Wheatstone bridge controller
     *
     * @return a YBridgeControl object allowing you to drive the Wheatstone bridge controller.
     */
    static YBridgeControl* FindBridgeControl(string func);

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
    virtual int         registerValueCallback(YBridgeControlValueCallback callback);
    using YFunction::registerValueCallback;

    virtual int         _invokeValueCallback(string value);


    inline static YBridgeControl* Find(string func)
    { return YBridgeControl::FindBridgeControl(func); }

    /**
     * Continues the enumeration of Wheatstone bridge controllers started using yFirstBridgeControl().
     *
     * @return a pointer to a YBridgeControl object, corresponding to
     *         a Wheatstone bridge controller currently online, or a NULL pointer
     *         if there are no more Wheatstone bridge controllers to enumerate.
     */
           YBridgeControl  *nextBridgeControl(void);
    inline YBridgeControl  *next(void)
    { return this->nextBridgeControl();}

    /**
     * Starts the enumeration of Wheatstone bridge controllers currently accessible.
     * Use the method YBridgeControl.nextBridgeControl() to iterate on
     * next Wheatstone bridge controllers.
     *
     * @return a pointer to a YBridgeControl object, corresponding to
     *         the first Wheatstone bridge controller currently online, or a NULL pointer
     *         if there are none.
     */
           static YBridgeControl* FirstBridgeControl(void);
    inline static YBridgeControl* First(void)
    { return YBridgeControl::FirstBridgeControl();}
#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of YBridgeControl accessors declaration)
};

//--- (BridgeControl functions declaration)

/**
 * Retrieves a Wheatstone bridge controller for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that the Wheatstone bridge controller is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method YBridgeControl.isOnline() to test if the Wheatstone bridge controller is
 * indeed online at a given time. In case of ambiguity when looking for
 * a Wheatstone bridge controller by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the Wheatstone bridge controller
 *
 * @return a YBridgeControl object allowing you to drive the Wheatstone bridge controller.
 */
inline YBridgeControl* yFindBridgeControl(const string& func)
{ return YBridgeControl::FindBridgeControl(func);}
/**
 * Starts the enumeration of Wheatstone bridge controllers currently accessible.
 * Use the method YBridgeControl.nextBridgeControl() to iterate on
 * next Wheatstone bridge controllers.
 *
 * @return a pointer to a YBridgeControl object, corresponding to
 *         the first Wheatstone bridge controller currently online, or a NULL pointer
 *         if there are none.
 */
inline YBridgeControl* yFirstBridgeControl(void)
{ return YBridgeControl::FirstBridgeControl();}

//--- (end of BridgeControl functions declaration)

#endif
