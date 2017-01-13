/*********************************************************************
 *
 * $Id: yocto_steppermotor.cpp 26277 2017-01-04 15:35:59Z seb $
 *
 * Implements yFindStepperMotor(), the high-level API for StepperMotor functions
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
#include "yocto_steppermotor.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

YStepperMotor::YStepperMotor(const string& func): YFunction(func)
//--- (StepperMotor initialization)
    ,_motorState(MOTORSTATE_INVALID)
    ,_diags(DIAGS_INVALID)
    ,_stepPos(STEPPOS_INVALID)
    ,_speed(SPEED_INVALID)
    ,_pullinSpeed(PULLINSPEED_INVALID)
    ,_maxAccel(MAXACCEL_INVALID)
    ,_maxSpeed(MAXSPEED_INVALID)
    ,_stepping(STEPPING_INVALID)
    ,_overcurrent(OVERCURRENT_INVALID)
    ,_tCurrStop(TCURRSTOP_INVALID)
    ,_tCurrRun(TCURRRUN_INVALID)
    ,_alertMode(ALERTMODE_INVALID)
    ,_auxMode(AUXMODE_INVALID)
    ,_auxSignal(AUXSIGNAL_INVALID)
    ,_command(COMMAND_INVALID)
    ,_valueCallbackStepperMotor(NULL)
//--- (end of StepperMotor initialization)
{
    _className="StepperMotor";
}

YStepperMotor::~YStepperMotor()
{
//--- (YStepperMotor cleanup)
//--- (end of YStepperMotor cleanup)
}
//--- (YStepperMotor implementation)
// static attributes
const double YStepperMotor::STEPPOS_INVALID = YAPI_INVALID_DOUBLE;
const double YStepperMotor::SPEED_INVALID = YAPI_INVALID_DOUBLE;
const double YStepperMotor::PULLINSPEED_INVALID = YAPI_INVALID_DOUBLE;
const double YStepperMotor::MAXACCEL_INVALID = YAPI_INVALID_DOUBLE;
const double YStepperMotor::MAXSPEED_INVALID = YAPI_INVALID_DOUBLE;
const string YStepperMotor::ALERTMODE_INVALID = YAPI_INVALID_STRING;
const string YStepperMotor::AUXMODE_INVALID = YAPI_INVALID_STRING;
const string YStepperMotor::COMMAND_INVALID = YAPI_INVALID_STRING;

int YStepperMotor::_parseAttr(yJsonStateMachine& j)
{
    if(!strcmp(j.token, "motorState")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _motorState =  (Y_MOTORSTATE_enum)atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "diags")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _diags =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "stepPos")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _stepPos =  atof(j.token) / 16.0;
        return 1;
    }
    if(!strcmp(j.token, "speed")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _speed =  floor(atof(j.token) * 1000.0 / 65536.0 + 0.5) / 1000.0;
        return 1;
    }
    if(!strcmp(j.token, "pullinSpeed")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _pullinSpeed =  floor(atof(j.token) * 1000.0 / 65536.0 + 0.5) / 1000.0;
        return 1;
    }
    if(!strcmp(j.token, "maxAccel")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _maxAccel =  floor(atof(j.token) * 1000.0 / 65536.0 + 0.5) / 1000.0;
        return 1;
    }
    if(!strcmp(j.token, "maxSpeed")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _maxSpeed =  floor(atof(j.token) * 1000.0 / 65536.0 + 0.5) / 1000.0;
        return 1;
    }
    if(!strcmp(j.token, "stepping")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _stepping =  (Y_STEPPING_enum)atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "overcurrent")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _overcurrent =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "tCurrStop")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _tCurrStop =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "tCurrRun")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _tCurrRun =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "alertMode")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _alertMode =  _parseString(j);
        return 1;
    }
    if(!strcmp(j.token, "auxMode")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _auxMode =  _parseString(j);
        return 1;
    }
    if(!strcmp(j.token, "auxSignal")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _auxSignal =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "command")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _command =  _parseString(j);
        return 1;
    }
    failed:
    return YFunction::_parseAttr(j);
}


/**
 * Returns the motor working state.
 *
 * @return a value among Y_MOTORSTATE_ABSENT, Y_MOTORSTATE_ALERT, Y_MOTORSTATE_HI_Z,
 * Y_MOTORSTATE_STOP, Y_MOTORSTATE_RUN and Y_MOTORSTATE_BATCH corresponding to the motor working state
 *
 * On failure, throws an exception or returns Y_MOTORSTATE_INVALID.
 */
