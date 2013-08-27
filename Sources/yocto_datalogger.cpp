/*********************************************************************
 *
 * $Id: yocto_datalogger.cpp 12326 2013-08-13 15:52:20Z mvuilleu $
 *
 * Implements yFindDataLogger(), the high-level API for DataLogger functions
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


#define _CRT_SECURE_NO_DEPRECATE //do not use windows secure crt
#include "yocto_datalogger.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


const double DATA_INVALID = Y_DATA_INVALID;


// Preload all values into the data stream object
int YDataStream::loadStream(void)
{
    string              buffer;
    yJsonStateMachine   j;
    int                 res, ival;
    double              fval;
    unsigned            p, c;
    vector<int>         coldiv, coltyp;
    vector<double>      colscl;
    vector<int>         colofs;
    vector<yCalibrationHandler> calhdl;
    vector<int>         caltyp;
    vector<intArr>      calpar;
    vector<floatArr>    calraw;
    vector<floatArr>    calref;
    
    vector<unsigned>    udat;
    vector<double>      dat;
    
    if((res = dataLogger->getData(_runNo, _timeStamp, buffer, j)) != YAPI_SUCCESS) {
        return res;
    }
    if(yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_STRUCT) {
    fail:
        dataLogger->_throw(YAPI_IO_ERROR, "Unexpected JSON reply format");
        return YAPI_IO_ERROR;
    }
    _nRows = _nCols = 0;
    _columnNames.clear();
    _values.clear();
    while(yJsonParse(&j) == YJSON_PARSE_AVAIL && j.st == YJSON_PARSE_MEMBNAME) {
        if(!strcmp(j.token, "time")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto fail;
            _timeStamp = atoi(j.token); 
        } else if(!strcmp(j.token, "UTC")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto fail;
            _utcStamp = atoi(j.token); 
        } else if(!strcmp(j.token, "interval")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto fail;
            _interval = atoi(j.token); 
        } else if(!strcmp(j.token, "nRows")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto fail;
            _nRows = atoi(j.token); 
        } else if(!strcmp(j.token, "keys")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_ARRAY) break;
            while(yJsonParse(&j) == YJSON_PARSE_AVAIL && j.st == YJSON_PARSE_STRING) {
                _columnNames.push_back(dataLogger->_parseString(j));
            }
            if(j.token[0] != ']') goto fail;
            if(_nCols == 0) {
                _nCols = (unsigned)_columnNames.size();
            } else if(_nCols != _columnNames.size()) {
                _nCols = 0; 
                goto fail;
            }
        } else if(!strcmp(j.token, "div")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_ARRAY) break;
            while(yJsonParse(&j) == YJSON_PARSE_AVAIL && j.st == YJSON_PARSE_NUM) {
                coldiv.push_back(atoi(j.token));
            }
            if(j.token[0] != ']') goto fail;
            if(_nCols == 0) {
                _nCols = (unsigned)coldiv.size();
            } else if(_nCols != coldiv.size()) {
                _nCols = 0; 
                goto fail;
            }
        } else if(!strcmp(j.token, "type")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_ARRAY) break;
            while(yJsonParse(&j) == YJSON_PARSE_AVAIL && j.st == YJSON_PARSE_NUM) {
                coltyp.push_back(atoi(j.token));
            }
            if(j.token[0] != ']') goto fail;
            if(_nCols == 0) {
                _nCols = (unsigned)coltyp.size();
            } else if(_nCols != coltyp.size()) {
                _nCols = 0; 
                goto fail;
            }
        } else if(!strcmp(j.token, "scal")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_ARRAY) break;
            c = 0;
            while(yJsonParse(&j) == YJSON_PARSE_AVAIL && j.st == YJSON_PARSE_NUM) {
                colscl.push_back((double)atoi(j.token) / 65536.0);
                colofs.push_back(coltyp[c++] != 0 ? -32767 : 0);
            }
            if(j.token[0] != ']') goto fail;
            if(_nCols == 0) {
                _nCols = (unsigned)colscl.size();
            } else if(_nCols != colscl.size()) {
                _nCols = 0; 
                goto fail;
            }
        } else if(!strcmp(j.token, "cal")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_ARRAY) break;
            calhdl = vector<yCalibrationHandler>(_nCols, NULL);
            caltyp = vector<int>(_nCols, 0);
            calpar = vector<intArr>(_nCols, intArr());
            calraw = vector<floatArr>(_nCols, floatArr());
            calref = vector<floatArr>(_nCols, floatArr());
            c = 0;
            while(yJsonParse(&j) == YJSON_PARSE_AVAIL && j.st == YJSON_PARSE_STRING) {
                char *p = j.token;
                int  calibType = (*p ? atoi(p) : 0);
                calhdl[c] = YAPI::_getCalibrationHandler(calibType);
                if(!calhdl[c]) {
                    caltyp[c] = 0;
                    continue;
                }
                caltyp[c] = calibType;
                while(*p && *p != ',') p++;
                while(*p) {
                    p++; // skip ','
                    ival = atoi(p);
                    if(calibType <= 10) {
                        fval = (double)(ival + colofs[c]) / coldiv[c];
                    } else {
                        fval = YAPI::_decimalToDouble(ival);
                    }
                    calpar[c].push_back(ival);
                    while(*p && *p != ',') p++;
                    if(calpar[c].size() & 1) {
                        calraw[c].push_back(fval);
                    } else {
                        calref[c].push_back(fval);
                    }
                }
                c++;
            }
            if(j.token[0] != ']') goto fail;
        } else if(!strcmp(j.token, "data")) {
            if(colscl.size() == 0) {
                for(p = 0; p < coldiv.size(); p++) {
                    colscl.push_back(1.0 / (double)coldiv[p]);
                    colofs.push_back(coltyp[p] != 0 ? -32767 : 0);
                }
            }
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) break;
            udat.clear();
            if(j.st == YJSON_PARSE_STRING) {
                string sdat = dataLogger->_parseString(j);
                for(p = 0; p < sdat.size();) {
                    unsigned val;
                    unsigned c = sdat[p++];
                    if(c >= 'a') {
                        int srcpos = (int)udat.size()-1-(c-'a');
                        if(srcpos < 0) goto fail;
                        val = udat[srcpos];
                    } else {
                        if(p+2 > sdat.size()) goto fail;
                        val = (c - '0');
                        c = sdat[p++];
                        val += (c - '0') << 5;
                        c = sdat[p++];
                        if(c == 'z') c = '\\';
                        val += (c - '0') << 10;
                    }
                    udat.push_back(val);
                }
            } else if(j.st == YJSON_PARSE_ARRAY) {
                while(yJsonParse(&j) == YJSON_PARSE_AVAIL && j.st == YJSON_PARSE_NUM) {
                    udat.push_back(atoi(j.token));
                }
                if(j.token[0] != ']' ) goto fail;
            } else {
                goto fail;
            }
            dat.clear();
            c = 0;
            for(p = 0; p < udat.size(); p++) {
                double val;
                if(coltyp[c] < 2) {
                    val = (udat[p] + colofs[c]) * colscl[c];
                } else {
                    val = YAPI::_decimalToDouble((int)udat[p]-32767);
                }
                if(caltyp[c] > 0 && calhdl.size() >= c && calhdl[c]) {                    
                    if(caltyp[c] <= 10) {
                        // linear calibration using unscaled value
                        val = calhdl[c]((udat[p] + colofs[c]) / coldiv[c], caltyp[c], calpar[c], calraw[c], calref[c]);
                    } else if(caltyp[c] > 20) {
                        // custom calibration function: floating-point value is uncalibrated in the datalogger
                        val = calhdl[c](val, caltyp[c], calpar[c], calraw[c], calref[c]);
                    }
                }
                dat.push_back(val);
                if(++c == _nCols) {
                    _values.push_back(dat);
                    dat.clear();
                    c = 0;
                }                
            }
            if(dat.size() > 0) goto fail;
        } else {
            // ignore unknown field
            yJsonSkip(&j, 1);
        }
    }
    
    return YAPI_SUCCESS;
}

/**
 * Returns the run index of the data stream. A run can be made of
 * multiple datastreams, for different time intervals.
 *
 * This method does not cause any access to the device, as the value
 * is preloaded in the object at instantiation time.
 * 
 * @return an unsigned number corresponding to the run index.
 */
