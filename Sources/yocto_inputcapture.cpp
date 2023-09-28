/*********************************************************************
 *
 *  $Id: svn_id $
 *
 *  Implements yFindInputCaptureData(), the high-level API for InputCaptureData functions
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

#include "yocto_inputcapture.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#define  __FILE_ID__  "inputcapturedata"

#ifdef YOCTOLIB_NAMESPACE
using namespace YOCTOLIB_NAMESPACE;
#endif

YInputCaptureData::YInputCaptureData(YFunction* parent, string sdata):
//--- (generated code: YInputCaptureData initialization)
    _fmt(0)
    ,_var1size(0)
    ,_var2size(0)
    ,_var3size(0)
    ,_nVars(0)
    ,_recOfs(0)
    ,_nRecs(0)
    ,_samplesPerSec(0)
    ,_trigType(0)
    ,_trigVal(0.0)
    ,_trigPos(0)
    ,_trigUTC(0.0)
//--- (end of generated code: YInputCaptureData initialization)
{
    this->_decodeSnapBin(sdata);
}

void YInputCaptureData::_throw(YRETCODE errType, string errMsg)
{
    // Method used to throw exceptions or save error type/message
    if (!YAPI::ExceptionsDisabled) {
        throw YAPI_Exception(errType, errMsg);
    }
}

YInputCaptureData::~YInputCaptureData()
{
//--- (generated code: YInputCaptureData cleanup)
//--- (end of generated code: YInputCaptureData cleanup)
}
//--- (generated code: YInputCaptureData implementation)
// static attributes


int YInputCaptureData::_decodeU16(string sdata,int ofs)
{
    int v = 0;
    v = ((u8)sdata[ofs]);
    v = v + 256 * ((u8)sdata[ofs+1]);
    return v;
}

double YInputCaptureData::_decodeU32(string sdata,int ofs)
{
    double v = 0.0;
    v = this->_decodeU16(sdata, ofs);
    v = v + 65536.0 * this->_decodeU16(sdata, ofs+2);
    return v;
}

double YInputCaptureData::_decodeVal(string sdata,int ofs,int len)
{
    double v = 0.0;
    double b = 0.0;
    v = this->_decodeU16(sdata, ofs);
    b = 65536.0;
    ofs = ofs + 2;
    len = len - 2;
    while (len > 0) {
        v = v + b * ((u8)sdata[ofs]);
        b = b * 256;
        ofs = ofs + 1;
        len = len - 1;
    }
    if (v > (b/2)) {
        // negative number
        v = v - b;
    }
    return v;
}

int YInputCaptureData::_decodeSnapBin(string sdata)
{
    int buffSize = 0;
    int recOfs = 0;
    int ms = 0;
    int recSize = 0;
    int count = 0;
    int mult1 = 0;
    int mult2 = 0;
    int mult3 = 0;
    double v = 0.0;

    buffSize = (int)(sdata).size();
    if (!(buffSize >= 24)) {
        _throw((YRETCODE)(YAPI_INVALID_ARGUMENT), "Invalid snapshot data (too short)");
        return YAPI_INVALID_ARGUMENT;
    }
    _fmt = ((u8)sdata[0]);
    _var1size = ((u8)sdata[1]) - 48;
    _var2size = ((u8)sdata[2]) - 48;
    _var3size = ((u8)sdata[3]) - 48;
    if (!(_fmt == 83)) {
        _throw((YRETCODE)(YAPI_INVALID_ARGUMENT), "Unsupported snapshot format");
        return YAPI_INVALID_ARGUMENT;
    }
    if (!((_var1size >= 2) && (_var1size <= 4))) {
        _throw((YRETCODE)(YAPI_INVALID_ARGUMENT), "Invalid sample size");
        return YAPI_INVALID_ARGUMENT;
    }
    if (!((_var2size >= 0) && (_var1size <= 4))) {
        _throw((YRETCODE)(YAPI_INVALID_ARGUMENT), "Invalid sample size");
        return YAPI_INVALID_ARGUMENT;
    }
    if (!((_var3size >= 0) && (_var1size <= 4))) {
        _throw((YRETCODE)(YAPI_INVALID_ARGUMENT), "Invalid sample size");
        return YAPI_INVALID_ARGUMENT;
    }
    if (_var2size == 0) {
        _nVars = 1;
    } else {
        if (_var3size == 0) {
            _nVars = 2;
        } else {
            _nVars = 3;
        }
    }
    recSize = _var1size + _var2size + _var3size;
    _recOfs = this->_decodeU16(sdata, 4);
    _nRecs = this->_decodeU16(sdata, 6);
    _samplesPerSec = this->_decodeU16(sdata, 8);
    _trigType = this->_decodeU16(sdata, 10);
    _trigVal = this->_decodeVal(sdata, 12, 4) / 1000;
    _trigPos = this->_decodeU16(sdata, 16);
    ms = this->_decodeU16(sdata, 18);
    _trigUTC = this->_decodeVal(sdata, 20, 4);
    _trigUTC = _trigUTC + (ms / 1000.0);
    recOfs = 24;
    while (((u8)sdata[recOfs]) >= 32) {
        _var1unit = YapiWrapper::ysprintf("%s%c", _var1unit.c_str(),((u8)sdata[recOfs]));
        recOfs = recOfs + 1;
    }
    if (_var2size > 0) {
        recOfs = recOfs + 1;
        while (((u8)sdata[recOfs]) >= 32) {
            _var2unit = YapiWrapper::ysprintf("%s%c", _var2unit.c_str(),((u8)sdata[recOfs]));
            recOfs = recOfs + 1;
        }
    }
    if (_var3size > 0) {
        recOfs = recOfs + 1;
        while (((u8)sdata[recOfs]) >= 32) {
            _var3unit = YapiWrapper::ysprintf("%s%c", _var3unit.c_str(),((u8)sdata[recOfs]));
            recOfs = recOfs + 1;
        }
    }
    if (((recOfs) & (1)) == 1) {
        // align to next word
        recOfs = recOfs + 1;
    }
    mult1 = 1;
    mult2 = 1;
    mult3 = 1;
    if (recOfs < _recOfs) {
        // load optional value multiplier
        mult1 = this->_decodeU16(sdata, _recOfs);
        recOfs = recOfs + 2;
        if (_var2size > 0) {
            mult2 = this->_decodeU16(sdata, _recOfs);
            recOfs = recOfs + 2;
        }
        if (_var3size > 0) {
            mult3 = this->_decodeU16(sdata, _recOfs);
            recOfs = recOfs + 2;
        }
    }
    recOfs = _recOfs;
    count = _nRecs;
    while ((count > 0) && (recOfs + _var1size <= buffSize)) {
        v = this->_decodeVal(sdata, recOfs, _var1size) / 1000.0;
        _var1samples.push_back(v*mult1);
        recOfs = recOfs + recSize;
    }
    if (_var2size > 0) {
        recOfs = _recOfs + _var1size;
        count = _nRecs;
        while ((count > 0) && (recOfs + _var2size <= buffSize)) {
            v = this->_decodeVal(sdata, recOfs, _var2size) / 1000.0;
            _var2samples.push_back(v*mult2);
            recOfs = recOfs + recSize;
        }
    }
    if (_var3size > 0) {
        recOfs = _recOfs + _var1size + _var2size;
        count = _nRecs;
        while ((count > 0) && (recOfs + _var3size <= buffSize)) {
            v = this->_decodeVal(sdata, recOfs, _var3size) / 1000.0;
            _var3samples.push_back(v*mult3);
            recOfs = recOfs + recSize;
        }
    }
    return YAPI_SUCCESS;
}

/**
 * Returns the number of series available in the capture.
 *
 * @return an integer corresponding to the number of
 *         simultaneous data series available.
 */
