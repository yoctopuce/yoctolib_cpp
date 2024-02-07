/*********************************************************************
 *
 *  $Id: yocto_network.cpp 54332 2023-05-02 08:35:37Z seb $
 *
 *  Implements yFindNetwork(), the high-level API for Network functions
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


#define _CRT_SECURE_NO_DEPRECATE //do not use windows secure crt
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "yocto_network.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#define  __FILE_ID__  "network"

#ifdef YOCTOLIB_NAMESPACE
using namespace YOCTOLIB_NAMESPACE;
#endif

YNetwork::YNetwork(const string& func): YFunction(func)
//--- (YNetwork initialization)
    ,_readiness(READINESS_INVALID)
    ,_macAddress(MACADDRESS_INVALID)
    ,_ipAddress(IPADDRESS_INVALID)
    ,_subnetMask(SUBNETMASK_INVALID)
    ,_router(ROUTER_INVALID)
    ,_currentDNS(CURRENTDNS_INVALID)
    ,_ipConfig(IPCONFIG_INVALID)
    ,_primaryDNS(PRIMARYDNS_INVALID)
    ,_secondaryDNS(SECONDARYDNS_INVALID)
    ,_ntpServer(NTPSERVER_INVALID)
    ,_userPassword(USERPASSWORD_INVALID)
    ,_adminPassword(ADMINPASSWORD_INVALID)
    ,_httpPort(HTTPPORT_INVALID)
    ,_httpsPort(HTTPSPORT_INVALID)
    ,_defaultPage(DEFAULTPAGE_INVALID)
    ,_discoverable(DISCOVERABLE_INVALID)
    ,_wwwWatchdogDelay(WWWWATCHDOGDELAY_INVALID)
    ,_callbackUrl(CALLBACKURL_INVALID)
    ,_callbackMethod(CALLBACKMETHOD_INVALID)
    ,_callbackEncoding(CALLBACKENCODING_INVALID)
    ,_callbackTemplate(CALLBACKTEMPLATE_INVALID)
    ,_callbackCredentials(CALLBACKCREDENTIALS_INVALID)
    ,_callbackInitialDelay(CALLBACKINITIALDELAY_INVALID)
    ,_callbackSchedule(CALLBACKSCHEDULE_INVALID)
    ,_callbackMinDelay(CALLBACKMINDELAY_INVALID)
    ,_callbackMaxDelay(CALLBACKMAXDELAY_INVALID)
    ,_poeCurrent(POECURRENT_INVALID)
    ,_valueCallbackNetwork(NULL)
//--- (end of YNetwork initialization)
{
    _className="Network";
}

YNetwork::~YNetwork()
{
//--- (YNetwork cleanup)
//--- (end of YNetwork cleanup)
}
//--- (YNetwork implementation)
// static attributes
const string YNetwork::MACADDRESS_INVALID = YAPI_INVALID_STRING;
const string YNetwork::IPADDRESS_INVALID = YAPI_INVALID_STRING;
const string YNetwork::SUBNETMASK_INVALID = YAPI_INVALID_STRING;
const string YNetwork::ROUTER_INVALID = YAPI_INVALID_STRING;
const string YNetwork::CURRENTDNS_INVALID = YAPI_INVALID_STRING;
const string YNetwork::IPCONFIG_INVALID = YAPI_INVALID_STRING;
const string YNetwork::PRIMARYDNS_INVALID = YAPI_INVALID_STRING;
const string YNetwork::SECONDARYDNS_INVALID = YAPI_INVALID_STRING;
const string YNetwork::NTPSERVER_INVALID = YAPI_INVALID_STRING;
const string YNetwork::USERPASSWORD_INVALID = YAPI_INVALID_STRING;
const string YNetwork::ADMINPASSWORD_INVALID = YAPI_INVALID_STRING;
const string YNetwork::DEFAULTPAGE_INVALID = YAPI_INVALID_STRING;
const string YNetwork::CALLBACKURL_INVALID = YAPI_INVALID_STRING;
const string YNetwork::CALLBACKCREDENTIALS_INVALID = YAPI_INVALID_STRING;
const string YNetwork::CALLBACKSCHEDULE_INVALID = YAPI_INVALID_STRING;

int YNetwork::_parseAttr(YJSONObject *json_val)
{
    if(json_val->has("readiness")) {
        _readiness =  (Y_READINESS_enum)json_val->getInt("readiness");
    }
    if(json_val->has("macAddress")) {
        _macAddress =  json_val->getString("macAddress");
    }
    if(json_val->has("ipAddress")) {
        _ipAddress =  json_val->getString("ipAddress");
    }
    if(json_val->has("subnetMask")) {
        _subnetMask =  json_val->getString("subnetMask");
    }
    if(json_val->has("router")) {
        _router =  json_val->getString("router");
    }
    if(json_val->has("currentDNS")) {
        _currentDNS =  json_val->getString("currentDNS");
    }
    if(json_val->has("ipConfig")) {
        _ipConfig =  json_val->getString("ipConfig");
    }
    if(json_val->has("primaryDNS")) {
        _primaryDNS =  json_val->getString("primaryDNS");
    }
    if(json_val->has("secondaryDNS")) {
        _secondaryDNS =  json_val->getString("secondaryDNS");
    }
    if(json_val->has("ntpServer")) {
        _ntpServer =  json_val->getString("ntpServer");
    }
    if(json_val->has("userPassword")) {
        _userPassword =  json_val->getString("userPassword");
    }
    if(json_val->has("adminPassword")) {
        _adminPassword =  json_val->getString("adminPassword");
    }
    if(json_val->has("httpPort")) {
        _httpPort =  json_val->getInt("httpPort");
    }
    if(json_val->has("httpsPort")) {
        _httpsPort =  json_val->getInt("httpsPort");
    }
    if(json_val->has("defaultPage")) {
        _defaultPage =  json_val->getString("defaultPage");
    }
    if(json_val->has("discoverable")) {
        _discoverable =  (Y_DISCOVERABLE_enum)json_val->getInt("discoverable");
    }
    if(json_val->has("wwwWatchdogDelay")) {
        _wwwWatchdogDelay =  json_val->getInt("wwwWatchdogDelay");
    }
    if(json_val->has("callbackUrl")) {
        _callbackUrl =  json_val->getString("callbackUrl");
    }
    if(json_val->has("callbackMethod")) {
        _callbackMethod =  (Y_CALLBACKMETHOD_enum)json_val->getInt("callbackMethod");
    }
    if(json_val->has("callbackEncoding")) {
        _callbackEncoding =  (Y_CALLBACKENCODING_enum)json_val->getInt("callbackEncoding");
    }
    if(json_val->has("callbackTemplate")) {
        _callbackTemplate =  (Y_CALLBACKTEMPLATE_enum)json_val->getInt("callbackTemplate");
    }
    if(json_val->has("callbackCredentials")) {
        _callbackCredentials =  json_val->getString("callbackCredentials");
    }
    if(json_val->has("callbackInitialDelay")) {
        _callbackInitialDelay =  json_val->getInt("callbackInitialDelay");
    }
    if(json_val->has("callbackSchedule")) {
        _callbackSchedule =  json_val->getString("callbackSchedule");
    }
    if(json_val->has("callbackMinDelay")) {
        _callbackMinDelay =  json_val->getInt("callbackMinDelay");
    }
    if(json_val->has("callbackMaxDelay")) {
        _callbackMaxDelay =  json_val->getInt("callbackMaxDelay");
    }
    if(json_val->has("poeCurrent")) {
        _poeCurrent =  json_val->getInt("poeCurrent");
    }
    return YFunction::_parseAttr(json_val);
}


/**
 * Returns the current established working mode of the network interface.
 * Level zero (DOWN_0) means that no hardware link has been detected. Either there is no signal
 * on the network cable, or the selected wireless access point cannot be detected.
 * Level 1 (LIVE_1) is reached when the network is detected, but is not yet connected.
 * For a wireless network, this shows that the requested SSID is present.
 * Level 2 (LINK_2) is reached when the hardware connection is established.
 * For a wired network connection, level 2 means that the cable is attached at both ends.
 * For a connection to a wireless access point, it shows that the security parameters
 * are properly configured. For an ad-hoc wireless connection, it means that there is
 * at least one other device connected on the ad-hoc network.
 * Level 3 (DHCP_3) is reached when an IP address has been obtained using DHCP.
 * Level 4 (DNS_4) is reached when the DNS server is reachable on the network.
 * Level 5 (WWW_5) is reached when global connectivity is demonstrated by properly loading the
 * current time from an NTP server.
 *
 * @return a value among YNetwork::READINESS_DOWN, YNetwork::READINESS_EXISTS,
 * YNetwork::READINESS_LINKED, YNetwork::READINESS_LAN_OK and YNetwork::READINESS_WWW_OK corresponding to
 * the current established working mode of the network interface
 *
 * On failure, throws an exception or returns YNetwork::READINESS_INVALID.
 */
