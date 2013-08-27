/*********************************************************************
 *
 * $Id: yocto_network.cpp 12337 2013-08-14 15:22:22Z mvuilleu $
 *
 * Implements yFindNetwork(), the high-level API for Network functions
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


#define _CRT_SECURE_NO_DEPRECATE //do not use windows secure crt
#include "yocto_network.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

//--- (YNetwork constructor)
// Constructor is protected, use yFindNetwork factory function to instantiate
YNetwork::YNetwork(const string& func): YFunction("Network", func)
//--- (end of YNetwork constructor)
//--- (Network initialization)
            ,_callback(NULL)
            ,_logicalName(Y_LOGICALNAME_INVALID)
            ,_advertisedValue(Y_ADVERTISEDVALUE_INVALID)
            ,_readiness(Y_READINESS_INVALID)
            ,_macAddress(Y_MACADDRESS_INVALID)
            ,_ipAddress(Y_IPADDRESS_INVALID)
            ,_subnetMask(Y_SUBNETMASK_INVALID)
            ,_router(Y_ROUTER_INVALID)
            ,_ipConfig(Y_IPCONFIG_INVALID)
            ,_primaryDNS(Y_PRIMARYDNS_INVALID)
            ,_secondaryDNS(Y_SECONDARYDNS_INVALID)
            ,_userPassword(Y_USERPASSWORD_INVALID)
            ,_adminPassword(Y_ADMINPASSWORD_INVALID)
            ,_discoverable(Y_DISCOVERABLE_INVALID)
            ,_wwwWatchdogDelay(Y_WWWWATCHDOGDELAY_INVALID)
            ,_callbackUrl(Y_CALLBACKURL_INVALID)
            ,_callbackMethod(Y_CALLBACKMETHOD_INVALID)
            ,_callbackEncoding(Y_CALLBACKENCODING_INVALID)
            ,_callbackCredentials(Y_CALLBACKCREDENTIALS_INVALID)
            ,_callbackMinDelay(Y_CALLBACKMINDELAY_INVALID)
            ,_callbackMaxDelay(Y_CALLBACKMAXDELAY_INVALID)
            ,_poeCurrent(Y_POECURRENT_INVALID)
//--- (end of Network initialization)
{}

YNetwork::~YNetwork() 
{
//--- (YNetwork cleanup)
//--- (end of YNetwork cleanup)
}
//--- (YNetwork implementation)

const string YNetwork::LOGICALNAME_INVALID = "!INVALID!";
const string YNetwork::ADVERTISEDVALUE_INVALID = "!INVALID!";
const string YNetwork::MACADDRESS_INVALID = "!INVALID!";
const string YNetwork::IPADDRESS_INVALID = "!INVALID!";
const string YNetwork::SUBNETMASK_INVALID = "!INVALID!";
const string YNetwork::ROUTER_INVALID = "!INVALID!";
const string YNetwork::IPCONFIG_INVALID = "!INVALID!";
const string YNetwork::PRIMARYDNS_INVALID = "!INVALID!";
const string YNetwork::SECONDARYDNS_INVALID = "!INVALID!";
const string YNetwork::USERPASSWORD_INVALID = "!INVALID!";
const string YNetwork::ADMINPASSWORD_INVALID = "!INVALID!";
const string YNetwork::CALLBACKURL_INVALID = "!INVALID!";
const string YNetwork::CALLBACKCREDENTIALS_INVALID = "!INVALID!";



int YNetwork::_parse(yJsonStateMachine& j)
{
    if(yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_STRUCT) {
    failed:
        return -1;
    }
    while(yJsonParse(&j) == YJSON_PARSE_AVAIL && j.st == YJSON_PARSE_MEMBNAME) {
        if(!strcmp(j.token, "logicalName")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _logicalName =  _parseString(j);
        } else if(!strcmp(j.token, "advertisedValue")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _advertisedValue =  _parseString(j);
        } else if(!strcmp(j.token, "readiness")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _readiness =  (Y_READINESS_enum)atoi(j.token);
        } else if(!strcmp(j.token, "macAddress")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _macAddress =  _parseString(j);
        } else if(!strcmp(j.token, "ipAddress")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _ipAddress =  _parseString(j);
        } else if(!strcmp(j.token, "subnetMask")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _subnetMask =  _parseString(j);
        } else if(!strcmp(j.token, "router")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _router =  _parseString(j);
        } else if(!strcmp(j.token, "ipConfig")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _ipConfig =  _parseString(j);
        } else if(!strcmp(j.token, "primaryDNS")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _primaryDNS =  _parseString(j);
        } else if(!strcmp(j.token, "secondaryDNS")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _secondaryDNS =  _parseString(j);
        } else if(!strcmp(j.token, "userPassword")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _userPassword =  _parseString(j);
        } else if(!strcmp(j.token, "adminPassword")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _adminPassword =  _parseString(j);
        } else if(!strcmp(j.token, "discoverable")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _discoverable =  (Y_DISCOVERABLE_enum)atoi(j.token);
        } else if(!strcmp(j.token, "wwwWatchdogDelay")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _wwwWatchdogDelay =  atoi(j.token);
        } else if(!strcmp(j.token, "callbackUrl")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _callbackUrl =  _parseString(j);
        } else if(!strcmp(j.token, "callbackMethod")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _callbackMethod =  (Y_CALLBACKMETHOD_enum)atoi(j.token);
        } else if(!strcmp(j.token, "callbackEncoding")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _callbackEncoding =  (Y_CALLBACKENCODING_enum)atoi(j.token);
        } else if(!strcmp(j.token, "callbackCredentials")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _callbackCredentials =  _parseString(j);
        } else if(!strcmp(j.token, "callbackMinDelay")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _callbackMinDelay =  atoi(j.token);
        } else if(!strcmp(j.token, "callbackMaxDelay")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _callbackMaxDelay =  atoi(j.token);
        } else if(!strcmp(j.token, "poeCurrent")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _poeCurrent =  atoi(j.token);
        } else {
            // ignore unknown field
            yJsonSkip(&j, 1);
        }
    }
    if(j.st != YJSON_PARSE_STRUCT) goto failed;
    return 0;
}

/**
 * Returns the logical name of the network interface, corresponding to the network name of the module.
 * 
 * @return a string corresponding to the logical name of the network interface, corresponding to the
 * network name of the module
 * 
 * On failure, throws an exception or returns Y_LOGICALNAME_INVALID.
 */
