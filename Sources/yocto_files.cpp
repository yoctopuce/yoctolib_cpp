/*********************************************************************
 *
 * $Id: pic24config.php 9668 2013-02-04 12:36:11Z martinm $
 *
 * Implements yFindFiles(), the high-level API for Files functions
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
 *    THE SOFTWARE AND DOCUMENTATION ARE PROVIDED 'AS IS' WITHOUT
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


#define _CRT_SECURE_NO_DEPRECATE //do not use windows secure crt
#include "yocto_files.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


YFileRecord::YFileRecord(const string& json)
{
    yJsonStateMachine j;
    
    // Parse JSON data 
    j.src = json.c_str();
    j.end = j.src + strlen(j.src);
    j.st = YJSON_START;
    if(yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_STRUCT) {
        return ;
    }
    while(yJsonParse(&j) == YJSON_PARSE_AVAIL && j.st == YJSON_PARSE_MEMBNAME) {
        if (!strcmp(j.token, "name")) {
            if (yJsonParse(&j) != YJSON_PARSE_AVAIL) {
                return ;
            }
            _name = (string)j.token;
            while(j.next == YJSON_PARSE_STRINGCONT && yJsonParse(&j) == YJSON_PARSE_AVAIL) {
                _name =(string)j.token;
            }
        } else if(!strcmp(j.token, "crc")) {
            if (yJsonParse(&j) != YJSON_PARSE_AVAIL) {
                return;
            }
            _crc = atoi(j.token);;
        } else if(!strcmp(j.token, "size")) {
            if (yJsonParse(&j) != YJSON_PARSE_AVAIL) {
                return;
            }
            _size = atoi(j.token);;
        } else {
            yJsonSkip(&j, 1);
        }
    }
}

//--- (generated code: YFileRecord implementation)



string YFileRecord::get_name()
{
    return this->_name;
}

int YFileRecord::get_size()
{
    return this->_size;
}

int YFileRecord::get_crc()
{
    return this->_crc;
}

string YFileRecord::name()
{
    return this->_name;
}

int YFileRecord::size()
{
    return this->_size;
}

int YFileRecord::crc()
{
    return this->_crc;
}
//--- (end of generated code: YFileRecord implementation)


//--- (generated code: YFiles implementation)

const string YFiles::LOGICALNAME_INVALID = "!INVALID!";
const string YFiles::ADVERTISEDVALUE_INVALID = "!INVALID!";

std::map<string,YFiles*> YFiles::_FilesCache;

int YFiles::_parse(yJsonStateMachine& j)
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
        } else if(!strcmp(j.token, "filesCount")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _filesCount =  atoi(j.token);
        } else if(!strcmp(j.token, "freeSpace")) {
            if(yJsonParse(&j) != YJSON_PARSE_AVAIL) return -1;
            _freeSpace =  atoi(j.token);
        } else {
            // ignore unknown field
            yJsonSkip(&j, 1);
        }
    }
    if(j.st != YJSON_PARSE_STRUCT) goto failed;
    return 0;
}

/**
 * Returns the logical name of the filesystem.
 * 
 * @return a string corresponding to the logical name of the filesystem
 * 
 * On failure, throws an exception or returns Y_LOGICALNAME_INVALID.
 */
string YFiles::get_logicalName(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_LOGICALNAME_INVALID;
    }
    return _logicalName;
}

/**
 * Changes the logical name of the filesystem. You can use yCheckLogicalName()
 * prior to this call to make sure that your parameter is valid.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 * 
 * @param newval : a string corresponding to the logical name of the filesystem
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YFiles::set_logicalName(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("logicalName", rest_val);
}

/**
 * Returns the current value of the filesystem (no more than 6 characters).
 * 
 * @return a string corresponding to the current value of the filesystem (no more than 6 characters)
 * 
 * On failure, throws an exception or returns Y_ADVERTISEDVALUE_INVALID.
 */
string YFiles::get_advertisedValue(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_ADVERTISEDVALUE_INVALID;
    }
    return _advertisedValue;
}

/**
 * Returns the number of files currently loaded in the filesystem.
 * 
 * @return an integer corresponding to the number of files currently loaded in the filesystem
 * 
 * On failure, throws an exception or returns Y_FILESCOUNT_INVALID.
 */
unsigned YFiles::get_filesCount(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_FILESCOUNT_INVALID;
    }
    return _filesCount;
}

/**
 * Returns the free space for uploading new files to the filesystem, in bytes.
 * 
 * @return an integer corresponding to the free space for uploading new files to the filesystem, in bytes
 * 
 * On failure, throws an exception or returns Y_FREESPACE_INVALID.
 */