Y_READINESS_enum YNetwork::get_readiness(void)
{
    Y_READINESS_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YNetwork::READINESS_INVALID;
                }
            }
        }
        res = _readiness;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the MAC address of the network interface. The MAC address is also available on a sticker
 * on the module, in both numeric and barcode forms.
 *
 * @return a string corresponding to the MAC address of the network interface
 *
 * On failure, throws an exception or returns YNetwork::MACADDRESS_INVALID.
 */
string YNetwork::get_macAddress(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration == 0) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YNetwork::MACADDRESS_INVALID;
                }
            }
        }
        res = _macAddress;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the IP address currently in use by the device. The address may have been configured
 * statically, or provided by a DHCP server.
 *
 * @return a string corresponding to the IP address currently in use by the device
 *
 * On failure, throws an exception or returns YNetwork::IPADDRESS_INVALID.
 */
string YNetwork::get_ipAddress(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YNetwork::IPADDRESS_INVALID;
                }
            }
        }
        res = _ipAddress;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the subnet mask currently used by the device.
 *
 * @return a string corresponding to the subnet mask currently used by the device
 *
 * On failure, throws an exception or returns YNetwork::SUBNETMASK_INVALID.
 */
string YNetwork::get_subnetMask(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YNetwork::SUBNETMASK_INVALID;
                }
            }
        }
        res = _subnetMask;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the IP address of the router on the device subnet (default gateway).
 *
 * @return a string corresponding to the IP address of the router on the device subnet (default gateway)
 *
 * On failure, throws an exception or returns YNetwork::ROUTER_INVALID.
 */
