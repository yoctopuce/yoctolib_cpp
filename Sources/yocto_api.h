/*********************************************************************
 *
 * $Id: yocto_api.h 10717 2013-03-27 11:18:20Z mvuilleu $
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

#ifndef YOCTO_API_H
#define YOCTO_API_H

#include "yapi/ydef.h"
#include "yapi/yjson.h"
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <stdexcept>

using std::string;
using std::vector;
using std::queue;
using std::map;

//--- (generated code: definitions)
class YModule; //forward declaration

typedef void (*YModuleLogCallback)(YModule *module, const string& log);
typedef enum {
    Y_PERSISTENTSETTINGS_LOADED = 0,
    Y_PERSISTENTSETTINGS_SAVED = 1,
    Y_PERSISTENTSETTINGS_MODIFIED = 2,
    Y_PERSISTENTSETTINGS_INVALID = -1,
} Y_PERSISTENTSETTINGS_enum;

typedef enum {
    Y_BEACON_OFF = 0,
    Y_BEACON_ON = 1,
    Y_BEACON_INVALID = -1,
} Y_BEACON_enum;

typedef enum {
    Y_USBBANDWIDTH_SIMPLE = 0,
    Y_USBBANDWIDTH_DOUBLE = 1,
    Y_USBBANDWIDTH_INVALID = -1,
} Y_USBBANDWIDTH_enum;

#define Y_PRODUCTNAME_INVALID           (YAPI::INVALID_STRING)
#define Y_SERIALNUMBER_INVALID          (YAPI::INVALID_STRING)
#define Y_LOGICALNAME_INVALID           (YAPI::INVALID_STRING)
#define Y_PRODUCTID_INVALID             (-1)
#define Y_PRODUCTRELEASE_INVALID        (-1)
#define Y_FIRMWARERELEASE_INVALID       (YAPI::INVALID_STRING)
#define Y_LUMINOSITY_INVALID            (-1)
#define Y_UPTIME_INVALID                (0xffffffff)
#define Y_USBCURRENT_INVALID            (0xffffffff)
#define Y_REBOOTCOUNTDOWN_INVALID       (0x80000000)
//--- (end of generated code: definitions)

// yInitAPI argument
const int Y_DETECT_NONE = 0;
const int Y_DETECT_USB = 1;
const int Y_DETECT_NET = 2;
const int Y_DETECT_ALL = (Y_DETECT_USB | Y_DETECT_NET);

// Forward-declaration
class YFunction;

/// prototype of the log callback
typedef void (*yLogFunction)(const string& msg);

/// prototype of the device arrival/update/removal callback
typedef void (*yDeviceUpdateCallback)(YModule *module);

/// prototype of the value calibration handlers
typedef vector<double>  floatArr;
typedef vector<int>     intArr;
typedef double (*yCalibrationHandler)(double rawValue, int calibType, vector<int> params, vector<double> rawValues, vector<double> refValues);

typedef YAPI_DEVICE     YDEV_DESCR;
typedef YAPI_FUNCTION   YFUN_DESCR;
#define Y_FUNCTIONDESCRIPTOR_INVALID      (-1)
#define Y_HARDWAREID_INVALID              (YAPI::INVALID_STRING)
#define Y_FUNCTIONID_INVALID              (YAPI::INVALID_STRING)
#define Y_FRIENDLYNAME_INVALID            (YAPI::INVALID_STRING)

// 
// Class used to report exceptions within Yocto-API
// Do not instantiate directly
//
class YAPI_Exception : public std::runtime_error {
public:
    YRETCODE errorType;
    explicit YAPI_Exception(YRETCODE errType, string errMsg) : std::runtime_error(errMsg), errorType(errType) { }
};

typedef enum {
    YAPI_DEV_ARRIVAL,
    YAPI_DEV_REMOVAL,
    YAPI_DEV_CHANGE,
    YAPI_DEV_LOG,
    YAPI_FUN_UPDATE,
    YAPI_FUN_VALUE,
    YAPI_FUN_REFRESH
} yapiEventType;

typedef struct{
    unsigned    type;
    union{
        YModule  *module;
        struct {
            YFunction   *fun;
            char        value[YOCTO_PUBVAL_LEN];
        };
    };
}yapiEvent;

// 
// YAPI Context
//
// This class provides C++-style entry points to lowlevcel functions defined to yapi.h
// Could be implemented by a singleton, we use static methods insead
//
class YAPI {
private:
    static  queue<yapiEvent>    _plug_events;
    static  queue<yapiEvent>    _data_events;
    static  u64                 _nextEnum;
    static  bool                _apiInitialized;
    static  map<int,yCalibrationHandler> _calibHandlers;
    static  void        _yapiLogFunctionFwd(const char *log, u32 loglen);
    static  void        _yapiDeviceArrivalCallbackFwd(YDEV_DESCR devdesc);
    static  void        _yapiDeviceRemovalCallbackFwd(YDEV_DESCR devdesc);
    static  void        _yapiDeviceChangeCallbackFwd(YDEV_DESCR devdesc);
    static  void        _yapiDeviceLogCallbackFwd(YDEV_DESCR devdesc);
public:
    static  void        _notifyLogs(YModule* mod);
    static  void        _yapiFunctionUpdateCallbackFwd(YFUN_DESCR fundesc, const char *value);
    static  double      _decimalToDouble(s16 val);
    static  s16         _doubleToDecimal(double val);
    static  yCalibrationHandler _getCalibrationHandler(int calibType);
    static  double      _applyCalibration(double rawValue, string calibParams, int calibOffset, double resolution);
    
    static  int         DefaultCacheValidity;
    static  bool        ExceptionsDisabled;
    static  const string      INVALID_STRING;

    // Callback functions
    static  yLogFunction            LogFunction;
    static  yDeviceUpdateCallback   DeviceArrivalCallback;
    static  yDeviceUpdateCallback   DeviceRemovalCallback;
    static  yDeviceUpdateCallback   DeviceChangeCallback;

    static const u32 DETECT_NONE = 0;
    static const u32 DETECT_USB  = 1;
    static const u32 DETECT_NET  = 2;
    static const u32 DETECT_ALL  = (Y_DETECT_USB | Y_DETECT_NET);

//--- (generated code: return codes)
       static const int SUCCESS               = 0;       // everything worked allright
       static const int NOT_INITIALIZED       = -1;      // call yInitAPI() first !
       static const int INVALID_ARGUMENT      = -2;      // one of the arguments passed to the function is invalid
       static const int NOT_SUPPORTED         = -3;      // the operation attempted is (currently) not supported
       static const int DEVICE_NOT_FOUND      = -4;      // the requested device is not reachable
       static const int VERSION_MISMATCH      = -5;      // the device firmware is incompatible with this API version
       static const int DEVICE_BUSY           = -6;      // the device is busy with another task and cannot answer
       static const int TIMEOUT               = -7;      // the device took too long to provide an answer
       static const int IO_ERROR              = -8;      // there was an I/O problem while talking to the device
       static const int NO_MORE_DATA          = -9;      // there is no more data to read from
       static const int EXHAUSTED             = -10;     // you have run out of a limited ressource, check the documentation
       static const int DOUBLE_ACCES          = -11;     // you have two process that try to acces to the same device
       static const int UNAUTHORIZED          = -12;     // unauthorized access to password-protected device
//--- (end of generated code: return codes)


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
    static string GetAPIVersion(void);


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
    static  YRETCODE    InitAPI(int mode, string& errmsg);

    /**
     * Frees dynamically allocated memory blocks used by the Yoctopuce library.
     * It is generally not required to call this function, unless you
     * want to free all dynamically allocated memory blocks in order to
     * track a memory leak for instance.
     * You should not call any other library function after calling
     * yFreeAPI(), or your program will crash.
     */
    static  void        FreeAPI(void);

    /**
     * Disables the use of exceptions to report runtime errors.
     * When exceptions are disabled, every function returns a specific
     * error value which depends on its type and which is documented in
     * this reference manual.
     */
    inline static  void        DisableExceptions(void)
    { YAPI::ExceptionsDisabled = true; }

    /**
     * Re-enables the use of exceptions for runtime error handling.
     * Be aware than when exceptions are enabled, every function that fails
     * triggers an exception. If the exception is not caught by the user code,
     * it  either fires the debugger or aborts (i.e. crash) the program.
     * On failure, throws an exception or returns a negative error code.
     */
    inline static  void        EnableExceptions(void)
    { YAPI::ExceptionsDisabled=false; }

    /**
     * Registers a log callback function. This callback will be called each time
     * the API have something to say. Quite usefull to debug the API.
     * 
     * @param logfun : a procedure taking a string parameter, or null
     *         to unregister a previously registered  callback.
     */
    static  void        RegisterLogFunction(yLogFunction logfun);

    /**
     * Register a callback function, to be called each time
     * a device is pluged. This callback will be invoked while yUpdateDeviceList
     * is running. You will have to call this function on a regular basis.
     * 
     * @param arrivalCallback : a procedure taking a YModule parameter, or null
     *         to unregister a previously registered  callback.
     */
    static  void        RegisterDeviceArrivalCallback(yDeviceUpdateCallback arrivalCallback);

    /**
     * Register a callback function, to be called each time
     * a device is unpluged. This callback will be invoked while yUpdateDeviceList
     * is running. You will have to call this function on a regular basis.
     * 
     * @param removalCallback : a procedure taking a YModule parameter, or null
     *         to unregister a previously registered  callback.
     */
    static  void        RegisterDeviceRemovalCallback(yDeviceUpdateCallback removalCallback);

    static  void        RegisterDeviceChangeCallback(yDeviceUpdateCallback changeCallback);

    // Register a new value calibration handler for a given calibration type
    //
    static void         RegisterCalibrationHandler(int calibrationType, yCalibrationHandler calibrationHandler);
    
    // Standard value calibration handler (n-point linear error correction)
    //
    static double       LinearCalibrationHandler(double rawValue, int calibType, intArr params, floatArr rawValues, floatArr refValues);
    
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
    static  YRETCODE    RegisterHub(const string& url, string& errmsg);
    
    /**
     *
     */
    static  YRETCODE    PreregisterHub(const string& url, string& errmsg);

    /**
     * Setup the Yoctopuce library to no more use modules connected on a previously
     * registered machine with RegisterHub.
     * 
     * @param url : a string containing either "usb" or the
     *         root URL of the hub to monitor
     */
    static  void        UnregisterHub(const string& url);

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
    static  YRETCODE    UpdateDeviceList(string& errmsg);
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
    static  YRETCODE    HandleEvents(string& errmsg);
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
    static  YRETCODE    Sleep(unsigned ms_duration, string& errmsg);
    /**
     * Returns the current value of a monotone millisecond-based time counter.
     * This counter can be used to compute delays in relation with
     * Yoctopuce devices, which also uses the milisecond as timebase.
     * 
     * @return a long integer corresponding to the millisecond counter.
     */
    static  u64         GetTickCount(void);
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
    static  bool        CheckLogicalName(const string& name);
};

    
// Wrappers to yapi low-level API
class YapiWrapper {
public:
    static  u16         getAPIVersion(string& version, string& date);
    static  YDEV_DESCR  getDevice(const string& device_str, string& errmsg);
    static  int         getAllDevices(vector<YDEV_DESCR>& buffer, string& errmsg);
    static  YRETCODE    getDeviceInfo(YDEV_DESCR devdesc, yDeviceSt& infos, string& errmsg);
    static  YFUN_DESCR  getFunction(const string& class_str, const string& function_str, string& errmsg);
    static  int         getFunctionsByClass(const string& class_str, YFUN_DESCR prevfundesc, vector<YFUN_DESCR>& buffer, int maxsize, string& errmsg);
    static  int         getFunctionsByDevice(YDEV_DESCR devdesc, YFUN_DESCR prevfundesc, vector<YFUN_DESCR>& buffer, int maxsize, string& errmsg);
    static  YDEV_DESCR  getDeviceByFunction(YFUN_DESCR fundesc, string& errmsg);
    static  YRETCODE    getFunctionInfo(YFUN_DESCR fundesc, YDEV_DESCR& devdescr, string& serial, string& funcId, string& funcName, string& funcVal, string& errmsg);
    // pure yapi mapper
    static  YRETCODE    updateDeviceList(bool forceupdate, string& errmsg);
    static  YRETCODE    handleEvents(string& errmsg);
    static  string      ysprintf(const char *fmt, ...);
};


