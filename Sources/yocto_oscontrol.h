/*********************************************************************
 *
 * $Id: yocto_oscontrol.h 12337 2013-08-14 15:22:22Z mvuilleu $
 *
 * Declares yFindOsControl(), the high-level API for OsControl functions
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


#ifndef YOCTO_OSCONTROL_H
#define YOCTO_OSCONTROL_H

#include "yocto_api.h"
#include <cfloat>
#include <cmath>
#include <map>

//--- (return codes)
//--- (end of return codes)
//--- (YOsControl definitions)
class YOsControl; //forward declaration

typedef void (*YOsControlUpdateCallback)(YOsControl *func, const string& functionValue);
#define Y_LOGICALNAME_INVALID           (YAPI::INVALID_STRING)
#define Y_ADVERTISEDVALUE_INVALID       (YAPI::INVALID_STRING)
#define Y_SHUTDOWNCOUNTDOWN_INVALID     (0xffffffff)
//--- (end of YOsControl definitions)

//--- (YOsControl declaration)
/**
 * YOsControl Class: OS control
 * 
 * The OScontrol object allows some control over the operating system running a VirtualHub.
 * OsControl is available on the VirtualHub software only. This feature must be activated at the VirtualHub
 * start up with -o option.
 */
class YOsControl: public YFunction {
protected:
    // Attributes (function value cache)
    YOsControlUpdateCallback _callback;
    string          _logicalName;
    string          _advertisedValue;
    unsigned        _shutdownCountdown;

    friend YOsControl *yFindOsControl(const string& func);
    friend YOsControl *yFirstOsControl(void);

    // Function-specific method for parsing of JSON output and caching result
    int             _parse(yJsonStateMachine& j);
    //--- (end of YOsControl declaration)

    //--- (YOsControl constructor)
    // Constructor is protected, use yFindOsControl factory function to instantiate
    YOsControl(const string& func);
    //--- (end of YOsControl constructor)
    //--- (OsControl initialization)
    //--- (end of OsControl initialization)

public:
    ~YOsControl();
    //--- (YOsControl accessors declaration)

    static const string LOGICALNAME_INVALID;
    static const string ADVERTISEDVALUE_INVALID;
    static const unsigned SHUTDOWNCOUNTDOWN_INVALID = 0xffffffff;

    /**
     * Returns the logical name of the OS control, corresponding to the network name of the module.
     * 
     * @return a string corresponding to the logical name of the OS control, corresponding to the network
     * name of the module
     * 
     * On failure, throws an exception or returns Y_LOGICALNAME_INVALID.
     */
           string          get_logicalName(void);
    inline string          logicalName(void)
    { return this->get_logicalName(); }

    /**
     * Changes the logical name of the OS control. You can use yCheckLogicalName()
     * prior to this call to make sure that your parameter is valid.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     * 
     * @param newval : a string corresponding to the logical name of the OS control
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_logicalName(const string& newval);
    inline int      setLogicalName(const string& newval)
    { return this->set_logicalName(newval); }

    /**
     * Returns the current value of the OS control (no more than 6 characters).
     * 
     * @return a string corresponding to the current value of the OS control (no more than 6 characters)
     * 
     * On failure, throws an exception or returns Y_ADVERTISEDVALUE_INVALID.
     */
           string          get_advertisedValue(void);
    inline string          advertisedValue(void)
    { return this->get_advertisedValue(); }

    /**
     * Returns the remaining number of seconds before the OS shutdown, or zero when no
     * shutdown has been scheduled.
     * 
     * @return an integer corresponding to the remaining number of seconds before the OS shutdown, or zero when no
     *         shutdown has been scheduled
     * 
     * On failure, throws an exception or returns Y_SHUTDOWNCOUNTDOWN_INVALID.
     */
           unsigned        get_shutdownCountdown(void);
    inline unsigned        shutdownCountdown(void)
    { return this->get_shutdownCountdown(); }

    int             set_shutdownCountdown(unsigned newval);
    inline int      setShutdownCountdown(unsigned newval)
    { return this->set_shutdownCountdown(newval); }

    /**
     * Schedules an OS shutdown after a given number of seconds.
     * 
     * @param secBeforeShutDown : number of seconds before shutdown
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             shutdown(int secBeforeShutDown);


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
    void registerValueCallback(YOsControlUpdateCallback callback);

    void advertiseValue(const string& value);

    /**
     * Continues the enumeration of OS control started using yFirstOsControl().
     * 
     * @return a pointer to a YOsControl object, corresponding to
     *         OS control currently online, or a null pointer
     *         if there are no more OS control to enumerate.
     */
           YOsControl      *nextOsControl(void);
    inline YOsControl      *next(void)
    { return this->nextOsControl();}

    /**
     * Retrieves OS control for a given identifier.
     * The identifier can be specified using several formats:
     * <ul>
     * <li>FunctionLogicalName</li>
     * <li>ModuleSerialNumber.FunctionIdentifier</li>
     * <li>ModuleSerialNumber.FunctionLogicalName</li>
     * <li>ModuleLogicalName.FunctionIdentifier</li>
     * <li>ModuleLogicalName.FunctionLogicalName</li>
     * </ul>
     * 
     * This function does not require that the OS control is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method YOsControl.isOnline() to test if the OS control is
     * indeed online at a given time. In case of ambiguity when looking for
     * OS control by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     * 
     * @param func : a string that uniquely characterizes the OS control
     * 
     * @return a YOsControl object allowing you to drive the OS control.
     */
           static YOsControl* FindOsControl(const string& func);
    inline static YOsControl* Find(const string& func)
    { return YOsControl::FindOsControl(func);}
    /**
     * Starts the enumeration of OS control currently accessible.
     * Use the method YOsControl.nextOsControl() to iterate on
     * next OS control.
     * 
     * @return a pointer to a YOsControl object, corresponding to
     *         the first OS control currently online, or a null pointer
     *         if there are none.
     */
           static YOsControl* FirstOsControl(void);
    inline static YOsControl* First(void)
    { return YOsControl::FirstOsControl();}
    //--- (end of YOsControl accessors declaration)
};

//--- (OsControl functions declaration)

/**
 * Retrieves OS control for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 * 
 * This function does not require that the OS control is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method YOsControl.isOnline() to test if the OS control is
 * indeed online at a given time. In case of ambiguity when looking for
 * OS control by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 * 
 * @param func : a string that uniquely characterizes the OS control
 * 
 * @return a YOsControl object allowing you to drive the OS control.
 */
inline YOsControl* yFindOsControl(const string& func)
{ return YOsControl::FindOsControl(func);}
/**
 * Starts the enumeration of OS control currently accessible.
 * Use the method YOsControl.nextOsControl() to iterate on
 * next OS control.
 * 
 * @return a pointer to a YOsControl object, corresponding to
 *         the first OS control currently online, or a null pointer
 *         if there are none.
 */
inline YOsControl* yFirstOsControl(void)
{ return YOsControl::FirstOsControl();}

//--- (end of OsControl functions declaration)

#endif
