/*********************************************************************
 *
 * $Id: yocto_api.cpp 65973 2025-04-22 09:50:13Z seb $
 *
 * High-level programming interface, common to all modules
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
#define __FILE_ID__ "yocto_api"
#define _CRT_SECURE_NO_DEPRECATE
#define YMEMORY_ALLOW_MALLOC

#include "yocto_api.h"

#ifdef WINDOWS_API
#include <Windows.h>
#define yySleep(ms)          Sleep(ms)
#else
#include <unistd.h>
#define yySleep(ms)          usleep(ms*1000)
#endif

#if defined(_MSC_VER) && (_MSC_VER < 1900)
#define yyisnan(x)           _isnan(x)
#else
#define yyisnan(x)           std::isnan(x)
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <math.h>
#include <iostream>

#include "yapi/yproto.h"
#include "yapi/yapi.h"

#ifdef YOCTOLIB_NAMESPACE
using namespace YOCTOLIB_NAMESPACE;
#endif

static yCRITICAL_SECTION _updateDeviceList_CS;
static yCRITICAL_SECTION _handleEvent_CS;

static std::vector<YFunction*> _FunctionCallbacks;
static std::vector<YFunction*> _TimedReportCallbackList;
static std::map<YModule*, int> _moduleCallbackList;

const string YFunction::HARDWAREID_INVALID = YAPI_INVALID_STRING;
const string YFunction::FUNCTIONID_INVALID = YAPI_INVALID_STRING;
const string YFunction::FRIENDLYNAME_INVALID = YAPI_INVALID_STRING;

const double YDataStream::DATA_INVALID = Y_DATA_INVALID;

#ifdef YOCTOLIB_NAMESPACE
namespace YOCTOLIB_NAMESPACE
{
#endif

int _ystrpos(const string &haystack, const string &needle)
{
    size_t pos = haystack.find(needle);
    if (pos == string::npos) {
        return -1;
    }
    return (int)pos;
}

vector<string> _strsplit(const string &str, char delimiter)
{
    vector<string> res;
    size_t pos = 0;
    size_t found;

    do {
        found = str.find(delimiter, pos);
        if (found != std::string::npos) {
            res.push_back(str.substr(pos, found - pos));
            pos = found + 1;
        }
    } while (found != std::string::npos);
    res.push_back(str.substr(pos));
    return res;
}


s64 yatoi(const char *p)
{
    s64 value = 0;
    bool neg = *p == '-';
    if (*p == '+' || neg) {
        p++;
    }
    while (*p >= '0' && *p <= '9') {
        value *= 10;
        value += *p - '0';
        p++;
    }
    if (neg) {
        return -value;
    } else {
        return value;
    }
}

#ifdef YOCTOLIB_NAMESPACE
}
#endif

YJSONContent* YJSONContent::ParseJson(const string &data, int start, int stop)
{
    int cur_pos = YJSONContent::SkipGarbage(data, start, stop);
    YJSONContent *res;
    if (data[cur_pos] == '[') {
        res = new YJSONArray(data, start, stop);
    } else if (data[cur_pos] == '{') {
        res = new YJSONObject(data, start, stop);
    } else if (data[cur_pos] == '"') {
        res = new YJSONString(data, start, stop);
    } else {
        res = new YJSONNumber(data, start, stop);
    }
    res->parse();
    return res;
}

YJSONContent::YJSONContent(const string &data, int start, int stop, YJSONType type)
{
    _data = data;
    _data_start = start;
    _data_boundary = stop;
    _type = type;
}

YJSONContent::YJSONContent(YJSONType type)
{
    _data = ""; //todo: check not null
}

YJSONContent::YJSONContent(YJSONContent *ref)
{
    _data = ref->_data;
    _data_start = ref->_data_start;
    _data_boundary = ref->_data_boundary;
    _data_len = ref->_data_len;
    _type = ref->_type;
}


YJSONContent::~YJSONContent()
{
    _data = "";
}

YJSONType YJSONContent::getJSONType()
{
    return _type;
}

int YJSONContent::SkipGarbage(const string &data, int start, int stop)
{
    if (data.length() <= (unsigned)start) {
        return start;
    }
    char sti = data[start];
    while (start < stop && (sti == '\n' || sti == '\r' || sti == ' ')) {
        start++;
        sti = data[start];
    }
    return start;
}

string YJSONContent::FormatError(const string &errmsg, int cur_pos)
{
    int ststart = cur_pos - 10;
    int stend = cur_pos + 10;
    if (ststart < 0)
        ststart = 0;
    if (stend > _data_boundary)
        stend = _data_boundary;
    if (_data == "") {
        //todo: check not null
        return errmsg;
    }
    return errmsg + " near " + _data.substr(ststart, cur_pos - ststart) + _data.substr(cur_pos, stend - cur_pos);
}


YJSONArray::YJSONArray(const string &data, int start, int stop) : YJSONContent(data, start, stop, YARRAY)
{ }


YJSONArray::YJSONArray() : YJSONContent(YARRAY)
{ }

YJSONArray::YJSONArray(YJSONArray *ref) : YJSONContent(ref)
{
    for (unsigned i = 0; i < ref->_arrayValue.size(); i++) {
        YJSONType type = ref->_arrayValue[i]->getJSONType();
        switch (type) {
        case YARRAY: {
            YJSONArray *tmp = new YJSONArray((YJSONArray*)ref->_arrayValue[i]);
            _arrayValue.push_back(tmp);
        }
        break;
        case YNUMBER: {
            YJSONNumber *tmp = new YJSONNumber((YJSONNumber*)ref->_arrayValue[i]);
            _arrayValue.push_back(tmp);
        }
        break;
        case YSTRING: {
            YJSONString *tmp = new YJSONString((YJSONString*)ref->_arrayValue[i]);
            _arrayValue.push_back(tmp);
        }
        break;
        case YOBJECT: {
            YJSONObject *tmp = new YJSONObject((YJSONObject*)ref->_arrayValue[i]);
            _arrayValue.push_back(tmp);
        }
        break;
        }
    }
}


YJSONArray::~YJSONArray()
{
    for (unsigned i = 0; i < _arrayValue.size(); i++) {
        delete _arrayValue[i];
    }
    _arrayValue.clear();
}

int YJSONArray::length()
{
    return (int)_arrayValue.size();
}

int YJSONArray::parse()
{
    int cur_pos = SkipGarbage(_data, _data_start, _data_boundary);

    if (cur_pos >= _data_boundary || _data[cur_pos] != '[') {
        throw YAPI_Exception(YAPI_IO_ERROR, FormatError("Opening braces was expected", cur_pos));
    }
    cur_pos++;
    Tjstate state = JWAITFORDATA;

    while (cur_pos < _data_boundary) {
        char sti = _data[cur_pos];
        switch (state) {
        case JWAITFORDATA:
            if (sti == '{') {
                YJSONObject *jobj = new YJSONObject(_data, cur_pos, _data_boundary);
                int len = jobj->parse();
                cur_pos += len;
                _arrayValue.push_back(jobj);
                state = JWAITFORNEXTARRAYITEM;
                //cur_pos is already incremented
                continue;
            } else if (sti == '[') {
                YJSONArray *jobj = new YJSONArray(_data, cur_pos, _data_boundary);
                int len = jobj->parse();
                cur_pos += len;
                _arrayValue.push_back(jobj);
                state = JWAITFORNEXTARRAYITEM;
                //cur_pos is already incremented
                continue;
            } else if (sti == '"') {
                YJSONString *jobj = new YJSONString(_data, cur_pos, _data_boundary);
                int len = jobj->parse();
                cur_pos += len;
                _arrayValue.push_back(jobj);
                state = JWAITFORNEXTARRAYITEM;
                //cur_pos is already incremented
                continue;
            } else if (sti == '-' || (sti >= '0' && sti <= '9')) {
                YJSONNumber *jobj = new YJSONNumber(_data, cur_pos, _data_boundary);
                int len = jobj->parse();
                cur_pos += len;
                _arrayValue.push_back(jobj);
                state = JWAITFORNEXTARRAYITEM;
                //cur_pos is already incremented
                continue;
            } else if (sti == ']') {
                _data_len = cur_pos + 1 - _data_start;
                return _data_len;
            } else if (sti != ' ' && sti != '\n' && sti != '\r') {
                throw YAPI_Exception(YAPI_IO_ERROR, FormatError("invalid char: was expecting  \",0..9,t or f", cur_pos));
            }
            break;
        case JWAITFORNEXTARRAYITEM:
            if (sti == ',') {
                state = JWAITFORDATA;
            } else if (sti == ']') {
                _data_len = cur_pos + 1 - _data_start;
                return _data_len;
            } else {
                if (sti != ' ' && sti != '\n' && sti != '\r') {
                    throw YAPI_Exception(YAPI_IO_ERROR, FormatError("invalid char: was expecting ,", cur_pos));
                }
            }
            break;
        default:
            throw YAPI_Exception(YAPI_IO_ERROR, FormatError("invalid state for YJSONObject", cur_pos));
        }
        cur_pos++;
    }
    throw YAPI_Exception(YAPI_IO_ERROR, FormatError("unexpected end of data", cur_pos));
}

YJSONObject* YJSONArray::getYJSONObject(int i)
{
    return (YJSONObject*)_arrayValue[i];
}

string YJSONArray::getString(int i)
{
    YJSONString *ystr = (YJSONString*)_arrayValue[i];
    return ystr->getString();
}

YJSONContent* YJSONArray::get(int i)
{
    return _arrayValue[i];
}

YJSONArray* YJSONArray::getYJSONArray(int i)
{
    return (YJSONArray*)_arrayValue[i];
}

int YJSONArray::getInt(int i)
{
    YJSONNumber *ystr = (YJSONNumber*)_arrayValue[i];
    return ystr->getInt();
}

s64 YJSONArray::getLong(int i)
{
    YJSONNumber *ystr = (YJSONNumber*)_arrayValue[i];
    return ystr->getLong();
}

void YJSONArray::put(const string &flatAttr)
{
    YJSONString *strobj = new YJSONString();
    strobj->setContent(flatAttr);
    _arrayValue.push_back(strobj);
}

string YJSONArray::toJSON()
{
    string res = "[";
    string sep = "";
    unsigned int i;
    for (i = 0; i < _arrayValue.size(); i++) {
        YJSONContent *yjsonContent = _arrayValue[i];
        string subres = yjsonContent->toJSON();
        res += sep;
        res += subres;
        sep = ",";
    }
    res += ']';
    return res;
}

string YJSONArray::toString()
{
    string res = "[";
    string sep = "";
    unsigned int i;
    for (i = 0; i < _arrayValue.size(); i++) {
        YJSONContent *yjsonContent = _arrayValue[i];
        string subres = yjsonContent->toString();
        res += sep;
        res += subres;
        sep = ",";
    }
    res += ']';
    return res;
}


YJSONString::YJSONString(const string &data, int start, int stop) : YJSONContent(data, start, stop, YSTRING)
{ }

YJSONString::YJSONString() : YJSONContent(YSTRING)
{ }

YJSONString::YJSONString(YJSONString *ref) : YJSONContent(ref)
{
    _stringValue = ref->_stringValue;
}

int YJSONString::parse()
{
    string value = "";
    int cur_pos = SkipGarbage(_data, _data_start, _data_boundary);

    if (cur_pos >= _data_boundary || _data[cur_pos] != '"') {
        throw YAPI_Exception(YAPI_IO_ERROR, FormatError("double quote was expected", cur_pos));
    }
    cur_pos++;
    int str_start = cur_pos;
    Tjstate state = JWAITFORSTRINGVALUE;

    while (cur_pos < _data_boundary) {
        unsigned char sti = _data[cur_pos];
        switch (state) {
        case JWAITFORSTRINGVALUE:
            if (sti == '\\') {
                value += _data.substr(str_start, cur_pos - str_start);
                str_start = cur_pos;
                state = JWAITFORSTRINGVALUE_ESC;
            } else if (sti == '"') {
                value += _data.substr(str_start, cur_pos - str_start);
                _stringValue = value;
                _data_len = (cur_pos + 1) - _data_start;
                return _data_len;
            } else if (sti < 32) {
                throw YAPI_Exception(YAPI_IO_ERROR, FormatError("invalid char: was expecting string value", cur_pos));
            }
            break;
        case JWAITFORSTRINGVALUE_ESC:
            value += sti;
            state = JWAITFORSTRINGVALUE;
            str_start = cur_pos + 1;
            break;
        default:
            throw YAPI_Exception(YAPI_IO_ERROR, FormatError("invalid state for YJSONObject", cur_pos));
        }
        cur_pos++;
    }
    throw YAPI_Exception(YAPI_IO_ERROR, FormatError("unexpected end of data", cur_pos));
}

string YJSONString::toJSON()
{
    string res = "\"";
    const char *c = _stringValue.c_str();
    while (*c) {
        switch (*c) {
        case '"':
            res += "\\\"";
            break;
        case '\\':
            res += "\\\\";
            break;
        case '/':
            res += "\\/";
            break;
        case '\b':
            res += "\\b";
            break;
        case '\f':
            res += "\\f";
            break;
        case '\n':
            res += "\\n";
            break;
        case '\r':
            res += "\\r";
            break;
        case '\t':
            res += "\\t";
            break;
        default:
            res += *c;
            break;
        }
        c++;
    }
    res += '"';
    return res;
}

string YJSONString::getString()
{
    return _stringValue;
}

string YJSONString::toString()
{
    return _stringValue;
}

void YJSONString::setContent(const string &value)
{
    _stringValue = value;
}


YJSONNumber::YJSONNumber(const string &data, int start, int stop) : YJSONContent(data, start, stop, YNUMBER), _intValue(0), _doubleValue(0), _isFloat(false)
{ }

YJSONNumber::YJSONNumber(YJSONNumber *ref) : YJSONContent(ref)
{
    _intValue = ref->_intValue;
    _doubleValue = ref->_doubleValue;
    _isFloat = ref->_isFloat;
}


int YJSONNumber::decode_number(bool neg, int start, int cur_pos)
{
    string numberpart = _data.substr(start, cur_pos - start);
    if (_isFloat) {
        _doubleValue = atof((numberpart).c_str());
    } else {
        _intValue = yatoi((numberpart).c_str());
    }
    if (neg) {
        _doubleValue = 0 - _doubleValue;
        _intValue = 0 - _intValue;
    }
    return cur_pos - _data_start;
}

int YJSONNumber::parse()
{
    bool neg = false;
    int start;
    char sti;
    int cur_pos = SkipGarbage(_data, _data_start, _data_boundary);
    sti = _data[cur_pos];
    if (sti == '-') {
        neg = true;
        cur_pos++;
    }
    start = cur_pos;
    while (cur_pos < _data_boundary) {
        sti = _data[cur_pos];
        if (sti == '.' && _isFloat == false) {
            string int_part = _data.substr(start, cur_pos - start);
            _intValue = yatoi((int_part).c_str());
            _isFloat = true;
        } else if (sti < '0' || sti > '9') {
            return decode_number(neg, start, cur_pos);
        }
        cur_pos++;
    }
     return decode_number(neg, start, cur_pos);
}

string YJSONNumber::toJSON()
{
    if (_isFloat)
        return YapiWrapper::ysprintf("%f", _doubleValue);
    else
        return YapiWrapper::ysprintf("%d", _intValue);
}

s64 YJSONNumber::getLong()
{
    if (_isFloat)
        return (s64)_doubleValue;
    else
        return _intValue;
}

int YJSONNumber::getInt()
{
    if (_isFloat)
        return (int)_doubleValue;
    else
        return (int)_intValue;
}

double YJSONNumber::getDouble()
{
    if (_isFloat)
        return _doubleValue;
    else
        return (double)_intValue;
}

string YJSONNumber::toString()
{
    if (_isFloat)
        return YapiWrapper::ysprintf("%f", _doubleValue);
    else
        return YapiWrapper::ysprintf("%d", _intValue);
}


YJSONObject::YJSONObject(const string &data) : YJSONContent(data, 0, (int)data.length(), YOBJECT)
{ }

YJSONObject::YJSONObject(const string &data, int start, int len) : YJSONContent(data, start, len, YOBJECT)
{ }

YJSONObject::YJSONObject(YJSONObject *ref) : YJSONContent(ref)
{
    for (unsigned i = 0; i < ref->_keys.size(); i++) {
        string key = ref->_keys[i];
        _keys.push_back(key);
        YJSONType type = ref->_parsed[key]->getJSONType();
        switch (type) {
        case YARRAY:
            _parsed[key] = new YJSONArray((YJSONArray*)ref->_parsed[key]);
            break;
        case YNUMBER:
            _parsed[key] = new YJSONNumber((YJSONNumber*)ref->_parsed[key]);
            break;
        case YSTRING:
            _parsed[key] = new YJSONString((YJSONString*)ref->_parsed[key]);
            break;
        case YOBJECT:
            _parsed[key] = new YJSONObject((YJSONObject*)ref->_parsed[key]);
            break;
        }
    }
}

YJSONObject::~YJSONObject()
{
    //printf("relase YJSONObject\n");
    for (unsigned i = 0; i < _keys.size(); i++) {
        delete _parsed[_keys[i]];
    }
    _parsed.clear();
    _keys.clear();
}

int YJSONObject::parse()
{
    string current_name = "";
    int name_start = _data_start;
    int cur_pos = SkipGarbage(_data, _data_start, _data_boundary);

    if (_data.length() <= (unsigned)cur_pos || cur_pos >= _data_boundary || _data[cur_pos] != '{') {
        throw YAPI_Exception(YAPI_IO_ERROR, FormatError("Opening braces was expected", cur_pos));
    }
    cur_pos++;
    Tjstate state = JWAITFORNAME;

    while (cur_pos < _data_boundary) {
        char sti = _data[cur_pos];
        switch (state) {
        case JWAITFORNAME:
            if (sti == '"') {
                state = JWAITFORENDOFNAME;
                name_start = cur_pos + 1;
            } else if (sti == '}') {
                _data_len = cur_pos + 1 - _data_start;
                return _data_len;
            } else {
                if (sti != ' ' && sti != '\n' && sti != '\r') {
                    throw YAPI_Exception(YAPI_IO_ERROR, FormatError("invalid char: was expecting \"", cur_pos));
                }
            }
            break;
        case JWAITFORENDOFNAME:
            if (sti == '"') {
                current_name = _data.substr(name_start, cur_pos - name_start);
                state = JWAITFORCOLON;

            } else {
                if (sti < 32) {
                    throw YAPI_Exception(YAPI_IO_ERROR, FormatError("invalid char: was expecting an identifier compliant char", cur_pos));
                }
            }
            break;
        case JWAITFORCOLON:
            if (sti == ':') {
                state = JWAITFORDATA;
            } else {
                if (sti != ' ' && sti != '\n' && sti != '\r') {
                    throw YAPI_Exception(YAPI_IO_ERROR, FormatError("invalid char: was expecting \"", cur_pos));
                }
            }
            break;
        case JWAITFORDATA:
            if (sti == '{') {
                YJSONObject *jobj = new YJSONObject(_data, cur_pos, _data_boundary);
                int len = jobj->parse();
                cur_pos += len;
                _parsed[current_name] = jobj;
                _keys.push_back(current_name);
                state = JWAITFORNEXTSTRUCTMEMBER;
                //cur_pos is already incremented
                continue;
            } else if (sti == '[') {
                YJSONArray *jobj = new YJSONArray(_data, cur_pos, _data_boundary);
                int len = jobj->parse();
                cur_pos += len;
                _parsed[current_name] = jobj;
                _keys.push_back(current_name);
                state = JWAITFORNEXTSTRUCTMEMBER;
                //cur_pos is already incremented
                continue;
            } else if (sti == '"') {
                YJSONString *jobj = new YJSONString(_data, cur_pos, _data_boundary);
                int len = jobj->parse();
                cur_pos += len;
                _parsed[current_name] = jobj;
                _keys.push_back(current_name);
                state = JWAITFORNEXTSTRUCTMEMBER;
                //cur_pos is already incremented
                continue;
            } else if (sti == '-' || (sti >= '0' && sti <= '9')) {
                YJSONNumber *jobj = new YJSONNumber(_data, cur_pos, _data_boundary);
                int len = jobj->parse();
                cur_pos += len;
                _parsed[current_name] = jobj;
                _keys.push_back(current_name);
                state = JWAITFORNEXTSTRUCTMEMBER;
                //cur_pos is already incremented
                continue;
            } else if (sti != ' ' && sti != '\n' && sti != '\r') {
                throw YAPI_Exception(YAPI_IO_ERROR, FormatError("invalid char: was expecting  \",0..9,t or f", cur_pos));
            }
            break;
        case JWAITFORNEXTSTRUCTMEMBER:
            if (sti == ',') {
                state = JWAITFORNAME;
                name_start = cur_pos + 1;
            } else if (sti == '}') {
                _data_len = cur_pos + 1 - _data_start;
                return _data_len;
            } else {
                if (sti != ' ' && sti != '\n' && sti != '\r') {
                    throw YAPI_Exception(YAPI_IO_ERROR, FormatError("invalid char: was expecting ,", cur_pos));
                }
            }
            break;
        default:
            throw YAPI_Exception(YAPI_IO_ERROR, FormatError("invalid state for YJSONObject", cur_pos));
        }
        cur_pos++;
    }
    throw YAPI_Exception(YAPI_IO_ERROR, FormatError("unexpected end of data", cur_pos));
}

bool YJSONObject::has(const string &key)
{
    if (_parsed.find(key) == _parsed.end()) {
        return false;
    } else {
        return true;
    }
}

YJSONObject* YJSONObject::getYJSONObject(const string &key)
{
    return (YJSONObject*)_parsed[key];
}

YJSONString* YJSONObject::getYJSONString(const string &key)
{
    return (YJSONString*)_parsed[key];
}

YJSONArray* YJSONObject::getYJSONArray(const string &key)
{
    return (YJSONArray*)_parsed[key];
}

vector<string> YJSONObject::keys()
{
    vector<string> v;
    for (map<string, YJSONContent*>::iterator it = _parsed.begin(); it != _parsed.end(); ++it) {
        v.push_back(it->first);
    }
    return v;
}

YJSONNumber* YJSONObject::getYJSONNumber(const string &key)
{
    return (YJSONNumber*)_parsed[key];
}

string YJSONObject::getString(const string &key)
{
    YJSONString *ystr = (YJSONString*)_parsed[key];
    return ystr->getString();
}

int YJSONObject::getInt(const string &key)
{
    YJSONNumber *yint = (YJSONNumber*)_parsed[key];
    return yint->getInt();
}

YJSONContent* YJSONObject::get(const string &key)
{
    return _parsed[key];
}

s64 YJSONObject::getLong(const string &key)
{
    YJSONNumber *yint = (YJSONNumber*)_parsed[key];
    return yint->getLong();
}

double YJSONObject::getDouble(const string &key)
{
    YJSONNumber *yint = (YJSONNumber*)_parsed[key];
    return yint->getDouble();
}

string YJSONObject::toJSON()
{
    string res = "{";
    string sep = "";
    unsigned int i;
    for (i = 0; i < _keys.size(); i++) {
        string key = _keys[i];
        YJSONContent *subContent = _parsed[key];
        string subres = subContent->toJSON();
        res += sep;
        res += '"';
        res += key;
        res += "\":";
        res += subres;
        sep = ",";
    }
    res += '}';
    return res;
}

string YJSONObject::toString()
{
    string res = "{";
    string sep = "";
    unsigned int i;
    for (i = 0; i < _keys.size(); i++) {
        string key = _keys[i];
        YJSONContent *subContent = _parsed[key];
        string subres = subContent->toString();
        res += sep;
        res += '"';
        res += key;
        res += "\":";
        res += subres;
        sep = ",";
    }
    res += '}';
    return res;
}


void YJSONObject::parseWithRef(YJSONObject *reference)
{
    if (reference != NULL) {
        try {
            YJSONArray *yzon = new YJSONArray(_data, _data_start, _data_boundary);
            yzon->parse();
            convert(reference, yzon);
            delete yzon;
            return;
        } catch (std::exception &) { }
    }
    this->parse();
}

void YJSONObject::convert(YJSONObject *reference, YJSONArray *newArray)
{
    int length = newArray->length();
    for (int i = 0; i < length; i++) {
        string key = reference->getKeyFromIdx(i);
        YJSONContent *item = newArray->get(i);
        YJSONContent *reference_item = reference->get(key);
        YJSONType type = item->getJSONType();
        if (type == reference_item->getJSONType()) {
            switch (type) {
            case YARRAY:
                _parsed[key] = new YJSONArray((YJSONArray*)item);
                break;
            case YNUMBER:
                _parsed[key] = new YJSONNumber((YJSONNumber*)item);
                break;
            case YSTRING:
                _parsed[key] = new YJSONString((YJSONString*)item);
                break;
            case YOBJECT:
                _parsed[key] = new YJSONObject((YJSONObject*)item);
                break;
            }
            _keys.push_back(key);
        } else if (type == YARRAY && reference_item->getJSONType() == YOBJECT) {
            YJSONObject *jobj = new YJSONObject(item->_data, item->_data_start, reference_item->_data_boundary);
            jobj->convert((YJSONObject*)reference_item, (YJSONArray*)item);
            _parsed[key] = jobj;
            _keys.push_back(key);
        } else {
            throw YAPI_Exception(YAPI_IO_ERROR, "Unable to convert JZON struct");

        }
    }
}

string YJSONObject::getKeyFromIdx(int i)
{
    return _keys[i];
}


YDataStream::YDataStream(YFunction *parent):
    //--- (generated code: YDataStream initialization)
    _parent(NULL)
    ,_runNo(0)
    ,_utcStamp(0)
    ,_nCols(0)
    ,_nRows(0)
    ,_startTime(0.0)
    ,_duration(0.0)
    ,_dataSamplesInterval(0.0)
    ,_firstMeasureDuration(0.0)
    ,_isClosed(0)
    ,_isAvg(0)
    ,_minVal(0.0)
    ,_avgVal(0.0)
    ,_maxVal(0.0)
    ,_caltyp(0)
    ,_isLoaded(0)
//--- (end of generated code: YDataStream initialization)
{
    _parent = parent;
}


// YDataStream constructor for the new datalogger
YDataStream::YDataStream(YFunction *parent, YDataSet &dataset, const vector<int> &encoded):
    //--- (generated code: YDataStream initialization)
    _parent(NULL)
    ,_runNo(0)
    ,_utcStamp(0)
    ,_nCols(0)
    ,_nRows(0)
    ,_startTime(0.0)
    ,_duration(0.0)
    ,_dataSamplesInterval(0.0)
    ,_firstMeasureDuration(0.0)
    ,_isClosed(0)
    ,_isAvg(0)
    ,_minVal(0.0)
    ,_avgVal(0.0)
    ,_maxVal(0.0)
    ,_caltyp(0)
    ,_isLoaded(0)
//--- (end of generated code: YDataStream initialization)
{
    _parent = parent;
    this->_initFromDataSet(&dataset, encoded);
}


YDataStream::~YDataStream()
{
    _columnNames.clear();
    _calpar.clear();
    _calraw.clear();
    _calref.clear();
    _values.clear();
}

// YDataSet constructor, when instantiated directly by a function
YDataSet::YDataSet(YFunction *parent, const string &functionId, const string &unit, double startTime, double endTime):
    //--- (generated code: YDataSet initialization)
    _parent(NULL)
    ,_bulkLoad(0)
    ,_startTimeMs(0.0)
    ,_endTimeMs(0.0)
    ,_progress(0)
    ,_summaryMinVal(0.0)
    ,_summaryMaxVal(0.0)
    ,_summaryTotalAvg(0.0)
    ,_summaryTotalTime(0.0)
//--- (end of generated code: YDataSet initialization)
{
    _parent = parent;
    _functionId = functionId;
    _unit = unit;
    _startTimeMs = startTime * 1000;
    _endTimeMs = endTime * 1000;
    _summary = YMeasure(0, 0, 0, 0, 0);
    _progress = -1;
}

// YDataSet constructor for the new datalogger
YDataSet::YDataSet(YFunction *parent):
    //--- (generated code: YDataSet initialization)
    _parent(NULL)
    ,_bulkLoad(0)
    ,_startTimeMs(0.0)
    ,_endTimeMs(0.0)
    ,_progress(0)
    ,_summaryMinVal(0.0)
    ,_summaryMaxVal(0.0)
    ,_summaryTotalAvg(0.0)
    ,_summaryTotalTime(0.0)
//--- (end of generated code: YDataSet initialization)
{
    _parent = parent;
    _startTimeMs = 0;
    _endTimeMs = 0;
    _summary = YMeasure(0, 0, 0, 0, 0);
}

// YDataSet parser for stream list
int YDataSet::_parse(const string &json)
{
    yJsonStateMachine j;

    // Parse JSON data
    j.src = json.c_str();
    j.end = j.src + strlen(j.src);
    j.st = YJSON_START;
    if (yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_STRUCT) {
        return YAPI_NOT_SUPPORTED;
    }
    while (yJsonParse(&j) == YJSON_PARSE_AVAIL && j.st == YJSON_PARSE_MEMBNAME) {
        if (!strcmp(j.token, "id")) {
            if (yJsonParse(&j) != YJSON_PARSE_AVAIL) {
                return YAPI_NOT_SUPPORTED;
            }
            _functionId = _parent->_parseString(j);
        } else if (!strcmp(j.token, "unit")) {
            if (yJsonParse(&j) != YJSON_PARSE_AVAIL) {
                return YAPI_NOT_SUPPORTED;
            }
            _unit = _parent->_parseString(j);
        } else if (!strcmp(j.token, "bulk")) {
            if (yJsonParse(&j) != YJSON_PARSE_AVAIL) {
                return YAPI_NOT_SUPPORTED;
            }
            _bulkLoad = atoi(_parent->_parseString(j).c_str());
        } else if (!strcmp(j.token, "calib")) {
            if (yJsonParse(&j) != YJSON_PARSE_AVAIL) {
                return YAPI_NOT_SUPPORTED;
            }
            _calib = YAPI::_decodeFloats(_parent->_parseString(j));
            _calib[0] = _calib[0] / 1000;
        } else if (!strcmp(j.token, "cal")) {
            if (yJsonParse(&j) != YJSON_PARSE_AVAIL) {
                return YAPI_NOT_SUPPORTED;
            }
            if (_calib.size() == 0) {
                _calib = YAPI::_decodeWords(_parent->_parseString(j));
            }
        } else if (!strcmp(j.token, "streams")) {
            YDataStream *stream;
            double streamEndTime;
            double streamStartTime;
            _streams = vector<YDataStream*>();
            _preview = vector<YMeasure>();
            _measures = vector<YMeasure>();
            if (yJsonParse(&j) != YJSON_PARSE_AVAIL || j.token[0] != '[') {
                return YAPI_NOT_SUPPORTED;
            }
            // select streams for specified timeframe
            while (yJsonParse(&j) == YJSON_PARSE_AVAIL && j.token[0] != ']') {
                stream = _parent->_findDataStream(*this, _parent->_parseString(j));
                // the timestamp in the data streams is the end of the measure, so the actual
                // measurement start time is computed as one interval before the first timestamp
                streamStartTime = floor(stream->get_realStartTimeUTC() * 1000 + 0.5);
                streamEndTime = streamStartTime + floor(stream->get_realDuration() * 1000 + 0.5);
                if (_startTimeMs > 0 && streamEndTime <= _startTimeMs) {
                    // this stream is too early, drop it
                } else if (_endTimeMs > 0 && streamStartTime >= _endTimeMs) {
                    // this stream is too late, drop it
                } else {
                    _streams.push_back(stream);
                }
            }
        } else {
            yJsonSkip(&j, 1);
        }
    }
    _progress = 0;
    return this->get_progress();
}

// YConsolidatedDataSet constructor
YConsolidatedDataSet::YConsolidatedDataSet(double startTime, double endTime, vector<YSensor*> sensorList) :
    //--- (generated code: YConsolidatedDataSet initialization)
    _start(0.0)
    ,_end(0.0)
    ,_nsensors(0)
//--- (end of generated code: YConsolidatedDataSet initialization)
{
    this->imm_init(startTime, endTime, sensorList);
}

//--- (generated code: YConsolidatedDataSet implementation)
// static attributes


int YConsolidatedDataSet::imm_init(double startt,double endt,vector<YSensor*> sensorList)
{
    _start = startt;
    _end = endt;
    _sensors = sensorList;
    _nsensors = -1;
    return YAPI_SUCCESS;
}

/**
 * Returns an object holding historical data for multiple
 * sensors, for a specified time interval.
 * The measures will be retrieved from the data logger, which must have been turned
 * on at the desired time. The resulting object makes it possible to load progressively
 * a large set of measures from multiple sensors, consolidating data on the fly
 * to align records based on measurement timestamps.
 *
 * @param sensorNames : array of logical names or hardware identifiers of the sensors
 *         for which data must be loaded from their data logger.
 * @param startTime : the start of the desired measure time interval,
 *         as a Unix timestamp, i.e. the number of seconds since
 *         January 1, 1970 UTC. The special value 0 can be used
 *         to include any measure, without initial limit.
 * @param endTime : the end of the desired measure time interval,
 *         as a Unix timestamp, i.e. the number of seconds since
 *         January 1, 1970 UTC. The special value 0 can be used
 *         to include any measure, without ending limit.
 *
 * @return an instance of YConsolidatedDataSet, providing access to
 *         consolidated historical data. Records can be loaded progressively
 *         using the YConsolidatedDataSet::nextRecord() method.
 */
YConsolidatedDataSet YConsolidatedDataSet::Init(vector<string> sensorNames,double startTime,double endTime)
{
    int nSensors = 0;
    vector<YSensor*> sensorList;
    int idx = 0;
    string sensorName;
    YSensor* s = NULL;
    YConsolidatedDataSet obj;
    nSensors = (int)sensorNames.size();
    sensorList.clear();
    idx = 0;
    while (idx < nSensors) {
        sensorName = sensorNames[idx];
        s = YSensor::FindSensor(sensorName);
        sensorList.push_back(s);
        idx = idx + 1;
    }
    obj = YConsolidatedDataSet(startTime,endTime,sensorList);
    return obj;
}