unsigned YDataStream::get_runIndex(void)
{
    return _runNo;
}

/**
 * Returns the start time of the data stream, relative to the beginning
 * of the run. If you want need an absolute time, use get_startTimeUTC().
 *
 * This method does not cause any access to the device, as the value
 * is preloaded in the object at instantiation time.
 * 
 * @return an unsigned number corresponding to the number of seconds
 *         between the start of the run and the beginning of this data
 *         stream.
 */
unsigned YDataStream::get_startTime(void)
{
    return _timeStamp;
}

/**
 * Returns the start time of the data stream, relative to the Jan 1, 1970.
 * If the UTC time was not set in the datalogger at the time of the recording
 * of this data stream, this method will return 0.
 *
 * This method does not cause any access to the device, as the value
 * is preloaded in the object at instantiation time.
 * 
 * @return an unsigned number corresponding to the number of seconds
 *         between the Jan 1, 1970 and the beginning of this data
 *         stream (i.e. Unix time representation of the absolute time).
 */
const time_t  &YDataStream::get_startTimeUTC(void)
{
    return _utcStamp;
}

/**
 * Returns the number of seconds elapsed between the every two consecutive 
 * rows of this data stream. By default, the data logger records one row
 * per second, but there might be alternate streams at lower resolution
 * created for archiving purpose by summarizing the original stream.
 *
 * This method does not cause any access to the device, as the value
 * is preloaded in the object at instantiation time.
 * 
 * @return an unsigned number corresponding to a number of seconds.
 */
