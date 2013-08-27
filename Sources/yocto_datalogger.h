/*********************************************************************
 *
 * $Id: yocto_datalogger.h 12326 2013-08-13 15:52:20Z mvuilleu $
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

class YDataLogger;
//--- (generated code: definitions)
class YDataLogger; //forward declaration

typedef void (*YDataLoggerUpdateCallback)(YDataLogger *func, const string& functionValue);
typedef enum {
    Y_RECORDING_OFF = 0,
    Y_RECORDING_ON = 1,
    Y_RECORDING_INVALID = -1,
} Y_RECORDING_enum;

typedef enum {
    Y_AUTOSTART_OFF = 0,
    Y_AUTOSTART_ON = 1,
    Y_AUTOSTART_INVALID = -1,
} Y_AUTOSTART_enum;

typedef enum {
    Y_CLEARHISTORY_FALSE = 0,
    Y_CLEARHISTORY_TRUE = 1,
    Y_CLEARHISTORY_INVALID = -1,
} Y_CLEARHISTORY_enum;

#define Y_LOGICALNAME_INVALID           (YAPI::INVALID_STRING)
#define Y_ADVERTISEDVALUE_INVALID       (YAPI::INVALID_STRING)
#define Y_OLDESTRUNINDEX_INVALID        (0xffffffff)
#define Y_CURRENTRUNINDEX_INVALID       (0xffffffff)
#define Y_SAMPLINGINTERVAL_INVALID      (0xffffffff)
#define Y_TIMEUTC_INVALID               (0xffffffff)
//--- (end of generated code: definitions)

#define Y_DATA_INVALID                  (-DBL_MAX)

// Forward-declaration
class YDataLogger;

/**
 * YDataStream Class: Sequence of measured data, returned by the data logger
 * 
 * A data stream is a small collection of consecutive measures for a set
 * of sensors. A few properties are available directly from the object itself
 * (they are preloaded at instantiation time), while most other properties and
 * the actual data are loaded on demand when accessed for the first time.
 */
class YDataStream {
protected:
    // Data preloaded on object instantiation
    YDataLogger     *dataLogger;
    unsigned        _runNo, _timeStamp, _interval;
    time_t          _utcStamp;
    
    // Data loaded using a specific connection
    unsigned        _nRows, _nCols;
    vector<string>  _columnNames;
    vector< vector<double> > _values;

public:    
    YDataStream(YDataLogger *parent, unsigned run, 
                unsigned stamp, unsigned utc, unsigned itv): 
        dataLogger(parent), _runNo(run), _timeStamp(stamp),  
        _interval(itv), _utcStamp(utc), _nRows(0), _nCols(0) {};
    
    static const double DATA_INVALID;

    
    // Preload all values into object
    int             loadStream(void);
    
    /**
     * Returns the run index of the data stream. A run can be made of
     * multiple datastreams, for different time intervals.
     * 
     * This method does not cause any access to the device, as the value
     * is preloaded in the object at instantiation time.
     * 
     * @return an unsigned number corresponding to the run index.
     */
           unsigned        get_runIndex(void);
    inline unsigned        runIndex(void)
    { return this->get_runIndex(); }

    /**
     * Returns the start time of the data stream, relative to the beginning
     * of the run. If you need an absolute time, use get_startTimeUTC().
     * 
     * This method does not cause any access to the device, as the value
     * is preloaded in the object at instantiation time.
     * 
     * @return an unsigned number corresponding to the number of seconds
     *         between the start of the run and the beginning of this data
     *         stream.
     */
           unsigned        get_startTime(void);
    inline unsigned        startTime(void)
    { return this->get_startTime(); }

    /**
     * Returns the start time of the data stream, relative to the Jan 1, 1970.
     * If the UTC time was not set in the datalogger at the time of the recording
     * of this data stream, this method returns 0.
     * 
     * This method does not cause any access to the device, as the value
     * is preloaded in the object at instantiation time.
     * 
     * @return an unsigned number corresponding to the number of seconds
     *         between the Jan 1, 1970 and the beginning of this data
     *         stream (i.e. Unix time representation of the absolute time).
     */
           const time_t    &get_startTimeUTC(void);
    inline const time_t    &startTimeUTC(void)
    { return this->get_startTimeUTC(); }

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
           unsigned        get_dataSamplesInterval(void);
    inline unsigned        dataSamplesInterval(void)
    { return this->get_dataSamplesInterval(); }

    /**
     * Returns the number of data rows present in this stream.
     * 
     * This method fetches the whole data stream from the device,
     * if not yet done.
     * 
     * @return an unsigned number corresponding to the number of rows.
     * 
     * On failure, throws an exception or returns zero.
     */
            unsigned        get_rowCount(void);
    inline  unsigned        rowCount(void)
    { return this->get_rowCount(); }

    
    /**
     * Returns the number of data columns present in this stream.
     * The meaning of the values present in each column can be obtained
     * using the method get_columnNames().
     * 
     * This method fetches the whole data stream from the device,
     * if not yet done.
     * 
     * @return an unsigned number corresponding to the number of rows.
     * 
     * On failure, throws an exception or returns zero.
     */
           unsigned        get_columnCount(void);
    inline unsigned        columnCount(void)
    { return this->get_columnCount(); }

