/*********************************************************************
 *
 *  $Id: svn_id $
 *
 *  Declares yFindSoundSpectrum(), the high-level API for SoundSpectrum functions
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


#ifndef YOCTO_SOUNDSPECTRUM_H
#define YOCTO_SOUNDSPECTRUM_H

#include <cfloat>
#include <cmath>

#include "yocto_api.h"

#ifdef YOCTOLIB_NAMESPACE
namespace YOCTOLIB_NAMESPACE
{
#endif

//--- (YSoundSpectrum return codes)
//--- (end of YSoundSpectrum return codes)
//--- (YSoundSpectrum yapiwrapper)
//--- (end of YSoundSpectrum yapiwrapper)
//--- (YSoundSpectrum definitions)
class YSoundSpectrum; // forward declaration

typedef void (*YSoundSpectrumValueCallback)(YSoundSpectrum *func, const string& functionValue);
#define Y_INTEGRATIONTIME_INVALID       (YAPI_INVALID_UINT)
#define Y_SPECTRUMDATA_INVALID          (YAPI_INVALID_STRING)
//--- (end of YSoundSpectrum definitions)

//--- (YSoundSpectrum declaration)
/**
 * YSoundSpectrum Class: sound spectrum analyzer control interface
 *
 * The YSoundSpectrum class allows you to read and configure Yoctopuce sound spectrum analyzers.
 * It inherits from YSensor class the core functions to read measurements,
 * to register callback functions, and to access the autonomous datalogger.
 */
class YOCTO_CLASS_EXPORT YSoundSpectrum: public YFunction {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of YSoundSpectrum declaration)
protected:
    //--- (YSoundSpectrum attributes)
    // Attributes (function value cache)
    int             _integrationTime;
    string          _spectrumData;
    YSoundSpectrumValueCallback _valueCallbackSoundSpectrum;

    friend YSoundSpectrum *yFindSoundSpectrum(const string& func);
    friend YSoundSpectrum *yFirstSoundSpectrum(void);

    // Function-specific method for parsing of JSON output and caching result
    virtual int     _parseAttr(YJSONObject *json_val);

    // Constructor is protected, use yFindSoundSpectrum factory function to instantiate
    YSoundSpectrum(const string& func);
    //--- (end of YSoundSpectrum attributes)

public:
    virtual ~YSoundSpectrum();
    //--- (YSoundSpectrum accessors declaration)

    static const int INTEGRATIONTIME_INVALID = YAPI_INVALID_UINT;
    static const string SPECTRUMDATA_INVALID;

    /**
     * Returns the integration time in milliseconds for calculating time
     * weighted spectrum data.
     *
     * @return an integer corresponding to the integration time in milliseconds for calculating time
     *         weighted spectrum data
     *
     * On failure, throws an exception or returns YSoundSpectrum::INTEGRATIONTIME_INVALID.
     */
    int                 get_integrationTime(void);

    inline int          integrationTime(void)
    { return this->get_integrationTime(); }

    /**
     * Changes the integration time in milliseconds for computing time weighted
     * spectrum data. Be aware that on some devices, changing the integration
     * time for time-weighted spectrum data may also affect the integration
     * period for one or more sound pressure level measurements.
     * Remember to call the saveToFlash() method of the
     * module if the modification must be kept.
     *
     * @param newval : an integer corresponding to the integration time in milliseconds for computing time weighted
     *         spectrum data
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_integrationTime(int newval);
    inline int      setIntegrationTime(int newval)
    { return this->set_integrationTime(newval); }

    string              get_spectrumData(void);

    inline string       spectrumData(void)
    { return this->get_spectrumData(); }

    /**
     * Retrieves a sound spectrum analyzer for a given identifier.
     * The identifier can be specified using several formats:
     *
     * - FunctionLogicalName
     * - ModuleSerialNumber.FunctionIdentifier
     * - ModuleSerialNumber.FunctionLogicalName
     * - ModuleLogicalName.FunctionIdentifier
     * - ModuleLogicalName.FunctionLogicalName
     *
     *
     * This function does not require that the sound spectrum analyzer is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method isOnline() to test if the sound spectrum analyzer is
     * indeed online at a given time. In case of ambiguity when looking for
     * a sound spectrum analyzer by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     *
     * If a call to this object's is_online() method returns FALSE although
     * you are certain that the matching device is plugged, make sure that you did
     * call registerHub() at application initialization time.
     *
     * @param func : a string that uniquely characterizes the sound spectrum analyzer, for instance
     *         MyDevice.soundSpectrum.
     *
     * @return a YSoundSpectrum object allowing you to drive the sound spectrum analyzer.
     */
    static YSoundSpectrum* FindSoundSpectrum(string func);

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
    virtual int         registerValueCallback(YSoundSpectrumValueCallback callback);
    using YFunction::registerValueCallback;

    virtual int         _invokeValueCallback(string value);


    inline static YSoundSpectrum *Find(string func)
    { return YSoundSpectrum::FindSoundSpectrum(func); }

    /**
     * comment from .yc definition
     */
           YSoundSpectrum  *nextSoundSpectrum(void);
    inline YSoundSpectrum  *next(void)
    { return this->nextSoundSpectrum();}

    /**
     * comment from .yc definition
     */
           static YSoundSpectrum *FirstSoundSpectrum(void);
    inline static YSoundSpectrum *First(void)
    { return YSoundSpectrum::FirstSoundSpectrum();}
#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of YSoundSpectrum accessors declaration)
};

//--- (YSoundSpectrum functions declaration)

/**
 * Retrieves a sound spectrum analyzer for a given identifier.
 * The identifier can be specified using several formats:
 *
 * - FunctionLogicalName
 * - ModuleSerialNumber.FunctionIdentifier
 * - ModuleSerialNumber.FunctionLogicalName
 * - ModuleLogicalName.FunctionIdentifier
 * - ModuleLogicalName.FunctionLogicalName
 *
 *
 * This function does not require that the sound spectrum analyzer is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the sound spectrum analyzer is
 * indeed online at a given time. In case of ambiguity when looking for
 * a sound spectrum analyzer by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the sound spectrum analyzer, for instance
 *         MyDevice.soundSpectrum.
 *
 * @return a YSoundSpectrum object allowing you to drive the sound spectrum analyzer.
 */
inline YSoundSpectrum *yFindSoundSpectrum(const string& func)
{ return YSoundSpectrum::FindSoundSpectrum(func);}
/**
 * comment from .yc definition
 */
inline YSoundSpectrum *yFirstSoundSpectrum(void)
{ return YSoundSpectrum::FirstSoundSpectrum();}

//--- (end of YSoundSpectrum functions declaration)

#ifdef YOCTOLIB_NAMESPACE
// end of namespace definition
}
#endif

#endif