Y_MOTORSTATE_enum YStepperMotor::get_motorState(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YStepperMotor::MOTORSTATE_INVALID;
        }
    }
    return _motorState;
}

/**
 * Returns the stepper motor controller diagnostics, as a bitmap.
 *
 * @return an integer corresponding to the stepper motor controller diagnostics, as a bitmap
 *
 * On failure, throws an exception or returns Y_DIAGS_INVALID.
 */
int YStepperMotor::get_diags(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YStepperMotor::DIAGS_INVALID;
        }
    }
    return _diags;
}

/**
 * Changes the current logical motor position, measured in steps.
 * This command does not cause any motor move, as its purpose is only to setup
 * the origin of the position counter. The fractional part of the position,
 * that corresponds to the physical position of the rotor, is not changed.
 * To trigger a motor move, use methods moveTo() or moveRel()
 * instead.
 *
 * @param newval : a floating point number corresponding to the current logical motor position, measured in steps
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YStepperMotor::set_stepPos(double newval)
{
    string rest_val;
    char buf[32]; sprintf(buf,"%.2f", floor(newval * 100.0)/100.0); rest_val = string(buf);
    return _setAttr("stepPos", rest_val);
}

/**
 * Returns the current logical motor position, measured in steps.
 * The value may include a fractional part when micro-stepping is in use.
 *
 * @return a floating point number corresponding to the current logical motor position, measured in steps
 *
 * On failure, throws an exception or returns Y_STEPPOS_INVALID.
 */
double YStepperMotor::get_stepPos(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YStepperMotor::STEPPOS_INVALID;
        }
    }
    return _stepPos;
}

/**
 * Returns current motor speed, measured in steps per second.
 * To change speed, use method changeSpeed().
 *
 * @return a floating point number corresponding to current motor speed, measured in steps per second
 *
 * On failure, throws an exception or returns Y_SPEED_INVALID.
 */
double YStepperMotor::get_speed(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YStepperMotor::SPEED_INVALID;
        }
    }
    return _speed;
}

/**
 * Changes the motor speed immediately reachable from stop state, measured in steps per second.
 *
 * @param newval : a floating point number corresponding to the motor speed immediately reachable from
 * stop state, measured in steps per second
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YStepperMotor::set_pullinSpeed(double newval)
{
    string rest_val;
    char buf[32]; sprintf(buf,"%d", (int)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
    return _setAttr("pullinSpeed", rest_val);
}

/**
 * Returns the motor speed immediately reachable from stop state, measured in steps per second.
 *
 * @return a floating point number corresponding to the motor speed immediately reachable from stop
 * state, measured in steps per second
 *
 * On failure, throws an exception or returns Y_PULLINSPEED_INVALID.
 */
double YStepperMotor::get_pullinSpeed(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YStepperMotor::PULLINSPEED_INVALID;
        }
    }
    return _pullinSpeed;
}

/**
 * Changes the maximal motor acceleration, measured in steps per second^2.
 *
 * @param newval : a floating point number corresponding to the maximal motor acceleration, measured
 * in steps per second^2
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YStepperMotor::set_maxAccel(double newval)
{
    string rest_val;
    char buf[32]; sprintf(buf,"%d", (int)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
    return _setAttr("maxAccel", rest_val);
}

/**
 * Returns the maximal motor acceleration, measured in steps per second^2.
 *
 * @return a floating point number corresponding to the maximal motor acceleration, measured in steps per second^2
 *
 * On failure, throws an exception or returns Y_MAXACCEL_INVALID.
 */
double YStepperMotor::get_maxAccel(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YStepperMotor::MAXACCEL_INVALID;
        }
    }
    return _maxAccel;
}

/**
 * Changes the maximal motor speed, measured in steps per second.
 *
 * @param newval : a floating point number corresponding to the maximal motor speed, measured in steps per second
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YStepperMotor::set_maxSpeed(double newval)
{
    string rest_val;
    char buf[32]; sprintf(buf,"%d", (int)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
    return _setAttr("maxSpeed", rest_val);
}

/**
 * Returns the maximal motor speed, measured in steps per second.
 *
 * @return a floating point number corresponding to the maximal motor speed, measured in steps per second
 *
 * On failure, throws an exception or returns Y_MAXSPEED_INVALID.
 */
double YStepperMotor::get_maxSpeed(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YStepperMotor::MAXSPEED_INVALID;
        }
    }
    return _maxSpeed;
}

