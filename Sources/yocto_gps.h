/*********************************************************************
 *
 *  $Id: yocto_gps.h 44049 2021-02-26 10:57:40Z web $
 *
 *  Declares yFindGps(), the high-level API for Gps functions
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


#ifndef YOCTO_GPS_H
#define YOCTO_GPS_H

#include <cfloat>
#include <cmath>

#include "yocto_api.h"

#ifdef YOCTOLIB_NAMESPACE
namespace YOCTOLIB_NAMESPACE
{
#endif

//--- (YGps return codes)
//--- (end of YGps return codes)
//--- (YGps yapiwrapper)
//--- (end of YGps yapiwrapper)
//--- (YGps definitions)
class YGps; // forward declaration

typedef void (*YGpsValueCallback)(YGps *func, const string& functionValue);
#ifndef _Y_ISFIXED_ENUM
#define _Y_ISFIXED_ENUM
typedef enum {
    Y_ISFIXED_FALSE = 0,
    Y_ISFIXED_TRUE = 1,
    Y_ISFIXED_INVALID = -1,
} Y_ISFIXED_enum;
#endif
#ifndef _Y_COORDSYSTEM_ENUM
#define _Y_COORDSYSTEM_ENUM
typedef enum {
    Y_COORDSYSTEM_GPS_DMS = 0,
    Y_COORDSYSTEM_GPS_DM = 1,
    Y_COORDSYSTEM_GPS_D = 2,
    Y_COORDSYSTEM_INVALID = -1,
} Y_COORDSYSTEM_enum;
#endif
#ifndef _Y_CONSTELLATION_ENUM
#define _Y_CONSTELLATION_ENUM
typedef enum {
    Y_CONSTELLATION_GNSS = 0,
    Y_CONSTELLATION_GPS = 1,
    Y_CONSTELLATION_GLONASS = 2,
    Y_CONSTELLATION_GALILEO = 3,
    Y_CONSTELLATION_GPS_GLONASS = 4,
    Y_CONSTELLATION_GPS_GALILEO = 5,
    Y_CONSTELLATION_GLONASS_GALILEO = 6,
    Y_CONSTELLATION_INVALID = -1,
} Y_CONSTELLATION_enum;
#endif
#define Y_SATCOUNT_INVALID              (YAPI_INVALID_LONG)
#define Y_SATPERCONST_INVALID           (YAPI_INVALID_LONG)
#define Y_GPSREFRESHRATE_INVALID        (YAPI_INVALID_DOUBLE)
#define Y_LATITUDE_INVALID              (YAPI_INVALID_STRING)
#define Y_LONGITUDE_INVALID             (YAPI_INVALID_STRING)
#define Y_DILUTION_INVALID              (YAPI_INVALID_DOUBLE)
#define Y_ALTITUDE_INVALID              (YAPI_INVALID_DOUBLE)
#define Y_GROUNDSPEED_INVALID           (YAPI_INVALID_DOUBLE)
#define Y_DIRECTION_INVALID             (YAPI_INVALID_DOUBLE)
#define Y_UNIXTIME_INVALID              (YAPI_INVALID_LONG)
#define Y_DATETIME_INVALID              (YAPI_INVALID_STRING)
#define Y_UTCOFFSET_INVALID             (YAPI_INVALID_INT)
#define Y_COMMAND_INVALID               (YAPI_INVALID_STRING)
//--- (end of YGps definitions)

//--- (YGps declaration)
/**
 * YGps Class: Geolocalization control interface (GPS, GNSS, ...), available for instance in the Yocto-GPS-V2
 *
 * The YGps class allows you to retrieve positioning
 * data from a GPS/GNSS sensor. This class can provides
 * complete positioning information. However, if you
 * wish to define callbacks on position changes or record
 * the position in the datalogger, you
 * should use the YLatitude et YLongitude classes.
 */
