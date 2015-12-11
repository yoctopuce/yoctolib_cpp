/*********************************************************************
 *
 * $Id: yocto_colorled.cpp 22191 2015-12-02 06:49:31Z mvuilleu $
 *
 * Implements yFindColorLed(), the high-level API for ColorLed functions
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
#include "yocto_colorled.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

YColorLed::YColorLed(const string& func): YFunction(func)
//--- (ColorLed initialization)
    ,_rgbColor(RGBCOLOR_INVALID)
    ,_hslColor(HSLCOLOR_INVALID)
    ,_rgbMove(RGBMOVE_INVALID)
    ,_hslMove(HSLMOVE_INVALID)
    ,_rgbColorAtPowerOn(RGBCOLORATPOWERON_INVALID)
    ,_blinkSeqSize(BLINKSEQSIZE_INVALID)
    ,_blinkSeqMaxSize(BLINKSEQMAXSIZE_INVALID)
    ,_blinkSeqSignature(BLINKSEQSIGNATURE_INVALID)
    ,_command(COMMAND_INVALID)
    ,_valueCallbackColorLed(NULL)
//--- (end of ColorLed initialization)
{
    _className="ColorLed";
}

YColorLed::~YColorLed()
{
//--- (YColorLed cleanup)
//--- (end of YColorLed cleanup)
}
//--- (YColorLed implementation)
// static attributes
const YMove YColorLed::RGBMOVE_INVALID = YMove();
const YMove YColorLed::HSLMOVE_INVALID = YMove();
const string YColorLed::COMMAND_INVALID = YAPI_INVALID_STRING;

int YColorLed::_parseAttr(yJsonStateMachine& j)
{
    if(!strcmp(j.token, "rgbColor")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _rgbColor =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "hslColor")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _hslColor =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "rgbMove")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        if(j.st != YJSON_PARSE_STRUCT) goto failed;
        while(yJsonParse(&j) == YJSON_PARSE_AVAIL && j.st == YJSON_PARSE_MEMBNAME) {
            if(!strcmp(j.token, "moving")) {
                if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
                _rgbMove.moving = atoi(j.token);
            } else if(!strcmp(j.token, "target")) {
                if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
                _rgbMove.target = atoi(j.token);
            } else if(!strcmp(j.token, "ms")) {
                if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
                _rgbMove.ms = atoi(j.token);
            }
        }
        if(j.st != YJSON_PARSE_STRUCT) goto failed;
        return 1;
    }
    if(!strcmp(j.token, "hslMove")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        if(j.st != YJSON_PARSE_STRUCT) goto failed;
        while(yJsonParse(&j) == YJSON_PARSE_AVAIL && j.st == YJSON_PARSE_MEMBNAME) {
            if(!strcmp(j.token, "moving")) {
                if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
                _hslMove.moving = atoi(j.token);
            } else if(!strcmp(j.token, "target")) {
                if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
                _hslMove.target = atoi(j.token);
            } else if(!strcmp(j.token, "ms")) {
                if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
                _hslMove.ms = atoi(j.token);
            }
        }
        if(j.st != YJSON_PARSE_STRUCT) goto failed;
        return 1;
    }
    if(!strcmp(j.token, "rgbColorAtPowerOn")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _rgbColorAtPowerOn =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "blinkSeqSize")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _blinkSeqSize =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "blinkSeqMaxSize")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _blinkSeqMaxSize =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "blinkSeqSignature")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _blinkSeqSignature =  atoi(j.token);
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
 * Returns the current RGB color of the led.
 *
 * @return an integer corresponding to the current RGB color of the led
 *
 * On failure, throws an exception or returns Y_RGBCOLOR_INVALID.
 */
int YColorLed::get_rgbColor(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YColorLed::RGBCOLOR_INVALID;
        }
    }
    return _rgbColor;
}