string YNetwork::get_router(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YNetwork::ROUTER_INVALID;
                }
            }
        }
        res = _router;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the IP address of the DNS server currently used by the device.
 *
 * @return a string corresponding to the IP address of the DNS server currently used by the device
 *
 * On failure, throws an exception or returns YNetwork::CURRENTDNS_INVALID.
 */
string YNetwork::get_currentDNS(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YNetwork::CURRENTDNS_INVALID;
                }
            }
        }
        res = _currentDNS;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the IP configuration of the network interface.
 *
 * If the network interface is setup to use a static IP address, the string starts with "STATIC:" and
 * is followed by three
 * parameters, separated by "/". The first is the device IP address, followed by the subnet mask
 * length, and finally the
 * router IP address (default gateway). For instance: "STATIC:192.168.1.14/16/192.168.1.1"
 *
 * If the network interface is configured to receive its IP from a DHCP server, the string start with
 * "DHCP:" and is followed by
 * three parameters separated by "/". The first is the fallback IP address, then the fallback subnet
 * mask length and finally the
 * fallback router IP address. These three parameters are used when no DHCP reply is received.
 *
 * @return a string corresponding to the IP configuration of the network interface
 *
 * On failure, throws an exception or returns YNetwork::IPCONFIG_INVALID.
 */
string YNetwork::get_ipConfig(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YNetwork::IPCONFIG_INVALID;
                }
            }
        }
        res = _ipConfig;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

