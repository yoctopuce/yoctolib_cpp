/*********************************************************************
 *
 * $Id: yocto_hubport.h 12337 2013-08-14 15:22:22Z mvuilleu $
 *
 * Declares yFindHubPort(), the high-level API for HubPort functions
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


#ifndef YOCTO_HUBPORT_H
#define YOCTO_HUBPORT_H

#include "yocto_api.h"
#include <cfloat>
#include <cmath>
#include <map>

//--- (return codes)
//--- (end of return codes)
//--- (YHubPort definitions)
class YHubPort; //forward declaration

typedef void (*YHubPortUpdateCallback)(YHubPort *func, const string& functionValue);
typedef enum {
    Y_ENABLED_FALSE = 0,
    Y_ENABLED_TRUE = 1,
    Y_ENABLED_INVALID = -1,
} Y_ENABLED_enum;

typedef enum {
    Y_PORTSTATE_OFF = 0,
    Y_PORTSTATE_OVRLD = 1,
    Y_PORTSTATE_ON = 2,
    Y_PORTSTATE_RUN = 3,
    Y_PORTSTATE_PROG = 4,
    Y_PORTSTATE_INVALID = -1,
} Y_PORTSTATE_enum;

#define Y_LOGICALNAME_INVALID           (YAPI::INVALID_STRING)
#define Y_ADVERTISEDVALUE_INVALID       (YAPI::INVALID_STRING)
#define Y_BAUDRATE_INVALID              (-1)
//--- (end of YHubPort definitions)

//--- (YHubPort declaration)
/**
 * YHubPort Class: Yocto-hub port interface
 * 
 * 
 */
class YHubPort: public YFunction {
protected:
    // Attributes (function value cache)
    YHubPortUpdateCallback _callback;
    string          _logicalName;
    string          _advertisedValue;
    Y_ENABLED_enum  _enabled;
    Y_PORTSTATE_enum _portState;
    int             _baudRate;

    friend YHubPort *yFindHubPort(const string& func);
    friend YHubPort *yFirstHubPort(void);

    // Function-specific method for parsing of JSON output and caching result
    int             _parse(yJsonStateMachine& j);
    //--- (end of YHubPort declaration)

    //--- (YHubPort constructor)
    // Constructor is protected, use yFindHubPort factory function to instantiate
    YHubPort(const string& func);
    //--- (end of YHubPort constructor)
    //--- (HubPort initialization)
    //--- (end of HubPort initialization)

public:
    ~YHubPort();
    //--- (YHubPort accessors declaration)

    static const string LOGICALNAME_INVALID;
    static const string ADVERTISEDVALUE_INVALID;
    static const Y_ENABLED_enum ENABLED_FALSE = Y_ENABLED_FALSE;
    static const Y_ENABLED_enum ENABLED_TRUE = Y_ENABLED_TRUE;
    static const Y_ENABLED_enum ENABLED_INVALID = Y_ENABLED_INVALID;
    static const Y_PORTSTATE_enum PORTSTATE_OFF = Y_PORTSTATE_OFF;
    static const Y_PORTSTATE_enum PORTSTATE_OVRLD = Y_PORTSTATE_OVRLD;
    static const Y_PORTSTATE_enum PORTSTATE_ON = Y_PORTSTATE_ON;
    static const Y_PORTSTATE_enum PORTSTATE_RUN = Y_PORTSTATE_RUN;
    static const Y_PORTSTATE_enum PORTSTATE_PROG = Y_PORTSTATE_PROG;
    static const Y_PORTSTATE_enum PORTSTATE_INVALID = Y_PORTSTATE_INVALID;
    static const int      BAUDRATE_INVALID = -1;

    /**
     * Returns the logical name of the Yocto-hub port, which is always the serial number of the
     * connected module.
     * 
     * @return a string corresponding to the logical name of the Yocto-hub port, which is always the
     * serial number of the
     *         connected module
     * 
     * On failure, throws an exception or returns Y_LOGICALNAME_INVALID.
     */
           string          get_logicalName(void);
    inline string          logicalName(void)
    { return this->get_logicalName(); }

    /**
     * It is not possible to configure the logical name of a Yocto-hub port. The logical
     * name is automatically set to the serial number of the connected module.
     * 
     * @param newval : a string
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_logicalName(const string& newval);
    inline int      setLogicalName(const string& newval)
    { return this->set_logicalName(newval); }

    /**
     * Returns the current value of the Yocto-hub port (no more than 6 characters).
     * 
     * @return a string corresponding to the current value of the Yocto-hub port (no more than 6 characters)
     * 
     * On failure, throws an exception or returns Y_ADVERTISEDVALUE_INVALID.
     */
           string          get_advertisedValue(void);
    inline string          advertisedValue(void)
    { return this->get_advertisedValue(); }

