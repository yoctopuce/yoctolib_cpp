/*********************************************************************
 *
 *  $Id: yocto_bluetoothlink.cpp 52570 2022-12-26 09:27:54Z seb $
 *
 *  Implements yFindBluetoothLink(), the high-level API for BluetoothLink functions
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

#include "yocto_bluetoothlink.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#define  __FILE_ID__  "bluetoothlink"

#ifdef YOCTOLIB_NAMESPACE
using namespace YOCTOLIB_NAMESPACE;
#endif

YBluetoothLink::YBluetoothLink(const string& func): YFunction(func)
//--- (YBluetoothLink initialization)
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
//--- (end of YBluetoothLink initialization)
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

int YBluetoothLink::_parseAttr(YJSONObject *json_val)
{
    if(json_val->has("ownAddress")) {
        _ownAddress =  json_val->getString("ownAddress");
    }
    if(json_val->has("pairingPin")) {
        _pairingPin =  json_val->getString("pairingPin");
    }
    if(json_val->has("remoteAddress")) {
        _remoteAddress =  json_val->getString("remoteAddress");
    }
    if(json_val->has("remoteName")) {
        _remoteName =  json_val->getString("remoteName");
    }
    if(json_val->has("mute")) {
        _mute =  (Y_MUTE_enum)json_val->getInt("mute");
    }
    if(json_val->has("preAmplifier")) {
        _preAmplifier =  json_val->getInt("preAmplifier");
    }
    if(json_val->has("volume")) {
        _volume =  json_val->getInt("volume");
    }
    if(json_val->has("linkState")) {
        _linkState =  (Y_LINKSTATE_enum)json_val->getInt("linkState");
    }
    if(json_val->has("linkQuality")) {
        _linkQuality =  json_val->getInt("linkQuality");
    }
    if(json_val->has("command")) {
        _command =  json_val->getString("command");
    }
    return YFunction::_parseAttr(json_val);
}


/**
 * Returns the MAC-48 address of the bluetooth interface, which is unique on the bluetooth network.
 *
 * @return a string corresponding to the MAC-48 address of the bluetooth interface, which is unique on
 * the bluetooth network
 *
 * On failure, throws an exception or returns YBluetoothLink::OWNADDRESS_INVALID.
 */
string YBluetoothLink::get_ownAddress(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YBluetoothLink::OWNADDRESS_INVALID;
                }
            }
        }
        res = _ownAddress;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
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
 * On failure, throws an exception or returns YBluetoothLink::PAIRINGPIN_INVALID.
 */
string YBluetoothLink::get_pairingPin(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YBluetoothLink::PAIRINGPIN_INVALID;
                }
            }
        }
        res = _pairingPin;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the PIN code used by the module for bluetooth pairing.
 * Remember to call the saveToFlash() method of the module to save the
 * new value in the device flash.
 *
 * @param newval : a string corresponding to the PIN code used by the module for bluetooth pairing
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YBluetoothLink::set_pairingPin(const string& newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = newval;
        res = _setAttr("pairingPin", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the MAC-48 address of the remote device to connect to.
 *
 * @return a string corresponding to the MAC-48 address of the remote device to connect to
 *
 * On failure, throws an exception or returns YBluetoothLink::REMOTEADDRESS_INVALID.
 */
string YBluetoothLink::get_remoteAddress(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YBluetoothLink::REMOTEADDRESS_INVALID;
                }
            }
        }
        res = _remoteAddress;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the MAC-48 address defining which remote device to connect to.
 * Remember to call the saveToFlash()
 * method of the module if the modification must be kept.
 *
 * @param newval : a string corresponding to the MAC-48 address defining which remote device to connect to
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YBluetoothLink::set_remoteAddress(const string& newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = newval;
        res = _setAttr("remoteAddress", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the bluetooth name the remote device, if found on the bluetooth network.
 *
 * @return a string corresponding to the bluetooth name the remote device, if found on the bluetooth network
 *
 * On failure, throws an exception or returns YBluetoothLink::REMOTENAME_INVALID.
 */
string YBluetoothLink::get_remoteName(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YBluetoothLink::REMOTENAME_INVALID;
                }
            }
        }
        res = _remoteName;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the state of the mute function.
 *
 * @return either YBluetoothLink::MUTE_FALSE or YBluetoothLink::MUTE_TRUE, according to the state of the
 * mute function
 *
 * On failure, throws an exception or returns YBluetoothLink::MUTE_INVALID.
 */