string YNetwork::get_logicalName(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_LOGICALNAME_INVALID;
    }
    return _logicalName;
}

/**
 * Changes the logical name of the network interface, corresponding to the network name of the module.
 * You can use yCheckLogicalName()
 * prior to this call to make sure that your parameter is valid.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 * 
 * @param newval : a string corresponding to the logical name of the network interface, corresponding
 * to the network name of the module
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::set_logicalName(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("logicalName", rest_val);
}

/**
 * Returns the current value of the network interface (no more than 6 characters).
 * 
 * @return a string corresponding to the current value of the network interface (no more than 6 characters)
 * 
 * On failure, throws an exception or returns Y_ADVERTISEDVALUE_INVALID.
 */
string YNetwork::get_advertisedValue(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_ADVERTISEDVALUE_INVALID;
    }
    return _advertisedValue;
}

/**
 * Returns the current established working mode of the network interface.
 * Level zero (DOWN_0) means that no hardware link has been detected. Either there is no signal
 * on the network cable, or the selected wireless access point cannot be detected.
 * Level 1 (LIVE_1) is reached when the network is detected, but is not yet connected,
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
 * @return a value among Y_READINESS_DOWN, Y_READINESS_EXISTS, Y_READINESS_LINKED, Y_READINESS_LAN_OK
 * and Y_READINESS_WWW_OK corresponding to the current established working mode of the network interface
 * 
 * On failure, throws an exception or returns Y_READINESS_INVALID.
 */
