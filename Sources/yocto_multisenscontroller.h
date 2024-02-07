/*********************************************************************
 *
 *  $Id: yocto_multisenscontroller.h 49575 2022-04-27 09:39:21Z seb $
 *
 *  Declares yFindMultiSensController(), the high-level API for MultiSensController functions
 *
 *  - - - - - - - - - License information: - - - - - - - - -
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


#ifndef YOCTO_MULTISENSCONTROLLER_H
#define YOCTO_MULTISENSCONTROLLER_H

#include <cfloat>
#include <cmath>

#include "yocto_api.h"

#ifdef YOCTOLIB_NAMESPACE
namespace YOCTOLIB_NAMESPACE
{
#endif

//--- (YMultiSensController return codes)
//--- (end of YMultiSensController return codes)
//--- (YMultiSensController yapiwrapper)
//--- (end of YMultiSensController yapiwrapper)
//--- (YMultiSensController definitions)
class YMultiSensController; // forward declaration

typedef void (*YMultiSensControllerValueCallback)(YMultiSensController *func, const string& functionValue);
#ifndef _Y_MAINTENANCEMODE_ENUM
#define _Y_MAINTENANCEMODE_ENUM
typedef enum {
    Y_MAINTENANCEMODE_FALSE = 0,
    Y_MAINTENANCEMODE_TRUE = 1,
    Y_MAINTENANCEMODE_INVALID = -1,
} Y_MAINTENANCEMODE_enum;
#endif
#define Y_NSENSORS_INVALID              (YAPI_INVALID_UINT)
#define Y_MAXSENSORS_INVALID            (YAPI_INVALID_UINT)
#define Y_LASTADDRESSDETECTED_INVALID   (YAPI_INVALID_UINT)
#define Y_COMMAND_INVALID               (YAPI_INVALID_STRING)
//--- (end of YMultiSensController definitions)

//--- (YMultiSensController declaration)
/**
 * YMultiSensController Class: Sensor chain configuration interface, available for instance in the
 * Yocto-Temperature-IR
 *
 * The YMultiSensController class allows you to setup a customized
 * sensor chain on devices featuring that functionality.
 */
class YOCTO_CLASS_EXPORT YMultiSensController: public YFunction {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of YMultiSensController declaration)
protected:
    //--- (YMultiSensController attributes)
    // Attributes (function value cache)
    int             _nSensors;
    int             _maxSensors;
    Y_MAINTENANCEMODE_enum _maintenanceMode;
    int             _lastAddressDetected;
    string          _command;
    YMultiSensControllerValueCallback _valueCallbackMultiSensController;

    friend YMultiSensController *yFindMultiSensController(const string& func);
    friend YMultiSensController *yFirstMultiSensController(void);

    // Function-specific method for parsing of JSON output and caching result
    virtual int     _parseAttr(YJSONObject *json_val);

    // Constructor is protected, use yFindMultiSensController factory function to instantiate
    YMultiSensController(const string& func);
    //--- (end of YMultiSensController attributes)