/**
 * Changes the current color of the led, using a RGB color. Encoding is done as follows: 0xRRGGBB.
 *
 * @param newval : an integer corresponding to the current color of the led, using a RGB color
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YColorLed::set_rgbColor(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf,"0x%06x",newval); rest_val = string(buf);
    return _setAttr("rgbColor", rest_val);
}

/**
 * Returns the current HSL color of the led.
 *
 * @return an integer corresponding to the current HSL color of the led
 *
 * On failure, throws an exception or returns Y_HSLCOLOR_INVALID.
 */
int YColorLed::get_hslColor(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YColorLed::HSLCOLOR_INVALID;
        }
    }
    return _hslColor;
}

/**
 * Changes the current color of the led, using a color HSL. Encoding is done as follows: 0xHHSSLL.
 *
 * @param newval : an integer corresponding to the current color of the led, using a color HSL
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YColorLed::set_hslColor(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf,"0x%06x",newval); rest_val = string(buf);
    return _setAttr("hslColor", rest_val);
}

YMove YColorLed::get_rgbMove(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YColorLed::RGBMOVE_INVALID;
        }
    }
    return _rgbMove;
}

int YColorLed::set_rgbMove(YMove newval)
{
    string rest_val;
    char buff[64]; sprintf(buff,"%d:%d",newval.target,newval.ms); rest_val = string(buff);
    return _setAttr("rgbMove", rest_val);
}

/**
 * Performs a smooth transition in the RGB color space between the current color and a target color.
 *
 * @param rgb_target  : desired RGB color at the end of the transition
 * @param ms_duration : duration of the transition, in millisecond
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YColorLed::rgbMove(int rgb_target,int ms_duration)
{
    string rest_val;
    char buff[64]; sprintf(buff,"%d:%d",rgb_target,ms_duration); rest_val = string(buff);
    return _setAttr("rgbMove", rest_val);
}

YMove YColorLed::get_hslMove(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YColorLed::HSLMOVE_INVALID;
        }
    }
    return _hslMove;
}

int YColorLed::set_hslMove(YMove newval)
{
    string rest_val;
    char buff[64]; sprintf(buff,"%d:%d",newval.target,newval.ms); rest_val = string(buff);
    return _setAttr("hslMove", rest_val);
}

/**
 * Performs a smooth transition in the HSL color space between the current color and a target color.
 *
 * @param hsl_target  : desired HSL color at the end of the transition
 * @param ms_duration : duration of the transition, in millisecond
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YColorLed::hslMove(int hsl_target,int ms_duration)
{
    string rest_val;
    char buff[64]; sprintf(buff,"%d:%d",hsl_target,ms_duration); rest_val = string(buff);
    return _setAttr("hslMove", rest_val);
}

/**
 * Returns the configured color to be displayed when the module is turned on.
 *
 * @return an integer corresponding to the configured color to be displayed when the module is turned on
 *
 * On failure, throws an exception or returns Y_RGBCOLORATPOWERON_INVALID.
 */
int YColorLed::get_rgbColorAtPowerOn(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YColorLed::RGBCOLORATPOWERON_INVALID;
        }
    }
    return _rgbColorAtPowerOn;
}

/**
 * Changes the color that the led will display by default when the module is turned on.
 *
 * @param newval : an integer corresponding to the color that the led will display by default when the
 * module is turned on
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YColorLed::set_rgbColorAtPowerOn(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf,"0x%06x",newval); rest_val = string(buf);
    return _setAttr("rgbColorAtPowerOn", rest_val);
}

/**
 * Returns the current length of the blinking sequence
 *
 * @return an integer corresponding to the current length of the blinking sequence
 *
 * On failure, throws an exception or returns Y_BLINKSEQSIZE_INVALID.
 */
int YColorLed::get_blinkSeqSize(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YColorLed::BLINKSEQSIZE_INVALID;
        }
    }
    return _blinkSeqSize;
}

/**
 * Returns the maximum length of the blinking sequence
 *
 * @return an integer corresponding to the maximum length of the blinking sequence
 *
 * On failure, throws an exception or returns Y_BLINKSEQMAXSIZE_INVALID.
 */
