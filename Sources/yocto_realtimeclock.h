/*********************************************************************
 *
 * $Id: yocto_realtimeclock.h 12324 2013-08-13 15:10:31Z mvuilleu $
 *
 * Declares yFindRealTimeClock(), the high-level API for RealTimeClock functions
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


#ifndef YOCTO_REALTIMECLOCK_H
#define YOCTO_REALTIMECLOCK_H

#include "yocto_api.h"
#include <cfloat>
#include <cmath>
#include <map>

//--- (return codes)
//--- (end of return codes)
//--- (YRealTimeClock definitions)
class YRealTimeClock; //forward declaration

typedef void (*YRealTimeClockUpdateCallback)(YRealTimeClock *func, const string& functionValue);
typedef enum {
    Y_TIMESET_FALSE = 0,
    Y_TIMESET_TRUE = 1,
    Y_TIMESET_INVALID = -1,
} Y_TIMESET_enum;

#define Y_LOGICALNAME_INVALID           (YAPI::INVALID_STRING)
#define Y_ADVERTISEDVALUE_INVALID       (YAPI::INVALID_STRING)
#define Y_UNIXTIME_INVALID              (0xffffffff)
#define Y_DATETIME_INVALID              (YAPI::INVALID_STRING)
#define Y_UTCOFFSET_INVALID             (0x80000000)
//--- (end of YRealTimeClock definitions)

//--- (YRealTimeClock declaration)
/**
 * YRealTimeClock Class: Real Time Clock function interface
 * 
 * The RealTimeClock function maintains and provides current date and time, even accross power cut
 * lasting several days. It is the base for automated wake-up functions provided by the WakeUpScheduler.
 * The current time may represent a local time as well as an UTC time, but no automatic time change
 * will occur to account for daylight saving time.
 */
class YRealTimeClock: public YFunction {
protected:
    // Attributes (function value cache)
    YRealTimeClockUpdateCallback _callback;
    string          _logicalName;
    string          _advertisedValue;
    unsigned        _unixTime;
    string          _dateTime;
    int             _utcOffset;
    Y_TIMESET_enum  _timeSet;

    friend YRealTimeClock *yFindRealTimeClock(const string& func);
    friend YRealTimeClock *yFirstRealTimeClock(void);

    // Function-specific method for parsing of JSON output and caching result
    int             _parse(yJsonStateMachine& j);
    //--- (end of YRealTimeClock declaration)

    //--- (YRealTimeClock constructor)
    // Constructor is protected, use yFindRealTimeClock factory function to instantiate
    YRealTimeClock(const string& func);
    //--- (end of YRealTimeClock constructor)
    //--- (RealTimeClock initialization)
    //--- (end of RealTimeClock initialization)

public:
    ~YRealTimeClock();
    //--- (YRealTimeClock accessors declaration)

    static const string LOGICALNAME_INVALID;
    static const string ADVERTISEDVALUE_INVALID;
    static const unsigned UNIXTIME_INVALID = 0xffffffff;
    static const string DATETIME_INVALID;
    static const int      UTCOFFSET_INVALID = 0x80000000;
    static const Y_TIMESET_enum TIMESET_FALSE = Y_TIMESET_FALSE;
    static const Y_TIMESET_enum TIMESET_TRUE = Y_TIMESET_TRUE;
    static const Y_TIMESET_enum TIMESET_INVALID = Y_TIMESET_INVALID;

    /**
     * Returns the logical name of the clock.
     * 
     * @return a string corresponding to the logical name of the clock
     * 
     * On failure, throws an exception or returns Y_LOGICALNAME_INVALID.
     */
           string          get_logicalName(void);
    inline string          logicalName(void)
    { return this->get_logicalName(); }

    /**
     * Changes the logical name of the clock. You can use yCheckLogicalName()
     * prior to this call to make sure that your parameter is valid.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     * 
     * @param newval : a string corresponding to the logical name of the clock
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_logicalName(const string& newval);
    inline int      setLogicalName(const string& newval)
    { return this->set_logicalName(newval); }

    /**
     * Returns the current value of the clock (no more than 6 characters).
     * 
     * @return a string corresponding to the current value of the clock (no more than 6 characters)
     * 
     * On failure, throws an exception or returns Y_ADVERTISEDVALUE_INVALID.
     */
           string          get_advertisedValue(void);
    inline string          advertisedValue(void)
    { return this->get_advertisedValue(); }

    /**
     * Returns the current time in Unix format (number of elapsed seconds since Jan 1st, 1970).
     * 
     * @return an integer corresponding to the current time in Unix format (number of elapsed seconds
     * since Jan 1st, 1970)
     * 
     * On failure, throws an exception or returns Y_UNIXTIME_INVALID.
     */
           unsigned        get_unixTime(void);
    inline unsigned        unixTime(void)
    { return this->get_unixTime(); }