// 
// YDevice Class (used internally)
//
// This class is used to cache device-level information
//
// In order to regroup multiple function queries on the same physical device,
// this class implements a device-wide API string cache (agnostic of API content).
// This is in addition to the function-specific cache implemented in YFunction.
//

class YDevice;

typedef void (*HTTPRequestCallback)(YDevice *device,void *context,YRETCODE returnval, const string& result,string& errmsg);

class YDevice
{
private:    
    // Static device-based JSON string cache
    static vector<YDevice*> _devCache;
    
    // Device cache entries
    YDEV_DESCR          _devdescr;    
    u64                 _cacheStamp; // used only by requestAPI method
    string              _cacheJson;  // used only ba requestAPI method
    vector<YFUN_DESCR>  _functions;
    char                _rootdevice[YOCTO_SERIAL_LEN];
    char                *_subpath;

  
    // Constructor is private, use getDevice factory method
    YDevice(YDEV_DESCR devdesc): _devdescr(devdesc), _cacheStamp(0), _subpath(NULL){ };
     YRETCODE   HTTPRequestPrepare(const string& request, string& fullrequest, char *errbuff);

public:
    static YDevice *getDevice(YDEV_DESCR devdescr);
    YRETCODE    HTTPRequestAsync(const string& request, HTTPRequestCallback callback, void *context, string& errmsg);
    YRETCODE    HTTPRequest(const string& request, string& buffer, string& errmsg);
    YRETCODE    requestAPI(string& apires, string& errmsg);
    YRETCODE    getFunctions(vector<YFUN_DESCR> **functions, string& errmsg);

};

