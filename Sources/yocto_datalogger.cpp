/*********************************************************************
 *
 * $Id: yocto_datalogger.cpp 23552 2016-03-21 15:49:51Z seb $
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


YOldDataStream::YOldDataStream(YDataLogger *parent, unsigned run, 
                               unsigned stamp, unsigned utc, unsigned itv)
: YDataStream(parent)
{
    _dataLogger = parent;
    _runNo = run;
    _timeStamp = stamp;
    _utcStamp = utc;
    _interval = itv;
    _samplesPerHour = 3600 / _interval;
    _isClosed = 1;
    _minVal = DATA_INVALID;
    _avgVal = DATA_INVALID;
    _maxVal = DATA_INVALID;
}

// Preload all values into the data stream object
int YOldDataStream::loadStream(void)
{
    string              buffer;
    yJsonStateMachine   j;
    int                 res, ival;
    double              fval;
    unsigned            p, c = 0;
    vector<int>         coldiv, coltyp;
    vector<double>      colscl;
    vector<int>         colofs;
    vector<yCalibrationHandler> calhdl;
    vector<int>         caltyp;
    vector<intArr>      calpar;
    vector<floatArr>    calraw;
    vector<floatArr>    calref;
    
    vector<int>         udat;
    vector<double>      dat;
    
    _values.clear();
    if((res = _dataLogger->getData(_runNo, _timeStamp, buffer, j)) != YAPI_SUCCESS) {
        return res;
    }
    if(yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_STRUCT) {
    fail:
        _parent->_throw(YAPI_IO_ERROR, "Unexpected JSON reply format");
        return YAPI_IO_ERROR;
    }
    _nRows = _nCols = 0;
    _columnNames.clear();
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
                _columnNames.push_back(_parent->_parseString(j));
            }
            if(j.token[0] != ']') goto fail;
            if(_nCols == 0) {
                _nCols = (int)_columnNames.size();
            } else if(_nCols != (int)_columnNames.size()) {
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
                _nCols = (int)coldiv.size();
            } else if(_nCols != (int)coldiv.size()) {
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
                _nCols = (int)coltyp.size();
            } else if(_nCols != (int)coltyp.size()) {
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
                _nCols = (int)colscl.size();
            } else if(_nCols != (int)colscl.size()) {
                _nCols = 0; 
                goto fail;
            }
        } else if(!strcmp(j.token, "cal")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_ARRAY) break;
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
            if(j.st == YJSON_PARSE_STRING) {
                udat = YAPI::_decodeWords(_parent->_parseString(j));
            } else if(j.st == YJSON_PARSE_ARRAY) {
                udat.clear();
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
                c++;
                if((int)c == _nCols) {
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
int YOldDataStream::get_startTime(void)
{
    return _timeStamp;
}

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
double YOldDataStream::get_dataSamplesInterval(void)
{
    return _interval;
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
        // used by old datalogger only
        sprintf(query, "GET %s?run=%u&time=%u \r\n\r\n", this->dataLoggerURL.c_str(), runIdx, timeIdx);
    } else {
        sprintf(query, "GET %s \r\n\r\n", this->dataLoggerURL.c_str());
    }
    res = dev->HTTPRequest(0, query, buffer, NULL, NULL, errmsg);
    if(YISERR(res)) {
        // Check if an update of the device list does not solve the issue
        res = YAPI::UpdateDeviceList(errmsg);
        if(YISERR(res)) {
            _throw((YRETCODE)res, errmsg);
            return (YRETCODE)res;
        }
        res = dev->HTTPRequest(0, query, buffer, NULL, NULL, errmsg);
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
int YDataLogger::get_dataStreams(vector<YDataStream *>& v)
{
    string              buffer;
    yJsonStateMachine   j;
    int                 res;
	unsigned            i, si, arr[4];
	YOldDataStream      *ods;
    
    v.clear();
    if((res = getData(0, 0, buffer, j)) != YAPI_SUCCESS) {
        return res;
    }
    if(yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_ARRAY) {
        _throw(YAPI_IO_ERROR, "Unexpected JSON reply format");
        return YAPI_IO_ERROR;
    }
    // expect arrays in array
    while(yJsonParse(&j) == YJSON_PARSE_AVAIL) {
        if(j.token[0] == '[') {
            // old datalogger format: [runIdx, timerel, utc, interval]
            for(i = 0; i < 4; i++) {
                if(yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_NUM) break;
                arr[i] = atoi(j.token);
            }
            if(i < 4) break;
            // skip any extra item in array
            while(yJsonParse(&j) == YJSON_PARSE_AVAIL && j.token[0] != ']');
			// instantiate a data stream
			ods = new YOldDataStream(this,arr[0],arr[1],arr[2],arr[3]);
			v.push_back(ods);
        } else if(j.token[0] == '{') {
            // new datalogger format: {"id":"...","unit":"...","streams":["...",...]}
            size_t pos = buffer.find("\r\n\r\n", 0);
            buffer = buffer.substr(pos+4);
            vector<YDataSet> sets = this->parse_dataSets(buffer);
            for (i=0; i < sets.size(); i++) { 
                vector<YDataStream*> ds = sets[i].get_privateDataStreams();
                for (si=0; si < ds.size(); si++) { 
					// return a user-owned copy

					v.push_back(new YDataStream(*ds[si]));
                }
            }
            break;
        } else break;
    }
    
    return YAPI_SUCCESS;
}



YDataLogger::YDataLogger(const string& func): YFunction(func)
//--- (generated code: DataLogger initialization)
    ,_currentRunIndex(CURRENTRUNINDEX_INVALID)
    ,_timeUTC(TIMEUTC_INVALID)
    ,_recording(RECORDING_INVALID)
    ,_autoStart(AUTOSTART_INVALID)
    ,_beaconDriven(BEACONDRIVEN_INVALID)
    ,_clearHistory(CLEARHISTORY_INVALID)
    ,_valueCallbackDataLogger(NULL)
//--- (end of generated code: DataLogger initialization)
{
    _className = "DataLogger";
}

YDataLogger::~YDataLogger()
{
    //--- (generated code: YDataLogger cleanup)
//--- (end of generated code: YDataLogger cleanup)
}


//--- (generated code: YDataLogger implementation)
// static attributes

int YDataLogger::_parseAttr(yJsonStateMachine& j)
{
    if(!strcmp(j.token, "currentRunIndex")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _currentRunIndex =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "timeUTC")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _timeUTC =  atol(j.token);
        return 1;
    }
    if(!strcmp(j.token, "recording")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _recording =  (Y_RECORDING_enum)atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "autoStart")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _autoStart =  (Y_AUTOSTART_enum)atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "beaconDriven")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _beaconDriven =  (Y_BEACONDRIVEN_enum)atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "clearHistory")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _clearHistory =  (Y_CLEARHISTORY_enum)atoi(j.token);
        return 1;
    }
    failed:
    return YFunction::_parseAttr(j);
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
int YDataLogger::get_currentRunIndex(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YDataLogger::CURRENTRUNINDEX_INVALID;
        }
    }
    return _currentRunIndex;
}

/**
 * Returns the Unix timestamp for current UTC time, if known.
 *
 * @return an integer corresponding to the Unix timestamp for current UTC time, if known
 *
 * On failure, throws an exception or returns Y_TIMEUTC_INVALID.
 */
