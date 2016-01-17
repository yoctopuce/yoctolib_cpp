/*********************************************************************
 *
 * $Id: yocto_refframe.cpp 22360 2015-12-15 13:31:40Z seb $
 *
 * Implements yFindRefFrame(), the high-level API for RefFrame functions
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


#define _CRT_SECURE_NO_DEPRECATE //do not use windows secure crt
#include "yocto_refframe.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

YRefFrame::YRefFrame(const string& func): YFunction(func)
//--- (RefFrame initialization)
    ,_mountPos(MOUNTPOS_INVALID)
    ,_bearing(BEARING_INVALID)
    ,_calibrationParam(CALIBRATIONPARAM_INVALID)
    ,_valueCallbackRefFrame(NULL)
    ,_calibStage(0)
    ,_calibStageProgress(0)
    ,_calibProgress(0)
    ,_calibCount(0)
    ,_calibInternalPos(0)
    ,_calibPrevTick(0)
    ,_calibAccXOfs(0.0)
    ,_calibAccYOfs(0.0)
    ,_calibAccZOfs(0.0)
    ,_calibAccXScale(0.0)
    ,_calibAccYScale(0.0)
    ,_calibAccZScale(0.0)
//--- (end of RefFrame initialization)
{
    _className="RefFrame";
}

YRefFrame::~YRefFrame()
{
//--- (YRefFrame cleanup)
//--- (end of YRefFrame cleanup)
}
//--- (YRefFrame implementation)
// static attributes
const double YRefFrame::BEARING_INVALID = YAPI_INVALID_DOUBLE;
const string YRefFrame::CALIBRATIONPARAM_INVALID = YAPI_INVALID_STRING;

int YRefFrame::_parseAttr(yJsonStateMachine& j)
{
    if(!strcmp(j.token, "mountPos")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _mountPos =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "bearing")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _bearing =  floor(atof(j.token) * 1000.0 / 65536.0 + 0.5) / 1000.0;
        return 1;
    }
    if(!strcmp(j.token, "calibrationParam")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _calibrationParam =  _parseString(j);
        return 1;
    }
    failed:
    return YFunction::_parseAttr(j);
}


int YRefFrame::get_mountPos(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YRefFrame::MOUNTPOS_INVALID;
        }
    }
    return _mountPos;
}

int YRefFrame::set_mountPos(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("mountPos", rest_val);
}

/**
 * Changes the reference bearing used by the compass. The relative bearing
 * indicated by the compass is the difference between the measured magnetic
 * heading and the reference bearing indicated here.
 *
 * For instance, if you setup as reference bearing the value of the earth
 * magnetic declination, the compass will provide the orientation relative
 * to the geographic North.
 *
 * Similarly, when the sensor is not mounted along the standard directions
 * because it has an additional yaw angle, you can set this angle in the reference
 * bearing so that the compass provides the expected natural direction.
 *
 * Remember to call the saveToFlash()
 * method of the module if the modification must be kept.
 *
 * @param newval : a floating point number corresponding to the reference bearing used by the compass
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YRefFrame::set_bearing(double newval)
{
    string rest_val;
    char buf[32]; sprintf(buf,"%d", (int)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
    return _setAttr("bearing", rest_val);
}

/**
 * Returns the reference bearing used by the compass. The relative bearing
 * indicated by the compass is the difference between the measured magnetic
 * heading and the reference bearing indicated here.
 *
 * @return a floating point number corresponding to the reference bearing used by the compass
 *
 * On failure, throws an exception or returns Y_BEARING_INVALID.
 */
double YRefFrame::get_bearing(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YRefFrame::BEARING_INVALID;
        }
    }
    return _bearing;
}

string YRefFrame::get_calibrationParam(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YRefFrame::CALIBRATIONPARAM_INVALID;
        }
    }
    return _calibrationParam;
}

int YRefFrame::set_calibrationParam(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("calibrationParam", rest_val);
}

