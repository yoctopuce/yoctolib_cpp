/*********************************************************************
 *
 * $Id: pic24config.php 12323 2013-08-13 15:09:18Z mvuilleu $
 *
 * Implements yFindDigitalIO(), the high-level API for DigitalIO functions
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
#include "yocto_digitalio.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

//--- (YDigitalIO constructor)
// Constructor is protected, use yFindDigitalIO factory function to instantiate
YDigitalIO::YDigitalIO(const string& func): YFunction("DigitalIO", func)
//--- (end of YDigitalIO constructor)
//--- (DigitalIO initialization)
            ,_callback(NULL)
            ,_logicalName(Y_LOGICALNAME_INVALID)
            ,_advertisedValue(Y_ADVERTISEDVALUE_INVALID)
            ,_portState(Y_PORTSTATE_INVALID)
            ,_portDirection(Y_PORTDIRECTION_INVALID)
            ,_portOpenDrain(Y_PORTOPENDRAIN_INVALID)
            ,_portSize(Y_PORTSIZE_INVALID)
            ,_outputVoltage(Y_OUTPUTVOLTAGE_INVALID)
            ,_command(Y_COMMAND_INVALID)
//--- (end of DigitalIO initialization)
{}

YDigitalIO::~YDigitalIO() 
{
//--- (YDigitalIO cleanup)
//--- (end of YDigitalIO cleanup)
}
//--- (YDigitalIO implementation)

const string YDigitalIO::LOGICALNAME_INVALID = "!INVALID!";
const string YDigitalIO::ADVERTISEDVALUE_INVALID = "!INVALID!";
const string YDigitalIO::COMMAND_INVALID = "!INVALID!";



int YDigitalIO::_parse(yJsonStateMachine& j)
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
        } else if(!strcmp(j.token, "portState")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _portState =  atoi(j.token);
        } else if(!strcmp(j.token, "portDirection")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _portDirection =  atoi(j.token);
        } else if(!strcmp(j.token, "portOpenDrain")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _portOpenDrain =  atoi(j.token);
        } else if(!strcmp(j.token, "portSize")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _portSize =  atoi(j.token);
        } else if(!strcmp(j.token, "outputVoltage")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _outputVoltage =  (Y_OUTPUTVOLTAGE_enum)atoi(j.token);
        } else if(!strcmp(j.token, "command")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _command =  _parseString(j);
        } else {
            // ignore unknown field
            yJsonSkip(&j, 1);
        }
    }
    if(j.st != YJSON_PARSE_STRUCT) goto failed;
    return 0;
}

/**
 * Returns the logical name of the digital IO port.
 * 
 * @return a string corresponding to the logical name of the digital IO port
 * 
 * On failure, throws an exception or returns Y_LOGICALNAME_INVALID.
 */
string YDigitalIO::get_logicalName(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_LOGICALNAME_INVALID;
    }
    return _logicalName;
}

/**
 * Changes the logical name of the digital IO port. You can use yCheckLogicalName()
 * prior to this call to make sure that your parameter is valid.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 * 
 * @param newval : a string corresponding to the logical name of the digital IO port
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YDigitalIO::set_logicalName(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("logicalName", rest_val);
}

/**
 * Returns the current value of the digital IO port (no more than 6 characters).
 * 
 * @return a string corresponding to the current value of the digital IO port (no more than 6 characters)
 * 
 * On failure, throws an exception or returns Y_ADVERTISEDVALUE_INVALID.
 */
string YDigitalIO::get_advertisedValue(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_ADVERTISEDVALUE_INVALID;
    }
    return _advertisedValue;
}

/**
 * Returns the digital IO port state: bit 0 represents input 0, and so on.
 * 
 * @return an integer corresponding to the digital IO port state: bit 0 represents input 0, and so on
 * 
 * On failure, throws an exception or returns Y_PORTSTATE_INVALID.
 */
int YDigitalIO::get_portState(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_PORTSTATE_INVALID;
    }
    return _portState;
}

/**
 * Changes the digital IO port state: bit 0 represents input 0, and so on. This function has no effect
 * on bits configured as input in portDirection.
 * 
 * @param newval : an integer corresponding to the digital IO port state: bit 0 represents input 0, and so on
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YDigitalIO::set_portState(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
    return _setAttr("portState", rest_val);
}

/**
 * Returns the IO direction of all bits of the port: 0 makes a bit an input, 1 makes it an output.
 * 
 * @return an integer corresponding to the IO direction of all bits of the port: 0 makes a bit an
 * input, 1 makes it an output
 * 
 * On failure, throws an exception or returns Y_PORTDIRECTION_INVALID.
 */