    /**
     * Returns the title (or meaning) of each data column present in this stream.
     * In most case, the title of the data column is the hardware identifier
     * of the sensor that produced the data. For archived streams created by
     * summarizing a high-resolution data stream, there can be a suffix appended
     * to the sensor identifier, such as _min for the minimum value, _avg for the
     * average value and _max for the maximal value.
     * 
     * This method fetches the whole data stream from the device,
     * if not yet done.
     * 
     * @return a list containing as many strings as there are columns in the
     *         data stream.
     * 
     * On failure, throws an exception or returns an empty array.
     */
           const vector<string>& get_columnNames(void);
    inline const vector<string>& columnNames(void)
    { return this->get_columnNames(); }

    /**
     * Returns the whole data set contained in the stream, as a bidimensional
     * table of numbers.
     * The meaning of the values present in each column can be obtained
     * using the method get_columnNames().
     * 
     * This method fetches the whole data stream from the device,
     * if not yet done.
     * 
     * @return a list containing as many elements as there are rows in the
     *         data stream. Each row itself is a list of floating-point
     *         numbers.
     * 
     * On failure, throws an exception or returns an empty array.
     */
           const vector< vector<double> >& get_dataRows(void);
    inline const vector< vector<double> >& dataRows(void)
    { return this->get_dataRows(); }
    
    /**
     * Returns a single measure from the data stream, specified by its
     * row and column index.
     * The meaning of the values present in each column can be obtained
     * using the method get_columnNames().
     * 
     * This method fetches the whole data stream from the device,
     * if not yet done.
     * 
     * @param row : row index
     * @param col : column index
     * 
     * @return a floating-point number
     * 
     * On failure, throws an exception or returns Y_DATA_INVALID.
     */
           double get_data(unsigned row, unsigned col);
    inline double data(unsigned row, unsigned col)
    { return this->get_data(row,col); }
};

//--- (generated code: YDataLogger declaration)
/**
 * YDataLogger Class: DataLogger function interface
 * 
 * Yoctopuce sensors include a non-volatile memory capable of storing ongoing measured
 * data automatically, without requiring a permanent connection to a computer.
 * The Yoctopuce application programming interface includes functions to control
 * how this internal data logger works.
 * Beacause the sensors do not include a battery, they do not have an absolute time
 * reference. Therefore, measures are simply indexed by the absolute run number
 * and time relative to the start of the run. Every new power up starts a new run.
 * It is however possible to setup an absolute UTC time by software at a given time,
 * so that the data logger keeps track of it until it is next powered off.
 */
class YDataLogger: public YFunction {
protected:
    // Attributes (function value cache)
    YDataLoggerUpdateCallback _callback;
    string          _logicalName;
    string          _advertisedValue;
    unsigned        _oldestRunIndex;
    unsigned        _currentRunIndex;
    unsigned        _samplingInterval;
    unsigned        _timeUTC;
    Y_RECORDING_enum _recording;
    Y_AUTOSTART_enum _autoStart;
    Y_CLEARHISTORY_enum _clearHistory;

    friend YDataLogger *yFindDataLogger(const string& func);
    friend YDataLogger *yFirstDataLogger(void);

    // Function-specific method for parsing of JSON output and caching result
    int             _parse(yJsonStateMachine& j);
    //--- (end of generated code: YDataLogger declaration)
    //--- (generated code: YDataLogger constructor)
    // Constructor is protected, use yFindDataLogger factory function to instantiate
    YDataLogger(const string& func);
    //--- (end of generated code: YDataLogger constructor)
    //--- (generated code: DataLogger initialization)
    //--- (end of generated code: DataLogger initialization)

    // device-specific URL to access the datalogger
    string          dataLoggerURL;
    
    // DataLogger-specific method to retrieve and pre-parse recorded data
    int             getData(unsigned runIdx, unsigned timeIdx, string &buffer, yJsonStateMachine &j);
    
    // YDataStream loadStream() will use our method getData()
    friend int YDataStream::loadStream(void);

public:
    ~YDataLogger();
    