int YInputCaptureData::get_serieCount(void)
{
    return _nVars;
}

/**
 * Returns the number of records captured (in a serie).
 * In the exceptional case where it was not possible
 * to transfer all data in time, the number of records
 * actually present in the series might be lower than
 * the number of records captured
 *
 * @return an integer corresponding to the number of
 *         records expected in each serie.
 */
int YInputCaptureData::get_recordCount(void)
{
    return _nRecs;
}

/**
 * Returns the effective sampling rate of the device.
 *
 * @return an integer corresponding to the number of
 *         samples taken each second.
 */
int YInputCaptureData::get_samplingRate(void)
{
    return _samplesPerSec;
}

/**
 * Returns the type of automatic conditional capture
 * that triggered the capture of this data sequence.
 *
 * @return the type of conditional capture.
 */
Y_CAPTURETYPEALL_enum YInputCaptureData::get_captureType(void)
{
    return (Y_CAPTURETYPEALL_enum) _trigType;
}

/**
 * Returns the threshold value that triggered
 * this automatic conditional capture, if it was
 * not an instant captured triggered manually.
 *
 * @return the conditional threshold value
 *         at the time of capture.
 */
double YInputCaptureData::get_triggerValue(void)
{
    return _trigVal;
}

/**
 * Returns the index in the series of the sample
 * corresponding to the exact time when the capture
 * was triggered. In case of trigger based on average
 * or RMS value, the trigger index corresponds to
 * the end of the averaging period.
 *
 * @return an integer corresponding to a position
 *         in the data serie.
 */
