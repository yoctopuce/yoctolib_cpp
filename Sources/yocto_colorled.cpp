/*********************************************************************
 *
 * $Id: yocto_colorled.cpp 12324 2013-08-13 15:10:31Z mvuilleu $
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

//--- (YColorLed constructor)
// Constructor is protected, use yFindColorLed factory function to instantiate
YColorLed::YColorLed(const string& func): YFunction("ColorLed", func)
//--- (end of YColorLed constructor)
//--- (ColorLed initialization)
            ,_callback(NULL)
            ,_logicalName(Y_LOGICALNAME_INVALID)
            ,_advertisedValue(Y_ADVERTISEDVALUE_INVALID)
            ,_rgbColor(Y_RGBCOLOR_INVALID)
            ,_hslColor(Y_HSLCOLOR_INVALID)
            ,_rgbMove()
            ,_hslMove()
            ,_rgbColorAtPowerOn(Y_RGBCOLORATPOWERON_INVALID)
//--- (end of ColorLed initialization)
{}

YColorLed::~YColorLed() 
{
//--- (YColorLed cleanup)
//--- (end of YColorLed cleanup)
}
//--- (YColorLed implementation)
YMove YCOLORLED_INVALID_MOVE;

const string YColorLed::LOGICALNAME_INVALID = "!INVALID!";
const string YColorLed::ADVERTISEDVALUE_INVALID = "!INVALID!";



int YColorLed::_parse(yJsonStateMachine& j)
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
        } else if(!strcmp(j.token, "rgbColor")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _rgbColor =  atoi(j.token);
        } else if(!strcmp(j.token, "hslColor")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _hslColor =  atoi(j.token);
        } else if(!strcmp(j.token, "rgbMove")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
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
            
        } else if(!strcmp(j.token, "hslMove")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
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
            
        } else if(!strcmp(j.token, "rgbColorAtPowerOn")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _rgbColorAtPowerOn =  atoi(j.token);
        } else {
            // ignore unknown field
            yJsonSkip(&j, 1);
        }
    }
    if(j.st != YJSON_PARSE_STRUCT) goto failed;
    return 0;
}

/**
 * Returns the logical name of the RGB led.
 * 
 * @return a string corresponding to the logical name of the RGB led
 * 
 * On failure, throws an exception or returns Y_LOGICALNAME_INVALID.
 */
string YColorLed::get_logicalName(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_LOGICALNAME_INVALID;
    }
    return _logicalName;
}

/**
 * Changes the logical name of the RGB led. You can use yCheckLogicalName()
 * prior to this call to make sure that your parameter is valid.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 * 
 * @param newval : a string corresponding to the logical name of the RGB led
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YColorLed::set_logicalName(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("logicalName", rest_val);
}

/**
 * Returns the current value of the RGB led (no more than 6 characters).
 * 
 * @return a string corresponding to the current value of the RGB led (no more than 6 characters)
 * 
 * On failure, throws an exception or returns Y_ADVERTISEDVALUE_INVALID.
 */
string YColorLed::get_advertisedValue(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_ADVERTISEDVALUE_INVALID;
    }
    return _advertisedValue;
}

/**
 * Returns the current RGB color of the led.
 * 
 * @return an integer corresponding to the current RGB color of the led
 * 
 * On failure, throws an exception or returns Y_RGBCOLOR_INVALID.
 */
unsigned YColorLed::get_rgbColor(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_RGBCOLOR_INVALID;
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
int YColorLed::set_rgbColor(unsigned newval)
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
unsigned YColorLed::get_hslColor(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_HSLCOLOR_INVALID;
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
int YColorLed::set_hslColor(unsigned newval)
{
    string rest_val;
    char buf[32]; sprintf(buf,"0x%06x",newval); rest_val = string(buf);
    return _setAttr("hslColor", rest_val);
}

const YMove * YColorLed::get_rgbMove(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_RGBMOVE_INVALID;
    }
    return &_rgbMove;
}

int YColorLed::set_rgbMove(const YMove * newval)
{
    string rest_val;
    char buff[64]; sprintf(buff,"%d:%d",newval->target,newval->ms); rest_val = string(buff);
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

const YMove * YColorLed::get_hslMove(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_HSLMOVE_INVALID;
    }
    return &_hslMove;
}

int YColorLed::set_hslMove(const YMove * newval)
{
    string rest_val;
    char buff[64]; sprintf(buff,"%d:%d",newval->target,newval->ms); rest_val = string(buff);
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
unsigned YColorLed::get_rgbColorAtPowerOn(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_RGBCOLORATPOWERON_INVALID;
    }
    return _rgbColorAtPowerOn;
}

/**
 * Changes the color that the led will display by default when the module is turned on.
 * This color will be displayed as soon as the module is powered on.
 * Remember to call the saveToFlash() method of the module if the
 * change should be kept.
 * 
 * @param newval : an integer corresponding to the color that the led will display by default when the
 * module is turned on
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YColorLed::set_rgbColorAtPowerOn(unsigned newval)
{
    string rest_val;
    char buf[32]; sprintf(buf,"0x%06x",newval); rest_val = string(buf);
    return _setAttr("rgbColorAtPowerOn", rest_val);
}

YColorLed *YColorLed::nextColorLed(void)
{
    string  hwid;
    
    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return yFindColorLed(hwid);
}

void YColorLed::registerValueCallback(YColorLedUpdateCallback callback)
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

void YColorLed::advertiseValue(const string& value)
{
    if (_callback != NULL) {
        _callback(this, value);
    }
}


YColorLed* YColorLed::FindColorLed(const string& func)
{
    if(YAPI::_YFunctionsCaches["YColorLed"].find(func) != YAPI::_YFunctionsCaches["YColorLed"].end())
        return (YColorLed*) YAPI::_YFunctionsCaches["YColorLed"][func];
    
    YColorLed *newColorLed = new YColorLed(func);
    YAPI::_YFunctionsCaches["YColorLed"][func] = newColorLed ;
    return newColorLed;
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
