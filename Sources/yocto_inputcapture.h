/*********************************************************************
 *
 *  $Id: svn_id $
 *
 *  Declares yFindInputCaptureData(), the high-level API for InputCaptureData functions
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


#ifndef YOCTO_INPUTCAPTUREDATA_H
#define YOCTO_INPUTCAPTUREDATA_H

#include <cfloat>
#include <cmath>

#include "yocto_api.h"

#ifdef YOCTOLIB_NAMESPACE
namespace YOCTOLIB_NAMESPACE
{
#endif

//--- (generated code: YInputCapture definitions)
class YInputCapture; // forward declaration

typedef void (*YInputCaptureValueCallback)(YInputCapture *func, const string& functionValue);
#ifndef _Y_CAPTURETYPE_ENUM
#define _Y_CAPTURETYPE_ENUM
typedef enum {
    Y_CAPTURETYPE_NONE = 0,
    Y_CAPTURETYPE_TIMED = 1,
    Y_CAPTURETYPE_V_MAX = 2,
    Y_CAPTURETYPE_V_MIN = 3,
    Y_CAPTURETYPE_I_MAX = 4,
    Y_CAPTURETYPE_I_MIN = 5,
    Y_CAPTURETYPE_P_MAX = 6,
    Y_CAPTURETYPE_P_MIN = 7,
    Y_CAPTURETYPE_V_AVG_MAX = 8,
    Y_CAPTURETYPE_V_AVG_MIN = 9,
    Y_CAPTURETYPE_V_RMS_MAX = 10,
    Y_CAPTURETYPE_V_RMS_MIN = 11,
    Y_CAPTURETYPE_I_AVG_MAX = 12,
    Y_CAPTURETYPE_I_AVG_MIN = 13,
    Y_CAPTURETYPE_I_RMS_MAX = 14,
    Y_CAPTURETYPE_I_RMS_MIN = 15,
    Y_CAPTURETYPE_P_AVG_MAX = 16,
    Y_CAPTURETYPE_P_AVG_MIN = 17,
    Y_CAPTURETYPE_PF_MIN = 18,
    Y_CAPTURETYPE_DPF_MIN = 19,
    Y_CAPTURETYPE_INVALID = -1,
} Y_CAPTURETYPE_enum;
#endif
#ifndef _Y_CAPTURETYPEATSTARTUP_ENUM
#define _Y_CAPTURETYPEATSTARTUP_ENUM
typedef enum {
    Y_CAPTURETYPEATSTARTUP_NONE = 0,
    Y_CAPTURETYPEATSTARTUP_TIMED = 1,
    Y_CAPTURETYPEATSTARTUP_V_MAX = 2,
    Y_CAPTURETYPEATSTARTUP_V_MIN = 3,
    Y_CAPTURETYPEATSTARTUP_I_MAX = 4,
    Y_CAPTURETYPEATSTARTUP_I_MIN = 5,
    Y_CAPTURETYPEATSTARTUP_P_MAX = 6,
    Y_CAPTURETYPEATSTARTUP_P_MIN = 7,
    Y_CAPTURETYPEATSTARTUP_V_AVG_MAX = 8,
    Y_CAPTURETYPEATSTARTUP_V_AVG_MIN = 9,
    Y_CAPTURETYPEATSTARTUP_V_RMS_MAX = 10,
    Y_CAPTURETYPEATSTARTUP_V_RMS_MIN = 11,
    Y_CAPTURETYPEATSTARTUP_I_AVG_MAX = 12,
    Y_CAPTURETYPEATSTARTUP_I_AVG_MIN = 13,
    Y_CAPTURETYPEATSTARTUP_I_RMS_MAX = 14,
    Y_CAPTURETYPEATSTARTUP_I_RMS_MIN = 15,
    Y_CAPTURETYPEATSTARTUP_P_AVG_MAX = 16,
    Y_CAPTURETYPEATSTARTUP_P_AVG_MIN = 17,
    Y_CAPTURETYPEATSTARTUP_PF_MIN = 18,
    Y_CAPTURETYPEATSTARTUP_DPF_MIN = 19,
    Y_CAPTURETYPEATSTARTUP_INVALID = -1,
} Y_CAPTURETYPEATSTARTUP_enum;
#endif
#define Y_LASTCAPTURETIME_INVALID       (YAPI_INVALID_LONG)
#define Y_NSAMPLES_INVALID              (YAPI_INVALID_UINT)
#define Y_SAMPLINGRATE_INVALID          (YAPI_INVALID_UINT)
#define Y_CONDVALUE_INVALID             (YAPI_INVALID_DOUBLE)
#define Y_CONDALIGN_INVALID             (YAPI_INVALID_UINT)
#define Y_CONDVALUEATSTARTUP_INVALID    (YAPI_INVALID_DOUBLE)
//--- (end of generated code: YInputCapture definitions)

#define Y_CAPTURETYPEALL_enum Y_CAPTURETYPE_enum

//--- (generated code: YInputCaptureData return codes)
//--- (end of generated code: YInputCaptureData return codes)
//--- (generated code: YInputCaptureData yapiwrapper)
//--- (end of generated code: YInputCaptureData yapiwrapper)
//--- (generated code: YInputCaptureData definitions)
//--- (end of generated code: YInputCaptureData definitions)

//--- (generated code: YInputCaptureData declaration)
/**
 * YInputCaptureData Class: Sampled data from a Yoctopuce electrical sensor
 *
 * InputCaptureData objects represent raw data
 * sampled by the analog/digital converter present in
 * a Yoctopuce electrical sensor. When several inputs
 * are samples simultaneously, their data are provided
 * as distinct series.
 */
