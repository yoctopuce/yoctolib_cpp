/*********************************************************************
 *
 * $Id: yocto_motor.cpp 22191 2015-12-02 06:49:31Z mvuilleu $
 *
 * Implements yFindMotor(), the high-level API for Motor functions
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
#include "yocto_motor.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

YMotor::YMotor(const string& func): YFunction(func)
//--- (Motor initialization)
    ,_motorStatus(MOTORSTATUS_INVALID)
    ,_drivingForce(DRIVINGFORCE_INVALID)
    ,_brakingForce(BRAKINGFORCE_INVALID)
    ,_cutOffVoltage(CUTOFFVOLTAGE_INVALID)
    ,_overCurrentLimit(OVERCURRENTLIMIT_INVALID)
    ,_frequency(FREQUENCY_INVALID)
    ,_starterTime(STARTERTIME_INVALID)
    ,_failSafeTimeout(FAILSAFETIMEOUT_INVALID)
    ,_command(COMMAND_INVALID)
    ,_valueCallbackMotor(NULL)
//--- (end of Motor initialization)
{
    _className="Motor";
}

YMotor::~YMotor()
{
//--- (YMotor cleanup)
//--- (end of YMotor cleanup)
}
//--- (YMotor implementation)
// static attributes
const double YMotor::DRIVINGFORCE_INVALID = YAPI_INVALID_DOUBLE;
const double YMotor::BRAKINGFORCE_INVALID = YAPI_INVALID_DOUBLE;
const double YMotor::CUTOFFVOLTAGE_INVALID = YAPI_INVALID_DOUBLE;
const double YMotor::FREQUENCY_INVALID = YAPI_INVALID_DOUBLE;
const string YMotor::COMMAND_INVALID = YAPI_INVALID_STRING;

int YMotor::_parseAttr(yJsonStateMachine& j)
{
    if(!strcmp(j.token, "motorStatus")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _motorStatus =  (Y_MOTORSTATUS_enum)atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "drivingForce")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _drivingForce =  floor(atof(j.token) * 1000.0 / 65536.0 + 0.5) / 1000.0;
        return 1;
    }
    if(!strcmp(j.token, "brakingForce")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _brakingForce =  floor(atof(j.token) * 1000.0 / 65536.0 + 0.5) / 1000.0;
        return 1;
    }
    if(!strcmp(j.token, "cutOffVoltage")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _cutOffVoltage =  floor(atof(j.token) * 1000.0 / 65536.0 + 0.5) / 1000.0;
        return 1;
    }
    if(!strcmp(j.token, "overCurrentLimit")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _overCurrentLimit =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "frequency")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _frequency =  floor(atof(j.token) * 1000.0 / 65536.0 + 0.5) / 1000.0;
        return 1;
    }
    if(!strcmp(j.token, "starterTime")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _starterTime =  atoi(j.token);
        return 1;
    }
    if(!strcmp(j.token, "failSafeTimeout")) {
        if(yJsonParse(&j) != YJSON_PARSE_AVAIL) goto failed;
        _failSafeTimeout =  atoi(j.token);
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
 * Return the controller state. Possible states are:
 * IDLE   when the motor is stopped/in free wheel, ready to start;
 * FORWD  when the controller is driving the motor forward;
 * BACKWD when the controller is driving the motor backward;
 * BRAKE  when the controller is braking;
 * LOVOLT when the controller has detected a low voltage condition;
 * HICURR when the controller has detected an overcurrent condition;
 * HIHEAT when the controller has detected an overheat condition;
 * FAILSF when the controller switched on the failsafe security.
 *
 * When an error condition occurred (LOVOLT, HICURR, HIHEAT, FAILSF), the controller
 * status must be explicitly reset using the resetStatus function.
 *
 * @return a value among Y_MOTORSTATUS_IDLE, Y_MOTORSTATUS_BRAKE, Y_MOTORSTATUS_FORWD,
 * Y_MOTORSTATUS_BACKWD, Y_MOTORSTATUS_LOVOLT, Y_MOTORSTATUS_HICURR, Y_MOTORSTATUS_HIHEAT and Y_MOTORSTATUS_FAILSF
 *
 * On failure, throws an exception or returns Y_MOTORSTATUS_INVALID.
 */
Y_MOTORSTATUS_enum YMotor::get_motorStatus(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YMotor::MOTORSTATUS_INVALID;
        }
    }
    return _motorStatus;
}

int YMotor::set_motorStatus(Y_MOTORSTATUS_enum newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("motorStatus", rest_val);
}