/**
 * Retrieves $AFUNCTION$ for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that $THEFUNCTION$ is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method YRefFrame.isOnline() to test if $THEFUNCTION$ is
 * indeed online at a given time. In case of ambiguity when looking for
 * $AFUNCTION$ by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * @param func : a string that uniquely characterizes $THEFUNCTION$
 *
 * @return a YRefFrame object allowing you to drive $THEFUNCTION$.
 */
YRefFrame* YRefFrame::FindRefFrame(string func)
{
    YRefFrame* obj = NULL;
    obj = (YRefFrame*) YFunction::_FindFromCache("RefFrame", func);
    if (obj == NULL) {
        obj = new YRefFrame(func);
        YFunction::_AddToCache("RefFrame", func, obj);
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
int YRefFrame::registerValueCallback(YRefFrameValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackRefFrame = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YRefFrame::_invokeValueCallback(string value)
{
    if (_valueCallbackRefFrame != NULL) {
        _valueCallbackRefFrame(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

/**
 * Returns the installation position of the device, as configured
 * in order to define the reference frame for the compass and the
 * pitch/roll tilt sensors.
 *
 * @return a value among the Y_MOUNTPOSITION enumeration
 *         (Y_MOUNTPOSITION_BOTTOM,   Y_MOUNTPOSITION_TOP,
 *         Y_MOUNTPOSITION_FRONT,    Y_MOUNTPOSITION_RIGHT,
 *         Y_MOUNTPOSITION_REAR,     Y_MOUNTPOSITION_LEFT),
 *         corresponding to the installation in a box, on one of the six faces.
 *
 * On failure, throws an exception or returns a negative error code.
 */
Y_MOUNTPOSITION YRefFrame::get_mountPosition(void)
{
    int position = 0;
    position = this->get_mountPos();
    return (Y_MOUNTPOSITION) ((position) >> (2));
}

/**
 * Returns the installation orientation of the device, as configured
 * in order to define the reference frame for the compass and the
 * pitch/roll tilt sensors.
 *
 * @return a value among the enumeration Y_MOUNTORIENTATION
 *         (Y_MOUNTORIENTATION_TWELVE, Y_MOUNTORIENTATION_THREE,
 *         Y_MOUNTORIENTATION_SIX,     Y_MOUNTORIENTATION_NINE)
 *         corresponding to the orientation of the "X" arrow on the device,
 *         as on a clock dial seen from an observer in the center of the box.
 *         On the bottom face, the 12H orientation points to the front, while
 *         on the top face, the 12H orientation points to the rear.
 *
 * On failure, throws an exception or returns a negative error code.
 */
Y_MOUNTORIENTATION YRefFrame::get_mountOrientation(void)
{
    int position = 0;
    position = this->get_mountPos();
    return (Y_MOUNTORIENTATION) ((position) & (3));
}

/**
 * Changes the compass and tilt sensor frame of reference. The magnetic compass
 * and the tilt sensors (pitch and roll) naturally work in the plane
 * parallel to the earth surface. In case the device is not installed upright
 * and horizontally, you must select its reference orientation (parallel to
 * the earth surface) so that the measures are made relative to this position.
 *
 * @param position : a value among the Y_MOUNTPOSITION enumeration
 *         (Y_MOUNTPOSITION_BOTTOM,   Y_MOUNTPOSITION_TOP,
 *         Y_MOUNTPOSITION_FRONT,    Y_MOUNTPOSITION_RIGHT,
 *         Y_MOUNTPOSITION_REAR,     Y_MOUNTPOSITION_LEFT),
 *         corresponding to the installation in a box, on one of the six faces.
 * @param orientation : a value among the enumeration Y_MOUNTORIENTATION
 *         (Y_MOUNTORIENTATION_TWELVE, Y_MOUNTORIENTATION_THREE,
 *         Y_MOUNTORIENTATION_SIX,     Y_MOUNTORIENTATION_NINE)
 *         corresponding to the orientation of the "X" arrow on the device,
 *         as on a clock dial seen from an observer in the center of the box.
 *         On the bottom face, the 12H orientation points to the front, while
 *         on the top face, the 12H orientation points to the rear.
 *
 * Remember to call the saveToFlash()
 * method of the module if the modification must be kept.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YRefFrame::set_mountPosition(Y_MOUNTPOSITION position,Y_MOUNTORIENTATION orientation)
{
    int mixedPos = 0;
    mixedPos = ((position) << (2)) + orientation;
    return this->set_mountPos(mixedPos);
}

int YRefFrame::_calibSort(int start,int stopidx)
{
    int idx = 0;
    int changed = 0;
    double a = 0.0;
    double b = 0.0;
    double xa = 0.0;
    double xb = 0.0;
    // bubble sort is good since we will re-sort again after offset adjustment
    changed = 1;
    while (changed > 0) {
        changed = 0;
        a = _calibDataAcc[start];
        idx = start + 1;
        while (idx < stopidx) {
            b = _calibDataAcc[idx];
            if (a > b) {
                _calibDataAcc[idx-1] = b;
                _calibDataAcc[idx] = a;
                xa = _calibDataAccX[idx-1];
                xb = _calibDataAccX[idx];
                _calibDataAccX[idx-1] = xb;
                _calibDataAccX[idx] = xa;
                xa = _calibDataAccY[idx-1];
                xb = _calibDataAccY[idx];
                _calibDataAccY[idx-1] = xb;
                _calibDataAccY[idx] = xa;
                xa = _calibDataAccZ[idx-1];
                xb = _calibDataAccZ[idx];
                _calibDataAccZ[idx-1] = xb;
                _calibDataAccZ[idx] = xa;
                changed = changed + 1;
            } else {
                a = b;
            }
            idx = idx + 1;
        }
    }
    return 0;
}

/**
 * Initiates the sensors tridimensional calibration process.
 * This calibration is used at low level for inertial position estimation
 * and to enhance the precision of the tilt sensors.
 *
 * After calling this method, the device should be moved according to the
 * instructions provided by method get_3DCalibrationHint,
 * and more3DCalibration should be invoked about 5 times per second.
 * The calibration procedure is completed when the method
 * get_3DCalibrationProgress returns 100. At this point,
 * the computed calibration parameters can be applied using method
 * save3DCalibration. The calibration process can be canceled
 * at any time using method cancel3DCalibration.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YRefFrame::start3DCalibration(void)
{
    if (!(this->isOnline())) {
        return YAPI_DEVICE_NOT_FOUND;
    }
    if (_calibStage != 0) {
        this->cancel3DCalibration();
    }
    _calibSavedParams = this->get_calibrationParam();
    this->set_calibrationParam("0");
    _calibCount = 50;
    _calibStage = 1;
    _calibStageHint = "Set down the device on a steady horizontal surface";
    _calibStageProgress = 0;
    _calibProgress = 1;
    _calibInternalPos = 0;
    _calibPrevTick = (int) ((YAPI::GetTickCount()) & (0x7FFFFFFF));
    _calibOrient.clear();
    _calibDataAccX.clear();
    _calibDataAccY.clear();
    _calibDataAccZ.clear();
    _calibDataAcc.clear();
    return YAPI_SUCCESS;
}

/**
 * Continues the sensors tridimensional calibration process previously
 * initiated using method start3DCalibration.
 * This method should be called approximately 5 times per second, while
 * positioning the device according to the instructions provided by method
 * get_3DCalibrationHint. Note that the instructions change during
 * the calibration process.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YRefFrame::more3DCalibration(void)
{
    int currTick = 0;
    string jsonData;
    double xVal = 0.0;
    double yVal = 0.0;
    double zVal = 0.0;
    double xSq = 0.0;
    double ySq = 0.0;
    double zSq = 0.0;
    double norm = 0.0;
    int orient = 0;
    int idx = 0;
    int intpos = 0;
    int err = 0;
    // make sure calibration has been started
    if (_calibStage == 0) {
        return YAPI_INVALID_ARGUMENT;
    }
    if (_calibProgress == 100) {
        return YAPI_SUCCESS;
    }
    // make sure we leave at least 160ms between samples
    currTick =  (int) ((YAPI::GetTickCount()) & (0x7FFFFFFF));
    if (((currTick - _calibPrevTick) & (0x7FFFFFFF)) < 160) {
        return YAPI_SUCCESS;
    }
    // load current accelerometer values, make sure we are on a straight angle
    // (default timeout to 0,5 sec without reading measure when out of range)
    _calibStageHint = "Set down the device on a steady horizontal surface";
    _calibPrevTick = ((currTick + 500) & (0x7FFFFFFF));
    jsonData = this->_download("api/accelerometer.json");
    xVal = atoi((this->_json_get_key(jsonData, "xValue")).c_str()) / 65536.0;
    yVal = atoi((this->_json_get_key(jsonData, "yValue")).c_str()) / 65536.0;
    zVal = atoi((this->_json_get_key(jsonData, "zValue")).c_str()) / 65536.0;
    xSq = xVal * xVal;
    if (xSq >= 0.04 && xSq < 0.64) {
        return YAPI_SUCCESS;
    }
    if (xSq >= 1.44) {
        return YAPI_SUCCESS;
    }
    ySq = yVal * yVal;
    if (ySq >= 0.04 && ySq < 0.64) {
        return YAPI_SUCCESS;
    }
    if (ySq >= 1.44) {
        return YAPI_SUCCESS;
    }
    zSq = zVal * zVal;
    if (zSq >= 0.04 && zSq < 0.64) {
        return YAPI_SUCCESS;
    }
    if (zSq >= 1.44) {
        return YAPI_SUCCESS;
    }
    norm = sqrt(xSq + ySq + zSq);
    if (norm < 0.8 || norm > 1.2) {
        return YAPI_SUCCESS;
    }
    _calibPrevTick = currTick;
    // Determine the device orientation index
    orient = 0;
    if (zSq > 0.5) {
        if (zVal > 0) {
            orient = 0;
        } else {
            orient = 1;
        }
    }
    if (xSq > 0.5) {
        if (xVal > 0) {
            orient = 2;
        } else {
            orient = 3;
        }
    }
    if (ySq > 0.5) {
        if (yVal > 0) {
            orient = 4;
        } else {
            orient = 5;
        }
    }
    // Discard measures that are not in the proper orientation
    if (_calibStageProgress == 0) {
        idx = 0;
        err = 0;
        while (idx + 1 < _calibStage) {
            if (_calibOrient[idx] == orient) {
                err = 1;
            }
            idx = idx + 1;
        }
        if (err != 0) {
            _calibStageHint = "Turn the device on another face";
            return YAPI_SUCCESS;
        }
        _calibOrient.push_back(orient);
    } else {
        if (orient != _calibOrient[_calibStage-1]) {
            _calibStageHint = "Not yet done, please move back to the previous face";
            return YAPI_SUCCESS;
        }
    }
    // Save measure
    _calibStageHint = "calibrating...";
    _calibDataAccX.push_back(xVal);
    _calibDataAccY.push_back(yVal);
    _calibDataAccZ.push_back(zVal);
    _calibDataAcc.push_back(norm);
    _calibInternalPos = _calibInternalPos + 1;
    _calibProgress = 1 + 16 * (_calibStage - 1) + ((16 * _calibInternalPos) / (_calibCount));
    if (_calibInternalPos < _calibCount) {
        _calibStageProgress = 1 + ((99 * _calibInternalPos) / (_calibCount));
        return YAPI_SUCCESS;
    }
    // Stage done, compute preliminary result
    intpos = (_calibStage - 1) * _calibCount;
    this->_calibSort(intpos, intpos + _calibCount);
    intpos = intpos + ((_calibCount) / (2));
    _calibLogMsg = YapiWrapper::ysprintf("Stage %d: median is %d,%d,%d", _calibStage,
    (int) floor(1000*_calibDataAccX[intpos]+0.5),
    (int) floor(1000*_calibDataAccY[intpos]+0.5),(int) floor(1000*_calibDataAccZ[intpos]+0.5));
    // move to next stage
    _calibStage = _calibStage + 1;
    if (_calibStage < 7) {
        _calibStageHint = "Turn the device on another face";
        _calibPrevTick = ((currTick + 500) & (0x7FFFFFFF));
        _calibStageProgress = 0;
        _calibInternalPos = 0;
        return YAPI_SUCCESS;
    }
    // Data collection completed, compute accelerometer shift
    xVal = 0;
    yVal = 0;
    zVal = 0;
    idx = 0;
    while (idx < 6) {
        intpos = idx * _calibCount + ((_calibCount) / (2));
        orient = _calibOrient[idx];
        if (orient == 0 || orient == 1) {
            zVal = zVal + _calibDataAccZ[intpos];
        }
        if (orient == 2 || orient == 3) {
            xVal = xVal + _calibDataAccX[intpos];
        }
        if (orient == 4 || orient == 5) {
            yVal = yVal + _calibDataAccY[intpos];
        }
        idx = idx + 1;
    }
    _calibAccXOfs = xVal / 2.0;
    _calibAccYOfs = yVal / 2.0;
    _calibAccZOfs = zVal / 2.0;
    // Recompute all norms, taking into account the computed shift, and re-sort
    intpos = 0;
    while (intpos < (int)_calibDataAcc.size()) {
        xVal = _calibDataAccX[intpos] - _calibAccXOfs;
        yVal = _calibDataAccY[intpos] - _calibAccYOfs;
        zVal = _calibDataAccZ[intpos] - _calibAccZOfs;
        norm = sqrt(xVal * xVal + yVal * yVal + zVal * zVal);
        _calibDataAcc[intpos] = norm;
        intpos = intpos + 1;
    }
    idx = 0;
    while (idx < 6) {
        intpos = idx * _calibCount;
        this->_calibSort(intpos, intpos + _calibCount);
        idx = idx + 1;
    }
    // Compute the scaling factor for each axis
    xVal = 0;
    yVal = 0;
    zVal = 0;
    idx = 0;
    while (idx < 6) {
        intpos = idx * _calibCount + ((_calibCount) / (2));
        orient = _calibOrient[idx];
        if (orient == 0 || orient == 1) {
            zVal = zVal + _calibDataAcc[intpos];
        }
        if (orient == 2 || orient == 3) {
            xVal = xVal + _calibDataAcc[intpos];
        }
        if (orient == 4 || orient == 5) {
            yVal = yVal + _calibDataAcc[intpos];
        }
        idx = idx + 1;
    }
    _calibAccXScale = xVal / 2.0;
    _calibAccYScale = yVal / 2.0;
    _calibAccZScale = zVal / 2.0;
    // Report completion
    _calibProgress = 100;
    _calibStageHint = "Calibration data ready for saving";
    return YAPI_SUCCESS;
}

/**
 * Returns instructions to proceed to the tridimensional calibration initiated with
 * method start3DCalibration.
 *
 * @return a character string.
 */
string YRefFrame::get_3DCalibrationHint(void)
{
    return _calibStageHint;
}

/**
 * Returns the global process indicator for the tridimensional calibration
 * initiated with method start3DCalibration.
 *
 * @return an integer between 0 (not started) and 100 (stage completed).
 */
int YRefFrame::get_3DCalibrationProgress(void)
{
    return _calibProgress;
}

/**
 * Returns index of the current stage of the calibration
 * initiated with method start3DCalibration.
 *
 * @return an integer, growing each time a calibration stage is completed.
 */
int YRefFrame::get_3DCalibrationStage(void)
{
    return _calibStage;
}

/**
 * Returns the process indicator for the current stage of the calibration
 * initiated with method start3DCalibration.
 *
 * @return an integer between 0 (not started) and 100 (stage completed).
 */
int YRefFrame::get_3DCalibrationStageProgress(void)
{
    return _calibStageProgress;
}

/**
 * Returns the latest log message from the calibration process.
 * When no new message is available, returns an empty string.
 *
 * @return a character string.
 */
string YRefFrame::get_3DCalibrationLogMsg(void)
{
    string msg;
    msg = _calibLogMsg;
    _calibLogMsg = "";
    return msg;
}

/**
 * Applies the sensors tridimensional calibration parameters that have just been computed.
 * Remember to call the saveToFlash()  method of the module if the changes
 * must be kept when the device is restarted.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YRefFrame::save3DCalibration(void)
{
    int shiftX = 0;
    int shiftY = 0;
    int shiftZ = 0;
    int scaleExp = 0;
    int scaleX = 0;
    int scaleY = 0;
    int scaleZ = 0;
    int scaleLo = 0;
    int scaleHi = 0;
    string newcalib;
    if (_calibProgress != 100) {
        return YAPI_INVALID_ARGUMENT;
    }
    // Compute integer values (correction unit is 732ug/count)
    shiftX = -(int) floor(_calibAccXOfs / 0.000732+0.5);
    if (shiftX < 0) {
        shiftX = shiftX + 65536;
    }
    shiftY = -(int) floor(_calibAccYOfs / 0.000732+0.5);
    if (shiftY < 0) {
        shiftY = shiftY + 65536;
    }
    shiftZ = -(int) floor(_calibAccZOfs / 0.000732+0.5);
    if (shiftZ < 0) {
        shiftZ = shiftZ + 65536;
    }
    scaleX = (int) floor(2048.0 / _calibAccXScale+0.5) - 2048;
    scaleY = (int) floor(2048.0 / _calibAccYScale+0.5) - 2048;
    scaleZ = (int) floor(2048.0 / _calibAccZScale+0.5) - 2048;
    if (scaleX < -2048 || scaleX >= 2048 || scaleY < -2048 || scaleY >= 2048 || scaleZ < -2048 || scaleZ >= 2048) {
        scaleExp = 3;
    } else {
        if (scaleX < -1024 || scaleX >= 1024 || scaleY < -1024 || scaleY >= 1024 || scaleZ < -1024 || scaleZ >= 1024) {
            scaleExp = 2;
        } else {
            if (scaleX < -512 || scaleX >= 512 || scaleY < -512 || scaleY >= 512 || scaleZ < -512 || scaleZ >= 512) {
                scaleExp = 1;
            } else {
                scaleExp = 0;
            }
        }
    }
    if (scaleExp > 0) {
        scaleX = ((scaleX) >> (scaleExp));
        scaleY = ((scaleY) >> (scaleExp));
        scaleZ = ((scaleZ) >> (scaleExp));
    }
    if (scaleX < 0) {
        scaleX = scaleX + 1024;
    }
    if (scaleY < 0) {
        scaleY = scaleY + 1024;
    }
    if (scaleZ < 0) {
        scaleZ = scaleZ + 1024;
    }
    scaleLo = ((((scaleY) & (15))) << (12)) + ((scaleX) << (2)) + scaleExp;
    scaleHi = ((scaleZ) << (6)) + ((scaleY) >> (4));
    // Save calibration parameters
    newcalib = YapiWrapper::ysprintf("5,%d,%d,%d,%d,%d", shiftX, shiftY, shiftZ, scaleLo,scaleHi);
    _calibStage = 0;
    return this->set_calibrationParam(newcalib);
}

/**
 * Aborts the sensors tridimensional calibration process et restores normal settings.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YRefFrame::cancel3DCalibration(void)
{
    if (_calibStage == 0) {
        return YAPI_SUCCESS;
    }
    // may throw an exception
    _calibStage = 0;
    return this->set_calibrationParam(_calibSavedParams);
}

YRefFrame *YRefFrame::nextRefFrame(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YRefFrame::FindRefFrame(hwid);
}

YRefFrame* YRefFrame::FirstRefFrame(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("RefFrame", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YRefFrame::FindRefFrame(serial+"."+funcId);
}

//--- (end of YRefFrame implementation)

//--- (RefFrame functions)
//--- (end of RefFrame functions)