typedef void (*YFunctionUpdateCallback)(YFunction *func, const string& functionValue);

/**
 * YFunction Class (virtual class, used internally)
 *
 * This is the parent class for all public objects representing device functions documented in
 * the high-level programming API. This abstract class does all the real job, but without 
 * knowledge of the specific function attributes.
 *
 * Instantiating a child class of YFunction does not cause any communication.
 * The instance simply keeps track of its function identifier, and will dynamically bind
 * to a matching device at the time it is really beeing used to read or set an attribute.
 * In order to allow true hot-plug replacement of one device by another, the binding stay
 * dynamic through the life of the object.
 *
 * The YFunction class implements a generic high-level cache for the attribute values of
 * the specified function, pre-parsed from the REST API string. For strongly typed languages
 * the cache variable is defined in the concrete subclass.
 */

std::ostream& operator<<( std::ostream &stream, YFunction &fun );
class YFunction {
protected:    
    // Protected attributes
    string      _className;
    string      _func;
    YRETCODE    _lastErrorType;
    string      _lastErrorMsg;
    u64         _cacheExpiration;
    YFUN_DESCR  _fundescr;
    void*                   _userData;
    YFunctionUpdateCallback _genCallback;
    
    // Constructor is protected. Use the device-specific factory function to instantiate
    YFunction(const string& classname, const string& func);

    // Method used to throw exceptions or save error type/message
    void        _throw(YRETCODE errType, string errMsg);

    // Method used to retrieve our unique function descriptor (may trigger a hub scan)
    YRETCODE    _getDescriptor(YFUN_DESCR& fundescr, string& errMsg);
    
    // Method used to retrieve our device object (may trigger a hub scan)
    YRETCODE    _getDevice(YDevice*& dev, string& errMsg);

    // Method used to find the next instance of our function
    YRETCODE    _nextFunction(string &hwId);
    
    // Function-specific method for parsing JSON output and caching result
    virtual int _parse(yJsonStateMachine& j) = 0;
    string      _parseString(yJsonStateMachine& j);

    YRETCODE    _buildSetRequest( const string& changeattr, const string  *changeval, string& request, string& errmsg);
    
    // Method used to change attributes
    YRETCODE    _setAttr(string attrname, string newvalue);
    
    // Method used to encode calibration points into fixed-point 16-bit integers
    string      _encodeCalibrationPoints(const floatArr& rawValues, const floatArr& refValues, double resolution, int calibrationOffset,const string& actualCparams);
    