int YDigitalIO::get_portDirection(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_PORTDIRECTION_INVALID;
    }
    return _portDirection;
}

/**
 * Changes the IO direction of all bits of the port: 0 makes a bit an input, 1 makes it an output.
 * Remember to call the saveToFlash() method  to make sure the setting will be kept after a reboot.
 * 
 * @param newval : an integer corresponding to the IO direction of all bits of the port: 0 makes a bit
 * an input, 1 makes it an output
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YDigitalIO::set_portDirection(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
    return _setAttr("portDirection", rest_val);
}

/**
 * Returns the electrical interface for each bit of the port. 0 makes a bit a regular input/output, 1 makes
 * it an open-drain (open-collector) input/output.
 * 
 * @return an integer corresponding to the electrical interface for each bit of the port
 * 
 * On failure, throws an exception or returns Y_PORTOPENDRAIN_INVALID.
 */
int YDigitalIO::get_portOpenDrain(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_PORTOPENDRAIN_INVALID;
    }
    return _portOpenDrain;
}

/**
 * Changes the electrical interface for each bit of the port. 0 makes a bit a regular input/output, 1 makes
 * it an open-drain (open-collector) input/output. Remember to call the
 * saveToFlash() method  to make sure the setting will be kept after a reboot.
 * 
 * @param newval : an integer corresponding to the electrical interface for each bit of the port
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YDigitalIO::set_portOpenDrain(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
    return _setAttr("portOpenDrain", rest_val);
}

/**
 * Returns the number of bits implemented in the I/O port.
 * 
 * @return an integer corresponding to the number of bits implemented in the I/O port
 * 
 * On failure, throws an exception or returns Y_PORTSIZE_INVALID.
 */
unsigned YDigitalIO::get_portSize(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_PORTSIZE_INVALID;
    }
    return _portSize;
}

/**
 * Returns the voltage source used to drive output bits.
 * 
 * @return a value among Y_OUTPUTVOLTAGE_USB_5V, Y_OUTPUTVOLTAGE_USB_3V3 and Y_OUTPUTVOLTAGE_EXT_V
 * corresponding to the voltage source used to drive output bits
 * 
 * On failure, throws an exception or returns Y_OUTPUTVOLTAGE_INVALID.
 */
Y_OUTPUTVOLTAGE_enum YDigitalIO::get_outputVoltage(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_OUTPUTVOLTAGE_INVALID;
    }
    return _outputVoltage;
}

