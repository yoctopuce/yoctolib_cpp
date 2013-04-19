/*********************************************************************
 *
 * $Id: yocto_api.cpp 10717 2013-03-27 11:18:20Z mvuilleu $
 *
 * High-level programming interface, common to all modules
 *
 * - - - - - - - - - License information: - - - - - - - - - 
 *
 * Copyright (C) 2011 and beyond by Yoctopuce Sarl, Switzerland.
 *
 * 1) If you have obtained this file from www.yoctopuce.com,
 *    Yoctopuce Sarl licenses to you (hereafter Licensee) the
 *    right to use, modify, copy, and integrate this source file
 *    into your own solution for the sole purpose of interfacing
 *    a Yoctopuce product with Licensee's solution.
 *
 *    The use of this file and all relationship between Yoctopuce 
 *    and Licensee are governed by Yoctopuce General Terms and 
 *    Conditions.
 *
 *    THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 *    WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING 
 *    WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS 
 *    FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO
 *    EVENT SHALL LICENSOR BE LIABLE FOR ANY INCIDENTAL, SPECIAL,
 *    INDIRECT OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, 
 *    COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR 
 *    SERVICES, ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT 
 *    LIMITED TO ANY DEFENSE THEREOF), ANY CLAIMS FOR INDEMNITY OR
 *    CONTRIBUTION, OR OTHER SIMILAR COSTS, WHETHER ASSERTED ON THE
 *    BASIS OF CONTRACT, TORT (INCLUDING NEGLIGENCE), BREACH OF
 *    WARRANTY, OR OTHERWISE.
 *
 * 2) If your intent is not to interface with Yoctopuce products,
 *    you are not entitled to use, read or create any derived 
 *    material from this source file.
 *
 *********************************************************************/

#define _CRT_SECURE_NO_DEPRECATE
#include "yocto_api.h"
#include "yapi/yapi.h"

#ifdef WINDOWS_API
#include <Windows.h>
#define yySleep(ms)          Sleep(ms)
#else
#include <unistd.h>
#define yySleep(ms)          usleep(ms*1000)
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <cfloat>
#include <cmath>
#include <stdarg.h>

static  yCRITICAL_SECTION   _updateDeviceList_CS;
static  yCRITICAL_SECTION   _handleEvent_CS;

static  std::vector<YFunction*>     _FunctionCache;
static  std::vector<YFunction*>     _FunctionCallbacks;

#define Y_INVALID_STRING "!INVALID!"

const string YFunction::HARDWAREID_INVALID = Y_INVALID_STRING;
const string YFunction::FUNCTIONID_INVALID = Y_INVALID_STRING;
const string YFunction::FRIENDLYNAME_INVALID = Y_INVALID_STRING;


// Constructor is protected. Use the device-specific factory function to instantiate
YFunction::YFunction(const string& classname, const string& func):
    _className(classname),_func(func),
    _lastErrorType(YAPI_SUCCESS),_lastErrorMsg(""),
    _cacheExpiration(0), _fundescr(Y_FUNCTIONDESCRIPTOR_INVALID), _userData(NULL),_genCallback(NULL)
{
    _FunctionCache.push_back(this);
};


std::ostream& operator<<( std::ostream &stream, YFunction &fun )
{
    stream << fun.describe();
    return stream;
}

void YFunction::_throw(YRETCODE errType, string errMsg)
{
    _lastErrorType = errType;
    _lastErrorMsg = errMsg;
    // Method used to throw exceptions or save error type/message
    if(!YAPI::ExceptionsDisabled) {
        throw YAPI_Exception(errType, errMsg);
    }
}

// Method used to resolve our name to our unique function descriptor (may trigger a hub scan)
YRETCODE YFunction::_getDescriptor(YFUN_DESCR& fundescr, string& errmsg)
{
    int res;
    YFUN_DESCR tmp_fundescr;

    tmp_fundescr = YapiWrapper::getFunction(_className, _func, errmsg);
    if(YISERR(tmp_fundescr)) {
        res = YapiWrapper::updateDeviceList(true,errmsg);
        if(YISERR(res)) {
            return (YRETCODE)res;
        }
        tmp_fundescr = YapiWrapper::getFunction(_className, _func, errmsg);
        if(YISERR(tmp_fundescr)) {
            return (YRETCODE)tmp_fundescr;
        }
    }
    _fundescr =  fundescr = tmp_fundescr;
    return YAPI_SUCCESS;
}

// Return a pointer to our device caching object (may trigger a hub scan)
YRETCODE YFunction::_getDevice(YDevice*& dev, string& errmsg)
{
    YFUN_DESCR   fundescr;
    YDEV_DESCR     devdescr;
    YRETCODE    res;

    // Resolve function name
    res = _getDescriptor(fundescr, errmsg);
    if(YISERR(res)) return res;
    
    // Get device descriptor
    devdescr = YapiWrapper::getDeviceByFunction(fundescr, errmsg);
    if(YISERR(devdescr)) return (YRETCODE)devdescr;
    
    // Get device object
    dev = YDevice::getDevice(devdescr);

    return YAPI_SUCCESS;
}

// Return the next known function of current class listed in the yellow pages
YRETCODE YFunction::_nextFunction(string& hwid)
{
    vector<YFUN_DESCR>   v_fundescr;
    YFUN_DESCR           fundescr;
    YDEV_DESCR           devdescr;
    string               serial, funcId, funcName, funcVal, errmsg;
    int                  res;

    res = _getDescriptor(fundescr, errmsg);
    if(YISERR(res)) {
        _throw((YRETCODE)res, errmsg);
        return (YRETCODE)res;
    }
    res = YapiWrapper::getFunctionsByClass(_className, fundescr, v_fundescr, sizeof(YFUN_DESCR), errmsg);
    if(YISERR((YRETCODE)res)) {
        _throw((YRETCODE)res, errmsg);
        return (YRETCODE)res;
    }
    if(v_fundescr.size() == 0) {
        hwid = "";
        return YAPI_SUCCESS;
    }
    res = YapiWrapper::getFunctionInfo(v_fundescr[0], devdescr, serial, funcId, funcName, funcVal, errmsg);
    if(YISERR(res)) {
        _throw((YRETCODE)res, errmsg);
        return (YRETCODE)res;
    }
    hwid = serial+"."+funcId;

    return YAPI_SUCCESS;
}

// Parse a long JSON string
string  YFunction::_parseString(yJsonStateMachine& j)
{
    string  res = j.token;
    
    while(j.next == YJSON_PARSE_STRINGCONT && yJsonParse(&j) == YJSON_PARSE_AVAIL) {
        res += j.token;
    }
    return res;
}

YRETCODE  YFunction::_buildSetRequest( const string& changeattr, const string  *changeval, string& request, string& errmsg)
{
    int res;
    YFUN_DESCR fundesc;
    char        funcid[YOCTO_FUNCTION_LEN];
    char        errbuff[YOCTO_ERRMSG_LEN];
    
    
    // Resolve the function name
    res = _getDescriptor(fundesc, errmsg);
    if(YISERR(res)) {
        return (YRETCODE)res;
    }

    if(YISERR(res=yapiGetFunctionInfo(fundesc, NULL, NULL, funcid, NULL, NULL,errbuff))){
        errmsg = errbuff;
        _throw((YRETCODE)res, errmsg);
        return (YRETCODE)res;
    }
    request = "GET /api/";
    request.append(funcid);
    request.append("/");
    //request.append(".json/");
    
    if(changeattr!="") {
        request.append(changeattr);
        if(changeval) {
            const char *p;
            unsigned char        c;
            unsigned char       esc[3];
            request.append("?");
            request.append(changeattr);
            request.append("=");
            esc[0]='%';
            for(p=changeval->c_str(); (c = *p) != 0; p++) {
                if(c <= ' ' || (c > 'z' && c != '~') || c == '"' || c == '%' || c == '&' || 
                   c == '+' || c == '<' || c == '=' || c == '>' || c == '\\' || c == '^' || c == '`') {
                    esc[1]=(c >= 0xa0 ? (c>>4)-10+'A' : (c>>4)+'0');
                    c &= 0xf;
                    esc[2]=(c >= 0xa ? c-10+'A' : c+'0');
                    request.append((char*)esc,3);
                } else {
                    request.append((char*)&c,1);
                }
            }
        }
    }
    // don't append HTTP/1.1 so that we get light headers from hub
    request.append(" \r\n\r\n");
    return YAPI_SUCCESS;
}

// Method used to encode calibration points into fixed-point 16-bit integers or decimal floating-point
//
string YFunction::_encodeCalibrationPoints(const floatArr& rawValues, const floatArr& refValues, double resolution, int calibrationOffset, const string& actualCparams)
{
    int     npt = (int)(rawValues.size() < refValues.size() ? rawValues.size() : refValues.size());
    int     caltype;
    int     rawVal, refVal;
    int     minRaw = 0;
    char    buff[32];
    string  res;
    
    if(npt ==0 ){
        return "0";
    }
    if(actualCparams == ""){
        caltype = 10 + npt;
    }else{
        int pos = (int)actualCparams.find(',');
        if(pos<0)
            caltype =0;
        else
            caltype = atoi(actualCparams.substr(0,pos).c_str());
        if(caltype <= 10) {
            caltype = npt;
        } else {
            caltype = 10+npt;
        }
    }
    sprintf(buff, "%u", caltype);
    res = string(buff);
    if (caltype <=10){
        for(int i = 0; i < npt; i++) {
            rawVal = (int) (rawValues[i] / resolution - calibrationOffset + .5);
            if(rawVal >= minRaw && rawVal < 65536) {
                refVal = (int) (refValues[i] / resolution - calibrationOffset + .5);
                if(refVal >= 0 && refVal < 65536) {
                    sprintf(buff, ",%d,%d", rawVal, refVal);
                    res += string(buff);
                    minRaw = rawVal+1;
                }
            }
        }
    } else {
        // 16-bit floating-point decimal encoding
        for(int i = 0; i < npt; i++) {
            rawVal = YAPI::_doubleToDecimal(rawValues[i]);
            refVal = YAPI::_doubleToDecimal(refValues[i]);
            sprintf(buff, ",%d,%d", rawVal, refVal);
            res += string(buff);
        }
    }
    return res;
}

