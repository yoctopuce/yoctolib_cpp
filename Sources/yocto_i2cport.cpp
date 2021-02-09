/*********************************************************************
 *
 *  $Id: yocto_i2cport.cpp 43580 2021-01-26 17:46:01Z mvuilleu $
 *
 *  Implements yFindI2cPort(), the high-level API for I2cPort functions
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

#include "yocto_i2cport.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#define  __FILE_ID__  "i2cport"

#ifdef YOCTOLIB_NAMESPACE
using namespace YOCTOLIB_NAMESPACE;
#endif

YI2cSnoopingRecord::YI2cSnoopingRecord(const string& json):
//--- (generated code: YI2cSnoopingRecord initialization)
    _tim(0)
    ,_dir(0)
//--- (end of generated code: YI2cSnoopingRecord initialization)
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

//--- (generated code: YI2cSnoopingRecord implementation)
// static attributes


/**
 * Returns the elapsed time, in ms, since the beginning of the preceding message.
 *
 * @return the elapsed time, in ms, since the beginning of the preceding message.
 */
int YI2cSnoopingRecord::get_time(void)
{
    return _tim;
}

/**
 * Returns the message direction (RX=0, TX=1).
 *
 * @return the message direction (RX=0, TX=1).
 */
int YI2cSnoopingRecord::get_direction(void)
{
    return _dir;
}

/**
 * Returns the message content.
 *
 * @return the message content.
 */
string YI2cSnoopingRecord::get_message(void)
{
    return _msg;
}
//--- (end of generated code: YI2cSnoopingRecord implementation)

YI2cPort::YI2cPort(const string& func): YFunction(func)
//--- (generated code: YI2cPort initialization)
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
    ,_i2cVoltageLevel(I2CVOLTAGELEVEL_INVALID)
    ,_i2cMode(I2CMODE_INVALID)
    ,_valueCallbackI2cPort(NULL)
    ,_rxptr(0)
    ,_rxbuffptr(0)
//--- (end of generated code: YI2cPort initialization)
{
    _className="I2cPort";
}

YI2cPort::~YI2cPort()
{
//--- (generated code: YI2cPort cleanup)
//--- (end of generated code: YI2cPort cleanup)
}
//--- (generated code: YI2cPort implementation)
// static attributes
const string YI2cPort::LASTMSG_INVALID = YAPI_INVALID_STRING;
const string YI2cPort::CURRENTJOB_INVALID = YAPI_INVALID_STRING;
const string YI2cPort::STARTUPJOB_INVALID = YAPI_INVALID_STRING;
const string YI2cPort::COMMAND_INVALID = YAPI_INVALID_STRING;
const string YI2cPort::PROTOCOL_INVALID = YAPI_INVALID_STRING;
const string YI2cPort::I2CMODE_INVALID = YAPI_INVALID_STRING;

int YI2cPort::_parseAttr(YJSONObject *json_val)
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
    if(json_val->has("i2cVoltageLevel")) {
        _i2cVoltageLevel =  (Y_I2CVOLTAGELEVEL_enum)json_val->getInt("i2cVoltageLevel");
    }
    if(json_val->has("i2cMode")) {
        _i2cMode =  json_val->getString("i2cMode");
    }
    return YFunction::_parseAttr(json_val);
}


/**
 * Returns the total number of bytes received since last reset.
 *
 * @return an integer corresponding to the total number of bytes received since last reset
 *
 * On failure, throws an exception or returns YI2cPort::RXCOUNT_INVALID.
 */