    // Method used to decode calibration points from fixed-point 16-bit integers
    int         _decodeCalibrationPoints(const string& calibParams, floatArr& rawValues, floatArr& refValues, double resolution, int calibrationOffset);
    
    void        _registerFuncCallback(YFunction* func);
    void        _unregisterFuncCallback(YFunction* func);

    // Method used to send http request to the device (not the function)
    string      _request(const string& request);
    string      _download(const string& url);
    // Method used to upload a file to the device
    YRETCODE    _upload(const string& path, const string& content);
    string      _json_get_key(const string& json, const string& data);
    vector<string> _json_get_array(const string& json);

    
    
public:
    
    static const YFUN_DESCR FUNCTIONDESCRIPTOR_INVALID = Y_FUNCTIONDESCRIPTOR_INVALID;
    static const string     HARDWAREID_INVALID;
    static const string     FUNCTIONID_INVALID;
    static const string     FRIENDLYNAME_INVALID;
    
    /**
     * Returns a short text that describes the function in the form TYPE(NAME)=SERIAL&#46;FUNCTIONID.
     * More precisely,
     * TYPE       is the type of the function,
     * NAME       it the name used for the first access to the function,
     * SERIAL     is the serial number of the module if the module is connected or "unresolved", and
     * FUNCTIONID is  the hardware identifier of the function if the module is connected.
     * For example, this method returns Relay(MyCustomName.relay1)=RELAYLO1-123456.relay1 if the
     * module is already connected or Relay(BadCustomeName.relay1)=unresolved if the module has
     * not yet been connected. This method does not trigger any USB or TCP transaction and can therefore be used in
     * a debugger.
     * 
     * @return a string that describes the function
     *         (ex: Relay(MyCustomName.relay1)=RELAYLO1-123456.relay1)
     */
    string      describe(void);

    /**
     * Returns a global identifier of the function in the format MODULE_NAME&#46;FUNCTION_NAME.
     * The returned string uses the logical names of the module and of the function if they are defined,
     * otherwise the serial number of the module and the hardware identifier of the function
     * (for exemple: MyCustomName.relay1)
     * 
     * @return a string that uniquely identifies the function using logical names
     *         (ex: MyCustomName.relay1)
     * 
     * On failure, throws an exception or returns  Y_FRIENDLYNAME_INVALID.
     */
     virtual string      get_friendlyName(void);

    /**
     * Returns the unique hardware identifier of the function in the form SERIAL&#46;FUNCTIONID.
     * The unique hardware identifier is composed of the device serial
     * number and of the hardware identifier of the function. (for example RELAYLO1-123456.relay1)
     * 
     * @return a string that uniquely identifies the function (ex: RELAYLO1-123456.relay1)
     * 
     * On failure, throws an exception or returns  Y_HARDWAREID_INVALID.
     */
    string      get_hardwareId(void);

    /**
     * Returns the hardware identifier of the function, without reference to the module. For example
     * relay1
     * 
     * @return a string that identifies the function (ex: relay1)
     * 
     * On failure, throws an exception or returns  Y_FUNCTIONID_INVALID.
     */
    string      get_functionId(void);

    
    /**
     * Returns the numerical error code of the latest error with this function.
     * This method is mostly useful when using the Yoctopuce library with
     * exceptions disabled.
     * 
     * @return a number corresponding to the code of the latest error that occured while
     *         using this function object
     */
           YRETCODE    get_errorType(void);
    inline YRETCODE    errorType(void)
    {return this->get_errorType();}
    inline YRETCODE    errType(void)
    {return this->get_errorType();}
    
    /**
     * Returns the error message of the latest error with this function.
     * This method is mostly useful when using the Yoctopuce library with
     * exceptions disabled.
     * 
     * @return a string corresponding to the latest error message that occured while
     *         using this function object
     */
           string      get_errorMessage(void);
    inline string      errorMessage(void)
    {return this->get_errorMessage();}
    inline string      errMessage(void)
    {return this->get_errorMessage();}
    
    /**
     * Checks if the function is currently reachable, without raising any error.
     * If there is a cached value for the function in cache, that has not yet
     * expired, the device is considered reachable.
     * No exception is raised if there is an error while trying to contact the
     * device hosting the requested function.
     * 
     * @return true if the function can be reached, and false otherwise
     */
    bool        isOnline(void);

    /**
     * Preloads the function cache with a specified validity duration.
     * By default, whenever accessing a device, all function attributes
     * are kept in cache for the standard duration (5 ms). This method can be
     * used to temporarily mark the cache as valid for a longer period, in order
     * to reduce network trafic for instance.
     * 
     * @param msValidity : an integer corresponding to the validity attributed to the
     *         loaded function parameters, in milliseconds
     * 
     * @return YAPI_SUCCESS when the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    YRETCODE    load(int msValidity);

    /**
     * Gets the YModule object for the device on which the function is located.
     * If the function cannot be located on any module, the returned instance of
     * YModule is not shown as on-line.
     * 
     * @return an instance of YModule
     */
           YModule     *get_module(void);
    inline YModule     *module(void)
    {return this->get_module();}

    /**
     * Returns a unique identifier of type YFUN_DESCR corresponding to the function.
     * This identifier can be used to test if two instances of YFunction reference the same
     * physical function on the same physical device.
     * 
     * @return an identifier of type YFUN_DESCR.
     * 
     * If the function has never been contacted, the returned value is Y_FUNCTIONDESCRIPTOR_INVALID.
     */
           YFUN_DESCR     get_functionDescriptor(void);
    inline YFUN_DESCR     functionDescriptor(void)
    {return this->get_functionDescriptor();}

