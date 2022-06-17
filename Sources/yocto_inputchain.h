/*********************************************************************
 *
 *  $Id: svn_id $
 *
 *  Declares yFindInputChain(), the high-level API for InputChain functions
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


#ifndef YOCTO_INPUTCHAIN_H
#define YOCTO_INPUTCHAIN_H

#include <cfloat>
#include <cmath>

#include "yocto_api.h"

#ifdef YOCTOLIB_NAMESPACE
namespace YOCTOLIB_NAMESPACE
{
#endif

//--- (YInputChain return codes)
//--- (end of YInputChain return codes)
//--- (YInputChain yapiwrapper)
//--- (end of YInputChain yapiwrapper)
//--- (YInputChain definitions)
class YInputChain; // forward declaration

typedef void (*YInputChainValueCallback)(YInputChain *func, const string& functionValue);
typedef void (*YEventCallback)(YInputChain *inputChain, int timestamp, const string& eventType, const string& eventData, const string& eventChange);

#ifndef _Y_LOOPBACKTEST_ENUM
#define _Y_LOOPBACKTEST_ENUM
typedef enum {
    Y_LOOPBACKTEST_OFF = 0,
    Y_LOOPBACKTEST_ON = 1,
    Y_LOOPBACKTEST_INVALID = -1,
} Y_LOOPBACKTEST_enum;
#endif
#define Y_EXPECTEDNODES_INVALID         (YAPI_INVALID_UINT)
#define Y_DETECTEDNODES_INVALID         (YAPI_INVALID_UINT)
#define Y_REFRESHRATE_INVALID           (YAPI_INVALID_UINT)
#define Y_BITCHAIN1_INVALID             (YAPI_INVALID_STRING)
#define Y_BITCHAIN2_INVALID             (YAPI_INVALID_STRING)
#define Y_BITCHAIN3_INVALID             (YAPI_INVALID_STRING)
#define Y_BITCHAIN4_INVALID             (YAPI_INVALID_STRING)
#define Y_BITCHAIN5_INVALID             (YAPI_INVALID_STRING)
#define Y_BITCHAIN6_INVALID             (YAPI_INVALID_STRING)
#define Y_BITCHAIN7_INVALID             (YAPI_INVALID_STRING)
#define Y_WATCHDOGPERIOD_INVALID        (YAPI_INVALID_UINT)
#define Y_CHAINDIAGS_INVALID            (YAPI_INVALID_UINT)
//--- (end of YInputChain definitions)

//--- (YInputChain declaration)
/**
 * YInputChain Class: InputChain function interface
 *
 * The YInputChain class provides access to separate
 * digital inputs connected in a chain.
 */
class YOCTO_CLASS_EXPORT YInputChain: public YFunction {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of YInputChain declaration)
protected:
    static void yInternalEventCallback(YInputChain *obj, const string& value);

    //--- (YInputChain attributes)
    // Attributes (function value cache)
    int             _expectedNodes;
    int             _detectedNodes;
    Y_LOOPBACKTEST_enum _loopbackTest;
    int             _refreshRate;
    string          _bitChain1;
    string          _bitChain2;
    string          _bitChain3;
    string          _bitChain4;
    string          _bitChain5;
    string          _bitChain6;
    string          _bitChain7;
    int             _watchdogPeriod;
    int             _chainDiags;
    YInputChainValueCallback _valueCallbackInputChain;
    YEventCallback  _eventCallback;
    int             _prevPos;
    int             _eventPos;
    int             _eventStamp;
    vector<string>  _eventChains;

    friend YInputChain *yFindInputChain(const string& func);
    friend YInputChain *yFirstInputChain(void);

    // Function-specific method for parsing of JSON output and caching result
    virtual int     _parseAttr(YJSONObject *json_val);

    // Constructor is protected, use yFindInputChain factory function to instantiate
    YInputChain(const string& func);
    //--- (end of YInputChain attributes)