int YInputCaptureData::get_triggerPosition(void)
{
    return _trigPos;
}

/**
 * Returns the absolute time when the capture was
 * triggered, as a Unix timestamp. Milliseconds are
 * included in this timestamp (floating-point number).
 *
 * @return a floating-point number corresponding to
 *         the number of seconds between the Jan 1,
 *         1970 and the moment where the capture
 *         was triggered.
 */
double YInputCaptureData::get_triggerRealTimeUTC(void)
{
    return _trigUTC;
}

/**
 * Returns the unit of measurement for data points in
 * the first serie.
 *
 * @return a string containing to a physical unit of
 *         measurement.
 */
string YInputCaptureData::get_serie1Unit(void)
{
    return _var1unit;
}

/**
 * Returns the unit of measurement for data points in
 * the second serie.
 *
 * @return a string containing to a physical unit of
 *         measurement.
 */
string YInputCaptureData::get_serie2Unit(void)
{
    if (!(_nVars >= 2)) {
        _throw((YRETCODE)(YAPI_INVALID_ARGUMENT), "There is no serie 2 in this capture data");
        return "";
    }
    return _var2unit;
}

/**
 * Returns the unit of measurement for data points in
 * the third serie.
 *
 * @return a string containing to a physical unit of
 *         measurement.
 */
string YInputCaptureData::get_serie3Unit(void)
{
    if (!(_nVars >= 3)) {
        _throw((YRETCODE)(YAPI_INVALID_ARGUMENT), "There is no serie 3 in this capture data");
        return "";
    }
    return _var3unit;
}

/**
 * Returns the sampled data corresponding to the first serie.
 * The corresponding physical unit can be obtained
 * using the method get_serie1Unit().
 *
 * @return a list of real numbers corresponding to all
 *         samples received for serie 1.
 *
 * On failure, throws an exception or returns an empty array.
 */
vector<double> YInputCaptureData::get_serie1Values(void)
{
    return _var1samples;
}

/**
 * Returns the sampled data corresponding to the second serie.
 * The corresponding physical unit can be obtained
 * using the method get_serie2Unit().
 *
 * @return a list of real numbers corresponding to all
 *         samples received for serie 2.
 *
 * On failure, throws an exception or returns an empty array.
 */
vector<double> YInputCaptureData::get_serie2Values(void)
{
    if (!(_nVars >= 2)) {
        _throw((YRETCODE)(YAPI_INVALID_ARGUMENT), "There is no serie 2 in this capture data");
        return _var2samples;
    }
    return _var2samples;
}

/**
 * Returns the sampled data corresponding to the third serie.
 * The corresponding physical unit can be obtained
 * using the method get_serie3Unit().
 *
 * @return a list of real numbers corresponding to all
 *         samples received for serie 3.
 *
 * On failure, throws an exception or returns an empty array.
 */
vector<double> YInputCaptureData::get_serie3Values(void)
{
    if (!(_nVars >= 3)) {
        _throw((YRETCODE)(YAPI_INVALID_ARGUMENT), "There is no serie 3 in this capture data");
        return _var3samples;
    }
    return _var3samples;
}
//--- (end of generated code: YInputCaptureData implementation)

//--- (generated code: YInputCaptureData functions)
//--- (end of generated code: YInputCaptureData functions)


