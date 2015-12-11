/*********************************************************************
 *
 * $Id: yocto_bluetoothlink.cpp 22191 2015-12-02 06:49:31Z mvuilleu $
 *
 * Implements yFindBluetoothLink(), the high-level API for BluetoothLink functions
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
#include "yocto_bluetoothlink.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

YBluetoothLink::YBluetoothLink(const string& func): YFunction(func)
//--- (BluetoothLink initialization)
    ,_ownAddress(OWNADDRESS_INVALID)
    ,_pairingPin(PAIRINGPIN_INVALID)
    ,_remoteAddress(REMOTEADDRESS_INVALID)
    ,_remoteName(REMOTENAME_INVALID)
    ,_mute(MUTE_INVALID)
    ,_preAmplifier(PREAMPLIFIER_INVALID)
    ,_volume(VOLUME_INVALID)
    ,_linkState(LINKSTATE_INVALID)
    ,_linkQuality(LINKQUALITY_INVALID)
    ,_command(COMMAND_INVALID)
    ,_valueCallbackBluetoothLink(NULL)
//--- (end of BluetoothLink initialization)
{
    _className="BluetoothLink";
}

YBluetoothLink::~YBluetoothLink()
{
//--- (YBluetoothLink cleanup)
//--- (end of YBluetoothLink cleanup)
}
//--- (YBluetoothLink implementation)
// static attributes
const string YBluetoothLink::OWNADDRESS_INVALID = YAPI_INVALID_STRING;
const string YBluetoothLink::PAIRINGPIN_INVALID = YAPI_INVALID_STRING;
const string YBluetoothLink::REMOTEADDRESS_INVALID = YAPI_INVALID_STRING;
const string YBluetoothLink::REMOTENAME_INVALID = YAPI_INVALID_STRING;
const string YBluetoothLink::COMMAND_INVALID = YAPI_INVALID_STRING;

int YBluetoothLink::_parseAttr(yJsonStateMachine& j)
{
    if(!strcmp(j.token, "ownAddress")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _ownAddress =  _parseString(j);
        return 1;
    }
    if(!strcmp(j.token, "pairingPin")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _pairingPin =  _parseString(j);
        return 1;
    }
    if(!strcmp(j.token, "remoteAddress")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _remoteAddress =  _parseString(j);
        return 1;
    }
    if(!strcmp(j.token, "remoteName")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _remoteName =  _parseString(j);
        return 1;
    }
    if(!strcmp(j.token, "mute")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _mute =  (Y_MUTE_enum)atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "preAmplifier")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _preAmplifier =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "volume")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _volume =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "linkState")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _linkState =  (Y_LINKSTATE_enum)atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "linkQuality")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _linkQuality =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "command")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _command =  _parseString(j);
        return 1;
    }
    failed:
    return YFunction::_parseAttr(j);
}


/**
 * Returns the MAC-48 address of the bluetooth interface, which is unique on the bluetooth network.
 *
 * @return a string corresponding to the MAC-48 address of the bluetooth interface, which is unique on
 * the bluetooth network
 *
 * On failure, throws an exception or returns Y_OWNADDRESS_INVALID.
 */
string YBluetoothLink::get_ownAddress(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YBluetoothLink::OWNADDRESS_INVALID;
        }
    }
    return _ownAddress;
}

/**
 * Returns an opaque string if a PIN code has been configured in the device to access
 * the SIM card, or an empty string if none has been configured or if the code provided
 * was rejected by the SIM card.
 *
 * @return a string corresponding to an opaque string if a PIN code has been configured in the device to access
 *         the SIM card, or an empty string if none has been configured or if the code provided
 *         was rejected by the SIM card
 *
 * On failure, throws an exception or returns Y_PAIRINGPIN_INVALID.
 */
string YBluetoothLink::get_pairingPin(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YBluetoothLink::PAIRINGPIN_INVALID;
        }
    }
    return _pairingPin;
}

/**
 * Changes the PIN code used by the module for bluetooth pairing.
 * Remember to call the saveToFlash() method of the module to save the
 * new value in the device flash.
 *
 * @param newval : a string corresponding to the PIN code used by the module for bluetooth pairing
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YBluetoothLink::set_pairingPin(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("pairingPin", rest_val);
}

/**
 * Returns the MAC-48 address of the remote device to connect to.
 *
 * @return a string corresponding to the MAC-48 address of the remote device to connect to
 *
 * On failure, throws an exception or returns Y_REMOTEADDRESS_INVALID.
 */
string YBluetoothLink::get_remoteAddress(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YBluetoothLink::REMOTEADDRESS_INVALID;
        }
    }
    return _remoteAddress;
}

/**
 * Changes the MAC-48 address defining which remote device to connect to.
 *
 * @param newval : a string corresponding to the MAC-48 address defining which remote device to connect to
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YBluetoothLink::set_remoteAddress(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("remoteAddress", rest_val);
}

/**
 * Returns the bluetooth name the remote device, if found on the bluetooth network.
 *
 * @return a string corresponding to the bluetooth name the remote device, if found on the bluetooth network
 *
 * On failure, throws an exception or returns Y_REMOTENAME_INVALID.
 */
string YBluetoothLink::get_remoteName(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YBluetoothLink::REMOTENAME_INVALID;
        }
    }
    return _remoteName;
}