    /**
     * Returns the value of the userData attribute, as previously stored using method
     * set_userData.
     * This attribute is never touched directly by the API, and is at disposal of the caller to
     * store a context.
     * 
     * @return the object stored previously by the caller.
     */
           void        *get_userData(void);
    inline void        *userData(void)
    {return this->get_userData();}

    /**
     * Stores a user context provided as argument in the userData attribute of the function.
     * This attribute is never touched by the API, and is at disposal of the caller to store a context.
     * 
     * @param data : any kind of object to be stored
     * @noreturn
     */
           void        set_userData(void* data);
    inline void        setUserData(void* data)
    { this->set_userData(data);}
    
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
    void        registerValueCallback(YFunctionUpdateCallback callback);

    virtual void     advertiseValue(const string& value);
};

typedef void (*YModuleLogCallback)(YModule *module, const string& log);


//--- (generated code: YModule declaration)
/**
 * YModule Class: Module control interface
 * 
 * This interface is identical for all Yoctopuce USB modules.
 * It can be used to control the module global parameters, and
 * to enumerate the functions provided by each module.
 */
class YModule: public YFunction {
protected:
    // Attributes (function value cache)
    YModuleLogCallback _logCallback;
    string                   _logContinuation;
    string          _productName;
    string          _serialNumber;
    string          _logicalName;
    int             _productId;
    int             _productRelease;
    string          _firmwareRelease;
    Y_PERSISTENTSETTINGS_enum _persistentSettings;
    int             _luminosity;
    Y_BEACON_enum   _beacon;
    unsigned        _upTime;
    unsigned        _usbCurrent;
    int             _rebootCountdown;
    Y_USBBANDWIDTH_enum _usbBandwidth;
    // Static function object cache
    static std::map<string,YModule*> _ModuleCache;

    friend YModule *yFindModule(const string& func);
    friend YModule *yFirstModule(void);

    // Function-specific method for parsing of JSON output and caching result
    int             _parse(yJsonStateMachine& j);
    //--- (end of generated code: YModule declaration)
    
    //--- (generated code: YModule constructor)
    // Constructor is protected, use yFindModule factory function to instantiate
    YModule(const string& func): YFunction("Module", func)
    //--- (end of generated code: YModule constructor)
    //--- (generated code: Module initialization)
            ,_logCallback(NULL)
            ,_logContinuation("0")
            ,_productName(Y_PRODUCTNAME_INVALID)
            ,_serialNumber(Y_SERIALNUMBER_INVALID)
            ,_logicalName(Y_LOGICALNAME_INVALID)
            ,_productId(Y_PRODUCTID_INVALID)
            ,_productRelease(Y_PRODUCTRELEASE_INVALID)
            ,_firmwareRelease(Y_FIRMWARERELEASE_INVALID)
            ,_persistentSettings(Y_PERSISTENTSETTINGS_INVALID)
            ,_luminosity(Y_LUMINOSITY_INVALID)
            ,_beacon(Y_BEACON_INVALID)
            ,_upTime(Y_UPTIME_INVALID)
            ,_usbCurrent(Y_USBCURRENT_INVALID)
            ,_rebootCountdown(Y_REBOOTCOUNTDOWN_INVALID)
            ,_usbBandwidth(Y_USBBANDWIDTH_INVALID)
    //--- (end of generated code: Module initialization)
    {};

    // Method used to retrieve details of the nth function of our device
    YRETCODE        _getFunction(int idx, string& serial, string& funcId, string& funcName, string& funcVal, string& errMsg);
    
public:

    /**
     * Returns a global identifier of the function in the format MODULE_NAME&#46;FUNCTION_NAME.
     * The returned string uses the logical names of the module and of the function if they are defined,
     * otherwise the serial number of the module and the hardware identifier of the function
     * (for exemple: MyCustomName.relay1)
     * 
     * @return a string that uniquely identifies the function using logical names
     *         (ex: MyCustomName.relay1)
     * 
     * On failure, throws an exception or returns  Y_FRIENDLYNAME_INVALID.
     */
    virtual string      get_friendlyName(void);


    /**
     * Returns the number of functions (beside the "module" interface) available on the module.
     * 
     * @return the number of functions on the module
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             functionCount();
    
    /**
     * Retrieves the hardware identifier of the <i>n</i>th function on the module.
     * 
     * @param functionIndex : the index of the function for which the information is desired, starting at
     * 0 for the first function.
     * 
     * @return a string corresponding to the unambiguous hardware identifier of the requested module function
     * 
     * On failure, throws an exception or returns an empty string.
     */
    string          functionId(int functionIndex);
    
    /**
     * Retrieves the logical name of the <i>n</i>th function on the module.
     * 
     * @param functionIndex : the index of the function for which the information is desired, starting at
     * 0 for the first function.
     * 
     * @return a string corresponding to the logical name of the requested module function
     * 
     * On failure, throws an exception or returns an empty string.
     */
    string          functionName(int functionIndex);
    
    /**
     * Retrieves the advertised value of the <i>n</i>th function on the module.
     * 
     * @param functionIndex : the index of the function for which the information is desired, starting at
     * 0 for the first function.
     * 
     * @return a short string (up to 6 characters) corresponding to the advertised value of the requested
     * module function
     * 
     * On failure, throws an exception or returns an empty string.
     */
    string          functionValue(int functionIndex);
    
    
    
    /*
     * update Immutable Attributes form callback
     * 
     */
    void            setImmutableAttributes(yDeviceSt *infos);


