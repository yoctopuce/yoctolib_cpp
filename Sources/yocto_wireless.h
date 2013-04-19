/*********************************************************************
 *
 * $Id: yocto_wireless.h 9921 2013-02-20 09:39:16Z seb $
 *
 * Declares yFindWireless(), the high-level API for Wireless functions
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


#ifndef YOCTO_WIRELESS_H
#define YOCTO_WIRELESS_H

#include "yocto_api.h"
#include <cfloat>
#include <cmath>
#include <map>

//--- (return codes)
//--- (end of return codes)
//--- (YWireless definitions)
class YWireless; //forward declaration

typedef void (*YWirelessUpdateCallback)(YWireless *func, const string& functionValue);
typedef enum {
    Y_SECURITY_UNKNOWN = 0,
    Y_SECURITY_OPEN = 1,
    Y_SECURITY_WEP = 2,
    Y_SECURITY_WPA = 3,
    Y_SECURITY_WPA2 = 4,
    Y_SECURITY_INVALID = -1,
} Y_SECURITY_enum;

#define Y_LOGICALNAME_INVALID           (YAPI::INVALID_STRING)
#define Y_ADVERTISEDVALUE_INVALID       (YAPI::INVALID_STRING)
#define Y_LINKQUALITY_INVALID           (-1)
#define Y_SSID_INVALID                  (YAPI::INVALID_STRING)
#define Y_CHANNEL_INVALID               (0xffffffff)
#define Y_MESSAGE_INVALID               (YAPI::INVALID_STRING)
#define Y_WLANCONFIG_INVALID            (YAPI::INVALID_STRING)
//--- (end of YWireless definitions)

//--- (YWireless declaration)
/**
 * YWireless Class: Wireless function interface
 * 
 * 
 */
class YWireless: public YFunction {
protected:
    // Attributes (function value cache)
    YWirelessUpdateCallback _callback;
    string          _logicalName;
    string          _advertisedValue;
    int             _linkQuality;
    string          _ssid;
    unsigned        _channel;
    Y_SECURITY_enum _security;
    string          _message;
    string          _wlanConfig;
    // Static function object cache
    static std::map<string,YWireless*> _WirelessCache;

    friend YWireless *yFindWireless(const string& func);
    friend YWireless *yFirstWireless(void);

    // Function-specific method for parsing of JSON output and caching result
    int             _parse(yJsonStateMachine& j);
    //--- (end of YWireless declaration)

    //--- (YWireless constructor)
    // Constructor is protected, use yFindWireless factory function to instantiate
    YWireless(const string& func): YFunction("Wireless", func)
    //--- (end of YWireless constructor)
    //--- (Wireless initialization)
            ,_callback(NULL)
            ,_logicalName(Y_LOGICALNAME_INVALID)
            ,_advertisedValue(Y_ADVERTISEDVALUE_INVALID)
            ,_linkQuality(Y_LINKQUALITY_INVALID)
            ,_ssid(Y_SSID_INVALID)
            ,_channel(Y_CHANNEL_INVALID)
            ,_security(Y_SECURITY_INVALID)
            ,_message(Y_MESSAGE_INVALID)
            ,_wlanConfig(Y_WLANCONFIG_INVALID)
    //--- (end of Wireless initialization)
    {};

public:
    //--- (YWireless accessors declaration)

    static const string LOGICALNAME_INVALID;
    static const string ADVERTISEDVALUE_INVALID;
    static const int      LINKQUALITY_INVALID = -1;
    static const string SSID_INVALID;
    static const unsigned CHANNEL_INVALID = 0xffffffff;
    static const Y_SECURITY_enum SECURITY_UNKNOWN = Y_SECURITY_UNKNOWN;
    static const Y_SECURITY_enum SECURITY_OPEN = Y_SECURITY_OPEN;
    static const Y_SECURITY_enum SECURITY_WEP = Y_SECURITY_WEP;
    static const Y_SECURITY_enum SECURITY_WPA = Y_SECURITY_WPA;
    static const Y_SECURITY_enum SECURITY_WPA2 = Y_SECURITY_WPA2;
    static const Y_SECURITY_enum SECURITY_INVALID = Y_SECURITY_INVALID;
    static const string MESSAGE_INVALID;
    static const string WLANCONFIG_INVALID;

    /**
     * Returns the logical name of the wireless lan interface.
     * 
     * @return a string corresponding to the logical name of the wireless lan interface
     * 
     * On failure, throws an exception or returns Y_LOGICALNAME_INVALID.
     */
           string          get_logicalName(void);
    inline string          logicalName(void)
    { return this->get_logicalName(); }

    /**
     * Changes the logical name of the wireless lan interface. You can use yCheckLogicalName()
     * prior to this call to make sure that your parameter is valid.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     * 
     * @param newval : a string corresponding to the logical name of the wireless lan interface
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_logicalName(const string& newval);
    inline int      setLogicalName(const string& newval)
    { return this->set_logicalName(newval); }

    /**
     * Returns the current value of the wireless lan interface (no more than 6 characters).
     * 
     * @return a string corresponding to the current value of the wireless lan interface (no more than 6 characters)
     * 
     * On failure, throws an exception or returns Y_ADVERTISEDVALUE_INVALID.
     */
           string          get_advertisedValue(void);
    inline string          advertisedValue(void)
    { return this->get_advertisedValue(); }

    /**
     * Returns the link quality, expressed in per cents.
     * 
     * @return an integer corresponding to the link quality, expressed in per cents
     * 
     * On failure, throws an exception or returns Y_LINKQUALITY_INVALID.
     */
           int             get_linkQuality(void);
    inline int             linkQuality(void)
    { return this->get_linkQuality(); }