// Method used to decode calibration points given as 16-bit fixed-point or decimal floating-point
//
int YFunction::_decodeCalibrationPoints(const string& calibParams, floatArr& rawPt, floatArr& calPt, double resolution, int calibrationOffset)
{
    const char *p = calibParams.c_str();
    int        calibType, npt, i = 0, ival;
    double     fval;

    // parse calibration parameters
    rawPt.clear();
    calPt.clear();
    calibType = (*p ? atoi(p) : 0);
    npt = (calibType <= 20 ? calibType % 10 : 99);    
    while(*p && *p != ',') p++;
    while(*p && npt) {
        p++; // skip ','
        ival = atoi(p);
        if(calibType <= 10) {
            fval = (ival + calibrationOffset) * resolution;
        } else {
            fval = YAPI::_decimalToDouble(ival);
        }
        while(*p && *p != ',') p++;
        if(!(i++ & 1)) {
            rawPt.push_back(fval);
        } else {
            calPt.push_back(fval);
            npt--;
        }
    }
    return YAPI_SUCCESS;
}

// Set an attribute in the function, and parse the resulting new function state
YRETCODE YFunction::_setAttr(string attrname, string newvalue)
{
    string      errmsg, request;
    int         res;
    YDevice     *dev;
    
    // Execute http request
    res = _buildSetRequest(attrname, &newvalue, request, errmsg);
    if(YISERR(res)) {
        _throw((YRETCODE)res, errmsg);
        return (YRETCODE)res;
    }
    // Get device Object
    res = _getDevice(dev,  errmsg);
    if(YISERR(res)) {
        _throw((YRETCODE)res, errmsg);
        return (YRETCODE)res;
    }
    
    res = dev->HTTPRequestAsync(request,NULL,NULL,errmsg);
    if(YISERR(res)) {
        // Check if an update of the device list does not solve the issue
        res = YapiWrapper::updateDeviceList(true,errmsg);
        if(YISERR(res)) {
            _throw((YRETCODE)res, errmsg);
            return (YRETCODE)res;
        }
        res = dev->HTTPRequestAsync(request,NULL,NULL,errmsg);
        if(YISERR(res)) {
            _throw((YRETCODE)res, errmsg);
            return (YRETCODE)res;
        }
    }
    _cacheExpiration=0;
    return YAPI_SUCCESS;
    
}


// Method used to send http request to the device (not the function)
string      YFunction::_request(const string& request)
{
    YDevice     *dev;
    string      errmsg, buffer;
    int         res;
    
    
    // Resolve our reference to our device, load REST API
    res = _getDevice(dev, errmsg);
    if(YISERR(res)) {
        _throw((YRETCODE)res, errmsg);
        return YAPI::INVALID_STRING;
    }
    res = dev->HTTPRequest(request, buffer, errmsg);
    if(YISERR(res)) {
        // Check if an update of the device list does notb solve the issue
        res = YapiWrapper::updateDeviceList(true,errmsg);
        if(YISERR(res)) {
            this->_throw((YRETCODE)res,errmsg);
            return YAPI::INVALID_STRING;
        }
        res = dev->HTTPRequest(request, buffer, errmsg);
        if(YISERR(res)) {
            this->_throw((YRETCODE)res,errmsg);
            return YAPI::INVALID_STRING;
        }
    }
    if(0 != buffer.find("OK\r\n")){
        if(0 != buffer.find("HTTP/1.1 200 OK\r\n")){
            this->_throw(YAPI_IO_ERROR,"http request failed");
            return YAPI::INVALID_STRING;
        }
    }
    return buffer;
}


// Method used to send http request to the device (not the function)
string      YFunction::_download(const string& url)
{
    string      request,buffer;
	size_t      found;
      
    request = "GET /"+url+" HTTP/1.1\r\n\r\n";
    buffer = this->_request(request);
    found = buffer.find("\r\n\r\n");
    if(string::npos == found){
        this->_throw(YAPI_IO_ERROR,"http request failed");
        return YAPI::INVALID_STRING;
    }
    return buffer.substr(found+4);
}


// Method used to upload a file to the device
YRETCODE    YFunction::_upload(const string& path, const string& content)
{

    string      request,buffer;
    string      boundary;
	size_t      found;
  
    request = "POST /upload.html HTTP/1.1\r\n";
    string body =   "Content-Disposition: form-data; name=\""+path+"\"; filename=\"api\"\r\n"+
                    "Content-Type: application/octet-stream\r\n"+
                    "Content-Transfer-Encoding: binary\r\n\r\n"+content;
    do {
        boundary = YapiWrapper::ysprintf("Zz%06xzZ", rand() & 0xffffff); 
    } while(body.find(boundary) !=string::npos);
    request += "Content-Type: multipart/form-data; boundary="+boundary+"\r\n";
    request += "\r\n--"+boundary+"\r\n"+body+"\r\n--"+boundary+"--\r\n";
    buffer = this->_request(request);
    found = buffer.find("\r\n\r\n");
    if(string::npos == found){
        this->_throw(YAPI_IO_ERROR,"http request failed");
        return YAPI_IO_ERROR;
    }
    return YAPI_SUCCESS;
}


string      YFunction::_json_get_key(const string& json, const string& key)
{
    yJsonStateMachine j;
    
    // Parse JSON data for the device and locate our function in it
    j.src = json.c_str();
    j.end = j.src + strlen(j.src);
    j.st = YJSON_START;
    if(yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_STRUCT) {
        this->_throw(YAPI_IO_ERROR,"JSON structure expected");
        return YAPI::INVALID_STRING;
    }
    while(yJsonParse(&j) == YJSON_PARSE_AVAIL && j.st == YJSON_PARSE_MEMBNAME) {
        if (!strcmp(j.token, key.c_str())) {
            if (yJsonParse(&j) != YJSON_PARSE_AVAIL) {
                this->_throw(YAPI_IO_ERROR,"JSON structure expected");
                return YAPI::INVALID_STRING;
            }
            return  _parseString(j);
        }
        yJsonSkip(&j, 1);
    }
    this->_throw(YAPI_IO_ERROR,"invalid JSON structure");
    return YAPI::INVALID_STRING;   
}

vector<string> YFunction::_json_get_array(const string& json)
{
    vector<string> res;
    yJsonStateMachine j;
    const char *json_cstr,*last;
    j.src = json_cstr = json.c_str();
    j.end = j.src + strlen(j.src);
    j.st = YJSON_START;
    if(yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_ARRAY) {
        this->_throw(YAPI_IO_ERROR,"JSON structure expected");
        return res;
    }
    int depth =j.depth;
    do {
        last=j.src;
        while(yJsonParse(&j) == YJSON_PARSE_AVAIL && j.depth> depth );
        if (j.depth == depth ) {
            long location,length;
            while(*last ==',' || *last =='\n')last++;
            location =last -json_cstr;
            length = j.src-last;
            string item= json.substr(location,length);
            res.push_back(item);
        }
    } while ( j.st != YJSON_PARSE_ARRAY);
    return res;   
}




// Return a string that describes the function (class and logical name or hardware id)
string YFunction::describe(void)
{
    YFUN_DESCR  fundescr;
    YDEV_DESCR  devdescr;
    string      errmsg, serial, funcId, funcName, funcValue;
    string      descr =  _func;

    fundescr = YapiWrapper::getFunction(_className, _func, errmsg);
    if(!YISERR(fundescr) && !YISERR(YapiWrapper::getFunctionInfo(fundescr, devdescr, serial, funcId, funcName, funcValue, errmsg))) {
            return _className +"("+_func+")="+serial+"."+funcId;
    }
    return _className +"("+_func+")=unresolved";
}

// Return a string that describes the function (class and logical name or hardware id)
string YFunction::get_friendlyName(void)
{
    YFUN_DESCR   fundescr,moddescr;
    YDEV_DESCR   devdescr;
    YRETCODE     retcode;
    string       errmsg, serial, funcId, funcName, funcValue;
    string       mod_serial, mod_funcId,mod_funcname;
    
    // Resolve the function name
    retcode = _getDescriptor(fundescr, errmsg);
    if(!YISERR(retcode) && !YISERR(YapiWrapper::getFunctionInfo(fundescr, devdescr, serial, funcId, funcName, funcValue, errmsg))) {
        if(funcName!="") {
            funcId = funcName;
        }
        
        moddescr = YapiWrapper::getFunction("Module", serial, errmsg);
        if(!YISERR(moddescr) && !YISERR(YapiWrapper::getFunctionInfo(moddescr, devdescr, mod_serial, mod_funcId, mod_funcname, funcValue, errmsg))) {
            if(mod_funcname!="") {
                return mod_funcname+"."+funcId;
            }
        }
        return serial+"."+funcId;
    }
    _throw((YRETCODE)YAPI::DEVICE_NOT_FOUND, errmsg);
    return Y_FRIENDLYNAME_INVALID;
}




