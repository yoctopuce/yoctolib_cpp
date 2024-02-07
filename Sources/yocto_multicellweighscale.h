/*********************************************************************
 *
 *  $Id: yocto_multicellweighscale.h 44049 2021-02-26 10:57:40Z web $
 *
 *  Declares yFindMultiCellWeighScale(), the high-level API for MultiCellWeighScale functions
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


#ifndef YOCTO_MULTICELLWEIGHSCALE_H
#define YOCTO_MULTICELLWEIGHSCALE_H

#include <cfloat>
#include <cmath>

#include "yocto_api.h"

#ifdef YOCTOLIB_NAMESPACE
namespace YOCTOLIB_NAMESPACE
{
#endif

//--- (YMultiCellWeighScale return codes)
//--- (end of YMultiCellWeighScale return codes)
//--- (YMultiCellWeighScale yapiwrapper)
//--- (end of YMultiCellWeighScale yapiwrapper)
//--- (YMultiCellWeighScale definitions)
class YMultiCellWeighScale; // forward declaration

typedef void (*YMultiCellWeighScaleValueCallback)(YMultiCellWeighScale *func, const string& functionValue);
class YMeasure; // forward declaration
typedef void (*YMultiCellWeighScaleTimedReportCallback)(YMultiCellWeighScale *func, YMeasure measure);
#ifndef _Y_EXTERNALSENSE_ENUM
#define _Y_EXTERNALSENSE_ENUM
typedef enum {
    Y_EXTERNALSENSE_FALSE = 0,
    Y_EXTERNALSENSE_TRUE = 1,
    Y_EXTERNALSENSE_INVALID = -1,
} Y_EXTERNALSENSE_enum;
#endif
#ifndef _Y_EXCITATION_ENUM
#define _Y_EXCITATION_ENUM
typedef enum {
    Y_EXCITATION_OFF = 0,
    Y_EXCITATION_DC = 1,
    Y_EXCITATION_AC = 2,
    Y_EXCITATION_INVALID = -1,
} Y_EXCITATION_enum;
#endif
#define Y_CELLCOUNT_INVALID             (YAPI_INVALID_UINT)
#define Y_TEMPAVGADAPTRATIO_INVALID     (YAPI_INVALID_DOUBLE)
#define Y_TEMPCHGADAPTRATIO_INVALID     (YAPI_INVALID_DOUBLE)
#define Y_COMPTEMPAVG_INVALID           (YAPI_INVALID_DOUBLE)
#define Y_COMPTEMPCHG_INVALID           (YAPI_INVALID_DOUBLE)
#define Y_COMPENSATION_INVALID          (YAPI_INVALID_DOUBLE)
#define Y_ZEROTRACKING_INVALID          (YAPI_INVALID_DOUBLE)
#define Y_COMMAND_INVALID               (YAPI_INVALID_STRING)
//--- (end of YMultiCellWeighScale definitions)

//--- (YMultiCellWeighScale declaration)
/**
 * YMultiCellWeighScale Class: multi-cell weighing scale sensor control interface, available for
 * instance in the Yocto-MaxiBridge
 *
 * The YMultiCellWeighScale class provides a weight measurement from a set of ratiometric
 * sensors. It can be used to control the bridge excitation parameters, in order to avoid
 * measure shifts caused by temperature variation in the electronics, and can also
 * automatically apply an additional correction factor based on temperature to
 * compensate for offsets in the load cells themselves.
 */
class YOCTO_CLASS_EXPORT YMultiCellWeighScale: public YSensor {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of YMultiCellWeighScale declaration)
protected:
    //--- (YMultiCellWeighScale attributes)
    // Attributes (function value cache)
    int             _cellCount;
    Y_EXTERNALSENSE_enum _externalSense;
    Y_EXCITATION_enum _excitation;
    double          _tempAvgAdaptRatio;
    double          _tempChgAdaptRatio;
    double          _compTempAvg;
    double          _compTempChg;
    double          _compensation;
    double          _zeroTracking;
    string          _command;
    YMultiCellWeighScaleValueCallback _valueCallbackMultiCellWeighScale;
    YMultiCellWeighScaleTimedReportCallback _timedReportCallbackMultiCellWeighScale;

    friend YMultiCellWeighScale *yFindMultiCellWeighScale(const string& func);
    friend YMultiCellWeighScale *yFirstMultiCellWeighScale(void);

    // Function-specific method for parsing of JSON output and caching result
    virtual int     _parseAttr(YJSONObject *json_val);

    // Constructor is protected, use yFindMultiCellWeighScale factory function to instantiate
    YMultiCellWeighScale(const string& func);
    //--- (end of YMultiCellWeighScale attributes)

