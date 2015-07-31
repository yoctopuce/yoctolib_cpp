/*********************************************************************
 *
 * $Id: yocto_datalogger.h 20704 2015-06-20 19:43:34Z mvuilleu $
 *
 * Declares yFindDataLogger(), the high-level API for DataLogger functions
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
 *  THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
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


#ifndef YOCTO_DATALOGGER_H
#define YOCTO_DATALOGGER_H

#include "yocto_api.h"
#include <ctime>
#include <cfloat>
#include <cmath>
#include <map>

//--- (generated code: YDataLogger definitions)
class YDataLogger; // forward declaration

typedef void (*YDataLoggerValueCallback)(YDataLogger *func, const string& functionValue);
#ifndef _Y_RECORDING_ENUM
#define _Y_RECORDING_ENUM
typedef enum {
    Y_RECORDING_OFF = 0,
    Y_RECORDING_ON = 1,
    Y_RECORDING_PENDING = 2,
    Y_RECORDING_INVALID = -1,
} Y_RECORDING_enum;
#endif
#ifndef _Y_AUTOSTART_ENUM
#define _Y_AUTOSTART_ENUM
typedef enum {
    Y_AUTOSTART_OFF = 0,
    Y_AUTOSTART_ON = 1,
    Y_AUTOSTART_INVALID = -1,
} Y_AUTOSTART_enum;
#endif
#ifndef _Y_BEACONDRIVEN_ENUM
#define _Y_BEACONDRIVEN_ENUM
typedef enum {
    Y_BEACONDRIVEN_OFF = 0,
    Y_BEACONDRIVEN_ON = 1,
    Y_BEACONDRIVEN_INVALID = -1,
} Y_BEACONDRIVEN_enum;
#endif
#ifndef _Y_CLEARHISTORY_ENUM
#define _Y_CLEARHISTORY_ENUM
typedef enum {
    Y_CLEARHISTORY_FALSE = 0,
    Y_CLEARHISTORY_TRUE = 1,
    Y_CLEARHISTORY_INVALID = -1,
} Y_CLEARHISTORY_enum;
#endif
#define Y_CURRENTRUNINDEX_INVALID       (YAPI_INVALID_UINT)
#define Y_TIMEUTC_INVALID               (YAPI_INVALID_LONG)
//--- (end of generated code: YDataLogger definitions)

/**
 * YOldDataStream Class: Sequence of measured data, returned by the data logger
 *  
 * A data stream is a small collection of consecutive measures for a set
 * of sensors. A few properties are available directly from the object itself
 * (they are preloaded at instantiation time), while most other properties and
 * the actual data are loaded on demand when accessed for the first time.
 *
 * This is the old version of the YDataStream class, used for backward-compatibility
 * with devices with firmware < 13000
 */
class YOldDataStream: public YDataStream {
protected:
    // Data preloaded on object instantiation
    YDataLogger     *_dataLogger;
    unsigned        _timeStamp;
    unsigned        _interval;
    
public:    
    YOldDataStream(YDataLogger *parent, unsigned run, 
                   unsigned stamp, unsigned utc, unsigned itv);

    // override new version with backward-compatible code
    int loadStream(void);

    /**
     * Returns the relative start time of the data stream, measured in seconds.
     * For recent firmwares, the value is relative to the present time,
     * which means the value is always negative.
     * If the device uses a firmware older than version 13000, value is
     * relative to the start of the time the device was powered on, and
     * is always positive.
     * If you need an absolute UTC timestamp, use get_startTimeUTC().
     * 
     * @return an unsigned number corresponding to the number of seconds
     *         between the start of the run and the beginning of this data
     *         stream.
     */
           int          get_startTime(void);
    inline int          startTime(void)
    { return this->get_startTime(); }
    
    /**
     * Returns the number of seconds elapsed between  two consecutive
     * rows of this data stream. By default, the data logger records one row
     * per second, but there might be alternative streams at lower resolution
     * created by summarizing the original stream for archiving purposes.
     * 
     * This method does not cause any access to the device, as the value
     * is preloaded in the object at instantiation time.
     * 
     * @return an unsigned number corresponding to a number of seconds.
     */
           double       get_dataSamplesInterval(void);
    inline double       dataSamplesInterval(void)
    { return this->get_dataSamplesInterval(); }
};

