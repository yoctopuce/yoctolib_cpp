/*********************************************************************
 *
 * $Id: yocto_anbutton.h 9898 2013-02-19 13:19:20Z mvuilleu $
 *
 * Declares yFindAnButton(), the high-level API for AnButton functions
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


#ifndef YOCTO_ANBUTTON_H
#define YOCTO_ANBUTTON_H

#include "yocto_api.h"
#include <cfloat>
#include <cmath>
#include <map>

//--- (return codes)
//--- (end of return codes)
//--- (YAnButton definitions)
class YAnButton; //forward declaration

typedef void (*YAnButtonUpdateCallback)(YAnButton *func, const string& functionValue);
typedef enum {
    Y_ANALOGCALIBRATION_OFF = 0,
    Y_ANALOGCALIBRATION_ON = 1,
    Y_ANALOGCALIBRATION_INVALID = -1,
} Y_ANALOGCALIBRATION_enum;

typedef enum {
    Y_ISPRESSED_FALSE = 0,
    Y_ISPRESSED_TRUE = 1,
    Y_ISPRESSED_INVALID = -1,
} Y_ISPRESSED_enum;

#define Y_LOGICALNAME_INVALID           (YAPI::INVALID_STRING)
#define Y_ADVERTISEDVALUE_INVALID       (YAPI::INVALID_STRING)
#define Y_CALIBRATEDVALUE_INVALID       (0xffffffff)
#define Y_RAWVALUE_INVALID              (0xffffffff)
#define Y_CALIBRATIONMAX_INVALID        (0xffffffff)
#define Y_CALIBRATIONMIN_INVALID        (0xffffffff)
#define Y_SENSITIVITY_INVALID           (0xffffffff)
#define Y_LASTTIMEPRESSED_INVALID       (0xffffffff)
#define Y_LASTTIMERELEASED_INVALID      (0xffffffff)
#define Y_PULSECOUNTER_INVALID          (0xffffffff)
#define Y_PULSETIMER_INVALID            (0xffffffff)
//--- (end of YAnButton definitions)

//--- (YAnButton declaration)
/**
 * YAnButton Class: AnButton function interface
 * 
 * Yoctopuce application programming interface allows you to measure the state
 * of a simple button as well as to read an analog potentiometer (variable resistance).
 * This can be use for instance with a continuous rotating knob, a throttle grip
 * or a joystick. The module is capable to calibrate itself on min and max values,
 * in order to compute a calibrated value that varies proportionally with the
 * potentiometer position, regardless of its total resistance.
 */
class YAnButton: public YFunction {
protected:
    // Attributes (function value cache)
    YAnButtonUpdateCallback _callback;
    string          _logicalName;
    string          _advertisedValue;
    unsigned        _calibratedValue;
    unsigned        _rawValue;
    Y_ANALOGCALIBRATION_enum _analogCalibration;
    unsigned        _calibrationMax;
    unsigned        _calibrationMin;
    unsigned        _sensitivity;
    Y_ISPRESSED_enum _isPressed;
    unsigned        _lastTimePressed;
    unsigned        _lastTimeReleased;
    unsigned        _pulseCounter;
    unsigned        _pulseTimer;
    // Static function object cache
    static std::map<string,YAnButton*> _AnButtonCache;

    friend YAnButton *yFindAnButton(const string& func);
    friend YAnButton *yFirstAnButton(void);

    // Function-specific method for parsing of JSON output and caching result
    int             _parse(yJsonStateMachine& j);
    //--- (end of YAnButton declaration)

    //--- (YAnButton constructor)
    // Constructor is protected, use yFindAnButton factory function to instantiate
    YAnButton(const string& func): YFunction("AnButton", func)
    //--- (end of YAnButton constructor)
    //--- (AnButton initialization)
            ,_callback(NULL)
            ,_logicalName(Y_LOGICALNAME_INVALID)
            ,_advertisedValue(Y_ADVERTISEDVALUE_INVALID)
            ,_calibratedValue(Y_CALIBRATEDVALUE_INVALID)
            ,_rawValue(Y_RAWVALUE_INVALID)
            ,_analogCalibration(Y_ANALOGCALIBRATION_INVALID)
            ,_calibrationMax(Y_CALIBRATIONMAX_INVALID)
            ,_calibrationMin(Y_CALIBRATIONMIN_INVALID)
            ,_sensitivity(Y_SENSITIVITY_INVALID)
            ,_isPressed(Y_ISPRESSED_INVALID)
            ,_lastTimePressed(Y_LASTTIMEPRESSED_INVALID)
            ,_lastTimeReleased(Y_LASTTIMERELEASED_INVALID)
            ,_pulseCounter(Y_PULSECOUNTER_INVALID)
            ,_pulseTimer(Y_PULSETIMER_INVALID)
    //--- (end of AnButton initialization)
    {};

public:
    //--- (YAnButton accessors declaration)