class YOCTO_CLASS_EXPORT YGps: public YFunction {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of YGps declaration)
protected:
    //--- (YGps attributes)
    // Attributes (function value cache)
    Y_ISFIXED_enum  _isFixed;
    s64             _satCount;
    s64             _satPerConst;
    double          _gpsRefreshRate;
    Y_COORDSYSTEM_enum _coordSystem;
    Y_CONSTELLATION_enum _constellation;
    string          _latitude;
    string          _longitude;
    double          _dilution;
    double          _altitude;
    double          _groundSpeed;
    double          _direction;
    s64             _unixTime;
    string          _dateTime;
    int             _utcOffset;
    string          _command;
    YGpsValueCallback _valueCallbackGps;

    friend YGps *yFindGps(const string& func);
    friend YGps *yFirstGps(void);

    // Function-specific method for parsing of JSON output and caching result
    virtual int     _parseAttr(YJSONObject *json_val);

    // Constructor is protected, use yFindGps factory function to instantiate
    YGps(const string& func);
    //--- (end of YGps attributes)

public:
    virtual ~YGps();
    //--- (YGps accessors declaration)

    static const Y_ISFIXED_enum ISFIXED_FALSE = Y_ISFIXED_FALSE;
    static const Y_ISFIXED_enum ISFIXED_TRUE = Y_ISFIXED_TRUE;
    static const Y_ISFIXED_enum ISFIXED_INVALID = Y_ISFIXED_INVALID;
    static const s64 SATCOUNT_INVALID = YAPI_INVALID_LONG;
    static const s64 SATPERCONST_INVALID = YAPI_INVALID_LONG;
    static const double GPSREFRESHRATE_INVALID;
    static const Y_COORDSYSTEM_enum COORDSYSTEM_GPS_DMS = Y_COORDSYSTEM_GPS_DMS;
    static const Y_COORDSYSTEM_enum COORDSYSTEM_GPS_DM = Y_COORDSYSTEM_GPS_DM;
    static const Y_COORDSYSTEM_enum COORDSYSTEM_GPS_D = Y_COORDSYSTEM_GPS_D;
    static const Y_COORDSYSTEM_enum COORDSYSTEM_INVALID = Y_COORDSYSTEM_INVALID;
    static const Y_CONSTELLATION_enum CONSTELLATION_GNSS = Y_CONSTELLATION_GNSS;
    static const Y_CONSTELLATION_enum CONSTELLATION_GPS = Y_CONSTELLATION_GPS;
    static const Y_CONSTELLATION_enum CONSTELLATION_GLONASS = Y_CONSTELLATION_GLONASS;
    static const Y_CONSTELLATION_enum CONSTELLATION_GALILEO = Y_CONSTELLATION_GALILEO;
    static const Y_CONSTELLATION_enum CONSTELLATION_GPS_GLONASS = Y_CONSTELLATION_GPS_GLONASS;
    static const Y_CONSTELLATION_enum CONSTELLATION_GPS_GALILEO = Y_CONSTELLATION_GPS_GALILEO;
    static const Y_CONSTELLATION_enum CONSTELLATION_GLONASS_GALILEO = Y_CONSTELLATION_GLONASS_GALILEO;
    static const Y_CONSTELLATION_enum CONSTELLATION_INVALID = Y_CONSTELLATION_INVALID;
    static const string LATITUDE_INVALID;
    static const string LONGITUDE_INVALID;
    static const double DILUTION_INVALID;
    static const double ALTITUDE_INVALID;
    static const double GROUNDSPEED_INVALID;
    static const double DIRECTION_INVALID;
    static const s64 UNIXTIME_INVALID = YAPI_INVALID_LONG;
    static const string DATETIME_INVALID;
    static const int UTCOFFSET_INVALID = YAPI_INVALID_INT;
    static const string COMMAND_INVALID;

    /**
     * Returns TRUE if the receiver has found enough satellites to work.
     *
     * @return either YGps::ISFIXED_FALSE or YGps::ISFIXED_TRUE, according to TRUE if the receiver has found
     * enough satellites to work
     *
     * On failure, throws an exception or returns YGps::ISFIXED_INVALID.
     */
    Y_ISFIXED_enum      get_isFixed(void);

    inline Y_ISFIXED_enum isFixed(void)
    { return this->get_isFixed(); }

    /**
     * Returns the total count of satellites used to compute GPS position.
     *
     * @return an integer corresponding to the total count of satellites used to compute GPS position
     *
     * On failure, throws an exception or returns YGps::SATCOUNT_INVALID.
     */
    s64                 get_satCount(void);

    inline s64          satCount(void)
    { return this->get_satCount(); }

    /**
     * Returns the count of visible satellites per constellation encoded
     * on a 32 bit integer: bits 0..5: GPS satellites count,  bits 6..11 : Glonass, bits 12..17 : Galileo.
     * this value is refreshed every 5 seconds only.
     *
     * @return an integer corresponding to the count of visible satellites per constellation encoded
     *         on a 32 bit integer: bits 0.
     *
     * On failure, throws an exception or returns YGps::SATPERCONST_INVALID.
     */
    s64                 get_satPerConst(void);

    inline s64          satPerConst(void)
    { return this->get_satPerConst(); }

    /**
     * Returns effective GPS data refresh frequency.
     * this value is refreshed every 5 seconds only.
     *
     * @return a floating point number corresponding to effective GPS data refresh frequency
     *
     * On failure, throws an exception or returns YGps::GPSREFRESHRATE_INVALID.
     */
    double              get_gpsRefreshRate(void);

    inline double       gpsRefreshRate(void)
    { return this->get_gpsRefreshRate(); }

    /**
     * Returns the representation system used for positioning data.
     *
     * @return a value among YGps::COORDSYSTEM_GPS_DMS, YGps::COORDSYSTEM_GPS_DM and YGps::COORDSYSTEM_GPS_D
     * corresponding to the representation system used for positioning data
     *
     * On failure, throws an exception or returns YGps::COORDSYSTEM_INVALID.
     */
    Y_COORDSYSTEM_enum  get_coordSystem(void);

    inline Y_COORDSYSTEM_enum coordSystem(void)
    { return this->get_coordSystem(); }

    /**
     * Changes the representation system used for positioning data.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     *
     * @param newval : a value among YGps::COORDSYSTEM_GPS_DMS, YGps::COORDSYSTEM_GPS_DM and
     * YGps::COORDSYSTEM_GPS_D corresponding to the representation system used for positioning data
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_coordSystem(Y_COORDSYSTEM_enum newval);
    inline int      setCoordSystem(Y_COORDSYSTEM_enum newval)
    { return this->set_coordSystem(newval); }

    /**
     * Returns the the satellites constellation used to compute
     * positioning data.
     *
     * @return a value among YGps::CONSTELLATION_GNSS, YGps::CONSTELLATION_GPS, YGps::CONSTELLATION_GLONASS,
     * YGps::CONSTELLATION_GALILEO, YGps::CONSTELLATION_GPS_GLONASS, YGps::CONSTELLATION_GPS_GALILEO and
     * YGps::CONSTELLATION_GLONASS_GALILEO corresponding to the the satellites constellation used to compute
     *         positioning data
     *
     * On failure, throws an exception or returns YGps::CONSTELLATION_INVALID.
     */
    Y_CONSTELLATION_enum get_constellation(void);

    inline Y_CONSTELLATION_enum constellation(void)
    { return this->get_constellation(); }

    /**
     * Changes the satellites constellation used to compute
     * positioning data. Possible  constellations are GNSS ( = all supported constellations),
     * GPS, Glonass, Galileo , and the 3 possible pairs. This setting has  no effect on Yocto-GPS (V1).
     *
     * @param newval : a value among YGps::CONSTELLATION_GNSS, YGps::CONSTELLATION_GPS,
     * YGps::CONSTELLATION_GLONASS, YGps::CONSTELLATION_GALILEO, YGps::CONSTELLATION_GPS_GLONASS,
     * YGps::CONSTELLATION_GPS_GALILEO and YGps::CONSTELLATION_GLONASS_GALILEO corresponding to the
     * satellites constellation used to compute
     *         positioning data
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_constellation(Y_CONSTELLATION_enum newval);
    inline int      setConstellation(Y_CONSTELLATION_enum newval)
    { return this->set_constellation(newval); }

    /**
     * Returns the current latitude.
     *
     * @return a string corresponding to the current latitude
     *
     * On failure, throws an exception or returns YGps::LATITUDE_INVALID.
     */
    string              get_latitude(void);

    inline string       latitude(void)
    { return this->get_latitude(); }

    /**
     * Returns the current longitude.
     *
     * @return a string corresponding to the current longitude
     *
     * On failure, throws an exception or returns YGps::LONGITUDE_INVALID.
     */
    string              get_longitude(void);

    inline string       longitude(void)
    { return this->get_longitude(); }

    /**
     * Returns the current horizontal dilution of precision,
     * the smaller that number is, the better .
     *
     * @return a floating point number corresponding to the current horizontal dilution of precision,
     *         the smaller that number is, the better
     *
     * On failure, throws an exception or returns YGps::DILUTION_INVALID.
     */
    double              get_dilution(void);

    inline double       dilution(void)
    { return this->get_dilution(); }

    /**
     * Returns the current altitude. Beware:  GPS technology
     * is very inaccurate regarding altitude.
     *
     * @return a floating point number corresponding to the current altitude
     *
     * On failure, throws an exception or returns YGps::ALTITUDE_INVALID.
     */
    double              get_altitude(void);

    inline double       altitude(void)
    { return this->get_altitude(); }

    /**
     * Returns the current ground speed in Km/h.
     *
     * @return a floating point number corresponding to the current ground speed in Km/h
     *
     * On failure, throws an exception or returns YGps::GROUNDSPEED_INVALID.
     */
    double              get_groundSpeed(void);

    inline double       groundSpeed(void)
    { return this->get_groundSpeed(); }

    /**
     * Returns the current move bearing in degrees, zero
     * is the true (geographic) north.
     *
     * @return a floating point number corresponding to the current move bearing in degrees, zero
     *         is the true (geographic) north
     *
     * On failure, throws an exception or returns YGps::DIRECTION_INVALID.
     */
    double              get_direction(void);

    inline double       direction(void)
    { return this->get_direction(); }

    /**
     * Returns the current time in Unix format (number of
     * seconds elapsed since Jan 1st, 1970).
     *
     * @return an integer corresponding to the current time in Unix format (number of
     *         seconds elapsed since Jan 1st, 1970)
     *
     * On failure, throws an exception or returns YGps::UNIXTIME_INVALID.
     */
    s64                 get_unixTime(void);

    inline s64          unixTime(void)
    { return this->get_unixTime(); }

    /**
     * Returns the current time in the form "YYYY/MM/DD hh:mm:ss".
     *
     * @return a string corresponding to the current time in the form "YYYY/MM/DD hh:mm:ss"
     *
     * On failure, throws an exception or returns YGps::DATETIME_INVALID.
     */
    string              get_dateTime(void);

    inline string       dateTime(void)
    { return this->get_dateTime(); }

    /**
     * Returns the number of seconds between current time and UTC time (time zone).
     *
     * @return an integer corresponding to the number of seconds between current time and UTC time (time zone)
     *
     * On failure, throws an exception or returns YGps::UTCOFFSET_INVALID.
     */
    int                 get_utcOffset(void);

    inline int          utcOffset(void)
    { return this->get_utcOffset(); }

    /**
     * Changes the number of seconds between current time and UTC time (time zone).
     * The timezone is automatically rounded to the nearest multiple of 15 minutes.
     * If current UTC time is known, the current time is automatically be updated according to the selected time zone.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     *
     * @param newval : an integer corresponding to the number of seconds between current time and UTC time (time zone)
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_utcOffset(int newval);
    inline int      setUtcOffset(int newval)
    { return this->set_utcOffset(newval); }

    string              get_command(void);

    inline string       command(void)
    { return this->get_command(); }

    int             set_command(const string& newval);
    inline int      setCommand(const string& newval)
    { return this->set_command(newval); }

    /**
     * Retrieves a geolocalization module for a given identifier.
     * The identifier can be specified using several formats:
     * <ul>
     * <li>FunctionLogicalName</li>
     * <li>ModuleSerialNumber.FunctionIdentifier</li>
     * <li>ModuleSerialNumber.FunctionLogicalName</li>
     * <li>ModuleLogicalName.FunctionIdentifier</li>
     * <li>ModuleLogicalName.FunctionLogicalName</li>
     * </ul>
     *
     * This function does not require that the geolocalization module is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method isOnline() to test if the geolocalization module is
     * indeed online at a given time. In case of ambiguity when looking for
     * a geolocalization module by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     *
     * If a call to this object's is_online() method returns FALSE although
     * you are certain that the matching device is plugged, make sure that you did
     * call registerHub() at application initialization time.
     *
     * @param func : a string that uniquely characterizes the geolocalization module, for instance
     *         YGNSSMK2.gps.
     *
     * @return a YGps object allowing you to drive the geolocalization module.
     */
    static YGps*        FindGps(string func);

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
    virtual int         registerValueCallback(YGpsValueCallback callback);
    using YFunction::registerValueCallback;

    virtual int         _invokeValueCallback(string value);


    inline static YGps *Find(string func)
    { return YGps::FindGps(func); }

    /**
     * Continues the enumeration of geolocalization modules started using yFirstGps().
     * Caution: You can't make any assumption about the returned geolocalization modules order.
     * If you want to find a specific a geolocalization module, use Gps.findGps()
     * and a hardwareID or a logical name.
     *
     * @return a pointer to a YGps object, corresponding to
     *         a geolocalization module currently online, or a NULL pointer
     *         if there are no more geolocalization modules to enumerate.
     */
           YGps            *nextGps(void);
    inline YGps            *next(void)
    { return this->nextGps();}

    /**
     * Starts the enumeration of geolocalization modules currently accessible.
     * Use the method YGps::nextGps() to iterate on
     * next geolocalization modules.
     *
     * @return a pointer to a YGps object, corresponding to
     *         the first geolocalization module currently online, or a NULL pointer
     *         if there are none.
     */
           static YGps *FirstGps(void);
    inline static YGps *First(void)
    { return YGps::FirstGps();}
#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of YGps accessors declaration)
};

//--- (YGps functions declaration)

/**
 * Retrieves a geolocalization module for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that the geolocalization module is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the geolocalization module is
 * indeed online at a given time. In case of ambiguity when looking for
 * a geolocalization module by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the geolocalization module, for instance
 *         YGNSSMK2.gps.
 *
 * @return a YGps object allowing you to drive the geolocalization module.
 */
inline YGps *yFindGps(const string& func)
{ return YGps::FindGps(func);}
/**
 * Starts the enumeration of geolocalization modules currently accessible.
 * Use the method YGps::nextGps() to iterate on
 * next geolocalization modules.
 *
 * @return a pointer to a YGps object, corresponding to
 *         the first geolocalization module currently online, or a NULL pointer
 *         if there are none.
 */
inline YGps *yFirstGps(void)
{ return YGps::FirstGps();}

//--- (end of YGps functions declaration)

#ifdef YOCTOLIB_NAMESPACE
// end of namespace definition
}
#endif

#endif