public:
    virtual ~YInputChain();
    //--- (YInputChain accessors declaration)

    static const int EXPECTEDNODES_INVALID = YAPI_INVALID_UINT;
    static const int DETECTEDNODES_INVALID = YAPI_INVALID_UINT;
    static const Y_LOOPBACKTEST_enum LOOPBACKTEST_OFF = Y_LOOPBACKTEST_OFF;
    static const Y_LOOPBACKTEST_enum LOOPBACKTEST_ON = Y_LOOPBACKTEST_ON;
    static const Y_LOOPBACKTEST_enum LOOPBACKTEST_INVALID = Y_LOOPBACKTEST_INVALID;
    static const int REFRESHRATE_INVALID = YAPI_INVALID_UINT;
    static const string BITCHAIN1_INVALID;
    static const string BITCHAIN2_INVALID;
    static const string BITCHAIN3_INVALID;
    static const string BITCHAIN4_INVALID;
    static const string BITCHAIN5_INVALID;
    static const string BITCHAIN6_INVALID;
    static const string BITCHAIN7_INVALID;
    static const int WATCHDOGPERIOD_INVALID = YAPI_INVALID_UINT;
    static const int CHAINDIAGS_INVALID = YAPI_INVALID_UINT;

    /**
     * Returns the number of nodes expected in the chain.
     *
     * @return an integer corresponding to the number of nodes expected in the chain
     *
     * On failure, throws an exception or returns YInputChain::EXPECTEDNODES_INVALID.
     */
    int                 get_expectedNodes(void);

    inline int          expectedNodes(void)
    { return this->get_expectedNodes(); }

    /**
     * Changes the number of nodes expected in the chain.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     *
     * @param newval : an integer corresponding to the number of nodes expected in the chain
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_expectedNodes(int newval);
    inline int      setExpectedNodes(int newval)
    { return this->set_expectedNodes(newval); }

    /**
     * Returns the number of nodes detected in the chain.
     *
     * @return an integer corresponding to the number of nodes detected in the chain
     *
     * On failure, throws an exception or returns YInputChain::DETECTEDNODES_INVALID.
     */
    int                 get_detectedNodes(void);

    inline int          detectedNodes(void)
    { return this->get_detectedNodes(); }

    /**
     * Returns the activation state of the exhaustive chain connectivity test.
     * The connectivity test requires a cable connecting the end of the chain
     * to the loopback test connector.
     *
     * @return either YInputChain::LOOPBACKTEST_OFF or YInputChain::LOOPBACKTEST_ON, according to the
     * activation state of the exhaustive chain connectivity test
     *
     * On failure, throws an exception or returns YInputChain::LOOPBACKTEST_INVALID.
     */
    Y_LOOPBACKTEST_enum get_loopbackTest(void);

    inline Y_LOOPBACKTEST_enum loopbackTest(void)
    { return this->get_loopbackTest(); }

    /**
     * Changes the activation state of the exhaustive chain connectivity test.
     * The connectivity test requires a cable connecting the end of the chain
     * to the loopback test connector.
     *
     * @param newval : either YInputChain::LOOPBACKTEST_OFF or YInputChain::LOOPBACKTEST_ON, according to
     * the activation state of the exhaustive chain connectivity test
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_loopbackTest(Y_LOOPBACKTEST_enum newval);
    inline int      setLoopbackTest(Y_LOOPBACKTEST_enum newval)
    { return this->set_loopbackTest(newval); }

    /**
     * Returns the desired refresh rate, measured in Hz.
     * The higher the refresh rate is set, the higher the
     * communication speed on the chain will be.
     *
     * @return an integer corresponding to the desired refresh rate, measured in Hz
     *
     * On failure, throws an exception or returns YInputChain::REFRESHRATE_INVALID.
     */
    int                 get_refreshRate(void);

    inline int          refreshRate(void)
    { return this->get_refreshRate(); }

    /**
     * Changes the desired refresh rate, measured in Hz.
     * The higher the refresh rate is set, the higher the
     * communication speed on the chain will be.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     *
     * @param newval : an integer corresponding to the desired refresh rate, measured in Hz
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_refreshRate(int newval);
    inline int      setRefreshRate(int newval)
    { return this->set_refreshRate(newval); }

    /**
     * Returns the state of input 1 for all nodes of the input chain,
     * as a hexadecimal string. The node nearest to the controller
     * is the lowest bit of the result.
     *
     * @return a string corresponding to the state of input 1 for all nodes of the input chain,
     *         as a hexadecimal string
     *
     * On failure, throws an exception or returns YInputChain::BITCHAIN1_INVALID.
     */
    string              get_bitChain1(void);

    inline string       bitChain1(void)
    { return this->get_bitChain1(); }

    /**
     * Returns the state of input 2 for all nodes of the input chain,
     * as a hexadecimal string. The node nearest to the controller
     * is the lowest bit of the result.
     *
     * @return a string corresponding to the state of input 2 for all nodes of the input chain,
     *         as a hexadecimal string
     *
     * On failure, throws an exception or returns YInputChain::BITCHAIN2_INVALID.
     */
    string              get_bitChain2(void);

    inline string       bitChain2(void)
    { return this->get_bitChain2(); }

    /**
     * Returns the state of input 3 for all nodes of the input chain,
     * as a hexadecimal string. The node nearest to the controller
     * is the lowest bit of the result.
     *
     * @return a string corresponding to the state of input 3 for all nodes of the input chain,
     *         as a hexadecimal string
     *
     * On failure, throws an exception or returns YInputChain::BITCHAIN3_INVALID.
     */
    string              get_bitChain3(void);

    inline string       bitChain3(void)
    { return this->get_bitChain3(); }

    /**
     * Returns the state of input 4 for all nodes of the input chain,
     * as a hexadecimal string. The node nearest to the controller
     * is the lowest bit of the result.
     *
     * @return a string corresponding to the state of input 4 for all nodes of the input chain,
     *         as a hexadecimal string
     *
     * On failure, throws an exception or returns YInputChain::BITCHAIN4_INVALID.
     */
    string              get_bitChain4(void);

    inline string       bitChain4(void)
    { return this->get_bitChain4(); }

    /**
     * Returns the state of input 5 for all nodes of the input chain,
     * as a hexadecimal string. The node nearest to the controller
     * is the lowest bit of the result.
     *
     * @return a string corresponding to the state of input 5 for all nodes of the input chain,
     *         as a hexadecimal string
     *
     * On failure, throws an exception or returns YInputChain::BITCHAIN5_INVALID.
     */
    string              get_bitChain5(void);

    inline string       bitChain5(void)
    { return this->get_bitChain5(); }

    /**
     * Returns the state of input 6 for all nodes of the input chain,
     * as a hexadecimal string. The node nearest to the controller
     * is the lowest bit of the result.
     *
     * @return a string corresponding to the state of input 6 for all nodes of the input chain,
     *         as a hexadecimal string
     *
     * On failure, throws an exception or returns YInputChain::BITCHAIN6_INVALID.
     */
    string              get_bitChain6(void);

    inline string       bitChain6(void)
    { return this->get_bitChain6(); }

    /**
     * Returns the state of input 7 for all nodes of the input chain,
     * as a hexadecimal string. The node nearest to the controller
     * is the lowest bit of the result.
     *
     * @return a string corresponding to the state of input 7 for all nodes of the input chain,
     *         as a hexadecimal string
     *
     * On failure, throws an exception or returns YInputChain::BITCHAIN7_INVALID.
     */
    string              get_bitChain7(void);

    inline string       bitChain7(void)
    { return this->get_bitChain7(); }

    /**
     * Returns the wait time in seconds before triggering an inactivity
     * timeout error.
     *
     * @return an integer corresponding to the wait time in seconds before triggering an inactivity
     *         timeout error
     *
     * On failure, throws an exception or returns YInputChain::WATCHDOGPERIOD_INVALID.
     */
    int                 get_watchdogPeriod(void);

    inline int          watchdogPeriod(void)
    { return this->get_watchdogPeriod(); }

    /**
     * Changes the wait time in seconds before triggering an inactivity
     * timeout error. Remember to call the saveToFlash() method
     * of the module if the modification must be kept.
     *
     * @param newval : an integer corresponding to the wait time in seconds before triggering an inactivity
     *         timeout error
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_watchdogPeriod(int newval);
    inline int      setWatchdogPeriod(int newval)
    { return this->set_watchdogPeriod(newval); }

    /**
     * Returns the controller state diagnostics. Bit 0 indicates a chain length
     * error, bit 1 indicates an inactivity timeout and bit 2 indicates
     * a loopback test failure.
     *
     * @return an integer corresponding to the controller state diagnostics
     *
     * On failure, throws an exception or returns YInputChain::CHAINDIAGS_INVALID.
     */
    int                 get_chainDiags(void);

    inline int          chainDiags(void)
    { return this->get_chainDiags(); }

    /**
     * Retrieves a digital input chain for a given identifier.
     * The identifier can be specified using several formats:
     * <ul>
     * <li>FunctionLogicalName</li>
     * <li>ModuleSerialNumber.FunctionIdentifier</li>
     * <li>ModuleSerialNumber.FunctionLogicalName</li>
     * <li>ModuleLogicalName.FunctionIdentifier</li>
     * <li>ModuleLogicalName.FunctionLogicalName</li>
     * </ul>
     *
     * This function does not require that the digital input chain is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method isOnline() to test if the digital input chain is
     * indeed online at a given time. In case of ambiguity when looking for
     * a digital input chain by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     *
     * If a call to this object's is_online() method returns FALSE although
     * you are certain that the matching device is plugged, make sure that you did
     * call registerHub() at application initialization time.
     *
     * @param func : a string that uniquely characterizes the digital input chain, for instance
     *         MyDevice.inputChain.
     *
     * @return a YInputChain object allowing you to drive the digital input chain.
     */
    static YInputChain* FindInputChain(string func);

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
    virtual int         registerValueCallback(YInputChainValueCallback callback);
    using YFunction::registerValueCallback;

    virtual int         _invokeValueCallback(string value);

    /**
     * Resets the application watchdog countdown.
     * If you have setup a non-zero watchdogPeriod, you should
     * call this function on a regular basis to prevent the application
     * inactivity error to be triggered.
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         resetWatchdog(void);

    /**
     * Returns a string with last events observed on the digital input chain.
     * This method return only events that are still buffered in the device memory.
     *
     * @return a string with last events observed (one per line).
     *
     * On failure, throws an exception or returns  YAPI::INVALID_STRING.
     */
    virtual string      get_lastEvents(void);

    /**
     * Registers a callback function to be called each time that an event is detected on the
     * input chain.The callback is invoked only during the execution of
     * ySleep or yHandleEvents. This provides control over the time when
     * the callback is triggered. For good responsiveness, remember to call one of these
     * two functions periodically. To unregister a callback, pass a NULL pointer as argument.
     *
     * @param callback : the callback function to call, or a NULL pointer.
     *         The callback function should take four arguments:
     *         the YInputChain object that emitted the event, the
     *         UTC timestamp of the event, a character string describing
     *         the type of event and a character string with the event data.
     *         On failure, throws an exception or returns a negative error code.
     */
    virtual int         registerEventCallback(YEventCallback callback);

    virtual int         _internalEventHandler(string cbpos);

    virtual string      _strXor(string a,string b);

    virtual vector<int> hex2array(string hexstr);


    inline static YInputChain *Find(string func)
    { return YInputChain::FindInputChain(func); }

    /**
     * Continues the enumeration of digital input chains started using yFirstInputChain().
     * Caution: You can't make any assumption about the returned digital input chains order.
     * If you want to find a specific a digital input chain, use InputChain.findInputChain()
     * and a hardwareID or a logical name.
     *
     * @return a pointer to a YInputChain object, corresponding to
     *         a digital input chain currently online, or a NULL pointer
     *         if there are no more digital input chains to enumerate.
     */
           YInputChain     *nextInputChain(void);
    inline YInputChain     *next(void)
    { return this->nextInputChain();}

    /**
     * Starts the enumeration of digital input chains currently accessible.
     * Use the method YInputChain::nextInputChain() to iterate on
     * next digital input chains.
     *
     * @return a pointer to a YInputChain object, corresponding to
     *         the first digital input chain currently online, or a NULL pointer
     *         if there are none.
     */
           static YInputChain *FirstInputChain(void);
    inline static YInputChain *First(void)
    { return YInputChain::FirstInputChain();}
#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of YInputChain accessors declaration)
};

//--- (YInputChain functions declaration)

/**
 * Retrieves a digital input chain for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that the digital input chain is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the digital input chain is
 * indeed online at a given time. In case of ambiguity when looking for
 * a digital input chain by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the digital input chain, for instance
 *         MyDevice.inputChain.
 *
 * @return a YInputChain object allowing you to drive the digital input chain.
 */
inline YInputChain *yFindInputChain(const string& func)
{ return YInputChain::FindInputChain(func);}
/**
 * Starts the enumeration of digital input chains currently accessible.
 * Use the method YInputChain::nextInputChain() to iterate on
 * next digital input chains.
 *
 * @return a pointer to a YInputChain object, corresponding to
 *         the first digital input chain currently online, or a NULL pointer
 *         if there are none.
 */
inline YInputChain *yFirstInputChain(void)
{ return YInputChain::FirstInputChain();}

//--- (end of YInputChain functions declaration)

#ifdef YOCTOLIB_NAMESPACE
// end of namespace definition
}
#endif

#endif