    static const string LOGICALNAME_INVALID;
    static const string ADVERTISEDVALUE_INVALID;
    static const unsigned CALIBRATEDVALUE_INVALID = 0xffffffff;
    static const unsigned RAWVALUE_INVALID = 0xffffffff;
    static const Y_ANALOGCALIBRATION_enum ANALOGCALIBRATION_OFF = Y_ANALOGCALIBRATION_OFF;
    static const Y_ANALOGCALIBRATION_enum ANALOGCALIBRATION_ON = Y_ANALOGCALIBRATION_ON;
    static const Y_ANALOGCALIBRATION_enum ANALOGCALIBRATION_INVALID = Y_ANALOGCALIBRATION_INVALID;
    static const unsigned CALIBRATIONMAX_INVALID = 0xffffffff;
    static const unsigned CALIBRATIONMIN_INVALID = 0xffffffff;
    static const unsigned SENSITIVITY_INVALID = 0xffffffff;
    static const Y_ISPRESSED_enum ISPRESSED_FALSE = Y_ISPRESSED_FALSE;
    static const Y_ISPRESSED_enum ISPRESSED_TRUE = Y_ISPRESSED_TRUE;
    static const Y_ISPRESSED_enum ISPRESSED_INVALID = Y_ISPRESSED_INVALID;
    static const unsigned LASTTIMEPRESSED_INVALID = 0xffffffff;
    static const unsigned LASTTIMERELEASED_INVALID = 0xffffffff;
    static const unsigned PULSECOUNTER_INVALID = 0xffffffff;
    static const unsigned PULSETIMER_INVALID = 0xffffffff;

    /**
     * Returns the logical name of the analog input.
     * 
     * @return a string corresponding to the logical name of the analog input
     * 
     * On failure, throws an exception or returns Y_LOGICALNAME_INVALID.
     */
           string          get_logicalName(void);
    inline string          logicalName(void)
    { return this->get_logicalName(); }

    /**
     * Changes the logical name of the analog input. You can use yCheckLogicalName()
     * prior to this call to make sure that your parameter is valid.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     * 
     * @param newval : a string corresponding to the logical name of the analog input
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_logicalName(const string& newval);
    inline int      setLogicalName(const string& newval)
    { return this->set_logicalName(newval); }

    /**
     * Returns the current value of the analog input (no more than 6 characters).
     * 
     * @return a string corresponding to the current value of the analog input (no more than 6 characters)
     * 
     * On failure, throws an exception or returns Y_ADVERTISEDVALUE_INVALID.
     */
           string          get_advertisedValue(void);
    inline string          advertisedValue(void)
    { return this->get_advertisedValue(); }

    /**
     * Returns the current calibrated input value (between 0 and 1000, included).
     * 
     * @return an integer corresponding to the current calibrated input value (between 0 and 1000, included)
     * 
     * On failure, throws an exception or returns Y_CALIBRATEDVALUE_INVALID.
     */
           unsigned        get_calibratedValue(void);
    inline unsigned        calibratedValue(void)
    { return this->get_calibratedValue(); }

    /**
     * Returns the current measured input value as-is (between 0 and 4095, included).
     * 
     * @return an integer corresponding to the current measured input value as-is (between 0 and 4095, included)
     * 
     * On failure, throws an exception or returns Y_RAWVALUE_INVALID.
     */
           unsigned        get_rawValue(void);
    inline unsigned        rawValue(void)
    { return this->get_rawValue(); }

    /**
     * Tells if a calibration process is currently ongoing.
     * 
     * @return either Y_ANALOGCALIBRATION_OFF or Y_ANALOGCALIBRATION_ON
     * 
     * On failure, throws an exception or returns Y_ANALOGCALIBRATION_INVALID.
     */
           Y_ANALOGCALIBRATION_enum get_analogCalibration(void);
    inline Y_ANALOGCALIBRATION_enum analogCalibration(void)
    { return this->get_analogCalibration(); }

