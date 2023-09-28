/*********************************************************************
 *
 *  $Id: yocto_sdi12port.cpp 54278 2023-04-28 10:10:10Z seb $
 *
 *  Implements yFindSdi12Port(), the high-level API for Sdi12Port functions
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

#include "yocto_sdi12port.h"
#include "yapi/yproto.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#define  __FILE_ID__  "sdi12port"

#ifdef YOCTOLIB_NAMESPACE
using namespace YOCTOLIB_NAMESPACE;
#endif

YSdi12SnoopingRecord::YSdi12SnoopingRecord(const string& json):
//--- (generated code: YSdi12SnoopingRecord initialization)
    _tim(0)
    ,_dir(0)
//--- (end of generated code: YSdi12SnoopingRecord initialization)
{
    yJsonStateMachine j;
    // Parse JSON data
    j.src = json.c_str();
    j.end = j.src + strlen(j.src);
    j.st = YJSON_START;
    if(yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_STRUCT) {
        return ;
    }
    while(yJsonParse(&j) == YJSON_PARSE_AVAIL && j.st == YJSON_PARSE_MEMBNAME) {
        if (!strcmp(j.token, "m")) {
            string tmp;
            if (yJsonParse(&j) != YJSON_PARSE_AVAIL) {
                return ;
            }
            tmp = (string)j.token;
            while(j.next == YJSON_PARSE_STRINGCONT && yJsonParse(&j) == YJSON_PARSE_AVAIL) {
                tmp +=(string)j.token;
            }
            _dir = (tmp[0] == '<' ? 1 : 0);
            _msg = tmp.substr(1);
        } else if(!strcmp(j.token, "t")) {
            if (yJsonParse(&j) != YJSON_PARSE_AVAIL) {
                return;
            }
            _tim = atoi(j.token);;
        } else {
            yJsonSkip(&j, 1);
        }
    }
}

//--- (generated code: YSdi12SnoopingRecord implementation)
// static attributes


/**
 * Returns the elapsed time, in ms, since the beginning of the preceding message.
 *
 * @return the elapsed time, in ms, since the beginning of the preceding message.
 */
int YSdi12SnoopingRecord::get_time(void)
{
    return _tim;
}

/**
 * Returns the message direction (RX=0, TX=1).
 *
 * @return the message direction (RX=0, TX=1).
 */
int YSdi12SnoopingRecord::get_direction(void)
{
    return _dir;
}

/**
 * Returns the message content.
 *
 * @return the message content.
 */
string YSdi12SnoopingRecord::get_message(void)
{
    return _msg;
}
//--- (end of generated code: YSdi12SnoopingRecord implementation)

YSdi12Sensor::YSdi12Sensor(YSdi12Port *sdi12Port,const string& json):
//--- (generated code: YSdi12Sensor initialization)
    _sdi12Port(NULL)
//--- (end of generated code: YSdi12Sensor initialization)
{
    _sdi12Port = sdi12Port;
    _parseInfoStr(json);
}

//--- (generated code: YSdi12Sensor implementation)
// static attributes


string YSdi12Sensor::get_sensorAddress(void)
{
    return _addr;
}

string YSdi12Sensor::get_sensorProtocol(void)
{
    return _proto;
}

string YSdi12Sensor::get_sensorVendor(void)
{
    return _mfg;
}

string YSdi12Sensor::get_sensorModel(void)
{
    return _model;
}

string YSdi12Sensor::get_sensorVersion(void)
{
    return _ver;
}

string YSdi12Sensor::get_sensorSerial(void)
{
    return _sn;
}

int YSdi12Sensor::get_measureCount(void)
{
    return (int)_valuesDesc.size();
}

string YSdi12Sensor::get_measureCommand(int measureIndex)
{
    return _valuesDesc[measureIndex][0];
}

int YSdi12Sensor::get_measurePosition(int measureIndex)
{
    return atoi((_valuesDesc[measureIndex][2]).c_str());
}

string YSdi12Sensor::get_measureSymbol(int measureIndex)
{
    return _valuesDesc[measureIndex][3];
}

string YSdi12Sensor::get_measureUnit(int measureIndex)
{
    return _valuesDesc[measureIndex][4];
}

string YSdi12Sensor::get_measureDescription(int measureIndex)
{
    return _valuesDesc[measureIndex][5];
}

vector< vector<string> > YSdi12Sensor::get_typeMeasure(void)
{
    return _valuesDesc;
}

void YSdi12Sensor::_parseInfoStr(string infoStr)
{
    string errmsg;

    if ((int)(infoStr).length() > 1) {
        if ((infoStr).substr(0, 2) == "ER") {
            errmsg = (infoStr).substr(2, (int)(infoStr).length()-2);
            _addr = errmsg;
            _proto = errmsg;
            _mfg = errmsg;
            _model = errmsg;
            _ver = errmsg;
            _sn = errmsg;
        } else {
            _addr = (infoStr).substr(0, 1);
            _proto = (infoStr).substr(1, 2);
            _mfg = (infoStr).substr(3, 8);
            _model = (infoStr).substr(11, 6);
            _ver = (infoStr).substr(17, 3);
            _sn = (infoStr).substr(20, (int)(infoStr).length()-20);
        }
    }
}

void YSdi12Sensor::_queryValueInfo(void)
{
    vector< vector<string> > val;
    vector<string> data;
    string infoNbVal;
    string cmd;
    string infoVal;
    string value;
    int nbVal = 0;
    int k = 0;
    int i = 0;
    int j = 0;
    vector<string> listVal;

    k = 0;
    while (k < 10) {
        infoNbVal = _sdi12Port->querySdi12(_addr, YapiWrapper::ysprintf("IM%d",k), 5000);
        if ((int)(infoNbVal).length() > 1) {
            value = (infoNbVal).substr(4, (int)(infoNbVal).length()-4);
            nbVal = atoi((value).c_str());
            if (nbVal != 0) {
                val.clear();
                i = 0;
                while (i < nbVal) {
                    cmd = YapiWrapper::ysprintf("IM%d_00%d", k,i+1);
                    infoVal = _sdi12Port->querySdi12(_addr, cmd, 5000);
                    data = _strsplit(infoVal,';');
                    data = _strsplit(data[0],',');
                    listVal.clear();
                    listVal.push_back(YapiWrapper::ysprintf("M%d",k));
                    listVal.push_back(YapiWrapper::ysprintf("%d",i+1));
                    j = 0;
                    while (j < (int)data.size()) {
                        listVal.push_back(data[j]);
                        j = j + 1;
                    }
                    val.push_back(listVal);
                    i = i + 1;
                }
            }
        }
        k = k + 1;
    }
    _valuesDesc = val;
}
//--- (end of generated code: YSdi12Sensor implementation)