/**
 * Changes the voltage source used to drive output bits.
 * Remember to call the saveToFlash() method  to make sure the setting will be kept after a reboot.
 * 
 * @param newval : a value among Y_OUTPUTVOLTAGE_USB_5V, Y_OUTPUTVOLTAGE_USB_3V3 and
 * Y_OUTPUTVOLTAGE_EXT_V corresponding to the voltage source used to drive output bits
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YDigitalIO::set_outputVoltage(Y_OUTPUTVOLTAGE_enum newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
    return _setAttr("outputVoltage", rest_val);
}

string YDigitalIO::get_command(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_COMMAND_INVALID;
    }
    return _command;
}

int YDigitalIO::set_command(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("command", rest_val);
}

/**
 * Set a single bit of the I/O port.
 * 
 * @param bitno: the bit number; lowest bit is index 0
 * @param bitval: the value of the bit (1 or 0)
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YDigitalIO::set_bitState(int bitno,int bitval)
{
    if (!(bitval >= 0)) {this->_throw( YAPI_INVALID_ARGUMENT, "invalid bitval"); return  YAPI_INVALID_ARGUMENT;};
    if (!(bitval <= 1)) {this->_throw( YAPI_INVALID_ARGUMENT, "invalid bitval"); return  YAPI_INVALID_ARGUMENT;};
    return this->set_command(YapiWrapper::ysprintf("%c%d",82+bitval, bitno)); 
    
}

/**
 * Returns the value of a single bit of the I/O port.
 * 
 * @param bitno: the bit number; lowest bit is index 0
 * 
 * @return the bit value (0 or 1)
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YDigitalIO::get_bitState(int bitno)
{
    int portVal;
    portVal = this->get_portState();
    return ((((portVal) >> (bitno))) & (1));
    
}

/**
 * Revert a single bit of the I/O port.
 * 
 * @param bitno: the bit number; lowest bit is index 0
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YDigitalIO::toggle_bitState(int bitno)
{
    return this->set_command(YapiWrapper::ysprintf("T%d", bitno)); 
    
}

/**
 * Change  the direction of a single bit from the I/O port.
 * 
 * @param bitno: the bit number; lowest bit is index 0
 * @param bitdirection: direction to set, 0 makes the bit an input, 1 makes it an output.
 *         Remember to call the   saveToFlash() method to make sure the setting will be kept after a reboot.
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YDigitalIO::set_bitDirection(int bitno,int bitdirection)
{
    if (!(bitdirection >= 0)) {this->_throw( YAPI_INVALID_ARGUMENT, "invalid direction"); return  YAPI_INVALID_ARGUMENT;};
    if (!(bitdirection <= 1)) {this->_throw( YAPI_INVALID_ARGUMENT, "invalid direction"); return  YAPI_INVALID_ARGUMENT;};
    return this->set_command(YapiWrapper::ysprintf("%c%d",73+6*bitdirection, bitno)); 
    
}

/**
 * Change  the direction of a single bit from the I/O port (0 means the bit is an input, 1  an output).
 * 
 * @param bitno: the bit number; lowest bit is index 0
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YDigitalIO::get_bitDirection(int bitno)
{
    int portDir;
    portDir = this->get_portDirection();
    return ((((portDir) >> (bitno))) & (1));
    
}

/**
 * Change  the electrical interface of a single bit from the I/O port.
 * 
 * @param bitno: the bit number; lowest bit is index 0
 * @param opendrain: value to set, 0 makes a bit a regular input/output, 1 makes
 *         it an open-drain (open-collector) input/output. Remember to call the
 *         saveToFlash() method to make sure the setting will be kept after a reboot.
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YDigitalIO::set_bitOpenDrain(int bitno,int opendrain)
{
    if (!(opendrain >= 0)) {this->_throw( YAPI_INVALID_ARGUMENT, "invalid state"); return  YAPI_INVALID_ARGUMENT;};
    if (!(opendrain <= 1)) {this->_throw( YAPI_INVALID_ARGUMENT, "invalid state"); return  YAPI_INVALID_ARGUMENT;};
    return this->set_command(YapiWrapper::ysprintf("%c%d",100-32*opendrain, bitno)); 
    
}

/**
 * Returns the type of electrical interface of a single bit from the I/O port. (0 means the bit is an
 * input, 1  an output).
 * 
 * @param bitno: the bit number; lowest bit is index 0
 * 
 * @return   0 means the a bit is a regular input/output, 1means the b it an open-drain
 * (open-collector) input/output.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YDigitalIO::get_bitOpenDrain(int bitno)
{
    int portOpenDrain;
    portOpenDrain = this->get_portOpenDrain();
    return ((((portOpenDrain) >> (bitno))) & (1));
    
}

YDigitalIO *YDigitalIO::nextDigitalIO(void)
{
    string  hwid;
    
    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return yFindDigitalIO(hwid);
}

void YDigitalIO::registerValueCallback(YDigitalIOUpdateCallback callback)
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

void YDigitalIO::advertiseValue(const string& value)
{
    if (_callback != NULL) {
        _callback(this, value);
    }
}


YDigitalIO* YDigitalIO::FindDigitalIO(const string& func)
{
    if(YAPI::_YFunctionsCaches["YDigitalIO"].find(func) != YAPI::_YFunctionsCaches["YDigitalIO"].end())
        return (YDigitalIO*) YAPI::_YFunctionsCaches["YDigitalIO"][func];
    
    YDigitalIO *newDigitalIO = new YDigitalIO(func);
    YAPI::_YFunctionsCaches["YDigitalIO"][func] = newDigitalIO ;
    return newDigitalIO;
}

YDigitalIO* YDigitalIO::FirstDigitalIO(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;
    
    if(YISERR(YapiWrapper::getFunctionsByClass("DigitalIO", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YDigitalIO::FindDigitalIO(serial+"."+funcId);
}

//--- (end of YDigitalIO implementation)

//--- (DigitalIO functions)
//--- (end of DigitalIO functions)