// Returns the unique hardware ID of the function
string YFunction::get_hardwareId(void)
{
    YRETCODE    retcode;
    YFUN_DESCR  fundesc;
    string      errmsg;
    char        snum[YOCTO_SERIAL_LEN];
    char        funcid[YOCTO_FUNCTION_LEN];
    char        errbuff[YOCTO_ERRMSG_LEN];
    
    
    // Resolve the function name
    retcode = _getDescriptor(fundesc, errmsg);
    if(YISERR(retcode)) {
        _throw(retcode, errmsg);
        return HARDWAREID_INVALID;
    }    
    if(YISERR(retcode=yapiGetFunctionInfo(fundesc, NULL, snum, funcid, NULL, NULL,errbuff))){
        errmsg = errbuff;
        _throw(retcode, errmsg);
        return HARDWAREID_INVALID;
    }

    return string(snum)+string(".")+string(funcid);
}

// Returns the unique function ID of the function
string YFunction::get_functionId(void)
{
    YRETCODE    retcode;
    YFUN_DESCR  fundesc;
    string      errmsg;
    char        funcid[YOCTO_FUNCTION_LEN];
    char        errbuff[YOCTO_ERRMSG_LEN];
    
    
    // Resolve the function name
    retcode = _getDescriptor(fundesc, errmsg);
    if(YISERR(retcode)) {
        _throw(retcode, errmsg);
        return HARDWAREID_INVALID;
    }
    if(YISERR(retcode=yapiGetFunctionInfo(fundesc, NULL, NULL, funcid, NULL, NULL,errbuff))){
        errmsg = errbuff;
        _throw(retcode, errmsg);
        return HARDWAREID_INVALID;
    }
    
    return string(funcid);
}


// Return the numerical error type of the last error with this function
YRETCODE YFunction::get_errorType(void)
{
    return _lastErrorType;
}

// Return the human-readable explanation about the last error with this function
string YFunction::get_errorMessage(void)
{
    return _lastErrorMsg;
}

// Return true if the function can be reached, and false otherwise. No exception will be raised.
// If there is a valid value in cache, the device is considered reachable.
bool YFunction::isOnline(void)
{
    YDevice     *dev;
    string      errmsg, apires;

    // A valid value in cache means that the device is online
    if(_cacheExpiration > yapiGetTickCount()) return true;
    
    // Check that the function is available, without throwing exceptions
    if(YISERR(_getDevice(dev, errmsg))) return false;

    // Try to execute a function request to be positively sure that the device is ready
    if(YISERR(dev->requestAPI(apires, errmsg))) {
		printf("not online:%s",errmsg.c_str());
		return false;
	}
    return true;   
}

YRETCODE YFunction::load(int msValidity)
{
    yJsonStateMachine j;
    YDevice     *dev;
    string      errmsg, apires;
    YFUN_DESCR   fundescr;
    int         res;
    char        errbuf[YOCTO_ERRMSG_LEN];
    char        funcId[YOCTO_FUNCTION_LEN];

    // Resolve our reference to our device, load REST API
    res = _getDevice(dev, errmsg);
    if(YISERR(res)) {
        _throw((YRETCODE)res, errmsg);
        return (YRETCODE)res;
    }
    res = dev->requestAPI(apires, errmsg);
    if(YISERR(res)) {
        _throw((YRETCODE)res, errmsg);
        return (YRETCODE)res;
    }                
    
    // Get our function Id
    fundescr = YapiWrapper::getFunction(_className, _func, errmsg);
    if(YISERR(fundescr)) {
        _throw((YRETCODE)fundescr, errmsg);
        return (YRETCODE)fundescr;
    }
    res = yapiGetFunctionInfo(fundescr, NULL, NULL, funcId, NULL, NULL, errbuf);
    if(YISERR(res)) {
        _throw((YRETCODE)res, errbuf);
        return (YRETCODE)res;
    }            

    // Parse JSON data for the device and locate our function in it
    j.src = apires.data();
    j.end = j.src + apires.size();
    j.st = YJSON_START;
    if(yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_STRUCT) {
        _throw(YAPI_IO_ERROR, "JSON structure expected");
        return YAPI_IO_ERROR;
    }
    while(yJsonParse(&j) == YJSON_PARSE_AVAIL && j.st == YJSON_PARSE_MEMBNAME) {
        if(!strcmp(j.token, funcId)) {
            _parse(j);
            break;
        }
        yJsonSkip(&j, 1);
    }
    _cacheExpiration = yapiGetTickCount() + msValidity;
    
    return YAPI_SUCCESS;
}

YModule *YFunction::get_module(void)
{
    YFUN_DESCR   fundescr;
    YDEV_DESCR     devdescr;
    string      errmsg, serial, funcId, funcName, funcValue;

    fundescr = YapiWrapper::getFunction(_className, _func, errmsg);
    if(!YISERR(fundescr)) {
        if(!YISERR(YapiWrapper::getFunctionInfo(fundescr, devdescr, serial, funcId, funcName, funcValue, errmsg))) {
            return yFindModule(serial+".module");
        }
    }
    // return a true YModule object even if it is not a module valid for communicating
    return yFindModule(string("module_of_")+_className+"_"+_func);
}



void     *YFunction::get_userData(void)
{
    return _userData;
}


void      YFunction::set_userData(void* data)
{
    _userData = data;
}


YFUN_DESCR YFunction::get_functionDescriptor(void)
{
    return _fundescr;
}



void YFunction::_registerFuncCallback(YFunction* func)
{
    isOnline();
    vector<YFunction*>::iterator it;
    for ( it=_FunctionCallbacks.begin() ; it < _FunctionCallbacks.end(); it++ ){
        if (*it == func)
            return;
    }
    _FunctionCallbacks.push_back(func);
}

void YFunction::_unregisterFuncCallback(YFunction* func)
{
    vector<YFunction*>::iterator it;
    for ( it=_FunctionCallbacks.begin() ; it < _FunctionCallbacks.end(); it++ ){
        if (*it == func) {
             _FunctionCallbacks.erase(it);
        }
    }
}

/**
 * 
 */
void YFunction::registerValueCallback(YFunctionUpdateCallback callback)
{
    if (callback != NULL)
    {
        _registerFuncCallback(this);
    } else {
        _unregisterFuncCallback(this);
    }
    _genCallback = callback;
}


void YFunction::advertiseValue(const string& value)
{
    if(_genCallback)
        _genCallback(this,value);
}





// This is the internal device cache object
vector<YDevice*> YDevice::_devCache;

YDevice *YDevice::getDevice(YDEV_DESCR devdescr)
{
    // Search in cache
    for(unsigned int idx = 0; idx < YDevice::_devCache.size(); idx++) {
        if(YDevice::_devCache[idx]->_devdescr == devdescr) {
            return YDevice::_devCache[idx];
        }
    }

    // Not found, add new entry
    YDevice *dev = new YDevice(devdescr);
    YDevice::_devCache.push_back(dev);
    
    return dev;
}


YRETCODE    YDevice::HTTPRequestPrepare(const string& request, string& fullrequest, char *errbuff)
{
    YRETCODE    res;
    size_t      pos;
    
    if(_subpath==NULL){
        int neededsize;
        res = yapiGetDevicePath(_devdescr, _rootdevice, NULL, 0, &neededsize, errbuff);
        if(YISERR(res)) return res;
        _subpath = new char[neededsize];
        res = yapiGetDevicePath(_devdescr, _rootdevice, _subpath, neededsize, NULL, errbuff);
        if(YISERR(res)) return res;
    }
    pos = request.find_first_of('/');
    fullrequest = request.substr(0,pos) + (string)_subpath + request.substr(pos+1);

    return YAPI_SUCCESS;
}

YRETCODE    YDevice::HTTPRequestAsync(const string& request, HTTPRequestCallback callback, void *context, string& errmsg)
{
    char        errbuff[YOCTO_ERRMSG_LEN]="";
    YRETCODE    res;
    string      fullrequest;

    _cacheStamp     = YAPI::GetTickCount(); //invalidate cache
    if(YISERR(res=HTTPRequestPrepare(request, fullrequest, errbuff)) ||
       YISERR(res=yapiHTTPRequestAsync(_rootdevice, fullrequest.c_str(), NULL, NULL, errbuff))){
        errmsg = (string)errbuff;
        return res;
    }
    return YAPI_SUCCESS;
}


YRETCODE    YDevice::HTTPRequest(const string& request, string& buffer, string& errmsg)
{
    char        errbuff[YOCTO_ERRMSG_LEN]="";
    YRETCODE    res;
    YIOHDL      iohdl;
    string      fullrequest;
    char        *reply;
    int         replysize = 0;
     
    if(YISERR(res=HTTPRequestPrepare(request, fullrequest, errbuff))) {
        errmsg = (string)errbuff;
        return res;
    }
    if(YISERR(res=yapiHTTPRequestSyncStartEx(&iohdl, _rootdevice, fullrequest.data(), (int)fullrequest.size(), &reply, &replysize, errbuff))) {
        errmsg = (string)errbuff;
        return res;
    }
    buffer = string(reply, replysize);
    if(YISERR(res=yapiHTTPRequestSyncDone(&iohdl, errbuff))) {
        errmsg = (string)errbuff;
        return res;
    }

    return YAPI_SUCCESS;
}


