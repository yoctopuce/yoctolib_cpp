/*********************************************************************
 *
 *  $Id: yocto_weighscale.h 44049 2021-02-26 10:57:40Z web $
 *
 *  Declares yFindWeighScale(), the high-level API for WeighScale functions
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


#ifndef YOCTO_WEIGHSCALE_H
#define YOCTO_WEIGHSCALE_H

#include <cfloat>
#include <cmath>

#include "yocto_api.h"

#ifdef YOCTOLIB_NAMESPACE
namespace YOCTOLIB_NAMESPACE
{
#endif

//--- (YWeighScale return codes)
//--- (end of YWeighScale return codes)
//--- (YWeighScale yapiwrapper)
//--- (end of YWeighScale yapiwrapper)
//--- (YWeighScale definitions)
class YWeighScale; // forward declaration

typedef void (*YWeighScaleValueCallback)(YWeighScale *func, const string& functionValue);
class YMeasure; // forward declaration
typedef void (*YWeighScaleTimedReportCallback)(YWeighScale *func, YMeasure measure);
#ifndef _Y_EXCITATION_ENUM
#define _Y_EXCITATION_ENUM
typedef enum {
    Y_EXCITATION_OFF = 0,
    Y_EXCITATION_DC = 1,
    Y_EXCITATION_AC = 2,
    Y_EXCITATION_INVALID = -1,
} Y_EXCITATION_enum;
#endif
#define Y_TEMPAVGADAPTRATIO_INVALID     (YAPI_INVALID_DOUBLE)
#define Y_TEMPCHGADAPTRATIO_INVALID     (YAPI_INVALID_DOUBLE)
#define Y_COMPTEMPAVG_INVALID           (YAPI_INVALID_DOUBLE)
#define Y_COMPTEMPCHG_INVALID           (YAPI_INVALID_DOUBLE)
#define Y_COMPENSATION_INVALID          (YAPI_INVALID_DOUBLE)
#define Y_ZEROTRACKING_INVALID          (YAPI_INVALID_DOUBLE)
#define Y_COMMAND_INVALID               (YAPI_INVALID_STRING)
//--- (end of YWeighScale definitions)

//--- (YWeighScale declaration)
/**
 * YWeighScale Class: weighing scale sensor control interface, available for instance in the
 * Yocto-Bridge or the Yocto-MaxiBridge
 *
 * The YWeighScale class provides a weight measurement from a ratiometric sensor.
 * It can be used to control the bridge excitation parameters, in order to avoid
 * measure shifts caused by temperature variation in the electronics, and can also
 * automatically apply an additional correction factor based on temperature to
 * compensate for offsets in the load cell itself.
 */
class YOCTO_CLASS_EXPORT YWeighScale: public YSensor {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of YWeighScale declaration)
protected:
    //--- (YWeighScale attributes)
    // Attributes (function value cache)
    Y_EXCITATION_enum _excitation;
    double          _tempAvgAdaptRatio;
    double          _tempChgAdaptRatio;
    double          _compTempAvg;
    double          _compTempChg;
    double          _compensation;
    double          _zeroTracking;
    string          _command;
    YWeighScaleValueCallback _valueCallbackWeighScale;
    YWeighScaleTimedReportCallback _timedReportCallbackWeighScale;

    friend YWeighScale *yFindWeighScale(const string& func);
    friend YWeighScale *yFirstWeighScale(void);

    // Function-specific method for parsing of JSON output and caching result
    virtual int     _parseAttr(YJSONObject *json_val);

    // Constructor is protected, use yFindWeighScale factory function to instantiate
    YWeighScale(const string& func);
    //--- (end of YWeighScale attributes)