int YNetwork::set_ipConfig(const string& newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = newval;
        res = _setAttr("ipConfig", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the IP address of the primary name server to be used by the module.
 *
 * @return a string corresponding to the IP address of the primary name server to be used by the module
 *
 * On failure, throws an exception or returns YNetwork::PRIMARYDNS_INVALID.
 */
string YNetwork::get_primaryDNS(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YNetwork::PRIMARYDNS_INVALID;
                }
            }
        }
        res = _primaryDNS;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the IP address of the primary name server to be used by the module.
 * When using DHCP, if a value is specified, it overrides the value received from the DHCP server.
 * Remember to call the saveToFlash() method and then to reboot the module to apply this setting.
 *
 * @param newval : a string corresponding to the IP address of the primary name server to be used by the module
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::set_primaryDNS(const string& newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = newval;
        res = _setAttr("primaryDNS", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the IP address of the secondary name server to be used by the module.
 *
 * @return a string corresponding to the IP address of the secondary name server to be used by the module
 *
 * On failure, throws an exception or returns YNetwork::SECONDARYDNS_INVALID.
 */
string YNetwork::get_secondaryDNS(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YNetwork::SECONDARYDNS_INVALID;
                }
            }
        }
        res = _secondaryDNS;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the IP address of the secondary name server to be used by the module.
 * When using DHCP, if a value is specified, it overrides the value received from the DHCP server.
 * Remember to call the saveToFlash() method and then to reboot the module to apply this setting.
 *
 * @param newval : a string corresponding to the IP address of the secondary name server to be used by the module
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::set_secondaryDNS(const string& newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = newval;
        res = _setAttr("secondaryDNS", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the IP address of the NTP server to be used by the device.
 *
 * @return a string corresponding to the IP address of the NTP server to be used by the device
 *
 * On failure, throws an exception or returns YNetwork::NTPSERVER_INVALID.
 */
string YNetwork::get_ntpServer(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YNetwork::NTPSERVER_INVALID;
                }
            }
        }
        res = _ntpServer;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the IP address of the NTP server to be used by the module. Use an empty
 * string to restore the factory set  address.
 * Remember to call the saveToFlash() method and then to reboot the module to apply this setting.
 *
 * @param newval : a string corresponding to the IP address of the NTP server to be used by the module
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::set_ntpServer(const string& newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = newval;
        res = _setAttr("ntpServer", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns a hash string if a password has been set for "user" user,
 * or an empty string otherwise.
 *
 * @return a string corresponding to a hash string if a password has been set for "user" user,
 *         or an empty string otherwise
 *
 * On failure, throws an exception or returns YNetwork::USERPASSWORD_INVALID.
 */
string YNetwork::get_userPassword(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YNetwork::USERPASSWORD_INVALID;
                }
            }
        }
        res = _userPassword;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the password for the "user" user. This password becomes instantly required
 * to perform any use of the module. If the specified value is an
 * empty string, a password is not required anymore.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : a string corresponding to the password for the "user" user
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::set_userPassword(const string& newval)
{
    string rest_val;
    int res;
    if (newval.length() > YAPI_HASH_BUF_SIZE) {
        _throw(YAPI_INVALID_ARGUMENT, "Password too long :" + newval);
        return YAPI_INVALID_ARGUMENT;
    }
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = newval;
        res = _setAttr("userPassword", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns a hash string if a password has been set for user "admin",
 * or an empty string otherwise.
 *
 * @return a string corresponding to a hash string if a password has been set for user "admin",
 *         or an empty string otherwise
 *
 * On failure, throws an exception or returns YNetwork::ADMINPASSWORD_INVALID.
 */
string YNetwork::get_adminPassword(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YNetwork::ADMINPASSWORD_INVALID;
                }
            }
        }
        res = _adminPassword;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the password for the "admin" user. This password becomes instantly required
 * to perform any change of the module state. If the specified value is an
 * empty string, a password is not required anymore.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : a string corresponding to the password for the "admin" user
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::set_adminPassword(const string& newval)
{
    string rest_val;
    int res;
    if (newval.length() > YAPI_HASH_BUF_SIZE) {
        _throw(YAPI_INVALID_ARGUMENT, "Password too long :" + newval);
        return YAPI_INVALID_ARGUMENT;
    }
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = newval;
        res = _setAttr("adminPassword", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the TCP port used to serve the hub web UI.
 *
 * @return an integer corresponding to the TCP port used to serve the hub web UI
 *
 * On failure, throws an exception or returns YNetwork::HTTPPORT_INVALID.
 */
int YNetwork::get_httpPort(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YNetwork::HTTPPORT_INVALID;
                }
            }
        }
        res = _httpPort;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the the TCP port used to serve the hub web UI. The default value is port 80,
 * which is the default for all Web servers. Regardless of the value set here,
 * the hub will always reply on port 4444, which is used by default by Yoctopuce
 * API library. When you change this parameter, remember to call the saveToFlash()
 * method of the module if the modification must be kept.
 *
 * @param newval : an integer corresponding to the the TCP port used to serve the hub web UI
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::set_httpPort(int newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("httpPort", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the secure TCP port used to serve the hub web UI.
 *
 * @return an integer corresponding to the secure TCP port used to serve the hub web UI
 *
 * On failure, throws an exception or returns YNetwork::HTTPSPORT_INVALID.
 */
int YNetwork::get_httpsPort(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YNetwork::HTTPSPORT_INVALID;
                }
            }
        }
        res = _httpsPort;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the secure TCP port used to serve the hub web UI. The default value is port 4443,
 * which is the default for all Web servers. When you change this parameter, remember to call the saveToFlash()
 * method of the module if the modification must be kept.
 *
 * @param newval : an integer corresponding to the secure TCP port used to serve the hub web UI
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::set_httpsPort(int newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("httpsPort", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the HTML page to serve for the URL "/"" of the hub.
 *
 * @return a string corresponding to the HTML page to serve for the URL "/"" of the hub
 *
 * On failure, throws an exception or returns YNetwork::DEFAULTPAGE_INVALID.
 */
string YNetwork::get_defaultPage(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YNetwork::DEFAULTPAGE_INVALID;
                }
            }
        }
        res = _defaultPage;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the default HTML page returned by the hub. If not value are set the hub return
 * "index.html" which is the web interface of the hub. It is possible to change this page
 * for file that has been uploaded on the hub. The maximum filename size is 15 characters.
 * When you change this parameter, remember to call the saveToFlash()
 * method of the module if the modification must be kept.
 *
 * @param newval : a string corresponding to the default HTML page returned by the hub
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::set_defaultPage(const string& newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = newval;
        res = _setAttr("defaultPage", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the activation state of the multicast announce protocols to allow easy
 * discovery of the module in the network neighborhood (uPnP/Bonjour protocol).
 *
 * @return either YNetwork::DISCOVERABLE_FALSE or YNetwork::DISCOVERABLE_TRUE, according to the
 * activation state of the multicast announce protocols to allow easy
 *         discovery of the module in the network neighborhood (uPnP/Bonjour protocol)
 *
 * On failure, throws an exception or returns YNetwork::DISCOVERABLE_INVALID.
 */
Y_DISCOVERABLE_enum YNetwork::get_discoverable(void)
{
    Y_DISCOVERABLE_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YNetwork::DISCOVERABLE_INVALID;
                }
            }
        }
        res = _discoverable;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the activation state of the multicast announce protocols to allow easy
 * discovery of the module in the network neighborhood (uPnP/Bonjour protocol).
 * Remember to call the saveToFlash()
 * method of the module if the modification must be kept.
 *
 * @param newval : either YNetwork::DISCOVERABLE_FALSE or YNetwork::DISCOVERABLE_TRUE, according to the
 * activation state of the multicast announce protocols to allow easy
 *         discovery of the module in the network neighborhood (uPnP/Bonjour protocol)
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::set_discoverable(Y_DISCOVERABLE_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = (newval>0 ? "1" : "0");
        res = _setAttr("discoverable", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the allowed downtime of the WWW link (in seconds) before triggering an automated
 * reboot to try to recover Internet connectivity. A zero value disables automated reboot
 * in case of Internet connectivity loss.
 *
 * @return an integer corresponding to the allowed downtime of the WWW link (in seconds) before
 * triggering an automated
 *         reboot to try to recover Internet connectivity
 *
 * On failure, throws an exception or returns YNetwork::WWWWATCHDOGDELAY_INVALID.
 */
int YNetwork::get_wwwWatchdogDelay(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YNetwork::WWWWATCHDOGDELAY_INVALID;
                }
            }
        }
        res = _wwwWatchdogDelay;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the allowed downtime of the WWW link (in seconds) before triggering an automated
 * reboot to try to recover Internet connectivity. A zero value disables automated reboot
 * in case of Internet connectivity loss. The smallest valid non-zero timeout is
 * 90 seconds. Remember to call the saveToFlash()
 * method of the module if the modification must be kept.
 *
 * @param newval : an integer corresponding to the allowed downtime of the WWW link (in seconds)
 * before triggering an automated
 *         reboot to try to recover Internet connectivity
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::set_wwwWatchdogDelay(int newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("wwwWatchdogDelay", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the callback URL to notify of significant state changes.
 *
 * @return a string corresponding to the callback URL to notify of significant state changes
 *
 * On failure, throws an exception or returns YNetwork::CALLBACKURL_INVALID.
 */
string YNetwork::get_callbackUrl(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YNetwork::CALLBACKURL_INVALID;
                }
            }
        }
        res = _callbackUrl;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the callback URL to notify significant state changes. Remember to call the
 * saveToFlash() method of the module if the modification must be kept.
 *
 * @param newval : a string corresponding to the callback URL to notify significant state changes
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::set_callbackUrl(const string& newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = newval;
        res = _setAttr("callbackUrl", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the HTTP method used to notify callbacks for significant state changes.
 *
 * @return a value among YNetwork::CALLBACKMETHOD_POST, YNetwork::CALLBACKMETHOD_GET and
 * YNetwork::CALLBACKMETHOD_PUT corresponding to the HTTP method used to notify callbacks for
 * significant state changes
 *
 * On failure, throws an exception or returns YNetwork::CALLBACKMETHOD_INVALID.
 */
Y_CALLBACKMETHOD_enum YNetwork::get_callbackMethod(void)
{
    Y_CALLBACKMETHOD_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YNetwork::CALLBACKMETHOD_INVALID;
                }
            }
        }
        res = _callbackMethod;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the HTTP method used to notify callbacks for significant state changes.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : a value among YNetwork::CALLBACKMETHOD_POST, YNetwork::CALLBACKMETHOD_GET and
 * YNetwork::CALLBACKMETHOD_PUT corresponding to the HTTP method used to notify callbacks for
 * significant state changes
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::set_callbackMethod(Y_CALLBACKMETHOD_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("callbackMethod", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the encoding standard to use for representing notification values.
 *
 * @return a value among YNetwork::CALLBACKENCODING_FORM, YNetwork::CALLBACKENCODING_JSON,
 * YNetwork::CALLBACKENCODING_JSON_ARRAY, YNetwork::CALLBACKENCODING_CSV,
 * YNetwork::CALLBACKENCODING_YOCTO_API, YNetwork::CALLBACKENCODING_JSON_NUM,
 * YNetwork::CALLBACKENCODING_EMONCMS, YNetwork::CALLBACKENCODING_AZURE,
 * YNetwork::CALLBACKENCODING_INFLUXDB, YNetwork::CALLBACKENCODING_MQTT,
 * YNetwork::CALLBACKENCODING_YOCTO_API_JZON, YNetwork::CALLBACKENCODING_PRTG and
 * YNetwork::CALLBACKENCODING_INFLUXDB_V2 corresponding to the encoding standard to use for
 * representing notification values
 *
 * On failure, throws an exception or returns YNetwork::CALLBACKENCODING_INVALID.
 */
Y_CALLBACKENCODING_enum YNetwork::get_callbackEncoding(void)
{
    Y_CALLBACKENCODING_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YNetwork::CALLBACKENCODING_INVALID;
                }
            }
        }
        res = _callbackEncoding;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the encoding standard to use for representing notification values.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : a value among YNetwork::CALLBACKENCODING_FORM, YNetwork::CALLBACKENCODING_JSON,
 * YNetwork::CALLBACKENCODING_JSON_ARRAY, YNetwork::CALLBACKENCODING_CSV,
 * YNetwork::CALLBACKENCODING_YOCTO_API, YNetwork::CALLBACKENCODING_JSON_NUM,
 * YNetwork::CALLBACKENCODING_EMONCMS, YNetwork::CALLBACKENCODING_AZURE,
 * YNetwork::CALLBACKENCODING_INFLUXDB, YNetwork::CALLBACKENCODING_MQTT,
 * YNetwork::CALLBACKENCODING_YOCTO_API_JZON, YNetwork::CALLBACKENCODING_PRTG and
 * YNetwork::CALLBACKENCODING_INFLUXDB_V2 corresponding to the encoding standard to use for
 * representing notification values
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::set_callbackEncoding(Y_CALLBACKENCODING_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("callbackEncoding", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the activation state of the custom template file to customize callback
 * format. If the custom callback template is disabled, it will be ignored even
 * if present on the YoctoHub.
 *
 * @return either YNetwork::CALLBACKTEMPLATE_OFF or YNetwork::CALLBACKTEMPLATE_ON, according to the
 * activation state of the custom template file to customize callback
 *         format
 *
 * On failure, throws an exception or returns YNetwork::CALLBACKTEMPLATE_INVALID.
 */
Y_CALLBACKTEMPLATE_enum YNetwork::get_callbackTemplate(void)
{
    Y_CALLBACKTEMPLATE_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YNetwork::CALLBACKTEMPLATE_INVALID;
                }
            }
        }
        res = _callbackTemplate;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Enable the use of a template file to customize callbacks format.
 * When the custom callback template file is enabled, the template file
 * will be loaded for each callback in order to build the data to post to the
 * server. If template file does not exist on the YoctoHub, the callback will
 * fail with an error message indicating the name of the expected template file.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : either YNetwork::CALLBACKTEMPLATE_OFF or YNetwork::CALLBACKTEMPLATE_ON
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::set_callbackTemplate(Y_CALLBACKTEMPLATE_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = (newval>0 ? "1" : "0");
        res = _setAttr("callbackTemplate", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns a hashed version of the notification callback credentials if set,
 * or an empty string otherwise.
 *
 * @return a string corresponding to a hashed version of the notification callback credentials if set,
 *         or an empty string otherwise
 *
 * On failure, throws an exception or returns YNetwork::CALLBACKCREDENTIALS_INVALID.
 */
string YNetwork::get_callbackCredentials(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YNetwork::CALLBACKCREDENTIALS_INVALID;
                }
            }
        }
        res = _callbackCredentials;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the credentials required to connect to the callback address. The credentials
 * must be provided as returned by function get_callbackCredentials,
 * in the form username:hash. The method used to compute the hash varies according
 * to the the authentication scheme implemented by the callback, For Basic authentication,
 * the hash is the MD5 of the string username:password. For Digest authentication,
 * the hash is the MD5 of the string username:realm:password. For a simpler
 * way to configure callback credentials, use function callbackLogin instead.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : a string corresponding to the credentials required to connect to the callback address
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::set_callbackCredentials(const string& newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = newval;
        res = _setAttr("callbackCredentials", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Connects to the notification callback and saves the credentials required to
 * log into it. The password is not stored into the module, only a hashed
 * copy of the credentials are saved. Remember to call the
 * saveToFlash() method of the module if the modification must be kept.
 *
 * @param username : username required to log to the callback
 * @param password : password required to log to the callback
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::callbackLogin(string username,string password)
{
    string rest_val;
    rest_val = string(username)+string(":")+string(password);
    return _setAttr("callbackCredentials", rest_val);
}

/**
 * Returns the initial waiting time before first callback notifications, in seconds.
 *
 * @return an integer corresponding to the initial waiting time before first callback notifications, in seconds
 *
 * On failure, throws an exception or returns YNetwork::CALLBACKINITIALDELAY_INVALID.
 */
int YNetwork::get_callbackInitialDelay(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YNetwork::CALLBACKINITIALDELAY_INVALID;
                }
            }
        }
        res = _callbackInitialDelay;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the initial waiting time before first callback notifications, in seconds.
 * Remember to call the saveToFlash() method of the module if the modification must be kept.
 *
 * @param newval : an integer corresponding to the initial waiting time before first callback
 * notifications, in seconds
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::set_callbackInitialDelay(int newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("callbackInitialDelay", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the HTTP callback schedule strategy, as a text string.
 *
 * @return a string corresponding to the HTTP callback schedule strategy, as a text string
 *
 * On failure, throws an exception or returns YNetwork::CALLBACKSCHEDULE_INVALID.
 */
string YNetwork::get_callbackSchedule(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YNetwork::CALLBACKSCHEDULE_INVALID;
                }
            }
        }
        res = _callbackSchedule;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the HTTP callback schedule strategy, as a text string.
 * Remember to call the saveToFlash()
 * method of the module if the modification must be kept.
 *
 * @param newval : a string corresponding to the HTTP callback schedule strategy, as a text string
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::set_callbackSchedule(const string& newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = newval;
        res = _setAttr("callbackSchedule", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the minimum waiting time between two HTTP callbacks, in seconds.
 *
 * @return an integer corresponding to the minimum waiting time between two HTTP callbacks, in seconds
 *
 * On failure, throws an exception or returns YNetwork::CALLBACKMINDELAY_INVALID.
 */
int YNetwork::get_callbackMinDelay(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YNetwork::CALLBACKMINDELAY_INVALID;
                }
            }
        }
        res = _callbackMinDelay;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the minimum waiting time between two HTTP callbacks, in seconds.
 * Remember to call the saveToFlash() method of the module if the modification must be kept.
 *
 * @param newval : an integer corresponding to the minimum waiting time between two HTTP callbacks, in seconds
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::set_callbackMinDelay(int newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("callbackMinDelay", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the waiting time between two HTTP callbacks when there is nothing new.
 *
 * @return an integer corresponding to the waiting time between two HTTP callbacks when there is nothing new
 *
 * On failure, throws an exception or returns YNetwork::CALLBACKMAXDELAY_INVALID.
 */
int YNetwork::get_callbackMaxDelay(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YNetwork::CALLBACKMAXDELAY_INVALID;
                }
            }
        }
        res = _callbackMaxDelay;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the waiting time between two HTTP callbacks when there is nothing new.
 * Remember to call the saveToFlash() method of the module if the modification must be kept.
 *
 * @param newval : an integer corresponding to the waiting time between two HTTP callbacks when there
 * is nothing new
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::set_callbackMaxDelay(int newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("callbackMaxDelay", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the current consumed by the module from Power-over-Ethernet (PoE), in milliamps.
 * The current consumption is measured after converting PoE source to 5 Volt, and should
 * never exceed 1800 mA.
 *
 * @return an integer corresponding to the current consumed by the module from Power-over-Ethernet
 * (PoE), in milliamps
 *
 * On failure, throws an exception or returns YNetwork::POECURRENT_INVALID.
 */
int YNetwork::get_poeCurrent(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YNetwork::POECURRENT_INVALID;
                }
            }
        }
        res = _poeCurrent;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Retrieves a network interface for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that the network interface is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the network interface is
 * indeed online at a given time. In case of ambiguity when looking for
 * a network interface by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the network interface, for instance
 *         YHUBETH1.network.
 *
 * @return a YNetwork object allowing you to drive the network interface.
 */