Y_READINESS_enum YNetwork::get_readiness(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_READINESS_INVALID;
    }
    return _readiness;
}

/**
 * Returns the MAC address of the network interface. The MAC address is also available on a sticker
 * on the module, in both numeric and barcode forms.
 * 
 * @return a string corresponding to the MAC address of the network interface
 * 
 * On failure, throws an exception or returns Y_MACADDRESS_INVALID.
 */
string YNetwork::get_macAddress(void)
{
    if(_macAddress == Y_MACADDRESS_INVALID) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_MACADDRESS_INVALID;
    }
    return _macAddress;
}

/**
 * Returns the IP address currently in use by the device. The adress may have been configured
 * statically, or provided by a DHCP server.
 * 
 * @return a string corresponding to the IP address currently in use by the device
 * 
 * On failure, throws an exception or returns Y_IPADDRESS_INVALID.
 */
string YNetwork::get_ipAddress(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_IPADDRESS_INVALID;
    }
    return _ipAddress;
}

/**
 * Returns the subnet mask currently used by the device.
 * 
 * @return a string corresponding to the subnet mask currently used by the device
 * 
 * On failure, throws an exception or returns Y_SUBNETMASK_INVALID.
 */
string YNetwork::get_subnetMask(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_SUBNETMASK_INVALID;
    }
    return _subnetMask;
}

/**
 * Returns the IP address of the router on the device subnet (default gateway).
 * 
 * @return a string corresponding to the IP address of the router on the device subnet (default gateway)
 * 
 * On failure, throws an exception or returns Y_ROUTER_INVALID.
 */
string YNetwork::get_router(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_ROUTER_INVALID;
    }
    return _router;
}

string YNetwork::get_ipConfig(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_IPCONFIG_INVALID;
    }
    return _ipConfig;
}

int YNetwork::set_ipConfig(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("ipConfig", rest_val);
}

/**
 * Changes the configuration of the network interface to enable the use of an
 * IP address received from a DHCP server. Until an address is received from a DHCP
 * server, the module uses the IP parameters specified to this function.
 * Remember to call the saveToFlash() method and then to reboot the module to apply this setting.
 * 
 * @param fallbackIpAddr : fallback IP address, to be used when no DHCP reply is received
 * @param fallbackSubnetMaskLen : fallback subnet mask length when no DHCP reply is received, as an
 *         integer (eg. 24 means 255.255.255.0)
 * @param fallbackRouter : fallback router IP address, to be used when no DHCP reply is received
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::useDHCP(string fallbackIpAddr,int fallbackSubnetMaskLen,string fallbackRouter)
{
    string rest_val;
    char buff[128]; sprintf(buff,"DHCP:%s/%d/%s", fallbackIpAddr.c_str(), fallbackSubnetMaskLen, fallbackRouter.c_str()); rest_val = string(buff);
    return _setAttr("ipConfig", rest_val);
}

/**
 * Changes the configuration of the network interface to use a static IP address.
 * Remember to call the saveToFlash() method and then to reboot the module to apply this setting.
 * 
 * @param ipAddress : device IP address
 * @param subnetMaskLen : subnet mask length, as an integer (eg. 24 means 255.255.255.0)
 * @param router : router IP address (default gateway)
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::useStaticIP(string ipAddress,int subnetMaskLen,string router)
{
    string rest_val;
    char buff[128]; sprintf(buff,"STATIC:%s/%d/%s", ipAddress.c_str(), subnetMaskLen, router.c_str()); rest_val = string(buff);
    return _setAttr("ipConfig", rest_val);
}

/**
 * Returns the IP address of the primary name server to be used by the module.
 * 
 * @return a string corresponding to the IP address of the primary name server to be used by the module
 * 
 * On failure, throws an exception or returns Y_PRIMARYDNS_INVALID.
 */