     /**
     * 
     */
    void            registerLogCallback(YModuleLogCallback callback);

    void            checkLogs(void);

    

    //--- (generated code: YModule accessors declaration)

    static const string PRODUCTNAME_INVALID;
    static const string SERIALNUMBER_INVALID;
    static const string LOGICALNAME_INVALID;
    static const int      PRODUCTID_INVALID = -1;
    static const int      PRODUCTRELEASE_INVALID = -1;
    static const string FIRMWARERELEASE_INVALID;
    static const Y_PERSISTENTSETTINGS_enum PERSISTENTSETTINGS_LOADED = Y_PERSISTENTSETTINGS_LOADED;
    static const Y_PERSISTENTSETTINGS_enum PERSISTENTSETTINGS_SAVED = Y_PERSISTENTSETTINGS_SAVED;
    static const Y_PERSISTENTSETTINGS_enum PERSISTENTSETTINGS_MODIFIED = Y_PERSISTENTSETTINGS_MODIFIED;
    static const Y_PERSISTENTSETTINGS_enum PERSISTENTSETTINGS_INVALID = Y_PERSISTENTSETTINGS_INVALID;
    static const int      LUMINOSITY_INVALID = -1;
    static const Y_BEACON_enum BEACON_OFF = Y_BEACON_OFF;
    static const Y_BEACON_enum BEACON_ON = Y_BEACON_ON;
    static const Y_BEACON_enum BEACON_INVALID = Y_BEACON_INVALID;
    static const unsigned UPTIME_INVALID = 0xffffffff;
    static const unsigned USBCURRENT_INVALID = 0xffffffff;
    static const int      REBOOTCOUNTDOWN_INVALID = 0x80000000;
    static const Y_USBBANDWIDTH_enum USBBANDWIDTH_SIMPLE = Y_USBBANDWIDTH_SIMPLE;
    static const Y_USBBANDWIDTH_enum USBBANDWIDTH_DOUBLE = Y_USBBANDWIDTH_DOUBLE;
    static const Y_USBBANDWIDTH_enum USBBANDWIDTH_INVALID = Y_USBBANDWIDTH_INVALID;

    /**
     * Returns the commercial name of the module, as set by the factory.
     * 
     * @return a string corresponding to the commercial name of the module, as set by the factory
     * 
     * On failure, throws an exception or returns Y_PRODUCTNAME_INVALID.
     */
           string          get_productName(void);
    inline string          productName(void)
    { return this->get_productName(); }

    /**
     * Returns the serial number of the module, as set by the factory.
     * 
     * @return a string corresponding to the serial number of the module, as set by the factory
     * 
     * On failure, throws an exception or returns Y_SERIALNUMBER_INVALID.
     */
           string          get_serialNumber(void);
    inline string          serialNumber(void)
    { return this->get_serialNumber(); }

    /**
     * Returns the logical name of the module.
     * 
     * @return a string corresponding to the logical name of the module
     * 
     * On failure, throws an exception or returns Y_LOGICALNAME_INVALID.
     */
           string          get_logicalName(void);
    inline string          logicalName(void)
    { return this->get_logicalName(); }

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
    int             set_logicalName(const string& newval);
    inline int      setLogicalName(const string& newval)
    { return this->set_logicalName(newval); }

    /**
     * Returns the USB device identifier of the module.
     * 
     * @return an integer corresponding to the USB device identifier of the module
     * 
     * On failure, throws an exception or returns Y_PRODUCTID_INVALID.
     */
           int             get_productId(void);
    inline int             productId(void)
    { return this->get_productId(); }

    /**
     * Returns the hardware release version of the module.
     * 
     * @return an integer corresponding to the hardware release version of the module
     * 
     * On failure, throws an exception or returns Y_PRODUCTRELEASE_INVALID.
     */
           int             get_productRelease(void);
    inline int             productRelease(void)
    { return this->get_productRelease(); }

    /**
     * Returns the version of the firmware embedded in the module.
     * 
     * @return a string corresponding to the version of the firmware embedded in the module
     * 
     * On failure, throws an exception or returns Y_FIRMWARERELEASE_INVALID.
     */
           string          get_firmwareRelease(void);
    inline string          firmwareRelease(void)
    { return this->get_firmwareRelease(); }

    /**
     * Returns the current state of persistent module settings.
     * 
     * @return a value among Y_PERSISTENTSETTINGS_LOADED, Y_PERSISTENTSETTINGS_SAVED and
     * Y_PERSISTENTSETTINGS_MODIFIED corresponding to the current state of persistent module settings
     * 
     * On failure, throws an exception or returns Y_PERSISTENTSETTINGS_INVALID.
     */
           Y_PERSISTENTSETTINGS_enum get_persistentSettings(void);
    inline Y_PERSISTENTSETTINGS_enum persistentSettings(void)
    { return this->get_persistentSettings(); }

    int             set_persistentSettings(Y_PERSISTENTSETTINGS_enum newval);
    inline int      setPersistentSettings(Y_PERSISTENTSETTINGS_enum newval)
    { return this->set_persistentSettings(newval); }

    /**
     * Saves current settings in the nonvolatile memory of the module.
     * Warning: the number of allowed save operations during a module life is
     * limited (about 100000 cycles). Do not call this function within a loop.
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             saveToFlash(void);

    /**
     * Reloads the settings stored in the nonvolatile memory, as
     * when the module is powered on.
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             revertFromFlash(void);

    /**
     * Returns the luminosity of the  module informative leds (from 0 to 100).
     * 
     * @return an integer corresponding to the luminosity of the  module informative leds (from 0 to 100)
     * 
     * On failure, throws an exception or returns Y_LUMINOSITY_INVALID.
     */
           int             get_luminosity(void);
    inline int             luminosity(void)
    { return this->get_luminosity(); }

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
    int             set_luminosity(int newval);
    inline int      setLuminosity(int newval)
    { return this->set_luminosity(newval); }

