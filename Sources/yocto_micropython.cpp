/*********************************************************************
 *
 *  $Id: svn_id $
 *
 *  Implements yFindMicroPython(), the high-level API for MicroPython functions
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

#include "yocto_micropython.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#define  __FILE_ID__  "micropython"

#ifdef YOCTOLIB_NAMESPACE
using namespace YOCTOLIB_NAMESPACE;
#endif

YMicroPython::YMicroPython(const string& func): YFunction(func)
//--- (generated code: YMicroPython initialization)
    ,_lastMsg(LASTMSG_INVALID)
    ,_heapUsage(HEAPUSAGE_INVALID)
    ,_heapFrag(HEAPFRAG_INVALID)
    ,_xheapUsage(XHEAPUSAGE_INVALID)
    ,_stackUsage(STACKUSAGE_INVALID)
    ,_currentScript(CURRENTSCRIPT_INVALID)
    ,_startupScript(STARTUPSCRIPT_INVALID)
    ,_startupDelay(STARTUPDELAY_INVALID)
    ,_debugMode(DEBUGMODE_INVALID)
    ,_command(COMMAND_INVALID)
    ,_valueCallbackMicroPython(NULL)
    ,_logCallback(NULL)
    ,_isFirstCb(0)
    ,_prevCbPos(0)
    ,_logPos(0)
//--- (end of generated code: YMicroPython initialization)
{
    _className="MicroPython";
}

YMicroPython::~YMicroPython()
{
//--- (generated code: YMicroPython cleanup)
//--- (end of generated code: YMicroPython cleanup)
}
//--- (generated code: YMicroPython implementation)
void YMicroPython::yInternalEventCallback(YMicroPython *obj, const string& value)
{
    obj->_internalEventHandler(value);
}

// static attributes
const string YMicroPython::LASTMSG_INVALID = YAPI_INVALID_STRING;
const string YMicroPython::CURRENTSCRIPT_INVALID = YAPI_INVALID_STRING;
const string YMicroPython::STARTUPSCRIPT_INVALID = YAPI_INVALID_STRING;
const double YMicroPython::STARTUPDELAY_INVALID = YAPI_INVALID_DOUBLE;
const string YMicroPython::COMMAND_INVALID = YAPI_INVALID_STRING;

int YMicroPython::_parseAttr(YJSONObject *json_val)
{
    if(json_val->has("lastMsg")) {
        _lastMsg =  json_val->getString("lastMsg");
    }
    if(json_val->has("heapUsage")) {
        _heapUsage =  json_val->getInt("heapUsage");
    }
    if(json_val->has("heapFrag")) {
        _heapFrag =  json_val->getInt("heapFrag");
    }
    if(json_val->has("xheapUsage")) {
        _xheapUsage =  json_val->getInt("xheapUsage");
    }
    if(json_val->has("stackUsage")) {
        _stackUsage =  json_val->getInt("stackUsage");
    }
    if(json_val->has("currentScript")) {
        _currentScript =  json_val->getString("currentScript");
    }
    if(json_val->has("startupScript")) {
        _startupScript =  json_val->getString("startupScript");
    }
    if(json_val->has("startupDelay")) {
        _startupDelay =  floor(json_val->getDouble("startupDelay") / 65.536 + 0.5) / 1000.0;
    }
    if(json_val->has("debugMode")) {
        _debugMode =  (Y_DEBUGMODE_enum)json_val->getInt("debugMode");
    }
    if(json_val->has("command")) {
        _command =  json_val->getString("command");
    }
    return YFunction::_parseAttr(json_val);
}


/**
 * Returns the last message produced by a python script.
 *
 * @return a string corresponding to the last message produced by a python script
 *
 * On failure, throws an exception or returns YMicroPython::LASTMSG_INVALID.
 */
string YMicroPython::get_lastMsg(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YMicroPython::LASTMSG_INVALID;
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
 * Returns the percentage of MicroPython main memory in use,
 * as observed at the end of the last garbage collection.
 *
 * @return an integer corresponding to the percentage of MicroPython main memory in use,
 *         as observed at the end of the last garbage collection
 *
 * On failure, throws an exception or returns YMicroPython::HEAPUSAGE_INVALID.
 */
int YMicroPython::get_heapUsage(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YMicroPython::HEAPUSAGE_INVALID;
                }
            }
        }
        res = _heapUsage;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the fragmentation ratio of MicroPython main memory,
 * as observed at the end of the last garbage collection.
 *
 * @return an integer corresponding to the fragmentation ratio of MicroPython main memory,
 *         as observed at the end of the last garbage collection
 *
 * On failure, throws an exception or returns YMicroPython::HEAPFRAG_INVALID.
 */