public:
    virtual ~YMultiCellWeighScale();
    //--- (YMultiCellWeighScale accessors declaration)

    static const int CELLCOUNT_INVALID = YAPI_INVALID_UINT;
    static const Y_EXTERNALSENSE_enum EXTERNALSENSE_FALSE = Y_EXTERNALSENSE_FALSE;
    static const Y_EXTERNALSENSE_enum EXTERNALSENSE_TRUE = Y_EXTERNALSENSE_TRUE;
    static const Y_EXTERNALSENSE_enum EXTERNALSENSE_INVALID = Y_EXTERNALSENSE_INVALID;
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
     * Returns the number of load cells in use.
     *
     * @return an integer corresponding to the number of load cells in use
     *
     * On failure, throws an exception or returns YMultiCellWeighScale::CELLCOUNT_INVALID.
     */
    int                 get_cellCount(void);

    inline int          cellCount(void)
    { return this->get_cellCount(); }

    /**
     * Changes the number of load cells in use. Remember to call the saveToFlash()
     * method of the module if the modification must be kept.
     *
     * @param newval : an integer corresponding to the number of load cells in use
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_cellCount(int newval);
    inline int      setCellCount(int newval)
    { return this->set_cellCount(newval); }

    /**
     * Returns true if entry 4 is used as external sense for 6-wires load cells.
     *
     * @return either YMultiCellWeighScale::EXTERNALSENSE_FALSE or YMultiCellWeighScale::EXTERNALSENSE_TRUE,
     * according to true if entry 4 is used as external sense for 6-wires load cells
     *
     * On failure, throws an exception or returns YMultiCellWeighScale::EXTERNALSENSE_INVALID.
     */
    Y_EXTERNALSENSE_enum get_externalSense(void);

    inline Y_EXTERNALSENSE_enum externalSense(void)
    { return this->get_externalSense(); }

    /**
     * Changes the configuration to tell if entry 4 is used as external sense for
     * 6-wires load cells. Remember to call the saveToFlash() method of the
     * module if the modification must be kept.
     *
     * @param newval : either YMultiCellWeighScale::EXTERNALSENSE_FALSE or
     * YMultiCellWeighScale::EXTERNALSENSE_TRUE, according to the configuration to tell if entry 4 is used
     * as external sense for
     *         6-wires load cells
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_externalSense(Y_EXTERNALSENSE_enum newval);
    inline int      setExternalSense(Y_EXTERNALSENSE_enum newval)
    { return this->set_externalSense(newval); }

    /**
     * Returns the current load cell bridge excitation method.
     *
     * @return a value among YMultiCellWeighScale::EXCITATION_OFF, YMultiCellWeighScale::EXCITATION_DC and
     * YMultiCellWeighScale::EXCITATION_AC corresponding to the current load cell bridge excitation method
     *
     * On failure, throws an exception or returns YMultiCellWeighScale::EXCITATION_INVALID.
     */
    Y_EXCITATION_enum   get_excitation(void);

    inline Y_EXCITATION_enum excitation(void)
    { return this->get_excitation(); }

    /**
     * Changes the current load cell bridge excitation method.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     *
     * @param newval : a value among YMultiCellWeighScale::EXCITATION_OFF,
     * YMultiCellWeighScale::EXCITATION_DC and YMultiCellWeighScale::EXCITATION_AC corresponding to the
     * current load cell bridge excitation method
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
     * On failure, throws an exception or returns YMultiCellWeighScale::TEMPAVGADAPTRATIO_INVALID.
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
     * On failure, throws an exception or returns YMultiCellWeighScale::TEMPCHGADAPTRATIO_INVALID.
     */
    double              get_tempChgAdaptRatio(void);

    inline double       tempChgAdaptRatio(void)
    { return this->get_tempChgAdaptRatio(); }

    /**
     * Returns the current averaged temperature, used for thermal compensation.
     *
     * @return a floating point number corresponding to the current averaged temperature, used for thermal compensation
     *
     * On failure, throws an exception or returns YMultiCellWeighScale::COMPTEMPAVG_INVALID.
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
     * On failure, throws an exception or returns YMultiCellWeighScale::COMPTEMPCHG_INVALID.
     */
    double              get_compTempChg(void);

    inline double       compTempChg(void)
    { return this->get_compTempChg(); }

    /**
     * Returns the current current thermal compensation value.
     *
     * @return a floating point number corresponding to the current current thermal compensation value
     *
     * On failure, throws an exception or returns YMultiCellWeighScale::COMPENSATION_INVALID.
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
     * On failure, throws an exception or returns YMultiCellWeighScale::ZEROTRACKING_INVALID.
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
     * Retrieves a multi-cell weighing scale sensor for a given identifier.
     * The identifier can be specified using several formats:
     * <ul>
     * <li>FunctionLogicalName</li>
     * <li>ModuleSerialNumber.FunctionIdentifier</li>
     * <li>ModuleSerialNumber.FunctionLogicalName</li>
     * <li>ModuleLogicalName.FunctionIdentifier</li>
     * <li>ModuleLogicalName.FunctionLogicalName</li>
     * </ul>
     *
     * This function does not require that the multi-cell weighing scale sensor is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method isOnline() to test if the multi-cell weighing scale sensor is
     * indeed online at a given time. In case of ambiguity when looking for
     * a multi-cell weighing scale sensor by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     *
     * If a call to this object's is_online() method returns FALSE although
     * you are certain that the matching device is plugged, make sure that you did
     * call registerHub() at application initialization time.
     *
     * @param func : a string that uniquely characterizes the multi-cell weighing scale sensor, for instance
     *         YWMBRDG1.multiCellWeighScale.
     *
     * @return a YMultiCellWeighScale object allowing you to drive the multi-cell weighing scale sensor.
     */
    static YMultiCellWeighScale* FindMultiCellWeighScale(string func);

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
    virtual int         registerValueCallback(YMultiCellWeighScaleValueCallback callback);
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
    virtual int         registerTimedReportCallback(YMultiCellWeighScaleTimedReportCallback callback);
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
     * Configures the load cells span parameters (stored in the corresponding genericSensors)
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


    inline static YMultiCellWeighScale *Find(string func)
    { return YMultiCellWeighScale::FindMultiCellWeighScale(func); }

    /**
     * Continues the enumeration of multi-cell weighing scale sensors started using yFirstMultiCellWeighScale().
     * Caution: You can't make any assumption about the returned multi-cell weighing scale sensors order.
     * If you want to find a specific a multi-cell weighing scale sensor, use
     * MultiCellWeighScale.findMultiCellWeighScale()
     * and a hardwareID or a logical name.
     *
     * @return a pointer to a YMultiCellWeighScale object, corresponding to
     *         a multi-cell weighing scale sensor currently online, or a NULL pointer
     *         if there are no more multi-cell weighing scale sensors to enumerate.
     */
           YMultiCellWeighScale *nextMultiCellWeighScale(void);
    inline YMultiCellWeighScale *next(void)
    { return this->nextMultiCellWeighScale();}

    /**
     * Starts the enumeration of multi-cell weighing scale sensors currently accessible.
     * Use the method YMultiCellWeighScale::nextMultiCellWeighScale() to iterate on
     * next multi-cell weighing scale sensors.
     *
     * @return a pointer to a YMultiCellWeighScale object, corresponding to
     *         the first multi-cell weighing scale sensor currently online, or a NULL pointer
     *         if there are none.
     */
           static YMultiCellWeighScale *FirstMultiCellWeighScale(void);
    inline static YMultiCellWeighScale *First(void)
    { return YMultiCellWeighScale::FirstMultiCellWeighScale();}