/**
 * Extracts the next data record from the data logger of all sensors linked to this
 * object.
 *
 * @param datarec : array of floating point numbers, that will be filled by the
 *         function with the timestamp of the measure in first position,
 *         followed by the measured value in next positions.
 *
 * @return an integer in the range 0 to 100 (percentage of completion),
 *         or a negative error code in case of failure.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YConsolidatedDataSet::nextRecord(vector<double>& datarec)
{
    int s = 0;
    int idx = 0;
    YSensor* sensor = NULL;
    YDataSet newdataset;
    int globprogress = 0;
    int currprogress = 0;
    double currnexttim = 0.0;
    double newvalue = 0.0;
    vector<YMeasure> measures;
    double nexttime = 0.0;
    //
    // Ensure the dataset have been retrieved
    //
    if (_nsensors == -1) {
        _nsensors = (int)_sensors.size();
        _datasets.clear();
        _progresss.clear();
        _nextidx.clear();
        _nexttim.clear();
        s = 0;
        while (s < _nsensors) {
            sensor = _sensors[s];
            newdataset = sensor->get_recordedData(_start, _end);
            _datasets.push_back(newdataset);
            _progresss.push_back(0);
            _nextidx.push_back(0);
            _nexttim.push_back(0.0);
            s = s + 1;
        }
    }
    datarec.clear();
    //
    // Find next timestamp to process
    //
    nexttime = 0;
    s = 0;
    while (s < _nsensors) {
        currnexttim = _nexttim[s];
        if (currnexttim == 0) {
            idx = _nextidx[s];
            measures = _datasets[s].get_measures();
            currprogress = _progresss[s];
            while ((idx >= (int)measures.size()) && (currprogress < 100)) {
                currprogress = _datasets[s].loadMore();
                if (currprogress < 0) {
                    currprogress = 100;
                }
                _progresss[s] = currprogress;
                measures = _datasets[s].get_measures();
            }
            if (idx < (int)measures.size()) {
                currnexttim = measures[idx].get_endTimeUTC();
                _nexttim[s] = currnexttim;
            }
        }
        if (currnexttim > 0) {
            if ((nexttime == 0) || (nexttime > currnexttim)) {
                nexttime = currnexttim;
            }
        }
        s = s + 1;
    }
    if (nexttime == 0) {
        return 100;
    }
    //
    // Extract data for this timestamp
    //
    datarec.clear();
    datarec.push_back(nexttime);
    globprogress = 0;
    s = 0;
    while (s < _nsensors) {
        if (_nexttim[s] == nexttime) {
            idx = _nextidx[s];
            measures = _datasets[s].get_measures();
            newvalue = measures[idx].get_averageValue();
            datarec.push_back(newvalue);
            _nexttim[s] = 0.0;
            _nextidx[s] = idx + 1;
        } else {
            datarec.push_back(NAN);
        }
        currprogress = _progresss[s];
        globprogress = globprogress + currprogress;
        s = s + 1;
    }
    if (globprogress > 0) {
        globprogress = (globprogress / _nsensors);
        if (globprogress > 99) {
            globprogress = 99;
        }
    }
    return globprogress;
}
//--- (end of generated code: YConsolidatedDataSet implementation)

YFirmwareUpdate::YFirmwareUpdate(string serialNumber, string path, string settings) :
    //--- (generated code: YFirmwareUpdate initialization)
    _progress_c(0)
    ,_progress(0)
    ,_restore_step(0)
    ,_force(0)
//--- (end of generated code: YFirmwareUpdate initialization)
{
    _serial = serialNumber;
    _settings = settings;
    _firmwarepath = path;
}

YFirmwareUpdate::YFirmwareUpdate(string serialNumber, string path, string settings, bool force) :
    //--- (generated code: YFirmwareUpdate initialization)
    _progress_c(0)
    ,_progress(0)
    ,_restore_step(0)
    ,_force(0)
//--- (end of generated code: YFirmwareUpdate initialization)
{
    _serial = serialNumber;
    _settings = settings;
    _firmwarepath = path;
    _force = force;
}

YFirmwareUpdate::YFirmwareUpdate() :
    //--- (generated code: YFirmwareUpdate initialization)
    _progress_c(0)
    ,_progress(0)
    ,_restore_step(0)
    ,_force(0)
//--- (end of generated code: YFirmwareUpdate initialization)
{}


//--- (generated code: YFirmwareUpdate implementation)
// static attributes


int YFirmwareUpdate::_processMore(int newupdate)
{
    char errmsg[YOCTO_ERRMSG_LEN];
    YModule* m = NULL;
    int res = 0;
    string serial;
    string firmwarepath;
    string settings;
    string prod_prefix;
    int force = 0;
    if ((_progress_c < 100) && (_progress_c != YAPI_VERSION_MISMATCH)) {
        serial = _serial;
        firmwarepath = _firmwarepath;
        settings = _settings;
        if (_force) {
            force = 1;
        } else {
            force = 0;
        }
        res = yapiUpdateFirmwareEx(serial.c_str(), firmwarepath.c_str(), settings.c_str(), force, newupdate, errmsg);
        if ((res == YAPI_VERSION_MISMATCH) && ((int)(_settings).size() != 0)) {
            _progress_c = res;
            _progress_msg = string(errmsg);
            return _progress;
        }
        if (res < 0) {
            _progress = res;
            _progress_msg = string(errmsg);
            return res;
        }
        _progress_c = res;
        _progress = ((_progress_c * 9) / 10);
        _progress_msg = string(errmsg);
    } else {
        if (((int)(_settings).size() != 0) && ( _progress_c != 101)) {
            _progress_msg = "restoring settings";
            m = YModule::FindModule(_serial + ".module");
            if (!(m->isOnline())) {
                return _progress;
            }
            if (_progress < 95) {
                prod_prefix = (m->get_productName()).substr(0, 8);
                if (prod_prefix == "YoctoHub") {
                    {string ignore_error; YAPI::Sleep(1000, ignore_error);};
                    _progress = _progress + 1;
                    return _progress;
                } else {
                    _progress = 95;
                }
            }
            if (_progress < 100) {
                m->set_allSettingsAndFiles(_settings);
                m->saveToFlash();
                _settings = string(0, (char)0);
                if (_progress_c == YAPI_VERSION_MISMATCH) {
                    _progress = YAPI_IO_ERROR;
                    _progress_msg = "Unable to update firmware";
                } else {
                    _progress = 100;
                    _progress_msg = "success";
                }
            }
        } else {
            _progress = 100;
            _progress_msg = "success";
        }
    }
    return _progress;
}

/**
 * Returns a list of all the modules in "firmware update" mode.
 *
 * @return an array of strings containing the serial numbers of devices in "firmware update" mode.
 */
vector<string> YFirmwareUpdate::GetAllBootLoaders(void)
{
    char errmsg[YOCTO_ERRMSG_LEN];
    char smallbuff[1024];
    char *bigbuff;
    int buffsize = 0;
    int fullsize = 0;
    int yapi_res = 0;
    string bootloader_list;
    vector<string> bootladers;
    fullsize = 0;
    yapi_res = yapiGetBootloaders(smallbuff, 1024, &fullsize, errmsg);
    if (yapi_res < 0) {
        return bootladers;
    }
    if (fullsize <= 1024) {
        bootloader_list = string(smallbuff, yapi_res);
    } else {
        buffsize = fullsize;
        bigbuff = (char *)malloc(buffsize);
        yapi_res = yapiGetBootloaders(bigbuff, buffsize, &fullsize, errmsg);
        if (yapi_res < 0) {
            free(bigbuff);
            return bootladers;
        } else {
            bootloader_list = string(bigbuff, yapi_res);
        }
        free(bigbuff);
    }
    if (!(bootloader_list == "")) {
        bootladers = _strsplit(bootloader_list,',');
    }
    return bootladers;
}

/**
 * Test if the byn file is valid for this module. It is possible to pass a directory instead of a file.
 * In that case, this method returns the path of the most recent appropriate byn file. This method will
 * ignore any firmware older than minrelease.
 *
 * @param serial : the serial number of the module to update
 * @param path : the path of a byn file or a directory that contains byn files
 * @param minrelease : a positive integer
 *
 * @return : the path of the byn file to use, or an empty string if no byn files matches the requirement
 *
 * On failure, returns a string that starts with "error:".
 */
string YFirmwareUpdate::CheckFirmware(string serial,string path,int minrelease)
{
    char errmsg[YOCTO_ERRMSG_LEN];
    char smallbuff[1024];
    char *bigbuff;
    int buffsize = 0;
    int fullsize = 0;
    int res = 0;
    string firmware_path;
    string release;
    fullsize = 0;
    release = YapiWrapper::ysprintf("%d",minrelease);
    res = yapiCheckFirmware(serial.c_str(), release.c_str(), path.c_str(), smallbuff, 1024, &fullsize, errmsg);
    if (res < 0) {
        firmware_path = "error:" + string(errmsg);
        return "error:" + string(errmsg);
    }
    if (fullsize <= 1024) {
        firmware_path = string(smallbuff, fullsize);
    } else {
        buffsize = fullsize;
        bigbuff = (char *)malloc(buffsize);
        res = yapiCheckFirmware(serial.c_str(), release.c_str(), path.c_str(), bigbuff, buffsize, &fullsize, errmsg);
        if (res < 0) {
            firmware_path = "error:" + string(errmsg);
        } else {
            firmware_path = string(bigbuff, fullsize);
        }
        free(bigbuff);
    }
    return firmware_path;
}

/**
 * Returns the progress of the firmware update, on a scale from 0 to 100. When the object is
 * instantiated, the progress is zero. The value is updated during the firmware update process until
 * the value of 100 is reached. The 100 value means that the firmware update was completed
 * successfully. If an error occurs during the firmware update, a negative value is returned, and the
 * error message can be retrieved with get_progressMessage.
 *
 * @return an integer in the range 0 to 100 (percentage of completion)
 *         or a negative error code in case of failure.
 */
int YFirmwareUpdate::get_progress(void)
{
    if (_progress >= 0) {
        this->_processMore(0);
    }
    return _progress;
}

/**
 * Returns the last progress message of the firmware update process. If an error occurs during the
 * firmware update process, the error message is returned
 *
 * @return a string  with the latest progress message, or the error message.
 */
string YFirmwareUpdate::get_progressMessage(void)
{
    return _progress_msg;
}

/**
 * Starts the firmware update process. This method starts the firmware update process in background. This method
 * returns immediately. You can monitor the progress of the firmware update with the get_progress()
 * and get_progressMessage() methods.
 *
 * @return an integer in the range 0 to 100 (percentage of completion),
 *         or a negative error code in case of failure.
 *
 * On failure returns a negative error code.
 */
int YFirmwareUpdate::startUpdate(void)
{
    string err;
    int leng = 0;
    err = _settings;
    leng = (int)(err).length();
    if ((leng >= 6) && ("error:" == err.substr(0, 6))) {
        _progress = -1;
        _progress_msg = err.substr(6, leng - 6);
    } else {
        _progress = 0;
        _progress_c = 0;
        this->_processMore(1);
    }
    return _progress;
}
//--- (end of generated code: YFirmwareUpdate implementation)


//--- (generated code: YDataStream implementation)
// static attributes


int YDataStream::_initFromDataSet(YDataSet* dataset,vector<int> encoded)
{
    int val = 0;
    int i = 0;
    int maxpos = 0;
    int ms_offset = 0;
    int samplesPerHour = 0;
    double fRaw = 0.0;
    double fRef = 0.0;
    vector<int> iCalib;
    // decode sequence header to extract data
    _runNo = encoded[0] + ((encoded[1] << 16));
    _utcStamp = encoded[2] + ((encoded[3] << 16));
    val = encoded[4];
    _isAvg = ((val & 0x100) == 0);
    samplesPerHour = (val & 0xff);
    if ((val & 0x100) != 0) {
        samplesPerHour = samplesPerHour * 3600;
    } else {
        if ((val & 0x200) != 0) {
            samplesPerHour = samplesPerHour * 60;
        }
    }
    _dataSamplesInterval = 3600.0 / samplesPerHour;
    ms_offset = encoded[6];
    if (ms_offset < 1000) {
        // new encoding -> add the ms to the UTC timestamp
        _startTime = _utcStamp + (ms_offset / 1000.0);
    } else {
        // legacy encoding subtract the measure interval form the UTC timestamp
        _startTime = _utcStamp - _dataSamplesInterval;
    }
    _firstMeasureDuration = encoded[5];
    if (!(_isAvg)) {
        _firstMeasureDuration = _firstMeasureDuration / 1000.0;
    }
    val = encoded[7];
    _isClosed = (val != 0xffff);
    if (val == 0xffff) {
        val = 0;
    }
    _nRows = val;
    if (_nRows > 0) {
        if (_firstMeasureDuration > 0) {
            _duration = _firstMeasureDuration + (_nRows - 1) * _dataSamplesInterval;
        } else {
            _duration = _nRows * _dataSamplesInterval;
        }
    } else {
        _duration = 0;
    }
    // precompute decoding parameters
    iCalib = dataset->_get_calibration();
    _caltyp = iCalib[0];
    if (_caltyp != 0) {
        _calhdl = YAPI::_getCalibrationHandler(_caltyp);
        maxpos = (int)iCalib.size();
        _calpar.clear();
        _calraw.clear();
        _calref.clear();
        i = 1;
        while (i < maxpos) {
            _calpar.push_back(iCalib[i]);
            i = i + 1;
        }
        i = 1;
        while (i + 1 < maxpos) {
            fRaw = iCalib[i];
            fRaw = fRaw / 1000.0;
            fRef = iCalib[i + 1];
            fRef = fRef / 1000.0;
            _calraw.push_back(fRaw);
            _calref.push_back(fRef);
            i = i + 2;
        }
    }
    // preload column names for backward-compatibility
    _functionId = dataset->get_functionId();
    if (_isAvg) {
        _columnNames.clear();
        _columnNames.push_back(YapiWrapper::ysprintf("%s_min",_functionId.c_str()));
        _columnNames.push_back(YapiWrapper::ysprintf("%s_avg",_functionId.c_str()));
        _columnNames.push_back(YapiWrapper::ysprintf("%s_max",_functionId.c_str()));
        _nCols = 3;
    } else {
        _columnNames.clear();
        _columnNames.push_back(_functionId);
        _nCols = 1;
    }
    // decode min/avg/max values for the sequence
    if (_nRows > 0) {
        _avgVal = this->_decodeAvg(encoded[8] + (((encoded[9] ^ 0x8000) << 16)), 1);
        _minVal = this->_decodeVal(encoded[10] + ((encoded[11] << 16)));
        _maxVal = this->_decodeVal(encoded[12] + ((encoded[13] << 16)));
    }
    return 0;
}

int YDataStream::_parseStream(string sdata)
{
    int idx = 0;
    vector<int> udat;
    vector<double> dat;
    if (_isLoaded && !(_isClosed)) {
        return YAPI_SUCCESS;
    }
    if ((int)(sdata).size() == 0) {
        _nRows = 0;
        return YAPI_SUCCESS;
    }

    udat = YAPI::_decodeWords(_parent->_json_get_string(sdata));
    _values.clear();
    idx = 0;
    if (_isAvg) {
        while (idx + 3 < (int)udat.size()) {
            dat.clear();
            if ((udat[idx] == 65535) && (udat[idx + 1] == 65535)) {
                dat.push_back(NAN);
                dat.push_back(NAN);
                dat.push_back(NAN);
            } else {
                dat.push_back(this->_decodeVal(udat[idx + 2] + (((udat[idx + 3]) << 16))));
                dat.push_back(this->_decodeAvg(udat[idx] + ((((udat[idx + 1]) ^ 0x8000) << 16)), 1));
                dat.push_back(this->_decodeVal(udat[idx + 4] + (((udat[idx + 5]) << 16))));
            }
            idx = idx + 6;
            _values.push_back(dat);
        }
    } else {
        while (idx + 1 < (int)udat.size()) {
            dat.clear();
            if ((udat[idx] == 65535) && (udat[idx + 1] == 65535)) {
                dat.push_back(NAN);
            } else {
                dat.push_back(this->_decodeAvg(udat[idx] + ((((udat[idx + 1]) ^ 0x8000) << 16)), 1));
            }
            _values.push_back(dat);
            idx = idx + 2;
        }
    }

    _nRows = (int)_values.size();
    _isLoaded = true;
    return YAPI_SUCCESS;
}

bool YDataStream::_wasLoaded(void)
{
    return _isLoaded;
}

string YDataStream::_get_url(void)
{
    string url;
    url = YapiWrapper::ysprintf("logger.json?id=%s&run=%d&utc=%u",_functionId.c_str(),_runNo,_utcStamp);
    return url;
}

string YDataStream::_get_baseurl(void)
{
    string url;
    url = YapiWrapper::ysprintf("logger.json?id=%s&run=%d&utc=",_functionId.c_str(),_runNo);
    return url;
}

string YDataStream::_get_urlsuffix(void)
{
    string url;
    url = YapiWrapper::ysprintf("%u",_utcStamp);
    return url;
}

int YDataStream::loadStream(void)
{
    return this->_parseStream(_parent->_download(this->_get_url()));
}

double YDataStream::_decodeVal(int w)
{
    double val = 0.0;
    val = w;
    val = val / 1000.0;
    if (_caltyp != 0) {
        if (_calhdl != NULL) {
            val = _calhdl(val, _caltyp, _calpar, _calraw, _calref);
        }
    }
    return val;
}

double YDataStream::_decodeAvg(int dw,int count)
{
    double val = 0.0;
    val = dw;
    val = val / 1000.0;
    if (_caltyp != 0) {
        if (_calhdl != NULL) {
            val = _calhdl(val, _caltyp, _calpar, _calraw, _calref);
        }
    }
    return val;
}

bool YDataStream::isClosed(void)
{
    return _isClosed;
}

/**
 * Returns the run index of the data stream. A run can be made of
 * multiple datastreams, for different time intervals.
 *
 * @return an unsigned number corresponding to the run index.
 */
int YDataStream::get_runIndex(void)
{
    return _runNo;
}

/**
 * Returns the relative start time of the data stream, measured in seconds.
 * For recent firmwares, the value is relative to the present time,
 * which means the value is always negative.
 * If the device uses a firmware older than version 13000, value is
 * relative to the start of the time the device was powered on, and
 * is always positive.
 * If you need an absolute UTC timestamp, use get_realStartTimeUTC().
 *
 * <b>DEPRECATED</b>: This method has been replaced by get_realStartTimeUTC().
 *
 * @return an unsigned number corresponding to the number of seconds
 *         between the start of the run and the beginning of this data
 *         stream.
 */
int YDataStream::get_startTime(void)
{
    return (int)(_utcStamp - ((unsigned)time(NULL)));
}

/**
 * Returns the start time of the data stream, relative to the Jan 1, 1970.
 * If the UTC time was not set in the datalogger at the time of the recording
 * of this data stream, this method returns 0.
 *
 * <b>DEPRECATED</b>: This method has been replaced by get_realStartTimeUTC().
 *
 * @return an unsigned number corresponding to the number of seconds
 *         between the Jan 1, 1970 and the beginning of this data
 *         stream (i.e. Unix time representation of the absolute time).
 */
s64 YDataStream::get_startTimeUTC(void)
{
    return (int) floor(_startTime+0.5);
}

/**
 * Returns the start time of the data stream, relative to the Jan 1, 1970.
 * If the UTC time was not set in the datalogger at the time of the recording
 * of this data stream, this method returns 0.
 *
 * @return a floating-point number  corresponding to the number of seconds
 *         between the Jan 1, 1970 and the beginning of this data
 *         stream (i.e. Unix time representation of the absolute time).
 */
double YDataStream::get_realStartTimeUTC(void)
{
    return _startTime;
}

/**
 * Returns the number of milliseconds between two consecutive
 * rows of this data stream. By default, the data logger records one row
 * per second, but the recording frequency can be changed for
 * each device function
 *
 * @return an unsigned number corresponding to a number of milliseconds.
 */
int YDataStream::get_dataSamplesIntervalMs(void)
{
    return (int) floor(_dataSamplesInterval*1000+0.5);
}

double YDataStream::get_dataSamplesInterval(void)
{
    return _dataSamplesInterval;
}

double YDataStream::get_firstDataSamplesInterval(void)
{
    return _firstMeasureDuration;
}

/**
 * Returns the number of data rows present in this stream.
 *
 * If the device uses a firmware older than version 13000,
 * this method fetches the whole data stream from the device
 * if not yet done, which can cause a little delay.
 *
 * @return an unsigned number corresponding to the number of rows.
 *
 * On failure, throws an exception or returns zero.
 */
int YDataStream::get_rowCount(void)
{
    if ((_nRows != 0) && _isClosed) {
        return _nRows;
    }
    this->loadStream();
    return _nRows;
}

/**
 * Returns the number of data columns present in this stream.
 * The meaning of the values present in each column can be obtained
 * using the method get_columnNames().
 *
 * If the device uses a firmware older than version 13000,
 * this method fetches the whole data stream from the device
 * if not yet done, which can cause a little delay.
 *
 * @return an unsigned number corresponding to the number of columns.
 *
 * On failure, throws an exception or returns zero.
 */
int YDataStream::get_columnCount(void)
{
    if (_nCols != 0) {
        return _nCols;
    }
    this->loadStream();
    return _nCols;
}

/**
 * Returns the title (or meaning) of each data column present in this stream.
 * In most case, the title of the data column is the hardware identifier
 * of the sensor that produced the data. For streams recorded at a lower
 * recording rate, the dataLogger stores the min, average and max value
 * during each measure interval into three columns with suffixes _min,
 * _avg and _max respectively.
 *
 * If the device uses a firmware older than version 13000,
 * this method fetches the whole data stream from the device
 * if not yet done, which can cause a little delay.
 *
 * @return a list containing as many strings as there are columns in the
 *         data stream.
 *
 * On failure, throws an exception or returns an empty array.
 */
vector<string> YDataStream::get_columnNames(void)
{
    if ((int)_columnNames.size() != 0) {
        return _columnNames;
    }
    this->loadStream();
    return _columnNames;
}

/**
 * Returns the smallest measure observed within this stream.
 * If the device uses a firmware older than version 13000,
 * this method will always return YDataStream::DATA_INVALID.
 *
 * @return a floating-point number corresponding to the smallest value,
 *         or YDataStream::DATA_INVALID if the stream is not yet complete (still recording).
 *
 * On failure, throws an exception or returns YDataStream::DATA_INVALID.
 */
double YDataStream::get_minValue(void)
{
    return _minVal;
}

/**
 * Returns the average of all measures observed within this stream.
 * If the device uses a firmware older than version 13000,
 * this method will always return YDataStream::DATA_INVALID.
 *
 * @return a floating-point number corresponding to the average value,
 *         or YDataStream::DATA_INVALID if the stream is not yet complete (still recording).
 *
 * On failure, throws an exception or returns YDataStream::DATA_INVALID.
 */
double YDataStream::get_averageValue(void)
{
    return _avgVal;
}

/**
 * Returns the largest measure observed within this stream.
 * If the device uses a firmware older than version 13000,
 * this method will always return YDataStream::DATA_INVALID.
 *
 * @return a floating-point number corresponding to the largest value,
 *         or YDataStream::DATA_INVALID if the stream is not yet complete (still recording).
 *
 * On failure, throws an exception or returns YDataStream::DATA_INVALID.
 */
double YDataStream::get_maxValue(void)
{
    return _maxVal;
}

double YDataStream::get_realDuration(void)
{
    if (_isClosed) {
        return _duration;
    }
    return (double) ((unsigned)time(NULL)) - _utcStamp;
}

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
vector< vector<double> > YDataStream::get_dataRows(void)
{
    if (((int)_values.size() == 0) || !(_isClosed)) {
        this->loadStream();
    }
    return _values;
}

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
 * On failure, throws an exception or returns YDataStream::DATA_INVALID.
 */
double YDataStream::get_data(int row,int col)
{
    if (((int)_values.size() == 0) || !(_isClosed)) {
        this->loadStream();
    }
    if (row >= (int)_values.size()) {
        return Y_DATA_INVALID;
    }
    if (col >= (int)_values[row].size()) {
        return Y_DATA_INVALID;
    }
    return _values[row][col];
}
//--- (end of generated code: YDataStream implementation)

YMeasure::YMeasure(double start, double end, double minVal, double avgVal, double maxVal):
    //--- (generated code: YMeasure initialization)
    _start(0.0)
    ,_end(0.0)
    ,_minVal(0.0)
    ,_avgVal(0.0)
    ,_maxVal(0.0)
//--- (end of generated code: YMeasure initialization)
{
    _start = start;
    _end = end;
    _minVal = minVal;
    _avgVal = avgVal;
    _maxVal = maxVal;
    _startTime_t = (time_t)(start + 0.5);
    _stopTime_t = (time_t)(end + 0.5);
}

YMeasure::YMeasure():
    //--- (generated code: YMeasure initialization)
    _start(0.0)
    ,_end(0.0)
    ,_minVal(0.0)
    ,_avgVal(0.0)
    ,_maxVal(0.0)
//--- (end of generated code: YMeasure initialization)
{
    _start = 0;
    _end = 0;
    _minVal = 0;
    _avgVal = 0;
    _maxVal = 0;
    _startTime_t = 0;
    _stopTime_t = 0;
}

//--- (generated code: YMeasure implementation)
// static attributes


/**
 * Returns the start time of the measure, relative to the Jan 1, 1970 UTC
 * (Unix timestamp). When the recording rate is higher then 1 sample
 * per second, the timestamp may have a fractional part.
 *
 * @return a floating point number corresponding to the number of seconds
 *         between the Jan 1, 1970 UTC and the beginning of this measure.
 */
double YMeasure::get_startTimeUTC(void)
{
    return _start;
}

/**
 * Returns the end time of the measure, relative to the Jan 1, 1970 UTC
 * (Unix timestamp). When the recording rate is higher than 1 sample
 * per second, the timestamp may have a fractional part.
 *
 * @return a floating point number corresponding to the number of seconds
 *         between the Jan 1, 1970 UTC and the end of this measure.
 */
double YMeasure::get_endTimeUTC(void)
{
    return _end;
}

/**
 * Returns the smallest value observed during the time interval
 * covered by this measure.
 *
 * @return a floating-point number corresponding to the smallest value observed.
 */
double YMeasure::get_minValue(void)
{
    return _minVal;
}

/**
 * Returns the average value observed during the time interval
 * covered by this measure.
 *
 * @return a floating-point number corresponding to the average value observed.
 */
double YMeasure::get_averageValue(void)
{
    return _avgVal;
}

/**
 * Returns the largest value observed during the time interval
 * covered by this measure.
 *
 * @return a floating-point number corresponding to the largest value observed.
 */
double YMeasure::get_maxValue(void)
{
    return _maxVal;
}
//--- (end of generated code: YMeasure implementation)

time_t* YMeasure::get_startTimeUTC_asTime_t(time_t *time)
{
    if (time) {
        memcpy(time, &this->_startTime_t, sizeof(time_t));
    }
    return &this->_startTime_t;
}

time_t* YMeasure::get_endTimeUTC_asTime_t(time_t *time)
{
    if (time) {
        memcpy(time, &this->_stopTime_t, sizeof(time_t));
    }
    return &this->_stopTime_t;
}

//--- (generated code: YDataSet implementation)
// static attributes


vector<int> YDataSet::_get_calibration(void)
{
    return _calib;
}

int YDataSet::loadSummary(string data)
{
    vector< vector<double> > dataRows;
    double tim = 0.0;
    double mitv = 0.0;
    double itv = 0.0;
    double fitv = 0.0;
    double end_ = 0.0;
    int nCols = 0;
    int minCol = 0;
    int avgCol = 0;
    int maxCol = 0;
    int res = 0;
    int m_pos = 0;
    double previewTotalTime = 0.0;
    double previewTotalAvg = 0.0;
    double previewMinVal = 0.0;
    double previewMaxVal = 0.0;
    double previewAvgVal = 0.0;
    double previewStartMs = 0.0;
    double previewStopMs = 0.0;
    double previewDuration = 0.0;
    double streamStartTimeMs = 0.0;
    double streamDuration = 0.0;
    double streamEndTimeMs = 0.0;
    double minVal = 0.0;
    double avgVal = 0.0;
    double maxVal = 0.0;
    double summaryStartMs = 0.0;
    double summaryStopMs = 0.0;
    double summaryTotalTime = 0.0;
    double summaryTotalAvg = 0.0;
    double summaryMinVal = 0.0;
    double summaryMaxVal = 0.0;
    string url;
    string strdata;
    vector<double> measure_data;

    if (_progress < 0) {
        strdata = data;
        if (strdata == "{}") {
            _parent->_throw(YAPI_VERSION_MISMATCH, "device firmware is too old");
            return YAPI_VERSION_MISMATCH;
        }
        res = this->_parse(strdata);
        if (res < 0) {
            return res;
        }
    }
    summaryTotalTime = 0;
    summaryTotalAvg = 0;
    summaryMinVal = YAPI_MAX_DOUBLE;
    summaryMaxVal = YAPI_MIN_DOUBLE;
    summaryStartMs = YAPI_MAX_DOUBLE;
    summaryStopMs = YAPI_MIN_DOUBLE;

    // Parse complete streams
    for (unsigned ii = 0; ii < _streams.size(); ii++) {
        streamStartTimeMs = floor(_streams[ii]->get_realStartTimeUTC() * 1000+0.5);
        streamDuration = _streams[ii]->get_realDuration();
        streamEndTimeMs = streamStartTimeMs + floor(streamDuration * 1000+0.5);
        if ((streamStartTimeMs >= _startTimeMs) && ((_endTimeMs == 0) || (streamEndTimeMs <= _endTimeMs))) {
            // stream that are completely inside the dataset
            previewMinVal = _streams[ii]->get_minValue();
            previewAvgVal = _streams[ii]->get_averageValue();
            previewMaxVal = _streams[ii]->get_maxValue();
            previewStartMs = streamStartTimeMs;
            previewStopMs = streamEndTimeMs;
            previewDuration = streamDuration;
        } else {
            // stream that are partially in the dataset
            // we need to parse data to filter value outside the dataset
            if (!(_streams[ii]->_wasLoaded())) {
                url = _streams[ii]->_get_url();
                data = _parent->_download(url);
                _streams[ii]->_parseStream(data);
            }
            dataRows = _streams[ii]->get_dataRows();
            if ((int)dataRows.size() == 0) {
                return this->get_progress();
            }
            tim = streamStartTimeMs;
            fitv = floor(_streams[ii]->get_firstDataSamplesInterval() * 1000+0.5);
            itv = floor(_streams[ii]->get_dataSamplesInterval() * 1000+0.5);
            nCols = (int)dataRows[0].size();
            minCol = 0;
            if (nCols > 2) {
                avgCol = 1;
            } else {
                avgCol = 0;
            }
            if (nCols > 2) {
                maxCol = 2;
            } else {
                maxCol = 0;
            }
            previewTotalTime = 0;
            previewTotalAvg = 0;
            previewStartMs = streamEndTimeMs;
            previewStopMs = streamStartTimeMs;
            previewMinVal = YAPI_MAX_DOUBLE;
            previewMaxVal = YAPI_MIN_DOUBLE;
            m_pos = 0;
            while (m_pos < (int)dataRows.size()) {
                measure_data = dataRows[m_pos];
                if (m_pos == 0) {
                    mitv = fitv;
                } else {
                    mitv = itv;
                }
                end_ = tim + mitv;
                if ((end_ > _startTimeMs) && ((_endTimeMs == 0) || (tim < _endTimeMs))) {
                    minVal = measure_data[minCol];
                    avgVal = measure_data[avgCol];
                    maxVal = measure_data[maxCol];
                    if (previewStartMs > tim) {
                        previewStartMs = tim;
                    }
                    if (previewStopMs < end_) {
                        previewStopMs = end_;
                    }
                    if (previewMinVal > minVal) {
                        previewMinVal = minVal;
                    }
                    if (previewMaxVal < maxVal) {
                        previewMaxVal = maxVal;
                    }
                    if (!(yyisnan(avgVal))) {
                        previewTotalAvg = previewTotalAvg + (avgVal * mitv);
                        previewTotalTime = previewTotalTime + mitv;
                    }
                }
                tim = end_;
                m_pos = m_pos + 1;
            }
            if (previewTotalTime > 0) {
                previewAvgVal = previewTotalAvg / previewTotalTime;
                previewDuration = (previewStopMs - previewStartMs) / 1000.0;
            } else {
                previewAvgVal = 0.0;
                previewDuration = 0.0;
            }
        }
        _preview.push_back(YMeasure(previewStartMs / 1000.0,previewStopMs / 1000.0,previewMinVal,previewAvgVal,previewMaxVal));
        if (summaryMinVal > previewMinVal) {
            summaryMinVal = previewMinVal;
        }
        if (summaryMaxVal < previewMaxVal) {
            summaryMaxVal = previewMaxVal;
        }
        if (summaryStartMs > previewStartMs) {
            summaryStartMs = previewStartMs;
        }
        if (summaryStopMs < previewStopMs) {
            summaryStopMs = previewStopMs;
        }
        summaryTotalAvg = summaryTotalAvg + (previewAvgVal * previewDuration);
        summaryTotalTime = summaryTotalTime + previewDuration;
    }
    if ((_startTimeMs == 0) || (_startTimeMs > summaryStartMs)) {
        _startTimeMs = summaryStartMs;
    }
    if ((_endTimeMs == 0) || (_endTimeMs < summaryStopMs)) {
        _endTimeMs = summaryStopMs;
    }
    if (summaryTotalTime > 0) {
        _summary = YMeasure(summaryStartMs / 1000.0,summaryStopMs / 1000.0,summaryMinVal,summaryTotalAvg / summaryTotalTime,summaryMaxVal);
    } else {
        _summary = YMeasure(0.0,0.0,YAPI_INVALID_DOUBLE,YAPI_INVALID_DOUBLE,YAPI_INVALID_DOUBLE);
    }
    return this->get_progress();
}