public:
    virtual ~YWeighScale();
    //--- (YWeighScale accessors declaration)

    static const Y_EXCITATION_enum EXCITATION_OFF = Y_EXCITATION_OFF;
    static const Y_EXCITATION_enum EXCITATION_DC = Y_EXCITATION_DC;
    static const Y_EXCITATION_enum EXCITATION_AC = Y_EXCITATION_AC;
    static const Y_EXCITATION_enum EXCITATION_INVALID = Y_EXCITATION_INVALID;
    static const double TEMPAVGADAPTRATIO_INVALID;
    static const double TEMPCHGADAPTRATIO_INVALID;
    static const double COMPTEMPAVG_INVALID;
    static const double COMPTEMPCHG_INVALID;
    static const double COMPENSATION_INVALID;
    static const double ZEROTRACKING_INVALID;
    static const string COMMAND_INVALID;

    /**
     * Changes the measuring unit for the weight.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     *
     * @param newval : a string corresponding to the measuring unit for the weight
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_unit(const string& newval);
    inline int      setUnit(const string& newval)
    { return this->set_unit(newval); }

    /**
     * Returns the current load cell bridge excitation method.
     *
     * @return a value among YWeighScale::EXCITATION_OFF, YWeighScale::EXCITATION_DC and
     * YWeighScale::EXCITATION_AC corresponding to the current load cell bridge excitation method
     *
     * On failure, throws an exception or returns YWeighScale::EXCITATION_INVALID.
     */
    Y_EXCITATION_enum   get_excitation(void);

    inline Y_EXCITATION_enum excitation(void)
    { return this->get_excitation(); }

    /**
     * Changes the current load cell bridge excitation method.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     *
     * @param newval : a value among YWeighScale::EXCITATION_OFF, YWeighScale::EXCITATION_DC and
     * YWeighScale::EXCITATION_AC corresponding to the current load cell bridge excitation method
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_excitation(Y_EXCITATION_enum newval);
    inline int      setExcitation(Y_EXCITATION_enum newval)
    { return this->set_excitation(newval); }

    /**
     * Changes the averaged temperature update rate, in per mille.
     * The purpose of this adaptation ratio is to model the thermal inertia of the load cell.
     * The averaged temperature is updated every 10 seconds, by applying this adaptation rate
     * to the difference between the measures ambient temperature and the current compensation
     * temperature. The standard rate is 0.2 per mille, and the maximal rate is 65 per mille.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     *
     * @param newval : a floating point number corresponding to the averaged temperature update rate, in per mille
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_tempAvgAdaptRatio(double newval);
    inline int      setTempAvgAdaptRatio(double newval)
    { return this->set_tempAvgAdaptRatio(newval); }

    /**
     * Returns the averaged temperature update rate, in per mille.
     * The purpose of this adaptation ratio is to model the thermal inertia of the load cell.
     * The averaged temperature is updated every 10 seconds, by applying this adaptation rate
     * to the difference between the measures ambient temperature and the current compensation
     * temperature. The standard rate is 0.2 per mille, and the maximal rate is 65 per mille.
     *
     * @return a floating point number corresponding to the averaged temperature update rate, in per mille
     *
     * On failure, throws an exception or returns YWeighScale::TEMPAVGADAPTRATIO_INVALID.
     */
    double              get_tempAvgAdaptRatio(void);

    inline double       tempAvgAdaptRatio(void)
    { return this->get_tempAvgAdaptRatio(); }

    /**
     * Changes the temperature change update rate, in per mille.
     * The temperature change is updated every 10 seconds, by applying this adaptation rate
     * to the difference between the measures ambient temperature and the current temperature used for
     * change compensation. The standard rate is 0.6 per mille, and the maximal rate is 65 per mille.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     *
     * @param newval : a floating point number corresponding to the temperature change update rate, in per mille
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_tempChgAdaptRatio(double newval);
    inline int      setTempChgAdaptRatio(double newval)
    { return this->set_tempChgAdaptRatio(newval); }

    /**
     * Returns the temperature change update rate, in per mille.
     * The temperature change is updated every 10 seconds, by applying this adaptation rate
     * to the difference between the measures ambient temperature and the current temperature used for
     * change compensation. The standard rate is 0.6 per mille, and the maximal rate is 65 per mille.
     *
     * @return a floating point number corresponding to the temperature change update rate, in per mille
     *
     * On failure, throws an exception or returns YWeighScale::TEMPCHGADAPTRATIO_INVALID.
     */
    double              get_tempChgAdaptRatio(void);

    inline double       tempChgAdaptRatio(void)
    { return this->get_tempChgAdaptRatio(); }

    /**
     * Returns the current averaged temperature, used for thermal compensation.
     *
     * @return a floating point number corresponding to the current averaged temperature, used for thermal compensation
     *
     * On failure, throws an exception or returns YWeighScale::COMPTEMPAVG_INVALID.
     */
    double              get_compTempAvg(void);

    inline double       compTempAvg(void)
    { return this->get_compTempAvg(); }

    /**
     * Returns the current temperature variation, used for thermal compensation.
     *
     * @return a floating point number corresponding to the current temperature variation, used for
     * thermal compensation
     *
     * On failure, throws an exception or returns YWeighScale::COMPTEMPCHG_INVALID.
     */
    double              get_compTempChg(void);

    inline double       compTempChg(void)
    { return this->get_compTempChg(); }

    /**
     * Returns the current current thermal compensation value.
     *
     * @return a floating point number corresponding to the current current thermal compensation value
     *
     * On failure, throws an exception or returns YWeighScale::COMPENSATION_INVALID.
     */
    double              get_compensation(void);

    inline double       compensation(void)
    { return this->get_compensation(); }

    /**
     * Changes the zero tracking threshold value. When this threshold is larger than
     * zero, any measure under the threshold will automatically be ignored and the
     * zero compensation will be updated.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     *
     * @param newval : a floating point number corresponding to the zero tracking threshold value
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_zeroTracking(double newval);
    inline int      setZeroTracking(double newval)
    { return this->set_zeroTracking(newval); }

    /**
     * Returns the zero tracking threshold value. When this threshold is larger than
     * zero, any measure under the threshold will automatically be ignored and the
     * zero compensation will be updated.
     *
     * @return a floating point number corresponding to the zero tracking threshold value
     *
     * On failure, throws an exception or returns YWeighScale::ZEROTRACKING_INVALID.
     */
    double              get_zeroTracking(void);

    inline double       zeroTracking(void)
    { return this->get_zeroTracking(); }

    string              get_command(void);

    inline string       command(void)
    { return this->get_command(); }

    int             set_command(const string& newval);
    inline int      setCommand(const string& newval)
    { return this->set_command(newval); }

    /**
     * Retrieves a weighing scale sensor for a given identifier.
     * The identifier can be specified using several formats:
     * <ul>
     * <li>FunctionLogicalName</li>
     * <li>ModuleSerialNumber.FunctionIdentifier</li>
     * <li>ModuleSerialNumber.FunctionLogicalName</li>
     * <li>ModuleLogicalName.FunctionIdentifier</li>
     * <li>ModuleLogicalName.FunctionLogicalName</li>
     * </ul>
     *
     * This function does not require that the weighing scale sensor is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method isOnline() to test if the weighing scale sensor is
     * indeed online at a given time. In case of ambiguity when looking for
     * a weighing scale sensor by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     *
     * If a call to this object's is_online() method returns FALSE although
     * you are certain that the matching device is plugged, make sure that you did
     * call registerHub() at application initialization time.
     *
     * @param func : a string that uniquely characterizes the weighing scale sensor, for instance
     *         YWBRIDG1.weighScale1.
     *
     * @return a YWeighScale object allowing you to drive the weighing scale sensor.
     */
    static YWeighScale* FindWeighScale(string func);

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
    virtual int         registerValueCallback(YWeighScaleValueCallback callback);
    using YSensor::registerValueCallback;

    virtual int         _invokeValueCallback(string value);

    /**
     * Registers the callback function that is invoked on every periodic timed notification.
     * The callback is invoked only during the execution of ySleep or yHandleEvents.
     * This provides control over the time when the callback is triggered. For good responsiveness, remember to call
     * one of these two functions periodically. To unregister a callback, pass a NULL pointer as argument.
     *
     * @param callback : the callback function to call, or a NULL pointer. The callback function should take two
     *         arguments: the function object of which the value has changed, and an YMeasure object describing
     *         the new advertised value.
     * @noreturn
     */
    virtual int         registerTimedReportCallback(YWeighScaleTimedReportCallback callback);
    using YSensor::registerTimedReportCallback;

    virtual int         _invokeTimedReportCallback(YMeasure value);

    /**
     * Adapts the load cell signal bias (stored in the corresponding genericSensor)
     * so that the current signal corresponds to a zero weight. Remember to call the
     * saveToFlash() method of the module if the modification must be kept.
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         tare(void);

    /**
     * Configures the load cell span parameters (stored in the corresponding genericSensor)
     * so that the current signal corresponds to the specified reference weight.
     *
     * @param currWeight : reference weight presently on the load cell.
     * @param maxWeight : maximum weight to be expected on the load cell.
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         setupSpan(double currWeight,double maxWeight);

    virtual int         setCompensationTable(int tableIndex,vector<double> tempValues,vector<double> compValues);

    virtual int         loadCompensationTable(int tableIndex,vector<double>& tempValues,vector<double>& compValues);

    /**
     * Records a weight offset thermal compensation table, in order to automatically correct the
     * measured weight based on the averaged compensation temperature.
     * The weight correction will be applied by linear interpolation between specified points.
     *
     * @param tempValues : array of floating point numbers, corresponding to all averaged
     *         temperatures for which an offset correction is specified.
     * @param compValues : array of floating point numbers, corresponding to the offset correction
     *         to apply for each of the temperature included in the first
     *         argument, index by index.
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         set_offsetAvgCompensationTable(vector<double> tempValues,vector<double> compValues);

    /**
     * Retrieves the weight offset thermal compensation table previously configured using the
     * set_offsetAvgCompensationTable function.
     * The weight correction is applied by linear interpolation between specified points.
     *
     * @param tempValues : array of floating point numbers, that is filled by the function
     *         with all averaged temperatures for which an offset correction is specified.
     * @param compValues : array of floating point numbers, that is filled by the function
     *         with the offset correction applied for each of the temperature
     *         included in the first argument, index by index.
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         loadOffsetAvgCompensationTable(vector<double>& tempValues,vector<double>& compValues);

    /**
     * Records a weight offset thermal compensation table, in order to automatically correct the
     * measured weight based on the variation of temperature.
     * The weight correction will be applied by linear interpolation between specified points.
     *
     * @param tempValues : array of floating point numbers, corresponding to temperature
     *         variations for which an offset correction is specified.
     * @param compValues : array of floating point numbers, corresponding to the offset correction
     *         to apply for each of the temperature variation included in the first
     *         argument, index by index.
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         set_offsetChgCompensationTable(vector<double> tempValues,vector<double> compValues);

    /**
     * Retrieves the weight offset thermal compensation table previously configured using the
     * set_offsetChgCompensationTable function.
     * The weight correction is applied by linear interpolation between specified points.
     *
     * @param tempValues : array of floating point numbers, that is filled by the function
     *         with all temperature variations for which an offset correction is specified.
     * @param compValues : array of floating point numbers, that is filled by the function
     *         with the offset correction applied for each of the temperature
     *         variation included in the first argument, index by index.
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         loadOffsetChgCompensationTable(vector<double>& tempValues,vector<double>& compValues);

    /**
     * Records a weight span thermal compensation table, in order to automatically correct the
     * measured weight based on the compensation temperature.
     * The weight correction will be applied by linear interpolation between specified points.
     *
     * @param tempValues : array of floating point numbers, corresponding to all averaged
     *         temperatures for which a span correction is specified.
     * @param compValues : array of floating point numbers, corresponding to the span correction
     *         (in percents) to apply for each of the temperature included in the first
     *         argument, index by index.
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         set_spanAvgCompensationTable(vector<double> tempValues,vector<double> compValues);

    /**
     * Retrieves the weight span thermal compensation table previously configured using the
     * set_spanAvgCompensationTable function.
     * The weight correction is applied by linear interpolation between specified points.
     *
     * @param tempValues : array of floating point numbers, that is filled by the function
     *         with all averaged temperatures for which an span correction is specified.
     * @param compValues : array of floating point numbers, that is filled by the function
     *         with the span correction applied for each of the temperature
     *         included in the first argument, index by index.
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         loadSpanAvgCompensationTable(vector<double>& tempValues,vector<double>& compValues);

    /**
     * Records a weight span thermal compensation table, in order to automatically correct the
     * measured weight based on the variation of temperature.
     * The weight correction will be applied by linear interpolation between specified points.
     *
     * @param tempValues : array of floating point numbers, corresponding to all variations of
     *         temperatures for which a span correction is specified.
     * @param compValues : array of floating point numbers, corresponding to the span correction
     *         (in percents) to apply for each of the temperature variation included
     *         in the first argument, index by index.
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         set_spanChgCompensationTable(vector<double> tempValues,vector<double> compValues);

    /**
     * Retrieves the weight span thermal compensation table previously configured using the
     * set_spanChgCompensationTable function.
     * The weight correction is applied by linear interpolation between specified points.
     *
     * @param tempValues : array of floating point numbers, that is filled by the function
     *         with all variation of temperature for which an span correction is specified.
     * @param compValues : array of floating point numbers, that is filled by the function
     *         with the span correction applied for each of variation of temperature
     *         included in the first argument, index by index.
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         loadSpanChgCompensationTable(vector<double>& tempValues,vector<double>& compValues);


    inline static YWeighScale *Find(string func)
    { return YWeighScale::FindWeighScale(func); }

    /**
     * Continues the enumeration of weighing scale sensors started using yFirstWeighScale().
     * Caution: You can't make any assumption about the returned weighing scale sensors order.
     * If you want to find a specific a weighing scale sensor, use WeighScale.findWeighScale()
     * and a hardwareID or a logical name.
     *
     * @return a pointer to a YWeighScale object, corresponding to
     *         a weighing scale sensor currently online, or a NULL pointer
     *         if there are no more weighing scale sensors to enumerate.
     */
           YWeighScale     *nextWeighScale(void);
    inline YWeighScale     *next(void)
    { return this->nextWeighScale();}

    /**
     * Starts the enumeration of weighing scale sensors currently accessible.
     * Use the method YWeighScale::nextWeighScale() to iterate on
     * next weighing scale sensors.
     *
     * @return a pointer to a YWeighScale object, corresponding to
     *         the first weighing scale sensor currently online, or a NULL pointer
     *         if there are none.
     */
           static YWeighScale *FirstWeighScale(void);
    inline static YWeighScale *First(void)
    { return YWeighScale::FirstWeighScale();}
#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of YWeighScale accessors declaration)
};

//--- (YWeighScale functions declaration)

/**
 * Retrieves a weighing scale sensor for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that the weighing scale sensor is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the weighing scale sensor is
 * indeed online at a given time. In case of ambiguity when looking for
 * a weighing scale sensor by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the weighing scale sensor, for instance
 *         YWBRIDG1.weighScale1.
 *
 * @return a YWeighScale object allowing you to drive the weighing scale sensor.
 */
inline YWeighScale *yFindWeighScale(const string& func)
{ return YWeighScale::FindWeighScale(func);}
/**
 * Starts the enumeration of weighing scale sensors currently accessible.
 * Use the method YWeighScale::nextWeighScale() to iterate on
 * next weighing scale sensors.
 *
 * @return a pointer to a YWeighScale object, corresponding to
 *         the first weighing scale sensor currently online, or a NULL pointer
 *         if there are none.
 */
inline YWeighScale *yFirstWeighScale(void)
{ return YWeighScale::FirstWeighScale();}

//--- (end of YWeighScale functions declaration)

#ifdef YOCTOLIB_NAMESPACE
// end of namespace definition
}
#endif

#endif