unsigned YDataStream::get_dataSamplesInterval(void)
{
    return _interval;
}

/**
 * Returns the number of data rows present in this stream.
 *
 * This method will cause fetching the whole data stream from the device,
 * if not yet done.
 * 
 * @return an unsigned number corresponding to the number of rows.
 * 
 * On failure, throws an exception or returns zero.
 */
unsigned YDataStream::get_rowCount(void)
{
    if(_nRows == 0) loadStream();
    return _nRows;
}

/**
 * Returns the number of data columns present in this stream.
 * The meaning of the values present in each column can be obtained
 * using the method get_columnNames().
 *
 * This method will cause fetching the whole data stream from the device,
 * if not yet done.
 * 
 * @return an unsigned number corresponding to the number of rows.
 * 
 * On failure, throws an exception or returns zero.
 */
unsigned YDataStream::get_columnCount(void)
{
    if(_nCols == 0) loadStream();
    return _nCols;
}

/**
 * Returns title (or meaning) of each data columns present in this stream.
 * In most case, the title of the data column is the hardware identifier 
 * of the sensor that produced the data. For archive streams created by
 * summarizing a high-resolution data stream, there can be a suffix appended
 * to the sensor identifier, such as _min for the minimum value, _avg for the
 * average value and _max for the maximal value found in the interval.
 *
 * This method will cause fetching the whole data stream from the device,
 * if not yet done.
 * 
 * @return a vector containing as many strings as there are columns in the 
 *         data stream. 
 * 
 * On failure, throws an exception or returns an empty array.
 */
const vector<string>& YDataStream::get_columnNames(void)
{
    if(_columnNames.size() == 0) loadStream();
    return _columnNames;
}

/**
 * Returns the whole data set contained in the stream, as a bidimensional
 * vector of numbers.
 * The meaning of the values present in each column can be obtained
 * using the method get_columnNames().
 *
 * This method will cause fetching the whole data stream from the device,
 * if not yet done.
 * 
 * @return a vector containing as many elements as there are rows in the 
 *         data stream. Each row itself is a vector of floating-point
 *         numbers.
 * 
 * On failure, throws an exception or returns an empty array.
 */
const vector< vector<double> >& YDataStream::get_dataRows(void)
{
    if(_values.size() == 0) loadStream();
    return _values;
}