YRETCODE YDevice::requestAPI(string& apires, string& errmsg)
{
    yJsonStateMachine j;
    string          rootdev, request, buffer;
    int             res;
    
    // Check if we have a valid cache value
    if(_cacheStamp > YAPI::GetTickCount()) {
        apires = _cacheJson;
        return YAPI_SUCCESS;
    }
    
    // send request, without HTTP/1.1 suffix to get light headers
    res = this->HTTPRequest("GET /api.json \r\n\r\n", buffer, errmsg);
    if(YISERR(res)) {
        // Check if an update of the device list does not solve the issue
        res = YapiWrapper::updateDeviceList(true,errmsg);
        if(YISERR(res)) {
            return (YRETCODE)res;
        }
        // send request, without HTTP/1.1 suffix to get light headers
        res = this->HTTPRequest("GET /api.json \r\n\r\n", buffer, errmsg);
        if(YISERR(res)) {
            return (YRETCODE)res;
        }
    }
    
    // Parse HTTP header
    j.src = buffer.data();
    j.end = j.src + buffer.size();
    j.st = YJSON_HTTP_START;
    if(yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_HTTP_READ_CODE) {
        errmsg = "Failed to parse HTTP header";
        return YAPI_IO_ERROR;
    }
    if(string(j.token) != "200") {
        errmsg = string("Unexpected HTTP return code: ")+j.token;
        return YAPI_IO_ERROR;
    }
    if(yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_HTTP_READ_MSG) {
        errmsg = "Unexpected HTTP header format";
        return YAPI_IO_ERROR;
    }
    if(yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_STRUCT) {
        errmsg = "Unexpected JSON reply format";
        return YAPI_IO_ERROR;
    }
    // we know for sure that the last character parsed was a '{'
    do j.src--; while(j.src[0] != '{');
    apires = string(j.src);    
    
    // store result in cache
    _cacheJson = string(j.src);
    _cacheStamp = yapiGetTickCount() + YAPI::DefaultCacheValidity;
    
    return YAPI_SUCCESS;
}

YRETCODE YDevice::getFunctions(vector<YFUN_DESCR> **functions, string& errmsg)
{        
    if(_functions.size() == 0) {
        int res = YapiWrapper::getFunctionsByDevice(_devdescr, 0, _functions, 64, errmsg);
        if(YISERR(res)) return (YRETCODE)res;
    }
    *functions = &_functions;

    return YAPI_SUCCESS;
}

//--- (generated code: return codes)
// this is a test
//--- (end of generated code: return codes)


queue<yapiEvent>    YAPI::_plug_events;
queue<yapiEvent>    YAPI::_data_events;

u64         YAPI::_nextEnum         = 0;
bool        YAPI::_apiInitialized   = false; 

std::map<int,yCalibrationHandler> YAPI::_calibHandlers;

// Default cache validity (in [ms]) before reloading data from device. This saves a lots of trafic.
// Note that a value undger 2 ms makes little sense since a USB bus itself has a 2ms roundtrip period
int YAPI::DefaultCacheValidity = 5; 

// Switch to turn off exceptions and use return codes instead, for source-code compatibility
// with languages without exception support like pure C
bool YAPI::ExceptionsDisabled = false; 

// standard error objects
const string YAPI::INVALID_STRING = Y_INVALID_STRING;


yLogFunction            YAPI::LogFunction            = NULL;
yDeviceUpdateCallback   YAPI::DeviceArrivalCallback  = NULL;
yDeviceUpdateCallback   YAPI::DeviceRemovalCallback  = NULL;
yDeviceUpdateCallback   YAPI::DeviceChangeCallback   = NULL;

void YAPI::_yapiLogFunctionFwd(const char *log, u32 loglen)
{
    if(YAPI::LogFunction)
        YAPI::LogFunction(string(log));    
}


void YAPI::_yapiDeviceLogCallbackFwd(YDEV_DESCR devdesc)
{
    YModule     *module;
    yDeviceSt   infos;
    string      errmsg;

    if(YapiWrapper::getDeviceInfo(devdesc, infos, errmsg) != YAPI_SUCCESS) return;
    module = yFindModule(string(infos.serial)+".module");
    YAPI::_notifyLogs(module);
}


void YAPI::_notifyLogs(YModule *module)
{
    yapiEvent    ev;
    
    ev.type   = YAPI_DEV_LOG;
    ev.module = module;
    _data_events.push(ev);
}

void YAPI::_yapiDeviceArrivalCallbackFwd(YDEV_DESCR devdesc)
{
    yapiEvent    ev;
    yDeviceSt    infos;
    string       errmsg;
    vector<YFunction*>::iterator it;
    
    ev.type      = YAPI_FUN_REFRESH;
    for ( it=_FunctionCallbacks.begin() ; it < _FunctionCallbacks.end(); it++ ){
        if ((*it)->functionDescriptor() == Y_FUNCTIONDESCRIPTOR_INVALID){
            ev.fun = *it;
            _data_events.push(ev);
        }
    }
    if (YAPI::DeviceArrivalCallback == NULL) return;
    ev.type      = YAPI_DEV_ARRIVAL;
    //the function is allready thread safe (use yapiLockDeviceCallaback)
    if(YapiWrapper::getDeviceInfo(devdesc, infos, errmsg) != YAPI_SUCCESS) return;
    ev.module = yFindModule(string(infos.serial)+".module");
    ev.module->setImmutableAttributes(&infos);
    _plug_events.push(ev);
}

void YAPI::_yapiDeviceRemovalCallbackFwd(YDEV_DESCR devdesc)
{
    yapiEvent    ev;
    yDeviceSt    infos;
    string       errmsg;

    if (YAPI::DeviceRemovalCallback == NULL) return;
    ev.type   = YAPI_DEV_REMOVAL;
    if(YapiWrapper::getDeviceInfo(devdesc, infos, errmsg) != YAPI_SUCCESS) return;
    ev.module = yFindModule(string(infos.serial)+".module");
    //the function is allready thread safe (use yapiLockDeviceCallaback)
    _plug_events.push(ev);
}

void YAPI::_yapiDeviceChangeCallbackFwd(YDEV_DESCR devdesc)
{
    yapiEvent    ev;
    yDeviceSt    infos;
    string       errmsg;

    if (YAPI::DeviceChangeCallback == NULL) return;
    ev.type      = YAPI_DEV_CHANGE;
    if(YapiWrapper::getDeviceInfo(devdesc, infos, errmsg) != YAPI_SUCCESS) return;
    ev.module = yFindModule(string(infos.serial)+".module");
    ev.module->setImmutableAttributes(&infos);
    //the function is allready thread safe (use yapiLockDeviceCallaback)
    _plug_events.push(ev);
}

void YAPI::_yapiFunctionUpdateCallbackFwd(YAPI_FUNCTION fundesc,const char *value)
{
    yapiEvent    ev;

    //the function is allready thread safe (use yapiLockFunctionCallaback)
    if(value==NULL){
        ev.type      = YAPI_FUN_UPDATE;
    }else{
        ev.type      = YAPI_FUN_VALUE;
        memcpy(ev.value,value,YOCTO_PUBVAL_LEN);
    }
    for (unsigned i=0 ; i< _FunctionCallbacks.size();i++) {
        if (_FunctionCallbacks[i]->functionDescriptor() == fundesc) {            
            ev.fun = _FunctionCallbacks[i];
            _data_events.push(ev);
        }
    }
}

static double decExp[16] = { 
    1.0e-6, 1.0e-5, 1.0e-4, 1.0e-3, 1.0e-2, 1.0e-1, 1.0, 
    1.0e1, 1.0e2, 1.0e3, 1.0e4, 1.0e5, 1.0e6, 1.0e7, 1.0e8, 1.0e9 };

// Convert Yoctopuce 16-bit decimal floats to standard double-precision floats
//
double YAPI::_decimalToDouble(s16 val)
{
    int     negate = 0;
    double  res;
        
    if(val == 0) return 0.0;
    if(val < 0) {
        negate = 1;
        val = -val;
    }
    res = (double)(val & 2047) * decExp[val >> 11];
    
    return (negate ? -res : res);
}

// Convert standard double-precision floats to Yoctopuce 16-bit decimal floats
//
s16 YAPI::_doubleToDecimal(double val)
{
    int     negate = 0;
    double  comp, mant;
    u16     decpow;
    s16     res;
    
    if(val == 0.0) {
        return 0;
    }
    if(val < 0) {
        negate = 1;
        val = -val;
    }
    comp = val / 1999.0;
    decpow = 0;
    while(comp > decExp[decpow] && decpow < 15) {
        decpow++;
    }
    mant = val / decExp[decpow];
    if(decpow == 15 && mant > 2047.0) {
        res = (15 << 11) + 2047; // overflow
    } else {
        res = (decpow << 11) + (u16)floor(mant+.5);
    }
    return (negate ? -res : res);
}

yCalibrationHandler YAPI::_getCalibrationHandler(int calibType)
{
    if(YAPI::_calibHandlers.find(calibType) == YAPI::_calibHandlers.end()) {
        return NULL;
    }
    return YAPI::_calibHandlers[calibType];
}

double YAPI::_applyCalibration(double rawValue, string calibParams, int calibOffset, double resolution)
{    
    const char  *p = calibParams.c_str();
    int         calibType, ival;
    double      fval;
    yCalibrationHandler handler;

    if(rawValue == -DBL_MAX || resolution == -DBL_MAX) {
        return -DBL_MAX;
    }
    if(!*p) return rawValue;
    calibType = atoi(p);
    if(calibType == 0) {
        return rawValue;
    }
    handler = YAPI::_getCalibrationHandler(calibType);
    if(!handler) {
        return -DBL_MAX;
    }

    // parse calibration parameters
    intArr   iParams;
    floatArr rawPt;
    floatArr calPt;
    while(*p && *p != ',') p++;
    while(*p) {
        p++; // skip ','
        ival = atoi(p);
        if(calibType <= 10) {
            fval = (ival + calibOffset) * resolution;
        } else {
            fval = YAPI::_decimalToDouble(ival);
        }
        while(*p && *p != ',') p++;
        iParams.push_back(ival);
        if(iParams.size() & 1) {
            rawPt.push_back(fval);
        } else {
            calPt.push_back(fval);
        }
    }
    if(iParams.size() < 10) {
        return -DBL_MAX;
    }
    return handler(rawValue, calibType, iParams, rawPt, calPt);    
}
  