int YDataSet::processMore(int progress,string data)
{
    YDataStream* stream = NULL;
    vector< vector<double> > dataRows;
    double tim = 0.0;
    double itv = 0.0;
    double fitv = 0.0;
    double avgv = 0.0;
    double end_ = 0.0;
    int nCols = 0;
    int minCol = 0;
    int avgCol = 0;
    int maxCol = 0;
    bool firstMeasure = 0;
    string baseurl;
    string url;
    string suffix;
    vector<string> suffixes;
    int idx = 0;
    string bulkFile;
    int urlIdx = 0;
    vector<string> streamBin;

    if (progress != _progress) {
        return _progress;
    }
    if (_progress < 0) {
        return this->loadSummary(data);
    }
    stream = _streams[_progress];
    if (!(stream->_wasLoaded())) {
        stream->_parseStream(data);
    }
    dataRows = stream->get_dataRows();
    _progress = _progress + 1;
    if ((int)dataRows.size() == 0) {
        return this->get_progress();
    }
    tim = floor(stream->get_realStartTimeUTC() * 1000+0.5);
    fitv = floor(stream->get_firstDataSamplesInterval() * 1000+0.5);
    itv = floor(stream->get_dataSamplesInterval() * 1000+0.5);
    if (fitv == 0) {
        fitv = itv;
    }
    if (tim < itv) {
        tim = itv;
    }
    nCols = (int)dataRows[0].size();
    minCol = 0;
    if (nCols > 2) {
        avgCol = 1;
    } else {
        avgCol = 0;
    }
    if (nCols > 2) {
        maxCol = 2;
    } else {
        maxCol = 0;
    }

    firstMeasure = true;
    for (unsigned ii = 0; ii < dataRows.size(); ii++) {
        if (firstMeasure) {
            end_ = tim + fitv;
            firstMeasure = false;
        } else {
            end_ = tim + itv;
        }
        avgv = dataRows[ii][avgCol];
        if ((end_ > _startTimeMs) && ((_endTimeMs == 0) || (tim < _endTimeMs)) && !(yyisnan(avgv))) {
            _measures.push_back(YMeasure(tim / 1000,end_ / 1000,dataRows[ii][minCol],avgv,dataRows[ii][maxCol]));
        }
        tim = end_;
    }
    // Perform bulk preload to speed-up network transfer
    if ((_bulkLoad > 0) && (_progress < (int)_streams.size())) {
        stream = _streams[_progress];
        if (stream->_wasLoaded()) {
            return this->get_progress();
        }
        baseurl = stream->_get_baseurl();
        url = stream->_get_url();
        suffix = stream->_get_urlsuffix();
        suffixes.push_back(suffix);
        idx = _progress + 1;
        while ((idx < (int)_streams.size()) && ((int)suffixes.size() < _bulkLoad)) {
            stream = _streams[idx];
            if (!(stream->_wasLoaded()) && (stream->_get_baseurl() == baseurl)) {
                suffix = stream->_get_urlsuffix();
                suffixes.push_back(suffix);
                url = url + "," + suffix;
            }
            idx = idx + 1;
        }
        bulkFile = _parent->_download(url);
        streamBin = _parent->_json_get_array(bulkFile);
        urlIdx = 0;
        idx = _progress;
        while ((idx < (int)_streams.size()) && (urlIdx < (int)suffixes.size()) && (urlIdx < (int)streamBin.size())) {
            stream = _streams[idx];
            if ((stream->_get_baseurl() == baseurl) && (stream->_get_urlsuffix() == suffixes[urlIdx])) {
                stream->_parseStream(streamBin[urlIdx]);
                urlIdx = urlIdx + 1;
            }
            idx = idx + 1;
        }
    }
    return this->get_progress();
}

vector<YDataStream*> YDataSet::get_privateDataStreams(void)
{
    return _streams;
}

/**
 * Returns the unique hardware identifier of the function who performed the measures,
 * in the form SERIAL.FUNCTIONID. The unique hardware identifier is composed of the
 * device serial number and of the hardware identifier of the function
 * (for example THRMCPL1-123456.temperature1)
 *
 * @return a string that uniquely identifies the function (ex: THRMCPL1-123456.temperature1)
 *
 * On failure, throws an exception or returns  YDataSet::HARDWAREID_INVALID.
 */
string YDataSet::get_hardwareId(void)
{
    YModule* mo = NULL;
    if (!(_hardwareId == "")) {
        return _hardwareId;
    }
    mo = _parent->get_module();
    _hardwareId = YapiWrapper::ysprintf("%s.%s",mo->get_serialNumber().c_str(),this->get_functionId().c_str());
    return _hardwareId;
}

/**
 * Returns the hardware identifier of the function that performed the measure,
 * without reference to the module. For example temperature1.
 *
 * @return a string that identifies the function (ex: temperature1)
 */
string YDataSet::get_functionId(void)
{
    return _functionId;
}

/**
 * Returns the measuring unit for the measured value.
 *
 * @return a string that represents a physical unit.
 *
 * On failure, throws an exception or returns  YDataSet::UNIT_INVALID.
 */
string YDataSet::get_unit(void)
{
    return _unit;
}

/**
 * Returns the start time of the dataset, relative to the Jan 1, 1970.
 * When the YDataSet object is created, the start time is the value passed
 * in parameter to the get_dataSet() function. After the
 * very first call to loadMore(), the start time is updated
 * to reflect the timestamp of the first measure actually found in the
 * dataLogger within the specified range.
 *
 * <b>DEPRECATED</b>: This method has been replaced by get_summary()
 * which contain more precise informations.
 *
 * @return an unsigned number corresponding to the number of seconds
 *         between the Jan 1, 1970 and the beginning of this data
 *         set (i.e. Unix time representation of the absolute time).
 */
s64 YDataSet::get_startTimeUTC(void)
{
    return this->imm_get_startTimeUTC();
}

s64 YDataSet::imm_get_startTimeUTC(void)
{
    return (s64) (_startTimeMs / 1000.0);
}

/**
 * Returns the end time of the dataset, relative to the Jan 1, 1970.
 * When the YDataSet object is created, the end time is the value passed
 * in parameter to the get_dataSet() function. After the
 * very first call to loadMore(), the end time is updated
 * to reflect the timestamp of the last measure actually found in the
 * dataLogger within the specified range.
 *
 * <b>DEPRECATED</b>: This method has been replaced by get_summary()
 * which contain more precise informations.
 *
 * @return an unsigned number corresponding to the number of seconds
 *         between the Jan 1, 1970 and the end of this data
 *         set (i.e. Unix time representation of the absolute time).
 */
s64 YDataSet::get_endTimeUTC(void)
{
    return this->imm_get_endTimeUTC();
}

s64 YDataSet::imm_get_endTimeUTC(void)
{
    return (s64) floor(_endTimeMs / 1000.0+0.5);
}

/**
 * Returns the progress of the downloads of the measures from the data logger,
 * on a scale from 0 to 100. When the object is instantiated by get_dataSet,
 * the progress is zero. Each time loadMore() is invoked, the progress
 * is updated, to reach the value 100 only once all measures have been loaded.
 *
 * @return an integer in the range 0 to 100 (percentage of completion).
 */
int YDataSet::get_progress(void)
{
    if (_progress < 0) {
        return 0;
    }
    // index not yet loaded
    if (_progress >= (int)_streams.size()) {
        return 100;
    }
    return ((1 + (1 + _progress) * 98) / (1 + (int)_streams.size()));
}

/**
 * Loads the next block of measures from the dataLogger, and updates
 * the progress indicator.
 *
 * @return an integer in the range 0 to 100 (percentage of completion),
 *         or a negative error code in case of failure.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDataSet::loadMore(void)
{
    string url;
    YDataStream* stream = NULL;
    if (_progress < 0) {
        url = YapiWrapper::ysprintf("logger.json?id=%s",_functionId.c_str());
        if (_startTimeMs != 0) {
            url = YapiWrapper::ysprintf("%s&from=%u",url.c_str(),this->imm_get_startTimeUTC());
        }
        if (_endTimeMs != 0) {
            url = YapiWrapper::ysprintf("%s&to=%u",url.c_str(),this->imm_get_endTimeUTC() + 1);
        }
    } else {
        if (_progress >= (int)_streams.size()) {
            return 100;
        } else {
            stream = _streams[_progress];
            if (stream->_wasLoaded()) {
                // Do not reload stream if it was already loaded
                return this->processMore(_progress, "");
            }
            url = stream->_get_url();
        }
    }
    try {
        return this->processMore(_progress, _parent->_download(url));
    } catch (std::exception&) {
        return this->processMore(_progress, _parent->_download(url));
    }
}

/**
 * Returns an YMeasure object which summarizes the whole
 * YDataSet:: In includes the following information:
 * - the start of a time interval
 * - the end of a time interval
 * - the minimal value observed during the time interval
 * - the average value observed during the time interval
 * - the maximal value observed during the time interval
 *
 * This summary is available as soon as loadMore() has
 * been called for the first time.
 *
 * @return an YMeasure object
 */
YMeasure YDataSet::get_summary(void)
{
    return _summary;
}

/**
 * Returns a condensed version of the measures that can
 * retrieved in this YDataSet, as a list of YMeasure
 * objects. Each item includes:
 * - the start of a time interval
 * - the end of a time interval
 * - the minimal value observed during the time interval
 * - the average value observed during the time interval
 * - the maximal value observed during the time interval
 *
 * This preview is available as soon as loadMore() has
 * been called for the first time.
 *
 * @return a table of records, where each record depicts the
 *         measured values during a time interval
 *
 * On failure, throws an exception or returns an empty array.
 */
vector<YMeasure> YDataSet::get_preview(void)
{
    return _preview;
}

/**
 * Returns the detailed set of measures for the time interval corresponding
 * to a given condensed measures previously returned by get_preview().
 * The result is provided as a list of YMeasure objects.
 *
 * @param measure : condensed measure from the list previously returned by
 *         get_preview().
 *
 * @return a table of records, where each record depicts the
 *         measured values during a time interval
 *
 * On failure, throws an exception or returns an empty array.
 */
vector<YMeasure> YDataSet::get_measuresAt(YMeasure measure)
{
    double startUtcMs = 0.0;
    YDataStream* stream = NULL;
    vector< vector<double> > dataRows;
    vector<YMeasure> measures;
    double tim = 0.0;
    double itv = 0.0;
    double end_ = 0.0;
    int nCols = 0;
    int minCol = 0;
    int avgCol = 0;
    int maxCol = 0;

    startUtcMs = measure.get_startTimeUTC() * 1000;
    stream = NULL;
    for (unsigned ii = 0; ii < _streams.size(); ii++) {
        if (floor(_streams[ii]->get_realStartTimeUTC() *1000+0.5) == startUtcMs) {
            stream = _streams[ii];
        }
    }
    if (stream == NULL) {
        return measures;
    }
    dataRows = stream->get_dataRows();
    if ((int)dataRows.size() == 0) {
        return measures;
    }
    tim = floor(stream->get_realStartTimeUTC() * 1000+0.5);
    itv = floor(stream->get_dataSamplesInterval() * 1000+0.5);
    if (tim < itv) {
        tim = itv;
    }
    nCols = (int)dataRows[0].size();
    minCol = 0;
    if (nCols > 2) {
        avgCol = 1;
    } else {
        avgCol = 0;
    }
    if (nCols > 2) {
        maxCol = 2;
    } else {
        maxCol = 0;
    }

    for (unsigned ii = 0; ii < dataRows.size(); ii++) {
        end_ = tim + itv;
        if ((end_ > _startTimeMs) && ((_endTimeMs == 0) || (tim < _endTimeMs))) {
            measures.push_back(YMeasure(tim / 1000.0,end_ / 1000.0,dataRows[ii][minCol],dataRows[ii][avgCol],dataRows[ii][maxCol]));
        }
        tim = end_;
    }
    return measures;
}

/**
 * Returns all measured values currently available for this DataSet,
 * as a list of YMeasure objects. Each item includes:
 * - the start of the measure time interval
 * - the end of the measure time interval
 * - the minimal value observed during the time interval
 * - the average value observed during the time interval
 * - the maximal value observed during the time interval
 *
 * Before calling this method, you should call loadMore()
 * to load data from the device. You may have to call loadMore()
 * several time until all rows are loaded, but you can start
 * looking at available data rows before the load is complete.
 *
 * The oldest measures are always loaded first, and the most
 * recent measures will be loaded last. As a result, timestamps
 * are normally sorted in ascending order within the measure table,
 * unless there was an unexpected adjustment of the datalogger UTC
 * clock.
 *
 * @return a table of records, where each record depicts the
 *         measured value for a given time interval
 *
 * On failure, throws an exception or returns an empty array.
 */
vector<YMeasure> YDataSet::get_measures(void)
{
    return _measures;
}
//--- (end of generated code: YDataSet implementation)


YAPIContext::YAPIContext():
    //--- (generated code: YAPIContext initialization)
    _defaultCacheValidity(5)
//--- (end of generated code: YAPIContext initialization)
{}

YAPIContext::~YAPIContext()
{
    //--- (generated code: YAPIContext cleanup)
//--- (end of generated code: YAPIContext cleanup)
}


//--- (generated code: YAPIContext implementation)
// static attributes


/**
 * Modifies the delay between each forced enumeration of the used YoctoHubs.
 * By default, the library performs a full enumeration every 10 seconds.
 * To reduce network traffic, you can increase this delay.
 * It's particularly useful when a YoctoHub is connected to the GSM network
 * where traffic is billed. This parameter doesn't impact modules connected by USB,
 * nor the working of module arrival/removal callbacks.
 * Note: you must call this function after yInitAPI.
 *
 * @param deviceListValidity : nubmer of seconds between each enumeration.
 * @noreturn
 */
void YAPIContext::SetDeviceListValidity(int deviceListValidity)
{
    yapiSetNetDevListValidity(deviceListValidity);
}

/**
 * Returns the delay between each forced enumeration of the used YoctoHubs.
 * Note: you must call this function after yInitAPI.
 *
 * @return the number of seconds between each enumeration.
 */
int YAPIContext::GetDeviceListValidity(void)
{
    int res = 0;
    res = yapiGetNetDevListValidity();
    return res;
}

/**
 * Returns the path to the dynamic YAPI library. This function is useful for debugging problems loading the
 * dynamic library YAPI:: This function is supported by the C#, Python and VB languages. The other
 * libraries return an
 * empty string.
 *
 * @return a string containing the path of the YAPI dynamic library.
 */
string YAPIContext::GetYAPISharedLibraryPath(void)
{
    char errmsg[YOCTO_ERRMSG_LEN];
    char smallbuff[4096];
    int res = 0;
    string path;
    res = yapiGetDLLPath(smallbuff, 4096, errmsg);
    if (res < 0) {
        path = "error:" + string(errmsg);
    } else {
        path = string(smallbuff, res);
    }
    return path;
}

/**
 * Adds a UDEV rule which authorizes all users to access Yoctopuce modules
 * connected to the USB ports. This function works only under Linux. The process that
 * calls this method must have root privileges because this method changes the Linux configuration.
 *
 * @param force : if true, overwrites any existing rule.
 *
 * @return an empty string if the rule has been added.
 *
 * On failure, returns a string that starts with "error:".
 */
string YAPIContext::AddUdevRule(bool force)
{
    string msg;
    int res = 0;
    int c_force = 0;
    char errmsg[YOCTO_ERRMSG_LEN];
    if (force) {
        c_force = 1;
    } else {
        c_force = 0;
    }
    res = yapiAddUdevRulesForYocto(c_force, errmsg);
    if (res < 0) {
        msg = "error: " + string(errmsg);
    } else {
        msg = "";
    }
    return msg;
}

/**
 * Download the TLS/SSL certificate from the hub. This function allows to download a TLS/SSL certificate to add it
 * to the list of trusted certificates using the AddTrustedCertificates method.
 *
 * @param url : the root URL of the VirtualHub V2 or HTTP server.
 * @param mstimeout : the number of milliseconds available to download the certificate.
 *
 * @return a string containing the certificate. In case of error, returns a string starting with "error:".
 */
string YAPIContext::DownloadHostCertificate(string url,u64 mstimeout)
{
    char errmsg[YOCTO_ERRMSG_LEN];
    char smallbuff[4096];
    char *bigbuff;
    int buffsize = 0;
    int fullsize = 0;
    int res = 0;
    string certifcate;
    fullsize = 0;
    res = yapiGetRemoteCertificate(url.c_str(), mstimeout, smallbuff, 4096, &fullsize, errmsg);
    if (res < 0) {
        if (res == YAPI_BUFFER_TOO_SMALL) {
            fullsize = fullsize * 2;
            buffsize = fullsize;
            bigbuff = (char *)malloc(buffsize);
            res = yapiGetRemoteCertificate(url.c_str(), mstimeout, bigbuff, buffsize, &fullsize, errmsg);
            if (res < 0) {
                certifcate = "error:" + string(errmsg);
            } else {
                certifcate = string(bigbuff, fullsize);
            }
            free(bigbuff);
        } else {
            certifcate = "error:" + string(errmsg);
        }
        return certifcate;
    } else {
        certifcate = string(smallbuff, fullsize);
    }
    return certifcate;
}

/**
 * Adds a TLS/SSL certificate to the list of trusted certificates. By default, the library
 * library will reject TLS/SSL connections to servers whose certificate is not known. This function
 * function allows to add a list of known certificates. It is also possible to disable the verification
 * using the SetNetworkSecurityOptions method.
 *
 * @param certificate : a string containing one or more certificates.
 *
 * @return an empty string if the certificate has been added correctly.
 *         In case of error, returns a string starting with "error:".
 */
string YAPIContext::AddTrustedCertificates(string certificate)
{
    char errmsg[YOCTO_ERRMSG_LEN];
    int size = 0;
    int res = 0;
    // null char must be inclued
    size = (int)(certificate).length() + 1;
    res = yapiAddSSLCertificateCli(certificate.c_str(), size, errmsg);
    if (res < 0) {
        return string(errmsg);
    } else {
        return "";
    }
}

/**
 * Set the path of Certificate Authority file on local filesystem. This method takes as a parameter
 * the path of a file containing all certificates in PEM format.
 * For technical reasons, only one file can be specified. So if you need to connect to several Hubs
 * instances with self-signed certificates, you'll need to use
 * a single file containing all the certificates end-to-end. Passing a empty string will restore the
 * default settings. This option is only supported by PHP library.
 *
 * @param certificatePath : the path of the file containing all certificates in PEM format.
 *
 * @return an empty string if the certificate has been added correctly.
 *         In case of error, returns a string starting with "error:".
 */
string YAPIContext::SetTrustedCertificatesList(string certificatePath)
{
    char errmsg[YOCTO_ERRMSG_LEN];
    int res = 0;
    res = yapiSetTrustedCertificatesList(certificatePath.c_str(), errmsg);
    if (res < 0) {
        return string(errmsg);
    } else {
        return "";
    }
}

/**
 * Enables or disables certain TLS/SSL certificate checks.
 *
 * @param opts : The options are YAPI::NO_TRUSTED_CA_CHECK,
 *         YAPI::NO_EXPIRATION_CHECK, YAPI::NO_HOSTNAME_CHECK.
 *
 * @return an empty string if the options are taken into account.
 *         On error, returns a string beginning with "error:".
 */
string YAPIContext::SetNetworkSecurityOptions(int opts)
{
    char errmsg[YOCTO_ERRMSG_LEN];
    int res = 0;
    res = yapiSetNetworkSecurityOptions(opts, errmsg);
    if (res < 0) {
        return string(errmsg);
    } else {
        return "";
    }
}

/**
 * Modifies the network connection delay for yRegisterHub() and yUpdateDeviceList().
 * This delay impacts only the YoctoHubs and VirtualHub
 * which are accessible through the network. By default, this delay is of 20000 milliseconds,
 * but depending on your network you may want to change this delay,
 * gor example if your network infrastructure is based on a GSM connection.
 *
 * @param networkMsTimeout : the network connection delay in milliseconds.
 * @noreturn
 */
void YAPIContext::SetNetworkTimeout(int networkMsTimeout)
{
    yapiSetNetworkTimeout(networkMsTimeout);
}

/**
 * Returns the network connection delay for yRegisterHub() and yUpdateDeviceList().
 * This delay impacts only the YoctoHubs and VirtualHub
 * which are accessible through the network. By default, this delay is of 20000 milliseconds,
 * but depending on your network you may want to change this delay,
 * for example if your network infrastructure is based on a GSM connection.
 *
 * @return the network connection delay in milliseconds.
 */
int YAPIContext::GetNetworkTimeout(void)
{
    int res = 0;
    res = yapiGetNetworkTimeout();
    return res;
}

/**
 * Change the validity period of the data loaded by the library.
 * By default, when accessing a module, all the attributes of the
 * module functions are automatically kept in cache for the standard
 * duration (5 ms). This method can be used to change this standard duration,
 * for example in order to reduce network or USB traffic. This parameter
 * does not affect value change callbacks
 * Note: This function must be called after yInitAPI.
 *
 * @param cacheValidityMs : an integer corresponding to the validity attributed to the
 *         loaded function parameters, in milliseconds.
 * @noreturn
 */
void YAPIContext::SetCacheValidity(u64 cacheValidityMs)
{
    _defaultCacheValidity = cacheValidityMs;
}

/**
 * Returns the validity period of the data loaded by the library.
 * This method returns the cache validity of all attributes
 * module functions.
 * Note: This function must be called after yInitAPI .
 *
 * @return an integer corresponding to the validity attributed to the
 *         loaded function parameters, in milliseconds
 */
u64 YAPIContext::GetCacheValidity(void)
{
    return _defaultCacheValidity;
}

YHub* YAPIContext::nextHubInUseInternal(int hubref)
{
    int nextref = 0;
    nextref = yapiGetNextHubRef(hubref);
    if (nextref < 0) {
        return NULL;
    }
    return this->getYHubObj(nextref);
}

YHub* YAPIContext::getYHubObj(int hubref)
{
    YHub* obj = NULL;
    int taken = 0;
    if (YAPI::_apiInitialized) {
        yEnterCriticalSection(&YAPI::_global_cs);
        taken = 1;
    }try {
        obj = this->_findYHubFromCache(hubref);
        if (obj == NULL) {
            obj = new YHub(this,hubref);
            this->_addYHubToCache(hubref, obj);
        }
    } catch (std::exception &) {
        if (taken) yLeaveCriticalSection(&YAPI::_global_cs);
        throw;
    }
    if (taken) yLeaveCriticalSection(&YAPI::_global_cs);
    return obj;
}
//--- (end of generated code: YAPIContext implementation)

//std::map<int, YHub*> YAPIContext::_yhub_cache;

YHub* YAPIContext::_findYHubFromCache(int ref)
{
    if (_yhub_cache.find(ref) != _yhub_cache.end())
        return _yhub_cache[ref];
    return NULL;
}

void YAPIContext::_addYHubToCache(int ref, YHub *obj)
{
    _yhub_cache[ref] = obj;
}

void YAPIContext::_ClearCache()
{
    for (std::map<int, YHub*>::iterator cache_iterator = _yhub_cache.begin();
         cache_iterator != _yhub_cache.end(); ++cache_iterator) {
        delete cache_iterator->second;
    }
    _yhub_cache.clear();
    _yhub_cache = std::map<int, YHub*>();
}


//--- (generated code: YAPIContext functions)
//--- (end of generated code: YAPIContext functions)


YHub::YHub(YAPIContext *ctx, int ref):
    //--- (generated code: YHub initialization)
    _ctx(NULL)
    ,_hubref(0)
    ,_userData(NULL)
//--- (end of generated code: YHub initialization)
{
    _ctx = ctx;
    _hubref = ref;
}

YHub::~YHub()
{
    //--- (generated code: YHub cleanup)
//--- (end of generated code: YHub cleanup)
}

//--- (generated code: YHub implementation)
// static attributes


string YHub::_getStrAttr(string attrName)
{
    char val[1024];
    int res = 0;
    int fullsize = 0;
    fullsize = 0;
    res = yapiGetHubStrAttr(_hubref, attrName.c_str(), val, 1024, &fullsize);
    if (res > 0) {
        return string(val, res);
    }
    return "";
}

int YHub::_getIntAttr(string attrName)
{
    return yapiGetHubIntAttr(_hubref, attrName.c_str());
}

void YHub::_setIntAttr(string attrName,int value)
{
    yapiSetHubIntAttr(_hubref, attrName.c_str(), value);
}

/**
 * Returns the URL that has been used first to register this hub.
 */
string YHub::get_registeredUrl(void)
{
    return this->_getStrAttr("registeredUrl");
}

/**
 * Returns all known URLs that have been used to register this hub.
 * URLs are pointing to the same hub when the devices connected
 * are sharing the same serial number.
 */
vector<string> YHub::get_knownUrls(void)
{
    char smallbuff[1024];
    char *bigbuff;
    int buffsize = 0;
    int fullsize = 0;
    int yapi_res = 0;
    string urls_packed;
    string known_url_val;
    vector<string> url_list;

    fullsize = 0;
    known_url_val = "knownUrls";
    yapi_res = yapiGetHubStrAttr(_hubref, known_url_val.c_str(), smallbuff, 1024, &fullsize);
    if (yapi_res < 0) {
        return url_list;
    }
    if (fullsize <= 1024) {
        urls_packed = string(smallbuff, yapi_res);
    } else {
        buffsize = fullsize;
        bigbuff = (char *)malloc(buffsize);
        yapi_res = yapiGetHubStrAttr(_hubref, known_url_val.c_str(), bigbuff, buffsize, &fullsize);
        if (yapi_res < 0) {
            free(bigbuff);
            return url_list;
        } else {
            urls_packed = string(bigbuff, yapi_res);
        }
        free(bigbuff);
    }
    if (!(urls_packed == "")) {
        url_list = _strsplit(urls_packed,'?');
    }
    return url_list;
}

/**
 * Returns the URL currently in use to communicate with this hub.
 */
string YHub::get_connectionUrl(void)
{
    return this->_getStrAttr("connectionUrl");
}

/**
 * Returns the hub serial number, if the hub was already connected once.
 */
string YHub::get_serialNumber(void)
{
    return this->_getStrAttr("serialNumber");
}

/**
 * Tells if this hub is still registered within the API.
 *
 * @return true if the hub has not been unregistered.
 */
bool YHub::isInUse(void)
{
    return this->_getIntAttr("isInUse") > 0;
}

/**
 * Tells if there is an active communication channel with this hub.
 *
 * @return true if the hub is currently connected.
 */
bool YHub::isOnline(void)
{
    return this->_getIntAttr("isOnline") > 0;
}

/**
 * Tells if write access on this hub is blocked. Return true if it
 * is not possible to change attributes on this hub
 *
 * @return true if it is not possible to change attributes on this hub.
 */
bool YHub::isReadOnly(void)
{
    return this->_getIntAttr("isReadOnly") > 0;
}

/**
 * Modifies tthe network connection delay for this hub.
 * The default value is inherited from ySetNetworkTimeout
 * at the time when the hub is registered, but it can be updated
 * afterward for each specific hub if necessary.
 *
 * @param networkMsTimeout : the network connection delay in milliseconds.
 * @noreturn
 */
void YHub::set_networkTimeout(int networkMsTimeout)
{
    this->_setIntAttr("networkTimeout",networkMsTimeout);
}

/**
 * Returns the network connection delay for this hub.
 * The default value is inherited from ySetNetworkTimeout
 * at the time when the hub is registered, but it can be updated
 * afterward for each specific hub if necessary.
 *
 * @return the network connection delay in milliseconds.
 */
int YHub::get_networkTimeout(void)
{
    return this->_getIntAttr("networkTimeout");
}

/**
 * Returns the numerical error code of the latest error with the hub.
 * This method is mostly useful when using the Yoctopuce library with
 * exceptions disabled.
 *
 * @return a number corresponding to the code of the latest error that occurred while
 *         using the hub object
 */
int YHub::get_errorType(void)
{
    return this->_getIntAttr("errorType");
}

/**
 * Returns the error message of the latest error with the hub.
 * This method is mostly useful when using the Yoctopuce library with
 * exceptions disabled.
 *
 * @return a string corresponding to the latest error message that occured while
 *         using the hub object
 */
string YHub::get_errorMessage(void)
{
    return this->_getStrAttr("errorMessage");
}

/**
 * Returns the value of the userData attribute, as previously stored
 * using method set_userData.
 * This attribute is never touched directly by the API, and is at
 * disposal of the caller to store a context.
 *
 * @return the object stored previously by the caller.
 */
void* YHub::get_userData(void)
{
    return _userData;
}

/**
 * Stores a user context provided as argument in the userData
 * attribute of the function.
 * This attribute is never touched by the API, and is at
 * disposal of the caller to store a context.
 *
 * @param data : any kind of object to be stored
 * @noreturn
 */
void YHub::set_userData(void* data)
{
    _userData = data;
}

/**
 * Starts the enumeration of hubs currently in use by the API.
 * Use the method YHub::nextHubInUse() to iterate on the
 * next hubs.
 *
 * @return a pointer to a YHub object, corresponding to
 *         the first hub currently in use by the API, or a
 *         NULL pointer if none has been registered.
 */
YHub* YHub::FirstHubInUse(void)
{
    return YAPI::nextHubInUseInternal(-1);
}

/**
 * Continues the module enumeration started using YHub::FirstHubInUse().
 * Caution: You can't make any assumption about the order of returned hubs.
 *
 * @return a pointer to a YHub object, corresponding to
 *         the next hub currenlty in use, or a NULL pointer
 *         if there are no more hubs to enumerate.
 */
YHub* YHub::nextHubInUse(void)
{
    return _ctx->nextHubInUseInternal(_hubref);
}
//--- (end of generated code: YHub implementation)

//--- (generated code: YHub functions)
//--- (end of generated code: YHub functions)


std::map<string, YFunction*> YFunction::_cache;


// Constructor is protected. Use the device-specific factory function to instantiate
YFunction::YFunction(const string &func):
    _className("Function"), _func(func),
    _lastErrorType(YAPI_SUCCESS), _lastErrorMsg(""),
    _fundescr(Y_FUNCTIONDESCRIPTOR_INVALID), _userData(NULL)

    //--- (generated code: YFunction initialization)
    ,_logicalName(LOGICALNAME_INVALID)
    ,_advertisedValue(ADVERTISEDVALUE_INVALID)
    ,_valueCallbackFunction(NULL)
    ,_cacheExpiration(0)
//--- (end of generated code: YFunction initialization)
{
    yInitializeCriticalSection(&_this_cs);
}

YFunction::~YFunction()
{
    //--- (generated code: YFunction cleanup)
//--- (end of generated code: YFunction cleanup)
    _clearDataStreamCache();
    yDeleteCriticalSection(&_this_cs);
}


// function cache methods
YFunction* YFunction::_FindFromCache(const string &classname, const string &func)
{
    if (_cache.find(classname + "_" + func) != _cache.end())
        return _cache[classname + "_" + func];
    return NULL;
}

void YFunction::_AddToCache(const string &classname, const string &func, YFunction *obj)
{
    _cache[classname + "_" + func] = obj;
}

void YFunction::_ClearCache()
{
    for (std::map<string, YFunction*>::iterator cache_iterator = _cache.begin();
         cache_iterator != _cache.end(); ++cache_iterator) {
        delete cache_iterator->second;
    }
    _cache.clear();
    _cache = std::map<string, YFunction*>();
}


//--- (generated code: YFunction implementation)
// static attributes
const string YFunction::LOGICALNAME_INVALID = YAPI_INVALID_STRING;
const string YFunction::ADVERTISEDVALUE_INVALID = YAPI_INVALID_STRING;

int YFunction::_parseAttr(YJSONObject *json_val)
{
    if(json_val->has("logicalName")) {
        _logicalName =  json_val->getString("logicalName");
    }
    if(json_val->has("advertisedValue")) {
        _advertisedValue =  json_val->getString("advertisedValue");
    }
    return 0;
}


/**
 * Returns the logical name of the function.
 *
 * @return a string corresponding to the logical name of the function
 *
 * On failure, throws an exception or returns YFunction::LOGICALNAME_INVALID.
 */
string YFunction::get_logicalName(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YFunction::LOGICALNAME_INVALID;
                }
            }
        }
        res = _logicalName;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the logical name of the function. You can use yCheckLogicalName()
 * prior to this call to make sure that your parameter is valid.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : a string corresponding to the logical name of the function
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YFunction::set_logicalName(const string& newval)
{
    string rest_val;
    int res;
    if (!YAPI::CheckLogicalName(newval)) {
        _throw(YAPI_INVALID_ARGUMENT, "Invalid name :" + newval);
        return YAPI_INVALID_ARGUMENT;
    }
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = newval;
        res = _setAttr("logicalName", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns a short string representing the current state of the function.
 *
 * @return a string corresponding to a short string representing the current state of the function
 *
 * On failure, throws an exception or returns YFunction::ADVERTISEDVALUE_INVALID.
 */
string YFunction::get_advertisedValue(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YFunction::ADVERTISEDVALUE_INVALID;
                }
            }
        }
        res = _advertisedValue;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