    /**
     * Clears the data logger memory and discards all recorded data streams.
     * This method also resets the current run index to zero.
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             forgetAllDataStreams(void);
    
    /**
     * Builds a list of all data streams hold by the data logger.
     * The caller must pass by reference an empty array to hold YDataStream
     * objects, and the function fills it with objects describing available
     * data sequences.
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

    static const string LOGICALNAME_INVALID;
    static const string ADVERTISEDVALUE_INVALID;
    static const unsigned OLDESTRUNINDEX_INVALID = 0xffffffff;
    static const unsigned CURRENTRUNINDEX_INVALID = 0xffffffff;
    static const unsigned SAMPLINGINTERVAL_INVALID = 0xffffffff;
    static const unsigned TIMEUTC_INVALID = 0xffffffff;
    static const Y_RECORDING_enum RECORDING_OFF = Y_RECORDING_OFF;
    static const Y_RECORDING_enum RECORDING_ON = Y_RECORDING_ON;
    static const Y_RECORDING_enum RECORDING_INVALID = Y_RECORDING_INVALID;
    static const Y_AUTOSTART_enum AUTOSTART_OFF = Y_AUTOSTART_OFF;
    static const Y_AUTOSTART_enum AUTOSTART_ON = Y_AUTOSTART_ON;
    static const Y_AUTOSTART_enum AUTOSTART_INVALID = Y_AUTOSTART_INVALID;
    static const Y_CLEARHISTORY_enum CLEARHISTORY_FALSE = Y_CLEARHISTORY_FALSE;
    static const Y_CLEARHISTORY_enum CLEARHISTORY_TRUE = Y_CLEARHISTORY_TRUE;
    static const Y_CLEARHISTORY_enum CLEARHISTORY_INVALID = Y_CLEARHISTORY_INVALID;

    /**
     * Returns the logical name of the data logger.
     * 
     * @return a string corresponding to the logical name of the data logger
     * 
     * On failure, throws an exception or returns Y_LOGICALNAME_INVALID.
     */
           string          get_logicalName(void);
    inline string          logicalName(void)
    { return this->get_logicalName(); }

    /**
     * Changes the logical name of the data logger. You can use yCheckLogicalName()
     * prior to this call to make sure that your parameter is valid.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     * 
     * @param newval : a string corresponding to the logical name of the data logger
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_logicalName(const string& newval);
    inline int      setLogicalName(const string& newval)
    { return this->set_logicalName(newval); }

    /**
     * Returns the current value of the data logger (no more than 6 characters).
     * 
     * @return a string corresponding to the current value of the data logger (no more than 6 characters)
     * 
     * On failure, throws an exception or returns Y_ADVERTISEDVALUE_INVALID.
     */
           string          get_advertisedValue(void);
    inline string          advertisedValue(void)
    { return this->get_advertisedValue(); }

    /**
     * Returns the index of the oldest run for which the non-volatile memory still holds recorded data.
     * 
     * @return an integer corresponding to the index of the oldest run for which the non-volatile memory
     * still holds recorded data
     * 
     * On failure, throws an exception or returns Y_OLDESTRUNINDEX_INVALID.
     */
           unsigned        get_oldestRunIndex(void);
    inline unsigned        oldestRunIndex(void)
    { return this->get_oldestRunIndex(); }

    /**
     * Returns the current run number, corresponding to the number of times the module was
     * powered on with the dataLogger enabled at some point.
     * 
     * @return an integer corresponding to the current run number, corresponding to the number of times the module was
     *         powered on with the dataLogger enabled at some point
     * 
     * On failure, throws an exception or returns Y_CURRENTRUNINDEX_INVALID.
     */
           unsigned        get_currentRunIndex(void);
    inline unsigned        currentRunIndex(void)
    { return this->get_currentRunIndex(); }

           unsigned        get_samplingInterval(void);
    inline unsigned        samplingInterval(void)
    { return this->get_samplingInterval(); }

    int             set_samplingInterval(unsigned newval);
    inline int      setSamplingInterval(unsigned newval)
    { return this->set_samplingInterval(newval); }

    /**
     * Returns the Unix timestamp for current UTC time, if known.
     * 
     * @return an integer corresponding to the Unix timestamp for current UTC time, if known
     * 
     * On failure, throws an exception or returns Y_TIMEUTC_INVALID.
     */
           unsigned        get_timeUTC(void);
    inline unsigned        timeUTC(void)
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
    int             set_timeUTC(unsigned newval);
    inline int      setTimeUTC(unsigned newval)
    { return this->set_timeUTC(newval); }

    /**
     * Returns the current activation state of the data logger.
     * 
     * @return either Y_RECORDING_OFF or Y_RECORDING_ON, according to the current activation state of the data logger
     * 
     * On failure, throws an exception or returns Y_RECORDING_INVALID.
     */
           Y_RECORDING_enum get_recording(void);
    inline Y_RECORDING_enum recording(void)
    { return this->get_recording(); }

    /**
     * Changes the activation state of the data logger to start/stop recording data.
     * 
     * @param newval : either Y_RECORDING_OFF or Y_RECORDING_ON, according to the activation state of the
     * data logger to start/stop recording data
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
           Y_AUTOSTART_enum get_autoStart(void);
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

           Y_CLEARHISTORY_enum get_clearHistory(void);
    inline Y_CLEARHISTORY_enum clearHistory(void)
    { return this->get_clearHistory(); }

    int             set_clearHistory(Y_CLEARHISTORY_enum newval);
    inline int      setClearHistory(Y_CLEARHISTORY_enum newval)
    { return this->set_clearHistory(newval); }


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
    void registerValueCallback(YDataLoggerUpdateCallback callback);

    void advertiseValue(const string& value);

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
           static YDataLogger* FindDataLogger(const string& func);
    inline static YDataLogger* Find(const string& func)
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
           static YDataLogger* FirstDataLogger(void);
    inline static YDataLogger* First(void)
    { return YDataLogger::FirstDataLogger();}
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