YInputCapture::YInputCapture(const string& func): YFunction(func)
//--- (generated code: YInputCapture initialization)
    ,_lastCaptureTime(LASTCAPTURETIME_INVALID)
    ,_nSamples(NSAMPLES_INVALID)
    ,_samplingRate(SAMPLINGRATE_INVALID)
    ,_captureType(CAPTURETYPE_INVALID)
    ,_condValue(CONDVALUE_INVALID)
    ,_condAlign(CONDALIGN_INVALID)
    ,_captureTypeAtStartup(CAPTURETYPEATSTARTUP_INVALID)
    ,_condValueAtStartup(CONDVALUEATSTARTUP_INVALID)
    ,_valueCallbackInputCapture(NULL)
//--- (end of generated code: YInputCapture initialization)
{
    _className="InputCapture";
}

YInputCapture::~YInputCapture()
{
//--- (generated code: YInputCapture cleanup)
//--- (end of generated code: YInputCapture cleanup)
}
//--- (generated code: YInputCapture implementation)
// static attributes
const double YInputCapture::CONDVALUE_INVALID = YAPI_INVALID_DOUBLE;
const double YInputCapture::CONDVALUEATSTARTUP_INVALID = YAPI_INVALID_DOUBLE;

int YInputCapture::_parseAttr(YJSONObject *json_val)
{
    if(json_val->has("lastCaptureTime")) {
        _lastCaptureTime =  json_val->getLong("lastCaptureTime");
    }
    if(json_val->has("nSamples")) {
        _nSamples =  json_val->getInt("nSamples");
    }
    if(json_val->has("samplingRate")) {
        _samplingRate =  json_val->getInt("samplingRate");
    }
    if(json_val->has("captureType")) {
        _captureType =  (Y_CAPTURETYPE_enum)json_val->getInt("captureType");
    }
    if(json_val->has("condValue")) {
        _condValue =  floor(json_val->getDouble("condValue") / 65.536 + 0.5) / 1000.0;
    }
    if(json_val->has("condAlign")) {
        _condAlign =  json_val->getInt("condAlign");
    }
    if(json_val->has("captureTypeAtStartup")) {
        _captureTypeAtStartup =  (Y_CAPTURETYPEATSTARTUP_enum)json_val->getInt("captureTypeAtStartup");
    }
    if(json_val->has("condValueAtStartup")) {
        _condValueAtStartup =  floor(json_val->getDouble("condValueAtStartup") / 65.536 + 0.5) / 1000.0;
    }
    return YFunction::_parseAttr(json_val);
}


/**
 * Returns the number of elapsed milliseconds between the module power on
 * and the last capture (time of trigger), or zero if no capture has been done.
 *
 * @return an integer corresponding to the number of elapsed milliseconds between the module power on
 *         and the last capture (time of trigger), or zero if no capture has been done
 *
 * On failure, throws an exception or returns YInputCapture::LASTCAPTURETIME_INVALID.
 */
s64 YInputCapture::get_lastCaptureTime(void)
{
    s64 res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YInputCapture::LASTCAPTURETIME_INVALID;
                }
            }
        }
        res = _lastCaptureTime;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the number of samples that will be captured.
 *
 * @return an integer corresponding to the number of samples that will be captured
 *
 * On failure, throws an exception or returns YInputCapture::NSAMPLES_INVALID.
 */