int YFunction::set_advertisedValue(const string& newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = newval;
        res = _setAttr("advertisedValue", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Retrieves a function for a given identifier.
 * The identifier can be specified using several formats:
 *
 * - FunctionLogicalName
 * - ModuleSerialNumber.FunctionIdentifier
 * - ModuleSerialNumber.FunctionLogicalName
 * - ModuleLogicalName.FunctionIdentifier
 * - ModuleLogicalName.FunctionLogicalName
 *
 *
 * This function does not require that the function is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the function is
 * indeed online at a given time. In case of ambiguity when looking for
 * a function by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the function, for instance
 *         MyDevice..
 *
 * @return a YFunction object allowing you to drive the function.
 */
YFunction* YFunction::FindFunction(string func)
{
    YFunction* obj = NULL;
    int taken = 0;
    if (YAPI::_apiInitialized) {
        yEnterCriticalSection(&YAPI::_global_cs);
        taken = 1;
    }try {
        obj = (YFunction*) YFunction::_FindFromCache("Function", func);
        if (obj == NULL) {
            obj = new YFunction(func);
            YFunction::_AddToCache("Function", func, obj);
        }
    } catch (std::exception &) {
        if (taken) yLeaveCriticalSection(&YAPI::_global_cs);
        throw;
    }
    if (taken) yLeaveCriticalSection(&YAPI::_global_cs);
    return obj;
}

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
int YFunction::registerValueCallback(YFunctionValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackFunction = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YFunction::_invokeValueCallback(string value)
{
    if (_valueCallbackFunction != NULL) {
        _valueCallbackFunction(this, value);
    } else {
    }
    return 0;
}

/**
 * Disables the propagation of every new advertised value to the parent hub.
 * You can use this function to save bandwidth and CPU on computers with limited
 * resources, or to prevent unwanted invocations of the HTTP callback.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @return YAPI::SUCCESS when the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YFunction::muteValueCallbacks(void)
{
    return this->set_advertisedValue("SILENT");
}

/**
 * Re-enables the propagation of every new advertised value to the parent hub.
 * This function reverts the effect of a previous call to muteValueCallbacks().
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @return YAPI::SUCCESS when the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YFunction::unmuteValueCallbacks(void)
{
    return this->set_advertisedValue("");
}

/**
 * Returns the current value of a single function attribute, as a text string, as quickly as
 * possible but without using the cached value.
 *
 * @param attrName : the name of the requested attribute
 *
 * @return a string with the value of the the attribute
 *
 * On failure, throws an exception or returns an empty string.
 */
string YFunction::loadAttribute(string attrName)
{
    string url;
    string attrVal;
    url = YapiWrapper::ysprintf("api/%s/%s",this->get_functionId().c_str(),attrName.c_str());
    attrVal = this->_download(url);
    return attrVal;
}

/**
 * Indicates whether changes to the function are prohibited or allowed.
 * Returns true if the function is blocked by an admin password
 * or if the function is not available.
 *
 * @return true if the function is write-protected or not online.
 */
bool YFunction::isReadOnly(void)
{
    string serial;
    char errmsg[YOCTO_ERRMSG_LEN];
    int res = 0;
    try {
        serial = this->get_serialNumber();
    } catch (std::exception&) {
        return true;
    }
    res = yapiIsModuleWritable(serial.c_str(), errmsg);
    if (res > 0) {
        return false;
    }
    return true;
}

/**
 * Returns the serial number of the module, as set by the factory.
 *
 * @return a string corresponding to the serial number of the module, as set by the factory.
 *
 * On failure, throws an exception or returns YFunction::SERIALNUMBER_INVALID.
 */
string YFunction::get_serialNumber(void)
{
    YModule* m = NULL;
    m = this->get_module();
    return m->get_serialNumber();
}

int YFunction::_parserHelper(void)
{
    return 0;
}

YFunction *YFunction::nextFunction(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YFunction::FindFunction(hwid);
}

YFunction *YFunction::FirstFunction(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("Function", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YFunction::FindFunction(serial+"."+funcId);
}

//--- (end of generated code: YFunction implementation)

//--- (generated code: YFunction functions)
//--- (end of generated code: YFunction functions)

void YFunction::_throw(YRETCODE errType, string errMsg)
{
    _lastErrorType = errType;
    _lastErrorMsg = errMsg;
    // Method used to throw exceptions or save error type/message
    if (!YAPI::ExceptionsDisabled) {
        throw YAPI_Exception(errType, errMsg);
    }
}

// Method used to resolve our name to our unique function descriptor (may trigger a hub scan)
YRETCODE YFunction::_getDescriptor(YFUN_DESCR &fundescr, string &errmsg)
{
    int res;
    YFUN_DESCR tmp_fundescr;

    tmp_fundescr = YapiWrapper::getFunction(_className, _func, errmsg);
    if (YISERR(tmp_fundescr)) {
        res = YapiWrapper::updateDeviceList(true, errmsg);
        if (YISERR(res)) {
            return (YRETCODE)res;
        }
        tmp_fundescr = YapiWrapper::getFunction(_className, _func, errmsg);
        if (YISERR(tmp_fundescr)) {
            return (YRETCODE)tmp_fundescr;
        }
    }
    _fundescr = fundescr = tmp_fundescr;
    return YAPI_SUCCESS;
}

// Return a pointer to our device caching object (may trigger a hub scan)
YRETCODE YFunction::_getDevice(YDevice *&dev, string &errmsg)
{
    YFUN_DESCR fundescr;
    YDEV_DESCR devdescr;
    YRETCODE res;

    // Resolve function name
    res = _getDescriptor(fundescr, errmsg);
    if (YISERR(res)) return res;

    // Get device descriptor
    devdescr = YapiWrapper::getDeviceByFunction(fundescr, errmsg);
    if (YISERR(devdescr)) return (YRETCODE)devdescr;

    // Get device object
    dev = YDevice::getDevice(devdescr);

    return YAPI_SUCCESS;
}

// Return the next known function of current class listed in the yellow pages
YRETCODE YFunction::_nextFunction(string &hwid)
{
    vector<YFUN_DESCR> v_fundescr;
    YFUN_DESCR fundescr;
    YDEV_DESCR devdescr;
    string serial, funcId, funcName, funcVal, errmsg;
    int res;

    res = _getDescriptor(fundescr, errmsg);
    if (YISERR(res)) {
        _throw((YRETCODE)res, errmsg);
        return (YRETCODE)res;
    }
    res = YapiWrapper::getFunctionsByClass(_className, fundescr, v_fundescr, sizeof(YFUN_DESCR), errmsg);
    if (YISERR((YRETCODE)res)) {
        _throw((YRETCODE)res, errmsg);
        return (YRETCODE)res;
    }
    if (v_fundescr.size() == 0) {
        hwid = "";
        return YAPI_SUCCESS;
    }
    res = YapiWrapper::getFunctionInfo(v_fundescr[0], devdescr, serial, funcId, funcName, funcVal, errmsg);
    if (YISERR(res)) {
        _throw((YRETCODE)res, errmsg);
        return (YRETCODE)res;
    }
    hwid = serial + "." + funcId;

    return YAPI_SUCCESS;
}

// Parse a long JSON string
string YFunction::_parseString(yJsonStateMachine &j)
{
    string res;

    do {
#ifdef WINDOWS_API
        res += j.token;
#else
        char buffer[128];
        char *pt, *s;
        s = j.token;
        pt = buffer;
        while (*s) {
            unsigned char c = *s++;
            if (c < 128) {
                *pt++ = c;
            } else {
                // UTF8-encode character
                *pt++ = 0xc2 + (c>0xbf ? 1 : 0);
                *pt++ = (c & 0x3f) + 0x80;
            }
        }
        *pt = 0;
        res += buffer;
#endif
    } while (j.next == YJSON_PARSE_STRINGCONT && yJsonParse(&j) == YJSON_PARSE_AVAIL);

    return res;
}

string YFunction::_json_get_key(const string &json, const string &key)
{
    yJsonStateMachine j;

    // Parse JSON data for the device and locate our function in it
    j.src = json.c_str();
    j.end = j.src + strlen(j.src);
    j.st = YJSON_START;
    if (yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_STRUCT) {
        this->_throw(YAPI_IO_ERROR, "JSON structure expected");
        return YAPI_INVALID_STRING;
    }
    while (yJsonParse(&j) == YJSON_PARSE_AVAIL && j.st == YJSON_PARSE_MEMBNAME) {
        if (!strcmp(j.token, key.c_str())) {
            if (yJsonParse(&j) != YJSON_PARSE_AVAIL) {
                this->_throw(YAPI_IO_ERROR, "JSON structure expected");
                return YAPI_INVALID_STRING;
            }
            return _parseString(j);
        }
        yJsonSkip(&j, 1);
    }
    this->_throw(YAPI_IO_ERROR, "invalid JSON structure");
    return YAPI_INVALID_STRING;
}

string YFunction::_json_get_string(const string &json)
{
    yJsonStateMachine j;
    j.src = json.c_str();
    j.end = j.src + strlen(j.src);
    j.st = YJSON_START;
    if (yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_STRING) {
        this->_throw(YAPI_IO_ERROR, "JSON string expected");
        return "";
    }
    return _parseString(j);
}

vector<string> YFunction::_json_get_array(const string &json)
{
    vector<string> res;
    yJsonStateMachine j;
    const char *json_cstr, *last;
    string backup = json;
    j.src = json_cstr = json.c_str();
    j.end = j.src + strlen(j.src);
    j.st = YJSON_START;
    int parse_res = yJsonParse(&j);
    if (parse_res != YJSON_PARSE_AVAIL || j.st != YJSON_PARSE_ARRAY) {
        this->_throw(YAPI_IO_ERROR, "JSON array expected");
        return res;
    }
    int depth = j.depth;
    do {
        last = j.src;
        while ((parse_res = yJsonParse(&j)) == YJSON_PARSE_AVAIL) {
            if (j.next == YJSON_PARSE_STRINGCONT || j.depth > depth) {
                continue;
            }
            break;
        }
        if (j.st == YJSON_PARSE_ERROR) {
            this->_throw(YAPI_IO_ERROR, "invalid JSON array");
            return res;
        }

        if (j.depth == depth) {
            long location, length;
            while (*last == ',' || *last == '\n' || *last == '\r') {
                last++;
            }
            location = (long)(last - json_cstr);
            length = (long)(j.src - last);
            string item = json.substr(location, length);
            res.push_back(item);
        }
    } while (parse_res == YJSON_PARSE_AVAIL && j.st != YJSON_PARSE_ARRAY);
    if (j.st != YJSON_PARSE_ARRAY) {
        this->_throw(YAPI_IO_ERROR, "missing closing tag in JSON array");
        return res;
    }
    return res;
}

string YFunction::_get_json_path(const string &json, const string &path)
{
    const char *json_data = json.c_str();
    int len = (int)(json.length() & 0x0fffffff);
    const char *p;
    char errbuff[YOCTO_ERRMSG_LEN];
    int res;

    res = yapiJsonGetPath(path.c_str(), json_data, len, &p, errbuff);
    if (res >= 0) {
        string result = string(p, res);
        if (res > 0) {
            yapiFreeMem((void*)p);
        }
        return result;
    }
    return "";
}

string YFunction::_decode_json_string(const string &json)
{
    int len = (int)(json.length() & 0x0fffffff);
    char buffer[128];
    char *p = buffer;
    int decoded_len;
    if (len == 0) {
        return "";
    }
    if (len >= 127) {
        p = (char*)malloc(len + 1);

    }
    decoded_len = yapiJsonDecodeString(json.c_str(), p);
    string result = string(p, decoded_len);
    if (len >= 127) {
        free(p);
    }
    return result;
}

int YFunction::_decode_json_int(const string &json)
{
    YJSONNumber obj = new YJSONNumber(json, 0, (int)json.length());
    obj.parse();
    return obj.getInt();
}

static string __escapeAttr(const string &changeval)
{
    const char *p;
    unsigned char c;
    unsigned char esc[3];
    string escaped = "";
    esc[0] = '%';
    for (p = changeval.c_str(); (c = *p) != 0; p++) {
        if (c <= ' ' || (c > 'z' && c != '~') || c == '"' || c == '%' || c == '&' ||
            c == '+' || c == '<' || c == '=' || c == '>' || c == '\\' || c == '^' || c == '`') {
            if ((c == 0xc2 || c == 0xc3) && (p[1] & 0xc0) == 0x80) {
                // UTF8-encoded ISO-8859-1 character: translate to plain ISO-8859-1
                c = (c & 1) * 0x40;
                p++;
                c += *p;
            }
            esc[1] = (c >= 0xa0 ? (c >> 4) - 10 + 'A' : (c >> 4) + '0');
            c &= 0xf;
            esc[2] = (c >= 0xa ? c - 10 + 'A' : c + '0');
            escaped.append((char*)esc, 3);
        } else {
            escaped.append((char*)&c, 1);
        }
    }
    return escaped;
}


string YFunction::_escapeAttr(const string &changeval)
{
    return __escapeAttr(changeval);
}


YRETCODE YFunction::_buildSetRequest(const string &changeattr, const string *changeval, string &request, string &errmsg)
{
    int res;
    YFUN_DESCR fundesc;
    char funcid[YOCTO_FUNCTION_LEN];
    char errbuff[YOCTO_ERRMSG_LEN];


    // Resolve the function name
    res = _getDescriptor(fundesc, errmsg);
    if (YISERR(res)) {
        return (YRETCODE)res;
    }

    if (YISERR(res=yapiGetFunctionInfo(fundesc, NULL, NULL, funcid, NULL, NULL,errbuff))) {
        errmsg = errbuff;
        _throw((YRETCODE)res, errmsg);
        return (YRETCODE)res;
    }
    request = "GET /api/";
    request.append(funcid);
    request.append("/");
    //request.append(".json/");

    if (changeattr != "") {
        request.append(changeattr);
        if (changeval) {
            request.append("?");
            request.append(changeattr);
            request.append("=");
            request.append(_escapeAttr(*changeval));
        }
    }
    // don't append HTTP/1.1 so that we get light headers from hub
    // but append &. suffix to enable connection keepalive on newer hubs
    request.append("&. \r\n\r\n");
    return YAPI_SUCCESS;
}


int YFunction::_parse(YJSONObject *j)
{
    this->_parseAttr(j);
    this->_parserHelper();
    return 0;
}

// Set an attribute in the function, and parse the resulting new function state
YRETCODE YFunction::_setAttr(string attrname, string newvalue)
{
    string errmsg, request;
    int res;
    YDevice *dev;

    // Execute http request
    res = _buildSetRequest(attrname, &newvalue, request, errmsg);
    if (YISERR(res)) {
        _throw((YRETCODE)res, errmsg);
        return (YRETCODE)res;
    }
    // Get device Object
    res = _getDevice(dev, errmsg);
    if (YISERR(res)) {
        _throw((YRETCODE)res, errmsg);
        return (YRETCODE)res;
    }

    res = dev->HTTPRequestAsync(0, request, NULL, NULL, errmsg);
    if (YISERR(res)) {
        // Check if an update of the device list does not solve the issue
        res = YapiWrapper::updateDeviceList(true, errmsg);
        if (YISERR(res)) {
            _throw((YRETCODE)res, errmsg);
            return (YRETCODE)res;
        }
        res = dev->HTTPRequestAsync(0, request, NULL, NULL, errmsg);
        if (YISERR(res)) {
            _throw((YRETCODE)res, errmsg);
            return (YRETCODE)res;
        }
    }
    if (_cacheExpiration != 0) {
        _cacheExpiration = YAPI::GetTickCount();
    }
    return YAPI_SUCCESS;
}


// Method used to send http request to the device (not the function)
string YFunction::_requestEx(int channel, const string &request, yapiRequestProgressCallback callback, void *context)
{
    YDevice *dev;
    string errmsg, buffer;
    int res;


    // Resolve our reference to our device, load REST API
    res = _getDevice(dev, errmsg);
    if (YISERR(res)) {
        _throw((YRETCODE)res, errmsg);
        return YAPI_INVALID_STRING;
    }
    res = dev->HTTPRequest(channel, request, buffer, callback, context, errmsg);
    if (YISERR(res)) {
        // Check if an update of the device list does notb solve the issue
        res = YapiWrapper::updateDeviceList(true, errmsg);
        if (YISERR(res)) {
            this->_throw((YRETCODE)res, errmsg);
            return YAPI_INVALID_STRING;
        }
        res = dev->HTTPRequest(channel, request, buffer, callback, context, errmsg);
        if (YISERR(res)) {
            this->_throw((YRETCODE)res, errmsg);
            return YAPI_INVALID_STRING;
        }
    }
    if (0 != buffer.find("OK\r\n")) {
        if (0 != buffer.find("HTTP/1.1 200 OK\r\n")) {
            this->_throw(YAPI_IO_ERROR, "http request failed");
            return YAPI_INVALID_STRING;
        }
    }
    return buffer;
}

string YFunction::_request(const string &request)
{
    return _requestEx(0, request, NULL, NULL);
}


// Method used to send http request to the device (not the function)
string YFunction::_download(const string &url)
{
    string request, buffer;
    size_t found;

    request = "GET /" + url + " HTTP/1.1\r\n\r\n";
    buffer = this->_request(request);
    found = buffer.find("\r\n\r\n");
    if (string::npos == found) {
        this->_throw(YAPI_IO_ERROR, "http request failed");
        return YAPI_INVALID_STRING;
    }
    return buffer.substr(found + 4);
}


// Method used to upload a file to the device
string YFunction::_uploadWithProgressEx(const string &path, const string &content, yapiRequestProgressCallback callback, void *context)
{
    string request, buffer;
    string boundary;
    size_t found;

    request = "POST /upload.html HTTP/1.1\r\n";
    string body = "Content-Disposition: form-data; name=\"" + path + "\"; filename=\"api\"\r\n" +
        "Content-Type: application/octet-stream\r\n" +
        "Content-Transfer-Encoding: binary\r\n\r\n" + content;
    do {
        boundary = YapiWrapper::ysprintf("Zz%06xzZ", rand() & 0xffffff);
    } while (body.find(boundary) != string::npos);
    request += "Content-Type: multipart/form-data; boundary=" + boundary + "\r\n";
    request += "\r\n--" + boundary + "\r\n" + body + "\r\n--" + boundary + "--\r\n";
    buffer = this->_requestEx(0, request, callback, context);
    found = buffer.find("\r\n\r\n");
    if (string::npos == found) {
        this->_throw(YAPI_IO_ERROR, "http request failed");
        return YAPI_INVALID_STRING;
    }
    return buffer.substr(found + 4);;
}

YRETCODE YFunction::_uploadWithProgress(const string &path, const string &content, yapiRequestProgressCallback callback, void *context)
{
    string res = this->_uploadWithProgressEx(path, content, NULL, NULL);
    if (res == YAPI_INVALID_STRING) {
        return YAPI_IO_ERROR;
    }
    return YAPI_SUCCESS;
}

// Method used to upload a file to the device
YRETCODE YFunction::_upload(const string &path, const string &content)
{
    return this->_uploadWithProgress(path, content, NULL, NULL);
}


// Method used to upload a file to the device
string YFunction::_uploadEx(const string &path, const string &content)
{
    return this->_uploadWithProgressEx(path, content, NULL, NULL);
}


// Method used to cache DataStream objects (new DataLogger)
YDataStream* YFunction::_findDataStream(YDataSet &dataset, const string &def)
{
    string key = dataset.get_functionId() + ":" + def;
    if (_dataStreams.find(key) != _dataStreams.end())
        return _dataStreams[key];

    vector<int> words = YAPI::_decodeWords(def);
    if (words.size() < 14) {
        _throw(YAPI_VERSION_MISMATCH, "device firmware is too old");
    }
    YDataStream *newDataStream = new YDataStream(this, dataset, words);
    _dataStreams[key] = newDataStream;
    return newDataStream;
}

// Method used to clear cache of DataStream object (undocumented)
void YFunction::_clearDataStreamCache()
{
    std::map<string, YDataStream*>::iterator it;
    for (it = _dataStreams.begin(); it != _dataStreams.end(); ++it) {
        YDataStream *ds = it->second;

        delete(ds);
    }
    _dataStreams.clear();
}


// Return a string that describes the function (class and logical name or hardware id)
string YFunction::describe(void)
{
    YFUN_DESCR fundescr;
    YDEV_DESCR devdescr;
    string errmsg, serial, funcId, funcName, funcValue;
    string descr = _func;
    string res;

    yEnterCriticalSection(&_this_cs);
    fundescr = YapiWrapper::getFunction(_className, _func, errmsg);
    if (!YISERR(fundescr) && !YISERR(YapiWrapper::getFunctionInfo(fundescr, devdescr, serial, funcId, funcName, funcValue, errmsg))) {
        res = _className + "(" + _func + ")=" + serial + "." + funcId;
    } else {
        res = _className + "(" + _func + ")=unresolved";
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

// Return a string that describes the function (class and logical name or hardware id)
string YFunction::get_friendlyName(void)
{
    YFUN_DESCR fundescr, moddescr;
    YDEV_DESCR devdescr;
    YRETCODE retcode;
    string errmsg, serial, funcId, funcName, funcValue;
    string mod_serial, mod_funcId, mod_funcname;

    yEnterCriticalSection(&_this_cs);
    // Resolve the function name
    retcode = _getDescriptor(fundescr, errmsg);
    if (!YISERR(retcode) && !YISERR(YapiWrapper::getFunctionInfo(fundescr, devdescr, serial, funcId, funcName, funcValue, errmsg))) {
        if (funcName != "") {
            funcId = funcName;
        }

        moddescr = YapiWrapper::getFunction("Module", serial, errmsg);
        if (!YISERR(moddescr) && !YISERR(YapiWrapper::getFunctionInfo(moddescr, devdescr, mod_serial, mod_funcId, mod_funcname, funcValue, errmsg))) {
            if (mod_funcname != "") {
                yLeaveCriticalSection(&_this_cs);
                return mod_funcname + "." + funcId;
            }
        }
        yLeaveCriticalSection(&_this_cs);
        return serial + "." + funcId;
    }
    yLeaveCriticalSection(&_this_cs);
    _throw((YRETCODE)YAPI::DEVICE_NOT_FOUND, errmsg);
    return Y_FRIENDLYNAME_INVALID;
}


/**
 * Returns the unique hardware identifier of the function in the form SERIAL.FUNCTIONID.
 * The unique hardware identifier is composed of the device serial
 * number and of the hardware identifier of the function (for example RELAYLO1-123456.relay1).
 *
 * @return a string that uniquely identifies the function (ex: RELAYLO1-123456.relay1)
 *
 * On failure, throws an exception or returns  YFunction::HARDWAREID_INVALID.
 */
string YFunction::get_hardwareId(void)
{
    YRETCODE retcode;
    YFUN_DESCR fundesc;
    string errmsg;
    char snum[YOCTO_SERIAL_LEN];
    char funcid[YOCTO_FUNCTION_LEN];
    char errbuff[YOCTO_ERRMSG_LEN];

    yEnterCriticalSection(&_this_cs);
    // Resolve the function name
    retcode = _getDescriptor(fundesc, errmsg);
    if (YISERR(retcode)) {
        yLeaveCriticalSection(&_this_cs);
        _throw(retcode, errmsg);
        return HARDWAREID_INVALID;
    }
    if (YISERR(retcode=yapiGetFunctionInfo(fundesc, NULL, snum, funcid, NULL, NULL,errbuff))) {
        errmsg = errbuff;
        yLeaveCriticalSection(&_this_cs);
        _throw(retcode, errmsg);
        return HARDWAREID_INVALID;
    }
    yLeaveCriticalSection(&_this_cs);
    return string(snum) + string(".") + string(funcid);
}

/**
 * Returns the hardware identifier of the function, without reference to the module. For example
 * relay1
 *
 * @return a string that identifies the function (ex: relay1)
 *
 * On failure, throws an exception or returns  YFunction::FUNCTIONID_INVALID.
 */
string YFunction::get_functionId(void)
{
    YRETCODE retcode;
    YFUN_DESCR fundesc;
    string errmsg;
    char funcid[YOCTO_FUNCTION_LEN];
    char errbuff[YOCTO_ERRMSG_LEN];

    yEnterCriticalSection(&_this_cs);
    // Resolve the function name
    retcode = _getDescriptor(fundesc, errmsg);
    if (YISERR(retcode)) {
        yLeaveCriticalSection(&_this_cs);
        _throw(retcode, errmsg);
        return HARDWAREID_INVALID;
    }
    if (YISERR(retcode=yapiGetFunctionInfo(fundesc, NULL, NULL, funcid, NULL, NULL,errbuff))) {
        errmsg = errbuff;
        yLeaveCriticalSection(&_this_cs);
        _throw(retcode, errmsg);
        return HARDWAREID_INVALID;
    }
    yLeaveCriticalSection(&_this_cs);
    return string(funcid);
}


/**
 * Returns the numerical error code of the latest error with the function.
 * This method is mostly useful when using the Yoctopuce library with
 * exceptions disabled.
 *
 * @return a number corresponding to the code of the latest error that occurred while
 *         using the function object
 */
YRETCODE YFunction::get_errorType(void)
{
    return _lastErrorType;
}

/**
 * Returns the error message of the latest error with the function.
 * This method is mostly useful when using the Yoctopuce library with
 * exceptions disabled.
 *
 * @return a string corresponding to the latest error message that occured while
 *         using the function object
 */
string YFunction::get_errorMessage(void)
{
    return _lastErrorMsg;
}

/**
 * Checks if the function is currently reachable, without raising any error.
 * If there is a cached value for the function in cache, that has not yet
 * expired, the device is considered reachable.
 * No exception is raised if there is an error while trying to contact the
 * device hosting the function.
 *
 * @return true if the function can be reached, and false otherwise
 */
bool YFunction::isOnline(void)
{
    YDevice *dev;
    string errmsg;
    YJSONObject *apires;

    yEnterCriticalSection(&_this_cs);
    try {
        // A valid value in cache means that the device is online
        if (_cacheExpiration > yapiGetTickCount()) {
            yLeaveCriticalSection(&_this_cs);
            return true;
        }

        // Check that the function is available, without throwing exceptions
        if (YISERR(_getDevice(dev, errmsg))) {
            yLeaveCriticalSection(&_this_cs);
            return false;
        }

        // Try to execute a function request to be positively sure that the device is ready
        if (YISERR(dev->requestAPI(apires, errmsg))) {
            yLeaveCriticalSection(&_this_cs);
            return false;
        }
        delete apires;
        // Preload the function data, since we have it in device cache
        this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity());
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        return false;
    }
    yLeaveCriticalSection(&_this_cs);
    return true;
}

YRETCODE YFunction::_load_unsafe(u64 msValidity)
{
    YJSONObject *j, *node;
    YDevice *dev;
    string errmsg;
    YFUN_DESCR fundescr;
    int res;
    char errbuf[YOCTO_ERRMSG_LEN];
    char serial[YOCTO_SERIAL_LEN];
    char funcId[YOCTO_FUNCTION_LEN];

    // Resolve our reference to our device, load REST API
    res = _getDevice(dev, errmsg);
    if (YISERR(res)) {
        _throw((YRETCODE)res, errmsg);
        return (YRETCODE)res;
    }
    res = dev->requestAPI(j, errmsg);
    if (YISERR(res)) {
        _throw((YRETCODE)res, errmsg);
        return (YRETCODE)res;
    }

    // Get our function Id
    fundescr = YapiWrapper::getFunction(_className, _func, errmsg);
    if (YISERR(fundescr)) {
        delete j;
        _throw((YRETCODE)fundescr, errmsg);
        return (YRETCODE)fundescr;
    }
    res = yapiGetFunctionInfo(fundescr, NULL, serial, funcId, NULL, NULL, errbuf);
    if (YISERR(res)) {
        delete j;
        _throw((YRETCODE)res, errbuf);
        return (YRETCODE)res;
    }
    _cacheExpiration = yapiGetTickCount() + msValidity;
    _serial = serial;
    _funId = funcId;
    _hwId = _serial + '.' + _funId;

    try {
        node = j->getYJSONObject(funcId);
    } catch (std::exception &) {
        delete j;
        _throw(YAPI_IO_ERROR, "unexpected JSON structure: missing function " + _funId);
        return YAPI_IO_ERROR;
    }
    _parse(node);
    delete j;
    return YAPI_SUCCESS;
}


/**
 * Preloads the function cache with a specified validity duration.
 * By default, whenever accessing a device, all function attributes
 * are kept in cache for the standard duration (5 ms). This method can be
 * used to temporarily mark the cache as valid for a longer period, in order
 * to reduce network traffic for instance.
 *
 * @param msValidity : an integer corresponding to the validity attributed to the
 *         loaded function parameters, in milliseconds
 *
 * @return YAPI::SUCCESS when the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
YRETCODE YFunction::load(int msValidity)
{
    YRETCODE res;
    yEnterCriticalSection(&_this_cs);
    try {
        res = this->_load_unsafe(msValidity);
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}


/**
 * Invalidates the cache. Invalidates the cache of the function attributes. Forces the
 * next call to get_xxx() or loadxxx() to use values that come from the device.
 *
 * @noreturn
 */
void YFunction::clearCache()
{
    YDevice *dev;
    string errmsg, apires;


    yEnterCriticalSection(&_this_cs);
    // Resolve our reference to our device, load REST API
    int res = _getDevice(dev, errmsg);
    if (YISERR(res)) {
        yLeaveCriticalSection(&_this_cs);
        return;
    }
    dev->clearCache(false);
    if (_cacheExpiration) {
        _cacheExpiration = yapiGetTickCount();
    }
    yLeaveCriticalSection(&_this_cs);
}


/**
 * Gets the YModule object for the device on which the function is located.
 * If the function cannot be located on any module, the returned instance of
 * YModule is not shown as on-line.
 *
 * @return an instance of YModule
 */
YModule* YFunction::get_module(void)
{
    YFUN_DESCR fundescr;
    YDEV_DESCR devdescr;
    string errmsg, serial, funcId, funcName, funcValue;

    yEnterCriticalSection(&_this_cs);
    if (_serial != "") {
        serial = _serial;
        yLeaveCriticalSection(&_this_cs);
        return yFindModule(serial + ".module");
    }
    fundescr = YapiWrapper::getFunction(_className, _func, errmsg);
    if (!YISERR(fundescr)) {
        if (!YISERR(YapiWrapper::getFunctionInfo(fundescr, devdescr, serial, funcId, funcName, funcValue, errmsg))) {
            yLeaveCriticalSection(&_this_cs);
            return yFindModule(serial + ".module");
        }
    }
    // return a true YModule object even if it is not a module valid for communicating
    yLeaveCriticalSection(&_this_cs);
    return yFindModule(string("module_of_") + _className + "_" + _func);
}


/**
 * Returns the value of the userData attribute, as previously stored using method
 * set_userData.
 * This attribute is never touched directly by the API, and is at disposal of the caller to
 * store a context.
 *
 * @return the object stored previously by the caller.
 */
void* YFunction::get_userData(void)
{
    void *res;
    yEnterCriticalSection(&_this_cs);
    res = _userData;
    yLeaveCriticalSection(&_this_cs);
    return res;
}


/**
 * Stores a user context provided as argument in the userData attribute of the function.
 * This attribute is never touched by the API, and is at disposal of the caller to store a context.
 *
 * @param data : any kind of object to be stored
 * @noreturn
 */
void YFunction::set_userData(void *data)
{
    yEnterCriticalSection(&_this_cs);
    _userData = data;
    yLeaveCriticalSection(&_this_cs);
}


/**
 * Returns a unique identifier of type YFUN_DESCR corresponding to the function.
 * This identifier can be used to test if two instances of YFunction reference the same
 * physical function on the same physical device.
 *
 * @return an identifier of type YFUN_DESCR.
 *
 * If the function has never been contacted, the returned value is Y$CLASSNAME$.FUNCTIONDESCRIPTOR_INVALID.
 */
YFUN_DESCR YFunction::get_functionDescriptor(void)
{
    // do not take CS un purpose
    return _fundescr;
}

void YFunction::_UpdateValueCallbackList(YFunction *func, bool add)
{
    if (add) {
        func->isOnline();
        vector<YFunction*>::iterator it;
        for (it = _FunctionCallbacks.begin(); it < _FunctionCallbacks.end(); it++) {
            if (*it == func)
                return;
        }
        _FunctionCallbacks.push_back(func);
    } else {
        vector<YFunction*>::iterator it;
        for (it = _FunctionCallbacks.begin(); it < _FunctionCallbacks.end(); it++) {
            if (*it == func) {
                _FunctionCallbacks.erase(it);
                break;
            }
        }
    }
}


void YFunction::_UpdateTimedReportCallbackList(YFunction *func, bool add)
{
    if (add) {
        func->isOnline();
        vector<YFunction*>::iterator it;
        for (it = _TimedReportCallbackList.begin(); it < _TimedReportCallbackList.end(); it++) {
            if (*it == func)
                return;
        }
        _TimedReportCallbackList.push_back(func);
    } else {
        vector<YFunction*>::iterator it;
        for (it = _TimedReportCallbackList.begin(); it < _TimedReportCallbackList.end(); it++) {
            if (*it == func) {
                _TimedReportCallbackList.erase(it);
                break;
            }
        }
    }
}


void YModule::_updateModuleCallbackList(YModule *module, bool add)
{
    map<YModule*, int>::iterator entry = _moduleCallbackList.find(module);
    if (add) {
        module->isOnline();
        if (entry != _moduleCallbackList.end()) {
            _moduleCallbackList[module]++;
        } else {
            _moduleCallbackList[module] = 1;
        }
    } else {
        if (entry != _moduleCallbackList.end()) {
            if (_moduleCallbackList[module] > 0) {
                _moduleCallbackList[module]--;
            }
        }
    }
}


// This is the internal device cache object
vector<YDevice*> YDevice::_devCache;

YDevice::YDevice(YDEV_DESCR devdesc): _devdescr(devdesc), _cacheStamp(0), _cacheJson(NULL), _subpath(NULL)
{
    yInitializeCriticalSection(&_lock);
};


YDevice::~YDevice() // destructor
{
    clearCache(true);
    yDeleteCriticalSection(&_lock);
}

void YDevice::ClearCache()
{
    for (unsigned int idx = 0; idx < YDevice::_devCache.size(); idx++) {
        delete _devCache[idx];
    }
    _devCache.clear();
    _devCache = vector<YDevice*>();
}


YDevice* YDevice::getDevice(YDEV_DESCR devdescr)
{
    // Search in cache
    yEnterCriticalSection(&YAPI::_global_cs);
    for (unsigned int idx = 0; idx < YDevice::_devCache.size(); idx++) {
        if (YDevice::_devCache[idx]->_devdescr == devdescr) {
            yLeaveCriticalSection(&YAPI::_global_cs);
            return YDevice::_devCache[idx];
        }
    }

    // Not found, add new entry
    YDevice *dev = new YDevice(devdescr);
    YDevice::_devCache.push_back(dev);
    yLeaveCriticalSection(&YAPI::_global_cs);

    return dev;
}


YRETCODE YDevice::HTTPRequestPrepare(const string &request, string &fullrequest, char *errbuff)
{
    YRETCODE res;
    size_t pos;
    // mutex already taken by caller
    if (_subpath == NULL) {
        int neededsize;
        res = yapiGetDevicePath(_devdescr, _rootdevice, NULL, 0, &neededsize, errbuff);
        if (YISERR(res)) return res;
        _subpath = new char[neededsize];
        res = yapiGetDevicePath(_devdescr, _rootdevice, _subpath, neededsize, NULL, errbuff);
        if (YISERR(res)) return res;
    }
    pos = request.find_first_of('/');
    fullrequest = request.substr(0, pos) + (string)_subpath + request.substr(pos + 1);

    return YAPI_SUCCESS;
}


YRETCODE YDevice::HTTPRequest_unsafe(int channel, const string &request, string &buffer, yapiRequestProgressCallback callback, void *context, string &errmsg)
{
    char errbuff[YOCTO_ERRMSG_LEN] = "";
    YRETCODE res;
    YIOHDL iohdl;
    string fullrequest;
    char *reply;
    int replysize = 0;

    if (YISERR(res = HTTPRequestPrepare(request, fullrequest, errbuff))) {
        errmsg = (string)errbuff;
        return res;
    }
    if (YISERR(res = yapiHTTPRequestSyncStartOutOfBand(&iohdl, channel, _rootdevice, fullrequest.data(), (int)fullrequest.size(), &reply, &replysize, callback, context, errbuff))) {
        errmsg = (string)errbuff;
        return res;
    }
    if (replysize > 0 && reply != NULL) {
        buffer = string(reply, replysize);
    } else {
        buffer = "";
    }
    if (YISERR(res = yapiHTTPRequestSyncDone(&iohdl, errbuff))) {
        errmsg = (string)errbuff;
        return res;
    }

    return YAPI_SUCCESS;
}


YRETCODE YDevice::HTTPRequestAsync(int channel, const string &request, HTTPRequestCallback callback, void *context, string &errmsg)
{
    char errbuff[YOCTO_ERRMSG_LEN] = "";
    YRETCODE res = YAPI_SUCCESS;
    string fullrequest;
    yEnterCriticalSection(&_lock);
    _cacheStamp = YAPI::GetTickCount(); //invalidate cache
    if (YISERR(res=HTTPRequestPrepare(request, fullrequest, errbuff)) ||
        YISERR(res=yapiHTTPRequestAsyncOutOfBand(channel, _rootdevice, fullrequest.c_str(), (int)fullrequest.length(), NULL, NULL, errbuff))) {
        errmsg = (string)errbuff;
    }
    yLeaveCriticalSection(&_lock);
    return res;
}


YRETCODE YDevice::HTTPRequest(int channel, const string &request, string &buffer, yapiRequestProgressCallback callback, void *context, string &errmsg)
{
    YRETCODE res;
    int locked = 0;
    int i;

    for (i = 0; !locked && i < 5; i++) {
        locked = yTryEnterCriticalSection(&_lock);
        if (!locked) {
            yApproximateSleep(50);
        }
    }
    if (!locked) {
        yEnterCriticalSection(&_lock);
    }
    res = HTTPRequest_unsafe(channel, request, buffer, callback, context, errmsg);
    yLeaveCriticalSection(&_lock);
    return res;
}


YRETCODE YDevice::requestAPI(YJSONObject *&apires, string &errmsg)
{
    yJsonStateMachine j;
    string rootdev, buffer;
    string request = "GET /api.json \r\n\r\n";
    string json_str;
    int res;

    yEnterCriticalSection(&_lock);

    // Check if we have a valid cache value
    if (_cacheStamp > YAPI::GetTickCount()) {
        apires = new YJSONObject(_cacheJson);
        yLeaveCriticalSection(&_lock);
        return YAPI_SUCCESS;
    }
    apires = NULL;
    if (_cacheJson == NULL) {
        request = "GET /api.json \r\n\r\n";
    } else {
        string fw_release = _cacheJson->getYJSONObject("module")->getString("firmwareRelease");
        fw_release = __escapeAttr(fw_release);
        request = "GET /api.json?fw=" + fw_release + " \r\n\r\n";
    }
    // send request, without HTTP/1.1 suffix to get light headers
    res = this->HTTPRequest_unsafe(0, request, buffer, NULL, NULL, errmsg);
    if (YISERR(res)) {
        yLeaveCriticalSection(&_lock);
        // Check if an update of the device list does not solve the issue
        res = YapiWrapper::updateDeviceList(true, errmsg);
        if (YISERR(res)) {
            return (YRETCODE)res;
        }
        yEnterCriticalSection(&_lock);
        // send request, without HTTP/1.1 suffix to get light headers
        res = this->HTTPRequest_unsafe(0, request, buffer, NULL, NULL, errmsg);
        if (YISERR(res)) {
            yLeaveCriticalSection(&_lock);
            return (YRETCODE)res;
        }
    }

    // Parse HTTP header
    j.src = buffer.data();
    j.end = j.src + buffer.size();
    j.st = YJSON_HTTP_START;
    if (yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_HTTP_READ_CODE) {
        errmsg = "Failed to parse HTTP header";
        yLeaveCriticalSection(&_lock);
        if (!YAPI::ExceptionsDisabled) {
            throw YAPI_Exception(YAPI_IO_ERROR, errmsg);
        }
        return YAPI_IO_ERROR;
    }
    if (string(j.token) != "200") {
        errmsg = string("Unexpected HTTP return code: ") + j.token;
        yLeaveCriticalSection(&_lock);
        if (!YAPI::ExceptionsDisabled) {
            throw YAPI_Exception(YAPI_IO_ERROR, errmsg);
        }
        return YAPI_IO_ERROR;
    }
    if (yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_HTTP_READ_MSG) {
        errmsg = "Unexpected HTTP header format";
        yLeaveCriticalSection(&_lock);
        if (!YAPI::ExceptionsDisabled) {
            throw YAPI_Exception(YAPI_IO_ERROR, errmsg);
        }
        return YAPI_IO_ERROR;
    }
    if (yJsonParse(&j) != YJSON_PARSE_AVAIL || (j.st != YJSON_PARSE_STRUCT && j.st != YJSON_PARSE_ARRAY)) {
        errmsg = "Unexpected JSON reply format";
        yLeaveCriticalSection(&_lock);
        if (!YAPI::ExceptionsDisabled) {
            throw YAPI_Exception(YAPI_IO_ERROR, errmsg);
        }
        return YAPI_IO_ERROR;
    }
    // we know for sure that the last character parsed was a '{' or '['
    do j.src--; while (j.src[0] != '{' && j.src[0] != '[');
    json_str = string(j.src);
    try {
        apires = new YJSONObject(json_str, 0, (int)json_str.length());
        apires->parseWithRef(_cacheJson);
    } catch (std::exception &ex) {
        errmsg = "unexpected JSON structure: " + string(ex.what());
        if (_cacheJson) {
            delete _cacheJson;
            _cacheJson = NULL;
        }
        if (apires != NULL) {
            delete apires;
            apires = NULL;
        }
        yLeaveCriticalSection(&_lock);
        if (!YAPI::ExceptionsDisabled) {
            throw YAPI_Exception(YAPI_IO_ERROR, errmsg);
        }
        return YAPI_IO_ERROR;
    }
    // store result in cache
    if (_cacheJson) {
        delete _cacheJson;
    }
    _cacheJson = new YJSONObject(apires);
    _cacheStamp = yapiGetTickCount() + YAPI::_yapiContext.GetCacheValidity();
    yLeaveCriticalSection(&_lock);

    return YAPI_SUCCESS;
}


void YDevice::clearCache(bool clearSubpath)
{
    yEnterCriticalSection(&_lock);
    _cacheStamp = 0;
    if (clearSubpath) {
        if (_cacheJson) {
            delete _cacheJson;
            _cacheJson = NULL;
        }
        if (_subpath) {
            delete _subpath;
            _subpath = NULL;
        }
    }
    yLeaveCriticalSection(&_lock);
}

YRETCODE YDevice::getFunctions(vector<YFUN_DESCR> **functions, string &errmsg)
{
    yEnterCriticalSection(&_lock);
    if (_functions.size() == 0) {
        int res = YapiWrapper::getFunctionsByDevice(_devdescr, 0, _functions, 64, errmsg);
        if (YISERR(res)) return (YRETCODE)res;
    }
    *functions = &_functions;
    yLeaveCriticalSection(&_lock);

    return YAPI_SUCCESS;
}


queue<yapiGlobalEvent> YAPI::_plug_events;
queue<yapiDataEvent> YAPI::_data_events;

u64 YAPI::_nextEnum = 0;
bool YAPI::_apiInitialized = false;

std::map<int, yCalibrationHandler> YAPI::_calibHandlers;
YHubDiscoveryCallback YAPI::_HubDiscoveryCallback = NULL;


// Switch to turn off exceptions and use return codes instead, for source-code compatibility
// with languages without exception support like pure C
bool YAPI::ExceptionsDisabled = false;

yCRITICAL_SECTION YAPI::_global_cs;
YAPIContext YAPI::_yapiContext;

// standard error objects
const string YAPI::INVALID_STRING = YAPI_INVALID_STRING;
const double YAPI::INVALID_DOUBLE = (-DBL_MAX);


yLogFunction YAPI::LogFunction = NULL;
yDeviceUpdateCallback YAPI::DeviceArrivalCallback = NULL;
yDeviceUpdateCallback YAPI::DeviceRemovalCallback = NULL;
yDeviceUpdateCallback YAPI::DeviceChangeCallback = NULL;

void YAPI::_yapiLogFunctionFwd(const char *log, u32 loglen)
{
    if (YAPI::LogFunction)
        YAPI::LogFunction(string(log));
}


void YAPI::_yapiDeviceLogCallbackFwd(YDEV_DESCR devdesc, const char *line)
{
    YModule *module;
    yDeviceSt infos;
    string errmsg;
    YModuleLogCallback callback;

    if (YapiWrapper::getDeviceInfo(devdesc, infos, errmsg) != YAPI_SUCCESS) return;
    module = YModule::FindModule(string(infos.serial) + ".module");
    callback = module->get_logCallback();
    if (callback) {
        callback(module, string(line));
    }
}


void YAPI::_yapiDeviceArrivalCallbackFwd(YDEV_DESCR devdesc)
{
    yapiGlobalEvent ev;
    yapiDataEvent dataEv;
    yDeviceSt infos;
    string errmsg;
    vector<YFunction*>::iterator it;

    YDevice *dev = YDevice::getDevice(devdesc);
    dev->clearCache(true);
    dataEv.type = YAPI_FUN_REFRESH;
    for (it = _FunctionCallbacks.begin(); it < _FunctionCallbacks.end(); it++) {
        if ((*it)->functionDescriptor() == Y_FUNCTIONDESCRIPTOR_INVALID) {
            dataEv.fun = *it;
            _data_events.push(dataEv);
        }
    }
    if (YAPI::DeviceArrivalCallback == NULL) return;
    ev.type = YAPI_DEV_ARRIVAL;
    //the function is already thread safe (use yapiLockDeviceCallaback)
    if (YapiWrapper::getDeviceInfo(devdesc, infos, errmsg) != YAPI_SUCCESS) return;
    ev.module = yFindModule(string(infos.serial) + ".module");
    ev.module->setImmutableAttributes(&infos);
    _plug_events.push(ev);
}

void YAPI::_yapiDeviceRemovalCallbackFwd(YDEV_DESCR devdesc)
{
    yapiGlobalEvent ev;
    yDeviceSt infos;
    string errmsg;

    if (YAPI::DeviceRemovalCallback == NULL) return;
    ev.type = YAPI_DEV_REMOVAL;
    if (YapiWrapper::getDeviceInfo(devdesc, infos, errmsg) != YAPI_SUCCESS) return;
    ev.module = yFindModule(string(infos.serial) + ".module");
    //the function is already thread safe (use yapiLockDeviceCallaback)
    _plug_events.push(ev);
}

void YAPI::_yapiDeviceChangeCallbackFwd(YDEV_DESCR devdesc)
{
    yapiGlobalEvent ev;
    yDeviceSt infos;
    string errmsg;

    if (YAPI::DeviceChangeCallback == NULL) return;
    ev.type = YAPI_DEV_CHANGE;
    if (YapiWrapper::getDeviceInfo(devdesc, infos, errmsg) != YAPI_SUCCESS) return;
    ev.module = yFindModule(string(infos.serial) + ".module");
    ev.module->setImmutableAttributes(&infos);
    //the function is already thread safe (use yapiLockDeviceCallaback)
    _plug_events.push(ev);
}

void YAPI::_yapiBeaconCallbackFwd(YDEV_DESCR devdesc, int beacon)
{
    yapiDataEvent ev;
    yDeviceSt infos;
    string errmsg;
    YModule *module;

    if (YapiWrapper::getDeviceInfo(devdesc, infos, errmsg) != YAPI_SUCCESS) return;
    module = yFindModule(string(infos.serial) + ".module");
    if (_moduleCallbackList.find(module) != _moduleCallbackList.end() && _moduleCallbackList[module] > 0) {
        ev.type = YAPI_DEV_BEACON;
        ev.module = module;
        ev.beacon = beacon;
        //the function is already thread safe (use yapiLockFunctionCallaback)
        _data_events.push(ev);
    }
}


void YAPI::_yapiDeviceConfigChangeCallbackFwd(YDEV_DESCR devdesc)
{
    yapiDataEvent ev;
    yDeviceSt infos;
    string errmsg;
    YModule *module;

    if (YapiWrapper::getDeviceInfo(devdesc, infos, errmsg) != YAPI_SUCCESS) return;
    module = yFindModule(string(infos.serial) + ".module");
    if (_moduleCallbackList.find(module) != _moduleCallbackList.end() && _moduleCallbackList[module] > 0) {
        ev.type = YAPI_DEV_CONFCHANGE;
        ev.module = module;
        ev.module->setImmutableAttributes(&infos);
        //the function is already thread safe (use yapiLockFunctionCallaback)
        _data_events.push(ev);
    }
}

void YAPI::_yapiFunctionUpdateCallbackFwd(YAPI_FUNCTION fundesc, const char *value)
{
    yapiDataEvent ev;

    //the function is already thread safe (use yapiLockFunctionCallaback)
    if (value == NULL) {
        ev.type = YAPI_FUN_UPDATE;
    } else {
        ev.type = YAPI_FUN_VALUE;
        memcpy(ev.value, value,YOCTO_PUBVAL_LEN);
    }
    for (unsigned i = 0; i < _FunctionCallbacks.size(); i++) {
        if (_FunctionCallbacks[i]->get_functionDescriptor() == fundesc) {
            ev.fun = _FunctionCallbacks[i];
            _data_events.push(ev);
        }
    }
}

void YAPI::_yapiFunctionTimedReportCallbackFwd(YAPI_FUNCTION fundesc, double timestamp, const u8 *bytes, u32 len, double duration)
{
    yapiDataEvent ev;

    for (unsigned i = 0; i < _TimedReportCallbackList.size(); i++) {
        if (_TimedReportCallbackList[i]->get_functionDescriptor() == fundesc) {
            u32 p = 0;
            ev.type = YAPI_FUN_TIMEDREPORT;
            ev.sensor = (YSensor*)_TimedReportCallbackList[i];
            ev.timestamp = timestamp;
            ev.duration = duration;
            ev.len = len;
            while (p < len) {
                ev.report[p++] = *bytes++;
            }
            _data_events.push(ev);
        }
    }
}

void YAPI::_yapiHubDiscoveryCallbackFwd(const char *serial, const char *url)
{
    yapiGlobalEvent ev;

    if (YAPI::_HubDiscoveryCallback == NULL) return;
    ev.type = YAPI_HUB_DISCOVER;
    strcpy(ev.serial, serial);
    strcpy(ev.url, url);
    _plug_events.push(ev);
}


static double decExp[16] = {
    1.0e-6, 1.0e-5, 1.0e-4, 1.0e-3, 1.0e-2, 1.0e-1, 1.0,
    1.0e1, 1.0e2, 1.0e3, 1.0e4, 1.0e5, 1.0e6, 1.0e7, 1.0e8, 1.0e9
};

// Convert Yoctopuce 16-bit decimal floats to standard double-precision floats
//
double YAPI::_decimalToDouble(s16 val)
{
    int negate = 0;
    int mantis = val & 2047;
    double res;

    if (mantis == 0) return 0.0;
    if (val < 0) {
        negate = 1;
        val = -val;
    }
    res = (double)(mantis) * decExp[val >> 11];

    return (negate ? -res : res);
}

// Convert standard double-precision floats to Yoctopuce 16-bit decimal floats
//
s16 YAPI::_doubleToDecimal(double val)
{
    int negate = 0;
    double comp, mant;
    int decpow;
    int res;

    if (val == 0.0) {
        return 0;
    }
    if (val < 0) {
        negate = 1;
        val = -val;
    }
    comp = val / 1999.0;
    decpow = 0;
    while (comp > decExp[decpow] && decpow < 15) {
        decpow++;
    }
    mant = val / decExp[decpow];
    if (decpow == 15 && mant > 2047.0) {
        res = (15 << 11) + 2047; // overflow
    } else {
        res = (decpow << 11) + (int)floor(mant + .5);
    }
    return (negate ? -res : res);
}

yCalibrationHandler YAPI::_getCalibrationHandler(int calibType)
{
    if (YAPI::_calibHandlers.find(calibType) == YAPI::_calibHandlers.end()) {
        return NULL;
    }
    return YAPI::_calibHandlers[calibType];
}


// Parse an array of u16 encoded in a base64-like string with memory-based compresssion
vector<int> YAPI::_decodeWords(string sdat)
{
    vector<int> udat;

    for (unsigned p = 0; p < sdat.size();) {
        unsigned val;
        unsigned c = sdat[p++];
        if (c == '*') {
            val = 0;
        } else if (c == 'X') {
            val = 0xffff;
        } else if (c == 'Y') {
            val = 0x7fff;
        } else if (c >= 'a') {
            int srcpos = (int)udat.size() - 1 - (c - 'a');
            if (srcpos < 0)
                val = 0;
            else
                val = udat[srcpos];
        } else {
            if (p + 2 > sdat.size()) return udat;
            val = (c - '0');
            c = sdat[p++];
            val += (c - '0') << 5;
            c = sdat[p++];
            if (c == 'z') c = '\\';
            val += (c - '0') << 10;
        }
        udat.push_back((int)val);
    }
    return udat;
}

// Parse a list of floats and return them as an array of fixed-point 1/1000 numbers
vector<int> YAPI::_decodeFloats(string sdat)
{
    vector<int> idat;

    for (unsigned p = 0; p < sdat.size();) {
        int val = 0;
        int sign = 1;
        int dec = 0;
        int decInc = 0;
        unsigned c = sdat[p++];
        while (c != '-' && (c < '0' || c > '9')) {
            if (p >= sdat.size()) {
                return idat;
            }
            c = sdat[p++];
        }
        if (c == '-') {
            if (p >= sdat.size()) {
                return idat;
            }
            sign = -sign;
            c = sdat[p++];
        }
        while ((c >= '0' && c <= '9') || c == '.') {
            if (c == '.') {
                decInc = 1;
            } else if (dec < 3) {
                val = val * 10 + (c - '0');
                dec += decInc;
            }
            if (p < sdat.size()) {
                c = sdat[p++];
            } else {
                c = 0;
            }
        }
        if (dec < 3) {
            if (dec == 0) val *= 1000;
            else if (dec == 1) val *= 100;
            else val *= 10;
        }
        idat.push_back(sign * val);
    }
    return idat;
}


static const char *hexArray = "0123456789ABCDEF";

string YAPI::_bin2HexStr(const string &data)
{
    const u8 *ptr = (u8*)data.data();
    size_t len = data.length();
    string res = string(len * 2, 0);
    for (size_t j = 0; j < len; j++, ptr++) {
        u8 v = *ptr;
        res[j * 2] = hexArray[v >> 4];
        res[j * 2 + 1] = hexArray[v & 0x0F];
    }
    return res;
}

string YAPI::_hexStr2Bin(const string &hex_str)
{
    size_t len = hex_str.length() / 2;
    const char *p = hex_str.c_str();
    string res = string(len, 0);
    for (size_t i = 0; i < len; i++) {
        u8 b = 0;
        int j;
        for (j = 0; j < 2; j++) {
            b <<= 4;
            if (*p >= 'a' && *p <= 'f') {
                b += 10 + *p - 'a';
            } else if (*p >= 'A' && *p <= 'F') {
                b += 10 + *p - 'A';
            } else if (*p >= '0' && *p <= '9') {
                b += *p - '0';
            }
            p++;
        }
        res[i] = b;
    }
    return res;
}

s64 YAPI::_hexStr2Long(const string &hex_str)
{
    size_t len = hex_str.length();
    const char *p = hex_str.c_str();
    s64 res = 0;
    for (size_t i = 0; i < len; i += 2) {
        u8 b = 0;
        int j;
        int nbdigit = 2;
        if ((len - i) < 2) {
            nbdigit = (int)(len - i);
        }
        for (j = 0; j < nbdigit; j++) {
            b <<= 4;
            if (*p >= 'a' && *p <= 'f') {
                b += 10 + *p - 'a';
            } else if (*p >= 'A' && *p <= 'F') {
                b += 10 + *p - 'A';
            } else if (*p >= '0' && *p <= '9') {
                b += *p - '0';
            }
            p++;
        }
        res = (res << 8) + b;
    }
    return res;
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
    YapiWrapper::getAPIVersion(version, date);
    return version;
}


/**
 * Initializes the Yoctopuce programming library explicitly.
 * It is not strictly needed to call yInitAPI(), as the library is
 * automatically  initialized when calling yRegisterHub() for the
 * first time.
 *
 * When YAPI::DETECT_NONE is used as detection mode,
 * you must explicitly use yRegisterHub() to point the API to the
 * VirtualHub on which your devices are connected before trying to access them.
 *
 * @param mode : an integer corresponding to the type of automatic
 *         device detection to use. Possible values are
 *         YAPI::DETECT_NONE, YAPI::DETECT_USB, YAPI::DETECT_NET,
 *         and YAPI::DETECT_ALL.
 * @param errmsg : a string passed by reference to receive any error message.
 *
 * @return YAPI::SUCCESS when the call succeeds.
 *
 * On failure returns a negative error code.
 */
YRETCODE YAPI::InitAPI(int mode, string &errmsg)
{
    char errbuf[YOCTO_ERRMSG_LEN];
    int i;

    if (YAPI::_apiInitialized)
        return YAPI_SUCCESS;
    YRETCODE res = yapiInitAPI(mode, errbuf);
    if (YISERR(res)) {
        errmsg = errbuf;
        return res;
    }
    yapiRegisterLogFunction(YAPI::_yapiLogFunctionFwd);
    yapiRegisterDeviceLogCallback(YAPI::_yapiDeviceLogCallbackFwd);
    yapiRegisterDeviceArrivalCallback(YAPI::_yapiDeviceArrivalCallbackFwd);
    yapiRegisterDeviceRemovalCallback(YAPI::_yapiDeviceRemovalCallbackFwd);
    yapiRegisterDeviceChangeCallback(YAPI::_yapiDeviceChangeCallbackFwd);
    yapiRegisterBeaconCallback(YAPI::_yapiBeaconCallbackFwd);
    yapiRegisterDeviceConfigChangeCallback(YAPI::_yapiDeviceConfigChangeCallbackFwd);
    yapiRegisterFunctionUpdateCallback(YAPI::_yapiFunctionUpdateCallbackFwd);
    yapiRegisterTimedReportCallback(YAPI::_yapiFunctionTimedReportCallbackFwd);
    yapiRegisterHubDiscoveryCallback(YAPI::_yapiHubDiscoveryCallbackFwd);

    yInitializeCriticalSection(&_updateDeviceList_CS);
    yInitializeCriticalSection(&_handleEvent_CS);
    yInitializeCriticalSection(&_global_cs);
    for (i = 0; i <= 20; i++) {
        YAPI::RegisterCalibrationHandler(i, YAPI::LinearCalibrationHandler);
    }
    YAPI::RegisterCalibrationHandler(YOCTO_CALIB_TYPE_OFS, YAPI::LinearCalibrationHandler);
    YAPI::_apiInitialized = true;

    return YAPI_SUCCESS;
}

/**
 * Waits for all pending communications with Yoctopuce devices to be
 * completed then frees dynamically allocated resources used by
 * the Yoctopuce library.
 *
 * From an operating system standpoint, it is generally not required to call
 * this function since the OS will automatically free allocated resources
 * once your program is completed. However, there are two situations when
 * you may really want to use that function:
 *
 * - Free all dynamically allocated memory blocks in order to
 * track a memory leak.
 *
 * - Send commands to devices right before the end
 * of the program. Since commands are sent in an asynchronous way
 * the program could exit before all commands are effectively sent.
 *
 * You should not call any other library function after calling
 * yFreeAPI(), or your program will crash.
 */
void YAPI::FreeAPI(void)
{
    if (YAPI::_apiInitialized) {
        yapiFreeAPI();
        yDeleteCriticalSection(&_updateDeviceList_CS);
        yDeleteCriticalSection(&_handleEvent_CS);
        yDeleteCriticalSection(&_global_cs);
        YDevice::ClearCache();
        YFunction::_ClearCache();
        while (!_plug_events.empty()) {
            _plug_events.pop();
        }
        while (!_data_events.empty()) {
            _data_events.pop();
        }
        _TimedReportCallbackList.clear();
        _FunctionCallbacks.clear();
        _moduleCallbackList.clear();
        _calibHandlers.clear();
        YAPI::_apiInitialized = false;
    }
}

/**
 * Disables the use of exceptions to report runtime errors.
 * When exceptions are disabled, every function returns a specific
 * error value which depends on its type and which is documented in
 * this reference manual.
 */
void YAPI::DisableExceptions(void)
{
    YAPI::ExceptionsDisabled = true;
}

/**
 * Re-enables the use of exceptions for runtime error handling.
 * Be aware than when exceptions are enabled, every function that fails
 * triggers an exception. If the exception is not caught by the user code,
 * it either fires the debugger or aborts (i.e. crash) the program.
 */
void YAPI::EnableExceptions(void)
{
    YAPI::ExceptionsDisabled = false;
}

/**
 * Registers a log callback function. This callback will be called each time
 * the API have something to say. Quite useful to debug the API.
 *
 * @param logfun : a procedure taking a string parameter, or NULL
 *         to unregister a previously registered  callback.
 */
void YAPI::RegisterLogFunction(yLogFunction logfun)
{
    YAPI::LogFunction = logfun;
}

/**
 * Register a callback function, to be called each time
 * a device is plugged. This callback will be invoked while yUpdateDeviceList
 * is running. You will have to call this function on a regular basis.
 *
 * @param arrivalCallback : a procedure taking a YModule parameter, or NULL
 *         to unregister a previously registered  callback.
 */
void YAPI::RegisterDeviceArrivalCallback(yDeviceUpdateCallback arrivalCallback)
{
    YAPI::DeviceArrivalCallback = arrivalCallback;
    if (arrivalCallback) {
        YModule *mod = YModule::FirstModule();
        while (mod) {
            if (mod->isOnline()) {
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
 * a device is unplugged. This callback will be invoked while yUpdateDeviceList
 * is running. You will have to call this function on a regular basis.
 *
 * @param removalCallback : a procedure taking a YModule parameter, or NULL
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

/**
 * Register a callback function, to be called each time an Network Hub send
 * an SSDP message. The callback has two string parameter, the first one
 * contain the serial number of the hub and the second contain the URL of the
 * network hub (this URL can be passed to RegisterHub). This callback will be invoked
 * while yUpdateDeviceList is running. You will have to call this function on a regular basis.
 *
 * @param hubDiscoveryCallback : a procedure taking two string parameter, the serial
 *         number and the hub URL. Use NULL to unregister a previously registered  callback.
 */
void YAPI::RegisterHubDiscoveryCallback(YHubDiscoveryCallback hubDiscoveryCallback)
{
    YAPI::_HubDiscoveryCallback = hubDiscoveryCallback;
    string error;
    YAPI::TriggerHubDiscovery(error);
}

/**
 * Force a hub discovery, if a callback as been registered with yRegisterHubDiscoveryCallback it
 * will be called for each net work hub that will respond to the discovery.
 *
 * @param errmsg : a string passed by reference to receive any error message.
 *
 * @return YAPI::SUCCESS when the call succeeds.
 *         On failure returns a negative error code.
 */
YRETCODE YAPI::TriggerHubDiscovery(string &errmsg)
{
    YRETCODE res;
    char errbuf[YOCTO_ERRMSG_LEN];
    if (!YAPI::_apiInitialized) {
        res = YAPI::InitAPI(0, errmsg);
        if (YISERR(res)) return res;
    }
    res = yapiTriggerHubDiscovery(errbuf);
    if (YISERR(res)) {
        errmsg = errbuf;
        return res;
    }
    return YAPI_SUCCESS;
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
    double x = rawValues[0];
    double adj = refValues[0] - x;
    int i = 0;
    int npt;

    if (calibType < YOCTO_CALIB_TYPE_OFS) {
        // calibration types n=1..10 and 11..20 are meant for linear calibration using n points
        npt = calibType % 10;
        if (npt > (int)rawValues.size()) npt = (int)rawValues.size();
        if (npt > (int)refValues.size()) npt = (int)refValues.size();
    } else {
        npt = (int)refValues.size();
    }
    while (rawValue > rawValues[i] && ++i < npt) {
        double x2 = x;
        double adj2 = adj;

        x = rawValues[i];
        adj = refValues[i] - x;

        if (rawValue < x && x > x2) {
            adj = adj2 + (adj - adj2) * (rawValue - x2) / (x - x2);
        }
    }
    return rawValue + adj;
}


/**
 * Test if the hub is reachable. This method do not register the hub, it only test if the
 * hub is usable. The url parameter follow the same convention as the yRegisterHub
 * method. This method is useful to verify the authentication parameters for a hub. It
 * is possible to force this method to return after mstimeout milliseconds.
 *
 * @param url : a string containing either "usb","callback" or the
 *         root URL of the hub to monitor
 * @param mstimeout : the number of millisecond available to test the connection.
 * @param errmsg : a string passed by reference to receive any error message.
 *
 * @return YAPI::SUCCESS when the call succeeds.
 *
 * On failure returns a negative error code.
 */
YRETCODE YAPI::TestHub(const string &url, int mstimeout, string &errmsg)
{
    char errbuf[YOCTO_ERRMSG_LEN];
    YRETCODE res;
    res = yapiTestHub(url.c_str(), mstimeout, errbuf);
    if (YISERR(res)) {
        errmsg = errbuf;
    }
    return res;
}


/**
 * Set up the Yoctopuce library to use modules connected on a given machine. Idealy this
 * call will be made once at the begining of your application.  The
 * parameter will determine how the API will work. Use the following values:
 *
 * <b>usb</b>: When the usb keyword is used, the API will work with
 * devices connected directly to the USB bus. Some programming languages such a JavaScript,
 * PHP, and Java don't provide direct access to USB hardware, so usb will
 * not work with these. In this case, use a VirtualHub or a networked YoctoHub (see below).
 *
 * <b><i>x.x.x.x</i></b> or <b><i>hostname</i></b>: The API will use the devices connected to the
 * host with the given IP address or hostname. That host can be a regular computer
 * running a <i>native VirtualHub</i>, a <i>VirtualHub for web</i> hosted on a server,
 * or a networked YoctoHub such as YoctoHub-Ethernet or
 * YoctoHub-Wireless. If you want to use the VirtualHub running on you local
 * computer, use the IP address 127.0.0.1. If the given IP is unresponsive, yRegisterHub
 * will not return until a time-out defined by ySetNetworkTimeout has elapsed.
 * However, it is possible to preventively test a connection  with yTestHub.
 * If you cannot afford a network time-out, you can use the non-blocking yPregisterHub
 * function that will establish the connection as soon as it is available.
 *
 *
 * <b>callback</b>: that keyword make the API run in "<i>HTTP Callback</i>" mode.
 * This a special mode allowing to take control of Yoctopuce devices
 * through a NAT filter when using a VirtualHub or a networked YoctoHub. You only
 * need to configure your hub to call your server script on a regular basis.
 * This mode is currently available for PHP and Node.JS only.
 *
 * Be aware that only one application can use direct USB access at a
 * given time on a machine. Multiple access would cause conflicts
 * while trying to access the USB modules. In particular, this means
 * that you must stop the VirtualHub software before starting
 * an application that uses direct USB access. The workaround
 * for this limitation is to set up the library to use the VirtualHub
 * rather than direct USB access.
 *
 * If access control has been activated on the hub, virtual or not, you want to
 * reach, the URL parameter should look like:
 *
 * http://username:password@address:port
 *
 * You can call <i>RegisterHub</i> several times to connect to several machines. On
 * the other hand, it is useless and even counterproductive to call <i>RegisterHub</i>
 * with to same address multiple times during the life of the application.
 *
 * @param url : a string containing either "usb","callback" or the
 *         root URL of the hub to monitor
 * @param errmsg : a string passed by reference to receive any error message.
 *
 * @return YAPI::SUCCESS when the call succeeds.
 *
 * On failure returns a negative error code.
 */
YRETCODE YAPI::RegisterHub(const string &url, string &errmsg)
{
    char errbuf[YOCTO_ERRMSG_LEN];
    YRETCODE res;
    if (!YAPI::_apiInitialized) {
        res = YAPI::InitAPI(0, errmsg);
        if (YISERR(res)) return res;
    }
    res = yapiRegisterHub(url.c_str(), errbuf);
    if (YISERR(res)) {
        errmsg = errbuf;
    }
    return res;
}

/**
 * Fault-tolerant alternative to yRegisterHub(). This function has the same
 * purpose and same arguments as yRegisterHub(), but does not trigger
 * an error when the selected hub is not available at the time of the function call.
 * If the connexion cannot be established immediately, a background task will automatically
 * perform periodic retries. This makes it possible to register a network hub independently of the current
 * connectivity, and to try to contact it only when a device is actively needed.
 *
 * @param url : a string containing either "usb","callback" or the
 *         root URL of the hub to monitor
 * @param errmsg : a string passed by reference to receive any error message.
 *
 * @return YAPI::SUCCESS when the call succeeds.
 *
 * On failure returns a negative error code.
 */
YRETCODE YAPI::PreregisterHub(const string &url, string &errmsg)
{
    char errbuf[YOCTO_ERRMSG_LEN];
    YRETCODE res;
    if (!YAPI::_apiInitialized) {
        res = YAPI::InitAPI(0, errmsg);
        if (YISERR(res)) return res;
    }
    res = yapiPreregisterHub(url.c_str(), errbuf);
    if (YISERR(res)) {
        errmsg = errbuf;
    }
    return res;
}

/**
 * Set up the Yoctopuce library to no more use modules connected on a previously
 * registered machine with RegisterHub.
 *
 * @param url : a string containing either "usb" or the
 *         root URL of the hub to monitor
 */
void YAPI::UnregisterHub(const string &url)
{
    if (!YAPI::_apiInitialized) {
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
 * and to make the application aware of hot-plug events. However, since device
 * detection is quite a heavy process, UpdateDeviceList shouldn't be called more
 * than once every two seconds.
 *
 * @param errmsg : a string passed by reference to receive any error message.
 *
 * @return YAPI::SUCCESS when the call succeeds.
 *
 * On failure returns a negative error code.
 */
YRETCODE YAPI::UpdateDeviceList(string &errmsg)
{
    if (!YAPI::_apiInitialized) {
        YRETCODE res = YAPI::InitAPI(0, errmsg);
        if (YISERR(res)) return res;
    }
    // prevent reentrance into this function
    yEnterCriticalSection(&_updateDeviceList_CS);
    // call the updateDeviceList of the yapi layer
    // yapi know when it is needed to do a full update
    YRETCODE res = YapiWrapper::updateDeviceList(false, errmsg);
    if (!YISERR(res)) {
        // handle other notification
        res = YapiWrapper::handleEvents(errmsg);
    }
    // unpop plug/unplug event and call user callback
    while (!_plug_events.empty()) {
        yapiGlobalEvent ev;
        yapiLockDeviceCallBack(NULL);
        if (_plug_events.empty()) {
            yapiUnlockDeviceCallBack(NULL);
            break;
        }
        ev = _plug_events.front();
        _plug_events.pop();
        yapiUnlockDeviceCallBack(NULL);
        switch (ev.type) {
        case YAPI_DEV_ARRIVAL:
            if (!YAPI::DeviceArrivalCallback) break;
            YAPI::DeviceArrivalCallback(ev.module);
            break;
        case YAPI_DEV_REMOVAL:
            if (!YAPI::DeviceRemovalCallback) break;
            YAPI::DeviceRemovalCallback(ev.module);
            break;
        case YAPI_DEV_CHANGE:
            if (!YAPI::DeviceChangeCallback) break;
            YAPI::DeviceChangeCallback(ev.module);
            break;
        case YAPI_HUB_DISCOVER:
            if (!YAPI::_HubDiscoveryCallback) break;
            YAPI::_HubDiscoveryCallback(string(ev.serial), string(ev.url));
            break;
        default:
            break;
        }
    }
    yLeaveCriticalSection(&_updateDeviceList_CS);
    return res;
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
 * @return YAPI::SUCCESS when the call succeeds.
 *
 * On failure returns a negative error code.
 */
YRETCODE YAPI::HandleEvents(string &errmsg)
{
    YRETCODE res;

    if (!YAPI::_apiInitialized) {
        YRETCODE res = YAPI::InitAPI(0, errmsg);
        if (YISERR(res)) return res;
    }

    // prevent reentrance into this function
    yEnterCriticalSection(&_handleEvent_CS);
    // handle other notification
    res = YapiWrapper::handleEvents(errmsg);
    if (YISERR(res)) {
        yLeaveCriticalSection(&_handleEvent_CS);
        return res;
    }
    // pop data event and call user callback
    while (!_data_events.empty()) {
        yapiDataEvent ev;
        YSensor *sensor;
        vector<int> report;

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
            ev.fun->_invokeValueCallback((string)ev.value);
            break;
        case YAPI_FUN_TIMEDREPORT:
            if (ev.report[0] <= 2) {
                sensor = ev.sensor;
                report.assign(ev.report, ev.report + ev.len);
                sensor->_invokeTimedReportCallback(sensor->_decodeTimedReport(ev.timestamp, ev.duration, report));
            }
            break;
        case YAPI_FUN_REFRESH:
            ev.fun->isOnline();
            break;
        case YAPI_DEV_CONFCHANGE:
            ev.module->_invokeConfigChangeCallback();
            break;
        case YAPI_DEV_BEACON:
            ev.module->_invokeBeaconCallback((Y_BEACON_enum)ev.beacon);
            break;
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
 * consume CPU cycles significantly. The processor is left available for
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
 * @return YAPI::SUCCESS when the call succeeds.
 *
 * On failure returns a negative error code.
 */
YRETCODE YAPI::Sleep(unsigned ms_duration, string &errmsg)
{
    char errbuf[YOCTO_ERRMSG_LEN];
    YRETCODE res;
    u64 waituntil = YAPI::GetTickCount() + ms_duration;
    do {
        res = YAPI::HandleEvents(errmsg);
        if (YISERR(res)) {
            errmsg = errbuf;
            return res;
        }
        if (waituntil > YAPI::GetTickCount()) {
            res = yapiSleep(2, errbuf);
            if (YISERR(res)) {
                errmsg = errbuf;
                return res;
            }
        }
    } while (waituntil > YAPI::GetTickCount());

    return YAPI_SUCCESS;
}

/**
 * Returns the current value of a monotone millisecond-based time counter.
 * This counter can be used to compute delays in relation with
 * Yoctopuce devices, which also uses the millisecond as timebase.
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
 * A...Z, a...z, 0...9, _, and -.
 * If you try to configure a logical name with an incorrect string,
 * the invalid characters are ignored.
 *
 * @param name : a string containing the name to check.
 *
 * @return true if the name is valid, false otherwise.
 */
bool YAPI::CheckLogicalName(const string &name)
{
    return (yapiCheckLogicalName(name.c_str()) != 0);
}

u16 YapiWrapper::getAPIVersion(string &version, string &date)
{
    const char *_ver, *_date;
    u16 res = yapiGetAPIVersion(&_ver, &_date);
    version = _ver;
    date = _date;
    return res;
}

YDEV_DESCR YapiWrapper::getDevice(const string &device_str, string &errmsg)
{
    char errbuf[YOCTO_ERRMSG_LEN];
    YDEV_DESCR res;

    res = yapiGetDevice(device_str.data(), errbuf);
    if (YISERR(res)) {
        errmsg = errbuf;
    }
    return (YDEV_DESCR)res;
}

int YapiWrapper::getAllDevices(vector<YDEV_DESCR> &buffer, string &errmsg)
{
    char errbuf[YOCTO_ERRMSG_LEN];
    int n_elems = 32;
    int initsize = n_elems * sizeof(YDEV_DESCR);
    int neededsize, res;
    YDEV_DESCR *ptr = new YDEV_DESCR[n_elems];

    res = yapiGetAllDevices(ptr, initsize, &neededsize, errbuf);
    if (YISERR(res)) {
        delete [] ptr;
        errmsg = errbuf;
        return (YRETCODE)res;
    }
    if (neededsize > initsize) {
        delete [] ptr;
        n_elems = neededsize / sizeof(YDEV_DESCR);
        initsize = n_elems * sizeof(YDEV_DESCR);
        ptr = new YDEV_DESCR[n_elems];
        res = yapiGetAllDevices(ptr, initsize, NULL, errbuf);
        if (YISERR(res)) {
            delete [] ptr;
            errmsg = errbuf;
            return (YRETCODE)res;
        }
    }
    buffer = vector<YDEV_DESCR>(ptr, ptr + res);
    delete [] ptr;

    return res;
}

YRETCODE YapiWrapper::getDeviceInfo(YDEV_DESCR devdesc, yDeviceSt &infos, string &errmsg)
{
    char errbuf[YOCTO_ERRMSG_LEN];
    YRETCODE res;

    res = yapiGetDeviceInfo(devdesc, &infos, errbuf);
    if (YISERR(res)) {
        errmsg = errbuf;
    }
    return res;
}

YFUN_DESCR YapiWrapper::getFunction(const string &class_str, const string &function_str, string &errmsg)
{
    char errbuf[YOCTO_ERRMSG_LEN];

    YFUN_DESCR res = yapiGetFunction(class_str.data(), function_str.data(), errbuf);
    if (YISERR(res)) {
        errmsg = errbuf;
    }
    return res;
}

int YapiWrapper::getFunctionsByClass(const string &class_str, YFUN_DESCR prevfundesc, vector<YFUN_DESCR> &buffer, int maxsize, string &errmsg)
{
    char errbuf[YOCTO_ERRMSG_LEN];
    int n_elems = 32;
    int initsize = n_elems * sizeof(YDEV_DESCR);
    int neededsize;
    YFUN_DESCR *ptr = new YFUN_DESCR[n_elems];

    int res = yapiGetFunctionsByClass(class_str.data(), prevfundesc, ptr, initsize, &neededsize, errbuf);
    if (YISERR(res)) {
        delete [] ptr;
        errmsg = errbuf;
        return res;
    }
    if (neededsize > initsize) {
        delete [] ptr;
        n_elems = neededsize / sizeof(YFUN_DESCR);
        initsize = n_elems * sizeof(YFUN_DESCR);
        ptr = new YFUN_DESCR[n_elems];
        res = yapiGetFunctionsByClass(class_str.data(), prevfundesc, ptr, initsize, NULL, errbuf);
        if (YISERR(res)) {
            delete [] ptr;
            errmsg = errbuf;
            return res;
        }
    }
    buffer = vector<YFUN_DESCR>(ptr, ptr + res);
    delete [] ptr;

    return res;
}

int YapiWrapper::getFunctionsByDevice(YDEV_DESCR devdesc, YFUN_DESCR prevfundesc, vector<YFUN_DESCR> &buffer, int maxsize, string &errmsg)
{
    char errbuf[YOCTO_ERRMSG_LEN];
    int n_elems = 32;
    int initsize = n_elems * sizeof(YDEV_DESCR);
    int neededsize;
    YFUN_DESCR *ptr = new YFUN_DESCR[n_elems];

    int res = yapiGetFunctionsByDevice(devdesc, prevfundesc, ptr, initsize, &neededsize, errbuf);
    if (YISERR(res)) {
        delete [] ptr;
        errmsg = errbuf;
        return res;
    }
    if (neededsize > initsize) {
        delete [] ptr;
        n_elems = neededsize / sizeof(YFUN_DESCR);
        initsize = n_elems * sizeof(YFUN_DESCR);
        ptr = new YFUN_DESCR[n_elems];
        res = yapiGetFunctionsByDevice(devdesc, prevfundesc, ptr, initsize, NULL, errbuf);
        if (YISERR(res)) {
            delete [] ptr;
            errmsg = errbuf;
            return res;
        }
    }
    buffer = vector<YFUN_DESCR>(ptr, ptr + res);
    delete [] ptr;

    return res;
}

YDEV_DESCR YapiWrapper::getDeviceByFunction(YFUN_DESCR fundesc, string &errmsg)
{
    char errbuf[YOCTO_ERRMSG_LEN];
    YDEV_DESCR dev;

    int res = yapiGetFunctionInfo(fundesc, &dev, NULL, NULL, NULL, NULL, errbuf);
    if (YISERR(res)) {
        errmsg = errbuf;
        return res;
    }

    return dev;
}

YRETCODE YapiWrapper::getFunctionInfo(YFUN_DESCR fundesc, YDEV_DESCR &devdescr, string &serial, string &funcId, string &funcName, string &funcVal, string &errmsg)
{
    char errbuf[YOCTO_ERRMSG_LEN];
    char snum[YOCTO_SERIAL_LEN];
    char fnid[YOCTO_FUNCTION_LEN];
    char fnam[YOCTO_LOGICAL_LEN];
    char fval[YOCTO_PUBVAL_LEN];

    YRETCODE res = yapiGetFunctionInfoEx(fundesc, &devdescr, snum, fnid, NULL, fnam, fval, errbuf);
    if (YISERR(res)) {
        errmsg = errbuf;
    } else {
        serial = snum;
        funcId = fnid;
        funcName = fnam;
        funcVal = fval;
    }

    return res;
}

YRETCODE YapiWrapper::getFunctionInfoEx(YFUN_DESCR fundesc, YDEV_DESCR &devdescr, string &serial, string &funcId, string &baseType, string &funcName, string &funcVal, string &errmsg)
{
    char errbuf[YOCTO_ERRMSG_LEN];
    char snum[YOCTO_SERIAL_LEN];
    char fnid[YOCTO_FUNCTION_LEN];
    char fbt[YOCTO_FUNCTION_LEN];
    char fnam[YOCTO_LOGICAL_LEN];
    char fval[YOCTO_PUBVAL_LEN];

    YRETCODE res = yapiGetFunctionInfoEx(fundesc, &devdescr, snum, fnid, fbt, fnam, fval, errbuf);
    if (YISERR(res)) {
        errmsg = errbuf;
    } else {
        serial = snum;
        funcId = fnid;
        baseType = fbt;
        funcName = fnam;
        funcVal = fval;
    }

    return res;
}

YRETCODE YapiWrapper::updateDeviceList(bool forceupdate, string &errmsg)
{
    char errbuf[YOCTO_ERRMSG_LEN];
    YRETCODE res = yapiUpdateDeviceList(forceupdate ? 1 : 0, errbuf);
    if (YISERR(res)) {
        errmsg = errbuf;
        return res;
    }
    return YAPI_SUCCESS;
}

YRETCODE YapiWrapper::handleEvents(string &errmsg)
{
    char errbuf[YOCTO_ERRMSG_LEN];
    YRETCODE res = yapiHandleEvents(errbuf);
    if (YISERR(res)) {
        errmsg = errbuf;
        return res;
    }
    return YAPI_SUCCESS;
}


string YapiWrapper::ysprintf(const char *fmt, ...)
{
    va_list args;
    char on_stack_buffer[1024];
    int n, size = 1024;
    char *buffer = on_stack_buffer;
    string res = "";

    for (int i = 0; i < 13; i++) {
        va_start(args, fmt);
        n = vsnprintf(buffer, size, fmt, args);
        va_end(args);
        if (n > -1 && n < size) {
            res = string(buffer);
            break;
        }

        if (n > -1) {
            size = n + 1;
        } else {
            size *= 2;
        }
        if (buffer != on_stack_buffer) {
            free(buffer);
        }
        buffer = (char*)malloc(size);
    }
    if (buffer != on_stack_buffer) {
        free(buffer);
    }
    return res;
}


//--- (generated code: YModule constructor)
YModule::YModule(const string &func): YFunction(func)
                                      //--- (end of generated code: YModule constructor)
                                      //--- (generated code: YModule initialization)
    ,_productName(PRODUCTNAME_INVALID)
    ,_serialNumber(SERIALNUMBER_INVALID)
    ,_productId(PRODUCTID_INVALID)
    ,_productRelease(PRODUCTRELEASE_INVALID)
    ,_firmwareRelease(FIRMWARERELEASE_INVALID)
    ,_persistentSettings(PERSISTENTSETTINGS_INVALID)
    ,_luminosity(LUMINOSITY_INVALID)
    ,_beacon(BEACON_INVALID)
    ,_upTime(UPTIME_INVALID)
    ,_usbCurrent(USBCURRENT_INVALID)
    ,_rebootCountdown(REBOOTCOUNTDOWN_INVALID)
    ,_userVar(USERVAR_INVALID)
    ,_valueCallbackModule(NULL)
    ,_logCallback(NULL)
    ,_confChangeCallback(NULL)
    ,_beaconCallback(NULL)
//--- (end of generated code: YModule initialization)
{
    _className = "Module";
}

YModule::~YModule()
{
    //--- (generated code: YModule cleanup)
//--- (end of generated code: YModule cleanup)
}


//--- (generated code: YModule implementation)
// static attributes
const string YModule::PRODUCTNAME_INVALID = YAPI_INVALID_STRING;
const string YModule::SERIALNUMBER_INVALID = YAPI_INVALID_STRING;
const string YModule::FIRMWARERELEASE_INVALID = YAPI_INVALID_STRING;

int YModule::_parseAttr(YJSONObject *json_val)
{
    if(json_val->has("productName")) {
        _productName =  json_val->getString("productName");
    }
    if(json_val->has("serialNumber")) {
        _serialNumber =  json_val->getString("serialNumber");
    }
    if(json_val->has("productId")) {
        _productId =  json_val->getInt("productId");
    }
    if(json_val->has("productRelease")) {
        _productRelease =  json_val->getInt("productRelease");
    }
    if(json_val->has("firmwareRelease")) {
        _firmwareRelease =  json_val->getString("firmwareRelease");
    }
    if(json_val->has("persistentSettings")) {
        _persistentSettings =  (Y_PERSISTENTSETTINGS_enum)json_val->getInt("persistentSettings");
    }
    if(json_val->has("luminosity")) {
        _luminosity =  json_val->getInt("luminosity");
    }
    if(json_val->has("beacon")) {
        _beacon =  (Y_BEACON_enum)json_val->getInt("beacon");
    }
    if(json_val->has("upTime")) {
        _upTime =  json_val->getLong("upTime");
    }
    if(json_val->has("usbCurrent")) {
        _usbCurrent =  json_val->getInt("usbCurrent");
    }
    if(json_val->has("rebootCountdown")) {
        _rebootCountdown =  json_val->getInt("rebootCountdown");
    }
    if(json_val->has("userVar")) {
        _userVar =  json_val->getInt("userVar");
    }
    return YFunction::_parseAttr(json_val);
}


/**
 * Returns the commercial name of the module, as set by the factory.
 *
 * @return a string corresponding to the commercial name of the module, as set by the factory
 *
 * On failure, throws an exception or returns YModule::PRODUCTNAME_INVALID.
 */
string YModule::get_productName(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration == 0) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YModule::PRODUCTNAME_INVALID;
                }
            }
        }
        res = _productName;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the serial number of the module, as set by the factory.
 *
 * @return a string corresponding to the serial number of the module, as set by the factory
 *
 * On failure, throws an exception or returns YModule::SERIALNUMBER_INVALID.
 */
string YModule::get_serialNumber(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration == 0) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YModule::SERIALNUMBER_INVALID;
                }
            }
        }
        res = _serialNumber;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the USB device identifier of the module.
 *
 * @return an integer corresponding to the USB device identifier of the module
 *
 * On failure, throws an exception or returns YModule::PRODUCTID_INVALID.
 */
int YModule::get_productId(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration == 0) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YModule::PRODUCTID_INVALID;
                }
            }
        }
        res = _productId;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the release number of the module hardware, preprogrammed at the factory.
 * The original hardware release returns value 1, revision B returns value 2, etc.
 *
 * @return an integer corresponding to the release number of the module hardware, preprogrammed at the factory
 *
 * On failure, throws an exception or returns YModule::PRODUCTRELEASE_INVALID.
 */
int YModule::get_productRelease(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration == 0) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YModule::PRODUCTRELEASE_INVALID;
                }
            }
        }
        res = _productRelease;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the version of the firmware embedded in the module.
 *
 * @return a string corresponding to the version of the firmware embedded in the module
 *
 * On failure, throws an exception or returns YModule::FIRMWARERELEASE_INVALID.
 */
string YModule::get_firmwareRelease(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YModule::FIRMWARERELEASE_INVALID;
                }
            }
        }
        res = _firmwareRelease;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the current state of persistent module settings.
 *
 * @return a value among YModule::PERSISTENTSETTINGS_LOADED, YModule::PERSISTENTSETTINGS_SAVED and
 * YModule::PERSISTENTSETTINGS_MODIFIED corresponding to the current state of persistent module settings
 *
 * On failure, throws an exception or returns YModule::PERSISTENTSETTINGS_INVALID.
 */