/**
 * Returns the stepping mode used to drive the motor.
 *
 * @return a value among Y_STEPPING_MICROSTEP16, Y_STEPPING_MICROSTEP8, Y_STEPPING_MICROSTEP4,
 * Y_STEPPING_HALFSTEP and Y_STEPPING_FULLSTEP corresponding to the stepping mode used to drive the motor
 *
 * On failure, throws an exception or returns Y_STEPPING_INVALID.
 */
Y_STEPPING_enum YStepperMotor::get_stepping(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YStepperMotor::STEPPING_INVALID;
        }
    }
    return _stepping;
}

/**
 * Changes the stepping mode used to drive the motor.
 *
 * @param newval : a value among Y_STEPPING_MICROSTEP16, Y_STEPPING_MICROSTEP8, Y_STEPPING_MICROSTEP4,
 * Y_STEPPING_HALFSTEP and Y_STEPPING_FULLSTEP corresponding to the stepping mode used to drive the motor
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YStepperMotor::set_stepping(Y_STEPPING_enum newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("stepping", rest_val);
}

/**
 * Returns the overcurrent alert and emergency stop threshold, measured in mA.
 *
 * @return an integer corresponding to the overcurrent alert and emergency stop threshold, measured in mA
 *
 * On failure, throws an exception or returns Y_OVERCURRENT_INVALID.
 */
int YStepperMotor::get_overcurrent(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YStepperMotor::OVERCURRENT_INVALID;
        }
    }
    return _overcurrent;
}

/**
 * Changes the overcurrent alert and emergency stop threshold, measured in mA.
 *
 * @param newval : an integer corresponding to the overcurrent alert and emergency stop threshold, measured in mA
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YStepperMotor::set_overcurrent(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("overcurrent", rest_val);
}

/**
 * Returns the torque regulation current when the motor is stopped, measured in mA.
 *
 * @return an integer corresponding to the torque regulation current when the motor is stopped, measured in mA
 *
 * On failure, throws an exception or returns Y_TCURRSTOP_INVALID.
 */
int YStepperMotor::get_tCurrStop(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YStepperMotor::TCURRSTOP_INVALID;
        }
    }
    return _tCurrStop;
}

/**
 * Changes the torque regulation current when the motor is stopped, measured in mA.
 *
 * @param newval : an integer corresponding to the torque regulation current when the motor is
 * stopped, measured in mA
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YStepperMotor::set_tCurrStop(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("tCurrStop", rest_val);
}

/**
 * Returns the torque regulation current when the motor is running, measured in mA.
 *
 * @return an integer corresponding to the torque regulation current when the motor is running, measured in mA
 *
 * On failure, throws an exception or returns Y_TCURRRUN_INVALID.
 */
int YStepperMotor::get_tCurrRun(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YStepperMotor::TCURRRUN_INVALID;
        }
    }
    return _tCurrRun;
}

/**
 * Changes the torque regulation current when the motor is running, measured in mA.
 *
 * @param newval : an integer corresponding to the torque regulation current when the motor is
 * running, measured in mA
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YStepperMotor::set_tCurrRun(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("tCurrRun", rest_val);
}

string YStepperMotor::get_alertMode(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YStepperMotor::ALERTMODE_INVALID;
        }
    }
    return _alertMode;
}

int YStepperMotor::set_alertMode(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("alertMode", rest_val);
}

string YStepperMotor::get_auxMode(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YStepperMotor::AUXMODE_INVALID;
        }
    }
    return _auxMode;
}

int YStepperMotor::set_auxMode(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("auxMode", rest_val);
}

/**
 * Returns the current value of the signal generated on the auxiliary output.
 *
 * @return an integer corresponding to the current value of the signal generated on the auxiliary output
 *
 * On failure, throws an exception or returns Y_AUXSIGNAL_INVALID.
 */
int YStepperMotor::get_auxSignal(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YStepperMotor::AUXSIGNAL_INVALID;
        }
    }
    return _auxSignal;
}

/**
 * Changes the value of the signal generated on the auxiliary output.
 * Acceptable values depend on the auxiliary output signal type configured.
 *
 * @param newval : an integer corresponding to the value of the signal generated on the auxiliary output
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YStepperMotor::set_auxSignal(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("auxSignal", rest_val);
}

string YStepperMotor::get_command(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YStepperMotor::COMMAND_INVALID;
        }
    }
    return _command;
}

int YStepperMotor::set_command(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("command", rest_val);
}

/**
 * Retrieves a stepper motor for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that the stepper motor is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method YStepperMotor.isOnline() to test if the stepper motor is
 * indeed online at a given time. In case of ambiguity when looking for
 * a stepper motor by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * @param func : a string that uniquely characterizes the stepper motor
 *
 * @return a YStepperMotor object allowing you to drive the stepper motor.
 */