int YInputCapture::get_nSamples(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YInputCapture::NSAMPLES_INVALID;
                }
            }
        }
        res = _nSamples;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the type of automatic conditional capture.
 * The maximum number of samples depends on the device memory.
 *
 * If you want the change to be kept after a device reboot,
 * make sure  to call the matching module saveToFlash().
 *
 * @param newval : an integer corresponding to the type of automatic conditional capture
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YInputCapture::set_nSamples(int newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("nSamples", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the sampling frequency, in Hz.
 *
 * @return an integer corresponding to the sampling frequency, in Hz
 *
 * On failure, throws an exception or returns YInputCapture::SAMPLINGRATE_INVALID.
 */
int YInputCapture::get_samplingRate(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YInputCapture::SAMPLINGRATE_INVALID;
                }
            }
        }
        res = _samplingRate;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the type of automatic conditional capture.
 *
 * @return a value among YInputCapture::CAPTURETYPE_NONE, YInputCapture::CAPTURETYPE_TIMED,
 * YInputCapture::CAPTURETYPE_V_MAX, YInputCapture::CAPTURETYPE_V_MIN, YInputCapture::CAPTURETYPE_I_MAX,
 * YInputCapture::CAPTURETYPE_I_MIN, YInputCapture::CAPTURETYPE_P_MAX, YInputCapture::CAPTURETYPE_P_MIN,
 * YInputCapture::CAPTURETYPE_V_AVG_MAX, YInputCapture::CAPTURETYPE_V_AVG_MIN,
 * YInputCapture::CAPTURETYPE_V_RMS_MAX, YInputCapture::CAPTURETYPE_V_RMS_MIN,
 * YInputCapture::CAPTURETYPE_I_AVG_MAX, YInputCapture::CAPTURETYPE_I_AVG_MIN,
 * YInputCapture::CAPTURETYPE_I_RMS_MAX, YInputCapture::CAPTURETYPE_I_RMS_MIN,
 * YInputCapture::CAPTURETYPE_P_AVG_MAX, YInputCapture::CAPTURETYPE_P_AVG_MIN,
 * YInputCapture::CAPTURETYPE_PF_MIN and YInputCapture::CAPTURETYPE_DPF_MIN corresponding to the type of
 * automatic conditional capture
 *
 * On failure, throws an exception or returns YInputCapture::CAPTURETYPE_INVALID.
 */