Y_PERSISTENTSETTINGS_enum YModule::get_persistentSettings(void)
{
    Y_PERSISTENTSETTINGS_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YModule::PERSISTENTSETTINGS_INVALID;
                }
            }
        }
        res = _persistentSettings;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

int YModule::set_persistentSettings(Y_PERSISTENTSETTINGS_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("persistentSettings", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the luminosity of the  module informative LEDs (from 0 to 100).
 *
 * @return an integer corresponding to the luminosity of the  module informative LEDs (from 0 to 100)
 *
 * On failure, throws an exception or returns YModule::LUMINOSITY_INVALID.
 */
int YModule::get_luminosity(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YModule::LUMINOSITY_INVALID;
                }
            }
        }
        res = _luminosity;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the luminosity of the module informative leds. The parameter is a
 * value between 0 and 100.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : an integer corresponding to the luminosity of the module informative leds
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YModule::set_luminosity(int newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("luminosity", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the state of the localization beacon.
 *
 * @return either YModule::BEACON_OFF or YModule::BEACON_ON, according to the state of the localization beacon
 *
 * On failure, throws an exception or returns YModule::BEACON_INVALID.
 */
Y_BEACON_enum YModule::get_beacon(void)
{
    Y_BEACON_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YModule::BEACON_INVALID;
                }
            }
        }
        res = _beacon;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Turns on or off the module localization beacon.
 *
 * @param newval : either YModule::BEACON_OFF or YModule::BEACON_ON
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YModule::set_beacon(Y_BEACON_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = (newval>0 ? "1" : "0");
        res = _setAttr("beacon", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the number of milliseconds spent since the module was powered on.
 *
 * @return an integer corresponding to the number of milliseconds spent since the module was powered on
 *
 * On failure, throws an exception or returns YModule::UPTIME_INVALID.
 */
s64 YModule::get_upTime(void)
{
    s64 res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YModule::UPTIME_INVALID;
                }
            }
        }
        res = _upTime;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the current consumed by the module on the USB bus, in milli-amps.
 *
 * @return an integer corresponding to the current consumed by the module on the USB bus, in milli-amps
 *
 * On failure, throws an exception or returns YModule::USBCURRENT_INVALID.
 */