#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of YMultiCellWeighScale accessors declaration)
};

//--- (YMultiCellWeighScale functions declaration)

/**
 * Retrieves a multi-cell weighing scale sensor for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that the multi-cell weighing scale sensor is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the multi-cell weighing scale sensor is
 * indeed online at a given time. In case of ambiguity when looking for
 * a multi-cell weighing scale sensor by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the multi-cell weighing scale sensor, for instance
 *         YWMBRDG1.multiCellWeighScale.
 *
 * @return a YMultiCellWeighScale object allowing you to drive the multi-cell weighing scale sensor.
 */
inline YMultiCellWeighScale *yFindMultiCellWeighScale(const string& func)
{ return YMultiCellWeighScale::FindMultiCellWeighScale(func);}
/**
 * Starts the enumeration of multi-cell weighing scale sensors currently accessible.
 * Use the method YMultiCellWeighScale::nextMultiCellWeighScale() to iterate on
 * next multi-cell weighing scale sensors.
 *
 * @return a pointer to a YMultiCellWeighScale object, corresponding to
 *         the first multi-cell weighing scale sensor currently online, or a NULL pointer
 *         if there are none.
 */
inline YMultiCellWeighScale *yFirstMultiCellWeighScale(void)
{ return YMultiCellWeighScale::FirstMultiCellWeighScale();}

//--- (end of YMultiCellWeighScale functions declaration)

#ifdef YOCTOLIB_NAMESPACE
// end of namespace definition
}
#endif

#endif
