/*********************************************************************
 *
 * $Id: yocto_files.cpp 22694 2016-01-12 23:13:27Z seb $
 *
 * Implements yFindFiles(), the high-level API for Files functions
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
// static attributes


string YFileRecord::get_name(void)
{
    return _name;
}

int YFileRecord::get_size(void)
{
    return _size;
}

int YFileRecord::get_crc(void)
{
    return _crc;
}
//--- (end of generated code: YFileRecord implementation)





YFiles::YFiles(const string& func): YFunction(func)
    //--- (generated code: Files initialization)
    ,_filesCount(FILESCOUNT_INVALID)
    ,_freeSpace(FREESPACE_INVALID)
    ,_valueCallbackFiles(NULL)
//--- (end of generated code: Files initialization)
{
    _className = "Files";
}

YFiles::~YFiles()
{
    //--- (generated code: YFiles cleanup)
//--- (end of generated code: YFiles cleanup)
}

//--- (generated code: YFiles implementation)
// static attributes

int YFiles::_parseAttr(yJsonStateMachine& j)
{
    if(!strcmp(j.token, "filesCount")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _filesCount =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "freeSpace")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _freeSpace =  atoi(j.token);
        return 1;
    }
    failed:
    return YFunction::_parseAttr(j);
}


/**
 * Returns the number of files currently loaded in the filesystem.
 *
 * @return an integer corresponding to the number of files currently loaded in the filesystem
 *
 * On failure, throws an exception or returns Y_FILESCOUNT_INVALID.
 */
int YFiles::get_filesCount(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YFiles::FILESCOUNT_INVALID;
        }
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
int YFiles::get_freeSpace(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YFiles::FREESPACE_INVALID;
        }
    }
    return _freeSpace;
}

/**
 * Retrieves a filesystem for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that the filesystem is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method YFiles.isOnline() to test if the filesystem is
 * indeed online at a given time. In case of ambiguity when looking for
 * a filesystem by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * @param func : a string that uniquely characterizes the filesystem
 *
 * @return a YFiles object allowing you to drive the filesystem.
 */
YFiles* YFiles::FindFiles(string func)
{
    YFiles* obj = NULL;
    obj = (YFiles*) YFunction::_FindFromCache("Files", func);
    if (obj == NULL) {
        obj = new YFiles(func);
        YFunction::_AddToCache("Files", func, obj);
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
int YFiles::registerValueCallback(YFilesValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackFiles = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YFiles::_invokeValueCallback(string value)
{
    if (_valueCallbackFiles != NULL) {
        _valueCallbackFiles(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

string YFiles::sendCommand(string command)
{
    string url;
    url = YapiWrapper::ysprintf("files.json?a=%s",command.c_str());
    // may throw an exception
    return this->_download(url);
}

/**
 * Reinitialize the filesystem to its clean, unfragmented, empty state.
 * All files previously uploaded are permanently lost.
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YFiles::format_fs(void)
{
    string json;
    string res;
    json = this->sendCommand("format");
    res = this->_json_get_key(json, "res");
    if (!(res == "ok")) {
        _throw(YAPI_IO_ERROR,"format failed");
        return YAPI_IO_ERROR;
    }
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
vector<YFileRecord> YFiles::get_list(string pattern)
{
    string json;
    vector<string> filelist;
    vector<YFileRecord> res;
    json = this->sendCommand(YapiWrapper::ysprintf("dir&f=%s",pattern.c_str()));
    filelist = this->_json_get_array(json);
    res.clear();
    for (unsigned ii = 0; ii < filelist.size(); ii++) {
        res.push_back(YFileRecord(filelist[ii]));
    }
    return res;
}

/**
 * Test if a file exist on the filesystem of the module.
 *
 * @param filename : the file name to test.
 *
 * @return a true if the file existe, false ortherwise.
 *
 * On failure, throws an exception.
 */
bool YFiles::fileExist(string filename)
{
    string json;
    vector<string> filelist;
    if ((int)(filename).length() == 0) {
        return false;
    }
    json = this->sendCommand(YapiWrapper::ysprintf("dir&f=%s",filename.c_str()));
    filelist = this->_json_get_array(json);
    if ((int)filelist.size() > 0) {
        return true;
    }
    return false;
}

/**
 * Downloads the requested file and returns a binary buffer with its content.
 *
 * @param pathname : path and name of the file to download
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
    return this->_upload(pathname, content);
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
    if (!(res == "ok")) {
        _throw(YAPI_IO_ERROR,"unable to remove file");
        return YAPI_IO_ERROR;
    }
    return YAPI_SUCCESS;
}

YFiles *YFiles::nextFiles(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YFiles::FindFiles(hwid);
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
