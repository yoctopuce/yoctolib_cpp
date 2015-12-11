/*********************************************************************
 *
 * $Id: yocto_buzzer.cpp 22191 2015-12-02 06:49:31Z mvuilleu $
 *
 * Implements yFindBuzzer(), the high-level API for Buzzer functions
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
#include "yocto_buzzer.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

YBuzzer::YBuzzer(const string& func): YFunction(func)
//--- (Buzzer initialization)
    ,_frequency(FREQUENCY_INVALID)
    ,_volume(VOLUME_INVALID)
    ,_playSeqSize(PLAYSEQSIZE_INVALID)
    ,_playSeqMaxSize(PLAYSEQMAXSIZE_INVALID)
    ,_playSeqSignature(PLAYSEQSIGNATURE_INVALID)
    ,_command(COMMAND_INVALID)
    ,_valueCallbackBuzzer(NULL)
//--- (end of Buzzer initialization)
{
    _className="Buzzer";
}

YBuzzer::~YBuzzer()
{
//--- (YBuzzer cleanup)
//--- (end of YBuzzer cleanup)
}
//--- (YBuzzer implementation)
// static attributes
const double YBuzzer::FREQUENCY_INVALID = YAPI_INVALID_DOUBLE;
const string YBuzzer::COMMAND_INVALID = YAPI_INVALID_STRING;

int YBuzzer::_parseAttr(yJsonStateMachine& j)
{
    if(!strcmp(j.token, "frequency")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _frequency =  floor(atof(j.token) * 1000.0 / 65536.0 + 0.5) / 1000.0;
        return 1;
    }
    if(!strcmp(j.token, "volume")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _volume =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "playSeqSize")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _playSeqSize =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "playSeqMaxSize")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _playSeqMaxSize =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "playSeqSignature")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _playSeqSignature =  atoi(j.token);
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
 * Changes the frequency of the signal sent to the buzzer. A zero value stops the buzzer.
 *
 * @param newval : a floating point number corresponding to the frequency of the signal sent to the buzzer
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YBuzzer::set_frequency(double newval)
{
    string rest_val;
    char buf[32]; sprintf(buf,"%d", (int)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
    return _setAttr("frequency", rest_val);
}

/**
 * Returns the  frequency of the signal sent to the buzzer/speaker.
 *
 * @return a floating point number corresponding to the  frequency of the signal sent to the buzzer/speaker
 *
 * On failure, throws an exception or returns Y_FREQUENCY_INVALID.
 */
double YBuzzer::get_frequency(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YBuzzer::FREQUENCY_INVALID;
        }
    }
    return _frequency;
}

/**
 * Returns the volume of the signal sent to the buzzer/speaker.
 *
 * @return an integer corresponding to the volume of the signal sent to the buzzer/speaker
 *
 * On failure, throws an exception or returns Y_VOLUME_INVALID.
 */
int YBuzzer::get_volume(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YBuzzer::VOLUME_INVALID;
        }
    }
    return _volume;
}

/**
 * Changes the volume of the signal sent to the buzzer/speaker.
 *
 * @param newval : an integer corresponding to the volume of the signal sent to the buzzer/speaker
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YBuzzer::set_volume(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("volume", rest_val);
}

/**
 * Returns the current length of the playing sequence
 *
 * @return an integer corresponding to the current length of the playing sequence
 *
 * On failure, throws an exception or returns Y_PLAYSEQSIZE_INVALID.
 */
int YBuzzer::get_playSeqSize(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YBuzzer::PLAYSEQSIZE_INVALID;
        }
    }
    return _playSeqSize;
}

/**
 * Returns the maximum length of the playing sequence
 *
 * @return an integer corresponding to the maximum length of the playing sequence
 *
 * On failure, throws an exception or returns Y_PLAYSEQMAXSIZE_INVALID.
 */
int YBuzzer::get_playSeqMaxSize(void)
{
    if (_cacheExpiration == 0) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YBuzzer::PLAYSEQMAXSIZE_INVALID;
        }
    }
    return _playSeqMaxSize;
}

/**
 * Returns the playing sequence signature. As playing
 * sequences cannot be read from the device, this can be used
 * to detect if a specific playing sequence is already
 * programmed.
 *
 * @return an integer corresponding to the playing sequence signature
 *
 * On failure, throws an exception or returns Y_PLAYSEQSIGNATURE_INVALID.
 */
int YBuzzer::get_playSeqSignature(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YBuzzer::PLAYSEQSIGNATURE_INVALID;
        }
    }
    return _playSeqSignature;
}

string YBuzzer::get_command(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YBuzzer::COMMAND_INVALID;
        }
    }
    return _command;
}