string YNetwork::get_primaryDNS(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_PRIMARYDNS_INVALID;
    }
    return _primaryDNS;
}

/**
 * Changes the IP address of the primary name server to be used by the module.
 * When using DHCP, if a value is specified, it overrides the value received from the DHCP server.
 * Remember to call the saveToFlash() method and then to reboot the module to apply this setting.
 * 
 * @param newval : a string corresponding to the IP address of the primary name server to be used by the module
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::set_primaryDNS(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("primaryDNS", rest_val);
}

/**
 * Returns the IP address of the secondary name server to be used by the module.
 * 
 * @return a string corresponding to the IP address of the secondary name server to be used by the module
 * 
 * On failure, throws an exception or returns Y_SECONDARYDNS_INVALID.
 */
string YNetwork::get_secondaryDNS(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_SECONDARYDNS_INVALID;
    }
    return _secondaryDNS;
}

/**
 * Changes the IP address of the secondarz name server to be used by the module.
 * When using DHCP, if a value is specified, it overrides the value received from the DHCP server.
 * Remember to call the saveToFlash() method and then to reboot the module to apply this setting.
 * 
 * @param newval : a string corresponding to the IP address of the secondarz name server to be used by the module
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::set_secondaryDNS(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("secondaryDNS", rest_val);
}

/**
 * Returns a hash string if a password has been set for "user" user,
 * or an empty string otherwise.
 * 
 * @return a string corresponding to a hash string if a password has been set for "user" user,
 *         or an empty string otherwise
 * 
 * On failure, throws an exception or returns Y_USERPASSWORD_INVALID.
 */
string YNetwork::get_userPassword(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_USERPASSWORD_INVALID;
    }
    return _userPassword;
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
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::set_userPassword(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("userPassword", rest_val);
}

/**
 * Returns a hash string if a password has been set for user "admin",
 * or an empty string otherwise.
 * 
 * @return a string corresponding to a hash string if a password has been set for user "admin",
 *         or an empty string otherwise
 * 
 * On failure, throws an exception or returns Y_ADMINPASSWORD_INVALID.
 */
string YNetwork::get_adminPassword(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_ADMINPASSWORD_INVALID;
    }
    return _adminPassword;
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
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::set_adminPassword(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("adminPassword", rest_val);
}

/**
 * Returns the activation state of the multicast announce protocols to allow easy
 * discovery of the module in the network neighborhood (uPnP/Bonjour protocol).
 * 
 * @return either Y_DISCOVERABLE_FALSE or Y_DISCOVERABLE_TRUE, according to the activation state of
 * the multicast announce protocols to allow easy
 *         discovery of the module in the network neighborhood (uPnP/Bonjour protocol)
 * 
 * On failure, throws an exception or returns Y_DISCOVERABLE_INVALID.
 */
Y_DISCOVERABLE_enum YNetwork::get_discoverable(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_DISCOVERABLE_INVALID;
    }
    return _discoverable;
}

/**
 * Changes the activation state of the multicast announce protocols to allow easy
 * discovery of the module in the network neighborhood (uPnP/Bonjour protocol).
 * 
 * @param newval : either Y_DISCOVERABLE_FALSE or Y_DISCOVERABLE_TRUE, according to the activation
 * state of the multicast announce protocols to allow easy
 *         discovery of the module in the network neighborhood (uPnP/Bonjour protocol)
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::set_discoverable(Y_DISCOVERABLE_enum newval)
{
    string rest_val;
    rest_val = (newval>0 ? "1" : "0");
    return _setAttr("discoverable", rest_val);
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
 * On failure, throws an exception or returns Y_WWWWATCHDOGDELAY_INVALID.
 */
unsigned YNetwork::get_wwwWatchdogDelay(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_WWWWATCHDOGDELAY_INVALID;
    }
    return _wwwWatchdogDelay;
}