//--- (generated code: YDataLogger declaration)
/**
 * YDataLogger Class: DataLogger function interface
 *
 * Yoctopuce sensors include a non-volatile memory capable of storing ongoing measured
 * data automatically, without requiring a permanent connection to a computer.
 * The DataLogger function controls the global parameters of the internal data
 * logger.
 */
class YOCTO_CLASS_EXPORT YDataLogger: public YFunction {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of generated code: YDataLogger declaration)
    //--- (generated code: YDataLogger attributes)
    // Attributes (function value cache)
    int             _currentRunIndex;
    s64             _timeUTC;
    Y_RECORDING_enum _recording;
    Y_AUTOSTART_enum _autoStart;
    Y_BEACONDRIVEN_enum _beaconDriven;
    Y_CLEARHISTORY_enum _clearHistory;
    YDataLoggerValueCallback _valueCallbackDataLogger;

    friend YDataLogger *yFindDataLogger(const string& func);
    friend YDataLogger *yFirstDataLogger(void);

    // Function-specific method for parsing of JSON output and caching result
    virtual int     _parseAttr(yJsonStateMachine& j);

    // Constructor is protected, use yFindDataLogger factory function to instantiate
    YDataLogger(const string& func);
    //--- (end of generated code: YDataLogger attributes)
    //--- (generated code: DataLogger initialization)
    //--- (end of generated code: DataLogger initialization)

    // device-specific URL to access the datalogger
    string          dataLoggerURL;
    
    // DataLogger-specific method to retrieve and pre-parse recorded data
    int             getData(unsigned runIdx, unsigned timeIdx, string &buffer, yJsonStateMachine &j);
    
    // YOldDataStream loadStream() will use our method getData()
    friend int YOldDataStream::loadStream(void);