/**
 * Changes immediately the power sent to the motor. The value is a percentage between -100%
 * to 100%. If you want go easy on your mechanics and avoid excessive current consumption,
 * try to avoid brutal power changes. For example, immediate transition from forward full power
 * to reverse full power is a very bad idea. Each time the driving power is modified, the
 * braking power is set to zero.
 *
 * @param newval : a floating point number corresponding to immediately the power sent to the motor
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YMotor::set_drivingForce(double newval)
{
    string rest_val;
    char buf[32]; sprintf(buf,"%d", (int)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
    return _setAttr("drivingForce", rest_val);
}

/**
 * Returns the power sent to the motor, as a percentage between -100% and +100%.
 *
 * @return a floating point number corresponding to the power sent to the motor, as a percentage
 * between -100% and +100%
 *
 * On failure, throws an exception or returns Y_DRIVINGFORCE_INVALID.
 */
double YMotor::get_drivingForce(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YMotor::DRIVINGFORCE_INVALID;
        }
    }
    return _drivingForce;
}

/**
 * Changes immediately the braking force applied to the motor (in percents).
 * The value 0 corresponds to no braking (free wheel). When the braking force
 * is changed, the driving power is set to zero. The value is a percentage.
 *
 * @param newval : a floating point number corresponding to immediately the braking force applied to
 * the motor (in percents)
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YMotor::set_brakingForce(double newval)
{
    string rest_val;
    char buf[32]; sprintf(buf,"%d", (int)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
    return _setAttr("brakingForce", rest_val);
}

/**
 * Returns the braking force applied to the motor, as a percentage.
 * The value 0 corresponds to no braking (free wheel).
 *
 * @return a floating point number corresponding to the braking force applied to the motor, as a percentage
 *
 * On failure, throws an exception or returns Y_BRAKINGFORCE_INVALID.
 */
double YMotor::get_brakingForce(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YMotor::BRAKINGFORCE_INVALID;
        }
    }
    return _brakingForce;
}

/**
 * Changes the threshold voltage under which the controller automatically switches to error state
 * and prevents further current draw. This setting prevent damage to a battery that can
 * occur when drawing current from an "empty" battery.
 * Note that whatever the cutoff threshold, the controller switches to undervoltage
 * error state if the power supply goes under 3V, even for a very brief time.
 *
 * @param newval : a floating point number corresponding to the threshold voltage under which the
 * controller automatically switches to error state
 *         and prevents further current draw
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YMotor::set_cutOffVoltage(double newval)
{
    string rest_val;
    char buf[32]; sprintf(buf,"%d", (int)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
    return _setAttr("cutOffVoltage", rest_val);
}

/**
 * Returns the threshold voltage under which the controller automatically switches to error state
 * and prevents further current draw. This setting prevents damage to a battery that can
 * occur when drawing current from an "empty" battery.
 *
 * @return a floating point number corresponding to the threshold voltage under which the controller
 * automatically switches to error state
 *         and prevents further current draw
 *
 * On failure, throws an exception or returns Y_CUTOFFVOLTAGE_INVALID.
 */
double YMotor::get_cutOffVoltage(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YMotor::CUTOFFVOLTAGE_INVALID;
        }
    }
    return _cutOffVoltage;
}

/**
 * Returns the current threshold (in mA) above which the controller automatically
 * switches to error state. A zero value means that there is no limit.
 *
 * @return an integer corresponding to the current threshold (in mA) above which the controller automatically
 *         switches to error state
 *
 * On failure, throws an exception or returns Y_OVERCURRENTLIMIT_INVALID.
 */
int YMotor::get_overCurrentLimit(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YMotor::OVERCURRENTLIMIT_INVALID;
        }
    }
    return _overCurrentLimit;
}

/**
 * Changes the current threshold (in mA) above which the controller automatically
 * switches to error state. A zero value means that there is no limit. Note that whatever the
 * current limit is, the controller switches to OVERCURRENT status if the current
 * goes above 32A, even for a very brief time.
 *
 * @param newval : an integer corresponding to the current threshold (in mA) above which the
 * controller automatically
 *         switches to error state
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YMotor::set_overCurrentLimit(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("overCurrentLimit", rest_val);
}

/**
 * Changes the PWM frequency used to control the motor. Low frequency is usually
 * more efficient and may help the motor to start, but an audible noise might be
 * generated. A higher frequency reduces the noise, but more energy is converted
 * into heat.
 *
 * @param newval : a floating point number corresponding to the PWM frequency used to control the motor
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YMotor::set_frequency(double newval)
{
    string rest_val;
    char buf[32]; sprintf(buf,"%d", (int)floor(newval * 65536.0 + 0.5)); rest_val = string(buf);
    return _setAttr("frequency", rest_val);
}

/**
 * Returns the PWM frequency used to control the motor.
 *
 * @return a floating point number corresponding to the PWM frequency used to control the motor
 *
 * On failure, throws an exception or returns Y_FREQUENCY_INVALID.
 */
double YMotor::get_frequency(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YMotor::FREQUENCY_INVALID;
        }
    }
    return _frequency;
}

/**
 * Returns the duration (in ms) during which the motor is driven at low frequency to help
 * it start up.
 *
 * @return an integer corresponding to the duration (in ms) during which the motor is driven at low
 * frequency to help
 *         it start up
 *
 * On failure, throws an exception or returns Y_STARTERTIME_INVALID.
 */