    /**
     * Starts or stops the calibration process. Remember to call the saveToFlash()
     * method of the module at the end of the calibration if the modification must be kept.
     * 
     * @param newval : either Y_ANALOGCALIBRATION_OFF or Y_ANALOGCALIBRATION_ON
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_analogCalibration(Y_ANALOGCALIBRATION_enum newval);
    inline int      setAnalogCalibration(Y_ANALOGCALIBRATION_enum newval)
    { return this->set_analogCalibration(newval); }

    /**
     * Returns the maximal value measured during the calibration (between 0 and 4095, included).
     * 
     * @return an integer corresponding to the maximal value measured during the calibration (between 0
     * and 4095, included)
     * 
     * On failure, throws an exception or returns Y_CALIBRATIONMAX_INVALID.
     */
           unsigned        get_calibrationMax(void);
    inline unsigned        calibrationMax(void)
    { return this->get_calibrationMax(); }

    /**
     * Changes the maximal calibration value for the input (between 0 and 4095, included), without actually
     * starting the automated calibration.  Remember to call the saveToFlash()
     * method of the module if the modification must be kept.
     * 
     * @param newval : an integer corresponding to the maximal calibration value for the input (between 0
     * and 4095, included), without actually
     *         starting the automated calibration
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_calibrationMax(unsigned newval);
    inline int      setCalibrationMax(unsigned newval)
    { return this->set_calibrationMax(newval); }

    /**
     * Returns the minimal value measured during the calibration (between 0 and 4095, included).
     * 
     * @return an integer corresponding to the minimal value measured during the calibration (between 0
     * and 4095, included)
     * 
     * On failure, throws an exception or returns Y_CALIBRATIONMIN_INVALID.
     */
           unsigned        get_calibrationMin(void);
    inline unsigned        calibrationMin(void)
    { return this->get_calibrationMin(); }

    /**
     * Changes the minimal calibration value for the input (between 0 and 4095, included), without actually
     * starting the automated calibration.  Remember to call the saveToFlash()
     * method of the module if the modification must be kept.
     * 
     * @param newval : an integer corresponding to the minimal calibration value for the input (between 0
     * and 4095, included), without actually
     *         starting the automated calibration
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_calibrationMin(unsigned newval);
    inline int      setCalibrationMin(unsigned newval)
    { return this->set_calibrationMin(newval); }

    /**
     * Returns the sensibility for the input (between 1 and 255, included) for triggering user callbacks.
     * 
     * @return an integer corresponding to the sensibility for the input (between 1 and 255, included) for
     * triggering user callbacks
     * 
     * On failure, throws an exception or returns Y_SENSITIVITY_INVALID.
     */
           unsigned        get_sensitivity(void);
    inline unsigned        sensitivity(void)
    { return this->get_sensitivity(); }

    /**
     * Changes the sensibility for the input (between 1 and 255, included) for triggering user callbacks.
     * Remember to call the saveToFlash() method of the module if the modification must be kept.
     * 
     * @param newval : an integer corresponding to the sensibility for the input (between 1 and 255,
     * included) for triggering user callbacks
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_sensitivity(unsigned newval);
    inline int      setSensitivity(unsigned newval)
    { return this->set_sensitivity(newval); }

    /**
     * Returns true if the input (considered as binary) is active (closed contact), and false otherwise.
     * 
     * @return either Y_ISPRESSED_FALSE or Y_ISPRESSED_TRUE, according to true if the input (considered as
     * binary) is active (closed contact), and false otherwise
     * 
     * On failure, throws an exception or returns Y_ISPRESSED_INVALID.
     */
           Y_ISPRESSED_enum get_isPressed(void);
    inline Y_ISPRESSED_enum isPressed(void)
    { return this->get_isPressed(); }

    /**
     * Returns the number of elapsed milliseconds between the module power on and the last time
     * the input button was pressed (the input contact transitionned from open to closed).
     * 
     * @return an integer corresponding to the number of elapsed milliseconds between the module power on
     * and the last time
     *         the input button was pressed (the input contact transitionned from open to closed)
     * 
     * On failure, throws an exception or returns Y_LASTTIMEPRESSED_INVALID.
     */
           unsigned        get_lastTimePressed(void);
    inline unsigned        lastTimePressed(void)
    { return this->get_lastTimePressed(); }