    /**
     * Changes the current time. Time is specifid in Unix format (number of elapsed seconds since Jan 1st, 1970).
     * If current UTC time is known, utcOffset will be automatically adjusted for the new specified time.
     * 
     * @param newval : an integer corresponding to the current time
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_unixTime(unsigned newval);
    inline int      setUnixTime(unsigned newval)
    { return this->set_unixTime(newval); }

    /**
     * Returns the current time in the form "YYYY/MM/DD hh:mm:ss"
     * 
     * @return a string corresponding to the current time in the form "YYYY/MM/DD hh:mm:ss"
     * 
     * On failure, throws an exception or returns Y_DATETIME_INVALID.
     */
           string          get_dateTime(void);
    inline string          dateTime(void)
    { return this->get_dateTime(); }

    /**
     * Returns the number of seconds between current time and UTC time (time zone).
     * 
     * @return an integer corresponding to the number of seconds between current time and UTC time (time zone)
     * 
     * On failure, throws an exception or returns Y_UTCOFFSET_INVALID.
     */
           int             get_utcOffset(void);
    inline int             utcOffset(void)
    { return this->get_utcOffset(); }

    /**
     * Changes the number of seconds between current time and UTC time (time zone).
     * The timezone is automatically rounded to the nearest multiple of 15 minutes.
     * If current UTC time is known, the current time will automatically be updated according to the
     * selected time zone.
     * 
     * @param newval : an integer corresponding to the number of seconds between current time and UTC time (time zone)
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_utcOffset(int newval);
    inline int      setUtcOffset(int newval)
    { return this->set_utcOffset(newval); }

    /**
     * Returns true if the clock has been set, and false otherwise.
     * 
     * @return either Y_TIMESET_FALSE or Y_TIMESET_TRUE, according to true if the clock has been set, and
     * false otherwise
     * 
     * On failure, throws an exception or returns Y_TIMESET_INVALID.
     */
           Y_TIMESET_enum  get_timeSet(void);
    inline Y_TIMESET_enum  timeSet(void)
    { return this->get_timeSet(); }


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
    void registerValueCallback(YRealTimeClockUpdateCallback callback);

    void advertiseValue(const string& value);

    /**
     * Continues the enumeration of clocks started using yFirstRealTimeClock().
     * 
     * @return a pointer to a YRealTimeClock object, corresponding to
     *         a clock currently online, or a null pointer
     *         if there are no more clocks to enumerate.
     */
           YRealTimeClock  *nextRealTimeClock(void);
    inline YRealTimeClock  *next(void)
    { return this->nextRealTimeClock();}

    /**
     * Retrieves a clock for a given identifier.
     * The identifier can be specified using several formats:
     * <ul>
     * <li>FunctionLogicalName</li>
     * <li>ModuleSerialNumber.FunctionIdentifier</li>
     * <li>ModuleSerialNumber.FunctionLogicalName</li>
     * <li>ModuleLogicalName.FunctionIdentifier</li>
     * <li>ModuleLogicalName.FunctionLogicalName</li>
     * </ul>
     * 
     * This function does not require that the clock is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method YRealTimeClock.isOnline() to test if the clock is
     * indeed online at a given time. In case of ambiguity when looking for
     * a clock by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     * 
     * @param func : a string that uniquely characterizes the clock
     * 
     * @return a YRealTimeClock object allowing you to drive the clock.
     */
           static YRealTimeClock* FindRealTimeClock(const string& func);
    inline static YRealTimeClock* Find(const string& func)
    { return YRealTimeClock::FindRealTimeClock(func);}
    /**
     * Starts the enumeration of clocks currently accessible.
     * Use the method YRealTimeClock.nextRealTimeClock() to iterate on
     * next clocks.
     * 
     * @return a pointer to a YRealTimeClock object, corresponding to
     *         the first clock currently online, or a null pointer
     *         if there are none.
     */
           static YRealTimeClock* FirstRealTimeClock(void);
    inline static YRealTimeClock* First(void)
    { return YRealTimeClock::FirstRealTimeClock();}
    //--- (end of YRealTimeClock accessors declaration)
};

//--- (RealTimeClock functions declaration)

/**
 * Retrieves a clock for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 * 
 * This function does not require that the clock is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method YRealTimeClock.isOnline() to test if the clock is
 * indeed online at a given time. In case of ambiguity when looking for
 * a clock by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 * 
 * @param func : a string that uniquely characterizes the clock
 * 
 * @return a YRealTimeClock object allowing you to drive the clock.
 */
inline YRealTimeClock* yFindRealTimeClock(const string& func)
{ return YRealTimeClock::FindRealTimeClock(func);}
/**
 * Starts the enumeration of clocks currently accessible.
 * Use the method YRealTimeClock.nextRealTimeClock() to iterate on
 * next clocks.
 * 
 * @return a pointer to a YRealTimeClock object, corresponding to
 *         the first clock currently online, or a null pointer
 *         if there are none.
 */
inline YRealTimeClock* yFirstRealTimeClock(void)
{ return YRealTimeClock::FirstRealTimeClock();}

//--- (end of RealTimeClock functions declaration)

#endif
