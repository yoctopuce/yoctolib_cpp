/*********************************************************************
 *
 * $Id: yocto_wireless.cpp 9425 2013-01-11 15:50:01Z seb $
 *
 * Implements yFindWireless(), the high-level API for Wireless functions
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


#define _CRT_SECURE_NO_DEPRECATE //do not use windows secure crt
#include "yocto_wireless.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//--- (YWireless implementation)

const string YWireless::LOGICALNAME_INVALID = "!INVALID!";
const string YWireless::ADVERTISEDVALUE_INVALID = "!INVALID!";
const string YWireless::SSID_INVALID = "!INVALID!";
const string YWireless::MESSAGE_INVALID = "!INVALID!";
const string YWireless::WLANCONFIG_INVALID = "!INVALID!";

std::map<string,YWireless*> YWireless::_WirelessCache;

int YWireless::_parse(yJsonStateMachine& j)
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
        } else if(!strcmp(j.token, "linkQuality")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _linkQuality =  atoi(j.token);
        } else if(!strcmp(j.token, "ssid")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _ssid =  _parseString(j);
        } else if(!strcmp(j.token, "channel")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _channel =  atoi(j.token);
        } else if(!strcmp(j.token, "security")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _security =  (Y_SECURITY_enum)atoi(j.token);
        } else if(!strcmp(j.token, "message")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _message =  _parseString(j);
        } else if(!strcmp(j.token, "wlanConfig")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _wlanConfig =  _parseString(j);
        } else {
            // ignore unknown field
            yJsonSkip(&j, 1);
        }
    }
    if(j.st != YJSON_PARSE_STRUCT) goto failed;
    return 0;
}

/**
 * Returns the logical name of the wireless lan interface.
 * 
 * @return a string corresponding to the logical name of the wireless lan interface
 * 
 * On failure, throws an exception or returns Y_LOGICALNAME_INVALID.
 */
string YWireless::get_logicalName(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_LOGICALNAME_INVALID;
    }
    return _logicalName;
}

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
int YWireless::set_logicalName(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("logicalName", rest_val);
}

/**
 * Returns the current value of the wireless lan interface (no more than 6 characters).
 * 
 * @return a string corresponding to the current value of the wireless lan interface (no more than 6 characters)
 * 
 * On failure, throws an exception or returns Y_ADVERTISEDVALUE_INVALID.
 */
string YWireless::get_advertisedValue(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_ADVERTISEDVALUE_INVALID;
    }
    return _advertisedValue;
}

/**
 * Returns the link quality, expressed in per cents.
 * 
 * @return an integer corresponding to the link quality, expressed in per cents
 * 
 * On failure, throws an exception or returns Y_LINKQUALITY_INVALID.
 */
int YWireless::get_linkQuality(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_LINKQUALITY_INVALID;
    }
    return _linkQuality;
}

/**
 * Returns the wireless network name (SSID).
 * 
 * @return a string corresponding to the wireless network name (SSID)
 * 
 * On failure, throws an exception or returns Y_SSID_INVALID.
 */
string YWireless::get_ssid(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_SSID_INVALID;
    }
    return _ssid;
}

/**
 * Returns the 802.11 channel currently used, or 0 when the selected network has not been found.
 * 
 * @return an integer corresponding to the 802
 * 
 * On failure, throws an exception or returns Y_CHANNEL_INVALID.
 */
unsigned YWireless::get_channel(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_CHANNEL_INVALID;
    }
    return _channel;
}

/**
 * Returns the security algorithm used by the selected wireless network.
 * 
 * @return a value among Y_SECURITY_UNKNOWN, Y_SECURITY_OPEN, Y_SECURITY_WEP, Y_SECURITY_WPA and
 * Y_SECURITY_WPA2 corresponding to the security algorithm used by the selected wireless network
 * 
 * On failure, throws an exception or returns Y_SECURITY_INVALID.
 */
Y_SECURITY_enum YWireless::get_security(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_SECURITY_INVALID;
    }
    return _security;
}

/**
 * Returns the last status message from the wireless interface.
 * 
 * @return a string corresponding to the last status message from the wireless interface
 * 
 * On failure, throws an exception or returns Y_MESSAGE_INVALID.
 */
string YWireless::get_message(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_MESSAGE_INVALID;
    }
    return _message;
}

string YWireless::get_wlanConfig(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_WLANCONFIG_INVALID;
    }
    return _wlanConfig;
}

int YWireless::set_wlanConfig(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("wlanConfig", rest_val);
}

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
int YWireless::joinNetwork(string ssid,string securityKey)
{
    string rest_val;
    rest_val = string("INFRA:")+string(ssid)+string("\\")+string(securityKey);
    return _setAttr("wlanConfig", rest_val);
}

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
int YWireless::adhocNetwork(string ssid,string securityKey)
{
    string rest_val;
    rest_val = string("ADHOC:")+string(ssid)+string("\\")+string(securityKey);
    return _setAttr("wlanConfig", rest_val);
}

YWireless *YWireless::nextWireless(void)
{
    string  hwid;
    
    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return yFindWireless(hwid);
}

void YWireless::registerValueCallback(YWirelessUpdateCallback callback)
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

void YWireless::advertiseValue(const string& value)
{
    if (_callback != NULL) {
        _callback(this, value);
    }
}


YWireless* YWireless::FindWireless(const string& func)
{
    if(YWireless::_WirelessCache.find(func) != YWireless::_WirelessCache.end())
        return YWireless::_WirelessCache[func];
    
    YWireless *newWireless = new YWireless(func);
    YWireless::_WirelessCache[func] = newWireless;
    
    return newWireless;
}

YWireless* YWireless::FirstWireless(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;
    
    if(YISERR(YapiWrapper::getFunctionsByClass("Wireless", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YWireless::FindWireless(serial+"."+funcId);
}

//--- (end of YWireless implementation)

//--- (Wireless functions)
//--- (end of Wireless functions)
