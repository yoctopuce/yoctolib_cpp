/*********************************************************************
 *
 * $Id: yocto_hubport.cpp 12337 2013-08-14 15:22:22Z mvuilleu $
 *
 * Implements yFindHubPort(), the high-level API for HubPort functions
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
#include "yocto_hubport.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

//--- (YHubPort constructor)
// Constructor is protected, use yFindHubPort factory function to instantiate
YHubPort::YHubPort(const string& func): YFunction("HubPort", func)
//--- (end of YHubPort constructor)
//--- (HubPort initialization)
            ,_callback(NULL)
            ,_logicalName(Y_LOGICALNAME_INVALID)
            ,_advertisedValue(Y_ADVERTISEDVALUE_INVALID)
            ,_enabled(Y_ENABLED_INVALID)
            ,_portState(Y_PORTSTATE_INVALID)
            ,_baudRate(Y_BAUDRATE_INVALID)
//--- (end of HubPort initialization)
{}

YHubPort::~YHubPort() 
{
//--- (YHubPort cleanup)
//--- (end of YHubPort cleanup)
}
//--- (YHubPort implementation)

const string YHubPort::LOGICALNAME_INVALID = "!INVALID!";
const string YHubPort::ADVERTISEDVALUE_INVALID = "!INVALID!";



int YHubPort::_parse(yJsonStateMachine& j)
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
        } else if(!strcmp(j.token, "enabled")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _enabled =  (Y_ENABLED_enum)atoi(j.token);
        } else if(!strcmp(j.token, "portState")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _portState =  (Y_PORTSTATE_enum)atoi(j.token);
        } else if(!strcmp(j.token, "baudRate")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _baudRate =  atoi(j.token);
        } else {
            // ignore unknown field
            yJsonSkip(&j, 1);
        }
    }
    if(j.st != YJSON_PARSE_STRUCT) goto failed;
    return 0;
}

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
string YHubPort::get_logicalName(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_LOGICALNAME_INVALID;
    }
    return _logicalName;
}

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
int YHubPort::set_logicalName(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("logicalName", rest_val);
}

/**
 * Returns the current value of the Yocto-hub port (no more than 6 characters).
 * 
 * @return a string corresponding to the current value of the Yocto-hub port (no more than 6 characters)
 * 
 * On failure, throws an exception or returns Y_ADVERTISEDVALUE_INVALID.
 */
string YHubPort::get_advertisedValue(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_ADVERTISEDVALUE_INVALID;
    }
    return _advertisedValue;
}

/**
 * Returns true if the Yocto-hub port is powered, false otherwise.
 * 
 * @return either Y_ENABLED_FALSE or Y_ENABLED_TRUE, according to true if the Yocto-hub port is
 * powered, false otherwise
 * 
 * On failure, throws an exception or returns Y_ENABLED_INVALID.
 */
Y_ENABLED_enum YHubPort::get_enabled(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_ENABLED_INVALID;
    }
    return _enabled;
}

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
int YHubPort::set_enabled(Y_ENABLED_enum newval)
{
    string rest_val;
    rest_val = (newval>0 ? "1" : "0");
    return _setAttr("enabled", rest_val);
}

/**
 * Returns the current state of the Yocto-hub port.
 * 
 * @return a value among Y_PORTSTATE_OFF, Y_PORTSTATE_OVRLD, Y_PORTSTATE_ON, Y_PORTSTATE_RUN and
 * Y_PORTSTATE_PROG corresponding to the current state of the Yocto-hub port
 * 
 * On failure, throws an exception or returns Y_PORTSTATE_INVALID.
 */
Y_PORTSTATE_enum YHubPort::get_portState(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_PORTSTATE_INVALID;
    }
    return _portState;
}

/**
 * Returns the current baud rate used by this Yocto-hub port, in kbps.
 * The default value is 1000 kbps, but a slower rate may be used if communication
 * problems are encountered.
 * 
 * @return an integer corresponding to the current baud rate used by this Yocto-hub port, in kbps
 * 
 * On failure, throws an exception or returns Y_BAUDRATE_INVALID.
 */
int YHubPort::get_baudRate(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_BAUDRATE_INVALID;
    }
    return _baudRate;
}

YHubPort *YHubPort::nextHubPort(void)
{
    string  hwid;
    
    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return yFindHubPort(hwid);
}

void YHubPort::registerValueCallback(YHubPortUpdateCallback callback)
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

void YHubPort::advertiseValue(const string& value)
{
    if (_callback != NULL) {
        _callback(this, value);
    }
}


YHubPort* YHubPort::FindHubPort(const string& func)
{
    if(YAPI::_YFunctionsCaches["YHubPort"].find(func) != YAPI::_YFunctionsCaches["YHubPort"].end())
        return (YHubPort*) YAPI::_YFunctionsCaches["YHubPort"][func];
    
    YHubPort *newHubPort = new YHubPort(func);
    YAPI::_YFunctionsCaches["YHubPort"][func] = newHubPort ;
    return newHubPort;
}

YHubPort* YHubPort::FirstHubPort(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;
    
    if(YISERR(YapiWrapper::getFunctionsByClass("HubPort", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YHubPort::FindHubPort(serial+"."+funcId);
}

//--- (end of YHubPort implementation)

//--- (HubPort functions)
//--- (end of HubPort functions)