class YOCTO_CLASS_EXPORT YInputCaptureData {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of generated code: YInputCaptureData declaration)
protected:
    //--- (generated code: YInputCaptureData attributes)
    // Attributes (function value cache)
    int             _fmt;
    int             _var1size;
    int             _var2size;
    int             _var3size;
    int             _nVars;
    int             _recOfs;
    int             _nRecs;
    int             _samplesPerSec;
    int             _trigType;
    double          _trigVal;
    int             _trigPos;
    double          _trigUTC;
    string          _var1unit;
    string          _var2unit;
    string          _var3unit;
    vector<double>  _var1samples;
    vector<double>  _var2samples;
    vector<double>  _var3samples;
    //--- (end of generated code: YInputCaptureData attributes)

public:
    YInputCaptureData(YFunction* parent, string sdata);
    void _throw(YRETCODE errType, string errMsg);
    virtual ~YInputCaptureData();
    //--- (generated code: YInputCaptureData accessors declaration)


    virtual int         _decodeU16(string sdata,int ofs);

    virtual double      _decodeU32(string sdata,int ofs);

    virtual double      _decodeVal(string sdata,int ofs,int len);

    virtual int         _decodeSnapBin(string sdata);

    /**
     * Returns the number of series available in the capture.
     *
     * @return an integer corresponding to the number of
     *         simultaneous data series available.
     */
    virtual int         get_serieCount(void);

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
    virtual int         get_recordCount(void);

    /**
     * Returns the effective sampling rate of the device.
     *
     * @return an integer corresponding to the number of
     *         samples taken each second.
     */
    virtual int         get_samplingRate(void);

    /**
     * Returns the type of automatic conditional capture
     * that triggered the capture of this data sequence.
     *
     * @return the type of conditional capture.
     */
    virtual Y_CAPTURETYPEALL_enum get_captureType(void);

    /**
     * Returns the threshold value that triggered
     * this automatic conditional capture, if it was
     * not an instant captured triggered manually.
     *
     * @return the conditional threshold value
     *         at the time of capture.
     */
    virtual double      get_triggerValue(void);

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
    virtual int         get_triggerPosition(void);

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
    virtual double      get_triggerRealTimeUTC(void);

    /**
     * Returns the unit of measurement for data points in
     * the first serie.
     *
     * @return a string containing to a physical unit of
     *         measurement.
     */
    virtual string      get_serie1Unit(void);

    /**
     * Returns the unit of measurement for data points in
     * the second serie.
     *
     * @return a string containing to a physical unit of
     *         measurement.
     */
    virtual string      get_serie2Unit(void);

    /**
     * Returns the unit of measurement for data points in
     * the third serie.
     *
     * @return a string containing to a physical unit of
     *         measurement.
     */
    virtual string      get_serie3Unit(void);

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
    virtual vector<double> get_serie1Values(void);

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
    virtual vector<double> get_serie2Values(void);

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
    virtual vector<double> get_serie3Values(void);