/** 
 * Returns a single measure from the data stream, specified by its
 * row and column index.
 * The meaning of the values present in each column can be obtained
 * using the method get_columnNames().
 *
 * This method will cause fetching the whole data stream from the device,
 * if not yet done.
 *  
 * @param row : row index
 * @param col : column index
 * 
 * @return a floating-point number
 *   
 * On failure, throws an exception or returns Y_DATA_INVALID.
 */
double YDataStream::get_data(unsigned row, unsigned col)
{
    if(_values.size() == 0) loadStream();
    if(row >= _values.size()) return Y_DATA_INVALID;
    if(col >= _values[row].size()) return Y_DATA_INVALID;
    return _values[row][col];
}

// DataLogger-specific method to retrieve and pre-parse recorded data
//
int YDataLogger::getData(unsigned runIdx, unsigned timeIdx, string &buffer, yJsonStateMachine &j)
{
    YDevice     *dev;
    char        query[128];
    string      errmsg;
    int         res;

    if(this->dataLoggerURL == "") this->dataLoggerURL = "/logger.json";
    
    // Resolve our reference to our device, load REST API
    res = _getDevice(dev, errmsg);
    if(YISERR(res)) {
        _throw((YRETCODE)res, errmsg);
        return (YRETCODE)res;
    }
    if(timeIdx) {
        sprintf(query, "GET %s?run=%u&time=%u HTTP/1.1\r\n\r\n", this->dataLoggerURL.c_str(), runIdx, timeIdx);
    } else {
        sprintf(query, "GET %s HTTP/1.1\r\n\r\n", this->dataLoggerURL.c_str());
    }
    res = dev->HTTPRequest(query, buffer, errmsg);
    if(YISERR(res)) {
        // Check if an update of the device list does not solve the issue
        res = YAPI::UpdateDeviceList(errmsg);
        if(YISERR(res)) {
            _throw((YRETCODE)res, errmsg);
            return (YRETCODE)res;
        }
        res = dev->HTTPRequest(query, buffer, errmsg);
        if(YISERR(res)) {
            _throw((YRETCODE)res, errmsg);
            return (YRETCODE)res;
        }
    }
    
    // Parse HTTP header
    j.src = buffer.data();
    j.end = j.src + buffer.size();
    j.st = YJSON_HTTP_START;
    if(yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_HTTP_READ_CODE) {
        _throw(YAPI_IO_ERROR, "Failed to parse HTTP header");
        return YAPI_IO_ERROR;
    }
    if(string(j.token) != "200") {
        if(string(j.token) == "404" && this->dataLoggerURL != "/dataLogger.json") {
            // retry using backward-compatible datalogger URL
            this->dataLoggerURL = "/dataLogger.json";
            return this->getData(runIdx, timeIdx, buffer, j);
        }
        _throw(YAPI_IO_ERROR, string("Unexpected HTTP return code: ")+j.token);
        return YAPI_IO_ERROR;
    }
    if(yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_HTTP_READ_MSG) {
        _throw(YAPI_IO_ERROR, "Unexpected HTTP header format");
        return YAPI_IO_ERROR;
    }
    
    return YAPI_SUCCESS;
}