int YModule::get_usbCurrent(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YModule::USBCURRENT_INVALID;
                }
            }
        }
        res = _usbCurrent;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the remaining number of seconds before the module restarts, or zero when no
 * reboot has been scheduled.
 *
 * @return an integer corresponding to the remaining number of seconds before the module restarts, or zero when no
 *         reboot has been scheduled
 *
 * On failure, throws an exception or returns YModule::REBOOTCOUNTDOWN_INVALID.
 */
int YModule::get_rebootCountdown(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YModule::REBOOTCOUNTDOWN_INVALID;
                }
            }
        }
        res = _rebootCountdown;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

int YModule::set_rebootCountdown(int newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("rebootCountdown", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the value previously stored in this attribute.
 * On startup and after a device reboot, the value is always reset to zero.
 *
 * @return an integer corresponding to the value previously stored in this attribute
 *
 * On failure, throws an exception or returns YModule::USERVAR_INVALID.
 */
int YModule::get_userVar(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YModule::USERVAR_INVALID;
                }
            }
        }
        res = _userVar;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Stores a 32 bit value in the device RAM. This attribute is at programmer disposal,
 * should he need to store a state variable.
 * On startup and after a device reboot, the value is always reset to zero.
 *
 * @param newval : an integer
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YModule::set_userVar(int newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("userVar", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Allows you to find a module from its serial number or from its logical name.
 *
 * This function does not require that the module is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the module is
 * indeed online at a given time. In case of ambiguity when looking for
 * a module by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string containing either the serial number or
 *         the logical name of the desired module
 *
 * @return a YModule object allowing you to drive the module
 *         or get additional information on the module.
 */
YModule* YModule::FindModule(string func)
{
    YModule* obj = NULL;
    string cleanHwId;
    int modpos = 0;
    cleanHwId = func;
    modpos = _ystrpos(func, ".module");
    if (modpos != ((int)(func).length() - 7)) {
        cleanHwId = func + ".module";
    }
    int taken = 0;
    if (YAPI::_apiInitialized) {
        yEnterCriticalSection(&YAPI::_global_cs);
        taken = 1;
    }try {
        obj = (YModule*) YFunction::_FindFromCache("Module", cleanHwId);
        if (obj == NULL) {
            obj = new YModule(cleanHwId);
            YFunction::_AddToCache("Module", cleanHwId, obj);
        }
    } catch (std::exception &) {
        if (taken) yLeaveCriticalSection(&YAPI::_global_cs);
        throw;
    }
    if (taken) yLeaveCriticalSection(&YAPI::_global_cs);
    return obj;
}

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
int YModule::registerValueCallback(YModuleValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackModule = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YModule::_invokeValueCallback(string value)
{
    if (_valueCallbackModule != NULL) {
        _valueCallbackModule(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

string YModule::get_productNameAndRevision(void)
{
    string prodname;
    int prodrel = 0;
    string fullname;

    prodname = this->get_productName();
    prodrel = this->get_productRelease();
    if (prodrel > 1) {
        fullname = YapiWrapper::ysprintf("%s rev. %c",prodname.c_str(),64 + prodrel);
    } else {
        fullname = prodname;
    }
    return fullname;
}

/**
 * Saves current settings in the nonvolatile memory of the module.
 * Warning: the number of allowed save operations during a module life is
 * limited (about 100000 cycles). Do not call this function within a loop.
 *
 * @return YAPI::SUCCESS when the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YModule::saveToFlash(void)
{
    return this->set_persistentSettings(Y_PERSISTENTSETTINGS_SAVED);
}

/**
 * Reloads the settings stored in the nonvolatile memory, as
 * when the module is powered on.
 *
 * @return YAPI::SUCCESS when the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YModule::revertFromFlash(void)
{
    return this->set_persistentSettings(Y_PERSISTENTSETTINGS_LOADED);
}

/**
 * Schedules a simple module reboot after the given number of seconds.
 *
 * @param secBeforeReboot : number of seconds before rebooting
 *
 * @return YAPI::SUCCESS when the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YModule::reboot(int secBeforeReboot)
{
    return this->set_rebootCountdown(secBeforeReboot);
}

/**
 * Schedules a module reboot into special firmware update mode.
 *
 * @param secBeforeReboot : number of seconds before rebooting
 *
 * @return YAPI::SUCCESS when the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YModule::triggerFirmwareUpdate(int secBeforeReboot)
{
    return this->set_rebootCountdown(-secBeforeReboot);
}

void YModule::_startStopDevLog(string serial,bool start)
{
    int i_start = 0;
    if (start) {
        i_start = 1;
    } else {
        i_start = 0;
    }

    yapiStartStopDeviceLogCallback(serial.c_str(), i_start);
}

/**
 * Registers a device log callback function. This callback will be called each time
 * that a module sends a new log message. Mostly useful to debug a Yoctopuce module.
 *
 * @param callback : the callback function to call, or a NULL pointer.
 *         The callback function should take two
 *         arguments: the module object that emitted the log message,
 *         and the character string containing the log.
 *         On failure, throws an exception or returns a negative error code.
 */
int YModule::registerLogCallback(YModuleLogCallback callback)
{
    string serial;

    serial = this->get_serialNumber();
    if (serial == YAPI_INVALID_STRING) {
        return YAPI_DEVICE_NOT_FOUND;
    }
    _logCallback = callback;
    this->_startStopDevLog(serial, callback != NULL);
    return 0;
}

YModuleLogCallback YModule::get_logCallback(void)
{
    return _logCallback;
}

/**
 * Register a callback function, to be called when a persistent settings in
 * a device configuration has been changed (e.g. change of unit, etc).
 *
 * @param callback : a procedure taking a YModule parameter, or NULL
 *         to unregister a previously registered  callback.
 */
int YModule::registerConfigChangeCallback(YModuleConfigChangeCallback callback)
{
    if (callback != NULL) {
        YModule::_updateModuleCallbackList(this, true);
    } else {
        YModule::_updateModuleCallbackList(this, false);
    }
    _confChangeCallback = callback;
    return 0;
}

int YModule::_invokeConfigChangeCallback(void)
{
    if (_confChangeCallback != NULL) {
        _confChangeCallback(this);
    }
    return 0;
}

/**
 * Register a callback function, to be called when the localization beacon of the module
 * has been changed. The callback function should take two arguments: the YModule object of
 * which the beacon has changed, and an integer describing the new beacon state.
 *
 * @param callback : The callback function to call, or NULL to unregister a
 *         previously registered callback.
 */
int YModule::registerBeaconCallback(YModuleBeaconCallback callback)
{
    if (callback != NULL) {
        YModule::_updateModuleCallbackList(this, true);
    } else {
        YModule::_updateModuleCallbackList(this, false);
    }
    _beaconCallback = callback;
    return 0;
}

int YModule::_invokeBeaconCallback(int beaconState)
{
    if (_beaconCallback != NULL) {
        _beaconCallback(this, beaconState);
    }
    return 0;
}

/**
 * Triggers a configuration change callback, to check if they are supported or not.
 */
int YModule::triggerConfigChangeCallback(void)
{
    this->_setAttr("persistentSettings", "2");
    return 0;
}

/**
 * Tests whether the byn file is valid for this module. This method is useful to test if the module
 * needs to be updated.
 * It is possible to pass a directory as argument instead of a file. In this case, this method returns
 * the path of the most recent
 * appropriate .byn file. If the parameter onlynew is true, the function discards firmwares that are older or
 * equal to the installed firmware.
 *
 * @param path : the path of a byn file or a directory that contains byn files
 * @param onlynew : returns only files that are strictly newer
 *
 * @return the path of the byn file to use or a empty string if no byn files matches the requirement
 *
 * On failure, throws an exception or returns a string that start with "error:".
 */
string YModule::checkFirmware(string path,bool onlynew)
{
    string serial;
    int release = 0;
    string tmp_res;
    if (onlynew) {
        release = atoi((this->get_firmwareRelease()).c_str());
    } else {
        release = 0;
    }
    //may throw an exception
    serial = this->get_serialNumber();
    tmp_res = YFirmwareUpdate::CheckFirmware(serial, path, release);
    if (_ystrpos(tmp_res, "error:") == 0) {
        this->_throw(YAPI_INVALID_ARGUMENT, tmp_res);
    }
    return tmp_res;
}

/**
 * Prepares a firmware update of the module. This method returns a YFirmwareUpdate object which
 * handles the firmware update process.
 *
 * @param path : the path of the .byn file to use.
 * @param force : true to force the firmware update even if some prerequisites appear not to be met
 *
 * @return a YFirmwareUpdate object or NULL on error.
 */
YFirmwareUpdate YModule::updateFirmwareEx(string path,bool force)
{
    string serial;
    string settings;

    serial = this->get_serialNumber();
    settings = this->get_allSettings();
    if ((int)(settings).size() == 0) {
        this->_throw(YAPI_IO_ERROR, "Unable to get device settings");
        settings = "error:Unable to get device settings";
    }
    return YFirmwareUpdate(serial, path, settings, force);
}

/**
 * Prepares a firmware update of the module. This method returns a YFirmwareUpdate object which
 * handles the firmware update process.
 *
 * @param path : the path of the .byn file to use.
 *
 * @return a YFirmwareUpdate object or NULL on error.
 */
YFirmwareUpdate YModule::updateFirmware(string path)
{
    return this->updateFirmwareEx(path, false);
}

/**
 * Returns all the settings and uploaded files of the module. Useful to backup all the
 * logical names, calibrations parameters, and uploaded files of a device.
 *
 * @return a binary buffer with all the settings.
 *
 * On failure, throws an exception or returns an binary object of size 0.
 */
string YModule::get_allSettings(void)
{
    string settings;
    string json;
    string res;
    string sep;
    string name;
    string item;
    string t_type;
    string pageid;
    string url;
    string file_data;
    string file_data_bin;
    string temp_data_bin;
    string ext_settings;
    vector<string> filelist;
    vector<string> templist;

    settings = this->_download("api.json");
    if ((int)(settings).size() == 0) {
        return settings;
    }
    ext_settings = ", \"extras\":[";
    templist = this->get_functionIds("Temperature");
    sep = "";
    for (unsigned ii = 0; ii < templist.size(); ii++) {
        if (atoi((this->get_firmwareRelease()).c_str()) > 9000) {
            url = YapiWrapper::ysprintf("api/%s/sensorType",templist[ii].c_str());
            t_type = this->_download(url);
            if (t_type == "RES_NTC" || t_type == "RES_LINEAR") {
                pageid = templist[ii].substr(11, (int)(templist[ii]).length() - 11);
                if (pageid == "") {
                    pageid = "1";
                }
                temp_data_bin = this->_download(YapiWrapper::ysprintf("extra.json?page=%s",pageid.c_str()));
                if ((int)(temp_data_bin).size() > 0) {
                    item = YapiWrapper::ysprintf("%s{\"fid\":\"%s\", \"json\":%s}\n",sep.c_str(),templist[ii].c_str(),temp_data_bin.c_str());
                    ext_settings = ext_settings + item;
                    sep = ",";
                }
            }
        }
    }
    ext_settings = ext_settings + "],\n\"files\":[";
    if (this->hasFunction("files")) {
        json = this->_download("files.json?a=dir&d=1&f=");
        if ((int)(json).size() == 0) {
            return json;
        }
        filelist = this->_json_get_array(json);
        sep = "";
        for (unsigned ii = 0; ii < filelist.size(); ii++) {
            name = this->_json_get_key(filelist[ii], "name");
            if (((int)(name).length() > 0) && !(name == "startupConf.json")) {
                if (name.substr((int)(name).length()-1, 1) == "/") {
                    file_data = "";
                } else {
                    file_data_bin = this->_download(this->_escapeAttr(name));
                    file_data = YAPI::_bin2HexStr(file_data_bin);
                }
                item = YapiWrapper::ysprintf("%s{\"name\":\"%s\", \"data\":\"%s\"}\n",sep.c_str(),name.c_str(),file_data.c_str());
                ext_settings = ext_settings + item;
                sep = ",";
            }
        }
    }
    res = "{ \"api\":" + settings + ext_settings + "]}";
    return res;
}

int YModule::loadThermistorExtra(string funcId,string jsonExtra)
{
    vector<string> values;
    string url;
    string curr;
    string binCurr;
    string currTemp;
    string binCurrTemp;
    int ofs = 0;
    int size = 0;
    url = "api/" + funcId + ".json?command=Z";

    this->_download(url);
    // add records in growing resistance value
    values = this->_json_get_array(jsonExtra);
    ofs = 0;
    size = (int)values.size();
    while (ofs + 1 < size) {
        binCurr = values[ofs];
        binCurrTemp = values[ofs + 1];
        curr = this->_json_get_string(binCurr);
        currTemp = this->_json_get_string(binCurrTemp);
        url = YapiWrapper::ysprintf("api/%s.json?command=m%s:%s",funcId.c_str(),curr.c_str(),currTemp.c_str());
        this->_download(url);
        ofs = ofs + 2;
    }
    return YAPI_SUCCESS;
}

int YModule::set_extraSettings(string jsonExtra)
{
    vector<string> extras;
    string tmp;
    string functionId;
    string data;
    extras = this->_json_get_array(jsonExtra);
    for (unsigned ii = 0; ii < extras.size(); ii++) {
        tmp = this->_get_json_path(extras[ii], "fid");
        functionId = this->_json_get_string(tmp);
        data = this->_get_json_path(extras[ii], "json");
        if (this->hasFunction(functionId)) {
            this->loadThermistorExtra(functionId, data);
        }
    }
    return YAPI_SUCCESS;
}

/**
 * Restores all the settings and uploaded files to the module.
 * This method is useful to restore all the logical names and calibrations parameters,
 * uploaded files etc. of a device from a backup.
 * Remember to call the saveToFlash() method of the module if the
 * modifications must be kept.
 *
 * @param settings : a binary buffer with all the settings.
 *
 * @return YAPI::SUCCESS when the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YModule::set_allSettingsAndFiles(string settings)
{
    string down;
    string json_bin;
    string json_api;
    string json_files;
    string json_extra;
    int fuperror = 0;
    int globalres = 0;
    fuperror = 0;
    json_api = this->_get_json_path(settings, "api");
    if ((int)(json_api).size() == 0) {
        return this->set_allSettings(settings);
    }
    json_extra = this->_get_json_path(settings, "extras");
    if ((int)(json_extra).size() > 0) {
        this->set_extraSettings(json_extra);
    }
    this->set_allSettings(json_api);
    if (this->hasFunction("files")) {
        vector<string> files;
        string res;
        string tmp;
        string name;
        string data;
        down = this->_download("files.json?a=format");
        down = this->_get_json_path(down, "res");
        res = this->_json_get_string(down);
        if (!(res == "ok")) {
            _throw((YRETCODE)(YAPI_IO_ERROR), "format failed");
            return YAPI_IO_ERROR;
        }
        json_files = this->_get_json_path(settings, "files");
        files = this->_json_get_array(json_files);
        for (unsigned ii = 0; ii < files.size(); ii++) {
            tmp = this->_get_json_path(files[ii], "name");
            name = this->_json_get_string(tmp);
            tmp = this->_get_json_path(files[ii], "data");
            data = this->_json_get_string(tmp);
            if (name == "") {
                fuperror = fuperror + 1;
            } else {
                this->_upload(name, YAPI::_hexStr2Bin(data));
            }
        }
    }
    // Apply settings a second time for file-dependent settings and dynamic sensor nodes
    globalres = this->set_allSettings(json_api);
    if (!(fuperror == 0)) {
        _throw((YRETCODE)(YAPI_IO_ERROR), "Error during file upload");
        return YAPI_IO_ERROR;
    }
    return globalres;
}

/**
 * Tests if the device includes a specific function. This method takes a function identifier
 * and returns a boolean.
 *
 * @param funcId : the requested function identifier
 *
 * @return true if the device has the function identifier
 */
bool YModule::hasFunction(string funcId)
{
    int count = 0;
    int i = 0;
    string fid;

    count = this->functionCount();
    i = 0;
    while (i < count) {
        fid = this->functionId(i);
        if (fid == funcId) {
            return true;
        }
        i = i + 1;
    }
    return false;
}

/**
 * Retrieve all hardware identifier that match the type passed in argument.
 *
 * @param funType : The type of function (Relay, LightSensor, Voltage,...)
 *
 * @return an array of strings.
 */
vector<string> YModule::get_functionIds(string funType)
{
    int count = 0;
    int i = 0;
    string ftype;
    vector<string> res;

    count = this->functionCount();
    i = 0;
    while (i < count) {
        ftype = this->functionType(i);
        if (ftype == funType) {
            res.push_back(this->functionId(i));
        } else {
            ftype = this->functionBaseType(i);
            if (ftype == funType) {
                res.push_back(this->functionId(i));
            }
        }
        i = i + 1;
    }
    return res;
}

string YModule::_flattenJsonStruct(string jsoncomplex)
{
    char errmsg[YOCTO_ERRMSG_LEN];
    char smallbuff[1024];
    char *bigbuff;
    int buffsize = 0;
    int fullsize = 0;
    int res = 0;
    string jsonflat;
    string jsoncomplexstr;
    fullsize = 0;
    jsoncomplexstr = jsoncomplex;
    res = yapiGetAllJsonKeys(jsoncomplexstr.c_str(), smallbuff, 1024, &fullsize, errmsg);
    if (res < 0) {
        this->_throw(YAPI_INVALID_ARGUMENT, string(errmsg));
        jsonflat = "error:" + string(errmsg);
        return jsonflat;
    }
    if (fullsize <= 1024) {
        jsonflat = string(smallbuff, fullsize);
    } else {
        fullsize = fullsize * 2;
        buffsize = fullsize;
        bigbuff = (char *)malloc(buffsize);
        res = yapiGetAllJsonKeys(jsoncomplexstr.c_str(), bigbuff, buffsize, &fullsize, errmsg);
        if (res < 0) {
            this->_throw(YAPI_INVALID_ARGUMENT, string(errmsg));
            jsonflat = "error:" + string(errmsg);
        } else {
            jsonflat = string(bigbuff, fullsize);
        }
        free(bigbuff);
    }
    return jsonflat;
}

int YModule::calibVersion(string cparams)
{
    if (cparams == "0,") {
        return 3;
    }
    if (_ystrpos(cparams, ",") >= 0) {
        if (_ystrpos(cparams, " ") > 0) {
            return 3;
        } else {
            return 1;
        }
    }
    if (cparams == "" || cparams == "0") {
        return 1;
    }
    if (((int)(cparams).length() < 2) || (_ystrpos(cparams, ".") >= 0)) {
        return 0;
    } else {
        return 2;
    }
}

int YModule::calibScale(string unit_name,string sensorType)
{
    if (unit_name == "g" || unit_name == "gauss" || unit_name == "W") {
        return 1000;
    }
    if (unit_name == "C") {
        if (sensorType == "") {
            return 16;
        }
        if (atoi((sensorType).c_str()) < 8) {
            return 16;
        } else {
            return 100;
        }
    }
    if (unit_name == "m" || unit_name == "deg") {
        return 10;
    }
    return 1;
}

int YModule::calibOffset(string unit_name)
{
    if (unit_name == "% RH" || unit_name == "mbar" || unit_name == "lx") {
        return 0;
    }
    return 32767;
}

string YModule::calibConvert(string param,string currentFuncValue,string unit_name,string sensorType)
{
    int paramVer = 0;
    int funVer = 0;
    int funScale = 0;
    int funOffset = 0;
    int paramScale = 0;
    int paramOffset = 0;
    vector<int> words;
    vector<string> words_str;
    vector<double> calibData;
    vector<int> iCalib;
    int calibType = 0;
    int i = 0;
    int maxSize = 0;
    double ratio = 0.0;
    int nPoints = 0;
    double wordVal = 0.0;
    // Initial guess for parameter encoding
    paramVer = this->calibVersion(param);
    funVer = this->calibVersion(currentFuncValue);
    funScale = this->calibScale(unit_name, sensorType);
    funOffset = this->calibOffset(unit_name);
    paramScale = funScale;
    paramOffset = funOffset;
    if (funVer < 3) {
        // Read the effective device scale if available
        if (funVer == 2) {
            words = YAPI::_decodeWords(currentFuncValue);
            if ((words[0] == 1366) && (words[1] == 12500)) {
                // Yocto-3D RefFrame used a special encoding
                funScale = 1;
                funOffset = 0;
            } else {
                funScale = words[1];
                funOffset = words[0];
            }
        } else {
            if (funVer == 1) {
                if (currentFuncValue == "" || (atoi((currentFuncValue).c_str()) > 10)) {
                    funScale = 0;
                }
            }
        }
    }
    calibData.clear();
    calibType = 0;
    if (paramVer < 3) {
        // Handle old 16 bit parameters formats
        if (paramVer == 2) {
            words = YAPI::_decodeWords(param);
            if ((words[0] == 1366) && (words[1] == 12500)) {
                // Yocto-3D RefFrame used a special encoding
                paramScale = 1;
                paramOffset = 0;
            } else {
                paramScale = words[1];
                paramOffset = words[0];
            }
            if (((int)words.size() >= 3) && (words[2] > 0)) {
                maxSize = 3 + 2 * ((words[2]) % (10));
                if (maxSize > (int)words.size()) {
                    maxSize = (int)words.size();
                }
                i = 3;
                while (i < maxSize) {
                    calibData.push_back((double) words[i]);
                    i = i + 1;
                }
            }
        } else {
            if (paramVer == 1) {
                words_str = _strsplit(param,',');
                for (unsigned ii = 0; ii < words_str.size(); ii++) {
                    words.push_back(atoi((words_str[ii]).c_str()));
                }
                if (param == "" || (words[0] > 10)) {
                    paramScale = 0;
                }
                if (((int)words.size() > 0) && (words[0] > 0)) {
                    maxSize = 1 + 2 * ((words[0]) % (10));
                    if (maxSize > (int)words.size()) {
                        maxSize = (int)words.size();
                    }
                    i = 1;
                    while (i < maxSize) {
                        calibData.push_back((double) words[i]);
                        i = i + 1;
                    }
                }
            } else {
                if (paramVer == 0) {
                    ratio = atof((param).c_str());
                    if (ratio > 0) {
                        calibData.push_back(0.0);
                        calibData.push_back(0.0);
                        calibData.push_back(floor(65535 / ratio+0.5));
                        calibData.push_back(65535.0);
                    }
                }
            }
        }
        i = 0;
        while (i < (int)calibData.size()) {
            if (paramScale > 0) {
                // scalar decoding
                calibData[i] = (calibData[i] - paramOffset) / paramScale;
            } else {
                // floating-point decoding
                calibData[i] = YAPI::_decimalToDouble((int) floor(calibData[i]+0.5));
            }
            i = i + 1;
        }
    } else {
        // Handle latest 32bit parameter format
        iCalib = YAPI::_decodeFloats(param);
        calibType = (int) floor(iCalib[0] / 1000.0+0.5);
        if (calibType >= 30) {
            calibType = calibType - 30;
        }
        i = 1;
        while (i < (int)iCalib.size()) {
            calibData.push_back(iCalib[i] / 1000.0);
            i = i + 1;
        }
    }
    if (funVer >= 3) {
        // Encode parameters in new format
        if ((int)calibData.size() == 0) {
            param = "0,";
        } else {
            param = YapiWrapper::ysprintf("%d",30 + calibType);
            i = 0;
            while (i < (int)calibData.size()) {
                if ((i & 1) > 0) {
                    param = param + ":";
                } else {
                    param = param + " ";
                }
                param = param + YapiWrapper::ysprintf("%d",(int) floor(calibData[i] * 1000.0 / 1000.0+0.5));
                i = i + 1;
            }
            param = param + ",";
        }
    } else {
        if (funVer >= 1) {
            // Encode parameters for older devices
            nPoints = ((int)calibData.size() / 2);
            param = YapiWrapper::ysprintf("%d",nPoints);
            i = 0;
            while (i < 2 * nPoints) {
                if (funScale == 0) {
                    wordVal = YAPI::_doubleToDecimal((int) floor(calibData[i]+0.5));
                } else {
                    wordVal = calibData[i] * funScale + funOffset;
                }
                param = param + "," + YapiWrapper::ysprintf("%f",floor(wordVal+0.5));
                i = i + 1;
            }
        } else {
            // Initial V0 encoding used for old Yocto-Light
            if ((int)calibData.size() == 4) {
                param = YapiWrapper::ysprintf("%f",floor(1000 * (calibData[3] - calibData[1]) / calibData[2] - calibData[0]+0.5));
            }
        }
    }
    return param;
}

int YModule::_tryExec(string url)
{
    int res = 0;
    int done = 0;
    res = YAPI_SUCCESS;
    done = 1;
    try {
        this->_download(url);
    } catch (std::exception&) {
        done = 0;
    }
    if (done == 0) {
        // retry silently after a short wait
        try {
            {string ignore_error; YAPI::Sleep(500, ignore_error);};
            this->_download(url);
        } catch (std::exception&) {
            // second failure, return error code
            res = this->get_errorType();
        }
    }
    return res;
}

/**
 * Restores all the settings of the device. Useful to restore all the logical names and calibrations parameters
 * of a module from a backup.Remember to call the saveToFlash() method of the module if the
 * modifications must be kept.
 *
 * @param settings : a binary buffer with all the settings.
 *
 * @return YAPI::SUCCESS when the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YModule::set_allSettings(string settings)
{
    vector<string> restoreLast;
    string old_json_flat;
    vector<string> old_dslist;
    vector<string> old_jpath;
    vector<int> old_jpath_len;
    vector<string> old_val_arr;
    string actualSettings;
    vector<string> new_dslist;
    vector<string> new_jpath;
    vector<int> new_jpath_len;
    vector<string> new_val_arr;
    int cpos = 0;
    int eqpos = 0;
    int leng = 0;
    int i = 0;
    int j = 0;
    int subres = 0;
    int res = 0;
    string njpath;
    string jpath;
    string fun;
    string attr;
    string value;
    string old_serial;
    string new_serial;
    string url;
    string tmp;
    string binTmp;
    string sensorType;
    string unit_name;
    string newval;
    string oldval;
    string old_calib;
    string each_str;
    bool do_update = 0;
    bool found = 0;
    res = YAPI_SUCCESS;
    binTmp = this->_get_json_path(settings, "api");
    if ((int)(binTmp).size() > 0) {
        settings = binTmp;
    }
    old_serial = "";
    oldval = "";
    newval = "";
    old_json_flat = this->_flattenJsonStruct(settings);
    old_dslist = this->_json_get_array(old_json_flat);
    for (unsigned ii = 0; ii < old_dslist.size(); ii++) {
        each_str = this->_json_get_string(old_dslist[ii]);
        // split json path and attr
        leng = (int)(each_str).length();
        eqpos = _ystrpos(each_str, "=");
        if ((eqpos < 0) || (leng == 0)) {
            this->_throw(YAPI_INVALID_ARGUMENT, "Invalid settings");
            return YAPI_INVALID_ARGUMENT;
        }
        jpath = each_str.substr(0, eqpos);
        eqpos = eqpos + 1;
        value = each_str.substr(eqpos, leng - eqpos);
        old_jpath.push_back(jpath);
        old_jpath_len.push_back((int)(jpath).length());
        old_val_arr.push_back(value);
        if (jpath == "module/serialNumber") {
            old_serial = value;
        }
    }

    try {
        actualSettings = this->_download("api.json");
    } catch (std::exception&) {
        // retry silently after a short wait
        {string ignore_error; YAPI::Sleep(500, ignore_error);};
        actualSettings = this->_download("api.json");
    }
    new_serial = this->get_serialNumber();
    if (old_serial == new_serial || old_serial == "") {
        old_serial = "_NO_SERIAL_FILTER_";
    }
    actualSettings = this->_flattenJsonStruct(actualSettings);
    new_dslist = this->_json_get_array(actualSettings);
    for (unsigned ii = 0; ii < new_dslist.size(); ii++) {
        // remove quotes
        each_str = this->_json_get_string(new_dslist[ii]);
        // split json path and attr
        leng = (int)(each_str).length();
        eqpos = _ystrpos(each_str, "=");
        if ((eqpos < 0) || (leng == 0)) {
            this->_throw(YAPI_INVALID_ARGUMENT, "Invalid settings");
            return YAPI_INVALID_ARGUMENT;
        }
        jpath = each_str.substr(0, eqpos);
        eqpos = eqpos + 1;
        value = each_str.substr(eqpos, leng - eqpos);
        new_jpath.push_back(jpath);
        new_jpath_len.push_back((int)(jpath).length());
        new_val_arr.push_back(value);
    }
    i = 0;
    while (i < (int)new_jpath.size()) {
        njpath = new_jpath[i];
        leng = (int)(njpath).length();
        cpos = _ystrpos(njpath, "/");
        if ((cpos < 0) || (leng == 0)) {
            continue;
        }
        fun = njpath.substr(0, cpos);
        cpos = cpos + 1;
        attr = njpath.substr(cpos, leng - cpos);
        do_update = true;
        if (fun == "services") {
            do_update = false;
        }
        if (do_update && (attr == "firmwareRelease")) {
            do_update = false;
        }
        if (do_update && (attr == "usbCurrent")) {
            do_update = false;
        }
        if (do_update && (attr == "upTime")) {
            do_update = false;
        }
        if (do_update && (attr == "persistentSettings")) {
            do_update = false;
        }
        if (do_update && (attr == "adminPassword")) {
            do_update = false;
        }
        if (do_update && (attr == "userPassword")) {
            do_update = false;
        }
        if (do_update && (attr == "rebootCountdown")) {
            do_update = false;
        }
        if (do_update && (attr == "advertisedValue")) {
            do_update = false;
        }
        if (do_update && (attr == "poeCurrent")) {
            do_update = false;
        }
        if (do_update && (attr == "readiness")) {
            do_update = false;
        }
        if (do_update && (attr == "ipAddress")) {
            do_update = false;
        }
        if (do_update && (attr == "subnetMask")) {
            do_update = false;
        }
        if (do_update && (attr == "router")) {
            do_update = false;
        }
        if (do_update && (attr == "linkQuality")) {
            do_update = false;
        }
        if (do_update && (attr == "ssid")) {
            do_update = false;
        }
        if (do_update && (attr == "channel")) {
            do_update = false;
        }
        if (do_update && (attr == "security")) {
            do_update = false;
        }
        if (do_update && (attr == "message")) {
            do_update = false;
        }
        if (do_update && (attr == "signalValue")) {
            do_update = false;
        }
        if (do_update && (attr == "currentValue")) {
            do_update = false;
        }
        if (do_update && (attr == "currentRawValue")) {
            do_update = false;
        }
        if (do_update && (attr == "currentRunIndex")) {
            do_update = false;
        }
        if (do_update && (attr == "pulseTimer")) {
            do_update = false;
        }
        if (do_update && (attr == "lastTimePressed")) {
            do_update = false;
        }
        if (do_update && (attr == "lastTimeReleased")) {
            do_update = false;
        }
        if (do_update && (attr == "filesCount")) {
            do_update = false;
        }
        if (do_update && (attr == "freeSpace")) {
            do_update = false;
        }
        if (do_update && (attr == "timeUTC")) {
            do_update = false;
        }
        if (do_update && (attr == "rtcTime")) {
            do_update = false;
        }
        if (do_update && (attr == "unixTime")) {
            do_update = false;
        }
        if (do_update && (attr == "dateTime")) {
            do_update = false;
        }
        if (do_update && (attr == "rawValue")) {
            do_update = false;
        }
        if (do_update && (attr == "lastMsg")) {
            do_update = false;
        }
        if (do_update && (attr == "delayedPulseTimer")) {
            do_update = false;
        }
        if (do_update && (attr == "rxCount")) {
            do_update = false;
        }
        if (do_update && (attr == "txCount")) {
            do_update = false;
        }
        if (do_update && (attr == "msgCount")) {
            do_update = false;
        }
        if (do_update && (attr == "rxMsgCount")) {
            do_update = false;
        }
        if (do_update && (attr == "txMsgCount")) {
            do_update = false;
        }
        if (do_update) {
            do_update = false;
            j = 0;
            found = false;
            newval = new_val_arr[i];
            while ((j < (int)old_jpath.size()) && !(found)) {
                if ((new_jpath_len[i] == old_jpath_len[j]) && (new_jpath[i] == old_jpath[j])) {
                    found = true;
                    oldval = old_val_arr[j];
                    if (!(newval == oldval) && !(oldval == old_serial)) {
                        do_update = true;
                    }
                }
                j = j + 1;
            }
        }
        if (do_update) {
            if (attr == "calibrationParam") {
                old_calib = "";
                unit_name = "";
                sensorType = "";
                j = 0;
                found = false;
                while ((j < (int)old_jpath.size()) && !(found)) {
                    if ((new_jpath_len[i] == old_jpath_len[j]) && (new_jpath[i] == old_jpath[j])) {
                        found = true;
                        old_calib = old_val_arr[j];
                    }
                    j = j + 1;
                }
                tmp = fun + "/unit";
                j = 0;
                found = false;
                while ((j < (int)new_jpath.size()) && !(found)) {
                    if (tmp == new_jpath[j]) {
                        found = true;
                        unit_name = new_val_arr[j];
                    }
                    j = j + 1;
                }
                tmp = fun + "/sensorType";
                j = 0;
                found = false;
                while ((j < (int)new_jpath.size()) && !(found)) {
                    if (tmp == new_jpath[j]) {
                        found = true;
                        sensorType = new_val_arr[j];
                    }
                    j = j + 1;
                }
                newval = this->calibConvert(old_calib, new_val_arr[i], unit_name, sensorType);
                url = "api/" + fun + ".json?" + attr + "=" + this->_escapeAttr(newval);
                subres = this->_tryExec(url);
                if ((res == YAPI_SUCCESS) && (subres != YAPI_SUCCESS)) {
                    res = subres;
                }
            } else {
                url = "api/" + fun + ".json?" + attr + "=" + this->_escapeAttr(oldval);
                if (attr == "resolution") {
                    restoreLast.push_back(url);
                } else {
                    subres = this->_tryExec(url);
                    if ((res == YAPI_SUCCESS) && (subres != YAPI_SUCCESS)) {
                        res = subres;
                    }
                }
            }
        }
        i = i + 1;
    }
    for (unsigned ii = 0; ii < restoreLast.size(); ii++) {
        subres = this->_tryExec(restoreLast[ii]);
        if ((res == YAPI_SUCCESS) && (subres != YAPI_SUCCESS)) {
            res = subres;
        }
    }
    this->clearCache();
    return res;
}

/**
 * Adds a file to the uploaded data at the next HTTP callback.
 * This function only affects the next HTTP callback and only works in
 * HTTP callback mode.
 *
 * @param filename : the name of the file to upload at the next HTTP callback
 *
 * @return nothing.
 */
int YModule::addFileToHTTPCallback(string filename)
{
    string content;

    content = this->_download("@YCB+" + filename);
    if ((int)(content).size() == 0) {
        return YAPI_NOT_SUPPORTED;
    }
    return YAPI_SUCCESS;
}

/**
 * Returns the unique hardware identifier of the module.
 * The unique hardware identifier is made of the device serial
 * number followed by string ".module".
 *
 * @return a string that uniquely identifies the module
 */
string YModule::get_hardwareId(void)
{
    string serial;

    serial = this->get_serialNumber();
    return serial + ".module";
}

/**
 * Downloads the specified built-in file and returns a binary buffer with its content.
 *
 * @param pathname : name of the new file to load
 *
 * @return a binary buffer with the file content
 *
 * On failure, throws an exception or returns  YAPI::INVALID_STRING.
 */
string YModule::download(string pathname)
{
    return this->_download(pathname);
}

/**
 * Returns the icon of the module. The icon is a PNG image and does not
 * exceed 1536 bytes.
 *
 * @return a binary buffer with module icon, in png format.
 *         On failure, throws an exception or returns  YAPI::INVALID_STRING.
 */
string YModule::get_icon2d(void)
{
    return this->_download("icon2d.png");
}

/**
 * Returns a string with last logs of the module. This method return only
 * logs that are still in the module.
 *
 * @return a string with last logs of the module.
 *         On failure, throws an exception or returns  YAPI::INVALID_STRING.
 */
string YModule::get_lastLogs(void)
{
    string content;

    content = this->_download("logs.txt");
    return content;
}

/**
 * Adds a text message to the device logs. This function is useful in
 * particular to trace the execution of HTTP callbacks. If a newline
 * is desired after the message, it must be included in the string.
 *
 * @param text : the string to append to the logs.
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YModule::log(string text)
{
    return this->_upload("logs.txt", text);
}

/**
 * Returns a list of all the modules that are plugged into the current module.
 * This method only makes sense when called for a YoctoHub/VirtualHub.
 * Otherwise, an empty array will be returned.
 *
 * @return an array of strings containing the sub modules.
 */
vector<string> YModule::get_subDevices(void)
{
    char errmsg[YOCTO_ERRMSG_LEN];
    char smallbuff[1024];
    char *bigbuff;
    int buffsize = 0;
    int fullsize = 0;
    int yapi_res = 0;
    string subdevice_list;
    vector<string> subdevices;
    string serial;

    serial = this->get_serialNumber();
    fullsize = 0;
    yapi_res = yapiGetSubdevices(serial.c_str(), smallbuff, 1024, &fullsize, errmsg);
    if (yapi_res < 0) {
        return subdevices;
    }
    if (fullsize <= 1024) {
        subdevice_list = string(smallbuff, yapi_res);
    } else {
        buffsize = fullsize;
        bigbuff = (char *)malloc(buffsize);
        yapi_res = yapiGetSubdevices(serial.c_str(), bigbuff, buffsize, &fullsize, errmsg);
        if (yapi_res < 0) {
            free(bigbuff);
            return subdevices;
        } else {
            subdevice_list = string(bigbuff, yapi_res);
        }
        free(bigbuff);
    }
    if (!(subdevice_list == "")) {
        subdevices = _strsplit(subdevice_list,',');
    }
    return subdevices;
}

/**
 * Returns the serial number of the YoctoHub on which this module is connected.
 * If the module is connected by USB, or if the module is the root YoctoHub, an
 * empty string is returned.
 *
 * @return a string with the serial number of the YoctoHub or an empty string
 */
string YModule::get_parentHub(void)
{
    char errmsg[YOCTO_ERRMSG_LEN];
    char hubserial[YOCTO_SERIAL_LEN];
    int pathsize = 0;
    int yapi_res = 0;
    string serial;

    serial = this->get_serialNumber();
    // retrieve device object
    pathsize = 0;
    yapi_res = yapiGetDevicePathEx(serial.c_str(), hubserial, NULL, 0, &pathsize, errmsg);
    if (yapi_res < 0) {
        return "";
    }
    return string(hubserial);
}

/**
 * Returns the URL used to access the module. If the module is connected by USB, the
 * string 'usb' is returned.
 *
 * @return a string with the URL of the module.
 */
string YModule::get_url(void)
{
    char errmsg[YOCTO_ERRMSG_LEN];
    char path[1024];
    int pathsize = 0;
    int yapi_res = 0;
    string serial;

    serial = this->get_serialNumber();
    // retrieve device object
    pathsize = 0;
    yapi_res = yapiGetDevicePathEx(serial.c_str(), NULL, path, 1024, &pathsize, errmsg);
    if (yapi_res < 0) {
        return "";
    }
    return string(path);
}

YModule *YModule::nextModule(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YModule::FindModule(hwid);
}

YModule *YModule::FirstModule(void)
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
    YFUN_DESCR fundescr, moddescr;
    YDEV_DESCR devdescr;
    string errmsg, serial, funcId, funcName, funcValue;
    string mod_serial, mod_funcId, mod_funcname;

    yEnterCriticalSection(&_this_cs);
    fundescr = YapiWrapper::getFunction(_className, _func, errmsg);
    if (!YISERR(fundescr) && !YISERR(YapiWrapper::getFunctionInfo(fundescr, devdescr, serial, funcId, funcName, funcValue, errmsg))) {
        moddescr = YapiWrapper::getFunction("Module", serial, errmsg);
        if (!YISERR(moddescr) && !YISERR(YapiWrapper::getFunctionInfo(moddescr, devdescr, mod_serial, mod_funcId, mod_funcname, funcValue, errmsg))) {
            if (mod_funcname != "") {
                yLeaveCriticalSection(&_this_cs);
                return mod_funcname;
            }
        }
        yLeaveCriticalSection(&_this_cs);
        return serial;
    }
    yLeaveCriticalSection(&_this_cs);
    return Y_FRIENDLYNAME_INVALID;
}


void YModule::setImmutableAttributes(yDeviceSt *infos)
{
    // do not take CS on purpose (called for update device list)
    _serialNumber = (string)infos->serial;
    _productName = (string)infos->productname;
    _productId = infos->deviceid;
    _cacheExpiration = YAPI::GetTickCount();
}


// Return the properties of the nth function of our device
YRETCODE YModule::_getFunction(int idx, string &serial, string &funcId, string &baseType, string &funcName, string &funcVal, string &errmsg)
{
    vector<YFUN_DESCR> *functions;
    YDevice *dev;
    int res;
    YFUN_DESCR fundescr;
    YDEV_DESCR devdescr;

    // retrieve device object
    res = _getDevice(dev, errmsg);
    if (YISERR(res)) {
        _throw((YRETCODE)res, errmsg);
        return (YRETCODE)res;
    }

    // get reference to all functions from the device
    res = dev->getFunctions(&functions, errmsg);
    if (YISERR(res)) return (YRETCODE)res;

    // get latest function info from yellow pages
    fundescr = functions->at(idx);
    res = YapiWrapper::getFunctionInfoEx(fundescr, devdescr, serial, funcId, baseType, funcName, funcVal, errmsg);
    if (YISERR(res)) return (YRETCODE)res;

    return YAPI_SUCCESS;
}

// Retrieve the number of functions (beside "module") in the device
int YModule::functionCount()
{
    vector<YFUN_DESCR> *functions;
    YDevice *dev;
    string errmsg;
    int res;

    yEnterCriticalSection(&_this_cs);
    res = _getDevice(dev, errmsg);
    if (YISERR(res)) {
        yLeaveCriticalSection(&_this_cs);
        _throw((YRETCODE)res, errmsg);
        return (YRETCODE)res;
    }
    res = dev->getFunctions(&functions, errmsg);
    if (YISERR(res)) {
        yLeaveCriticalSection(&_this_cs);
        _throw((YRETCODE)res, errmsg);
        return (YRETCODE)res;
    }
    yLeaveCriticalSection(&_this_cs);
    return (int)functions->size();
}

// Retrieve the Id of the nth function (beside "module") in the device
string YModule::functionId(int functionIndex)
{
    string serial, funcId, funcName, basetype, funcVal, errmsg;
    int res;

    yEnterCriticalSection(&_this_cs);
    try {
        res = _getFunction(functionIndex, serial, funcId, basetype, funcName, funcVal, errmsg);
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    if (YISERR(res)) {
        _throw((YRETCODE)res, errmsg);
        return YAPI_INVALID_STRING;
    }
    return funcId;
}

// Retrieve the logical name of the nth function (beside "module") in the device
string YModule::functionName(int functionIndex)
{
    string serial, funcId, basetype, funcName, funcVal, errmsg;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        res = _getFunction(functionIndex, serial, funcId, basetype, funcName, funcVal, errmsg);
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    if (YISERR(res)) {
        _throw((YRETCODE)res, errmsg);
        return YAPI_INVALID_STRING;
    }

    return funcName;
}

// Retrieve the advertised value of the nth function (beside "module") in the device
string YModule::functionValue(int functionIndex)
{
    string serial, funcId, basetype, funcName, funcVal, errmsg;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        res = _getFunction(functionIndex, serial, funcId, basetype, funcName, funcVal, errmsg);
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    if (YISERR(res)) {
        _throw((YRETCODE)res, errmsg);
        return YAPI_INVALID_STRING;
    }

    return funcVal;
}


// Retrieve the Id of the nth function (beside "module") in the device
string YModule::functionType(int functionIndex)
{
    string serial, funcId, basetype, funcName, funcVal, errmsg;
    char buffer[YOCTO_FUNCTION_LEN], *d = buffer;
    const char *p;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        res = _getFunction(functionIndex, serial, funcId, basetype, funcName, funcVal, errmsg);
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    if (YISERR(res)) {
        _throw((YRETCODE)res, errmsg);
        return YAPI_INVALID_STRING;
    }
    p = funcId.c_str();
    *d++ = *p++ & 0xdf;
    while (*p) {
        *d++ = *p++;
    }
    while (d > buffer && d[-1] <= '9') d--;
    *d = 0;
    return string(buffer);
}

// Retrieve the Id of the nth function (beside "module") in the device
string YModule::functionBaseType(int functionIndex)
{
    string serial, funcId, basetype, funcName, funcVal, errmsg;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        res = _getFunction(functionIndex, serial, funcId, basetype, funcName, funcVal, errmsg);
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    if (YISERR(res)) {
        _throw((YRETCODE)res, errmsg);
        return YAPI_INVALID_STRING;
    }

    return basetype;
}


//--- (generated code: YModule functions)
//--- (end of generated code: YModule functions)


YSensor::YSensor(const string &func): YFunction(func)
                                      //--- (generated code: YSensor initialization)
    ,_unit(UNIT_INVALID)
    ,_currentValue(CURRENTVALUE_INVALID)
    ,_lowestValue(LOWESTVALUE_INVALID)
    ,_highestValue(HIGHESTVALUE_INVALID)
    ,_currentRawValue(CURRENTRAWVALUE_INVALID)
    ,_logFrequency(LOGFREQUENCY_INVALID)
    ,_reportFrequency(REPORTFREQUENCY_INVALID)
    ,_advMode(ADVMODE_INVALID)
    ,_calibrationParam(CALIBRATIONPARAM_INVALID)
    ,_resolution(RESOLUTION_INVALID)
    ,_sensorState(SENSORSTATE_INVALID)
    ,_valueCallbackSensor(NULL)
    ,_timedReportCallbackSensor(NULL)
    ,_prevTimedReport(0.0)
    ,_iresol(0.0)
    ,_offset(0.0)
    ,_scale(0.0)
    ,_decexp(0.0)
    ,_caltyp(0)
//--- (end of generated code: YSensor initialization)
{
    _className = "Sensor";
}

YSensor::~YSensor()
{
    //--- (generated code: YSensor cleanup)
//--- (end of generated code: YSensor cleanup)
}


//--- (generated code: YSensor implementation)
// static attributes
const string YSensor::UNIT_INVALID = YAPI_INVALID_STRING;
const double YSensor::CURRENTVALUE_INVALID = YAPI_INVALID_DOUBLE;
const double YSensor::LOWESTVALUE_INVALID = YAPI_INVALID_DOUBLE;
const double YSensor::HIGHESTVALUE_INVALID = YAPI_INVALID_DOUBLE;
const double YSensor::CURRENTRAWVALUE_INVALID = YAPI_INVALID_DOUBLE;
const string YSensor::LOGFREQUENCY_INVALID = YAPI_INVALID_STRING;
const string YSensor::REPORTFREQUENCY_INVALID = YAPI_INVALID_STRING;
const string YSensor::CALIBRATIONPARAM_INVALID = YAPI_INVALID_STRING;
const double YSensor::RESOLUTION_INVALID = YAPI_INVALID_DOUBLE;

int YSensor::_parseAttr(YJSONObject *json_val)
{
    if(json_val->has("unit")) {
        _unit =  json_val->getString("unit");
    }
    if(json_val->has("currentValue")) {
        _currentValue =  floor(json_val->getDouble("currentValue") / 65.536 + 0.5) / 1000.0;
    }
    if(json_val->has("lowestValue")) {
        _lowestValue =  floor(json_val->getDouble("lowestValue") / 65.536 + 0.5) / 1000.0;
    }
    if(json_val->has("highestValue")) {
        _highestValue =  floor(json_val->getDouble("highestValue") / 65.536 + 0.5) / 1000.0;
    }
    if(json_val->has("currentRawValue")) {
        _currentRawValue =  floor(json_val->getDouble("currentRawValue") / 65.536 + 0.5) / 1000.0;
    }
    if(json_val->has("logFrequency")) {
        _logFrequency =  json_val->getString("logFrequency");
    }
    if(json_val->has("reportFrequency")) {
        _reportFrequency =  json_val->getString("reportFrequency");
    }
    if(json_val->has("advMode")) {
        _advMode =  (Y_ADVMODE_enum)json_val->getInt("advMode");
    }
    if(json_val->has("calibrationParam")) {
        _calibrationParam =  json_val->getString("calibrationParam");
    }
    if(json_val->has("resolution")) {
        _resolution =  floor(json_val->getDouble("resolution") / 65.536 + 0.5) / 1000.0;
    }
    if(json_val->has("sensorState")) {
        _sensorState =  json_val->getInt("sensorState");
    }
    return YFunction::_parseAttr(json_val);
}


/**
 * Returns the measuring unit for the measure.
 *
 * @return a string corresponding to the measuring unit for the measure
 *
 * On failure, throws an exception or returns YSensor::UNIT_INVALID.
 */
string YSensor::get_unit(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSensor::UNIT_INVALID;
                }
            }
        }
        res = _unit;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the current value of the measure, in the specified unit, as a floating point number.
 * Note that a get_currentValue() call will *not* start a measure in the device, it
 * will just return the last measure that occurred in the device. Indeed, internally, each Yoctopuce
 * devices is continuously making measures at a hardware specific frequency.
 *
 * If continuously calling  get_currentValue() leads you to performances issues, then
 * you might consider to switch to callback programming model. Check the "advanced
 * programming" chapter in in your device user manual for more information.
 *
 * @return a floating point number corresponding to the current value of the measure, in the specified
 * unit, as a floating point number
 *
 * On failure, throws an exception or returns YSensor::CURRENTVALUE_INVALID.
 */
double YSensor::get_currentValue(void)
{
    double res = 0.0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSensor::CURRENTVALUE_INVALID;
                }
            }
        }
        res = this->_applyCalibration(_currentRawValue);
        if (res == YSensor::CURRENTVALUE_INVALID) {
            res = _currentValue;
        }
        res = res * _iresol;
        res = floor(res+0.5) / _iresol;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the recorded minimal value observed. Can be used to reset the value returned
 * by get_lowestValue().
 *
 * @param newval : a floating point number corresponding to the recorded minimal value observed
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YSensor::set_lowestValue(double newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%" FMTs64, (s64)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
        res = _setAttr("lowestValue", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the minimal value observed for the measure since the device was started.
 * Can be reset to an arbitrary value thanks to set_lowestValue().
 *
 * @return a floating point number corresponding to the minimal value observed for the measure since
 * the device was started
 *
 * On failure, throws an exception or returns YSensor::LOWESTVALUE_INVALID.
 */
double YSensor::get_lowestValue(void)
{
    double res = 0.0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSensor::LOWESTVALUE_INVALID;
                }
            }
        }
        res = _lowestValue * _iresol;
        res = floor(res+0.5) / _iresol;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the recorded maximal value observed. Can be used to reset the value returned
 * by get_lowestValue().
 *
 * @param newval : a floating point number corresponding to the recorded maximal value observed
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YSensor::set_highestValue(double newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%" FMTs64, (s64)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
        res = _setAttr("highestValue", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the maximal value observed for the measure since the device was started.
 * Can be reset to an arbitrary value thanks to set_highestValue().
 *
 * @return a floating point number corresponding to the maximal value observed for the measure since
 * the device was started
 *
 * On failure, throws an exception or returns YSensor::HIGHESTVALUE_INVALID.
 */
double YSensor::get_highestValue(void)
{
    double res = 0.0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSensor::HIGHESTVALUE_INVALID;
                }
            }
        }
        res = _highestValue * _iresol;
        res = floor(res+0.5) / _iresol;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the uncalibrated, unrounded raw value returned by the
 * sensor, in the specified unit, as a floating point number.
 *
 * @return a floating point number corresponding to the uncalibrated, unrounded raw value returned by the
 *         sensor, in the specified unit, as a floating point number
 *
 * On failure, throws an exception or returns YSensor::CURRENTRAWVALUE_INVALID.
 */
double YSensor::get_currentRawValue(void)
{
    double res = 0.0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSensor::CURRENTRAWVALUE_INVALID;
                }
            }
        }
        res = _currentRawValue;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the datalogger recording frequency for this function, or "OFF"
 * when measures are not stored in the data logger flash memory.
 *
 * @return a string corresponding to the datalogger recording frequency for this function, or "OFF"
 *         when measures are not stored in the data logger flash memory
 *
 * On failure, throws an exception or returns YSensor::LOGFREQUENCY_INVALID.
 */