#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of generated code: YInputCaptureData accessors declaration)
};

//--- (generated code: YInputCaptureData functions declaration)
//--- (end of generated code: YInputCaptureData functions declaration)


//--- (generated code: YInputCapture return codes)
//--- (end of generated code: YInputCapture return codes)
//--- (generated code: YInputCapture yapiwrapper)
//--- (end of generated code: YInputCapture yapiwrapper)

//--- (generated code: YInputCapture declaration)
/**
 * YInputCapture Class: instant snapshot trigger control interface
 *
 * The YInputCapture class allows you to access data samples
 * measured by a Yoctopuce electrical sensor. The data capture can be
 * triggered manually, or be configured to detect specific events.
 */
class YOCTO_CLASS_EXPORT YInputCapture: public YFunction {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of generated code: YInputCapture declaration)
protected:
    //--- (generated code: YInputCapture attributes)
    // Attributes (function value cache)
    s64             _lastCaptureTime;
    int             _nSamples;
    int             _samplingRate;
    Y_CAPTURETYPE_enum _captureType;
    double          _condValue;
    int             _condAlign;
    Y_CAPTURETYPEATSTARTUP_enum _captureTypeAtStartup;
    double          _condValueAtStartup;
    YInputCaptureValueCallback _valueCallbackInputCapture;

    friend YInputCapture *yFindInputCapture(const string& func);
    friend YInputCapture *yFirstInputCapture(void);

    // Function-specific method for parsing of JSON output and caching result
    virtual int     _parseAttr(YJSONObject *json_val);

    // Constructor is protected, use yFindInputCapture factory function to instantiate
    YInputCapture(const string& func);
    //--- (end of generated code: YInputCapture attributes)

