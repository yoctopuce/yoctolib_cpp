/*********************************************************************
 *
 * $Id: yocto_vsource.cpp 10263 2013-03-11 17:25:38Z seb $
 *
 * Implements yFindVSource(), the high-level API for VSource functions
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
#include "yocto_vsource.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//--- (YVSource implementation)
YMove YVSOURCE_INVALID_MOVE;
YPulse YVSOURCE_INVALID_PULSE;

const string YVSource::LOGICALNAME_INVALID = "!INVALID!";
const string YVSource::ADVERTISEDVALUE_INVALID = "!INVALID!";
const string YVSource::UNIT_INVALID = "!INVALID!";

std::map<string,YVSource*> YVSource::_VSourceCache;

int YVSource::_parse(yJsonStateMachine& j)
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
        } else if(!strcmp(j.token, "unit")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _unit =  _parseString(j);
        } else if(!strcmp(j.token, "voltage")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _voltage =  atoi(j.token);
        } else if(!strcmp(j.token, "failure")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _failure =  (Y_FAILURE_enum)atoi(j.token);
        } else if(!strcmp(j.token, "overHeat")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _overHeat =  (Y_OVERHEAT_enum)atoi(j.token);
        } else if(!strcmp(j.token, "overCurrent")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _overCurrent =  (Y_OVERCURRENT_enum)atoi(j.token);
        } else if(!strcmp(j.token, "overLoad")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _overLoad =  (Y_OVERLOAD_enum)atoi(j.token);
        } else if(!strcmp(j.token, "regulationFailure")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _regulationFailure =  (Y_REGULATIONFAILURE_enum)atoi(j.token);
        } else if(!strcmp(j.token, "extPowerFailure")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _extPowerFailure =  (Y_EXTPOWERFAILURE_enum)atoi(j.token);
        } else if(!strcmp(j.token, "move")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            if(j.st != YJSON_PARSE_STRUCT) goto failed;
            while(yJsonParse(&j) == YJSON_PARSE_AVAIL && j.st == YJSON_PARSE_MEMBNAME) {
                if(!strcmp(j.token, "moving")) {
                    if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
                    _move.moving = atoi(j.token);
                } else if(!strcmp(j.token, "target")) {
                    if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
                    _move.target = atoi(j.token);
                } else if(!strcmp(j.token, "ms")) {
                    if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
                    _move.ms = atoi(j.token);
                }
            }
            if(j.st != YJSON_PARSE_STRUCT) goto failed; 
            
        } else if(!strcmp(j.token, "pulseTimer")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            if(j.st != YJSON_PARSE_STRUCT) goto failed;
            while(yJsonParse(&j) == YJSON_PARSE_AVAIL && j.st == YJSON_PARSE_MEMBNAME) {
                if(!strcmp(j.token, "moving")) {
                    if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
                    _pulseTimer.moving = atoi(j.token);
                } else if(!strcmp(j.token, "target")) {
                    if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
                    _pulseTimer.target = atoi(j.token);
                } else if(!strcmp(j.token, "ms")) {
                    if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
                    _pulseTimer.ms = atoi(j.token);
                }
            }
            if(j.st != YJSON_PARSE_STRUCT) goto failed; 
            
        } else {
            // ignore unknown field
            yJsonSkip(&j, 1);
        }
    }
    if(j.st != YJSON_PARSE_STRUCT) goto failed;
    return 0;
}

/**
 * Returns the logical name of the voltage source.
 * 
 * @return a string corresponding to the logical name of the voltage source
 * 
 * On failure, throws an exception or returns Y_LOGICALNAME_INVALID.
 */
string YVSource::get_logicalName(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_LOGICALNAME_INVALID;
    }
    return _logicalName;
}

/**
 * Changes the logical name of the voltage source. You can use yCheckLogicalName()
 * prior to this call to make sure that your parameter is valid.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 * 
 * @param newval : a string corresponding to the logical name of the voltage source
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YVSource::set_logicalName(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("logicalName", rest_val);
}

/**
 * Returns the current value of the voltage source (no more than 6 characters).
 * 
 * @return a string corresponding to the current value of the voltage source (no more than 6 characters)
 * 
 * On failure, throws an exception or returns Y_ADVERTISEDVALUE_INVALID.
 */
string YVSource::get_advertisedValue(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_ADVERTISEDVALUE_INVALID;
    }
    return _advertisedValue;
}

/**
 * Returns the measuring unit for the voltage.
 * 
 * @return a string corresponding to the measuring unit for the voltage
 * 
 * On failure, throws an exception or returns Y_UNIT_INVALID.
 */
string YVSource::get_unit(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_UNIT_INVALID;
    }
    return _unit;
}

/**
 * Returns the voltage output command (mV)
 * 
 * @return an integer corresponding to the voltage output command (mV)
 * 
 * On failure, throws an exception or returns Y_VOLTAGE_INVALID.
 */
int YVSource::get_voltage(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_VOLTAGE_INVALID;
    }
    return _voltage;
}

/**
 * Tunes the device output voltage (milliVolts).
 * 
 * @param newval : an integer
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YVSource::set_voltage(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("voltage", rest_val);
}

/**
 * Returns true if the  module is in failure mode. More information can be obtained by testing
 * get_overheat, get_overcurrent etc... When a error condition is met, the output voltage is
 * set to zéro and cannot be changed until the reset() function is called.
 * 
 * @return either Y_FAILURE_FALSE or Y_FAILURE_TRUE, according to true if the  module is in failure mode
 * 
 * On failure, throws an exception or returns Y_FAILURE_INVALID.
 */