/**
 * Returns the version identifier for the Yoctopuce library in use.
 * The version is a string in the form "Major.Minor.Build",
 * for instance "1.01.5535". For languages using an external
 * DLL (for instance C#, VisualBasic or Delphi), the character string
 * includes as well the DLL version, for instance
 * "1.01.5535 (1.01.5439)".
 * 
 * If you want to verify in your code that the library version is
 * compatible with the version that you have used during development,
 * verify that the major number is strictly equal and that the minor
 * number is greater or equal. The build number is not relevant
 * with respect to the library compatibility.
 * 
 * @return a character string describing the library version.
 */
string YAPI::GetAPIVersion(void)
{
    string version;
    string date;
    YapiWrapper::getAPIVersion(version,date);
    return version;
}


/**
 * Initializes the Yoctopuce programming library explicitly.
 * It is not strictly needed to call yInitAPI(), as the library is
 * automatically  initialized when calling yRegisterHub() for the
 * first time.
 * 
 * When Y_DETECT_NONE is used as detection mode,
 * you must explicitly use yRegisterHub() to point the API to the
 * VirtualHub on which your devices are connected before trying to access them.
 * 
 * @param mode : an integer corresponding to the type of automatic
 *         device detection to use. Possible values are
 *         Y_DETECT_NONE, Y_DETECT_USB, Y_DETECT_NET,
 *         and Y_DETECT_ALL.
 * @param errmsg : a string passed by reference to receive any error message.
 * 
 * @return YAPI_SUCCESS when the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
YRETCODE YAPI::InitAPI(int mode, string& errmsg)
{
    char errbuf[YOCTO_ERRMSG_LEN];
    int  i;
    
    if(YAPI::_apiInitialized) 
        return YAPI_SUCCESS;
    YRETCODE res = yapiInitAPI(mode, errbuf);
    if(YISERR(res)) {
        errmsg = errbuf;
        return res;
    }
    yapiRegisterLogFunction(YAPI::_yapiLogFunctionFwd);
    yapiRegisterDeviceLogCallback(YAPI::_yapiDeviceLogCallbackFwd);
    yapiRegisterDeviceArrivalCallback(YAPI::_yapiDeviceArrivalCallbackFwd);
    yapiRegisterDeviceRemovalCallback(YAPI::_yapiDeviceRemovalCallbackFwd);
    yapiRegisterDeviceChangeCallback(YAPI::_yapiDeviceChangeCallbackFwd);
    yapiRegisterFunctionUpdateCallback(YAPI::_yapiFunctionUpdateCallbackFwd);
    yInitializeCriticalSection(&_updateDeviceList_CS);
    yInitializeCriticalSection(&_handleEvent_CS);
    for(i = 0; i <= 20; i++) {
        YAPI::RegisterCalibrationHandler(i, YAPI::LinearCalibrationHandler);
    }
    YAPI::_apiInitialized = true;

    return YAPI_SUCCESS;
}

/**
 * Frees dynamically allocated memory blocks used by the Yoctopuce library.
 * It is generally not required to call this function, unless you
 * want to free all dynamically allocated memory blocks in order to
 * track a memory leak for instance.
 * You should not call any other library function after calling
 * yFreeAPI(), or your program will crash.
 */
void YAPI::FreeAPI(void)
{
    if(YAPI::_apiInitialized) {
        yapiFreeAPI();
        YAPI::_apiInitialized = false;
        yDeleteCriticalSection(&_updateDeviceList_CS);
        yDeleteCriticalSection(&_handleEvent_CS);
    }
}

/**
 * Registers a log callback function. This callback will be called each time
 * the API have something to say. Quite usefull to debug the API.
 * 
 * @param logfun : a procedure taking a string parameter, or null
 *         to unregister a previously registered  callback.
 */
void YAPI::RegisterLogFunction(yLogFunction logfun)
{
    YAPI::LogFunction = logfun;
}

/**
 * Register a callback function, to be called each time
 * a device is pluged. This callback will be invoked while yUpdateDeviceList
 * is running. You will have to call this function on a regular basis.
 * 
 * @param arrivalCallback : a procedure taking a YModule parameter, or null
 *         to unregister a previously registered  callback.
 */
void YAPI::RegisterDeviceArrivalCallback(yDeviceUpdateCallback arrivalCallback)
{
    YAPI::DeviceArrivalCallback = arrivalCallback;
    if(arrivalCallback) {
        YModule *mod =yFirstModule();
        while(mod){
            if(mod->isOnline()){
                yapiLockDeviceCallBack(NULL);
                _yapiDeviceArrivalCallbackFwd(mod->functionDescriptor());
                yapiUnlockDeviceCallBack(NULL);
            }
            mod = mod->nextModule();
        }
    }
}

/**
 * Register a callback function, to be called each time
 * a device is unpluged. This callback will be invoked while yUpdateDeviceList
 * is running. You will have to call this function on a regular basis.
 * 
 * @param removalCallback : a procedure taking a YModule parameter, or null
 *         to unregister a previously registered  callback.
 */
void YAPI::RegisterDeviceRemovalCallback(yDeviceUpdateCallback removalCallback)
{
    YAPI::DeviceRemovalCallback = removalCallback;
}

void YAPI::RegisterDeviceChangeCallback(yDeviceUpdateCallback changeCallback)
{
    YAPI::DeviceChangeCallback = changeCallback;
}

// Register a new value calibration handler for a given calibration type
//
void YAPI::RegisterCalibrationHandler(int calibrationType, yCalibrationHandler calibrationHandler)
{
    YAPI::_calibHandlers[calibrationType] = calibrationHandler;
}

// Standard value calibration handler (n-point linear error correction)
//
double YAPI::LinearCalibrationHandler(double rawValue, int calibType, intArr params, floatArr rawValues, floatArr refValues)
{
    // calibration types n=1..10 and 11.20 are meant for linear calibration using n points
    int    npt = calibType % 10;
    double x   = rawValues[0];
    double adj = refValues[0] - x;
    int    i   = 0;
    
    if(npt > (int)rawValues.size()) npt = (int)rawValues.size();
    if(npt > (int)refValues.size()) npt = (int)refValues.size();
    while(rawValue > rawValues[i] && ++i < npt) {
        double x2   = x;
        double adj2 = adj;
        
        x   = rawValues[i];
        adj = refValues[i] - x;
        
        if(rawValue < x && x > x2) {
            adj = adj2 + (adj - adj2) * (rawValue - x2) / (x - x2);
        }
    }
    return rawValue + adj;
}


/**
 * Setup the Yoctopuce library to use modules connected on a given machine.
 * When using Yoctopuce modules through the VirtualHub gateway,
 * you should provide as parameter the address of the machine on which the
 * VirtualHub software is running (typically "http://127.0.0.1:4444",
 * which represents the local machine).
 * When you use a language which has direct access to the USB hardware,
 * you can use the pseudo-URL "usb" instead.
 * 
 * Be aware that only one application can use direct USB access at a
 * given time on a machine. Multiple access would cause conflicts
 * while trying to access the USB modules. In particular, this means
 * that you must stop the VirtualHub software before starting
 * an application that uses direct USB access. The workaround
 * for this limitation is to setup the library to use the VirtualHub
 * rather than direct USB access.
 * 
 * If acces control has been activated on the VirtualHub you want to
 * reach, the URL parameter should look like:
 * 
 * http://username:password@adresse:port
 * 
 * @param url : a string containing either "usb" or the
 *         root URL of the hub to monitor
 * @param errmsg : a string passed by reference to receive any error message.
 * 
 * @return YAPI_SUCCESS when the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
YRETCODE YAPI::RegisterHub(const string& url, string& errmsg)
{
    char        errbuf[YOCTO_ERRMSG_LEN];
    YRETCODE    res;
    if(!YAPI::_apiInitialized) {
        res = YAPI::InitAPI(0, errmsg);
        if(YISERR(res)) return res;
    }
    res = yapiRegisterHub(url.c_str(), errbuf);
    if(YISERR(res)) {
        errmsg = errbuf;
    }
    return res;
}

/**
 *
 */
YRETCODE YAPI::PreregisterHub(const string& url, string& errmsg)
{
    char        errbuf[YOCTO_ERRMSG_LEN];
    YRETCODE    res;
    if(!YAPI::_apiInitialized) {
        res = YAPI::InitAPI(0, errmsg);
        if(YISERR(res)) return res;
    }
    res = yapiPreregisterHub(url.c_str(), errbuf);
    if(YISERR(res)) {
        errmsg = errbuf;
    }
    return res;
}
/**
 * Setup the Yoctopuce library to no more use modules connected on a previously
 * registered machine with RegisterHub.
 * 
 * @param url : a string containing either "usb" or the
 *         root URL of the hub to monitor
 */
void YAPI::UnregisterHub(const string& url)
{
    if(!YAPI::_apiInitialized){
        return;
    }
    yapiUnregisterHub(url.c_str());
}