    /**
     * Returns the wireless network name (SSID).
     * 
     * @return a string corresponding to the wireless network name (SSID)
     * 
     * On failure, throws an exception or returns Y_SSID_INVALID.
     */
           string          get_ssid(void);
    inline string          ssid(void)
    { return this->get_ssid(); }

    /**
     * Returns the 802.11 channel currently used, or 0 when the selected network has not been found.
     * 
     * @return an integer corresponding to the 802
     * 
     * On failure, throws an exception or returns Y_CHANNEL_INVALID.
     */
           unsigned        get_channel(void);
    inline unsigned        channel(void)
    { return this->get_channel(); }

    /**
     * Returns the security algorithm used by the selected wireless network.
     * 
     * @return a value among Y_SECURITY_UNKNOWN, Y_SECURITY_OPEN, Y_SECURITY_WEP, Y_SECURITY_WPA and
     * Y_SECURITY_WPA2 corresponding to the security algorithm used by the selected wireless network
     * 
     * On failure, throws an exception or returns Y_SECURITY_INVALID.
     */
           Y_SECURITY_enum get_security(void);
    inline Y_SECURITY_enum security(void)
    { return this->get_security(); }

    /**
     * Returns the last status message from the wireless interface.
     * 
     * @return a string corresponding to the last status message from the wireless interface
     * 
     * On failure, throws an exception or returns Y_MESSAGE_INVALID.
     */
           string          get_message(void);
    inline string          message(void)
    { return this->get_message(); }

           string          get_wlanConfig(void);
    inline string          wlanConfig(void)
    { return this->get_wlanConfig(); }

    int             set_wlanConfig(const string& newval);
    inline int      setWlanConfig(const string& newval)
    { return this->set_wlanConfig(newval); }

    /**
     * Changes the configuration of the wireless lan interface to connect to an existing
     * access point (infrastructure mode).
     * Remember to call the saveToFlash() method and then to reboot the module to apply this setting.
     * 
     * @param ssid : the name of the network to connect to
     * @param securityKey : the network key, as a character string
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             joinNetwork(string ssid,string securityKey);

    /**
     * Changes the configuration of the wireless lan interface to create an ad-hoc
     * wireless network, without using an access point. If a security key is specified,
     * the network will be protected by WEP128, since WPA is not standardized for
     * ad-hoc networks.
     * Remember to call the saveToFlash() method and then to reboot the module to apply this setting.
     * 
     * @param ssid : the name of the network to connect to
     * @param securityKey : the network key, as a character string
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             adhocNetwork(string ssid,string securityKey);


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
    void registerValueCallback(YWirelessUpdateCallback callback);

    void advertiseValue(const string& value);

    /**
     * Continues the enumeration of wireless lan interfaces started using yFirstWireless().
     * 
     * @return a pointer to a YWireless object, corresponding to
     *         a wireless lan interface currently online, or a null pointer
     *         if there are no more wireless lan interfaces to enumerate.
     */
           YWireless       *nextWireless(void);
    inline YWireless       *next(void)
    { return this->nextWireless();}

    /**
     * Retrieves a wireless lan interface for a given identifier.
     * The identifier can be specified using several formats:
     * <ul>
     * <li>FunctionLogicalName</li>
     * <li>ModuleSerialNumber.FunctionIdentifier</li>
     * <li>ModuleSerialNumber.FunctionLogicalName</li>
     * <li>ModuleLogicalName.FunctionIdentifier</li>
     * <li>ModuleLogicalName.FunctionLogicalName</li>
     * </ul>
     * 
     * This function does not require that the wireless lan interface is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method YWireless.isOnline() to test if the wireless lan interface is
     * indeed online at a given time. In case of ambiguity when looking for
     * a wireless lan interface by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     * 
     * @param func : a string that uniquely characterizes the wireless lan interface
     * 
     * @return a YWireless object allowing you to drive the wireless lan interface.
     */
           static YWireless* FindWireless(const string& func);
    inline static YWireless* Find(const string& func)
    { return YWireless::FindWireless(func);}
    /**
     * Starts the enumeration of wireless lan interfaces currently accessible.
     * Use the method YWireless.nextWireless() to iterate on
     * next wireless lan interfaces.
     * 
     * @return a pointer to a YWireless object, corresponding to
     *         the first wireless lan interface currently online, or a null pointer
     *         if there are none.
     */
           static YWireless* FirstWireless(void);
    inline static YWireless* First(void)
    { return YWireless::FirstWireless();}
    //--- (end of YWireless accessors declaration)
};

//--- (Wireless functions declaration)

/**
 * Retrieves a wireless lan interface for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 * 
 * This function does not require that the wireless lan interface is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method YWireless.isOnline() to test if the wireless lan interface is
 * indeed online at a given time. In case of ambiguity when looking for
 * a wireless lan interface by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 * 
 * @param func : a string that uniquely characterizes the wireless lan interface
 * 
 * @return a YWireless object allowing you to drive the wireless lan interface.
 */
inline YWireless* yFindWireless(const string& func)
{ return YWireless::FindWireless(func);}
/**
 * Starts the enumeration of wireless lan interfaces currently accessible.
 * Use the method YWireless.nextWireless() to iterate on
 * next wireless lan interfaces.
 * 
 * @return a pointer to a YWireless object, corresponding to
 *         the first wireless lan interface currently online, or a null pointer
 *         if there are none.
 */
inline YWireless* yFirstWireless(void)
{ return YWireless::FirstWireless();}

//--- (end of Wireless functions declaration)

#endif