/**
 * Changes the allowed downtime of the WWW link (in seconds) before triggering an automated
 * reboot to try to recover Internet connectivity. A zero value disable automated reboot
 * in case of Internet connectivity loss. The smallest valid non-zero timeout is
 * 90 seconds.
 * 
 * @param newval : an integer corresponding to the allowed downtime of the WWW link (in seconds)
 * before triggering an automated
 *         reboot to try to recover Internet connectivity
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::set_wwwWatchdogDelay(unsigned newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
    return _setAttr("wwwWatchdogDelay", rest_val);
}

/**
 * Returns the callback URL to notify of significant state changes.
 * 
 * @return a string corresponding to the callback URL to notify of significant state changes
 * 
 * On failure, throws an exception or returns Y_CALLBACKURL_INVALID.
 */
string YNetwork::get_callbackUrl(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_CALLBACKURL_INVALID;
    }
    return _callbackUrl;
}

/**
 * Changes the callback URL to notify significant state changes. Remember to call the
 * saveToFlash() method of the module if the modification must be kept.
 * 
 * @param newval : a string corresponding to the callback URL to notify significant state changes
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::set_callbackUrl(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("callbackUrl", rest_val);
}

/**
 * Returns the HTTP method used to notify callbacks for significant state changes.
 * 
 * @return a value among Y_CALLBACKMETHOD_POST, Y_CALLBACKMETHOD_GET and Y_CALLBACKMETHOD_PUT
 * corresponding to the HTTP method used to notify callbacks for significant state changes
 * 
 * On failure, throws an exception or returns Y_CALLBACKMETHOD_INVALID.
 */
Y_CALLBACKMETHOD_enum YNetwork::get_callbackMethod(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_CALLBACKMETHOD_INVALID;
    }
    return _callbackMethod;
}

/**
 * Changes the HTTP method used to notify callbacks for significant state changes.
 * 
 * @param newval : a value among Y_CALLBACKMETHOD_POST, Y_CALLBACKMETHOD_GET and Y_CALLBACKMETHOD_PUT
 * corresponding to the HTTP method used to notify callbacks for significant state changes
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::set_callbackMethod(Y_CALLBACKMETHOD_enum newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
    return _setAttr("callbackMethod", rest_val);
}

/**
 * Returns the encoding standard to use for representing notification values.
 * 
 * @return a value among Y_CALLBACKENCODING_FORM, Y_CALLBACKENCODING_JSON,
 * Y_CALLBACKENCODING_JSON_ARRAY, Y_CALLBACKENCODING_CSV and Y_CALLBACKENCODING_YOCTO_API
 * corresponding to the encoding standard to use for representing notification values
 * 
 * On failure, throws an exception or returns Y_CALLBACKENCODING_INVALID.
 */
Y_CALLBACKENCODING_enum YNetwork::get_callbackEncoding(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_CALLBACKENCODING_INVALID;
    }
    return _callbackEncoding;
}

/**
 * Changes the encoding standard to use for representing notification values.
 * 
 * @param newval : a value among Y_CALLBACKENCODING_FORM, Y_CALLBACKENCODING_JSON,
 * Y_CALLBACKENCODING_JSON_ARRAY, Y_CALLBACKENCODING_CSV and Y_CALLBACKENCODING_YOCTO_API
 * corresponding to the encoding standard to use for representing notification values
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::set_callbackEncoding(Y_CALLBACKENCODING_enum newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
    return _setAttr("callbackEncoding", rest_val);
}

/**
 * Returns a hashed version of the notification callback credentials if set,
 * or an empty string otherwise.
 * 
 * @return a string corresponding to a hashed version of the notification callback credentials if set,
 *         or an empty string otherwise
 * 
 * On failure, throws an exception or returns Y_CALLBACKCREDENTIALS_INVALID.
 */