unsigned YFiles::get_freeSpace(void)
{
    if(_cacheExpiration <= YAPI::GetTickCount()) {
        if(YISERR(load(YAPI::DefaultCacheValidity))) return Y_FREESPACE_INVALID;
    }
    return _freeSpace;
}

string YFiles::sendCommand(string command)
{
    string url;
    url =  YapiWrapper::ysprintf("files.json?a=%s",command.c_str());
    return this->_download(url);
    
}

/**
 * Reinitializes the filesystem to its clean, unfragmented, empty state.
 * All files previously uploaded are permanently lost.
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YFiles::format_fs()
{
    string json;
    string res;
    json = this->sendCommand("format"); 
    res  = this->_json_get_key(json, "res");
    if (!(res == "ok")) {this->_throw( YAPI_IO_ERROR, "format failed"); return  YAPI_IO_ERROR;};
    return YAPI_SUCCESS;
    
}

/**
 * Returns a list of YFileRecord objects that describe files currently loaded
 * in the filesystem.
 * 
 * @param pattern : an optional filter pattern, using star and question marks
 *         as wildcards. When an empty pattern is provided, all file records
 *         are returned.
 * 
 * @return a list of YFileRecord objects, containing the file path
 *         and name, byte size and 32-bit CRC of the file content.
 * 
 * On failure, throws an exception or returns an empty list.
 */
vector<YFileRecord*> YFiles::get_list(string pattern)
{
    string json;
    vector<string> list;
    vector<YFileRecord*> res;
    json = this->sendCommand(YapiWrapper::ysprintf("dir&f=%s",pattern.c_str()));
    list = this->_json_get_array(json);
    for (unsigned i_i=0 ; i_i <list.size() ; i_i++) { res.push_back(new YFileRecord(list[i_i]));};
    return res;
    
}

/**
 * Downloads the requested file and returns a binary buffer with its content.
 * 
 * @param pathname : path and name of the new file to load
 * 
 * @return a binary buffer with the file content
 * 
 * On failure, throws an exception or returns an empty content.
 */
string YFiles::download(string pathname)
{
    return this->_download(pathname);
    
}

/**
 * Uploads a file to the filesystem, to the specified full path name.
 * If a file already exists with the same path name, its content is overwritten.
 * 
 * @param pathname : path and name of the new file to create
 * @param content : binary buffer with the content to set
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YFiles::upload(string pathname,string content)
{
    return this->_upload(pathname,content);
    
}

/**
 * Deletes a file, given by its full path name, from the filesystem.
 * Because of filesystem fragmentation, deleting a file may not always
 * free up the whole space used by the file. However, rewriting a file
 * with the same path name will always reuse any space not freed previously.
 * If you need to ensure that no space is taken by previously deleted files,
 * you can use format_fs to fully reinitialize the filesystem.
 * 
 * @param pathname : path and name of the file to remove.
 * 
 * @return YAPI_SUCCESS if the call succeeds.
 * 
 * On failure, throws an exception or returns a negative error code.
 */
int YFiles::remove(string pathname)
{
    string json;
    string res;
    json = this->sendCommand(YapiWrapper::ysprintf("del&f=%s",pathname.c_str())); 
    res  = this->_json_get_key(json, "res");
    if (!(res == "ok")) {this->_throw( YAPI_IO_ERROR, "unable to remove file"); return  YAPI_IO_ERROR;};
    return YAPI_SUCCESS;
    
}

YFiles *YFiles::nextFiles(void)
{
    string  hwid;
    
    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return yFindFiles(hwid);
}

void YFiles::registerValueCallback(YFilesUpdateCallback callback)
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

void YFiles::advertiseValue(const string& value)
{
    if (_callback != NULL) {
        _callback(this, value);
    }
}


YFiles* YFiles::FindFiles(const string& func)
{
    if(YFiles::_FilesCache.find(func) != YFiles::_FilesCache.end())
        return YFiles::_FilesCache[func];
    
    YFiles *newFiles = new YFiles(func);
    YFiles::_FilesCache[func] = newFiles;
    
    return newFiles;
}

YFiles* YFiles::FirstFiles(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;
    
    if(YISERR(YapiWrapper::getFunctionsByClass("Files", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YFiles::FindFiles(serial+"."+funcId);
}

//--- (end of generated code: YFiles implementation)

//--- (generated code: Files functions)
//--- (end of generated code: Files functions)