YSdi12Port::YSdi12Port(const string& func): YFunction(func)
//--- (generated code: YSdi12Port initialization)
    ,_rxCount(RXCOUNT_INVALID)
    ,_txCount(TXCOUNT_INVALID)
    ,_errCount(ERRCOUNT_INVALID)
    ,_rxMsgCount(RXMSGCOUNT_INVALID)
    ,_txMsgCount(TXMSGCOUNT_INVALID)
    ,_lastMsg(LASTMSG_INVALID)
    ,_currentJob(CURRENTJOB_INVALID)
    ,_startupJob(STARTUPJOB_INVALID)
    ,_jobMaxTask(JOBMAXTASK_INVALID)
    ,_jobMaxSize(JOBMAXSIZE_INVALID)
    ,_command(COMMAND_INVALID)
    ,_protocol(PROTOCOL_INVALID)
    ,_voltageLevel(VOLTAGELEVEL_INVALID)
    ,_serialMode(SERIALMODE_INVALID)
    ,_valueCallbackSdi12Port(NULL)
    ,_rxptr(0)
    ,_rxbuffptr(0)
    ,_eventPos(0)
//--- (end of generated code: YSdi12Port initialization)
{
    _className="Sdi12Port";
}

YSdi12Port::~YSdi12Port()
{
//--- (generated code: YSdi12Port cleanup)
//--- (end of generated code: YSdi12Port cleanup)
}
//--- (generated code: YSdi12Port implementation)
// static attributes
const string YSdi12Port::LASTMSG_INVALID = YAPI_INVALID_STRING;
const string YSdi12Port::CURRENTJOB_INVALID = YAPI_INVALID_STRING;
const string YSdi12Port::STARTUPJOB_INVALID = YAPI_INVALID_STRING;
const string YSdi12Port::COMMAND_INVALID = YAPI_INVALID_STRING;
const string YSdi12Port::PROTOCOL_INVALID = YAPI_INVALID_STRING;
const string YSdi12Port::SERIALMODE_INVALID = YAPI_INVALID_STRING;

int YSdi12Port::_parseAttr(YJSONObject *json_val)
{
    if(json_val->has("rxCount")) {
        _rxCount =  json_val->getInt("rxCount");
    }
    if(json_val->has("txCount")) {
        _txCount =  json_val->getInt("txCount");
    }
    if(json_val->has("errCount")) {
        _errCount =  json_val->getInt("errCount");
    }
    if(json_val->has("rxMsgCount")) {
        _rxMsgCount =  json_val->getInt("rxMsgCount");
    }
    if(json_val->has("txMsgCount")) {
        _txMsgCount =  json_val->getInt("txMsgCount");
    }
    if(json_val->has("lastMsg")) {
        _lastMsg =  json_val->getString("lastMsg");
    }
    if(json_val->has("currentJob")) {
        _currentJob =  json_val->getString("currentJob");
    }
    if(json_val->has("startupJob")) {
        _startupJob =  json_val->getString("startupJob");
    }
    if(json_val->has("jobMaxTask")) {
        _jobMaxTask =  json_val->getInt("jobMaxTask");
    }
    if(json_val->has("jobMaxSize")) {
        _jobMaxSize =  json_val->getInt("jobMaxSize");
    }
    if(json_val->has("command")) {
        _command =  json_val->getString("command");
    }
    if(json_val->has("protocol")) {
        _protocol =  json_val->getString("protocol");
    }
    if(json_val->has("voltageLevel")) {
        _voltageLevel =  (Y_VOLTAGELEVEL_enum)json_val->getInt("voltageLevel");
    }
    if(json_val->has("serialMode")) {
        _serialMode =  json_val->getString("serialMode");
    }
    return YFunction::_parseAttr(json_val);
}


/**
 * Returns the total number of bytes received since last reset.
 *
 * @return an integer corresponding to the total number of bytes received since last reset
 *
 * On failure, throws an exception or returns YSdi12Port::RXCOUNT_INVALID.
 */
int YSdi12Port::get_rxCount(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSdi12Port::RXCOUNT_INVALID;
                }
            }
        }
        res = _rxCount;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the total number of bytes transmitted since last reset.
 *
 * @return an integer corresponding to the total number of bytes transmitted since last reset
 *
 * On failure, throws an exception or returns YSdi12Port::TXCOUNT_INVALID.
 */
int YSdi12Port::get_txCount(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSdi12Port::TXCOUNT_INVALID;
                }
            }
        }
        res = _txCount;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the total number of communication errors detected since last reset.
 *
 * @return an integer corresponding to the total number of communication errors detected since last reset
 *
 * On failure, throws an exception or returns YSdi12Port::ERRCOUNT_INVALID.
 */
int YSdi12Port::get_errCount(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSdi12Port::ERRCOUNT_INVALID;
                }
            }
        }
        res = _errCount;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the total number of messages received since last reset.
 *
 * @return an integer corresponding to the total number of messages received since last reset
 *
 * On failure, throws an exception or returns YSdi12Port::RXMSGCOUNT_INVALID.
 */
int YSdi12Port::get_rxMsgCount(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSdi12Port::RXMSGCOUNT_INVALID;
                }
            }
        }
        res = _rxMsgCount;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the total number of messages send since last reset.
 *
 * @return an integer corresponding to the total number of messages send since last reset
 *
 * On failure, throws an exception or returns YSdi12Port::TXMSGCOUNT_INVALID.
 */
int YSdi12Port::get_txMsgCount(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSdi12Port::TXMSGCOUNT_INVALID;
                }
            }
        }
        res = _txMsgCount;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the latest message fully received.
 *
 * @return a string corresponding to the latest message fully received
 *
 * On failure, throws an exception or returns YSdi12Port::LASTMSG_INVALID.
 */