    /**
     * Returns the number of elapsed milliseconds between the module power on and the last time
     * the input button was released (the input contact transitionned from closed to open).
     * 
     * @return an integer corresponding to the number of elapsed milliseconds between the module power on
     * and the last time
     *         the input button was released (the input contact transitionned from closed to open)
     * 
     * On failure, throws an exception or returns Y_LASTTIMERELEASED_INVALID.
     */
           unsigned        get_lastTimeReleased(void);
    inline unsigned        lastTimeReleased(void)
    { return this->get_lastTimeReleased(); }

    /**
     * Returns the pulse counter value
     * 
     * @return an integer corresponding to the pulse counter value
     * 
     * On failure, throws an exception or returns Y_PULSECOUNTER_INVALID.
     */
           unsigned        get_pulseCounter(void);
    inline unsigned        pulseCounter(void)
    { return this->get_pulseCounter(); }

    int             set_pulseCounter(unsigned newval);
    inline int      setPulseCounter(unsigned newval)
    { return this->set_pulseCounter(newval); }

    /**
     * Returns the pulse counter value as well as his timer
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             resetCounter(void);

    /**
     * Returns the timer of the pulses counter (ms)
     * 
     * @return an integer corresponding to the timer of the pulses counter (ms)
     * 
     * On failure, throws an exception or returns Y_PULSETIMER_INVALID.
     */
           unsigned        get_pulseTimer(void);
    inline unsigned        pulseTimer(void)
    { return this->get_pulseTimer(); }


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
    void registerValueCallback(YAnButtonUpdateCallback callback);

    void advertiseValue(const string& value);

    /**
     * Continues the enumeration of analog inputs started using yFirstAnButton().
     * 
     * @return a pointer to a YAnButton object, corresponding to
     *         an analog input currently online, or a null pointer
     *         if there are no more analog inputs to enumerate.
     */
           YAnButton       *nextAnButton(void);
    inline YAnButton       *next(void)
    { return this->nextAnButton();}

    /**
     * Retrieves an analog input for a given identifier.
     * The identifier can be specified using several formats:
     * <ul>
     * <li>FunctionLogicalName</li>
     * <li>ModuleSerialNumber.FunctionIdentifier</li>
     * <li>ModuleSerialNumber.FunctionLogicalName</li>
     * <li>ModuleLogicalName.FunctionIdentifier</li>
     * <li>ModuleLogicalName.FunctionLogicalName</li>
     * </ul>
     * 
     * This function does not require that the analog input is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method YAnButton.isOnline() to test if the analog input is
     * indeed online at a given time. In case of ambiguity when looking for
     * an analog input by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     * 
     * @param func : a string that uniquely characterizes the analog input
     * 
     * @return a YAnButton object allowing you to drive the analog input.
     */
           static YAnButton* FindAnButton(const string& func);
    inline static YAnButton* Find(const string& func)
    { return YAnButton::FindAnButton(func);}
    /**
     * Starts the enumeration of analog inputs currently accessible.
     * Use the method YAnButton.nextAnButton() to iterate on
     * next analog inputs.
     * 
     * @return a pointer to a YAnButton object, corresponding to
     *         the first analog input currently online, or a null pointer
     *         if there are none.
     */
           static YAnButton* FirstAnButton(void);
    inline static YAnButton* First(void)
    { return YAnButton::FirstAnButton();}
    //--- (end of YAnButton accessors declaration)
};

//--- (AnButton functions declaration)

/**
 * Retrieves an analog input for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 * 
 * This function does not require that the analog input is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method YAnButton.isOnline() to test if the analog input is
 * indeed online at a given time. In case of ambiguity when looking for
 * an analog input by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 * 
 * @param func : a string that uniquely characterizes the analog input
 * 
 * @return a YAnButton object allowing you to drive the analog input.
 */
inline YAnButton* yFindAnButton(const string& func)
{ return YAnButton::FindAnButton(func);}
/**
 * Starts the enumeration of analog inputs currently accessible.
 * Use the method YAnButton.nextAnButton() to iterate on
 * next analog inputs.
 * 
 * @return a pointer to a YAnButton object, corresponding to
 *         the first analog input currently online, or a null pointer
 *         if there are none.
 */
inline YAnButton* yFirstAnButton(void)
{ return YAnButton::FirstAnButton();}

//--- (end of AnButton functions declaration)

#endif