/**
 * Triggers a (re)detection of connected Yoctopuce modules.
 * The library searches the machines or USB ports previously registered using
 * yRegisterHub(), and invokes any user-defined callback function
 * in case a change in the list of connected devices is detected.
 * 
 * This function can be called as frequently as desired to refresh the device list
 * and to make the application aware of hot-plug events.
 * 
 * @param errmsg : a string passed by reference to receive any error message.
 * 
 * @return YAPI_SUCCESS when the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
YRETCODE YAPI::UpdateDeviceList(string& errmsg)
{
    if(!YAPI::_apiInitialized) {
        YRETCODE res = YAPI::InitAPI(0, errmsg);
        if(YISERR(res)) return res;
    }
    // prevent reentrance into this function
    yEnterCriticalSection(&_updateDeviceList_CS);
    // call the updateDeviceList of the yapi layer
    // yapi know when it is needed to do a full update
    YRETCODE res = YapiWrapper::updateDeviceList(false,errmsg);
    if(YISERR(res)) {
        yLeaveCriticalSection(&_updateDeviceList_CS);
        return res;
    }
    // handle other notification
    res = YapiWrapper::handleEvents(errmsg);
    if(YISERR(res)) {
        yLeaveCriticalSection(&_updateDeviceList_CS);
        return res;
    }
    // unpop plug/unplug event and call user callback
    while(!_plug_events.empty()){
        yapiEvent   ev;
        yapiLockDeviceCallBack(NULL);
        if(_plug_events.empty()){
            yapiUnlockDeviceCallBack(NULL);
            break;
        }
        ev = _plug_events.front();
        _plug_events.pop();
        yapiUnlockDeviceCallBack(NULL);
        switch(ev.type){
            case YAPI_DEV_ARRIVAL:
                if(!YAPI::DeviceArrivalCallback) break;
                YAPI::DeviceArrivalCallback(ev.module);    
                break;
            case YAPI_DEV_REMOVAL:
                if(!YAPI::DeviceRemovalCallback) break;
                YAPI::DeviceRemovalCallback(ev.module);    
                break;
            case YAPI_DEV_CHANGE:
                if(!YAPI::DeviceChangeCallback) break;
                YAPI::DeviceChangeCallback(ev.module);    
                break;
            default:
                break;
        }
    }
    yLeaveCriticalSection(&_updateDeviceList_CS);
    return YAPI_SUCCESS;
}

/**
 * Maintains the device-to-library communication channel.
 * If your program includes significant loops, you may want to include
 * a call to this function to make sure that the library takes care of
 * the information pushed by the modules on the communication channels.
 * This is not strictly necessary, but it may improve the reactivity
 * of the library for the following commands.
 * 
 * This function may signal an error in case there is a communication problem
 * while contacting a module.
 * 
 * @param errmsg : a string passed by reference to receive any error message.
 * 
 * @return YAPI_SUCCESS when the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
YRETCODE YAPI::HandleEvents(string& errmsg)
{
    YRETCODE    res;
    
    // prevent reentrance into this function
    yEnterCriticalSection(&_handleEvent_CS);
     // handle other notification
    res = YapiWrapper::handleEvents(errmsg);
    if(YISERR(res)) {
        yLeaveCriticalSection(&_handleEvent_CS);
        return res;
    }
    // pop data event and call user callback
    while (!_data_events.empty()) {
        yapiEvent       ev;
        yapiLockFunctionCallBack(NULL);
        if (_data_events.empty()) {
            yapiUnlockFunctionCallBack(NULL);
            break;
        }
        ev = _data_events.front();
        _data_events.pop();
        yapiUnlockFunctionCallBack(NULL);
        switch (ev.type) {
            case YAPI_FUN_VALUE:
                ev.fun->advertiseValue((string)ev.value);
                break;
            case YAPI_FUN_REFRESH:
                ev.fun->isOnline();
                break;
            case YAPI_DEV_LOG:
                ev.module->checkLogs();
            default:
                break;
        }
    }
    yLeaveCriticalSection(&_handleEvent_CS); 
    return YAPI_SUCCESS;
}

/**
 * Pauses the execution flow for a specified duration.
 * This function implements a passive waiting loop, meaning that it does not
 * consume CPU cycles significatively. The processor is left available for
 * other threads and processes. During the pause, the library nevertheless
 * reads from time to time information from the Yoctopuce modules by
 * calling yHandleEvents(), in order to stay up-to-date.
 * 
 * This function may signal an error in case there is a communication problem
 * while contacting a module.
 * 
 * @param ms_duration : an integer corresponding to the duration of the pause,
 *         in milliseconds.
 * @param errmsg : a string passed by reference to receive any error message.
 * 
 * @return YAPI_SUCCESS when the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
YRETCODE YAPI::Sleep(unsigned ms_duration, string& errmsg)
{
    char         errbuf[YOCTO_ERRMSG_LEN];
    YRETCODE    res;

    u64         waituntil=YAPI::GetTickCount()+ms_duration;
    do{
       res = YAPI::HandleEvents(errmsg);
        if(YISERR(res)) {
            errmsg = errbuf;
            return res;
        }
        if(waituntil>YAPI::GetTickCount()){
            res = yapiSleep(3, errbuf);
            if(YISERR(res)) {
                errmsg = errbuf;
                return res;
            }
        }
    }while(waituntil>YAPI::GetTickCount());
     
    return YAPI_SUCCESS;
}

/**
 * Returns the current value of a monotone millisecond-based time counter.
 * This counter can be used to compute delays in relation with
 * Yoctopuce devices, which also uses the milisecond as timebase.
 * 
 * @return a long integer corresponding to the millisecond counter.
 */
u64 YAPI::GetTickCount(void)
{
    return yapiGetTickCount();
}

/**
 * Checks if a given string is valid as logical name for a module or a function.
 * A valid logical name has a maximum of 19 characters, all among
 * A..Z, a..z, 0..9, _, and -.
 * If you try to configure a logical name with an incorrect string,
 * the invalid characters are ignored.
 * 
 * @param name : a string containing the name to check.
 * 
 * @return true if the name is valid, false otherwise.
 */
bool YAPI::CheckLogicalName(const string& name)
{
    return (yapiCheckLogicalName(name.c_str())!=0);
}

u16 YapiWrapper::getAPIVersion(string& version,string& date)
{
    const char  *_ver, *_date;    
    u16 res = yapiGetAPIVersion(&_ver, &_date);
    version     = _ver;
    date        = _date;    
    return res;
}

YDEV_DESCR YapiWrapper::getDevice(const string& device_str, string& errmsg)
{
    char        errbuf[YOCTO_ERRMSG_LEN];
    YDEV_DESCR     res;
    
    res = yapiGetDevice(device_str.data(), errbuf);
    if(YISERR(res)) {
        errmsg = errbuf;
    }
    return (YDEV_DESCR)res;
}

int YapiWrapper::getAllDevices(vector<YDEV_DESCR>& buffer, string& errmsg)
{
    char    errbuf[YOCTO_ERRMSG_LEN];
    int     n_elems = 32;
    int     initsize = n_elems * sizeof(YDEV_DESCR);
    int     neededsize, res;
    YDEV_DESCR *ptr = new YDEV_DESCR[n_elems];
    
    res = yapiGetAllDevices(ptr, initsize, &neededsize, errbuf);
    if(YISERR(res)) {
        delete [] ptr;
        errmsg = errbuf;
        return (YRETCODE)res;
    }
    if(neededsize > initsize) {
        delete [] ptr;
        n_elems = neededsize / sizeof(YDEV_DESCR);
        initsize = n_elems * sizeof(YDEV_DESCR);
        ptr = new YDEV_DESCR[n_elems];
        res = yapiGetAllDevices(ptr, initsize, NULL, errbuf);
        if(YISERR(res)) {
            delete [] ptr;
            errmsg = errbuf;
            return (YRETCODE)res;
        }
    }
    buffer = vector<YDEV_DESCR>(ptr, ptr+res);
    delete [] ptr;
    
    return res;
}

YRETCODE YapiWrapper::getDeviceInfo(YDEV_DESCR devdesc, yDeviceSt& infos, string& errmsg)
{
    char        errbuf[YOCTO_ERRMSG_LEN];
    YRETCODE    res;
    
    res = yapiGetDeviceInfo(devdesc, &infos, errbuf);
    if(YISERR(res)) {
        errmsg = errbuf;
    }
    return res;
}

YFUN_DESCR YapiWrapper::getFunction(const string& class_str, const string& function_str, string& errmsg)
{
    char errbuf[YOCTO_ERRMSG_LEN];
    
    YFUN_DESCR res = yapiGetFunction(class_str.data(), function_str.data(), errbuf);
    if(YISERR(res)) {
        errmsg = errbuf;
    }
    return res;
}

int YapiWrapper::getFunctionsByClass(const string& class_str, YFUN_DESCR prevfundesc, vector<YFUN_DESCR>& buffer, int maxsize, string& errmsg)
{
    char    errbuf[YOCTO_ERRMSG_LEN];
    int     n_elems = 32;
    int     initsize = n_elems * sizeof(YDEV_DESCR);
    int     neededsize;
    YFUN_DESCR *ptr = new YFUN_DESCR[n_elems];
    
    int res = yapiGetFunctionsByClass(class_str.data(), prevfundesc, ptr, initsize, &neededsize, errbuf);
    if(YISERR(res)) {
        delete [] ptr;
        errmsg = errbuf;
        return res;
    }
    if(neededsize > initsize) {
        delete [] ptr;
        n_elems = neededsize / sizeof(YFUN_DESCR);
        initsize = n_elems * sizeof(YFUN_DESCR);
        ptr = new YFUN_DESCR[n_elems];
        res = yapiGetFunctionsByClass(class_str.data(), prevfundesc, ptr, initsize, NULL, errbuf);
        if(YISERR(res)) {
            delete [] ptr;
            errmsg = errbuf;
            return res;
        }
    }
    buffer = vector<YFUN_DESCR>(ptr, ptr+res);
    delete [] ptr;
    
    return res;
}