public:
    virtual ~YMultiSensController();
    //--- (YMultiSensController accessors declaration)

    static const int NSENSORS_INVALID = YAPI_INVALID_UINT;
    static const int MAXSENSORS_INVALID = YAPI_INVALID_UINT;
    static const Y_MAINTENANCEMODE_enum MAINTENANCEMODE_FALSE = Y_MAINTENANCEMODE_FALSE;
    static const Y_MAINTENANCEMODE_enum MAINTENANCEMODE_TRUE = Y_MAINTENANCEMODE_TRUE;
    static const Y_MAINTENANCEMODE_enum MAINTENANCEMODE_INVALID = Y_MAINTENANCEMODE_INVALID;
    static const int LASTADDRESSDETECTED_INVALID = YAPI_INVALID_UINT;
    static const string COMMAND_INVALID;

    /**
     * Returns the number of sensors to poll.
     *
     * @return an integer corresponding to the number of sensors to poll
     *
     * On failure, throws an exception or returns YMultiSensController::NSENSORS_INVALID.
     */
    int                 get_nSensors(void);

    inline int          nSensors(void)
    { return this->get_nSensors(); }

    /**
     * Changes the number of sensors to poll. Remember to call the
     * saveToFlash() method of the module if the
     * modification must be kept. It is recommended to restart the
     * device with  module->reboot() after modifying
     * (and saving) this settings.
     *
     * @param newval : an integer corresponding to the number of sensors to poll
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_nSensors(int newval);
    inline int      setNSensors(int newval)
    { return this->set_nSensors(newval); }

    /**
     * Returns the maximum configurable sensor count allowed on this device.
     *
     * @return an integer corresponding to the maximum configurable sensor count allowed on this device
     *
     * On failure, throws an exception or returns YMultiSensController::MAXSENSORS_INVALID.
     */
    int                 get_maxSensors(void);

    inline int          maxSensors(void)
    { return this->get_maxSensors(); }

    /**
     * Returns true when the device is in maintenance mode.
     *
     * @return either YMultiSensController::MAINTENANCEMODE_FALSE or
     * YMultiSensController::MAINTENANCEMODE_TRUE, according to true when the device is in maintenance mode
     *
     * On failure, throws an exception or returns YMultiSensController::MAINTENANCEMODE_INVALID.
     */
    Y_MAINTENANCEMODE_enum get_maintenanceMode(void);

    inline Y_MAINTENANCEMODE_enum maintenanceMode(void)
    { return this->get_maintenanceMode(); }

    /**
     * Changes the device mode to enable maintenance and to stop sensor polling.
     * This way, the device does not automatically restart when it cannot
     * communicate with one of the sensors.
     *
     * @param newval : either YMultiSensController::MAINTENANCEMODE_FALSE or
     * YMultiSensController::MAINTENANCEMODE_TRUE, according to the device mode to enable maintenance and
     * to stop sensor polling
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_maintenanceMode(Y_MAINTENANCEMODE_enum newval);
    inline int      setMaintenanceMode(Y_MAINTENANCEMODE_enum newval)
    { return this->set_maintenanceMode(newval); }

    /**
     * Returns the I2C address of the most recently detected sensor. This method can
     * be used to in case of I2C communication error to determine what is the
     * last sensor that can be reached, or after a call to setupAddress
     * to make sure that the address change was properly processed.
     *
     * @return an integer corresponding to the I2C address of the most recently detected sensor
     *
     * On failure, throws an exception or returns YMultiSensController::LASTADDRESSDETECTED_INVALID.
     */
    int                 get_lastAddressDetected(void);

    inline int          lastAddressDetected(void)
    { return this->get_lastAddressDetected(); }

    string              get_command(void);

    inline string       command(void)
    { return this->get_command(); }

    int             set_command(const string& newval);
    inline int      setCommand(const string& newval)
    { return this->set_command(newval); }

    /**
     * Retrieves a multi-sensor controller for a given identifier.
     * The identifier can be specified using several formats:
     * <ul>
     * <li>FunctionLogicalName</li>
     * <li>ModuleSerialNumber.FunctionIdentifier</li>
     * <li>ModuleSerialNumber.FunctionLogicalName</li>
     * <li>ModuleLogicalName.FunctionIdentifier</li>
     * <li>ModuleLogicalName.FunctionLogicalName</li>
     * </ul>
     *
     * This function does not require that the multi-sensor controller is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method isOnline() to test if the multi-sensor controller is
     * indeed online at a given time. In case of ambiguity when looking for
     * a multi-sensor controller by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     *
     * If a call to this object's is_online() method returns FALSE although
     * you are certain that the matching device is plugged, make sure that you did
     * call registerHub() at application initialization time.
     *
     * @param func : a string that uniquely characterizes the multi-sensor controller, for instance
     *         YTEMPIR1.multiSensController.
     *
     * @return a YMultiSensController object allowing you to drive the multi-sensor controller.
     */
    static YMultiSensController* FindMultiSensController(string func);

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
    virtual int         registerValueCallback(YMultiSensControllerValueCallback callback);
    using YFunction::registerValueCallback;

    virtual int         _invokeValueCallback(string value);

    /**
     * Configures the I2C address of the only sensor connected to the device.
     * It is recommended to put the the device in maintenance mode before
     * changing sensor addresses.  This method is only intended to work with a single
     * sensor connected to the device. If several sensors are connected, the result
     * is unpredictable.
     *
     * Note that the device is expecting to find a sensor or a string of sensors with specific
     * addresses. Check the device documentation to find out which addresses should be used.
     *
     * @param addr : new address of the connected sensor
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *         On failure, throws an exception or returns a negative error code.
     */
    virtual int         setupAddress(int addr);

    /**
     * Triggers the I2C address detection procedure for the only sensor connected to the device.
     * This method is only intended to work with a single sensor connected to the device.
     * If several sensors are connected, the result is unpredictable.
     *
     * @return the I2C address of the detected sensor, or 0 if none is found
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         get_sensorAddress(void);


    inline static YMultiSensController *Find(string func)
    { return YMultiSensController::FindMultiSensController(func); }

    /**
     * Continues the enumeration of multi-sensor controllers started using yFirstMultiSensController().
     * Caution: You can't make any assumption about the returned multi-sensor controllers order.
     * If you want to find a specific a multi-sensor controller, use MultiSensController.findMultiSensController()
     * and a hardwareID or a logical name.
     *
     * @return a pointer to a YMultiSensController object, corresponding to
     *         a multi-sensor controller currently online, or a NULL pointer
     *         if there are no more multi-sensor controllers to enumerate.
     */
           YMultiSensController *nextMultiSensController(void);
    inline YMultiSensController *next(void)
    { return this->nextMultiSensController();}

    /**
     * Starts the enumeration of multi-sensor controllers currently accessible.
     * Use the method YMultiSensController::nextMultiSensController() to iterate on
     * next multi-sensor controllers.
     *
     * @return a pointer to a YMultiSensController object, corresponding to
     *         the first multi-sensor controller currently online, or a NULL pointer
     *         if there are none.
     */
           static YMultiSensController *FirstMultiSensController(void);
    inline static YMultiSensController *First(void)
    { return YMultiSensController::FirstMultiSensController();}
#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of YMultiSensController accessors declaration)
};

//--- (YMultiSensController functions declaration)

/**
 * Retrieves a multi-sensor controller for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that the multi-sensor controller is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the multi-sensor controller is
 * indeed online at a given time. In case of ambiguity when looking for
 * a multi-sensor controller by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the multi-sensor controller, for instance
 *         YTEMPIR1.multiSensController.
 *
 * @return a YMultiSensController object allowing you to drive the multi-sensor controller.
 */
inline YMultiSensController *yFindMultiSensController(const string& func)
{ return YMultiSensController::FindMultiSensController(func);}
/**
 * Starts the enumeration of multi-sensor controllers currently accessible.
 * Use the method YMultiSensController::nextMultiSensController() to iterate on
 * next multi-sensor controllers.
 *
 * @return a pointer to a YMultiSensController object, corresponding to
 *         the first multi-sensor controller currently online, or a NULL pointer
 *         if there are none.
 */
inline YMultiSensController *yFirstMultiSensController(void)
{ return YMultiSensController::FirstMultiSensController();}

//--- (end of YMultiSensController functions declaration)

#ifdef YOCTOLIB_NAMESPACE
// end of namespace definition
}
#endif

#endif