YStepperMotor* YStepperMotor::FindStepperMotor(string func)
{
    YStepperMotor* obj = NULL;
    obj = (YStepperMotor*) YFunction::_FindFromCache("StepperMotor", func);
    if (obj == NULL) {
        obj = new YStepperMotor(func);
        YFunction::_AddToCache("StepperMotor", func, obj);
    }
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
int YStepperMotor::registerValueCallback(YStepperMotorValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackStepperMotor = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YStepperMotor::_invokeValueCallback(string value)
{
    if (_valueCallbackStepperMotor != NULL) {
        _valueCallbackStepperMotor(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

int YStepperMotor::sendCommand(string command)
{
    return this->set_command(command);
}

/**
 * Reinitialize the controller and clear all alert flags.
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *         On failure, throws an exception or returns a negative error code.
 */
int YStepperMotor::reset(void)
{
    return this->sendCommand("Z");
}

/**
 * Starts the motor backward at the specified speed, to search for the motor home position.
 *
 * @param speed : desired speed, in steps per second.
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *         On failure, throws an exception or returns a negative error code.
 */
int YStepperMotor::findHomePosition(double speed)
{
    return this->sendCommand(YapiWrapper::ysprintf("H%d",(int) floor(1000*speed+0.5)));
}

/**
 * Starts the motor at a given speed. The time needed to reach the requested speed
 * will depend on the acceleration parameters configured for the motor.
 *
 * @param speed : desired speed, in steps per second. The minimal non-zero speed
 *         is 0.001 pulse per second.
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *         On failure, throws an exception or returns a negative error code.
 */
int YStepperMotor::changeSpeed(double speed)
{
    return this->sendCommand(YapiWrapper::ysprintf("R%d",(int) floor(1000*speed+0.5)));
}

/**
 * Starts the motor to reach a given absolute position. The time needed to reach the requested
 * position will depend on the acceleration and max speed parameters configured for
 * the motor.
 *
 * @param absPos : absolute position, measured in steps from the origin.
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *         On failure, throws an exception or returns a negative error code.
 */
int YStepperMotor::moveTo(double absPos)
{
    return this->sendCommand(YapiWrapper::ysprintf("M%d",(int) floor(16*absPos+0.5)));
}

/**
 * Starts the motor to reach a given relative position. The time needed to reach the requested
 * position will depend on the acceleration and max speed parameters configured for
 * the motor.
 *
 * @param relPos : relative position, measured in steps from the current position.
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *         On failure, throws an exception or returns a negative error code.
 */
int YStepperMotor::moveRel(double relPos)
{
    return this->sendCommand(YapiWrapper::ysprintf("m%d",(int) floor(16*relPos+0.5)));
}

/**
 * Keep the motor in the same state for the specified amount of time, before processing next command.
 *
 * @param waitMs : wait time, specified in milliseconds.
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *         On failure, throws an exception or returns a negative error code.
 */
int YStepperMotor::pause(int waitMs)
{
    return this->sendCommand(YapiWrapper::ysprintf("_%d",waitMs));
}

/**
 * Stops the motor with an emergency alert, without taking any additional precaution.
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *         On failure, throws an exception or returns a negative error code.
 */
int YStepperMotor::emergencyStop(void)
{
    return this->sendCommand("!");
}

/**
 * Move one step in the direction opposite the direction set when the most recent alert was raised.
 * The move occures even if the system is still in alert mode (end switch depressed). Caution.
 * use this function with great care as it may cause mechanical damages !
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *         On failure, throws an exception or returns a negative error code.
 */
int YStepperMotor::alertStepOut(void)
{
    return this->sendCommand(".");
}

/**
 * Stops the motor smoothly as soon as possible, without waiting for ongoing move completion.
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *         On failure, throws an exception or returns a negative error code.
 */
int YStepperMotor::abortAndBrake(void)
{
    return this->sendCommand("B");
}

/**
 * Turn the controller into Hi-Z mode immediately, without waiting for ongoing move completion.
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *         On failure, throws an exception or returns a negative error code.
 */
int YStepperMotor::abortAndHiZ(void)
{
    return this->sendCommand("z");
}

YStepperMotor *YStepperMotor::nextStepperMotor(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YStepperMotor::FindStepperMotor(hwid);
}

YStepperMotor* YStepperMotor::FirstStepperMotor(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("StepperMotor", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YStepperMotor::FindStepperMotor(serial+"."+funcId);
}

//--- (end of YStepperMotor implementation)

//--- (StepperMotor functions)
//--- (end of StepperMotor functions)