string YSensor::get_logFrequency(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSensor::LOGFREQUENCY_INVALID;
                }
            }
        }
        res = _logFrequency;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the datalogger recording frequency for this function.
 * The frequency can be specified as samples per second,
 * as sample per minute (for instance "15/m") or in samples per
 * hour (eg. "4/h"). To disable recording for this function, use
 * the value "OFF". Note that setting the  datalogger recording frequency
 * to a greater value than the sensor native sampling frequency is useless,
 * and even counterproductive: those two frequencies are not related.
 * Remember to call the saveToFlash() method of the module if the modification must be kept.
 *
 * @param newval : a string corresponding to the datalogger recording frequency for this function
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YSensor::set_logFrequency(const string& newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = newval;
        res = _setAttr("logFrequency", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the timed value notification frequency, or "OFF" if timed
 * value notifications are disabled for this function.
 *
 * @return a string corresponding to the timed value notification frequency, or "OFF" if timed
 *         value notifications are disabled for this function
 *
 * On failure, throws an exception or returns YSensor::REPORTFREQUENCY_INVALID.
 */
string YSensor::get_reportFrequency(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSensor::REPORTFREQUENCY_INVALID;
                }
            }
        }
        res = _reportFrequency;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the timed value notification frequency for this function.
 * The frequency can be specified as samples per second,
 * as sample per minute (for instance "15/m") or in samples per
 * hour (e.g. "4/h"). To disable timed value notifications for this
 * function, use the value "OFF". Note that setting the  timed value
 * notification frequency to a greater value than the sensor native
 * sampling frequency is unless, and even counterproductive: those two
 * frequencies are not related.
 * Remember to call the saveToFlash() method of the module if the modification must be kept.
 *
 * @param newval : a string corresponding to the timed value notification frequency for this function
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YSensor::set_reportFrequency(const string& newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = newval;
        res = _setAttr("reportFrequency", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the measuring mode used for the advertised value pushed to the parent hub.
 *
 * @return a value among YSensor::ADVMODE_IMMEDIATE, YSensor::ADVMODE_PERIOD_AVG,
 * YSensor::ADVMODE_PERIOD_MIN and YSensor::ADVMODE_PERIOD_MAX corresponding to the measuring mode used
 * for the advertised value pushed to the parent hub
 *
 * On failure, throws an exception or returns YSensor::ADVMODE_INVALID.
 */
Y_ADVMODE_enum YSensor::get_advMode(void)
{
    Y_ADVMODE_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSensor::ADVMODE_INVALID;
                }
            }
        }
        res = _advMode;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the measuring mode used for the advertised value pushed to the parent hub.
 * Remember to call the saveToFlash() method of the module if the modification must be kept.
 *
 * @param newval : a value among YSensor::ADVMODE_IMMEDIATE, YSensor::ADVMODE_PERIOD_AVG,
 * YSensor::ADVMODE_PERIOD_MIN and YSensor::ADVMODE_PERIOD_MAX corresponding to the measuring mode used
 * for the advertised value pushed to the parent hub
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YSensor::set_advMode(Y_ADVMODE_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("advMode", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

string YSensor::get_calibrationParam(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSensor::CALIBRATIONPARAM_INVALID;
                }
            }
        }
        res = _calibrationParam;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