YNetwork* YNetwork::FindNetwork(string func)
{
    YNetwork* obj = NULL;
    int taken = 0;
    if (YAPI::_apiInitialized) {
        yEnterCriticalSection(&YAPI::_global_cs);
        taken = 1;
    }try {
        obj = (YNetwork*) YFunction::_FindFromCache("Network", func);
        if (obj == NULL) {
            obj = new YNetwork(func);
            YFunction::_AddToCache("Network", func, obj);
        }
    } catch (std::exception &) {
        if (taken) yLeaveCriticalSection(&YAPI::_global_cs);
        throw;
    }
    if (taken) yLeaveCriticalSection(&YAPI::_global_cs);
    return obj;
}

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
int YNetwork::registerValueCallback(YNetworkValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackNetwork = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YNetwork::_invokeValueCallback(string value)
{
    if (_valueCallbackNetwork != NULL) {
        _valueCallbackNetwork(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

/**
 * Changes the configuration of the network interface to enable the use of an
 * IP address received from a DHCP server. Until an address is received from a DHCP
 * server, the module uses the IP parameters specified to this function.
 * Remember to call the saveToFlash() method and then to reboot the module to apply this setting.
 *
 * @param fallbackIpAddr : fallback IP address, to be used when no DHCP reply is received
 * @param fallbackSubnetMaskLen : fallback subnet mask length when no DHCP reply is received, as an
 *         integer (e.g. 24 means 255.255.255.0)
 * @param fallbackRouter : fallback router IP address, to be used when no DHCP reply is received
 *
 * @return YAPI::SUCCESS when the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::useDHCP(string fallbackIpAddr,int fallbackSubnetMaskLen,string fallbackRouter)
{
    return this->set_ipConfig(YapiWrapper::ysprintf("DHCP:%s/%d/%s", fallbackIpAddr.c_str(), fallbackSubnetMaskLen,fallbackRouter.c_str()));
}

/**
 * Changes the configuration of the network interface to enable the use of an
 * IP address received from a DHCP server. Until an address is received from a DHCP
 * server, the module uses an IP of the network 169.254.0.0/16 (APIPA).
 * Remember to call the saveToFlash() method and then to reboot the module to apply this setting.
 *
 * @return YAPI::SUCCESS when the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::useDHCPauto(void)
{
    return this->set_ipConfig("DHCP:");
}

/**
 * Changes the configuration of the network interface to use a static IP address.
 * Remember to call the saveToFlash() method and then to reboot the module to apply this setting.
 *
 * @param ipAddress : device IP address
 * @param subnetMaskLen : subnet mask length, as an integer (e.g. 24 means 255.255.255.0)
 * @param router : router IP address (default gateway)
 *
 * @return YAPI::SUCCESS when the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::useStaticIP(string ipAddress,int subnetMaskLen,string router)
{
    return this->set_ipConfig(YapiWrapper::ysprintf("STATIC:%s/%d/%s", ipAddress.c_str(), subnetMaskLen,router.c_str()));
}

/**
 * Pings host to test the network connectivity. Sends four ICMP ECHO_REQUEST requests from the
 * module to the target host. This method returns a string with the result of the
 * 4 ICMP ECHO_REQUEST requests.
 *
 * @param host : the hostname or the IP address of the target
 *
 * @return a string with the result of the ping.
 */
string YNetwork::ping(string host)
{
    string content;

    content = this->_download(YapiWrapper::ysprintf("ping.txt?host=%s",host.c_str()));
    return content;
}

/**
 * Trigger an HTTP callback quickly. This function can even be called within
 * an HTTP callback, in which case the next callback will be triggered 5 seconds
 * after the end of the current callback, regardless if the minimum time between
 * callbacks configured in the device.
 *
 * @return YAPI::SUCCESS when the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::triggerCallback(void)
{
    return this->set_callbackMethod(this->get_callbackMethod());
}

/**
 * Setup periodic HTTP callbacks (simplified function).
 *
 * @param interval : a string representing the callback periodicity, expressed in
 *         seconds, minutes or hours, eg. "60s", "5m", "1h", "48h".
 * @param offset : an integer representing the time offset relative to the period
 *         when the callback should occur. For instance, if the periodicity is
 *         24h, an offset of 7 will make the callback occur each day at 7AM.
 *
 * @return YAPI::SUCCESS when the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::set_periodicCallbackSchedule(string interval,int offset)
{
    return this->set_callbackSchedule(YapiWrapper::ysprintf("every %s+%d",interval.c_str(),offset));
}

YNetwork *YNetwork::nextNetwork(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YNetwork::FindNetwork(hwid);
}

YNetwork *YNetwork::FirstNetwork(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("Network", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YNetwork::FindNetwork(serial+"."+funcId);
}

//--- (end of YNetwork implementation)

//--- (YNetwork functions)
//--- (end of YNetwork functions)