    /**
     * Returns the state of the localization beacon.
     * 
     * @return either Y_BEACON_OFF or Y_BEACON_ON, according to the state of the localization beacon
     * 
     * On failure, throws an exception or returns Y_BEACON_INVALID.
     */
           Y_BEACON_enum   get_beacon(void);
    inline Y_BEACON_enum   beacon(void)
    { return this->get_beacon(); }

    /**
     * Turns on or off the module localization beacon.
     * 
     * @param newval : either Y_BEACON_OFF or Y_BEACON_ON
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_beacon(Y_BEACON_enum newval);
    inline int      setBeacon(Y_BEACON_enum newval)
    { return this->set_beacon(newval); }

    /**
     * Returns the number of milliseconds spent since the module was powered on.
     * 
     * @return an integer corresponding to the number of milliseconds spent since the module was powered on
     * 
     * On failure, throws an exception or returns Y_UPTIME_INVALID.
     */
           unsigned        get_upTime(void);
    inline unsigned        upTime(void)
    { return this->get_upTime(); }

    /**
     * Returns the current consumed by the module on the USB bus, in milli-amps.
     * 
     * @return an integer corresponding to the current consumed by the module on the USB bus, in milli-amps
     * 
     * On failure, throws an exception or returns Y_USBCURRENT_INVALID.
     */
           unsigned        get_usbCurrent(void);
    inline unsigned        usbCurrent(void)
    { return this->get_usbCurrent(); }

    /**
     * Returns the remaining number of seconds before the module restarts, or zero when no
     * reboot has been scheduled.
     * 
     * @return an integer corresponding to the remaining number of seconds before the module restarts, or zero when no
     *         reboot has been scheduled
     * 
     * On failure, throws an exception or returns Y_REBOOTCOUNTDOWN_INVALID.
     */
           int             get_rebootCountdown(void);
    inline int             rebootCountdown(void)
    { return this->get_rebootCountdown(); }

    int             set_rebootCountdown(int newval);
    inline int      setRebootCountdown(int newval)
    { return this->set_rebootCountdown(newval); }

    /**
     * Schedules a simple module reboot after the given number of seconds.
     * 
     * @param secBeforeReboot : number of seconds before rebooting
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             reboot(int secBeforeReboot);

    /**
     * Schedules a module reboot into special firmware update mode.
     * 
     * @param secBeforeReboot : number of seconds before rebooting
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             triggerFirmwareUpdate(int secBeforeReboot);

    /**
     * Returns the number of USB interfaces used by the module.
     * 
     * @return either Y_USBBANDWIDTH_SIMPLE or Y_USBBANDWIDTH_DOUBLE, according to the number of USB
     * interfaces used by the module
     * 
     * On failure, throws an exception or returns Y_USBBANDWIDTH_INVALID.
     */
           Y_USBBANDWIDTH_enum get_usbBandwidth(void);
    inline Y_USBBANDWIDTH_enum usbBandwidth(void)
    { return this->get_usbBandwidth(); }

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
    int             set_usbBandwidth(Y_USBBANDWIDTH_enum newval);
    inline int      setUsbBandwidth(Y_USBBANDWIDTH_enum newval)
    { return this->set_usbBandwidth(newval); }

    /**
     * Downloads the specified built-in file and returns a binary buffer with its content.
     * 
     * @param pathname : name of the new file to load
     * 
     * @return a binary buffer with the file content
     * 
     * On failure, throws an exception or returns an empty content.
     */
    string             download(string pathname);

    /**
     * Returns the icon of the module. The icon is a PNG image and does not
     * exceeds 1536 bytes.
     * 
     * @return a binary buffer with module icon, in png format.
     */
    string             get_icon2d();


    /**
     * Continues the module enumeration started using yFirstModule().
     * 
     * @return a pointer to a YModule object, corresponding to
     *         the next module found, or a null pointer
     *         if there are no more modules to enumerate.
     */
           YModule         *nextModule(void);
    inline YModule         *next(void)
    { return this->nextModule();}

    /**
     * Allows you to find a module from its serial number or from its logical name.
     * 
     * This function does not require that the module is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method YModule.isOnline() to test if the module is
     * indeed online at a given time. In case of ambiguity when looking for
     * a module by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     * 
     * @param func : a string containing either the serial number or
     *         the logical name of the desired module
     * 
     * @return a YModule object allowing you to drive the module
     *         or get additional information on the module.
     */
           static YModule* FindModule(const string& func);
    inline static YModule* Find(const string& func)
    { return YModule::FindModule(func);}
    /**
     * Starts the enumeration of modules currently accessible.
     * Use the method YModule.nextModule() to iterate on the
     * next modules.
     * 
     * @return a pointer to a YModule object, corresponding to
     *         the first module currently online, or a null pointer
     *         if there are none.
     */
           static YModule* FirstModule(void);
    inline static YModule* First(void)
    { return YModule::FirstModule();}
    //--- (end of generated code: YModule accessors declaration)
};


inline string yGetAPIVersion() 
{ return YAPI::GetAPIVersion(); }



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
inline YRETCODE yInitAPI(int mode, string& errmsg) 
{ return YAPI::InitAPI(mode,errmsg); }