int YapiWrapper::getFunctionsByDevice(YDEV_DESCR devdesc, YFUN_DESCR prevfundesc, vector<YFUN_DESCR>& buffer, int maxsize, string& errmsg)
{
    char    errbuf[YOCTO_ERRMSG_LEN];
    int     n_elems = 32;
    int     initsize = n_elems * sizeof(YDEV_DESCR);
    int     neededsize;
    YFUN_DESCR *ptr = new YFUN_DESCR[n_elems];
    
    int res = yapiGetFunctionsByDevice(devdesc, prevfundesc, ptr, initsize, &neededsize, errbuf);
    if(YISERR(res)) {
        delete [] ptr;
        errmsg = errbuf;
        return res;
    }
    if(neededsize > initsize) {
        delete [] ptr;
        n_elems = neededsize / sizeof(YFUN_DESCR);
        initsize = n_elems * sizeof(YFUN_DESCR);
        ptr = new YFUN_DESCR[n_elems];
        res = yapiGetFunctionsByDevice(devdesc, prevfundesc, ptr, initsize, NULL, errbuf);
        if(YISERR(res)) {
            delete [] ptr;
            errmsg = errbuf;
            return res;
        }
    }
    buffer = vector<YFUN_DESCR>(ptr, ptr+res);
    delete [] ptr;
    
    return res;
}

YDEV_DESCR YapiWrapper::getDeviceByFunction(YFUN_DESCR fundesc, string& errmsg)
{
    char    errbuf[YOCTO_ERRMSG_LEN];
    YDEV_DESCR dev;
    
    int res = yapiGetFunctionInfo(fundesc, &dev, NULL, NULL, NULL, NULL, errbuf);
    if(YISERR(res)) {
        errmsg = errbuf;
        return res;
    }
    
    return dev;
}

YRETCODE YapiWrapper::getFunctionInfo(YFUN_DESCR fundesc, YDEV_DESCR& devdescr, string& serial, string& funcId, string& funcName, string& funcVal, string& errmsg)
{
    char    errbuf[YOCTO_ERRMSG_LEN];
    char    snum[YOCTO_SERIAL_LEN];
    char    fnid[YOCTO_FUNCTION_LEN];
    char    fnam[YOCTO_LOGICAL_LEN];
    char    fval[YOCTO_PUBVAL_LEN];
    
    YRETCODE res = yapiGetFunctionInfo(fundesc, &devdescr, snum, fnid, fnam, fval, errbuf);
    if(YISERR(res)) {
        errmsg = errbuf;
    } else {
        serial = snum;
        funcId = fnid;
        funcName = fnam;
        funcVal = fval;
    }
    
    return res;
}

YRETCODE YapiWrapper::updateDeviceList(bool forceupdate,string& errmsg)
{
    char        errbuf[YOCTO_ERRMSG_LEN];
    YRETCODE    res = yapiUpdateDeviceList(forceupdate?1:0,errbuf);
    if(YISERR(res)) {
        errmsg = errbuf;
        return res;
    }
    return YAPI_SUCCESS;
}

YRETCODE YapiWrapper::handleEvents(string& errmsg)
{
    char        errbuf[YOCTO_ERRMSG_LEN];
    YRETCODE    res = yapiHandleEvents(errbuf);
    if(YISERR(res)) {
        errmsg = errbuf;
        return res;
    }
    return YAPI_SUCCESS;
}


string  YapiWrapper::ysprintf(const char *fmt, ...)
{
    va_list     args;
    char        buffer[2048];
    va_start( args, fmt );
#ifdef WINDOWS_API
    vsprintf_s(buffer,sizeof(buffer),fmt,args);
#else
    vsprintf(buffer,fmt,args);
#endif
    va_end(args);
    return (string)buffer;
}



//--- (generated code: YModule implementation)

const string YModule::PRODUCTNAME_INVALID = "!INVALID!";
const string YModule::SERIALNUMBER_INVALID = "!INVALID!";
const string YModule::LOGICALNAME_INVALID = "!INVALID!";
const string YModule::FIRMWARERELEASE_INVALID = "!INVALID!";

std::map<string,YModule*> YModule::_ModuleCache;

int YModule::_parse(yJsonStateMachine& j)
{
    if(yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_STRUCT) {
    failed:
        return -1;
    }
    while(yJsonParse(&j) == YJSON_PARSE_AVAIL && j.st == YJSON_PARSE_MEMBNAME) {
        if(!strcmp(j.token, "productName")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _productName =  _parseString(j);
        } else if(!strcmp(j.token, "serialNumber")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _serialNumber =  _parseString(j);
        } else if(!strcmp(j.token, "logicalName")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _logicalName =  _parseString(j);
        } else if(!strcmp(j.token, "productId")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _productId =  atoi(j.token);
        } else if(!strcmp(j.token, "productRelease")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _productRelease =  atoi(j.token);
        } else if(!strcmp(j.token, "firmwareRelease")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _firmwareRelease =  _parseString(j);
        } else if(!strcmp(j.token, "persistentSettings")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _persistentSettings =  (Y_PERSISTENTSETTINGS_enum)atoi(j.token);
        } else if(!strcmp(j.token, "luminosity")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _luminosity =  atoi(j.token);
        } else if(!strcmp(j.token, "beacon")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _beacon =  (Y_BEACON_enum)atoi(j.token);
        } else if(!strcmp(j.token, "upTime")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _upTime =  atoi(j.token);
        } else if(!strcmp(j.token, "usbCurrent")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _usbCurrent =  atoi(j.token);
        } else if(!strcmp(j.token, "rebootCountdown")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _rebootCountdown =  atoi(j.token);
        } else if(!strcmp(j.token, "usbBandwidth")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _usbBandwidth =  (Y_USBBANDWIDTH_enum)atoi(j.token);
        } else {
            // ignore unknown field
            yJsonSkip(&j, 1);
        }
    }
    if(j.st != YJSON_PARSE_STRUCT) goto failed;
    return 0;
}

/**
 * Returns the commercial name of the module, as set by the factory.
 * 
 * @return a string corresponding to the commercial name of the module, as set by the factory
 * 
 * On failure, throws an exception or returns Y_PRODUCTNAME_INVALID.
 */
string YModule::get_productName(void)
{
    if(_productName == Y_PRODUCTNAME_INVALID) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_PRODUCTNAME_INVALID;
    }
    return _productName;
}

/**
 * Returns the serial number of the module, as set by the factory.
 * 
 * @return a string corresponding to the serial number of the module, as set by the factory
 * 
 * On failure, throws an exception or returns Y_SERIALNUMBER_INVALID.
 */
string YModule::get_serialNumber(void)
{
    if(_serialNumber == Y_SERIALNUMBER_INVALID) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_SERIALNUMBER_INVALID;
    }
    return _serialNumber;
}

/**
 * Returns the logical name of the module.
 * 
 * @return a string corresponding to the logical name of the module
 * 
 * On failure, throws an exception or returns Y_LOGICALNAME_INVALID.
 */
string YModule::get_logicalName(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_LOGICALNAME_INVALID;
    }
    return _logicalName;
}

/**
 * Changes the logical name of the module. You can use yCheckLogicalName()
 * prior to this call to make sure that your parameter is valid.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 * 
 * @param newval : a string corresponding to the logical name of the module
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YModule::set_logicalName(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("logicalName", rest_val);
}

/**
 * Returns the USB device identifier of the module.
 * 
 * @return an integer corresponding to the USB device identifier of the module
 * 
 * On failure, throws an exception or returns Y_PRODUCTID_INVALID.
 */
int YModule::get_productId(void)
{
    if(_productId == Y_PRODUCTID_INVALID) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_PRODUCTID_INVALID;
    }
    return _productId;
}

/**
 * Returns the hardware release version of the module.
 * 
 * @return an integer corresponding to the hardware release version of the module
 * 
 * On failure, throws an exception or returns Y_PRODUCTRELEASE_INVALID.
 */
int YModule::get_productRelease(void)
{
    if(_productRelease == Y_PRODUCTRELEASE_INVALID) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_PRODUCTRELEASE_INVALID;
    }
    return _productRelease;
}

/**
 * Returns the version of the firmware embedded in the module.
 * 
 * @return a string corresponding to the version of the firmware embedded in the module
 * 
 * On failure, throws an exception or returns Y_FIRMWARERELEASE_INVALID.
 */
string YModule::get_firmwareRelease(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_FIRMWARERELEASE_INVALID;
    }
    return _firmwareRelease;
}

/**
 * Returns the current state of persistent module settings.
 * 
 * @return a value among Y_PERSISTENTSETTINGS_LOADED, Y_PERSISTENTSETTINGS_SAVED and
 * Y_PERSISTENTSETTINGS_MODIFIED corresponding to the current state of persistent module settings
 * 
 * On failure, throws an exception or returns Y_PERSISTENTSETTINGS_INVALID.
 */
Y_PERSISTENTSETTINGS_enum YModule::get_persistentSettings(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_PERSISTENTSETTINGS_INVALID;
    }
    return _persistentSettings;
}

int YModule::set_persistentSettings(Y_PERSISTENTSETTINGS_enum newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
    return _setAttr("persistentSettings", rest_val);
}

/**
 * Saves current settings in the nonvolatile memory of the module.
 * Warning: the number of allowed save operations during a module life is
 * limited (about 100000 cycles). Do not call this function within a loop.
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YModule::saveToFlash(void)
{
    string rest_val;
    rest_val = "1";
    return _setAttr("persistentSettings", rest_val);
}

/**
 * Reloads the settings stored in the nonvolatile memory, as
 * when the module is powered on.
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YModule::revertFromFlash(void)
{
    string rest_val;
    rest_val = "0";
    return _setAttr("persistentSettings", rest_val);
}

/**
 * Returns the luminosity of the  module informative leds (from 0 to 100).
 * 
 * @return an integer corresponding to the luminosity of the  module informative leds (from 0 to 100)
 * 
 * On failure, throws an exception or returns Y_LUMINOSITY_INVALID.
 */
int YModule::get_luminosity(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_LUMINOSITY_INVALID;
    }
    return _luminosity;
}