int YMicroPython::get_heapFrag(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YMicroPython::HEAPFRAG_INVALID;
                }
            }
        }
        res = _heapFrag;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the percentage of MicroPython external memory in use,
 * as observed at the end of the last garbage collection.
 *
 * @return an integer corresponding to the percentage of MicroPython external memory in use,
 *         as observed at the end of the last garbage collection
 *
 * On failure, throws an exception or returns YMicroPython::XHEAPUSAGE_INVALID.
 */
int YMicroPython::get_xheapUsage(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YMicroPython::XHEAPUSAGE_INVALID;
                }
            }
        }
        res = _xheapUsage;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the maximum percentage of MicroPython call stack in use,
 * as observed at the end of the last garbage collection.
 *
 * @return an integer corresponding to the maximum percentage of MicroPython call stack in use,
 *         as observed at the end of the last garbage collection
 *
 * On failure, throws an exception or returns YMicroPython::STACKUSAGE_INVALID.
 */
int YMicroPython::get_stackUsage(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YMicroPython::STACKUSAGE_INVALID;
                }
            }
        }
        res = _stackUsage;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the name of currently active script, if any.
 *
 * @return a string corresponding to the name of currently active script, if any
 *
 * On failure, throws an exception or returns YMicroPython::CURRENTSCRIPT_INVALID.
 */