/**
 * Returns the state of the mute function.
 *
 * @return either Y_MUTE_FALSE or Y_MUTE_TRUE, according to the state of the mute function
 *
 * On failure, throws an exception or returns Y_MUTE_INVALID.
 */
Y_MUTE_enum YBluetoothLink::get_mute(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YBluetoothLink::MUTE_INVALID;
        }
    }
    return _mute;
}

/**
 * Changes the state of the mute function. Remember to call the matching module
 * saveToFlash() method to save the setting permanently.
 *
 * @param newval : either Y_MUTE_FALSE or Y_MUTE_TRUE, according to the state of the mute function
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YBluetoothLink::set_mute(Y_MUTE_enum newval)
{
    string rest_val;
    rest_val = (newval>0 ? "1" : "0");
    return _setAttr("mute", rest_val);
}

/**
 * Returns the audio pre-amplifier volume, in per cents.
 *
 * @return an integer corresponding to the audio pre-amplifier volume, in per cents
 *
 * On failure, throws an exception or returns Y_PREAMPLIFIER_INVALID.
 */
int YBluetoothLink::get_preAmplifier(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YBluetoothLink::PREAMPLIFIER_INVALID;
        }
    }
    return _preAmplifier;
}

/**
 * Changes the audio pre-amplifier volume, in per cents.
 *
 * @param newval : an integer corresponding to the audio pre-amplifier volume, in per cents
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YBluetoothLink::set_preAmplifier(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("preAmplifier", rest_val);
}

/**
 * Returns the connected headset volume, in per cents.
 *
 * @return an integer corresponding to the connected headset volume, in per cents
 *
 * On failure, throws an exception or returns Y_VOLUME_INVALID.
 */
int YBluetoothLink::get_volume(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YBluetoothLink::VOLUME_INVALID;
        }
    }
    return _volume;
}

/**
 * Changes the connected headset volume, in per cents.
 *
 * @param newval : an integer corresponding to the connected headset volume, in per cents
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YBluetoothLink::set_volume(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("volume", rest_val);
}

/**
 * Returns the bluetooth link state.
 *
 * @return a value among Y_LINKSTATE_DOWN, Y_LINKSTATE_FREE, Y_LINKSTATE_SEARCH, Y_LINKSTATE_EXISTS,
 * Y_LINKSTATE_LINKED and Y_LINKSTATE_PLAY corresponding to the bluetooth link state
 *
 * On failure, throws an exception or returns Y_LINKSTATE_INVALID.
 */
Y_LINKSTATE_enum YBluetoothLink::get_linkState(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YBluetoothLink::LINKSTATE_INVALID;
        }
    }
    return _linkState;
}

/**
 * Returns the bluetooth receiver signal strength, in pourcents, or 0 if no connection is established.
 *
 * @return an integer corresponding to the bluetooth receiver signal strength, in pourcents, or 0 if
 * no connection is established
 *
 * On failure, throws an exception or returns Y_LINKQUALITY_INVALID.
 */
int YBluetoothLink::get_linkQuality(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YBluetoothLink::LINKQUALITY_INVALID;
        }
    }
    return _linkQuality;
}

string YBluetoothLink::get_command(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YBluetoothLink::COMMAND_INVALID;
        }
    }
    return _command;
}

int YBluetoothLink::set_command(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("command", rest_val);
}

/**
 * Retrieves $AFUNCTION$ for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that $THEFUNCTION$ is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method YBluetoothLink.isOnline() to test if $THEFUNCTION$ is
 * indeed online at a given time. In case of ambiguity when looking for
 * $AFUNCTION$ by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * @param func : a string that uniquely characterizes $THEFUNCTION$
 *
 * @return a YBluetoothLink object allowing you to drive $THEFUNCTION$.
 */
YBluetoothLink* YBluetoothLink::FindBluetoothLink(string func)
{
    YBluetoothLink* obj = NULL;
    obj = (YBluetoothLink*) YFunction::_FindFromCache("BluetoothLink", func);
    if (obj == NULL) {
        obj = new YBluetoothLink(func);
        YFunction::_AddToCache("BluetoothLink", func, obj);
    }
    return obj;
}

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
int YBluetoothLink::registerValueCallback(YBluetoothLinkValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackBluetoothLink = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YBluetoothLink::_invokeValueCallback(string value)
{
    if (_valueCallbackBluetoothLink != NULL) {
        _valueCallbackBluetoothLink(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

/**
 * Attempt to connect to the previously selected remote device.
 *
 * @return YAPI_SUCCESS when the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YBluetoothLink::connect(void)
{
    return this->set_command("C");
}

/**
 * Disconnect from the previously selected remote device.
 *
 * @return YAPI_SUCCESS when the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YBluetoothLink::disconnect(void)
{
    return this->set_command("D");
}

YBluetoothLink *YBluetoothLink::nextBluetoothLink(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YBluetoothLink::FindBluetoothLink(hwid);
}

YBluetoothLink* YBluetoothLink::FirstBluetoothLink(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("BluetoothLink", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YBluetoothLink::FindBluetoothLink(serial+"."+funcId);
}

//--- (end of YBluetoothLink implementation)

//--- (BluetoothLink functions)
//--- (end of BluetoothLink functions)