/**
 * Changes the luminosity of the module informative leds. The parameter is a
 * value between 0 and 100.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 * 
 * @param newval : an integer corresponding to the luminosity of the module informative leds
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YModule::set_luminosity(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
    return _setAttr("luminosity", rest_val);
}

/**
 * Returns the state of the localization beacon.
 * 
 * @return either Y_BEACON_OFF or Y_BEACON_ON, according to the state of the localization beacon
 * 
 * On failure, throws an exception or returns Y_BEACON_INVALID.
 */
Y_BEACON_enum YModule::get_beacon(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_BEACON_INVALID;
    }
    return _beacon;
}

/**
 * Turns on or off the module localization beacon.
 * 
 * @param newval : either Y_BEACON_OFF or Y_BEACON_ON
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YModule::set_beacon(Y_BEACON_enum newval)
{
    string rest_val;
    rest_val = (newval>0 ? "1" : "0");
    return _setAttr("beacon", rest_val);
}

/**
 * Returns the number of milliseconds spent since the module was powered on.
 * 
 * @return an integer corresponding to the number of milliseconds spent since the module was powered on
 * 
 * On failure, throws an exception or returns Y_UPTIME_INVALID.
 */
unsigned YModule::get_upTime(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_UPTIME_INVALID;
    }
    return _upTime;
}

/**
 * Returns the current consumed by the module on the USB bus, in milli-amps.
 * 
 * @return an integer corresponding to the current consumed by the module on the USB bus, in milli-amps
 * 
 * On failure, throws an exception or returns Y_USBCURRENT_INVALID.
 */
unsigned YModule::get_usbCurrent(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_USBCURRENT_INVALID;
    }
    return _usbCurrent;
}

/**
 * Returns the remaining number of seconds before the module restarts, or zero when no
 * reboot has been scheduled.
 * 
 * @return an integer corresponding to the remaining number of seconds before the module restarts, or zero when no
 *         reboot has been scheduled
 * 
 * On failure, throws an exception or returns Y_REBOOTCOUNTDOWN_INVALID.
 */
int YModule::get_rebootCountdown(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_REBOOTCOUNTDOWN_INVALID;
    }
    return _rebootCountdown;
}

int YModule::set_rebootCountdown(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("rebootCountdown", rest_val);
}

/**
 * Schedules a simple module reboot after the given number of seconds.
 * 
 * @param secBeforeReboot : number of seconds before rebooting
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YModule::reboot(int secBeforeReboot)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", secBeforeReboot); rest_val = string(buf);
    return _setAttr("rebootCountdown", rest_val);
}

/**
 * Schedules a module reboot into special firmware update mode.
 * 
 * @param secBeforeReboot : number of seconds before rebooting
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YModule::triggerFirmwareUpdate(int secBeforeReboot)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", -secBeforeReboot); rest_val = string(buf);
    return _setAttr("rebootCountdown", rest_val);
}

/**
 * Returns the number of USB interfaces used by the module.
 * 
 * @return either Y_USBBANDWIDTH_SIMPLE or Y_USBBANDWIDTH_DOUBLE, according to the number of USB
 * interfaces used by the module
 * 
 * On failure, throws an exception or returns Y_USBBANDWIDTH_INVALID.
 */
Y_USBBANDWIDTH_enum YModule::get_usbBandwidth(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_USBBANDWIDTH_INVALID;
    }
    return _usbBandwidth;
}

/**
 * Changes the number of USB interfaces used by the module. You must reboot the module
 * after changing this setting.
 * 
 * @param newval : either Y_USBBANDWIDTH_SIMPLE or Y_USBBANDWIDTH_DOUBLE, according to the number of
 * USB interfaces used by the module
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YModule::set_usbBandwidth(Y_USBBANDWIDTH_enum newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%u", newval); rest_val = string(buf);
    return _setAttr("usbBandwidth", rest_val);
}

/**
 * Downloads the specified built-in file and returns a binary buffer with its content.
 * 
 * @param pathname : name of the new file to load
 * 
 * @return a binary buffer with the file content
 * 
 * On failure, throws an exception or returns an empty content.
 */
string YModule::download(string pathname)
{
    return this->_download(pathname);
    
}

/**
 * Returns the icon of the module. The icon is a PNG image and does not
 * exceeds 1536 bytes.
 * 
 * @return a binary buffer with module icon, in png format.
 */
string YModule::get_icon2d()
{
    return this->_download("icon2d.png");
    
}

YModule *YModule::nextModule(void)
{
    string  hwid;
    
    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return yFindModule(hwid);
}


YModule* YModule::FindModule(const string& func)
{
    if(YModule::_ModuleCache.find(func) != YModule::_ModuleCache.end())
        return YModule::_ModuleCache[func];
    
    YModule *newModule = new YModule(func);
    YModule::_ModuleCache[func] = newModule;
    
    return newModule;
}

YModule* YModule::FirstModule(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;
    
    if(YISERR(YapiWrapper::getFunctionsByClass("Module", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YModule::FindModule(serial+"."+funcId);
}

//--- (end of generated code: YModule implementation)

// Return a string that describes the function (class and logical name or hardware id)
string YModule::get_friendlyName(void)
{
    YFUN_DESCR   fundescr,moddescr;
    YDEV_DESCR   devdescr;
    string       errmsg, serial, funcId, funcName, funcValue;
    string       mod_serial, mod_funcId,mod_funcname;
    
    fundescr = YapiWrapper::getFunction(_className, _func, errmsg);
    if(!YISERR(fundescr) && !YISERR(YapiWrapper::getFunctionInfo(fundescr, devdescr, serial, funcId, funcName, funcValue, errmsg))) {
        moddescr = YapiWrapper::getFunction("Module", serial, errmsg);
        if(!YISERR(moddescr) && !YISERR(YapiWrapper::getFunctionInfo(moddescr, devdescr, mod_serial, mod_funcId, mod_funcname, funcValue, errmsg))) {
            if(mod_funcname!="") {
                return mod_funcname;
            }
        }
        return serial;
    }
    return Y_FRIENDLYNAME_INVALID;
}




void YModule::setImmutableAttributes(yDeviceSt *infos)
{
    _serialNumber = (string) infos->serial;
    _productName  = (string) infos->productname;
    _productId    =  infos->deviceid;
}


// Return the properties of the nth function of our device
YRETCODE YModule::_getFunction(int idx, string& serial, string& funcId, string& funcName, string& funcVal, string& errmsg)
{
    vector<YFUN_DESCR> *functions;
    YDevice     *dev;
    int         res;
    YFUN_DESCR   fundescr;
    YDEV_DESCR     devdescr;
    
    // retrieve device object
    res = _getDevice(dev, errmsg);
    if(YISERR(res)) {
        _throw((YRETCODE)res, errmsg);
        return (YRETCODE)res;
    }
    
    // get reference to all functions from the device
    res = dev->getFunctions(&functions, errmsg);
    if(YISERR(res)) return (YRETCODE)res;
    
    // get latest function info from yellow pages
    fundescr = functions->at(idx);
    res = YapiWrapper::getFunctionInfo(fundescr, devdescr, serial, funcId, funcName, funcVal, errmsg);
    if(YISERR(res)) return (YRETCODE)res;
    
    return YAPI_SUCCESS;
}

// Retrieve the number of functions (beside "module") in the device
int YModule::functionCount()
{
    vector<YFUN_DESCR> *functions;
    YDevice     *dev;
    string      errmsg;
    int         res;
    
    res = _getDevice(dev, errmsg);
    if(YISERR(res)) {
        _throw((YRETCODE)res, errmsg);
        return (YRETCODE)res;
    }
    res = dev->getFunctions(&functions, errmsg);
    if(YISERR(res)) {
        _throw((YRETCODE)res, errmsg);
        return (YRETCODE)res;
    }
    return (int)functions->size();
}

// Retrieve the Id of the nth function (beside "module") in the device
string YModule::functionId(int functionIndex)
{
    string      serial, funcId, funcName, funcVal, errmsg;
    
    int res = _getFunction(functionIndex, serial, funcId, funcName, funcVal, errmsg);
    if(YISERR(res)) {
        _throw((YRETCODE)res, errmsg);
        return YAPI::INVALID_STRING;
    }
    
    return funcId;
}  

// Retrieve the logical name of the nth function (beside "module") in the device
string YModule::functionName(int functionIndex)
{
    string      serial, funcId, funcName, funcVal, errmsg;
    
    int res = _getFunction(functionIndex, serial, funcId, funcName, funcVal, errmsg);
    if(YISERR(res)) {
        _throw((YRETCODE)res, errmsg);
        return YAPI::INVALID_STRING;
    }
    
    return funcName;
}

// Retrieve the advertised value of the nth function (beside "module") in the device
string YModule::functionValue(int functionIndex)
{
    string      serial, funcId, funcName, funcVal, errmsg;
    
    int res = _getFunction(functionIndex, serial, funcId, funcName, funcVal, errmsg);
    if(YISERR(res)) {
        _throw((YRETCODE)res, errmsg);
        return YAPI::INVALID_STRING;
    }    
    
    return funcVal;
}

void YModule::registerLogCallback(YModuleLogCallback callback)
{
    string   errmsg;
    _logCallback = callback;
    yapiLockFunctionCallBack(NULL);
    YAPI::_notifyLogs(this);
    yapiUnlockFunctionCallBack(NULL);
}

void YModule::checkLogs(void)
{
    string logs;
    size_t end;

    if(_logCallback == NULL) {
        return;
    }
    logs = this->_download("/logs.txt?pos="+_logContinuation);
    end =logs.find_last_of("@");
    _logCallback(this,logs.substr(0,end));
    _logContinuation =  logs.substr(end+1);
}




//--- (generated code: Module functions)
//--- (end of generated code: Module functions)