Y_CAPTURETYPE_enum YInputCapture::get_captureType(void)
{
    Y_CAPTURETYPE_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YInputCapture::CAPTURETYPE_INVALID;
                }
            }
        }
        res = _captureType;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the type of automatic conditional capture.
 *
 * @param newval : a value among YInputCapture::CAPTURETYPE_NONE, YInputCapture::CAPTURETYPE_TIMED,
 * YInputCapture::CAPTURETYPE_V_MAX, YInputCapture::CAPTURETYPE_V_MIN, YInputCapture::CAPTURETYPE_I_MAX,
 * YInputCapture::CAPTURETYPE_I_MIN, YInputCapture::CAPTURETYPE_P_MAX, YInputCapture::CAPTURETYPE_P_MIN,
 * YInputCapture::CAPTURETYPE_V_AVG_MAX, YInputCapture::CAPTURETYPE_V_AVG_MIN,
 * YInputCapture::CAPTURETYPE_V_RMS_MAX, YInputCapture::CAPTURETYPE_V_RMS_MIN,
 * YInputCapture::CAPTURETYPE_I_AVG_MAX, YInputCapture::CAPTURETYPE_I_AVG_MIN,
 * YInputCapture::CAPTURETYPE_I_RMS_MAX, YInputCapture::CAPTURETYPE_I_RMS_MIN,
 * YInputCapture::CAPTURETYPE_P_AVG_MAX, YInputCapture::CAPTURETYPE_P_AVG_MIN,
 * YInputCapture::CAPTURETYPE_PF_MIN and YInputCapture::CAPTURETYPE_DPF_MIN corresponding to the type of
 * automatic conditional capture
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YInputCapture::set_captureType(Y_CAPTURETYPE_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("captureType", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes current threshold value for automatic conditional capture.
 *
 * @param newval : a floating point number corresponding to current threshold value for automatic
 * conditional capture
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YInputCapture::set_condValue(double newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%" FMTs64, (s64)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
        res = _setAttr("condValue", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns current threshold value for automatic conditional capture.
 *
 * @return a floating point number corresponding to current threshold value for automatic conditional capture
 *
 * On failure, throws an exception or returns YInputCapture::CONDVALUE_INVALID.
 */
double YInputCapture::get_condValue(void)
{
    double res = 0.0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YInputCapture::CONDVALUE_INVALID;
                }
            }
        }
        res = _condValue;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the relative position of the trigger event within the capture window.
 * When the value is 50%, the capture is centered on the event.
 *
 * @return an integer corresponding to the relative position of the trigger event within the capture window
 *
 * On failure, throws an exception or returns YInputCapture::CONDALIGN_INVALID.
 */
int YInputCapture::get_condAlign(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YInputCapture::CONDALIGN_INVALID;
                }
            }
        }
        res = _condAlign;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the relative position of the trigger event within the capture window.
 * The new value must be between 10% (on the left) and 90% (on the right).
 * When the value is 50%, the capture is centered on the event.
 *
 * If you want the change to be kept after a device reboot,
 * make sure  to call the matching module saveToFlash().
 *
 * @param newval : an integer corresponding to the relative position of the trigger event within the capture window
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YInputCapture::set_condAlign(int newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("condAlign", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the type of automatic conditional capture
 * applied at device power on.
 *
 * @return a value among YInputCapture::CAPTURETYPEATSTARTUP_NONE,
 * YInputCapture::CAPTURETYPEATSTARTUP_TIMED, YInputCapture::CAPTURETYPEATSTARTUP_V_MAX,
 * YInputCapture::CAPTURETYPEATSTARTUP_V_MIN, YInputCapture::CAPTURETYPEATSTARTUP_I_MAX,
 * YInputCapture::CAPTURETYPEATSTARTUP_I_MIN, YInputCapture::CAPTURETYPEATSTARTUP_P_MAX,
 * YInputCapture::CAPTURETYPEATSTARTUP_P_MIN, YInputCapture::CAPTURETYPEATSTARTUP_V_AVG_MAX,
 * YInputCapture::CAPTURETYPEATSTARTUP_V_AVG_MIN, YInputCapture::CAPTURETYPEATSTARTUP_V_RMS_MAX,
 * YInputCapture::CAPTURETYPEATSTARTUP_V_RMS_MIN, YInputCapture::CAPTURETYPEATSTARTUP_I_AVG_MAX,
 * YInputCapture::CAPTURETYPEATSTARTUP_I_AVG_MIN, YInputCapture::CAPTURETYPEATSTARTUP_I_RMS_MAX,
 * YInputCapture::CAPTURETYPEATSTARTUP_I_RMS_MIN, YInputCapture::CAPTURETYPEATSTARTUP_P_AVG_MAX,
 * YInputCapture::CAPTURETYPEATSTARTUP_P_AVG_MIN, YInputCapture::CAPTURETYPEATSTARTUP_PF_MIN and
 * YInputCapture::CAPTURETYPEATSTARTUP_DPF_MIN corresponding to the type of automatic conditional capture
 *         applied at device power on
 *
 * On failure, throws an exception or returns YInputCapture::CAPTURETYPEATSTARTUP_INVALID.
 */
Y_CAPTURETYPEATSTARTUP_enum YInputCapture::get_captureTypeAtStartup(void)
{
    Y_CAPTURETYPEATSTARTUP_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YInputCapture::CAPTURETYPEATSTARTUP_INVALID;
                }
            }
        }
        res = _captureTypeAtStartup;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the type of automatic conditional capture
 * applied at device power on.
 *
 * If you want the change to be kept after a device reboot,
 * make sure  to call the matching module saveToFlash().
 *
 * @param newval : a value among YInputCapture::CAPTURETYPEATSTARTUP_NONE,
 * YInputCapture::CAPTURETYPEATSTARTUP_TIMED, YInputCapture::CAPTURETYPEATSTARTUP_V_MAX,
 * YInputCapture::CAPTURETYPEATSTARTUP_V_MIN, YInputCapture::CAPTURETYPEATSTARTUP_I_MAX,
 * YInputCapture::CAPTURETYPEATSTARTUP_I_MIN, YInputCapture::CAPTURETYPEATSTARTUP_P_MAX,
 * YInputCapture::CAPTURETYPEATSTARTUP_P_MIN, YInputCapture::CAPTURETYPEATSTARTUP_V_AVG_MAX,
 * YInputCapture::CAPTURETYPEATSTARTUP_V_AVG_MIN, YInputCapture::CAPTURETYPEATSTARTUP_V_RMS_MAX,
 * YInputCapture::CAPTURETYPEATSTARTUP_V_RMS_MIN, YInputCapture::CAPTURETYPEATSTARTUP_I_AVG_MAX,
 * YInputCapture::CAPTURETYPEATSTARTUP_I_AVG_MIN, YInputCapture::CAPTURETYPEATSTARTUP_I_RMS_MAX,
 * YInputCapture::CAPTURETYPEATSTARTUP_I_RMS_MIN, YInputCapture::CAPTURETYPEATSTARTUP_P_AVG_MAX,
 * YInputCapture::CAPTURETYPEATSTARTUP_P_AVG_MIN, YInputCapture::CAPTURETYPEATSTARTUP_PF_MIN and
 * YInputCapture::CAPTURETYPEATSTARTUP_DPF_MIN corresponding to the type of automatic conditional capture
 *         applied at device power on
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YInputCapture::set_captureTypeAtStartup(Y_CAPTURETYPEATSTARTUP_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("captureTypeAtStartup", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes current threshold value for automatic conditional
 * capture applied at device power on.
 *
 * If you want the change to be kept after a device reboot,
 * make sure  to call the matching module saveToFlash().
 *
 * @param newval : a floating point number corresponding to current threshold value for automatic conditional
 *         capture applied at device power on
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YInputCapture::set_condValueAtStartup(double newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%" FMTs64, (s64)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
        res = _setAttr("condValueAtStartup", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the threshold value for automatic conditional
 * capture applied at device power on.
 *
 * @return a floating point number corresponding to the threshold value for automatic conditional
 *         capture applied at device power on
 *
 * On failure, throws an exception or returns YInputCapture::CONDVALUEATSTARTUP_INVALID.
 */
double YInputCapture::get_condValueAtStartup(void)
{
    double res = 0.0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YInputCapture::CONDVALUEATSTARTUP_INVALID;
                }
            }
        }
        res = _condValueAtStartup;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Retrieves an instant snapshot trigger for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that the instant snapshot trigger is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the instant snapshot trigger is
 * indeed online at a given time. In case of ambiguity when looking for
 * an instant snapshot trigger by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the instant snapshot trigger, for instance
 *         MyDevice.inputCapture.
 *
 * @return a YInputCapture object allowing you to drive the instant snapshot trigger.
 */
YInputCapture* YInputCapture::FindInputCapture(string func)
{
    YInputCapture* obj = NULL;
    int taken = 0;
    if (YAPI::_apiInitialized) {
        yEnterCriticalSection(&YAPI::_global_cs);
        taken = 1;
    }try {
        obj = (YInputCapture*) YFunction::_FindFromCache("InputCapture", func);
        if (obj == NULL) {
            obj = new YInputCapture(func);
            YFunction::_AddToCache("InputCapture", func, obj);
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
int YInputCapture::registerValueCallback(YInputCaptureValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackInputCapture = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YInputCapture::_invokeValueCallback(string value)
{
    if (_valueCallbackInputCapture != NULL) {
        _valueCallbackInputCapture(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

/**
 * Returns all details about the last automatic input capture.
 *
 * @return an YInputCaptureData object including
 *         data series and all related meta-information.
 *         On failure, throws an exception or returns an capture object.
 */
YInputCaptureData YInputCapture::get_lastCapture(void)
{
    string snapData;

    snapData = this->_download("snap.bin");
    return YInputCaptureData(this,snapData);
}

/**
 * Returns a new immediate capture of the device inputs.
 *
 * @param msDuration : duration of the capture window,
 *         in milliseconds (eg. between 20 and 1000).
 *
 * @return an YInputCaptureData object including
 *         data series for the specified duration.
 *         On failure, throws an exception or returns an capture object.
 */
YInputCaptureData YInputCapture::get_immediateCapture(int msDuration)
{
    string snapUrl;
    string snapData;
    int snapStart = 0;
    if (msDuration < 1) {
        msDuration = 20;
    }
    if (msDuration > 1000) {
        msDuration = 1000;
    }
    snapStart = ((-msDuration) / (2));
    snapUrl = YapiWrapper::ysprintf("snap.bin?t=%d&d=%d", snapStart,msDuration);

    snapData = this->_download(snapUrl);
    return YInputCaptureData(this,snapData);
}

YInputCapture *YInputCapture::nextInputCapture(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YInputCapture::FindInputCapture(hwid);
}

YInputCapture *YInputCapture::FirstInputCapture(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("InputCapture", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YInputCapture::FindInputCapture(serial+"."+funcId);
}

//--- (end of generated code: YInputCapture implementation)

//--- (generated code: YInputCapture functions)
//--- (end of generated code: YInputCapture functions)