/**
 * Clears the data logger memory and discards all recorded data streams.
 * This method also resets the current run index to zero.
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YDataLogger::forgetAllDataStreams(void)
{
    return set_clearHistory(Y_CLEARHISTORY_TRUE);
}

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
int YDataLogger::get_dataStreams(vector<YDataStream *>& v)
{
    string              buffer;
    yJsonStateMachine   j;
    int                 i, res;
    unsigned            arr[4];
    
    v.clear();
    if((res = getData(0, 0, buffer, j)) != YAPI_SUCCESS) {
        return res;
    }
    if(yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_ARRAY) {
        _throw(YAPI_IO_ERROR, "Unexpected JSON reply format");
        return YAPI_IO_ERROR;
    }
    // expect arrays in array
    while(yJsonParse(&j) == YJSON_PARSE_AVAIL && j.token[0] == '[') {
        // get four number
        for(i = 0; i < 4; i++) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_NUM) break;
            arr[i] = atoi(j.token);
        }
        if(i < 4) break;
        // skip any extra item in array
        while(yJsonParse(&j) != YJSON_PARSE_AVAIL && j.token[0] != ']');
        // instantiate a data stream
        v.push_back(new YDataStream(this,arr[0],arr[1],arr[2],arr[3]));
    }
    
    return YAPI_SUCCESS;
}



//--- (generated code: YDataLogger constructor)
// Constructor is protected, use yFindDataLogger factory function to instantiate
YDataLogger::YDataLogger(const string& func): YFunction("DataLogger", func)
//--- (end of generated code: YDataLogger constructor)
//--- (generated code: DataLogger initialization)
            ,_callback(NULL)
            ,_logicalName(Y_LOGICALNAME_INVALID)
            ,_advertisedValue(Y_ADVERTISEDVALUE_INVALID)
            ,_oldestRunIndex(Y_OLDESTRUNINDEX_INVALID)
            ,_currentRunIndex(Y_CURRENTRUNINDEX_INVALID)
            ,_samplingInterval(Y_SAMPLINGINTERVAL_INVALID)
            ,_timeUTC(Y_TIMEUTC_INVALID)
            ,_recording(Y_RECORDING_INVALID)
            ,_autoStart(Y_AUTOSTART_INVALID)
            ,_clearHistory(Y_CLEARHISTORY_INVALID)
//--- (end of generated code: DataLogger initialization)
{}

YDataLogger::~YDataLogger()
{
    //--- (generated code: YDataLogger cleanup)
//--- (end of generated code: YDataLogger cleanup)
}




//--- (generated code: YDataLogger implementation)

const string YDataLogger::LOGICALNAME_INVALID = "!INVALID!";
const string YDataLogger::ADVERTISEDVALUE_INVALID = "!INVALID!";



int YDataLogger::_parse(yJsonStateMachine& j)
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
        } else if(!strcmp(j.token, "oldestRunIndex")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _oldestRunIndex =  atoi(j.token);
        } else if(!strcmp(j.token, "currentRunIndex")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _currentRunIndex =  atoi(j.token);
        } else if(!strcmp(j.token, "samplingInterval")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _samplingInterval =  atoi(j.token);
        } else if(!strcmp(j.token, "timeUTC")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _timeUTC =  atoi(j.token);
        } else if(!strcmp(j.token, "recording")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _recording =  (Y_RECORDING_enum)atoi(j.token);
        } else if(!strcmp(j.token, "autoStart")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _autoStart =  (Y_AUTOSTART_enum)atoi(j.token);
        } else if(!strcmp(j.token, "clearHistory")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _clearHistory =  (Y_CLEARHISTORY_enum)atoi(j.token);
        } else {
            // ignore unknown field
            yJsonSkip(&j, 1);
        }
    }
    if(j.st != YJSON_PARSE_STRUCT) goto failed;
    return 0;
}

/**
 * Returns the logical name of the data logger.
 * 
 * @return a string corresponding to the logical name of the data logger
 * 
 * On failure, throws an exception or returns Y_LOGICALNAME_INVALID.
 */
string YDataLogger::get_logicalName(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_LOGICALNAME_INVALID;
    }
    return _logicalName;
}

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
int YDataLogger::set_logicalName(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("logicalName", rest_val);
}

/**
 * Returns the current value of the data logger (no more than 6 characters).
 * 
 * @return a string corresponding to the current value of the data logger (no more than 6 characters)
 * 
 * On failure, throws an exception or returns Y_ADVERTISEDVALUE_INVALID.
 */
string YDataLogger::get_advertisedValue(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_ADVERTISEDVALUE_INVALID;
    }
    return _advertisedValue;
}

/**
 * Returns the index of the oldest run for which the non-volatile memory still holds recorded data.
 * 
 * @return an integer corresponding to the index of the oldest run for which the non-volatile memory
 * still holds recorded data
 * 
 * On failure, throws an exception or returns Y_OLDESTRUNINDEX_INVALID.
 */
unsigned YDataLogger::get_oldestRunIndex(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_OLDESTRUNINDEX_INVALID;
    }
    return _oldestRunIndex;
}