int YSensor::set_calibrationParam(const string& newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = newval;
        res = _setAttr("calibrationParam", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the resolution of the measured physical values. The resolution corresponds to the numerical precision
 * when displaying value. It does not change the precision of the measure itself.
 * Remember to call the saveToFlash() method of the module if the modification must be kept.
 *
 * @param newval : a floating point number corresponding to the resolution of the measured physical values
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YSensor::set_resolution(double newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%" FMTs64, (s64)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
        res = _setAttr("resolution", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the resolution of the measured values. The resolution corresponds to the numerical precision
 * of the measures, which is not always the same as the actual precision of the sensor.
 * Remember to call the saveToFlash() method of the module if the modification must be kept.
 *
 * @return a floating point number corresponding to the resolution of the measured values
 *
 * On failure, throws an exception or returns YSensor::RESOLUTION_INVALID.
 */
double YSensor::get_resolution(void)
{
    double res = 0.0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSensor::RESOLUTION_INVALID;
                }
            }
        }
        res = _resolution;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the sensor state code, which is zero when there is an up-to-date measure
 * available or a positive code if the sensor is not able to provide a measure right now.
 *
 * @return an integer corresponding to the sensor state code, which is zero when there is an up-to-date measure
 *         available or a positive code if the sensor is not able to provide a measure right now
 *
 * On failure, throws an exception or returns YSensor::SENSORSTATE_INVALID.
 */
int YSensor::get_sensorState(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YSensor::SENSORSTATE_INVALID;
                }
            }
        }
        res = _sensorState;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Retrieves a sensor for a given identifier.
 * The identifier can be specified using several formats:
 *
 * - FunctionLogicalName
 * - ModuleSerialNumber.FunctionIdentifier
 * - ModuleSerialNumber.FunctionLogicalName
 * - ModuleLogicalName.FunctionIdentifier
 * - ModuleLogicalName.FunctionLogicalName
 *
 *
 * This function does not require that the sensor is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the sensor is
 * indeed online at a given time. In case of ambiguity when looking for
 * a sensor by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the sensor, for instance
 *         MyDevice..
 *
 * @return a YSensor object allowing you to drive the sensor.
 */
YSensor* YSensor::FindSensor(string func)
{
    YSensor* obj = NULL;
    int taken = 0;
    if (YAPI::_apiInitialized) {
        yEnterCriticalSection(&YAPI::_global_cs);
        taken = 1;
    }try {
        obj = (YSensor*) YFunction::_FindFromCache("Sensor", func);
        if (obj == NULL) {
            obj = new YSensor(func);
            YFunction::_AddToCache("Sensor", func, obj);
        }
    } catch (std::exception &) {
        if (taken) yLeaveCriticalSection(&YAPI::_global_cs);
        throw;
    }
    if (taken) yLeaveCriticalSection(&YAPI::_global_cs);
    return obj;
}

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
int YSensor::registerValueCallback(YSensorValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackSensor = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YSensor::_invokeValueCallback(string value)
{
    if (_valueCallbackSensor != NULL) {
        _valueCallbackSensor(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

int YSensor::_parserHelper(void)
{
    int position = 0;
    int maxpos = 0;
    vector<int> iCalib;
    int iRaw = 0;
    int iRef = 0;
    double fRaw = 0.0;
    double fRef = 0.0;
    _caltyp = -1;
    _scale = -1;
    _calpar.clear();
    _calraw.clear();
    _calref.clear();
    // Store inverted resolution, to provide better rounding
    if (_resolution > 0) {
        _iresol = floor(1.0 / _resolution+0.5);
    } else {
        _iresol = 10000;
        _resolution = 0.0001;
    }
    // Old format: supported when there is no calibration
    if (_calibrationParam == "" || _calibrationParam == "0") {
        _caltyp = 0;
        return 0;
    }
    if (_ystrpos(_calibrationParam, ",") >= 0) {
        // Plain text format
        iCalib = YAPI::_decodeFloats(_calibrationParam);
        _caltyp = (iCalib[0] / 1000);
        if (_caltyp > 0) {
            if (_caltyp < YOCTO_CALIB_TYPE_OFS) {
                // Unknown calibration type: calibrated value will be provided by the device
                _caltyp = -1;
                return 0;
            }
            _calhdl = YAPI::_getCalibrationHandler(_caltyp);
            if (!(_calhdl != NULL)) {
                // Unknown calibration type: calibrated value will be provided by the device
                _caltyp = -1;
                return 0;
            }
        }
        // New 32 bits text format
        _offset = 0;
        _scale = 1000;
        maxpos = (int)iCalib.size();
        _calpar.clear();
        position = 1;
        while (position < maxpos) {
            _calpar.push_back(iCalib[position]);
            position = position + 1;
        }
        _calraw.clear();
        _calref.clear();
        position = 1;
        while (position + 1 < maxpos) {
            fRaw = iCalib[position];
            fRaw = fRaw / 1000.0;
            fRef = iCalib[position + 1];
            fRef = fRef / 1000.0;
            _calraw.push_back(fRaw);
            _calref.push_back(fRef);
            position = position + 2;
        }
    } else {
        // Recorder-encoded format, including encoding
        iCalib = YAPI::_decodeWords(_calibrationParam);
        // In case of unknown format, calibrated value will be provided by the device
        if ((int)iCalib.size() < 2) {
            _caltyp = -1;
            return 0;
        }
        // Save variable format (scale for scalar, or decimal exponent)
        _offset = 0;
        _scale = 1;
        _decexp = 1.0;
        position = iCalib[0];
        while (position > 0) {
            _decexp = _decexp * 10;
            position = position - 1;
        }
        // Shortcut when there is no calibration parameter
        if ((int)iCalib.size() == 2) {
            _caltyp = 0;
            return 0;
        }
        _caltyp = iCalib[2];
        _calhdl = YAPI::_getCalibrationHandler(_caltyp);
        // parse calibration points
        if (_caltyp <= 10) {
            maxpos = _caltyp;
        } else {
            if (_caltyp <= 20) {
                maxpos = _caltyp - 10;
            } else {
                maxpos = 5;
            }
        }
        maxpos = 3 + 2 * maxpos;
        if (maxpos > (int)iCalib.size()) {
            maxpos = (int)iCalib.size();
        }
        _calpar.clear();
        _calraw.clear();
        _calref.clear();
        position = 3;
        while (position + 1 < maxpos) {
            iRaw = iCalib[position];
            iRef = iCalib[position + 1];
            _calpar.push_back(iRaw);
            _calpar.push_back(iRef);
            _calraw.push_back(YAPI::_decimalToDouble(iRaw));
            _calref.push_back(YAPI::_decimalToDouble(iRef));
            position = position + 2;
        }
    }
    return 0;
}

/**
 * Checks if the sensor is currently able to provide an up-to-date measure.
 * Returns false if the device is unreachable, or if the sensor does not have
 * a current measure to transmit. No exception is raised if there is an error
 * while trying to contact the device hosting $THEFUNCTION$.
 *
 * @return true if the sensor can provide an up-to-date measure, and false otherwise
 */
bool YSensor::isSensorReady(void)
{
    if (!(this->isOnline())) {
        return false;
    }
    if (!(_sensorState == 0)) {
        return false;
    }
    return true;
}

/**
 * Returns the YDatalogger object of the device hosting the sensor. This method returns an object
 * that can control global parameters of the data logger. The returned object
 * should not be freed.
 *
 * @return an YDatalogger object, or NULL on error.
 */
YDataLogger* YSensor::get_dataLogger(void)
{
    YDataLogger* logger = NULL;
    YModule* modu = NULL;
    string serial;
    string hwid;

    modu = this->get_module();
    serial = modu->get_serialNumber();
    if (serial == YAPI_INVALID_STRING) {
        return NULL;
    }
    hwid = serial + ".dataLogger";
    logger = YDataLogger::FindDataLogger(hwid);
    return logger;
}

/**
 * Starts the data logger on the device. Note that the data logger
 * will only save the measures on this sensor if the logFrequency
 * is not set to "OFF".
 *
 * @return YAPI::SUCCESS if the call succeeds.
 */
int YSensor::startDataLogger(void)
{
    string res;

    res = this->_download("api/dataLogger/recording?recording=1");
    if (!((int)(res).size() > 0)) {
        _throw((YRETCODE)(YAPI_IO_ERROR), "unable to start datalogger");
        return YAPI_IO_ERROR;
    }
    return YAPI_SUCCESS;
}

/**
 * Stops the datalogger on the device.
 *
 * @return YAPI::SUCCESS if the call succeeds.
 */
int YSensor::stopDataLogger(void)
{
    string res;

    res = this->_download("api/dataLogger/recording?recording=0");
    if (!((int)(res).size() > 0)) {
        _throw((YRETCODE)(YAPI_IO_ERROR), "unable to stop datalogger");
        return YAPI_IO_ERROR;
    }
    return YAPI_SUCCESS;
}

/**
 * Retrieves a YDataSet object holding historical data for this
 * sensor, for a specified time interval. The measures will be
 * retrieved from the data logger, which must have been turned
 * on at the desired time. See the documentation of the YDataSet
 * class for information on how to get an overview of the
 * recorded data, and how to load progressively a large set
 * of measures from the data logger.
 *
 * This function only works if the device uses a recent firmware,
 * as YDataSet objects are not supported by firmwares older than
 * version 13000.
 *
 * @param startTime : the start of the desired measure time interval,
 *         as a Unix timestamp, i.e. the number of seconds since
 *         January 1, 1970 UTC. The special value 0 can be used
 *         to include any measure, without initial limit.
 * @param endTime : the end of the desired measure time interval,
 *         as a Unix timestamp, i.e. the number of seconds since
 *         January 1, 1970 UTC. The special value 0 can be used
 *         to include any measure, without ending limit.
 *
 * @return an instance of YDataSet, providing access to historical
 *         data. Past measures can be loaded progressively
 *         using methods from the YDataSet object.
 */
YDataSet YSensor::get_recordedData(double startTime,double endTime)
{
    string funcid;
    string funit;

    funcid = this->get_functionId();
    funit = this->get_unit();
    return YDataSet(this,funcid,funit,startTime,endTime);
}

/**
 * Registers the callback function that is invoked on every periodic timed notification.
 * The callback is invoked only during the execution of ySleep or yHandleEvents.
 * This provides control over the time when the callback is triggered. For good responsiveness, remember to call
 * one of these two functions periodically. To unregister a callback, pass a NULL pointer as argument.
 *
 * @param callback : the callback function to call, or a NULL pointer. The callback function should take two
 *         arguments: the function object of which the value has changed, and an YMeasure object describing
 *         the new advertised value.
 * @noreturn
 */
int YSensor::registerTimedReportCallback(YSensorTimedReportCallback callback)
{
    YSensor* sensor = NULL;
    sensor = this;
    if (callback != NULL) {
        YFunction::_UpdateTimedReportCallbackList(sensor, true);
    } else {
        YFunction::_UpdateTimedReportCallbackList(sensor, false);
    }
    _timedReportCallbackSensor = callback;
    return 0;
}

int YSensor::_invokeTimedReportCallback(YMeasure value)
{
    if (_timedReportCallbackSensor != NULL) {
        _timedReportCallbackSensor(this, value);
    } else {
    }
    return 0;
}

/**
 * Configures error correction data points, in particular to compensate for
 * a possible perturbation of the measure caused by an enclosure. It is possible
 * to configure up to five correction points. Correction points must be provided
 * in ascending order, and be in the range of the sensor. The device will automatically
 * perform a linear interpolation of the error correction between specified
 * points. Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * For more information on advanced capabilities to refine the calibration of
 * sensors, please contact support@yoctopuce.com.
 *
 * @param rawValues : array of floating point numbers, corresponding to the raw
 *         values returned by the sensor for the correction points.
 * @param refValues : array of floating point numbers, corresponding to the corrected
 *         values for the correction points.
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YSensor::calibrateFromPoints(vector<double> rawValues,vector<double> refValues)
{
    string rest_val;
    int res = 0;

    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = this->_encodeCalibrationPoints(rawValues, refValues);
        res = this->_setAttr("calibrationParam", rest_val);
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Retrieves error correction data points previously entered using the method
 * calibrateFromPoints.
 *
 * @param rawValues : array of floating point numbers, that will be filled by the
 *         function with the raw sensor values for the correction points.
 * @param refValues : array of floating point numbers, that will be filled by the
 *         function with the desired values for the correction points.
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YSensor::loadCalibrationPoints(vector<double>& rawValues,vector<double>& refValues)
{
    rawValues.clear();
    refValues.clear();
    // Load function parameters if not yet loaded
    yEnterCriticalSection(&_this_cs);
    try {
        if ((_scale == 0) || (_cacheExpiration <= YAPI::GetTickCount())) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YAPI_DEVICE_NOT_FOUND;
                }
            }
        }
        if (_caltyp < 0) {
            this->_throw(YAPI_NOT_SUPPORTED, "Calibration parameters format mismatch. Please upgrade your library or firmware.");
            {
                yLeaveCriticalSection(&_this_cs);
                return YAPI_NOT_SUPPORTED;
            }
        }
        rawValues.clear();
        refValues.clear();
        for (unsigned ii = 0; ii < _calraw.size(); ii++) {
            rawValues.push_back(_calraw[ii]);
        }
        for (unsigned ii = 0; ii < _calref.size(); ii++) {
            refValues.push_back(_calref[ii]);
        }
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return YAPI_SUCCESS;
}

string YSensor::_encodeCalibrationPoints(vector<double> rawValues,vector<double> refValues)
{
    string res;
    int npt = 0;
    int idx = 0;
    npt = (int)rawValues.size();
    if (npt != (int)refValues.size()) {
        this->_throw(YAPI_INVALID_ARGUMENT, "Invalid calibration parameters (size mismatch)");
        return YAPI_INVALID_STRING;
    }
    // Shortcut when building empty calibration parameters
    if (npt == 0) {
        return "0";
    }
    // Load function parameters if not yet loaded
    if (_scale == 0) {
        if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
            return YAPI_INVALID_STRING;
        }
    }
    // Detect old firmware
    if ((_caltyp < 0) || (_scale < 0)) {
        this->_throw(YAPI_NOT_SUPPORTED, "Calibration parameters format mismatch. Please upgrade your library or firmware.");
        return "0";
    }
    // 32-bit fixed-point encoding
    res = YapiWrapper::ysprintf("%d",YOCTO_CALIB_TYPE_OFS);
    idx = 0;
    while (idx < npt) {
        res = YapiWrapper::ysprintf("%s,%g,%g",res.c_str(),rawValues[idx],refValues[idx]);
        idx = idx + 1;
    }
    return res;
}

double YSensor::_applyCalibration(double rawValue)
{
    if (rawValue == Y_CURRENTVALUE_INVALID) {
        return Y_CURRENTVALUE_INVALID;
    }
    if (_caltyp == 0) {
        return rawValue;
    }
    if (_caltyp < 0) {
        return Y_CURRENTVALUE_INVALID;
    }
    if (!(_calhdl != NULL)) {
        return Y_CURRENTVALUE_INVALID;
    }
    return _calhdl(rawValue, _caltyp, _calpar, _calraw, _calref);
}

YMeasure YSensor::_decodeTimedReport(double timestamp,double duration,vector<int> report)
{
    int i = 0;
    int byteVal = 0;
    double poww = 0.0;
    double minRaw = 0.0;
    double avgRaw = 0.0;
    double maxRaw = 0.0;
    int sublen = 0;
    double difRaw = 0.0;
    double startTime = 0.0;
    double endTime = 0.0;
    double minVal = 0.0;
    double avgVal = 0.0;
    double maxVal = 0.0;
    if (duration > 0) {
        startTime = timestamp - duration;
    } else {
        startTime = _prevTimedReport;
    }
    endTime = timestamp;
    _prevTimedReport = endTime;
    if (startTime == 0) {
        startTime = endTime;
    }
    // 32 bits timed report format
    if ((int)report.size() <= 5) {
        // sub-second report, 1-4 bytes
        poww = 1;
        avgRaw = 0;
        byteVal = 0;
        i = 1;
        while (i < (int)report.size()) {
            byteVal = report[i];
            avgRaw = avgRaw + poww * byteVal;
            poww = poww * 0x100;
            i = i + 1;
        }
        if ((byteVal & 0x80) != 0) {
            avgRaw = avgRaw - poww;
        }
        avgVal = avgRaw / 1000.0;
        if (_caltyp != 0) {
            if (_calhdl != NULL) {
                avgVal = _calhdl(avgVal, _caltyp, _calpar, _calraw, _calref);
            }
        }
        minVal = avgVal;
        maxVal = avgVal;
    } else {
        // averaged report: avg,avg-min,max-avg
        sublen = 1 + (report[1] & 3);
        poww = 1;
        avgRaw = 0;
        byteVal = 0;
        i = 2;
        while ((sublen > 0) && (i < (int)report.size())) {
            byteVal = report[i];
            avgRaw = avgRaw + poww * byteVal;
            poww = poww * 0x100;
            i = i + 1;
            sublen = sublen - 1;
        }
        if ((byteVal & 0x80) != 0) {
            avgRaw = avgRaw - poww;
        }
        sublen = 1 + ((report[1] >> 2) & 3);
        poww = 1;
        difRaw = 0;
        while ((sublen > 0) && (i < (int)report.size())) {
            byteVal = report[i];
            difRaw = difRaw + poww * byteVal;
            poww = poww * 0x100;
            i = i + 1;
            sublen = sublen - 1;
        }
        minRaw = avgRaw - difRaw;
        sublen = 1 + ((report[1] >> 4) & 3);
        poww = 1;
        difRaw = 0;
        while ((sublen > 0) && (i < (int)report.size())) {
            byteVal = report[i];
            difRaw = difRaw + poww * byteVal;
            poww = poww * 0x100;
            i = i + 1;
            sublen = sublen - 1;
        }
        maxRaw = avgRaw + difRaw;
        avgVal = avgRaw / 1000.0;
        minVal = minRaw / 1000.0;
        maxVal = maxRaw / 1000.0;
        if (_caltyp != 0) {
            if (_calhdl != NULL) {
                avgVal = _calhdl(avgVal, _caltyp, _calpar, _calraw, _calref);
                minVal = _calhdl(minVal, _caltyp, _calpar, _calraw, _calref);
                maxVal = _calhdl(maxVal, _caltyp, _calpar, _calraw, _calref);
            }
        }
    }
    return YMeasure(startTime,endTime,minVal,avgVal,maxVal);
}

double YSensor::_decodeVal(int w)
{
    double val = 0.0;
    val = w;
    if (_caltyp != 0) {
        if (_calhdl != NULL) {
            val = _calhdl(val, _caltyp, _calpar, _calraw, _calref);
        }
    }
    return val;
}

double YSensor::_decodeAvg(int dw)
{
    double val = 0.0;
    val = dw;
    if (_caltyp != 0) {
        if (_calhdl != NULL) {
            val = _calhdl(val, _caltyp, _calpar, _calraw, _calref);
        }
    }
    return val;
}

YSensor *YSensor::nextSensor(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YSensor::FindSensor(hwid);
}

YSensor *YSensor::FirstSensor(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("Sensor", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YSensor::FindSensor(serial+"."+funcId);
}

//--- (end of generated code: YSensor implementation)

YDataSet YSensor::get_recordedData(s64 startTime, s64 endTime)
{
    return this->get_recordedData((double)startTime, (double)endTime);
}

YDataSet YSensor::get_recordedData(int startTime, int endTime)
{
    return this->get_recordedData((double)startTime, (double)endTime);
}

//--- (generated code: YSensor functions)
//--- (end of generated code: YSensor functions)


// DataLogger-specific method to retrieve and pre-parse recorded data
//
int YDataLogger::getData(unsigned runIdx, unsigned timeIdx, string &buffer, yJsonStateMachine &j)
{
    YDevice *dev;
    char query[128];
    string errmsg;
    int res;

    if (this->dataLoggerURL == "") this->dataLoggerURL = "/logger.json";

    // Resolve our reference to our device, load REST API
    res = _getDevice(dev, errmsg);
    if (YISERR(res)) {
        _throw((YRETCODE)res, errmsg);
        return (YRETCODE)res;
    }
    if (timeIdx) {
        // used by old datalogger only
        SAFE_SPRINTF(query, 128, "GET %s?run=%u&time=%u \r\n\r\n", this->dataLoggerURL.c_str(), runIdx, timeIdx);
    } else {
        SAFE_SPRINTF(query, 128, "GET %s \r\n\r\n", this->dataLoggerURL.c_str());
    }
    res = dev->HTTPRequest(0, query, buffer, NULL, NULL, errmsg);
    if (YISERR(res)) {
        // Check if an update of the device list does not solve the issue
        res = YAPI::UpdateDeviceList(errmsg);
        if (YISERR(res)) {
            _throw((YRETCODE)res, errmsg);
            return (YRETCODE)res;
        }
        res = dev->HTTPRequest(0, query, buffer, NULL, NULL, errmsg);
        if (YISERR(res)) {
            _throw((YRETCODE)res, errmsg);
            return (YRETCODE)res;
        }
    }

    // Parse HTTP header
    j.src = buffer.data();
    j.end = j.src + buffer.size();
    j.st = YJSON_HTTP_START;
    if (yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_HTTP_READ_CODE) {
        _throw(YAPI_IO_ERROR, "Failed to parse HTTP header");
        return YAPI_IO_ERROR;
    }
    if (string(j.token) != "200") {
        _throw(YAPI_IO_ERROR, string("Unexpected HTTP return code: ") + j.token);
        return YAPI_IO_ERROR;
    }
    if (yJsonParse(&j) != YJSON_PARSE_AVAIL || j.st != YJSON_HTTP_READ_MSG) {
        _throw(YAPI_IO_ERROR, "Unexpected HTTP header format");
        return YAPI_IO_ERROR;
    }

    return YAPI_SUCCESS;
}


YDataLogger::YDataLogger(const string &func): YFunction(func)
                                              //--- (generated code: YDataLogger initialization)
    ,_currentRunIndex(CURRENTRUNINDEX_INVALID)
    ,_timeUTC(TIMEUTC_INVALID)
    ,_recording(RECORDING_INVALID)
    ,_autoStart(AUTOSTART_INVALID)
    ,_beaconDriven(BEACONDRIVEN_INVALID)
    ,_usage(USAGE_INVALID)
    ,_clearHistory(CLEARHISTORY_INVALID)
    ,_valueCallbackDataLogger(NULL)
//--- (end of generated code: YDataLogger initialization)
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

int YDataLogger::_parseAttr(YJSONObject *json_val)
{
    if(json_val->has("currentRunIndex")) {
        _currentRunIndex =  json_val->getInt("currentRunIndex");
    }
    if(json_val->has("timeUTC")) {
        _timeUTC =  json_val->getLong("timeUTC");
    }
    if(json_val->has("recording")) {
        _recording =  (Y_RECORDING_enum)json_val->getInt("recording");
    }
    if(json_val->has("autoStart")) {
        _autoStart =  (Y_AUTOSTART_enum)json_val->getInt("autoStart");
    }
    if(json_val->has("beaconDriven")) {
        _beaconDriven =  (Y_BEACONDRIVEN_enum)json_val->getInt("beaconDriven");
    }
    if(json_val->has("usage")) {
        _usage =  json_val->getInt("usage");
    }
    if(json_val->has("clearHistory")) {
        _clearHistory =  (Y_CLEARHISTORY_enum)json_val->getInt("clearHistory");
    }
    return YFunction::_parseAttr(json_val);
}


/**
 * Returns the current run number, corresponding to the number of times the module was
 * powered on with the dataLogger enabled at some point.
 *
 * @return an integer corresponding to the current run number, corresponding to the number of times the module was
 *         powered on with the dataLogger enabled at some point
 *
 * On failure, throws an exception or returns YDataLogger::CURRENTRUNINDEX_INVALID.
 */
int YDataLogger::get_currentRunIndex(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YDataLogger::CURRENTRUNINDEX_INVALID;
                }
            }
        }
        res = _currentRunIndex;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the Unix timestamp for current UTC time, if known.
 *
 * @return an integer corresponding to the Unix timestamp for current UTC time, if known
 *
 * On failure, throws an exception or returns YDataLogger::TIMEUTC_INVALID.
 */
s64 YDataLogger::get_timeUTC(void)
{
    s64 res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YDataLogger::TIMEUTC_INVALID;
                }
            }
        }
        res = _timeUTC;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the current UTC time reference used for recorded data.
 *
 * @param newval : an integer corresponding to the current UTC time reference used for recorded data
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDataLogger::set_timeUTC(s64 newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%u", (u32)newval); rest_val = string(buf);
        res = _setAttr("timeUTC", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the current activation state of the data logger.
 *
 * @return a value among YDataLogger::RECORDING_OFF, YDataLogger::RECORDING_ON and
 * YDataLogger::RECORDING_PENDING corresponding to the current activation state of the data logger
 *
 * On failure, throws an exception or returns YDataLogger::RECORDING_INVALID.
 */
Y_RECORDING_enum YDataLogger::get_recording(void)
{
    Y_RECORDING_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YDataLogger::RECORDING_INVALID;
                }
            }
        }
        res = _recording;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the activation state of the data logger to start/stop recording data.
 *
 * @param newval : a value among YDataLogger::RECORDING_OFF, YDataLogger::RECORDING_ON and
 * YDataLogger::RECORDING_PENDING corresponding to the activation state of the data logger to
 * start/stop recording data
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDataLogger::set_recording(Y_RECORDING_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("recording", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the default activation state of the data logger on power up.
 *
 * @return either YDataLogger::AUTOSTART_OFF or YDataLogger::AUTOSTART_ON, according to the default
 * activation state of the data logger on power up
 *
 * On failure, throws an exception or returns YDataLogger::AUTOSTART_INVALID.
 */
Y_AUTOSTART_enum YDataLogger::get_autoStart(void)
{
    Y_AUTOSTART_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YDataLogger::AUTOSTART_INVALID;
                }
            }
        }
        res = _autoStart;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the default activation state of the data logger on power up.
 * Do not forget to call the saveToFlash() method of the module to save the
 * configuration change.  Note: if the device doesn't have any time source at his disposal when
 * starting up, it will wait for ~8 seconds before automatically starting to record  with
 * an arbitrary timestamp
 *
 * @param newval : either YDataLogger::AUTOSTART_OFF or YDataLogger::AUTOSTART_ON, according to the
 * default activation state of the data logger on power up
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDataLogger::set_autoStart(Y_AUTOSTART_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = (newval>0 ? "1" : "0");
        res = _setAttr("autoStart", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns true if the data logger is synchronised with the localization beacon.
 *
 * @return either YDataLogger::BEACONDRIVEN_OFF or YDataLogger::BEACONDRIVEN_ON, according to true if
 * the data logger is synchronised with the localization beacon
 *
 * On failure, throws an exception or returns YDataLogger::BEACONDRIVEN_INVALID.
 */
Y_BEACONDRIVEN_enum YDataLogger::get_beaconDriven(void)
{
    Y_BEACONDRIVEN_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YDataLogger::BEACONDRIVEN_INVALID;
                }
            }
        }
        res = _beaconDriven;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the type of synchronisation of the data logger.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : either YDataLogger::BEACONDRIVEN_OFF or YDataLogger::BEACONDRIVEN_ON, according to
 * the type of synchronisation of the data logger
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDataLogger::set_beaconDriven(Y_BEACONDRIVEN_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = (newval>0 ? "1" : "0");
        res = _setAttr("beaconDriven", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the percentage of datalogger memory in use.
 *
 * @return an integer corresponding to the percentage of datalogger memory in use
 *
 * On failure, throws an exception or returns YDataLogger::USAGE_INVALID.
 */
int YDataLogger::get_usage(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YDataLogger::USAGE_INVALID;
                }
            }
        }
        res = _usage;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

Y_CLEARHISTORY_enum YDataLogger::get_clearHistory(void)
{
    Y_CLEARHISTORY_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YDataLogger::CLEARHISTORY_INVALID;
                }
            }
        }
        res = _clearHistory;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

int YDataLogger::set_clearHistory(Y_CLEARHISTORY_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = (newval>0 ? "1" : "0");
        res = _setAttr("clearHistory", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Retrieves a data logger for a given identifier.
 * The identifier can be specified using several formats:
 *
 * - FunctionLogicalName
 * - ModuleSerialNumber.FunctionIdentifier
 * - ModuleSerialNumber.FunctionLogicalName
 * - ModuleLogicalName.FunctionIdentifier
 * - ModuleLogicalName.FunctionLogicalName
 *
 *
 * This function does not require that the data logger is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the data logger is
 * indeed online at a given time. In case of ambiguity when looking for
 * a data logger by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the data logger, for instance
 *         LIGHTMK4.dataLogger.
 *
 * @return a YDataLogger object allowing you to drive the data logger.
 */
YDataLogger* YDataLogger::FindDataLogger(string func)
{
    YDataLogger* obj = NULL;
    int taken = 0;
    if (YAPI::_apiInitialized) {
        yEnterCriticalSection(&YAPI::_global_cs);
        taken = 1;
    }try {
        obj = (YDataLogger*) YFunction::_FindFromCache("DataLogger", func);
        if (obj == NULL) {
            obj = new YDataLogger(func);
            YFunction::_AddToCache("DataLogger", func, obj);
        }
    } catch (std::exception &) {
        if (taken) yLeaveCriticalSection(&YAPI::_global_cs);
        throw;
    }
    if (taken) yLeaveCriticalSection(&YAPI::_global_cs);
    return obj;
}

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
 * @return YAPI::SUCCESS if the call succeeds.
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

vector<YDataSet> YDataLogger::parse_dataSets(string jsonbuff)
{
    vector<string> dslist;
    YDataSet* dataset = NULL;
    vector<YDataSet> res;

    dslist = this->_json_get_array(jsonbuff);
    res.clear();
    for (unsigned ii = 0; ii < dslist.size(); ii++) {
        dataset = new YDataSet(this);
        dataset->_parse(dslist[ii]);
        res.push_back(*dataset);
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

YDataLogger *YDataLogger::FirstDataLogger(void)
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

//--- (generated code: YDataLogger functions)
//--- (end of generated code: YDataLogger functions)