public:
    ~YDataLogger();
    
    /**
     * Builds a list of all data streams hold by the data logger (legacy method).
     * The caller must pass by reference an empty array to hold YDataStream
     * objects, and the function fills it with objects describing available
     * data sequences.
     *
     * This is the old way to retrieve data from the DataLogger.
     * For new applications, you should rather use get_dataSets()
     * method, or call directly get_recordedData() on the
     * sensor object.
     *
     * @param v : an array of YDataStream objects to be filled in
     *
     * @return YAPI_SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             get_dataStreams(vector<YDataStream *>& v);
    inline int dataStreams(vector<YDataStream *>& v)
    { return this->get_dataStreams(v);}
    
    
    //--- (generated code: YDataLogger accessors declaration)

    static const int CURRENTRUNINDEX_INVALID = YAPI_INVALID_UINT;
    static const s64 TIMEUTC_INVALID = YAPI_INVALID_LONG;
    static const Y_RECORDING_enum RECORDING_OFF = Y_RECORDING_OFF;
    static const Y_RECORDING_enum RECORDING_ON = Y_RECORDING_ON;
    static const Y_RECORDING_enum RECORDING_PENDING = Y_RECORDING_PENDING;
    static const Y_RECORDING_enum RECORDING_INVALID = Y_RECORDING_INVALID;
    static const Y_AUTOSTART_enum AUTOSTART_OFF = Y_AUTOSTART_OFF;
    static const Y_AUTOSTART_enum AUTOSTART_ON = Y_AUTOSTART_ON;
    static const Y_AUTOSTART_enum AUTOSTART_INVALID = Y_AUTOSTART_INVALID;
    static const Y_BEACONDRIVEN_enum BEACONDRIVEN_OFF = Y_BEACONDRIVEN_OFF;
    static const Y_BEACONDRIVEN_enum BEACONDRIVEN_ON = Y_BEACONDRIVEN_ON;
    static const Y_BEACONDRIVEN_enum BEACONDRIVEN_INVALID = Y_BEACONDRIVEN_INVALID;
    static const Y_CLEARHISTORY_enum CLEARHISTORY_FALSE = Y_CLEARHISTORY_FALSE;
    static const Y_CLEARHISTORY_enum CLEARHISTORY_TRUE = Y_CLEARHISTORY_TRUE;
    static const Y_CLEARHISTORY_enum CLEARHISTORY_INVALID = Y_CLEARHISTORY_INVALID;

    /**
     * Returns the current run number, corresponding to the number of times the module was
     * powered on with the dataLogger enabled at some point.
     *
     * @return an integer corresponding to the current run number, corresponding to the number of times the module was
     *         powered on with the dataLogger enabled at some point
     *
     * On failure, throws an exception or returns Y_CURRENTRUNINDEX_INVALID.
     */
    int                 get_currentRunIndex(void);

    inline int          currentRunIndex(void)
    { return this->get_currentRunIndex(); }

    /**
     * Returns the Unix timestamp for current UTC time, if known.
     *
     * @return an integer corresponding to the Unix timestamp for current UTC time, if known
     *
     * On failure, throws an exception or returns Y_TIMEUTC_INVALID.
     */
    s64                 get_timeUTC(void);

    inline s64          timeUTC(void)
    { return this->get_timeUTC(); }

    /**
     * Changes the current UTC time reference used for recorded data.
     *
     * @param newval : an integer corresponding to the current UTC time reference used for recorded data
     *
     * @return YAPI_SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_timeUTC(s64 newval);
    inline int      setTimeUTC(s64 newval)
    { return this->set_timeUTC(newval); }

    /**
     * Returns the current activation state of the data logger.
     *
     * @return a value among Y_RECORDING_OFF, Y_RECORDING_ON and Y_RECORDING_PENDING corresponding to the
     * current activation state of the data logger
     *
     * On failure, throws an exception or returns Y_RECORDING_INVALID.
     */
    Y_RECORDING_enum    get_recording(void);

    inline Y_RECORDING_enum recording(void)
    { return this->get_recording(); }

    /**
     * Changes the activation state of the data logger to start/stop recording data.
     *
     * @param newval : a value among Y_RECORDING_OFF, Y_RECORDING_ON and Y_RECORDING_PENDING corresponding
     * to the activation state of the data logger to start/stop recording data
     *
     * @return YAPI_SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_recording(Y_RECORDING_enum newval);
    inline int      setRecording(Y_RECORDING_enum newval)
    { return this->set_recording(newval); }

    /**
     * Returns the default activation state of the data logger on power up.
     *
     * @return either Y_AUTOSTART_OFF or Y_AUTOSTART_ON, according to the default activation state of the
     * data logger on power up
     *
     * On failure, throws an exception or returns Y_AUTOSTART_INVALID.
     */
    Y_AUTOSTART_enum    get_autoStart(void);

    inline Y_AUTOSTART_enum autoStart(void)
    { return this->get_autoStart(); }

    /**
     * Changes the default activation state of the data logger on power up.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     *
     * @param newval : either Y_AUTOSTART_OFF or Y_AUTOSTART_ON, according to the default activation state
     * of the data logger on power up
     *
     * @return YAPI_SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_autoStart(Y_AUTOSTART_enum newval);
    inline int      setAutoStart(Y_AUTOSTART_enum newval)
    { return this->set_autoStart(newval); }

    /**
     * Return true if the data logger is synchronised with the localization beacon.
     *
     * @return either Y_BEACONDRIVEN_OFF or Y_BEACONDRIVEN_ON
     *
     * On failure, throws an exception or returns Y_BEACONDRIVEN_INVALID.
     */
    Y_BEACONDRIVEN_enum get_beaconDriven(void);

    inline Y_BEACONDRIVEN_enum beaconDriven(void)
    { return this->get_beaconDriven(); }

    /**
     * Changes the type of synchronisation of the data logger.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     *
     * @param newval : either Y_BEACONDRIVEN_OFF or Y_BEACONDRIVEN_ON, according to the type of
     * synchronisation of the data logger
     *
     * @return YAPI_SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_beaconDriven(Y_BEACONDRIVEN_enum newval);
    inline int      setBeaconDriven(Y_BEACONDRIVEN_enum newval)
    { return this->set_beaconDriven(newval); }

    Y_CLEARHISTORY_enum get_clearHistory(void);

    inline Y_CLEARHISTORY_enum clearHistory(void)
    { return this->get_clearHistory(); }

    int             set_clearHistory(Y_CLEARHISTORY_enum newval);
    inline int      setClearHistory(Y_CLEARHISTORY_enum newval)
    { return this->set_clearHistory(newval); }

    /**
     * Retrieves a data logger for a given identifier.
     * The identifier can be specified using several formats:
     * <ul>
     * <li>FunctionLogicalName</li>
     * <li>ModuleSerialNumber.FunctionIdentifier</li>
     * <li>ModuleSerialNumber.FunctionLogicalName</li>
     * <li>ModuleLogicalName.FunctionIdentifier</li>
     * <li>ModuleLogicalName.FunctionLogicalName</li>
     * </ul>
     *
     * This function does not require that the data logger is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method YDataLogger.isOnline() to test if the data logger is
     * indeed online at a given time. In case of ambiguity when looking for
     * a data logger by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     *
     * @param func : a string that uniquely characterizes the data logger
     *
     * @return a YDataLogger object allowing you to drive the data logger.
     */
    static YDataLogger* FindDataLogger(string func);

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
    virtual int         registerValueCallback(YDataLoggerValueCallback callback);
    using YFunction::registerValueCallback;

    virtual int         _invokeValueCallback(string value);

    /**
     * Clears the data logger memory and discards all recorded data streams.
     * This method also resets the current run index to zero.
     *
     * @return YAPI_SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         forgetAllDataStreams(void);

    /**
     * Returns a list of YDataSet objects that can be used to retrieve
     * all measures stored by the data logger.
     *
     * This function only works if the device uses a recent firmware,
     * as YDataSet objects are not supported by firmwares older than
     * version 13000.
     *
     * @return a list of YDataSet object.
     *
     * On failure, throws an exception or returns an empty list.
     */
    virtual vector<YDataSet> get_dataSets(void);

    virtual vector<YDataSet> parse_dataSets(string json);


    inline static YDataLogger* Find(string func)
    { return YDataLogger::FindDataLogger(func); }

    /**
     * Continues the enumeration of data loggers started using yFirstDataLogger().
     *
     * @return a pointer to a YDataLogger object, corresponding to
     *         a data logger currently online, or a null pointer
     *         if there are no more data loggers to enumerate.
     */
           YDataLogger     *nextDataLogger(void);
    inline YDataLogger     *next(void)
    { return this->nextDataLogger();}

    /**
     * Starts the enumeration of data loggers currently accessible.
     * Use the method YDataLogger.nextDataLogger() to iterate on
     * next data loggers.
     *
     * @return a pointer to a YDataLogger object, corresponding to
     *         the first data logger currently online, or a null pointer
     *         if there are none.
     */
           static YDataLogger* FirstDataLogger(void);
    inline static YDataLogger* First(void)
    { return YDataLogger::FirstDataLogger();}
#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of generated code: YDataLogger accessors declaration)
};


//--- (generated code: DataLogger functions declaration)

/**
 * Retrieves a data logger for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that the data logger is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method YDataLogger.isOnline() to test if the data logger is
 * indeed online at a given time. In case of ambiguity when looking for
 * a data logger by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * @param func : a string that uniquely characterizes the data logger
 *
 * @return a YDataLogger object allowing you to drive the data logger.
 */
inline YDataLogger* yFindDataLogger(const string& func)
{ return YDataLogger::FindDataLogger(func);}
/**
 * Starts the enumeration of data loggers currently accessible.
 * Use the method YDataLogger.nextDataLogger() to iterate on
 * next data loggers.
 *
 * @return a pointer to a YDataLogger object, corresponding to
 *         the first data logger currently online, or a null pointer
 *         if there are none.
 */
inline YDataLogger* yFirstDataLogger(void)
{ return YDataLogger::FirstDataLogger();}

//--- (end of generated code: DataLogger functions declaration)

#endif