/**
 * Frees dynamically allocated memory blocks used by the Yoctopuce library.
 * It is generally not required to call this function, unless you
 * want to free all dynamically allocated memory blocks in order to
 * track a memory leak for instance.
 * You should not call any other library function after calling
 * yFreeAPI(), or your program will crash.
 */
inline void yFreeAPI() 
{ YAPI::FreeAPI(); }

/**
 * Disables the use of exceptions to report runtime errors.
 * When exceptions are disabled, every function returns a specific
 * error value which depends on its type and which is documented in
 * this reference manual.
 */
inline void yDisableExceptions(void) 
{ YAPI::DisableExceptions(); }

/**
 * Re-enables the use of exceptions for runtime error handling.
 * Be aware than when exceptions are enabled, every function that fails
 * triggers an exception. If the exception is not caught by the user code,
 * it  either fires the debugger or aborts (i.e. crash) the program.
 * On failure, throws an exception or returns a negative error code.
 */
inline void yEnableExceptions(void)  { YAPI::EnableExceptions(); }

/**
 * Registers a log callback function. This callback will be called each time
 * the API have something to say. Quite usefull to debug the API.
 * 
 * @param logfun : a procedure taking a string parameter, or null
 *         to unregister a previously registered  callback.
 */
inline void yRegisterLogFunction(yLogFunction logfun)                               
{ YAPI::RegisterLogFunction(logfun); }

/**
 * Register a callback function, to be called each time
 * a device is pluged. This callback will be invoked while yUpdateDeviceList
 * is running. You will have to call this function on a regular basis.
 * 
 * @param arrivalCallback : a procedure taking a YModule parameter, or null
 *         to unregister a previously registered  callback.
 */
inline void yRegisterDeviceArrivalCallback(yDeviceUpdateCallback arrivalCallback)   
{ YAPI::RegisterDeviceArrivalCallback(arrivalCallback); }

/**
 * Register a callback function, to be called each time
 * a device is unpluged. This callback will be invoked while yUpdateDeviceList
 * is running. You will have to call this function on a regular basis.
 * 
 * @param removalCallback : a procedure taking a YModule parameter, or null
 *         to unregister a previously registered  callback.
 */
inline void yRegisterDeviceRemovalCallback(yDeviceUpdateCallback removalCallback)   
{ YAPI::RegisterDeviceRemovalCallback(removalCallback); }

/**
 * Changes the number of USB interfaces used by the module.
 * 
 * @param newval : either Y_USBBANDWIDTH_SIMPLE or Y_USBBANDWIDTH_DOUBLE, according to the number of
 * USB interfaces used by the module
 * 
 * @return the new value as recognized by the device.
 * 
 * On failure, throws an exception or returns Y_USBBANDWIDTH_INVALID.
 */
inline void yRegisterDeviceChangeCallback(yDeviceUpdateCallback removalCallback)    
{ YAPI::RegisterDeviceChangeCallback(removalCallback); }

// Register a new value calibration handler for a given calibration type
//
inline void yRegisterCalibrationHandler(int calibrationType, yCalibrationHandler calibrationHandler)
{ YAPI::RegisterCalibrationHandler(calibrationType, calibrationHandler); }

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
inline YRETCODE yRegisterHub(const string& url, string& errmsg) 
{ return YAPI::RegisterHub(url,errmsg); }

/**
 *
 */
inline YRETCODE yPreregisterHub(const string& url, string& errmsg) 
{ return YAPI::PreregisterHub(url,errmsg); }

/**
 * Setup the Yoctopuce library to no more use modules connected on a previously
 * registered machine with RegisterHub.
 * 
 * @param url : a string containing either "usb" or the
 *         root URL of the hub to monitor
 */
inline void yUnregisterHub(const string& url) 
{ YAPI::UnregisterHub(url); }



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
inline YRETCODE yUpdateDeviceList(string& errmsg) 
{ return YAPI::UpdateDeviceList(errmsg); }

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
inline YRETCODE yHandleEvents(string& errmsg)
{ return YAPI::HandleEvents(errmsg); }

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
inline YRETCODE ySleep(unsigned ms_duration, string& errmsg)
{ return YAPI::Sleep(ms_duration, errmsg); }

/**
 * Returns the current value of a monotone millisecond-based time counter.
 * This counter can be used to compute delays in relation with
 * Yoctopuce devices, which also uses the milisecond as timebase.
 * 
 * @return a long integer corresponding to the millisecond counter.
 */
inline u64 yGetTickCount(void)
{ return YAPI::GetTickCount(); }

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
inline bool yCheckLogicalName(const string& name)
{ return YAPI::CheckLogicalName(name); }

//--- (generated code: Module functions declaration)

/**
 * Allows you to find a module from its serial number or from its logical name.
 * 
 * This function does not require that the module is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method YModule.isOnline() to test if the module is
 * indeed online at a given time. In case of ambiguity when looking for
 * a module by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 * 
 * @param func : a string containing either the serial number or
 *         the logical name of the desired module
 * 
 * @return a YModule object allowing you to drive the module
 *         or get additional information on the module.
 */
inline YModule* yFindModule(const string& func)
{ return YModule::FindModule(func);}
/**
 * Starts the enumeration of modules currently accessible.
 * Use the method YModule.nextModule() to iterate on the
 * next modules.
 * 
 * @return a pointer to a YModule object, corresponding to
 *         the first module currently online, or a null pointer
 *         if there are none.
 */
inline YModule* yFirstModule(void)
{ return YModule::FirstModule();}

//--- (end of generated code: Module functions declaration)


#endif