s64 YDataLogger::get_timeUTC(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YDataLogger::TIMEUTC_INVALID;
        }
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
int YDataLogger::set_timeUTC(s64 newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", (u32)newval); rest_val = string(buf);
    return _setAttr("timeUTC", rest_val);
}

/**
 * Returns the current activation state of the data logger.
 *
 * @return a value among Y_RECORDING_OFF, Y_RECORDING_ON and Y_RECORDING_PENDING corresponding to the
 * current activation state of the data logger
 *
 * On failure, throws an exception or returns Y_RECORDING_INVALID.
 */
Y_RECORDING_enum YDataLogger::get_recording(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YDataLogger::RECORDING_INVALID;
        }
    }
    return _recording;
}

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
int YDataLogger::set_recording(Y_RECORDING_enum newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
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
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YDataLogger::AUTOSTART_INVALID;
        }
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

/**
 * Return true if the data logger is synchronised with the localization beacon.
 *
 * @return either Y_BEACONDRIVEN_OFF or Y_BEACONDRIVEN_ON
 *
 * On failure, throws an exception or returns Y_BEACONDRIVEN_INVALID.
 */
Y_BEACONDRIVEN_enum YDataLogger::get_beaconDriven(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YDataLogger::BEACONDRIVEN_INVALID;
        }
    }
    return _beaconDriven;
}

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
int YDataLogger::set_beaconDriven(Y_BEACONDRIVEN_enum newval)
{
    string rest_val;
    rest_val = (newval>0 ? "1" : "0");
    return _setAttr("beaconDriven", rest_val);
}

Y_CLEARHISTORY_enum YDataLogger::get_clearHistory(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YDataLogger::CLEARHISTORY_INVALID;
        }
    }
    return _clearHistory;
}

int YDataLogger::set_clearHistory(Y_CLEARHISTORY_enum newval)
{
    string rest_val;
    rest_val = (newval>0 ? "1" : "0");
    return _setAttr("clearHistory", rest_val);
}

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
YDataLogger* YDataLogger::FindDataLogger(string func)
{
    YDataLogger* obj = NULL;
    obj = (YDataLogger*) YFunction::_FindFromCache("DataLogger", func);
    if (obj == NULL) {
        obj = new YDataLogger(func);
        YFunction::_AddToCache("DataLogger", func, obj);
    }
    return obj;
}

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
int YDataLogger::registerValueCallback(YDataLoggerValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackDataLogger = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YDataLogger::_invokeValueCallback(string value)
{
    if (_valueCallbackDataLogger != NULL) {
        _valueCallbackDataLogger(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
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
    return this->set_clearHistory(Y_CLEARHISTORY_TRUE);
}

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
vector<YDataSet> YDataLogger::get_dataSets(void)
{
    return this->parse_dataSets(this->_download("logger.json"));
}

vector<YDataSet> YDataLogger::parse_dataSets(string json)
{
    vector<string> dslist;
    YDataSet* dataset = NULL;
    vector<YDataSet> res;
    // may throw an exception
    dslist = this->_json_get_array(json);
    res.clear();
    for (unsigned ii = 0; ii < dslist.size(); ii++) {
        dataset = new YDataSet(this);
        dataset->_parse(dslist[ii]);
        res.push_back(*dataset);;
    }
    return res;
}

YDataLogger *YDataLogger::nextDataLogger(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YDataLogger::FindDataLogger(hwid);
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