int YMotor::get_starterTime(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YMotor::STARTERTIME_INVALID;
        }
    }
    return _starterTime;
}

/**
 * Changes the duration (in ms) during which the motor is driven at low frequency to help
 * it start up.
 *
 * @param newval : an integer corresponding to the duration (in ms) during which the motor is driven
 * at low frequency to help
 *         it start up
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YMotor::set_starterTime(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("starterTime", rest_val);
}

/**
 * Returns the delay in milliseconds allowed for the controller to run autonomously without
 * receiving any instruction from the control process. When this delay has elapsed,
 * the controller automatically stops the motor and switches to FAILSAFE error.
 * Failsafe security is disabled when the value is zero.
 *
 * @return an integer corresponding to the delay in milliseconds allowed for the controller to run
 * autonomously without
 *         receiving any instruction from the control process
 *
 * On failure, throws an exception or returns Y_FAILSAFETIMEOUT_INVALID.
 */
int YMotor::get_failSafeTimeout(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YMotor::FAILSAFETIMEOUT_INVALID;
        }
    }
    return _failSafeTimeout;
}

/**
 * Changes the delay in milliseconds allowed for the controller to run autonomously without
 * receiving any instruction from the control process. When this delay has elapsed,
 * the controller automatically stops the motor and switches to FAILSAFE error.
 * Failsafe security is disabled when the value is zero.
 *
 * @param newval : an integer corresponding to the delay in milliseconds allowed for the controller to
 * run autonomously without
 *         receiving any instruction from the control process
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YMotor::set_failSafeTimeout(int newval)
{
    string rest_val;
    char buf[32]; sprintf(buf, "%d", newval); rest_val = string(buf);
    return _setAttr("failSafeTimeout", rest_val);
}

string YMotor::get_command(void)
{
    if (_cacheExpiration <= YAPI::GetTickCount()) {
        if (this->load(YAPI::DefaultCacheValidity) != YAPI_SUCCESS) {
            return YMotor::COMMAND_INVALID;
        }
    }
    return _command;
}

int YMotor::set_command(const string& newval)
{
    string rest_val;
    rest_val = newval;
    return _setAttr("command", rest_val);
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
 * Use the method YMotor.isOnline() to test if $THEFUNCTION$ is
 * indeed online at a given time. In case of ambiguity when looking for
 * $AFUNCTION$ by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * @param func : a string that uniquely characterizes $THEFUNCTION$
 *
 * @return a YMotor object allowing you to drive $THEFUNCTION$.
 */
YMotor* YMotor::FindMotor(string func)
{
    YMotor* obj = NULL;
    obj = (YMotor*) YFunction::_FindFromCache("Motor", func);
    if (obj == NULL) {
        obj = new YMotor(func);
        YFunction::_AddToCache("Motor", func, obj);
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
int YMotor::registerValueCallback(YMotorValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackMotor = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YMotor::_invokeValueCallback(string value)
{
    if (_valueCallbackMotor != NULL) {
        _valueCallbackMotor(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

/**
 * Rearms the controller failsafe timer. When the motor is running and the failsafe feature
 * is active, this function should be called periodically to prove that the control process
 * is running properly. Otherwise, the motor is automatically stopped after the specified
 * timeout. Calling a motor <i>set</i> function implicitely rearms the failsafe timer.
 */
int YMotor::keepALive(void)
{
    return this->set_command("K");
}

/**
 * Reset the controller state to IDLE. This function must be invoked explicitely
 * after any error condition is signaled.
 */
int YMotor::resetStatus(void)
{
    return this->set_motorStatus(Y_MOTORSTATUS_IDLE);
}

/**
 * Changes progressively the power sent to the moteur for a specific duration.
 *
 * @param targetPower : desired motor power, in percents (between -100% and +100%)
 * @param delay : duration (in ms) of the transition
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YMotor::drivingForceMove(double targetPower,int delay)
{
    return this->set_command(YapiWrapper::ysprintf("P%d,%d",(int) floor(targetPower*10+0.5),delay));
}

/**
 * Changes progressively the braking force applied to the motor for a specific duration.
 *
 * @param targetPower : desired braking force, in percents
 * @param delay : duration (in ms) of the transition
 *
 * @return YAPI_SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YMotor::brakingForceMove(double targetPower,int delay)
{
    return this->set_command(YapiWrapper::ysprintf("B%d,%d",(int) floor(targetPower*10+0.5),delay));
}

YMotor *YMotor::nextMotor(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YMotor::FindMotor(hwid);
}

YMotor* YMotor::FirstMotor(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("Motor", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YMotor::FindMotor(serial+"."+funcId);
}

//--- (end of YMotor implementation)

//--- (Motor functions)
//--- (end of Motor functions)