string YNetwork::get_callbackCredentials(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_CALLBACKCREDENTIALS_INVALID;
    }
    return _callbackCredentials;
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
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::set_callbackCredentials(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("callbackCredentials", rest_val);
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
 * @return YAPI_SUCCESS if the call succeeds.
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
 * Returns the minimum waiting time between two callback notifications, in seconds.
 * 
 * @return an integer corresponding to the minimum waiting time between two callback notifications, in seconds
 * 
 * On failure, throws an exception or returns Y_CALLBACKMINDELAY_INVALID.
 */
unsigned YNetwork::get_callbackMinDelay(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_CALLBACKMINDELAY_INVALID;
    }
    return _callbackMinDelay;
}

/**
 * Changes the minimum waiting time between two callback notifications, in seconds.
 * 
 * @param newval : an integer corresponding to the minimum waiting time between two callback
 * notifications, in seconds
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::set_callbackMinDelay(unsigned newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
    return _setAttr("callbackMinDelay", rest_val);
}

/**
 * Returns the maximum waiting time between two callback notifications, in seconds.
 * 
 * @return an integer corresponding to the maximum waiting time between two callback notifications, in seconds
 * 
 * On failure, throws an exception or returns Y_CALLBACKMAXDELAY_INVALID.
 */
unsigned YNetwork::get_callbackMaxDelay(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_CALLBACKMAXDELAY_INVALID;
    }
    return _callbackMaxDelay;
}

/**
 * Changes the maximum waiting time between two callback notifications, in seconds.
 * 
 * @param newval : an integer corresponding to the maximum waiting time between two callback
 * notifications, in seconds
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YNetwork::set_callbackMaxDelay(unsigned newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
    return _setAttr("callbackMaxDelay", rest_val);
}

/**
 * Returns the current consumed by the module from Power-over-Ethernet (PoE), in milli-amps.
 * The current consumption is measured after converting PoE source to 5 Volt, and should
 * never exceed 1800 mA.
 * 
 * @return an integer corresponding to the current consumed by the module from Power-over-Ethernet
 * (PoE), in milli-amps
 * 
 * On failure, throws an exception or returns Y_POECURRENT_INVALID.
 */
unsigned YNetwork::get_poeCurrent(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_POECURRENT_INVALID;
    }
    return _poeCurrent;
}

/**
 * Pings str_host to test the network connectivity. Sends four requests ICMP ECHO_REQUEST from the
 * module to the target str_host. This method returns a string with the result of the
 * 4 ICMP ECHO_REQUEST result.
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

YNetwork *YNetwork::nextNetwork(void)
{
    string  hwid;
    
    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return yFindNetwork(hwid);
}

void YNetwork::registerValueCallback(YNetworkUpdateCallback callback)
{
    if (callback != NULL) {
        _registerFuncCallback(this);
        yapiLockFunctionCallBack(NULL);
        YAPI::_yapiFunctionUpdateCallbackFwd(this->functionDescriptor(), this->get_advertisedValue().c_str());
        yapiUnlockFunctionCallBack(NULL);
    } else {
        _unregisterFuncCallback(this);
    }
    _callback = callback;
}

void YNetwork::advertiseValue(const string& value)
{
    if (_callback != NULL) {
        _callback(this, value);
    }
}


YNetwork* YNetwork::FindNetwork(const string& func)
{
    if(YAPI::_YFunctionsCaches["YNetwork"].find(func) != YAPI::_YFunctionsCaches["YNetwork"].end())
        return (YNetwork*) YAPI::_YFunctionsCaches["YNetwork"][func];
    
    YNetwork *newNetwork = new YNetwork(func);
    YAPI::_YFunctionsCaches["YNetwork"][func] = newNetwork ;
    return newNetwork;
}

YNetwork* YNetwork::FirstNetwork(void)
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

//--- (Network functions)
//--- (end of Network functions)