string YMicroPython::get_currentScript(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YMicroPython::CURRENTSCRIPT_INVALID;
                }
            }
        }
        res = _currentScript;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Stops current running script, and/or selects a script to run immediately in a
 * fresh new environment. If the MicroPython interpreter is busy running a script,
 * this function will abort it immediately and reset the execution environment.
 * If a non-empty string is given as argument, the new script will be started.
 *
 * @param newval : a string
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YMicroPython::set_currentScript(const string& newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = newval;
        res = _setAttr("currentScript", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the name of the script to run when the device is powered on.
 *
 * @return a string corresponding to the name of the script to run when the device is powered on
 *
 * On failure, throws an exception or returns YMicroPython::STARTUPSCRIPT_INVALID.
 */
string YMicroPython::get_startupScript(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YMicroPython::STARTUPSCRIPT_INVALID;
                }
            }
        }
        res = _startupScript;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the script to run when the device is powered on.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : a string corresponding to the script to run when the device is powered on
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YMicroPython::set_startupScript(const string& newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = newval;
        res = _setAttr("startupScript", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the wait time before running the startup script on power on, between 0.1
 * second and 25 seconds. Remember to call the saveToFlash() method of the
 * module if the modification must be kept.
 *
 * @param newval : a floating point number corresponding to the wait time before running the startup
 * script on power on, between 0.1
 *         second and 25 seconds
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YMicroPython::set_startupDelay(double newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%" FMTs64, (s64)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
        res = _setAttr("startupDelay", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the wait time before running the startup script on power on,
 * between 0.1 second and 25 seconds.
 *
 * @return a floating point number corresponding to the wait time before running the startup script on power on,
 *         between 0.1 second and 25 seconds
 *
 * On failure, throws an exception or returns YMicroPython::STARTUPDELAY_INVALID.
 */
double YMicroPython::get_startupDelay(void)
{
    double res = 0.0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YMicroPython::STARTUPDELAY_INVALID;
                }
            }
        }
        res = _startupDelay;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the activation state of MicroPython debugging interface.
 *
 * @return either YMicroPython::DEBUGMODE_OFF or YMicroPython::DEBUGMODE_ON, according to the activation
 * state of MicroPython debugging interface
 *
 * On failure, throws an exception or returns YMicroPython::DEBUGMODE_INVALID.
 */
Y_DEBUGMODE_enum YMicroPython::get_debugMode(void)
{
    Y_DEBUGMODE_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YMicroPython::DEBUGMODE_INVALID;
                }
            }
        }
        res = _debugMode;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the activation state of MicroPython debugging interface.
 *
 * @param newval : either YMicroPython::DEBUGMODE_OFF or YMicroPython::DEBUGMODE_ON, according to the
 * activation state of MicroPython debugging interface
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YMicroPython::set_debugMode(Y_DEBUGMODE_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = (newval>0 ? "1" : "0");
        res = _setAttr("debugMode", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

string YMicroPython::get_command(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YMicroPython::COMMAND_INVALID;
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

int YMicroPython::set_command(const string& newval)
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
 * Retrieves a MicroPython interpreter for a given identifier.
 * The identifier can be specified using several formats:
 *
 * - FunctionLogicalName
 * - ModuleSerialNumber.FunctionIdentifier
 * - ModuleSerialNumber.FunctionLogicalName
 * - ModuleLogicalName.FunctionIdentifier
 * - ModuleLogicalName.FunctionLogicalName
 *
 *
 * This function does not require that the MicroPython interpreter is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the MicroPython interpreter is
 * indeed online at a given time. In case of ambiguity when looking for
 * a MicroPython interpreter by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the MicroPython interpreter, for instance
 *         MyDevice.microPython.
 *
 * @return a YMicroPython object allowing you to drive the MicroPython interpreter.
 */
YMicroPython* YMicroPython::FindMicroPython(string func)
{
    YMicroPython* obj = NULL;
    int taken = 0;
    if (YAPI::_apiInitialized) {
        yEnterCriticalSection(&YAPI::_global_cs);
        taken = 1;
    }try {
        obj = (YMicroPython*) YFunction::_FindFromCache("MicroPython", func);
        if (obj == NULL) {
            obj = new YMicroPython(func);
            YFunction::_AddToCache("MicroPython", func, obj);
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
int YMicroPython::registerValueCallback(YMicroPythonValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackMicroPython = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YMicroPython::_invokeValueCallback(string value)
{
    if (_valueCallbackMicroPython != NULL) {
        _valueCallbackMicroPython(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

/**
 * Submit MicroPython code for execution in the interpreter.
 * If the MicroPython interpreter is busy, this function will
 * block until it becomes available. The code is then uploaded,
 * compiled and executed on the fly, without beeing stored on the device filesystem.
 *
 * There is no implicit reset of the MicroPython interpreter with
 * this function. Use method reset() if you need to start
 * from a fresh environment to run your code.
 *
 * Note that although MicroPython is mostly compatible with recent Python 3.x
 * interpreters, the limited ressources on the device impose some restrictions,
 * in particular regarding the libraries that can be used. Please refer to
 * the documentation for more details.
 *
 * @param codeName : name of the code file (used for error reporting only)
 * @param mpyCode : MicroPython code to compile and execute
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YMicroPython::eval(string codeName,string mpyCode)
{
    string fullname;
    int res = 0;
    fullname = YapiWrapper::ysprintf("mpy:%s",codeName.c_str());
    res = this->_upload(fullname, mpyCode);
    return res;
}

/**
 * Stops current execution, and reset the MicroPython interpreter to initial state.
 * All global variables are cleared, and all imports are forgotten.
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YMicroPython::reset(void)
{
    int res = 0;
    string state;

    res = this->set_command("Z");
    if (!(res == YAPI_SUCCESS)) {
        _throw((YRETCODE)(YAPI_IO_ERROR), "unable to trigger MicroPython reset");
        return YAPI_IO_ERROR;
    }
    // Wait until the reset is effective
    state = (this->get_advertisedValue()).substr(0, 1);
    while (!(state == "z")) {
        {string ignore_error; YAPI::Sleep(50, ignore_error);};
        state = (this->get_advertisedValue()).substr(0, 1);
    }
    return YAPI_SUCCESS;
}

/**
 * Clears MicroPython interpreter console log buffer.
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YMicroPython::clearLogs(void)
{
    int res = 0;

    res = this->set_command("z");
    return res;
}

/**
 * Returns a string with last logs of the MicroPython interpreter.
 * This method return only logs that are still in the module.
 *
 * @return a string with last MicroPython logs.
 *         On failure, throws an exception or returns  YAPI::INVALID_STRING.
 */
string YMicroPython::get_lastLogs(void)
{
    string buff;
    int bufflen = 0;
    string res;

    buff = this->_download("mpy.txt");
    bufflen = (int)(buff).size() - 1;
    while ((bufflen > 0) && (((u8)buff[bufflen]) != 64)) {
        bufflen = bufflen - 1;
    }
    res = buff.substr(0, bufflen);
    return res;
}

/**
 * Registers a device log callback function. This callback will be called each time
 * microPython sends a new log message.
 *
 * @param callback : the callback function to invoke, or a NULL pointer.
 *         The callback function should take two arguments:
 *         the module object that emitted the log message,
 *         and the character string containing the log.
 *         On failure, throws an exception or returns a negative error code.
 */
int YMicroPython::registerLogCallback(YMicroPythonLogCallback callback)
{
    string serial;

    serial = this->get_serialNumber();
    if (serial == YAPI_INVALID_STRING) {
        return YAPI_DEVICE_NOT_FOUND;
    }
    _logCallback = callback;
    _isFirstCb = true;
    if (callback != NULL) {
        this->registerValueCallback(yInternalEventCallback);
    } else {
        this->registerValueCallback((YMicroPythonValueCallback) NULL);
    }
    return 0;
}

YMicroPythonLogCallback YMicroPython::get_logCallback(void)
{
    return _logCallback;
}

int YMicroPython::_internalEventHandler(string cbVal)
{
    int cbPos = 0;
    int cbDPos = 0;
    string url;
    string content;
    int endPos = 0;
    string contentStr;
    vector<string> msgArr;
    int arrLen = 0;
    string lenStr;
    int arrPos = 0;
    string logMsg;
    // detect possible power cycle of the reader to clear event pointer
    cbPos = (int)YAPI::_hexStr2Long(cbVal.substr(1, (int)(cbVal).length()-1));
    cbDPos = ((cbPos - _prevCbPos) & 0xfffff);
    _prevCbPos = cbPos;
    if (cbDPos > 65536) {
        _logPos = 0;
    }
    if (!(_logCallback != NULL)) {
        return YAPI_SUCCESS;
    }
    if (_isFirstCb) {
        // use first emulated value callback caused by registerValueCallback:
        // to retrieve current logs position
        _logPos = 0;
        _prevPartialLog = "";
        url = "mpy.txt";
    } else {
        // load all messages since previous call
        url = YapiWrapper::ysprintf("mpy.txt?pos=%d",_logPos);
    }

    content = this->_download(url);
    contentStr = content;
    // look for new position indicator at end of logs
    endPos = (int)(content).size() - 1;
    while ((endPos >= 0) && (((u8)content[endPos]) != 64)) {
        endPos = endPos - 1;
    }
    if (!(endPos > 0)) {
        _throw((YRETCODE)(YAPI_IO_ERROR), "fail to download micropython logs");
        return YAPI_IO_ERROR;
    }
    lenStr = contentStr.substr(endPos+1, (int)(contentStr).length()-(endPos+1));
    // update processed event position pointer
    _logPos = atoi((lenStr).c_str());
    if (_isFirstCb) {
        // don't generate callbacks log messages before call to registerLogCallback
        _isFirstCb = false;
        return YAPI_SUCCESS;
    }
    // now generate callbacks for each complete log line
    endPos = endPos - 1;
    if (!(((u8)content[endPos]) == 10)) {
        _throw((YRETCODE)(YAPI_IO_ERROR), "fail to download micropython logs");
        return YAPI_IO_ERROR;
    }
    contentStr = contentStr.substr(0, endPos);
    msgArr = _strsplit(contentStr,'\n');
    arrLen = (int)msgArr.size() - 1;
    if (arrLen > 0) {
        logMsg = YapiWrapper::ysprintf("%s%s",_prevPartialLog.c_str(),msgArr[0].c_str());
        if (_logCallback != NULL) {
            _logCallback(this, logMsg);
        }
        _prevPartialLog = "";
        arrPos = 1;
        while (arrPos < arrLen) {
            logMsg = msgArr[arrPos];
            if (_logCallback != NULL) {
                _logCallback(this, logMsg);
            }
            arrPos = arrPos + 1;
        }
    }
    _prevPartialLog = YapiWrapper::ysprintf("%s%s",_prevPartialLog.c_str(),msgArr[arrLen].c_str());
    return YAPI_SUCCESS;
}

YMicroPython *YMicroPython::nextMicroPython(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YMicroPython::FindMicroPython(hwid);
}

YMicroPython *YMicroPython::FirstMicroPython(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("MicroPython", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YMicroPython::FindMicroPython(serial+"."+funcId);
}

//--- (end of generated code: YMicroPython implementation)

//--- (generated code: YMicroPython functions)
//--- (end of generated code: YMicroPython functions)