string YSdi12Port::get_lastMsg(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSdi12Port::LASTMSG_INVALID;
                }
            }
        }
        res = _lastMsg;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the name of the job file currently in use.
 *
 * @return a string corresponding to the name of the job file currently in use
 *
 * On failure, throws an exception or returns YSdi12Port::CURRENTJOB_INVALID.
 */
string YSdi12Port::get_currentJob(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSdi12Port::CURRENTJOB_INVALID;
                }
            }
        }
        res = _currentJob;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Selects a job file to run immediately. If an empty string is
 * given as argument, stops running current job file.
 *
 * @param newval : a string
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YSdi12Port::set_currentJob(const string& newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = newval;
        res = _setAttr("currentJob", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the job file to use when the device is powered on.
 *
 * @return a string corresponding to the job file to use when the device is powered on
 *
 * On failure, throws an exception or returns YSdi12Port::STARTUPJOB_INVALID.
 */
string YSdi12Port::get_startupJob(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSdi12Port::STARTUPJOB_INVALID;
                }
            }
        }
        res = _startupJob;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the job to use when the device is powered on.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : a string corresponding to the job to use when the device is powered on
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YSdi12Port::set_startupJob(const string& newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = newval;
        res = _setAttr("startupJob", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the maximum number of tasks in a job that the device can handle.
 *
 * @return an integer corresponding to the maximum number of tasks in a job that the device can handle
 *
 * On failure, throws an exception or returns YSdi12Port::JOBMAXTASK_INVALID.
 */
int YSdi12Port::get_jobMaxTask(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration == 0) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSdi12Port::JOBMAXTASK_INVALID;
                }
            }
        }
        res = _jobMaxTask;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns maximum size allowed for job files.
 *
 * @return an integer corresponding to maximum size allowed for job files
 *
 * On failure, throws an exception or returns YSdi12Port::JOBMAXSIZE_INVALID.
 */
int YSdi12Port::get_jobMaxSize(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration == 0) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSdi12Port::JOBMAXSIZE_INVALID;
                }
            }
        }
        res = _jobMaxSize;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