Y_FAILURE_enum YVSource::get_failure(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_FAILURE_INVALID;
    }
    return _failure;
}

int YVSource::set_failure(Y_FAILURE_enum newval)
{
    string rest_val;
    rest_val = (newval>0 ? "1" : "0");
    return _setAttr("failure", rest_val);
}

/**
 * Returns TRUE if the  module is overheating.
 * 
 * @return either Y_OVERHEAT_FALSE or Y_OVERHEAT_TRUE, according to TRUE if the  module is overheating
 * 
 * On failure, throws an exception or returns Y_OVERHEAT_INVALID.
 */
Y_OVERHEAT_enum YVSource::get_overHeat(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_OVERHEAT_INVALID;
    }
    return _overHeat;
}

/**
 * Returns true if the appliance connected to the device is too greedy .
 * 
 * @return either Y_OVERCURRENT_FALSE or Y_OVERCURRENT_TRUE, according to true if the appliance
 * connected to the device is too greedy
 * 
 * On failure, throws an exception or returns Y_OVERCURRENT_INVALID.
 */
Y_OVERCURRENT_enum YVSource::get_overCurrent(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_OVERCURRENT_INVALID;
    }
    return _overCurrent;
}

/**
 * Returns true if the device is not able to maintaint the requested voltage output  .
 * 
 * @return either Y_OVERLOAD_FALSE or Y_OVERLOAD_TRUE, according to true if the device is not able to
 * maintaint the requested voltage output
 * 
 * On failure, throws an exception or returns Y_OVERLOAD_INVALID.
 */
Y_OVERLOAD_enum YVSource::get_overLoad(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_OVERLOAD_INVALID;
    }
    return _overLoad;
}

/**
 * Returns true if the voltage output is too high regarding the requested voltage  .
 * 
 * @return either Y_REGULATIONFAILURE_FALSE or Y_REGULATIONFAILURE_TRUE, according to true if the
 * voltage output is too high regarding the requested voltage
 * 
 * On failure, throws an exception or returns Y_REGULATIONFAILURE_INVALID.
 */
Y_REGULATIONFAILURE_enum YVSource::get_regulationFailure(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_REGULATIONFAILURE_INVALID;
    }
    return _regulationFailure;
}

/**
 * Returns true if external power supply voltage is too low.
 * 
 * @return either Y_EXTPOWERFAILURE_FALSE or Y_EXTPOWERFAILURE_TRUE, according to true if external
 * power supply voltage is too low
 * 
 * On failure, throws an exception or returns Y_EXTPOWERFAILURE_INVALID.
 */
Y_EXTPOWERFAILURE_enum YVSource::get_extPowerFailure(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_EXTPOWERFAILURE_INVALID;
    }
    return _extPowerFailure;
}

const YMove * YVSource::get_move(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_MOVE_INVALID;
    }
    return &_move;
}

int YVSource::set_move(const YMove * newval)
{
    string rest_val;
    char buff[64]; sprintf(buff,"%d:%d",newval->target,newval->ms); rest_val = string(buff);
    return _setAttr("move", rest_val);
}

/**
 * Performs a smooth move at constant speed toward a given value.
 * 
 * @param target      : new output value at end of transition, in milliVolts.
 * @param ms_duration : transition duration, in milliseconds
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YVSource::voltageMove(int target,int ms_duration)
{
    string rest_val;
    char buff[64]; sprintf(buff,"%d:%d",target,ms_duration); rest_val = string(buff);
    return _setAttr("move", rest_val);
}

const YPulse * YVSource::get_pulseTimer(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_PULSETIMER_INVALID;
    }
    return &_pulseTimer;
}

int YVSource::set_pulseTimer(const YPulse * newval)
{
    string rest_val;
    char buff[64]; sprintf(buff,"%d:%d",newval->target,newval->ms); rest_val = string(buff);
    return _setAttr("pulseTimer", rest_val);
}

/**
 * Sets device output to a specific volatage, for a specified duration, then brings it
 * automatically to 0V.
 * 
 * @param voltage : pulse voltage, in millivolts
 * @param ms_duration : pulse duration, in millisecondes
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YVSource::pulse(int voltage,int ms_duration)
{
    string rest_val;
    char buff[64]; sprintf(buff,"%d:%d",voltage,ms_duration); rest_val = string(buff);
    return _setAttr("pulseTimer", rest_val);
}

YVSource *YVSource::nextVSource(void)
{
    string  hwid;
    
    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return yFindVSource(hwid);
}

void YVSource::registerValueCallback(YVSourceUpdateCallback callback)
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

void YVSource::advertiseValue(const string& value)
{
    if (_callback != NULL) {
        _callback(this, value);
    }
}


YVSource* YVSource::FindVSource(const string& func)
{
    if(YVSource::_VSourceCache.find(func) != YVSource::_VSourceCache.end())
        return YVSource::_VSourceCache[func];
    
    YVSource *newVSource = new YVSource(func);
    YVSource::_VSourceCache[func] = newVSource;
    
    return newVSource;
}

YVSource* YVSource::FirstVSource(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;
    
    if(YISERR(YapiWrapper::getFunctionsByClass("VSource", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YVSource::FindVSource(serial+"."+funcId);
}

//--- (end of YVSource implementation)

//--- (VSource functions)
//--- (end of VSource functions)