int YColorLed::get_blinkSeqMaxSize(void)
{
    if (_cacheExpiration == 0) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YColorLed::BLINKSEQMAXSIZE_INVALID;
        }
    }
    return _blinkSeqMaxSize;
}

/**
 * Return the blinking sequence signature. Since blinking
 * sequences cannot be read from the device, this can be used
 * to detect if a specific blinking sequence is already
 * programmed.
 *
 * @return an integer
 *
 * On failure, throws an exception or returns Y_BLINKSEQSIGNATURE_INVALID.
 */
int YColorLed::get_blinkSeqSignature(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YColorLed::BLINKSEQSIGNATURE_INVALID;
        }
    }
    return _blinkSeqSignature;
}

string YColorLed::get_command(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YColorLed::COMMAND_INVALID;
        }
    }
    return _command;
}

int YColorLed::set_command(const string& newval)
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
 * Use the method YColorLed.isOnline() to test if $THEFUNCTION$ is
 * indeed online at a given time. In case of ambiguity when looking for
 * $AFUNCTION$ by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * @param func : a string that uniquely characterizes $THEFUNCTION$
 *
 * @return a YColorLed object allowing you to drive $THEFUNCTION$.
 */
YColorLed* YColorLed::FindColorLed(string func)
{
    YColorLed* obj = NULL;
    obj = (YColorLed*) YFunction::_FindFromCache("ColorLed", func);
    if (obj == NULL) {
        obj = new YColorLed(func);
        YFunction::_AddToCache("ColorLed", func, obj);
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
int YColorLed::registerValueCallback(YColorLedValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackColorLed = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YColorLed::_invokeValueCallback(string value)
{
    if (_valueCallbackColorLed != NULL) {
        _valueCallbackColorLed(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

int YColorLed::sendCommand(string command)
{
    return this->set_command(command);
}

/**
 * Add a new transition to the blinking sequence, the move will
 * be performed in the HSL space.
 *
 * @param HSLcolor : desired HSL color when the traisntion is completed
 * @param msDelay : duration of the color transition, in milliseconds.
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *         On failure, throws an exception or returns a negative error code.
 */
int YColorLed::addHslMoveToBlinkSeq(int HSLcolor,int msDelay)
{
    return this->sendCommand(YapiWrapper::ysprintf("H%d,%d",HSLcolor,msDelay));
}

/**
 * Add a new transition to the blinking sequence, the move will
 * be performed in the RGB space.
 *
 * @param RGBcolor : desired RGB color when the transition is completed
 * @param msDelay : duration of the color transition, in milliseconds.
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *         On failure, throws an exception or returns a negative error code.
 */
int YColorLed::addRgbMoveToBlinkSeq(int RGBcolor,int msDelay)
{
    return this->sendCommand(YapiWrapper::ysprintf("R%d,%d",RGBcolor,msDelay));
}

/**
 * Starts the preprogrammed blinking sequence. The sequence will
 * run in loop until it is stopped by stopBlinkSeq or an explicit
 * change.
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *         On failure, throws an exception or returns a negative error code.
 */
int YColorLed::startBlinkSeq(void)
{
    return this->sendCommand("S");
}

/**
 * Stops the preprogrammed blinking sequence.
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *         On failure, throws an exception or returns a negative error code.
 */
int YColorLed::stopBlinkSeq(void)
{
    return this->sendCommand("X");
}

/**
 * Resets the preprogrammed blinking sequence.
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *         On failure, throws an exception or returns a negative error code.
 */
int YColorLed::resetBlinkSeq(void)
{
    return this->sendCommand("Z");
}

YColorLed *YColorLed::nextColorLed(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YColorLed::FindColorLed(hwid);
}

YColorLed* YColorLed::FirstColorLed(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("ColorLed", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YColorLed::FindColorLed(serial+"."+funcId);
}

//--- (end of YColorLed implementation)

//--- (ColorLed functions)
//--- (end of ColorLed functions)
