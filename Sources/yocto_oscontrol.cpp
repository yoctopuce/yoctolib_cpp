/*********************************************************************
 *
 * $Id: yocto_oscontrol.cpp 12337 2013-08-14 15:22:22Z mvuilleu $
 *
 * Implements yFindOsControl(), the high-level API for OsControl functions
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
#include "yocto_oscontrol.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

//--- (YOsControl constructor)
// Constructor is protected, use yFindOsControl factory function to instantiate
YOsControl::YOsControl(const string& func): YFunction("OsControl", func)
//--- (end of YOsControl constructor)
//--- (OsControl initialization)
            ,_callback(NULL)
            ,_logicalName(Y_LOGICALNAME_INVALID)
            ,_advertisedValue(Y_ADVERTISEDVALUE_INVALID)
            ,_shutdownCountdown(Y_SHUTDOWNCOUNTDOWN_INVALID)
//--- (end of OsControl initialization)
{}

YOsControl::~YOsControl() 
{
//--- (YOsControl cleanup)
//--- (end of YOsControl cleanup)
}
//--- (YOsControl implementation)

const string YOsControl::LOGICALNAME_INVALID = "!INVALID!";
const string YOsControl::ADVERTISEDVALUE_INVALID = "!INVALID!";



int YOsControl::_parse(yJsonStateMachine& j)
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
        } else if(!strcmp(j.token, "shutdownCountdown")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _shutdownCountdown =  atoi(j.token);
        } else {
            // ignore unknown field
            yJsonSkip(&j, 1);
        }
    }
    if(j.st != YJSON_PARSE_STRUCT) goto failed;
    return 0;
}

/**
 * Returns the logical name of the OS control, corresponding to the network name of the module.
 * 
 * @return a string corresponding to the logical name of the OS control, corresponding to the network
 * name of the module
 * 
 * On failure, throws an exception or returns Y_LOGICALNAME_INVALID.
 */
string YOsControl::get_logicalName(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_LOGICALNAME_INVALID;
    }
    return _logicalName;
}

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
int YOsControl::set_logicalName(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("logicalName", rest_val);
}

/**
 * Returns the current value of the OS control (no more than 6 characters).
 * 
 * @return a string corresponding to the current value of the OS control (no more than 6 characters)
 * 
 * On failure, throws an exception or returns Y_ADVERTISEDVALUE_INVALID.
 */
string YOsControl::get_advertisedValue(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_ADVERTISEDVALUE_INVALID;
    }
    return _advertisedValue;
}

/**
 * Returns the remaining number of seconds before the OS shutdown, or zero when no
 * shutdown has been scheduled.
 * 
 * @return an integer corresponding to the remaining number of seconds before the OS shutdown, or zero when no
 *         shutdown has been scheduled
 * 
 * On failure, throws an exception or returns Y_SHUTDOWNCOUNTDOWN_INVALID.
 */
unsigned YOsControl::get_shutdownCountdown(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_SHUTDOWNCOUNTDOWN_INVALID;
    }
    return _shutdownCountdown;
}

int YOsControl::set_shutdownCountdown(unsigned newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
    return _setAttr("shutdownCountdown", rest_val);
}

/**
 * Schedules an OS shutdown after a given number of seconds.
 * 
 * @param secBeforeShutDown : number of seconds before shutdown
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YOsControl::shutdown(int secBeforeShutDown)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", secBeforeShutDown); rest_val = string(buf);
    return _setAttr("shutdownCountdown", rest_val);
}

YOsControl *YOsControl::nextOsControl(void)
{
    string  hwid;
    
    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return yFindOsControl(hwid);
}

void YOsControl::registerValueCallback(YOsControlUpdateCallback callback)
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

void YOsControl::advertiseValue(const string& value)
{
    if (_callback != NULL) {
        _callback(this, value);
    }
}


YOsControl* YOsControl::FindOsControl(const string& func)
{
    if(YAPI::_YFunctionsCaches["YOsControl"].find(func) != YAPI::_YFunctionsCaches["YOsControl"].end())
        return (YOsControl*) YAPI::_YFunctionsCaches["YOsControl"][func];
    
    YOsControl *newOsControl = new YOsControl(func);
    YAPI::_YFunctionsCaches["YOsControl"][func] = newOsControl ;
    return newOsControl;
}

YOsControl* YOsControl::FirstOsControl(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;
    
    if(YISERR(YapiWrapper::getFunctionsByClass("OsControl", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YOsControl::FindOsControl(serial+"."+funcId);
}

//--- (end of YOsControl implementation)

//--- (OsControl functions)
//--- (end of OsControl functions)