int YI2cPort::get_rxCount(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YI2cPort::RXCOUNT_INVALID;
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
 * On failure, throws an exception or returns YI2cPort::TXCOUNT_INVALID.
 */
int YI2cPort::get_txCount(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YI2cPort::TXCOUNT_INVALID;
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
 * On failure, throws an exception or returns YI2cPort::ERRCOUNT_INVALID.
 */
int YI2cPort::get_errCount(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YI2cPort::ERRCOUNT_INVALID;
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
 * On failure, throws an exception or returns YI2cPort::RXMSGCOUNT_INVALID.
 */
int YI2cPort::get_rxMsgCount(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YI2cPort::RXMSGCOUNT_INVALID;
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
 * On failure, throws an exception or returns YI2cPort::TXMSGCOUNT_INVALID.
 */
int YI2cPort::get_txMsgCount(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YI2cPort::TXMSGCOUNT_INVALID;
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
 * Returns the latest message fully received (for Line and Frame protocols).
 *
 * @return a string corresponding to the latest message fully received (for Line and Frame protocols)
 *
 * On failure, throws an exception or returns YI2cPort::LASTMSG_INVALID.
 */
string YI2cPort::get_lastMsg(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YI2cPort::LASTMSG_INVALID;
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
 * On failure, throws an exception or returns YI2cPort::CURRENTJOB_INVALID.
 */
string YI2cPort::get_currentJob(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YI2cPort::CURRENTJOB_INVALID;
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
int YI2cPort::set_currentJob(const string& newval)
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
 * On failure, throws an exception or returns YI2cPort::STARTUPJOB_INVALID.
 */
string YI2cPort::get_startupJob(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YI2cPort::STARTUPJOB_INVALID;
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
int YI2cPort::set_startupJob(const string& newval)
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
 * On failure, throws an exception or returns YI2cPort::JOBMAXTASK_INVALID.
 */
int YI2cPort::get_jobMaxTask(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration == 0) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YI2cPort::JOBMAXTASK_INVALID;
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
 * On failure, throws an exception or returns YI2cPort::JOBMAXSIZE_INVALID.
 */
int YI2cPort::get_jobMaxSize(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration == 0) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YI2cPort::JOBMAXSIZE_INVALID;
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

string YI2cPort::get_command(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YI2cPort::COMMAND_INVALID;
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

int YI2cPort::set_command(const string& newval)
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
 * Returns the type of protocol used to send I2C messages, as a string.
 * Possible values are
 * "Line" for messages separated by LF or
 * "Char" for continuous stream of codes.
 *
 * @return a string corresponding to the type of protocol used to send I2C messages, as a string
 *
 * On failure, throws an exception or returns YI2cPort::PROTOCOL_INVALID.
 */
string YI2cPort::get_protocol(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YI2cPort::PROTOCOL_INVALID;
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
 * Changes the type of protocol used to send I2C messages.
 * Possible values are
 * "Line" for messages separated by LF or
 * "Char" for continuous stream of codes.
 * The suffix "/[wait]ms" can be added to reduce the transmit rate so that there
 * is always at lest the specified number of milliseconds between each message sent.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : a string corresponding to the type of protocol used to send I2C messages
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YI2cPort::set_protocol(const string& newval)
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
 * Returns the voltage level used on the I2C bus.
 *
 * @return a value among YI2cPort::I2CVOLTAGELEVEL_OFF, YI2cPort::I2CVOLTAGELEVEL_3V3 and
 * YI2cPort::I2CVOLTAGELEVEL_1V8 corresponding to the voltage level used on the I2C bus
 *
 * On failure, throws an exception or returns YI2cPort::I2CVOLTAGELEVEL_INVALID.
 */
Y_I2CVOLTAGELEVEL_enum YI2cPort::get_i2cVoltageLevel(void)
{
    Y_I2CVOLTAGELEVEL_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YI2cPort::I2CVOLTAGELEVEL_INVALID;
                }
            }
        }
        res = _i2cVoltageLevel;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the voltage level used on the I2C bus.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : a value among YI2cPort::I2CVOLTAGELEVEL_OFF, YI2cPort::I2CVOLTAGELEVEL_3V3 and
 * YI2cPort::I2CVOLTAGELEVEL_1V8 corresponding to the voltage level used on the I2C bus
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YI2cPort::set_i2cVoltageLevel(Y_I2CVOLTAGELEVEL_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
        res = _setAttr("i2cVoltageLevel", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the I2C port communication parameters, as a string such as
 * "400kbps,2000ms,NoRestart". The string includes the baud rate, the
 * recovery delay after communications errors, and if needed the option
 * NoRestart to use a Stop/Start sequence instead of the
 * Restart state when performing read on the I2C bus.
 *
 * @return a string corresponding to the I2C port communication parameters, as a string such as
 *         "400kbps,2000ms,NoRestart"
 *
 * On failure, throws an exception or returns YI2cPort::I2CMODE_INVALID.
 */
string YI2cPort::get_i2cMode(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YI2cPort::I2CMODE_INVALID;
                }
            }
        }
        res = _i2cMode;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the I2C port communication parameters, with a string such as
 * "400kbps,2000ms". The string includes the baud rate, the
 * recovery delay after communications errors, and if needed the option
 * NoRestart to use a Stop/Start sequence instead of the
 * Restart state when performing read on the I2C bus.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : a string corresponding to the I2C port communication parameters, with a string such as
 *         "400kbps,2000ms"
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YI2cPort::set_i2cMode(const string& newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = newval;
        res = _setAttr("i2cMode", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Retrieves an I2C port for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that the I2C port is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the I2C port is
 * indeed online at a given time. In case of ambiguity when looking for
 * an I2C port by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the I2C port, for instance
 *         YI2CMK01.i2cPort.
 *
 * @return a YI2cPort object allowing you to drive the I2C port.
 */
YI2cPort* YI2cPort::FindI2cPort(string func)
{
    YI2cPort* obj = NULL;
    int taken = 0;
    if (YAPI::_apiInitialized) {
        yEnterCriticalSection(&YAPI::_global_cs);
        taken = 1;
    }try {
        obj = (YI2cPort*) YFunction::_FindFromCache("I2cPort", func);
        if (obj == NULL) {
            obj = new YI2cPort(func);
            YFunction::_AddToCache("I2cPort", func, obj);
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
int YI2cPort::registerValueCallback(YI2cPortValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackI2cPort = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YI2cPort::_invokeValueCallback(string value)
{
    if (_valueCallbackI2cPort != NULL) {
        _valueCallbackI2cPort(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

int YI2cPort::sendCommand(string text)
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
string YI2cPort::readLine(void)
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
vector<string> YI2cPort::readMessages(string pattern,int maxWait)
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
int YI2cPort::read_seek(int absPos)
{
    _rxptr = absPos;
    return YAPI_SUCCESS;
}

/**
 * Returns the current absolute stream position pointer of the API object.
 *
 * @return the absolute position index for next read operations.
 */
int YI2cPort::read_tell(void)
{
    return _rxptr;
}

/**
 * Returns the number of bytes available to read in the input buffer starting from the
 * current absolute stream position pointer of the API object.
 *
 * @return the number of bytes available to read
 */
int YI2cPort::read_avail(void)
{
    string buff;
    int bufflen = 0;
    int res = 0;

    buff = this->_download(YapiWrapper::ysprintf("rxcnt.bin?pos=%d",_rxptr));
    bufflen = (int)(buff).size() - 1;
    while ((bufflen > 0) && (((u8)buff[bufflen]) != 64)) {
        bufflen = bufflen - 1;
    }
    res = atoi(((buff).substr( 0, bufflen)).c_str());
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
string YI2cPort::queryLine(string query,int maxWait)
{
    string url;
    string msgbin;
    vector<string> msgarr;
    int msglen = 0;
    string res;

    url = YapiWrapper::ysprintf("rxmsg.json?len=1&maxw=%d&cmd=!%s", maxWait,this->_escapeAttr(query).c_str());
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
string YI2cPort::queryHex(string hexString,int maxWait)
{
    string url;
    string msgbin;
    vector<string> msgarr;
    int msglen = 0;
    string res;

    url = YapiWrapper::ysprintf("rxmsg.json?len=1&maxw=%d&cmd=$%s", maxWait,hexString.c_str());
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
int YI2cPort::uploadJob(string jobfile,string jsonDef)
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
int YI2cPort::selectJob(string jobfile)
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
int YI2cPort::reset(void)
{
    _rxptr = 0;
    _rxbuffptr = 0;
    _rxbuff = string(0, (char)0);

    return this->sendCommand("Z");
}

/**
 * Sends a one-way message (provided as a a binary buffer) to a device on the I2C bus.
 * This function checks and reports communication errors on the I2C bus.
 *
 * @param slaveAddr : the 7-bit address of the slave device (without the direction bit)
 * @param buff : the binary buffer to be sent
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YI2cPort::i2cSendBin(int slaveAddr,string buff)
{
    int nBytes = 0;
    int idx = 0;
    int val = 0;
    string msg;
    string reply;
    msg = YapiWrapper::ysprintf("@%02x:",slaveAddr);
    nBytes = (int)(buff).size();
    idx = 0;
    while (idx < nBytes) {
        val = ((u8)buff[idx]);
        msg = YapiWrapper::ysprintf("%s%02x", msg.c_str(),val);
        idx = idx + 1;
    }

    reply = this->queryLine(msg,1000);
    if (!((int)(reply).length() > 0)) {
        _throw(YAPI_IO_ERROR,"No response from I2C device");
        return YAPI_IO_ERROR;
    }
    idx = _ystrpos(reply, "[N]!");
    if (!(idx < 0)) {
        _throw(YAPI_IO_ERROR,"No I2C ACK received");
        return YAPI_IO_ERROR;
    }
    idx = _ystrpos(reply, "!");
    if (!(idx < 0)) {
        _throw(YAPI_IO_ERROR,"I2C protocol error");
        return YAPI_IO_ERROR;
    }
    return YAPI_SUCCESS;
}

/**
 * Sends a one-way message (provided as a list of integer) to a device on the I2C bus.
 * This function checks and reports communication errors on the I2C bus.
 *
 * @param slaveAddr : the 7-bit address of the slave device (without the direction bit)
 * @param values : a list of data bytes to be sent
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YI2cPort::i2cSendArray(int slaveAddr,vector<int> values)
{
    int nBytes = 0;
    int idx = 0;
    int val = 0;
    string msg;
    string reply;
    msg = YapiWrapper::ysprintf("@%02x:",slaveAddr);
    nBytes = (int)values.size();
    idx = 0;
    while (idx < nBytes) {
        val = values[idx];
        msg = YapiWrapper::ysprintf("%s%02x", msg.c_str(),val);
        idx = idx + 1;
    }

    reply = this->queryLine(msg,1000);
    if (!((int)(reply).length() > 0)) {
        _throw(YAPI_IO_ERROR,"No response from I2C device");
        return YAPI_IO_ERROR;
    }
    idx = _ystrpos(reply, "[N]!");
    if (!(idx < 0)) {
        _throw(YAPI_IO_ERROR,"No I2C ACK received");
        return YAPI_IO_ERROR;
    }
    idx = _ystrpos(reply, "!");
    if (!(idx < 0)) {
        _throw(YAPI_IO_ERROR,"I2C protocol error");
        return YAPI_IO_ERROR;
    }
    return YAPI_SUCCESS;
}

/**
 * Sends a one-way message (provided as a a binary buffer) to a device on the I2C bus,
 * then read back the specified number of bytes from device.
 * This function checks and reports communication errors on the I2C bus.
 *
 * @param slaveAddr : the 7-bit address of the slave device (without the direction bit)
 * @param buff : the binary buffer to be sent
 * @param rcvCount : the number of bytes to receive once the data bytes are sent
 *
 * @return a list of bytes with the data received from slave device.
 *
 * On failure, throws an exception or returns an empty binary buffer.
 */
string YI2cPort::i2cSendAndReceiveBin(int slaveAddr,string buff,int rcvCount)
{
    int nBytes = 0;
    int idx = 0;
    int val = 0;
    string msg;
    string reply;
    string rcvbytes;
    msg = YapiWrapper::ysprintf("@%02x:",slaveAddr);
    nBytes = (int)(buff).size();
    idx = 0;
    while (idx < nBytes) {
        val = ((u8)buff[idx]);
        msg = YapiWrapper::ysprintf("%s%02x", msg.c_str(),val);
        idx = idx + 1;
    }
    idx = 0;
    while (idx < rcvCount) {
        msg = YapiWrapper::ysprintf("%sxx",msg.c_str());
        idx = idx + 1;
    }

    reply = this->queryLine(msg,1000);
    rcvbytes = string(0, (char)0);
    if (!((int)(reply).length() > 0)) {
        _throw(YAPI_IO_ERROR,"No response from I2C device");
        return rcvbytes;
    }
    idx = _ystrpos(reply, "[N]!");
    if (!(idx < 0)) {
        _throw(YAPI_IO_ERROR,"No I2C ACK received");
        return rcvbytes;
    }
    idx = _ystrpos(reply, "!");
    if (!(idx < 0)) {
        _throw(YAPI_IO_ERROR,"I2C protocol error");
        return rcvbytes;
    }
    reply = (reply).substr( (int)(reply).length()-2*rcvCount, 2*rcvCount);
    rcvbytes = YAPI::_hexStr2Bin(reply);
    return rcvbytes;
}

/**
 * Sends a one-way message (provided as a list of integer) to a device on the I2C bus,
 * then read back the specified number of bytes from device.
 * This function checks and reports communication errors on the I2C bus.
 *
 * @param slaveAddr : the 7-bit address of the slave device (without the direction bit)
 * @param values : a list of data bytes to be sent
 * @param rcvCount : the number of bytes to receive once the data bytes are sent
 *
 * @return a list of bytes with the data received from slave device.
 *
 * On failure, throws an exception or returns an empty array.
 */
vector<int> YI2cPort::i2cSendAndReceiveArray(int slaveAddr,vector<int> values,int rcvCount)
{
    int nBytes = 0;
    int idx = 0;
    int val = 0;
    string msg;
    string reply;
    string rcvbytes;
    vector<int> res;
    msg = YapiWrapper::ysprintf("@%02x:",slaveAddr);
    nBytes = (int)values.size();
    idx = 0;
    while (idx < nBytes) {
        val = values[idx];
        msg = YapiWrapper::ysprintf("%s%02x", msg.c_str(),val);
        idx = idx + 1;
    }
    idx = 0;
    while (idx < rcvCount) {
        msg = YapiWrapper::ysprintf("%sxx",msg.c_str());
        idx = idx + 1;
    }

    reply = this->queryLine(msg,1000);
    if (!((int)(reply).length() > 0)) {
        _throw(YAPI_IO_ERROR,"No response from I2C device");
        return res;
    }
    idx = _ystrpos(reply, "[N]!");
    if (!(idx < 0)) {
        _throw(YAPI_IO_ERROR,"No I2C ACK received");
        return res;
    }
    idx = _ystrpos(reply, "!");
    if (!(idx < 0)) {
        _throw(YAPI_IO_ERROR,"I2C protocol error");
        return res;
    }
    reply = (reply).substr( (int)(reply).length()-2*rcvCount, 2*rcvCount);
    rcvbytes = YAPI::_hexStr2Bin(reply);
    res.clear();
    idx = 0;
    while (idx < rcvCount) {
        val = ((u8)rcvbytes[idx]);
        res.push_back(val);
        idx = idx + 1;
    }
    return res;
}

/**
 * Sends a text-encoded I2C code stream to the I2C bus, as is.
 * An I2C code stream is a string made of hexadecimal data bytes,
 * but that may also include the I2C state transitions code:
 * "{S}" to emit a start condition,
 * "{R}" for a repeated start condition,
 * "{P}" for a stop condition,
 * "xx" for receiving a data byte,
 * "{A}" to ack a data byte received and
 * "{N}" to nack a data byte received.
 * If a newline ("\n") is included in the stream, the message
 * will be terminated and a newline will also be added to the
 * receive stream.
 *
 * @param codes : the code stream to send
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YI2cPort::writeStr(string codes)
{
    int bufflen = 0;
    string buff;
    int idx = 0;
    int ch = 0;
    buff = codes;
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
            return this->sendCommand(YapiWrapper::ysprintf("+%s",codes.c_str()));
        }
    }
    // send string using file upload
    return this->_upload("txdata", buff);
}

/**
 * Sends a text-encoded I2C code stream to the I2C bus, and terminate
 * the message en relâchant le bus.
 * An I2C code stream is a string made of hexadecimal data bytes,
 * but that may also include the I2C state transitions code:
 * "{S}" to emit a start condition,
 * "{R}" for a repeated start condition,
 * "{P}" for a stop condition,
 * "xx" for receiving a data byte,
 * "{A}" to ack a data byte received and
 * "{N}" to nack a data byte received.
 * At the end of the stream, a stop condition is added if missing
 * and a newline is added to the receive buffer as well.
 *
 * @param codes : the code stream to send
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YI2cPort::writeLine(string codes)
{
    int bufflen = 0;
    string buff;
    bufflen = (int)(codes).length();
    if (bufflen < 100) {
        return this->sendCommand(YapiWrapper::ysprintf("!%s",codes.c_str()));
    }
    // send string using file upload
    buff = YapiWrapper::ysprintf("%s\n",codes.c_str());
    return this->_upload("txdata", buff);
}

/**
 * Sends a single byte to the I2C bus. Depending on the I2C bus state, the byte
 * will be interpreted as an address byte or a data byte.
 *
 * @param code : the byte to send
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YI2cPort::writeByte(int code)
{
    return this->sendCommand(YapiWrapper::ysprintf("+%02X",code));
}

/**
 * Sends a byte sequence (provided as a hexadecimal string) to the I2C bus.
 * Depending on the I2C bus state, the first byte will be interpreted as an
 * address byte or a data byte.
 *
 * @param hexString : a string of hexadecimal byte codes
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YI2cPort::writeHex(string hexString)
{
    int bufflen = 0;
    string buff;
    bufflen = (int)(hexString).length();
    if (bufflen < 100) {
        return this->sendCommand(YapiWrapper::ysprintf("+%s",hexString.c_str()));
    }
    buff = hexString;

    return this->_upload("txdata", buff);
}

/**
 * Sends a binary buffer to the I2C bus, as is.
 * Depending on the I2C bus state, the first byte will be interpreted
 * as an address byte or a data byte.
 *
 * @param buff : the binary buffer to send
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YI2cPort::writeBin(string buff)
{
    int nBytes = 0;
    int idx = 0;
    int val = 0;
    string msg;
    msg = "";
    nBytes = (int)(buff).size();
    idx = 0;
    while (idx < nBytes) {
        val = ((u8)buff[idx]);
        msg = YapiWrapper::ysprintf("%s%02x", msg.c_str(),val);
        idx = idx + 1;
    }

    return this->writeHex(msg);
}

/**
 * Sends a byte sequence (provided as a list of bytes) to the I2C bus.
 * Depending on the I2C bus state, the first byte will be interpreted as an
 * address byte or a data byte.
 *
 * @param byteList : a list of byte codes
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YI2cPort::writeArray(vector<int> byteList)
{
    int nBytes = 0;
    int idx = 0;
    int val = 0;
    string msg;
    msg = "";
    nBytes = (int)byteList.size();
    idx = 0;
    while (idx < nBytes) {
        val = byteList[idx];
        msg = YapiWrapper::ysprintf("%s%02x", msg.c_str(),val);
        idx = idx + 1;
    }

    return this->writeHex(msg);
}

/**
 * Retrieves messages (both direction) in the I2C port buffer, starting at current position.
 *
 * If no message is found, the search waits for one up to the specified maximum timeout
 * (in milliseconds).
 *
 * @param maxWait : the maximum number of milliseconds to wait for a message if none is found
 *         in the receive buffer.
 *
 * @return an array of YI2cSnoopingRecord objects containing the messages found, if any.
 *
 * On failure, throws an exception or returns an empty array.
 */
vector<YI2cSnoopingRecord> YI2cPort::snoopMessages(int maxWait)
{
    string url;
    string msgbin;
    vector<string> msgarr;
    int msglen = 0;
    vector<YI2cSnoopingRecord> res;
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
        res.push_back(YI2cSnoopingRecord(msgarr[idx]));
        idx = idx + 1;
    }
    return res;
}

YI2cPort *YI2cPort::nextI2cPort(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YI2cPort::FindI2cPort(hwid);
}

YI2cPort *YI2cPort::FirstI2cPort(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("I2cPort", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YI2cPort::FindI2cPort(serial+"."+funcId);
}

//--- (end of generated code: YI2cPort implementation)

//--- (generated code: YI2cPort functions)
//--- (end of generated code: YI2cPort functions)