string YSdi12Port::get_command(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSdi12Port::COMMAND_INVALID;
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

int YSdi12Port::set_command(const string& newval)
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
 * Returns the type of protocol used over the serial line, as a string.
 * Possible values are "Line" for ASCII messages separated by CR and/or LF,
 * "Frame:[timeout]ms" for binary messages separated by a delay time,
 * "Char" for a continuous ASCII stream or
 * "Byte" for a continuous binary stream.
 *
 * @return a string corresponding to the type of protocol used over the serial line, as a string
 *
 * On failure, throws an exception or returns YSdi12Port::PROTOCOL_INVALID.
 */
string YSdi12Port::get_protocol(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSdi12Port::PROTOCOL_INVALID;
                }
            }
        }
        res = _protocol;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the type of protocol used over the serial line.
 * Possible values are "Line" for ASCII messages separated by CR and/or LF,
 * "Frame:[timeout]ms" for binary messages separated by a delay time,
 * "Char" for a continuous ASCII stream or
 * "Byte" for a continuous binary stream.
 * The suffix "/[wait]ms" can be added to reduce the transmit rate so that there
 * is always at lest the specified number of milliseconds between each bytes sent.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : a string corresponding to the type of protocol used over the serial line
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YSdi12Port::set_protocol(const string& newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = newval;
        res = _setAttr("protocol", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the voltage level used on the serial line.
 *
 * @return a value among YSdi12Port::VOLTAGELEVEL_OFF, YSdi12Port::VOLTAGELEVEL_TTL3V,
 * YSdi12Port::VOLTAGELEVEL_TTL3VR, YSdi12Port::VOLTAGELEVEL_TTL5V, YSdi12Port::VOLTAGELEVEL_TTL5VR,
 * YSdi12Port::VOLTAGELEVEL_RS232, YSdi12Port::VOLTAGELEVEL_RS485, YSdi12Port::VOLTAGELEVEL_TTL1V8 and
 * YSdi12Port::VOLTAGELEVEL_SDI12 corresponding to the voltage level used on the serial line
 *
 * On failure, throws an exception or returns YSdi12Port::VOLTAGELEVEL_INVALID.
 */
Y_VOLTAGELEVEL_enum YSdi12Port::get_voltageLevel(void)
{
    Y_VOLTAGELEVEL_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSdi12Port::VOLTAGELEVEL_INVALID;
                }
            }
        }
        res = _voltageLevel;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the voltage type used on the serial line. Valid
 * values  will depend on the Yoctopuce device model featuring
 * the serial port feature.  Check your device documentation
 * to find out which values are valid for that specific model.
 * Trying to set an invalid value will have no effect.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : a value among YSdi12Port::VOLTAGELEVEL_OFF, YSdi12Port::VOLTAGELEVEL_TTL3V,
 * YSdi12Port::VOLTAGELEVEL_TTL3VR, YSdi12Port::VOLTAGELEVEL_TTL5V, YSdi12Port::VOLTAGELEVEL_TTL5VR,
 * YSdi12Port::VOLTAGELEVEL_RS232, YSdi12Port::VOLTAGELEVEL_RS485, YSdi12Port::VOLTAGELEVEL_TTL1V8 and
 * YSdi12Port::VOLTAGELEVEL_SDI12 corresponding to the voltage type used on the serial line
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YSdi12Port::set_voltageLevel(Y_VOLTAGELEVEL_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("voltageLevel", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the serial port communication parameters, as a string such as
 * "1200,7E1,Simplex". The string includes the baud rate, the number of data bits,
 * the parity, and the number of stop bits. The suffix "Simplex" denotes
 * the fact that transmission in both directions is multiplexed on the
 * same transmission line.
 *
 * @return a string corresponding to the serial port communication parameters, as a string such as
 *         "1200,7E1,Simplex"
 *
 * On failure, throws an exception or returns YSdi12Port::SERIALMODE_INVALID.
 */
string YSdi12Port::get_serialMode(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSdi12Port::SERIALMODE_INVALID;
                }
            }
        }
        res = _serialMode;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the serial port communication parameters, with a string such as
 * "1200,7E1,Simplex". The string includes the baud rate, the number of data bits,
 * the parity, and the number of stop bits. The suffix "Simplex" denotes
 * the fact that transmission in both directions is multiplexed on the
 * same transmission line.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : a string corresponding to the serial port communication parameters, with a string such as
 *         "1200,7E1,Simplex"
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YSdi12Port::set_serialMode(const string& newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = newval;
        res = _setAttr("serialMode", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Retrieves a SDI12 port for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that the SDI12 port is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the SDI12 port is
 * indeed online at a given time. In case of ambiguity when looking for
 * a SDI12 port by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the SDI12 port, for instance
 *         MyDevice.sdi12Port.
 *
 * @return a YSdi12Port object allowing you to drive the SDI12 port.
 */
YSdi12Port* YSdi12Port::FindSdi12Port(string func)
{
    YSdi12Port* obj = NULL;
    int taken = 0;
    if (YAPI::_apiInitialized) {
        yEnterCriticalSection(&YAPI::_global_cs);
        taken = 1;
    }try {
        obj = (YSdi12Port*) YFunction::_FindFromCache("Sdi12Port", func);
        if (obj == NULL) {
            obj = new YSdi12Port(func);
            YFunction::_AddToCache("Sdi12Port", func, obj);
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
int YSdi12Port::registerValueCallback(YSdi12PortValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackSdi12Port = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YSdi12Port::_invokeValueCallback(string value)
{
    if (_valueCallbackSdi12Port != NULL) {
        _valueCallbackSdi12Port(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

int YSdi12Port::sendCommand(string text)
{
    return this->set_command(text);
}

/**
 * Reads a single line (or message) from the receive buffer, starting at current stream position.
 * This function is intended to be used when the serial port is configured for a message protocol,
 * such as 'Line' mode or frame protocols.
 *
 * If data at current stream position is not available anymore in the receive buffer,
 * the function returns the oldest available line and moves the stream position just after.
 * If no new full line is received, the function returns an empty line.
 *
 * @return a string with a single line of text
 *
 * On failure, throws an exception or returns a negative error code.
 */
string YSdi12Port::readLine(void)
{
    string url;
    string msgbin;
    vector<string> msgarr;
    int msglen = 0;
    string res;

    url = YapiWrapper::ysprintf("rxmsg.json?pos=%d&len=1&maxw=1",_rxptr);
    msgbin = this->_download(url);
    msgarr = this->_json_get_array(msgbin);
    msglen = (int)msgarr.size();
    if (msglen == 0) {
        return "";
    }
    // last element of array is the new position
    msglen = msglen - 1;
    _rxptr = atoi((msgarr[msglen]).c_str());
    if (msglen == 0) {
        return "";
    }
    res = this->_json_get_string(msgarr[0]);
    return res;
}

/**
 * Searches for incoming messages in the serial port receive buffer matching a given pattern,
 * starting at current position. This function will only compare and return printable characters
 * in the message strings. Binary protocols are handled as hexadecimal strings.
 *
 * The search returns all messages matching the expression provided as argument in the buffer.
 * If no matching message is found, the search waits for one up to the specified maximum timeout
 * (in milliseconds).
 *
 * @param pattern : a limited regular expression describing the expected message format,
 *         or an empty string if all messages should be returned (no filtering).
 *         When using binary protocols, the format applies to the hexadecimal
 *         representation of the message.
 * @param maxWait : the maximum number of milliseconds to wait for a message if none is found
 *         in the receive buffer.
 *
 * @return an array of strings containing the messages found, if any.
 *         Binary messages are converted to hexadecimal representation.
 *
 * On failure, throws an exception or returns an empty array.
 */
vector<string> YSdi12Port::readMessages(string pattern,int maxWait)
{
    string url;
    string msgbin;
    vector<string> msgarr;
    int msglen = 0;
    vector<string> res;
    int idx = 0;

    url = YapiWrapper::ysprintf("rxmsg.json?pos=%d&maxw=%d&pat=%s", _rxptr, maxWait,pattern.c_str());
    msgbin = this->_download(url);
    msgarr = this->_json_get_array(msgbin);
    msglen = (int)msgarr.size();
    if (msglen == 0) {
        return res;
    }
    // last element of array is the new position
    msglen = msglen - 1;
    _rxptr = atoi((msgarr[msglen]).c_str());
    idx = 0;
    while (idx < msglen) {
        res.push_back(this->_json_get_string(msgarr[idx]));
        idx = idx + 1;
    }
    return res;
}

/**
 * Changes the current internal stream position to the specified value. This function
 * does not affect the device, it only changes the value stored in the API object
 * for the next read operations.
 *
 * @param absPos : the absolute position index for next read operations.
 *
 * @return nothing.
 */
int YSdi12Port::read_seek(int absPos)
{
    _rxptr = absPos;
    return YAPI_SUCCESS;
}

/**
 * Returns the current absolute stream position pointer of the API object.
 *
 * @return the absolute position index for next read operations.
 */
int YSdi12Port::read_tell(void)
{
    return _rxptr;
}

/**
 * Returns the number of bytes available to read in the input buffer starting from the
 * current absolute stream position pointer of the API object.
 *
 * @return the number of bytes available to read
 */
int YSdi12Port::read_avail(void)
{
    string availPosStr;
    int atPos = 0;
    int res = 0;
    string databin;

    databin = this->_download(YapiWrapper::ysprintf("rxcnt.bin?pos=%d",_rxptr));
    availPosStr = databin;
    atPos = _ystrpos(availPosStr, "@");
    res = atoi(((availPosStr).substr(0, atPos)).c_str());
    return res;
}

int YSdi12Port::end_tell(void)
{
    string availPosStr;
    int atPos = 0;
    int res = 0;
    string databin;

    databin = this->_download(YapiWrapper::ysprintf("rxcnt.bin?pos=%d",_rxptr));
    availPosStr = databin;
    atPos = _ystrpos(availPosStr, "@");
    res = atoi(((availPosStr).substr(atPos+1, (int)(availPosStr).length()-atPos-1)).c_str());
    return res;
}

/**
 * Sends a text line query to the serial port, and reads the reply, if any.
 * This function is intended to be used when the serial port is configured for 'Line' protocol.
 *
 * @param query : the line query to send (without CR/LF)
 * @param maxWait : the maximum number of milliseconds to wait for a reply.
 *
 * @return the next text line received after sending the text query, as a string.
 *         Additional lines can be obtained by calling readLine or readMessages.
 *
 * On failure, throws an exception or returns an empty string.
 */
string YSdi12Port::queryLine(string query,int maxWait)
{
    int prevpos = 0;
    string url;
    string msgbin;
    vector<string> msgarr;
    int msglen = 0;
    string res;
    if ((int)(query).length() <= 80) {
        // fast query
        url = YapiWrapper::ysprintf("rxmsg.json?len=1&maxw=%d&cmd=!%s", maxWait,this->_escapeAttr(query).c_str());
    } else {
        // long query
        prevpos = this->end_tell();
        this->_upload("txdata", query + "\r\n");
        url = YapiWrapper::ysprintf("rxmsg.json?len=1&maxw=%d&pos=%d", maxWait,prevpos);
    }

    msgbin = this->_download(url);
    msgarr = this->_json_get_array(msgbin);
    msglen = (int)msgarr.size();
    if (msglen == 0) {
        return "";
    }
    // last element of array is the new position
    msglen = msglen - 1;
    _rxptr = atoi((msgarr[msglen]).c_str());
    if (msglen == 0) {
        return "";
    }
    res = this->_json_get_string(msgarr[0]);
    return res;
}

/**
 * Sends a binary message to the serial port, and reads the reply, if any.
 * This function is intended to be used when the serial port is configured for
 * Frame-based protocol.
 *
 * @param hexString : the message to send, coded in hexadecimal
 * @param maxWait : the maximum number of milliseconds to wait for a reply.
 *
 * @return the next frame received after sending the message, as a hex string.
 *         Additional frames can be obtained by calling readHex or readMessages.
 *
 * On failure, throws an exception or returns an empty string.
 */
string YSdi12Port::queryHex(string hexString,int maxWait)
{
    int prevpos = 0;
    string url;
    string msgbin;
    vector<string> msgarr;
    int msglen = 0;
    string res;
    if ((int)(hexString).length() <= 80) {
        // fast query
        url = YapiWrapper::ysprintf("rxmsg.json?len=1&maxw=%d&cmd=$%s", maxWait,hexString.c_str());
    } else {
        // long query
        prevpos = this->end_tell();
        this->_upload("txdata", YAPI::_hexStr2Bin(hexString));
        url = YapiWrapper::ysprintf("rxmsg.json?len=1&maxw=%d&pos=%d", maxWait,prevpos);
    }

    msgbin = this->_download(url);
    msgarr = this->_json_get_array(msgbin);
    msglen = (int)msgarr.size();
    if (msglen == 0) {
        return "";
    }
    // last element of array is the new position
    msglen = msglen - 1;
    _rxptr = atoi((msgarr[msglen]).c_str());
    if (msglen == 0) {
        return "";
    }
    res = this->_json_get_string(msgarr[0]);
    return res;
}

/**
 * Saves the job definition string (JSON data) into a job file.
 * The job file can be later enabled using selectJob().
 *
 * @param jobfile : name of the job file to save on the device filesystem
 * @param jsonDef : a string containing a JSON definition of the job
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YSdi12Port::uploadJob(string jobfile,string jsonDef)
{
    this->_upload(jobfile, jsonDef);
    return YAPI_SUCCESS;
}

/**
 * Load and start processing the specified job file. The file must have
 * been previously created using the user interface or uploaded on the
 * device filesystem using the uploadJob() function.
 *
 * @param jobfile : name of the job file (on the device filesystem)
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YSdi12Port::selectJob(string jobfile)
{
    return this->set_currentJob(jobfile);
}

/**
 * Clears the serial port buffer and resets counters to zero.
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YSdi12Port::reset(void)
{
    _eventPos = 0;
    _rxptr = 0;
    _rxbuffptr = 0;
    _rxbuff = string(0, (char)0);

    return this->sendCommand("Z");
}

/**
 * Sends a single byte to the serial port.
 *
 * @param code : the byte to send
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YSdi12Port::writeByte(int code)
{
    return this->sendCommand(YapiWrapper::ysprintf("$%02X",code));
}

/**
 * Sends an ASCII string to the serial port, as is.
 *
 * @param text : the text string to send
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YSdi12Port::writeStr(string text)
{
    string buff;
    int bufflen = 0;
    int idx = 0;
    int ch = 0;
    buff = text;
    bufflen = (int)(buff).size();
    if (bufflen < 100) {
        // if string is pure text, we can send it as a simple command (faster)
        ch = 0x20;
        idx = 0;
        while ((idx < bufflen) && (ch != 0)) {
            ch = ((u8)buff[idx]);
            if ((ch >= 0x20) && (ch < 0x7f)) {
                idx = idx + 1;
            } else {
                ch = 0;
            }
        }
        if (idx >= bufflen) {
            return this->sendCommand(YapiWrapper::ysprintf("+%s",text.c_str()));
        }
    }
    // send string using file upload
    return this->_upload("txdata", buff);
}

/**
 * Sends a binary buffer to the serial port, as is.
 *
 * @param buff : the binary buffer to send
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YSdi12Port::writeBin(string buff)
{
    return this->_upload("txdata", buff);
}

/**
 * Sends a byte sequence (provided as a list of bytes) to the serial port.
 *
 * @param byteList : a list of byte codes
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YSdi12Port::writeArray(vector<int> byteList)
{
    string buff;
    int bufflen = 0;
    int idx = 0;
    int hexb = 0;
    int res = 0;
    bufflen = (int)byteList.size();
    buff = string(bufflen, (char)0);
    idx = 0;
    while (idx < bufflen) {
        hexb = byteList[idx];
        buff[idx] = (char)(hexb);
        idx = idx + 1;
    }

    res = this->_upload("txdata", buff);
    return res;
}

/**
 * Sends a byte sequence (provided as a hexadecimal string) to the serial port.
 *
 * @param hexString : a string of hexadecimal byte codes
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YSdi12Port::writeHex(string hexString)
{
    string buff;
    int bufflen = 0;
    int idx = 0;
    int hexb = 0;
    int res = 0;
    bufflen = (int)(hexString).length();
    if (bufflen < 100) {
        return this->sendCommand(YapiWrapper::ysprintf("$%s",hexString.c_str()));
    }
    bufflen = ((bufflen) >> (1));
    buff = string(bufflen, (char)0);
    idx = 0;
    while (idx < bufflen) {
        hexb = (int)YAPI::_hexStr2Long((hexString).substr(2 * idx, 2));
        buff[idx] = (char)(hexb);
        idx = idx + 1;
    }

    res = this->_upload("txdata", buff);
    return res;
}

/**
 * Sends an ASCII string to the serial port, followed by a line break (CR LF).
 *
 * @param text : the text string to send
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YSdi12Port::writeLine(string text)
{
    string buff;
    int bufflen = 0;
    int idx = 0;
    int ch = 0;
    buff = YapiWrapper::ysprintf("%s\r\n",text.c_str());
    bufflen = (int)(buff).size()-2;
    if (bufflen < 100) {
        // if string is pure text, we can send it as a simple command (faster)
        ch = 0x20;
        idx = 0;
        while ((idx < bufflen) && (ch != 0)) {
            ch = ((u8)buff[idx]);
            if ((ch >= 0x20) && (ch < 0x7f)) {
                idx = idx + 1;
            } else {
                ch = 0;
            }
        }
        if (idx >= bufflen) {
            return this->sendCommand(YapiWrapper::ysprintf("!%s",text.c_str()));
        }
    }
    // send string using file upload
    return this->_upload("txdata", buff);
}

/**
 * Reads one byte from the receive buffer, starting at current stream position.
 * If data at current stream position is not available anymore in the receive buffer,
 * or if there is no data available yet, the function returns YAPI::NO_MORE_DATA.
 *
 * @return the next byte
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YSdi12Port::readByte(void)
{
    int currpos = 0;
    int reqlen = 0;
    string buff;
    int bufflen = 0;
    int mult = 0;
    int endpos = 0;
    int res = 0;
    // first check if we have the requested character in the look-ahead buffer
    bufflen = (int)(_rxbuff).size();
    if ((_rxptr >= _rxbuffptr) && (_rxptr < _rxbuffptr+bufflen)) {
        res = ((u8)_rxbuff[_rxptr-_rxbuffptr]);
        _rxptr = _rxptr + 1;
        return res;
    }
    // try to preload more than one byte to speed-up byte-per-byte access
    currpos = _rxptr;
    reqlen = 1024;
    buff = this->readBin(reqlen);
    bufflen = (int)(buff).size();
    if (_rxptr == currpos+bufflen) {
        res = ((u8)buff[0]);
        _rxptr = currpos+1;
        _rxbuffptr = currpos;
        _rxbuff = buff;
        return res;
    }
    // mixed bidirectional data, retry with a smaller block
    _rxptr = currpos;
    reqlen = 16;
    buff = this->readBin(reqlen);
    bufflen = (int)(buff).size();
    if (_rxptr == currpos+bufflen) {
        res = ((u8)buff[0]);
        _rxptr = currpos+1;
        _rxbuffptr = currpos;
        _rxbuff = buff;
        return res;
    }
    // still mixed, need to process character by character
    _rxptr = currpos;

    buff = this->_download(YapiWrapper::ysprintf("rxdata.bin?pos=%d&len=1",_rxptr));
    bufflen = (int)(buff).size() - 1;
    endpos = 0;
    mult = 1;
    while ((bufflen > 0) && (((u8)buff[bufflen]) != 64)) {
        endpos = endpos + mult * (((u8)buff[bufflen]) - 48);
        mult = mult * 10;
        bufflen = bufflen - 1;
    }
    _rxptr = endpos;
    if (bufflen == 0) {
        return YAPI_NO_MORE_DATA;
    }
    res = ((u8)buff[0]);
    return res;
}

/**
 * Reads data from the receive buffer as a string, starting at current stream position.
 * If data at current stream position is not available anymore in the receive buffer, the
 * function performs a short read.
 *
 * @param nChars : the maximum number of characters to read
 *
 * @return a string with receive buffer contents
 *
 * On failure, throws an exception or returns a negative error code.
 */
string YSdi12Port::readStr(int nChars)
{
    string buff;
    int bufflen = 0;
    int mult = 0;
    int endpos = 0;
    string res;
    if (nChars > 65535) {
        nChars = 65535;
    }

    buff = this->_download(YapiWrapper::ysprintf("rxdata.bin?pos=%d&len=%d", _rxptr,nChars));
    bufflen = (int)(buff).size() - 1;
    endpos = 0;
    mult = 1;
    while ((bufflen > 0) && (((u8)buff[bufflen]) != 64)) {
        endpos = endpos + mult * (((u8)buff[bufflen]) - 48);
        mult = mult * 10;
        bufflen = bufflen - 1;
    }
    _rxptr = endpos;
    res = (buff).substr(0, bufflen);
    return res;
}

/**
 * Reads data from the receive buffer as a binary buffer, starting at current stream position.
 * If data at current stream position is not available anymore in the receive buffer, the
 * function performs a short read.
 *
 * @param nChars : the maximum number of bytes to read
 *
 * @return a binary object with receive buffer contents
 *
 * On failure, throws an exception or returns a negative error code.
 */
string YSdi12Port::readBin(int nChars)
{
    string buff;
    int bufflen = 0;
    int mult = 0;
    int endpos = 0;
    int idx = 0;
    string res;
    if (nChars > 65535) {
        nChars = 65535;
    }

    buff = this->_download(YapiWrapper::ysprintf("rxdata.bin?pos=%d&len=%d", _rxptr,nChars));
    bufflen = (int)(buff).size() - 1;
    endpos = 0;
    mult = 1;
    while ((bufflen > 0) && (((u8)buff[bufflen]) != 64)) {
        endpos = endpos + mult * (((u8)buff[bufflen]) - 48);
        mult = mult * 10;
        bufflen = bufflen - 1;
    }
    _rxptr = endpos;
    res = string(bufflen, (char)0);
    idx = 0;
    while (idx < bufflen) {
        res[idx] = (char)(((u8)buff[idx]));
        idx = idx + 1;
    }
    return res;
}

/**
 * Reads data from the receive buffer as a list of bytes, starting at current stream position.
 * If data at current stream position is not available anymore in the receive buffer, the
 * function performs a short read.
 *
 * @param nChars : the maximum number of bytes to read
 *
 * @return a sequence of bytes with receive buffer contents
 *
 * On failure, throws an exception or returns an empty array.
 */
vector<int> YSdi12Port::readArray(int nChars)
{
    string buff;
    int bufflen = 0;
    int mult = 0;
    int endpos = 0;
    int idx = 0;
    int b = 0;
    vector<int> res;
    if (nChars > 65535) {
        nChars = 65535;
    }

    buff = this->_download(YapiWrapper::ysprintf("rxdata.bin?pos=%d&len=%d", _rxptr,nChars));
    bufflen = (int)(buff).size() - 1;
    endpos = 0;
    mult = 1;
    while ((bufflen > 0) && (((u8)buff[bufflen]) != 64)) {
        endpos = endpos + mult * (((u8)buff[bufflen]) - 48);
        mult = mult * 10;
        bufflen = bufflen - 1;
    }
    _rxptr = endpos;
    res.clear();
    idx = 0;
    while (idx < bufflen) {
        b = ((u8)buff[idx]);
        res.push_back(b);
        idx = idx + 1;
    }
    return res;
}

/**
 * Reads data from the receive buffer as a hexadecimal string, starting at current stream position.
 * If data at current stream position is not available anymore in the receive buffer, the
 * function performs a short read.
 *
 * @param nBytes : the maximum number of bytes to read
 *
 * @return a string with receive buffer contents, encoded in hexadecimal
 *
 * On failure, throws an exception or returns a negative error code.
 */
string YSdi12Port::readHex(int nBytes)
{
    string buff;
    int bufflen = 0;
    int mult = 0;
    int endpos = 0;
    int ofs = 0;
    string res;
    if (nBytes > 65535) {
        nBytes = 65535;
    }

    buff = this->_download(YapiWrapper::ysprintf("rxdata.bin?pos=%d&len=%d", _rxptr,nBytes));
    bufflen = (int)(buff).size() - 1;
    endpos = 0;
    mult = 1;
    while ((bufflen > 0) && (((u8)buff[bufflen]) != 64)) {
        endpos = endpos + mult * (((u8)buff[bufflen]) - 48);
        mult = mult * 10;
        bufflen = bufflen - 1;
    }
    _rxptr = endpos;
    res = "";
    ofs = 0;
    while (ofs + 3 < bufflen) {
        res = YapiWrapper::ysprintf("%s%02X%02X%02X%02X", res.c_str(), ((u8)buff[ofs]), ((u8)buff[ofs + 1]), ((u8)buff[ofs + 2]),((u8)buff[ofs + 3]));
        ofs = ofs + 4;
    }
    while (ofs < bufflen) {
        res = YapiWrapper::ysprintf("%s%02X", res.c_str(),((u8)buff[ofs]));
        ofs = ofs + 1;
    }
    return res;
}

/**
 * Sends a SDI-12 query to the bus, and reads the sensor immediate reply.
 * This function is intended to be used when the serial port is configured for 'SDI-12' protocol.
 *
 * @param sensorAddr : the sensor address, as a string
 * @param cmd : the SDI12 query to send (without address and exclamation point)
 * @param maxWait : the maximum timeout to wait for a reply from sensor, in millisecond
 *
 * @return the reply returned by the sensor, without newline, as a string.
 *
 * On failure, throws an exception or returns an empty string.
 */
string YSdi12Port::querySdi12(string sensorAddr,string cmd,int maxWait)
{
    string fullCmd;
    string cmdChar;
    string pattern;
    string url;
    string msgbin;
    vector<string> msgarr;
    int msglen = 0;
    string res;
    cmdChar  = "";

    pattern = sensorAddr;
    if ((int)(cmd).length() > 0) {
        cmdChar = (cmd).substr(0, 1);
    }
    if (sensorAddr == "?") {
        pattern = "..*";
    } else {
        if (cmdChar == "M" || cmdChar == "D") {
            pattern = YapiWrapper::ysprintf("%s:.*",sensorAddr.c_str());
        } else {
            pattern = YapiWrapper::ysprintf("%s.*",sensorAddr.c_str());
        }
    }
    pattern = this->_escapeAttr(pattern);
    fullCmd = this->_escapeAttr(YapiWrapper::ysprintf("+%s%s!", sensorAddr.c_str(),cmd.c_str()));
    url = YapiWrapper::ysprintf("rxmsg.json?len=1&maxw=%d&cmd=%s&pat=%s", maxWait, fullCmd.c_str(),pattern.c_str());

    msgbin = this->_download(url);
    if ((int)(msgbin).size()<2) {
        return "";
    }
    msgarr = this->_json_get_array(msgbin);
    msglen = (int)msgarr.size();
    if (msglen == 0) {
        return "";
    }
    // last element of array is the new position
    msglen = msglen - 1;
    _rxptr = atoi((msgarr[msglen]).c_str());
    if (msglen == 0) {
        return "";
    }
    res = this->_json_get_string(msgarr[0]);
    return res;
}

/**
 * Sends a discovery command to the bus, and reads the sensor information reply.
 * This function is intended to be used when the serial port is configured for 'SDI-12' protocol.
 * This function work when only one sensor is connected.
 *
 * @return the reply returned by the sensor, as a YSdi12Sensor object.
 *
 * On failure, throws an exception or returns an empty string.
 */
YSdi12Sensor YSdi12Port::discoverSingleSensor(void)
{
    string resStr;

    resStr = this->querySdi12("?","",5000);
    if (resStr == "") {
        return YSdi12Sensor( this,"ERSensor Not Found");
    }

    return this->getSensorInformation(resStr);
}

/**
 * Sends a discovery command to the bus, and reads all sensors information reply.
 * This function is intended to be used when the serial port is configured for 'SDI-12' protocol.
 *
 * @return all the information from every connected sensor, as an array of YSdi12Sensor object.
 *
 * On failure, throws an exception or returns an empty string.
 */
vector<YSdi12Sensor> YSdi12Port::discoverAllSensors(void)
{
    vector<YSdi12Sensor> sensors;
    vector<string> idSens;
    string res;
    int i = 0;
    string lettreMin;
    string lettreMaj;

    // 1. Search for sensors present
    idSens.clear();
    i = 0 ;
    while (i < 10) {
        res = this->querySdi12(YapiWrapper::ysprintf("%d",i),"!",500);
        if ((int)(res).length() >= 1) {
            idSens.push_back(res);
        }
        i = i+1;
    }
    lettreMin = "abcdefghijklmnopqrstuvwxyz";
    lettreMaj = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    i = 0;
    while (i<26) {
        res = this->querySdi12((lettreMin).substr(i, 1),"!",500);
        if ((int)(res).length() >= 1) {
            idSens.push_back(res);
        }
        i = i +1;
    }
    while (i<26) {
        res = this->querySdi12((lettreMaj).substr(i, 1),"!",500);
        if ((int)(res).length() >= 1) {
            idSens.push_back(res);
        }
        i = i +1;
    }
    // 2. Query existing sensors information
    i = 0;
    sensors.clear();
    while (i < (int)idSens.size()) {
        sensors.push_back(this->getSensorInformation(idSens[i]));
        i = i + 1;
    }
    return sensors;
}

/**
 * Sends a mesurement command to the SDI-12 bus, and reads the sensor immediate reply.
 * The supported commands are:
 * M: Measurement start control
 * M1...M9: Additional measurement start command
 * D: Measurement reading control
 * This function is intended to be used when the serial port is configured for 'SDI-12' protocol.
 *
 * @param sensorAddr : the sensor address, as a string
 * @param measCmd : the SDI12 query to send (without address and exclamation point)
 * @param maxWait : the maximum timeout to wait for a reply from sensor, in millisecond
 *
 * @return the reply returned by the sensor, without newline, as a list of float.
 *
 * On failure, throws an exception or returns an empty string.
 */
vector<double> YSdi12Port::readSensor(string sensorAddr,string measCmd,int maxWait)
{
    string resStr;
    vector<double> res;
    vector<string> tab;
    vector<string> split;
    int i = 0;
    double valdouble = 0.0;

    resStr = this->querySdi12(sensorAddr,measCmd,maxWait);
    tab = _strsplit(resStr,',');
    split = _strsplit(tab[0],':');
    if ((int)split.size() < 2) {
        return res;
    }
    valdouble = atof((split[1]).c_str());
    res.push_back(valdouble);
    i = 1;
    while (i < (int)tab.size()) {
        valdouble = atof((tab[i]).c_str());
        res.push_back(valdouble);
        i = i + 1;
    }
    return res;
}

/**
 * Changes the address of the selected sensor, and returns the sensor information with the new address.
 * This function is intended to be used when the serial port is configured for 'SDI-12' protocol.
 *
 * @param oldAddress : Actual sensor address, as a string
 * @param newAddress : New sensor address, as a string
 *
 * @return the sensor address and information , as a YSdi12Sensor object.
 *
 * On failure, throws an exception or returns an empty string.
 */
YSdi12Sensor YSdi12Port::changeAddress(string oldAddress,string newAddress)
{
    YSdi12Sensor addr;

    this->querySdi12(oldAddress, "A" + newAddress,1000);
    addr = this->getSensorInformation(newAddress);
    return addr;
}

/**
 * Sends a information command to the bus, and reads sensors information selected.
 * This function is intended to be used when the serial port is configured for 'SDI-12' protocol.
 *
 * @param sensorAddr : Sensor address, as a string
 *
 * @return the reply returned by the sensor, as a YSdi12Port object.
 *
 * On failure, throws an exception or returns an empty string.
 */
YSdi12Sensor YSdi12Port::getSensorInformation(string sensorAddr)
{
    string res;
    YSdi12Sensor sensor;

    res = this->querySdi12(sensorAddr,"I",1000);
    if (res == "") {
        return YSdi12Sensor(this ,"ERSensor Not Found");
    }
    sensor = YSdi12Sensor(this ,res);
    sensor._queryValueInfo();
    return sensor;
}

/**
 * Sends a information command to the bus, and reads sensors information selected.
 * This function is intended to be used when the serial port is configured for 'SDI-12' protocol.
 *
 * @param sensorAddr : Sensor address, as a string
 *
 * @return the reply returned by the sensor, as a YSdi12Port object.
 *
 * On failure, throws an exception or returns an empty string.
 */
vector<double> YSdi12Port::readConcurrentMeasurements(string sensorAddr)
{
    vector<double> res;

    res= this->readSensor(sensorAddr,"D",1000);
    return res;
}

/**
 * Sends a information command to the bus, and reads sensors information selected.
 * This function is intended to be used when the serial port is configured for 'SDI-12' protocol.
 *
 * @param sensorAddr : Sensor address, as a string
 *
 * @return the reply returned by the sensor, as a YSdi12Port object.
 *
 * On failure, throws an exception or returns an empty string.
 */
int YSdi12Port::requestConcurrentMeasurements(string sensorAddr)
{
    int timewait = 0;
    string wait;

    wait = this->querySdi12(sensorAddr,"C",1000);
    wait = (wait).substr(1, 3);
    timewait = atoi((wait).c_str()) * 1000;
    return timewait;
}

/**
 * Retrieves messages (both direction) in the SDI12 port buffer, starting at current position.
 *
 * If no message is found, the search waits for one up to the specified maximum timeout
 * (in milliseconds).
 *
 * @param maxWait : the maximum number of milliseconds to wait for a message if none is found
 *         in the receive buffer.
 *
 * @return an array of YSdi12SnoopingRecord objects containing the messages found, if any.
 *
 * On failure, throws an exception or returns an empty array.
 */
vector<YSdi12SnoopingRecord> YSdi12Port::snoopMessages(int maxWait)
{
    string url;
    string msgbin;
    vector<string> msgarr;
    int msglen = 0;
    vector<YSdi12SnoopingRecord> res;
    int idx = 0;

    url = YapiWrapper::ysprintf("rxmsg.json?pos=%d&maxw=%d&t=0", _rxptr,maxWait);
    msgbin = this->_download(url);
    msgarr = this->_json_get_array(msgbin);
    msglen = (int)msgarr.size();
    if (msglen == 0) {
        return res;
    }
    // last element of array is the new position
    msglen = msglen - 1;
    _rxptr = atoi((msgarr[msglen]).c_str());
    idx = 0;
    while (idx < msglen) {
        res.push_back(YSdi12SnoopingRecord(msgarr[idx]));
        idx = idx + 1;
    }
    return res;
}

YSdi12Port *YSdi12Port::nextSdi12Port(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YSdi12Port::FindSdi12Port(hwid);
}

YSdi12Port *YSdi12Port::FirstSdi12Port(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("Sdi12Port", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YSdi12Port::FindSdi12Port(serial+"."+funcId);
}

//--- (end of generated code: YSdi12Port implementation)

//--- (generated code: YSdi12Port functions)
//--- (end of generated code: YSdi12Port functions)