Y_MUTE_enum YBluetoothLink::get_mute(void)
{
    Y_MUTE_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YBluetoothLink::MUTE_INVALID;
                }
            }
        }
        res = _mute;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the state of the mute function. Remember to call the matching module
 * saveToFlash() method to save the setting permanently.
 *
 * @param newval : either YBluetoothLink::MUTE_FALSE or YBluetoothLink::MUTE_TRUE, according to the
 * state of the mute function
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YBluetoothLink::set_mute(Y_MUTE_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = (newval>0 ? "1" : "0");
        res = _setAttr("mute", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the audio pre-amplifier volume, in per cents.
 *
 * @return an integer corresponding to the audio pre-amplifier volume, in per cents
 *
 * On failure, throws an exception or returns YBluetoothLink::PREAMPLIFIER_INVALID.
 */
int YBluetoothLink::get_preAmplifier(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YBluetoothLink::PREAMPLIFIER_INVALID;
                }
            }
        }
        res = _preAmplifier;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the audio pre-amplifier volume, in per cents.
 * Remember to call the saveToFlash()
 * method of the module if the modification must be kept.
 *
 * @param newval : an integer corresponding to the audio pre-amplifier volume, in per cents
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YBluetoothLink::set_preAmplifier(int newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("preAmplifier", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the connected headset volume, in per cents.
 *
 * @return an integer corresponding to the connected headset volume, in per cents
 *
 * On failure, throws an exception or returns YBluetoothLink::VOLUME_INVALID.
 */
int YBluetoothLink::get_volume(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YBluetoothLink::VOLUME_INVALID;
                }
            }
        }
        res = _volume;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the connected headset volume, in per cents.
 *
 * @param newval : an integer corresponding to the connected headset volume, in per cents
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YBluetoothLink::set_volume(int newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("volume", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the bluetooth link state.
 *
 * @return a value among YBluetoothLink::LINKSTATE_DOWN, YBluetoothLink::LINKSTATE_FREE,
 * YBluetoothLink::LINKSTATE_SEARCH, YBluetoothLink::LINKSTATE_EXISTS, YBluetoothLink::LINKSTATE_LINKED
 * and YBluetoothLink::LINKSTATE_PLAY corresponding to the bluetooth link state
 *
 * On failure, throws an exception or returns YBluetoothLink::LINKSTATE_INVALID.
 */
Y_LINKSTATE_enum YBluetoothLink::get_linkState(void)
{
    Y_LINKSTATE_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YBluetoothLink::LINKSTATE_INVALID;
                }
            }
        }
        res = _linkState;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the bluetooth receiver signal strength, in pourcents, or 0 if no connection is established.
 *
 * @return an integer corresponding to the bluetooth receiver signal strength, in pourcents, or 0 if
 * no connection is established
 *
 * On failure, throws an exception or returns YBluetoothLink::LINKQUALITY_INVALID.
 */
int YBluetoothLink::get_linkQuality(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YBluetoothLink::LINKQUALITY_INVALID;
                }
            }
        }
        res = _linkQuality;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

string YBluetoothLink::get_command(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YBluetoothLink::COMMAND_INVALID;
                }
            }
        }
        res = _command;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

int YBluetoothLink::set_command(const string& newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = newval;
        res = _setAttr("command", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Retrieves a Bluetooth sound controller for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that the Bluetooth sound controller is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the Bluetooth sound controller is
 * indeed online at a given time. In case of ambiguity when looking for
 * a Bluetooth sound controller by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the Bluetooth sound controller, for instance
 *         MyDevice.bluetoothLink1.
 *
 * @return a YBluetoothLink object allowing you to drive the Bluetooth sound controller.
 */
YBluetoothLink* YBluetoothLink::FindBluetoothLink(string func)
{
    YBluetoothLink* obj = NULL;
    int taken = 0;
    if (YAPI::_apiInitialized) {
        yEnterCriticalSection(&YAPI::_global_cs);
        taken = 1;
    }try {
        obj = (YBluetoothLink*) YFunction::_FindFromCache("BluetoothLink", func);
        if (obj == NULL) {
            obj = new YBluetoothLink(func);
            YFunction::_AddToCache("BluetoothLink", func, obj);
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
 * @return YAPI::SUCCESS when the call succeeds.
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
 * @return YAPI::SUCCESS when the call succeeds.
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

YBluetoothLink *YBluetoothLink::FirstBluetoothLink(void)
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

//--- (YBluetoothLink functions)
//--- (end of YBluetoothLink functions)