    /**
     * Returns true if the Yocto-hub port is powered, false otherwise.
     * 
     * @return either Y_ENABLED_FALSE or Y_ENABLED_TRUE, according to true if the Yocto-hub port is
     * powered, false otherwise
     * 
     * On failure, throws an exception or returns Y_ENABLED_INVALID.
     */
           Y_ENABLED_enum  get_enabled(void);
    inline Y_ENABLED_enum  enabled(void)
    { return this->get_enabled(); }

    /**
     * Changes the activation of the Yocto-hub port. If the port is enabled, the
     * *      connected module is powered. Otherwise, port power is shut down.
     * 
     * @param newval : either Y_ENABLED_FALSE or Y_ENABLED_TRUE, according to the activation of the Yocto-hub port
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_enabled(Y_ENABLED_enum newval);
    inline int      setEnabled(Y_ENABLED_enum newval)
    { return this->set_enabled(newval); }

    /**
     * Returns the current state of the Yocto-hub port.
     * 
     * @return a value among Y_PORTSTATE_OFF, Y_PORTSTATE_OVRLD, Y_PORTSTATE_ON, Y_PORTSTATE_RUN and
     * Y_PORTSTATE_PROG corresponding to the current state of the Yocto-hub port
     * 
     * On failure, throws an exception or returns Y_PORTSTATE_INVALID.
     */
           Y_PORTSTATE_enum get_portState(void);
    inline Y_PORTSTATE_enum portState(void)
    { return this->get_portState(); }

    /**
     * Returns the current baud rate used by this Yocto-hub port, in kbps.
     * The default value is 1000 kbps, but a slower rate may be used if communication
     * problems are encountered.
     * 
     * @return an integer corresponding to the current baud rate used by this Yocto-hub port, in kbps
     * 
     * On failure, throws an exception or returns Y_BAUDRATE_INVALID.
     */
           int             get_baudRate(void);
    inline int             baudRate(void)
    { return this->get_baudRate(); }


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
    void registerValueCallback(YHubPortUpdateCallback callback);

    void advertiseValue(const string& value);

    /**
     * Continues the enumeration of Yocto-hub ports started using yFirstHubPort().
     * 
     * @return a pointer to a YHubPort object, corresponding to
     *         a Yocto-hub port currently online, or a null pointer
     *         if there are no more Yocto-hub ports to enumerate.
     */
           YHubPort        *nextHubPort(void);
    inline YHubPort        *next(void)
    { return this->nextHubPort();}

    /**
     * Retrieves a Yocto-hub port for a given identifier.
     * The identifier can be specified using several formats:
     * <ul>
     * <li>FunctionLogicalName</li>
     * <li>ModuleSerialNumber.FunctionIdentifier</li>
     * <li>ModuleSerialNumber.FunctionLogicalName</li>
     * <li>ModuleLogicalName.FunctionIdentifier</li>
     * <li>ModuleLogicalName.FunctionLogicalName</li>
     * </ul>
     * 
     * This function does not require that the Yocto-hub port is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method YHubPort.isOnline() to test if the Yocto-hub port is
     * indeed online at a given time. In case of ambiguity when looking for
     * a Yocto-hub port by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     * 
     * @param func : a string that uniquely characterizes the Yocto-hub port
     * 
     * @return a YHubPort object allowing you to drive the Yocto-hub port.
     */
           static YHubPort* FindHubPort(const string& func);
    inline static YHubPort* Find(const string& func)
    { return YHubPort::FindHubPort(func);}
    /**
     * Starts the enumeration of Yocto-hub ports currently accessible.
     * Use the method YHubPort.nextHubPort() to iterate on
     * next Yocto-hub ports.
     * 
     * @return a pointer to a YHubPort object, corresponding to
     *         the first Yocto-hub port currently online, or a null pointer
     *         if there are none.
     */
           static YHubPort* FirstHubPort(void);
    inline static YHubPort* First(void)
    { return YHubPort::FirstHubPort();}
    //--- (end of YHubPort accessors declaration)
};

//--- (HubPort functions declaration)

/**
 * Retrieves a Yocto-hub port for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 * 
 * This function does not require that the Yocto-hub port is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method YHubPort.isOnline() to test if the Yocto-hub port is
 * indeed online at a given time. In case of ambiguity when looking for
 * a Yocto-hub port by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 * 
 * @param func : a string that uniquely characterizes the Yocto-hub port
 * 
 * @return a YHubPort object allowing you to drive the Yocto-hub port.
 */
inline YHubPort* yFindHubPort(const string& func)
{ return YHubPort::FindHubPort(func);}
/**
 * Starts the enumeration of Yocto-hub ports currently accessible.
 * Use the method YHubPort.nextHubPort() to iterate on
 * next Yocto-hub ports.
 * 
 * @return a pointer to a YHubPort object, corresponding to
 *         the first Yocto-hub port currently online, or a null pointer
 *         if there are none.
 */
inline YHubPort* yFirstHubPort(void)
{ return YHubPort::FirstHubPort();}

//--- (end of HubPort functions declaration)

#endif