/**
 * Returns the current run number, corresponding to the number of times the module was
 * powered on with the dataLogger enabled at some point.
 * 
 * @return an integer corresponding to the current run number, corresponding to the number of times the module was
 *         powered on with the dataLogger enabled at some point
 * 
 * On failure, throws an exception or returns Y_CURRENTRUNINDEX_INVALID.
 */
unsigned YDataLogger::get_currentRunIndex(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_CURRENTRUNINDEX_INVALID;
    }
    return _currentRunIndex;
}

unsigned YDataLogger::get_samplingInterval(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_SAMPLINGINTERVAL_INVALID;
    }
    return _samplingInterval;
}

int YDataLogger::set_samplingInterval(unsigned newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
    return _setAttr("samplingInterval", rest_val);
}

/**
 * Returns the Unix timestamp for current UTC time, if known.
 * 
 * @return an integer corresponding to the Unix timestamp for current UTC time, if known
 * 
 * On failure, throws an exception or returns Y_TIMEUTC_INVALID.
 */
unsigned YDataLogger::get_timeUTC(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_TIMEUTC_INVALID;
    }
    return _timeUTC;
}

/**
 * Changes the current UTC time reference used for recorded data.
 * 
 * @param newval : an integer corresponding to the current UTC time reference used for recorded data
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YDataLogger::set_timeUTC(unsigned newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
    return _setAttr("timeUTC", rest_val);
}

/**
 * Returns the current activation state of the data logger.
 * 
 * @return either Y_RECORDING_OFF or Y_RECORDING_ON, according to the current activation state of the data logger
 * 
 * On failure, throws an exception or returns Y_RECORDING_INVALID.
 */
Y_RECORDING_enum YDataLogger::get_recording(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_RECORDING_INVALID;
    }
    return _recording;
}

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
int YDataLogger::set_recording(Y_RECORDING_enum newval)
{
    string rest_val;
    rest_val = (newval>0 ? "1" : "0");
    return _setAttr("recording", rest_val);
}

/**
 * Returns the default activation state of the data logger on power up.
 * 
 * @return either Y_AUTOSTART_OFF or Y_AUTOSTART_ON, according to the default activation state of the
 * data logger on power up
 * 
 * On failure, throws an exception or returns Y_AUTOSTART_INVALID.
 */
Y_AUTOSTART_enum YDataLogger::get_autoStart(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_AUTOSTART_INVALID;
    }
    return _autoStart;
}

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
int YDataLogger::set_autoStart(Y_AUTOSTART_enum newval)
{
    string rest_val;
    rest_val = (newval>0 ? "1" : "0");
    return _setAttr("autoStart", rest_val);
}

Y_CLEARHISTORY_enum YDataLogger::get_clearHistory(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_CLEARHISTORY_INVALID;
    }
    return _clearHistory;
}

int YDataLogger::set_clearHistory(Y_CLEARHISTORY_enum newval)
{
    string rest_val;
    rest_val = (newval>0 ? "1" : "0");
    return _setAttr("clearHistory", rest_val);
}

YDataLogger *YDataLogger::nextDataLogger(void)
{
    string  hwid;
    
    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return yFindDataLogger(hwid);
}

void YDataLogger::registerValueCallback(YDataLoggerUpdateCallback callback)
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

void YDataLogger::advertiseValue(const string& value)
{
    if (_callback != NULL) {
        _callback(this, value);
    }
}


YDataLogger* YDataLogger::FindDataLogger(const string& func)
{
    if(YAPI::_YFunctionsCaches["YDataLogger"].find(func) != YAPI::_YFunctionsCaches["YDataLogger"].end())
        return (YDataLogger*) YAPI::_YFunctionsCaches["YDataLogger"][func];
    
    YDataLogger *newDataLogger = new YDataLogger(func);
    YAPI::_YFunctionsCaches["YDataLogger"][func] = newDataLogger ;
    return newDataLogger;
}

YDataLogger* YDataLogger::FirstDataLogger(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;
    
    if(YISERR(YapiWrapper::getFunctionsByClass("DataLogger", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YDataLogger::FindDataLogger(serial+"."+funcId);
}

//--- (end of generated code: YDataLogger implementation)

//--- (generated code: DataLogger functions)
//--- (end of generated code: DataLogger functions)