int YBuzzer::set_command(const string& newval)
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
 * Use the method YBuzzer.isOnline() to test if $THEFUNCTION$ is
 * indeed online at a given time. In case of ambiguity when looking for
 * $AFUNCTION$ by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * @param func : a string that uniquely characterizes $THEFUNCTION$
 *
 * @return a YBuzzer object allowing you to drive $THEFUNCTION$.
 */
YBuzzer* YBuzzer::FindBuzzer(string func)
{
    YBuzzer* obj = NULL;
    obj = (YBuzzer*) YFunction::_FindFromCache("Buzzer", func);
    if (obj == NULL) {
        obj = new YBuzzer(func);
        YFunction::_AddToCache("Buzzer", func, obj);
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
int YBuzzer::registerValueCallback(YBuzzerValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackBuzzer = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YBuzzer::_invokeValueCallback(string value)
{
    if (_valueCallbackBuzzer != NULL) {
        _valueCallbackBuzzer(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

int YBuzzer::sendCommand(string command)
{
    return this->set_command(command);
}

/**
 * Adds a new frequency transition to the playing sequence.
 *
 * @param freq    : desired frequency when the transition is completed, in Hz
 * @param msDelay : duration of the frequency transition, in milliseconds.
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *         On failure, throws an exception or returns a negative error code.
 */
int YBuzzer::addFreqMoveToPlaySeq(int freq,int msDelay)
{
    return this->sendCommand(YapiWrapper::ysprintf("A%d,%d",freq,msDelay));
}

/**
 * Adds a pulse to the playing sequence.
 *
 * @param freq : pulse frequency, in Hz
 * @param msDuration : pulse duration, in milliseconds.
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *         On failure, throws an exception or returns a negative error code.
 */
int YBuzzer::addPulseToPlaySeq(int freq,int msDuration)
{
    return this->sendCommand(YapiWrapper::ysprintf("B%d,%d",freq,msDuration));
}

/**
 * Adds a new volume transition to the playing sequence. Frequency stays untouched:
 * if frequency is at zero, the transition has no effect.
 *
 * @param volume    : desired volume when the transition is completed, as a percentage.
 * @param msDuration : duration of the volume transition, in milliseconds.
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *         On failure, throws an exception or returns a negative error code.
 */
int YBuzzer::addVolMoveToPlaySeq(int volume,int msDuration)
{
    return this->sendCommand(YapiWrapper::ysprintf("C%d,%d",volume,msDuration));
}

/**
 * Starts the preprogrammed playing sequence. The sequence
 * runs in loop until it is stopped by stopPlaySeq or an explicit
 * change.
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *         On failure, throws an exception or returns a negative error code.
 */
int YBuzzer::startPlaySeq(void)
{
    return this->sendCommand("S");
}

/**
 * Stops the preprogrammed playing sequence and sets the frequency to zero.
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *         On failure, throws an exception or returns a negative error code.
 */
int YBuzzer::stopPlaySeq(void)
{
    return this->sendCommand("X");
}

/**
 * Resets the preprogrammed playing sequence and sets the frequency to zero.
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *         On failure, throws an exception or returns a negative error code.
 */
int YBuzzer::resetPlaySeq(void)
{
    return this->sendCommand("Z");
}

/**
 * Activates the buzzer for a short duration.
 *
 * @param frequency : pulse frequency, in hertz
 * @param duration : pulse duration in millseconds
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YBuzzer::pulse(int frequency,int duration)
{
    return this->set_command(YapiWrapper::ysprintf("P%d,%d",frequency,duration));
}

/**
 * Makes the buzzer frequency change over a period of time.
 *
 * @param frequency : frequency to reach, in hertz. A frequency under 25Hz stops the buzzer.
 * @param duration :  pulse duration in millseconds
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YBuzzer::freqMove(int frequency,int duration)
{
    return this->set_command(YapiWrapper::ysprintf("F%d,%d",frequency,duration));
}

/**
 * Makes the buzzer volume change over a period of time, frequency  stays untouched.
 *
 * @param volume : volume to reach in %
 * @param duration : change duration in millseconds
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YBuzzer::volumeMove(int volume,int duration)
{
    return this->set_command(YapiWrapper::ysprintf("V%d,%d",volume,duration));
}

YBuzzer *YBuzzer::nextBuzzer(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YBuzzer::FindBuzzer(hwid);
}

YBuzzer* YBuzzer::FirstBuzzer(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("Buzzer", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YBuzzer::FindBuzzer(serial+"."+funcId);
}

//--- (end of YBuzzer implementation)

//--- (Buzzer functions)
//--- (end of Buzzer functions)