public:
    virtual ~YInputCapture();
    //--- (generated code: YInputCapture accessors declaration)

    static const s64 LASTCAPTURETIME_INVALID = YAPI_INVALID_LONG;
    static const int NSAMPLES_INVALID = YAPI_INVALID_UINT;
    static const int SAMPLINGRATE_INVALID = YAPI_INVALID_UINT;
    static const Y_CAPTURETYPE_enum CAPTURETYPE_NONE = Y_CAPTURETYPE_NONE;
    static const Y_CAPTURETYPE_enum CAPTURETYPE_TIMED = Y_CAPTURETYPE_TIMED;
    static const Y_CAPTURETYPE_enum CAPTURETYPE_V_MAX = Y_CAPTURETYPE_V_MAX;
    static const Y_CAPTURETYPE_enum CAPTURETYPE_V_MIN = Y_CAPTURETYPE_V_MIN;
    static const Y_CAPTURETYPE_enum CAPTURETYPE_I_MAX = Y_CAPTURETYPE_I_MAX;
    static const Y_CAPTURETYPE_enum CAPTURETYPE_I_MIN = Y_CAPTURETYPE_I_MIN;
    static const Y_CAPTURETYPE_enum CAPTURETYPE_P_MAX = Y_CAPTURETYPE_P_MAX;
    static const Y_CAPTURETYPE_enum CAPTURETYPE_P_MIN = Y_CAPTURETYPE_P_MIN;
    static const Y_CAPTURETYPE_enum CAPTURETYPE_V_AVG_MAX = Y_CAPTURETYPE_V_AVG_MAX;
    static const Y_CAPTURETYPE_enum CAPTURETYPE_V_AVG_MIN = Y_CAPTURETYPE_V_AVG_MIN;
    static const Y_CAPTURETYPE_enum CAPTURETYPE_V_RMS_MAX = Y_CAPTURETYPE_V_RMS_MAX;
    static const Y_CAPTURETYPE_enum CAPTURETYPE_V_RMS_MIN = Y_CAPTURETYPE_V_RMS_MIN;
    static const Y_CAPTURETYPE_enum CAPTURETYPE_I_AVG_MAX = Y_CAPTURETYPE_I_AVG_MAX;
    static const Y_CAPTURETYPE_enum CAPTURETYPE_I_AVG_MIN = Y_CAPTURETYPE_I_AVG_MIN;
    static const Y_CAPTURETYPE_enum CAPTURETYPE_I_RMS_MAX = Y_CAPTURETYPE_I_RMS_MAX;
    static const Y_CAPTURETYPE_enum CAPTURETYPE_I_RMS_MIN = Y_CAPTURETYPE_I_RMS_MIN;
    static const Y_CAPTURETYPE_enum CAPTURETYPE_P_AVG_MAX = Y_CAPTURETYPE_P_AVG_MAX;
    static const Y_CAPTURETYPE_enum CAPTURETYPE_P_AVG_MIN = Y_CAPTURETYPE_P_AVG_MIN;
    static const Y_CAPTURETYPE_enum CAPTURETYPE_PF_MIN = Y_CAPTURETYPE_PF_MIN;
    static const Y_CAPTURETYPE_enum CAPTURETYPE_DPF_MIN = Y_CAPTURETYPE_DPF_MIN;
    static const Y_CAPTURETYPE_enum CAPTURETYPE_INVALID = Y_CAPTURETYPE_INVALID;
    static const double CONDVALUE_INVALID;
    static const int CONDALIGN_INVALID = YAPI_INVALID_UINT;
    static const Y_CAPTURETYPEATSTARTUP_enum CAPTURETYPEATSTARTUP_NONE = Y_CAPTURETYPEATSTARTUP_NONE;
    static const Y_CAPTURETYPEATSTARTUP_enum CAPTURETYPEATSTARTUP_TIMED = Y_CAPTURETYPEATSTARTUP_TIMED;
    static const Y_CAPTURETYPEATSTARTUP_enum CAPTURETYPEATSTARTUP_V_MAX = Y_CAPTURETYPEATSTARTUP_V_MAX;
    static const Y_CAPTURETYPEATSTARTUP_enum CAPTURETYPEATSTARTUP_V_MIN = Y_CAPTURETYPEATSTARTUP_V_MIN;
    static const Y_CAPTURETYPEATSTARTUP_enum CAPTURETYPEATSTARTUP_I_MAX = Y_CAPTURETYPEATSTARTUP_I_MAX;
    static const Y_CAPTURETYPEATSTARTUP_enum CAPTURETYPEATSTARTUP_I_MIN = Y_CAPTURETYPEATSTARTUP_I_MIN;
    static const Y_CAPTURETYPEATSTARTUP_enum CAPTURETYPEATSTARTUP_P_MAX = Y_CAPTURETYPEATSTARTUP_P_MAX;
    static const Y_CAPTURETYPEATSTARTUP_enum CAPTURETYPEATSTARTUP_P_MIN = Y_CAPTURETYPEATSTARTUP_P_MIN;
    static const Y_CAPTURETYPEATSTARTUP_enum CAPTURETYPEATSTARTUP_V_AVG_MAX = Y_CAPTURETYPEATSTARTUP_V_AVG_MAX;
    static const Y_CAPTURETYPEATSTARTUP_enum CAPTURETYPEATSTARTUP_V_AVG_MIN = Y_CAPTURETYPEATSTARTUP_V_AVG_MIN;
    static const Y_CAPTURETYPEATSTARTUP_enum CAPTURETYPEATSTARTUP_V_RMS_MAX = Y_CAPTURETYPEATSTARTUP_V_RMS_MAX;
    static const Y_CAPTURETYPEATSTARTUP_enum CAPTURETYPEATSTARTUP_V_RMS_MIN = Y_CAPTURETYPEATSTARTUP_V_RMS_MIN;
    static const Y_CAPTURETYPEATSTARTUP_enum CAPTURETYPEATSTARTUP_I_AVG_MAX = Y_CAPTURETYPEATSTARTUP_I_AVG_MAX;
    static const Y_CAPTURETYPEATSTARTUP_enum CAPTURETYPEATSTARTUP_I_AVG_MIN = Y_CAPTURETYPEATSTARTUP_I_AVG_MIN;
    static const Y_CAPTURETYPEATSTARTUP_enum CAPTURETYPEATSTARTUP_I_RMS_MAX = Y_CAPTURETYPEATSTARTUP_I_RMS_MAX;
    static const Y_CAPTURETYPEATSTARTUP_enum CAPTURETYPEATSTARTUP_I_RMS_MIN = Y_CAPTURETYPEATSTARTUP_I_RMS_MIN;
    static const Y_CAPTURETYPEATSTARTUP_enum CAPTURETYPEATSTARTUP_P_AVG_MAX = Y_CAPTURETYPEATSTARTUP_P_AVG_MAX;
    static const Y_CAPTURETYPEATSTARTUP_enum CAPTURETYPEATSTARTUP_P_AVG_MIN = Y_CAPTURETYPEATSTARTUP_P_AVG_MIN;
    static const Y_CAPTURETYPEATSTARTUP_enum CAPTURETYPEATSTARTUP_PF_MIN = Y_CAPTURETYPEATSTARTUP_PF_MIN;
    static const Y_CAPTURETYPEATSTARTUP_enum CAPTURETYPEATSTARTUP_DPF_MIN = Y_CAPTURETYPEATSTARTUP_DPF_MIN;
    static const Y_CAPTURETYPEATSTARTUP_enum CAPTURETYPEATSTARTUP_INVALID = Y_CAPTURETYPEATSTARTUP_INVALID;
    static const double CONDVALUEATSTARTUP_INVALID;

    /**
     * Returns the number of elapsed milliseconds between the module power on
     * and the last capture (time of trigger), or zero if no capture has been done.
     *
     * @return an integer corresponding to the number of elapsed milliseconds between the module power on
     *         and the last capture (time of trigger), or zero if no capture has been done
     *
     * On failure, throws an exception or returns YInputCapture::LASTCAPTURETIME_INVALID.
     */
    s64                 get_lastCaptureTime(void);

    inline s64          lastCaptureTime(void)
    { return this->get_lastCaptureTime(); }

    /**
     * Returns the number of samples that will be captured.
     *
     * @return an integer corresponding to the number of samples that will be captured
     *
     * On failure, throws an exception or returns YInputCapture::NSAMPLES_INVALID.
     */
    int                 get_nSamples(void);

    inline int          nSamples(void)
    { return this->get_nSamples(); }

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
    int             set_nSamples(int newval);
    inline int      setNSamples(int newval)
    { return this->set_nSamples(newval); }

    /**
     * Returns the sampling frequency, in Hz.
     *
     * @return an integer corresponding to the sampling frequency, in Hz
     *
     * On failure, throws an exception or returns YInputCapture::SAMPLINGRATE_INVALID.
     */
    int                 get_samplingRate(void);

    inline int          samplingRate(void)
    { return this->get_samplingRate(); }

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
    Y_CAPTURETYPE_enum  get_captureType(void);

    inline Y_CAPTURETYPE_enum captureType(void)
    { return this->get_captureType(); }

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
    int             set_captureType(Y_CAPTURETYPE_enum newval);
    inline int      setCaptureType(Y_CAPTURETYPE_enum newval)
    { return this->set_captureType(newval); }

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
    int             set_condValue(double newval);
    inline int      setCondValue(double newval)
    { return this->set_condValue(newval); }

    /**
     * Returns current threshold value for automatic conditional capture.
     *
     * @return a floating point number corresponding to current threshold value for automatic conditional capture
     *
     * On failure, throws an exception or returns YInputCapture::CONDVALUE_INVALID.
     */
    double              get_condValue(void);

    inline double       condValue(void)
    { return this->get_condValue(); }

    /**
     * Returns the relative position of the trigger event within the capture window.
     * When the value is 50%, the capture is centered on the event.
     *
     * @return an integer corresponding to the relative position of the trigger event within the capture window
     *
     * On failure, throws an exception or returns YInputCapture::CONDALIGN_INVALID.
     */
    int                 get_condAlign(void);

    inline int          condAlign(void)
    { return this->get_condAlign(); }

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
    int             set_condAlign(int newval);
    inline int      setCondAlign(int newval)
    { return this->set_condAlign(newval); }

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
    Y_CAPTURETYPEATSTARTUP_enum get_captureTypeAtStartup(void);

    inline Y_CAPTURETYPEATSTARTUP_enum captureTypeAtStartup(void)
    { return this->get_captureTypeAtStartup(); }

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
    int             set_captureTypeAtStartup(Y_CAPTURETYPEATSTARTUP_enum newval);
    inline int      setCaptureTypeAtStartup(Y_CAPTURETYPEATSTARTUP_enum newval)
    { return this->set_captureTypeAtStartup(newval); }

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
    int             set_condValueAtStartup(double newval);
    inline int      setCondValueAtStartup(double newval)
    { return this->set_condValueAtStartup(newval); }

    /**
     * Returns the threshold value for automatic conditional
     * capture applied at device power on.
     *
     * @return a floating point number corresponding to the threshold value for automatic conditional
     *         capture applied at device power on
     *
     * On failure, throws an exception or returns YInputCapture::CONDVALUEATSTARTUP_INVALID.
     */
    double              get_condValueAtStartup(void);

    inline double       condValueAtStartup(void)
    { return this->get_condValueAtStartup(); }

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
    static YInputCapture* FindInputCapture(string func);

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
    virtual int         registerValueCallback(YInputCaptureValueCallback callback);
    using YFunction::registerValueCallback;

    virtual int         _invokeValueCallback(string value);

    /**
     * Returns all details about the last automatic input capture.
     *
     * @return an YInputCaptureData object including
     *         data series and all related meta-information.
     *         On failure, throws an exception or returns an capture object.
     */
    virtual YInputCaptureData get_lastCapture(void);

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
    virtual YInputCaptureData get_immediateCapture(int msDuration);


    inline static YInputCapture *Find(string func)
    { return YInputCapture::FindInputCapture(func); }

    /**
     * comment from .yc definition
     */
           YInputCapture   *nextInputCapture(void);
    inline YInputCapture   *next(void)
    { return this->nextInputCapture();}

    /**
     * comment from .yc definition
     */
           static YInputCapture *FirstInputCapture(void);
    inline static YInputCapture *First(void)
    { return YInputCapture::FirstInputCapture();}
#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of generated code: YInputCapture accessors declaration)
};

//--- (generated code: YInputCapture functions declaration)

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
inline YInputCapture *yFindInputCapture(const string& func)
{ return YInputCapture::FindInputCapture(func);}
/**
 * comment from .yc definition
 */
inline YInputCapture *yFirstInputCapture(void)
{ return YInputCapture::FirstInputCapture();}

//--- (end of generated code: YInputCapture functions declaration)

#ifdef YOCTOLIB_NAMESPACE
// end of namespace definition
}
#endif

#endif
