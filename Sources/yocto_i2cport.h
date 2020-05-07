/*********************************************************************
 *
 *  $Id: yocto_i2cport.h 40195 2020-04-29 21:14:12Z mvuilleu $
 *
 *  Declares yFindI2cPort(), the high-level API for I2cPort functions
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


#ifndef YOCTO_I2CPORT_H
#define YOCTO_I2CPORT_H

#include <cfloat>
#include <cmath>

#include "yocto_api.h"

#ifdef YOCTOLIB_NAMESPACE
namespace YOCTOLIB_NAMESPACE
{
#endif

//--- (YI2cPort return codes)
//--- (end of YI2cPort return codes)
//--- (YI2cPort yapiwrapper)
//--- (end of YI2cPort yapiwrapper)
//--- (YI2cPort definitions)
class YI2cPort; // forward declaration

typedef void (*YI2cPortValueCallback)(YI2cPort *func, const string& functionValue);
#ifndef _Y_I2CVOLTAGELEVEL_ENUM
#define _Y_I2CVOLTAGELEVEL_ENUM
typedef enum {
    Y_I2CVOLTAGELEVEL_OFF = 0,
    Y_I2CVOLTAGELEVEL_3V3 = 1,
    Y_I2CVOLTAGELEVEL_1V8 = 2,
    Y_I2CVOLTAGELEVEL_INVALID = -1,
} Y_I2CVOLTAGELEVEL_enum;
#endif
#define Y_RXCOUNT_INVALID               (YAPI_INVALID_UINT)
#define Y_TXCOUNT_INVALID               (YAPI_INVALID_UINT)
#define Y_ERRCOUNT_INVALID              (YAPI_INVALID_UINT)
#define Y_RXMSGCOUNT_INVALID            (YAPI_INVALID_UINT)
#define Y_TXMSGCOUNT_INVALID            (YAPI_INVALID_UINT)
#define Y_LASTMSG_INVALID               (YAPI_INVALID_STRING)
#define Y_CURRENTJOB_INVALID            (YAPI_INVALID_STRING)
#define Y_STARTUPJOB_INVALID            (YAPI_INVALID_STRING)
#define Y_JOBMAXTASK_INVALID            (YAPI_INVALID_UINT)
#define Y_JOBMAXSIZE_INVALID            (YAPI_INVALID_UINT)
#define Y_COMMAND_INVALID               (YAPI_INVALID_STRING)
#define Y_PROTOCOL_INVALID              (YAPI_INVALID_STRING)
#define Y_I2CMODE_INVALID               (YAPI_INVALID_STRING)
//--- (end of YI2cPort definitions)

//--- (YI2cPort declaration)
/**
 * YI2cPort Class: I2C port control interface, available for instance in the Yocto-I2C
 *
 * The YI2cPort classe allows you to fully drive a Yoctopuce I2C port.
 * It can be used to send and receive data, and to configure communication
 * parameters (baud rate, etc).
 * Note that Yoctopuce I2C ports are not exposed as virtual COM ports.
 * They are meant to be used in the same way as all Yoctopuce devices.
 */
class YOCTO_CLASS_EXPORT YI2cPort: public YFunction {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of YI2cPort declaration)
protected:
    //--- (YI2cPort attributes)
    // Attributes (function value cache)
    int             _rxCount;
    int             _txCount;
    int             _errCount;
    int             _rxMsgCount;
    int             _txMsgCount;
    string          _lastMsg;
    string          _currentJob;
    string          _startupJob;
    int             _jobMaxTask;
    int             _jobMaxSize;
    string          _command;
    string          _protocol;
    Y_I2CVOLTAGELEVEL_enum _i2cVoltageLevel;
    string          _i2cMode;
    YI2cPortValueCallback _valueCallbackI2cPort;
    int             _rxptr;
    string          _rxbuff;
    int             _rxbuffptr;

    friend YI2cPort *yFindI2cPort(const string& func);
    friend YI2cPort *yFirstI2cPort(void);

    // Function-specific method for parsing of JSON output and caching result
    virtual int     _parseAttr(YJSONObject *json_val);

    // Constructor is protected, use yFindI2cPort factory function to instantiate
    YI2cPort(const string& func);
    //--- (end of YI2cPort attributes)

public:
    virtual ~YI2cPort();
    //--- (YI2cPort accessors declaration)

    static const int RXCOUNT_INVALID = YAPI_INVALID_UINT;
    static const int TXCOUNT_INVALID = YAPI_INVALID_UINT;
    static const int ERRCOUNT_INVALID = YAPI_INVALID_UINT;
    static const int RXMSGCOUNT_INVALID = YAPI_INVALID_UINT;
    static const int TXMSGCOUNT_INVALID = YAPI_INVALID_UINT;
    static const string LASTMSG_INVALID;
    static const string CURRENTJOB_INVALID;
    static const string STARTUPJOB_INVALID;
    static const int JOBMAXTASK_INVALID = YAPI_INVALID_UINT;
    static const int JOBMAXSIZE_INVALID = YAPI_INVALID_UINT;
    static const string COMMAND_INVALID;
    static const string PROTOCOL_INVALID;
    static const Y_I2CVOLTAGELEVEL_enum I2CVOLTAGELEVEL_OFF = Y_I2CVOLTAGELEVEL_OFF;
    static const Y_I2CVOLTAGELEVEL_enum I2CVOLTAGELEVEL_3V3 = Y_I2CVOLTAGELEVEL_3V3;
    static const Y_I2CVOLTAGELEVEL_enum I2CVOLTAGELEVEL_1V8 = Y_I2CVOLTAGELEVEL_1V8;
    static const Y_I2CVOLTAGELEVEL_enum I2CVOLTAGELEVEL_INVALID = Y_I2CVOLTAGELEVEL_INVALID;
    static const string I2CMODE_INVALID;

    /**
     * Returns the total number of bytes received since last reset.
     *
     * @return an integer corresponding to the total number of bytes received since last reset
     *
     * On failure, throws an exception or returns Y_RXCOUNT_INVALID.
     */
    int                 get_rxCount(void);

    inline int          rxCount(void)
    { return this->get_rxCount(); }

    /**
     * Returns the total number of bytes transmitted since last reset.
     *
     * @return an integer corresponding to the total number of bytes transmitted since last reset
     *
     * On failure, throws an exception or returns Y_TXCOUNT_INVALID.
     */
    int                 get_txCount(void);

    inline int          txCount(void)
    { return this->get_txCount(); }

    /**
     * Returns the total number of communication errors detected since last reset.
     *
     * @return an integer corresponding to the total number of communication errors detected since last reset
     *
     * On failure, throws an exception or returns Y_ERRCOUNT_INVALID.
     */
    int                 get_errCount(void);

    inline int          errCount(void)
    { return this->get_errCount(); }

    /**
     * Returns the total number of messages received since last reset.
     *
     * @return an integer corresponding to the total number of messages received since last reset
     *
     * On failure, throws an exception or returns Y_RXMSGCOUNT_INVALID.
     */
    int                 get_rxMsgCount(void);

    inline int          rxMsgCount(void)
    { return this->get_rxMsgCount(); }

    /**
     * Returns the total number of messages send since last reset.
     *
     * @return an integer corresponding to the total number of messages send since last reset
     *
     * On failure, throws an exception or returns Y_TXMSGCOUNT_INVALID.
     */
    int                 get_txMsgCount(void);

    inline int          txMsgCount(void)
    { return this->get_txMsgCount(); }

    /**
     * Returns the latest message fully received (for Line and Frame protocols).
     *
     * @return a string corresponding to the latest message fully received (for Line and Frame protocols)
     *
     * On failure, throws an exception or returns Y_LASTMSG_INVALID.
     */
    string              get_lastMsg(void);

    inline string       lastMsg(void)
    { return this->get_lastMsg(); }

    /**
     * Returns the name of the job file currently in use.
     *
     * @return a string corresponding to the name of the job file currently in use
     *
     * On failure, throws an exception or returns Y_CURRENTJOB_INVALID.
     */
    string              get_currentJob(void);

    inline string       currentJob(void)
    { return this->get_currentJob(); }

    /**
     * Selects a job file to run immediately. If an empty string is
     * given as argument, stops running current job file.
     *
     * @param newval : a string
     *
     * @return YAPI_SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_currentJob(const string& newval);
    inline int      setCurrentJob(const string& newval)
    { return this->set_currentJob(newval); }

    /**
     * Returns the job file to use when the device is powered on.
     *
     * @return a string corresponding to the job file to use when the device is powered on
     *
     * On failure, throws an exception or returns Y_STARTUPJOB_INVALID.
     */
    string              get_startupJob(void);

    inline string       startupJob(void)
    { return this->get_startupJob(); }

    /**
     * Changes the job to use when the device is powered on.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     *
     * @param newval : a string corresponding to the job to use when the device is powered on
     *
     * @return YAPI_SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_startupJob(const string& newval);
    inline int      setStartupJob(const string& newval)
    { return this->set_startupJob(newval); }

    /**
     * Returns the maximum number of tasks in a job that the device can handle.
     *
     * @return an integer corresponding to the maximum number of tasks in a job that the device can handle
     *
     * On failure, throws an exception or returns Y_JOBMAXTASK_INVALID.
     */
    int                 get_jobMaxTask(void);

    inline int          jobMaxTask(void)
    { return this->get_jobMaxTask(); }

    /**
     * Returns maximum size allowed for job files.
     *
     * @return an integer corresponding to maximum size allowed for job files
     *
     * On failure, throws an exception or returns Y_JOBMAXSIZE_INVALID.
     */
    int                 get_jobMaxSize(void);

    inline int          jobMaxSize(void)
    { return this->get_jobMaxSize(); }

    string              get_command(void);

    inline string       command(void)
    { return this->get_command(); }

    int             set_command(const string& newval);
    inline int      setCommand(const string& newval)
    { return this->set_command(newval); }

    /**
     * Returns the type of protocol used to send I2C messages, as a string.
     * Possible values are
     * "Line" for messages separated by LF or
     * "Char" for continuous stream of codes.
     *
     * @return a string corresponding to the type of protocol used to send I2C messages, as a string
     *
     * On failure, throws an exception or returns Y_PROTOCOL_INVALID.
     */
    string              get_protocol(void);

    inline string       protocol(void)
    { return this->get_protocol(); }

    /**
     * Changes the type of protocol used to send I2C messages.
     * Possible values are
     * "Line" for messages separated by LF or
     * "Char" for continuous stream of codes.
     * The suffix "/[wait]ms" can be added to reduce the transmit rate so that there
     * is always at lest the specified number of milliseconds between each message sent.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     *
     * @param newval : a string corresponding to the type of protocol used to send I2C messages
     *
     * @return YAPI_SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_protocol(const string& newval);
    inline int      setProtocol(const string& newval)
    { return this->set_protocol(newval); }

    /**
     * Returns the voltage level used on the I2C bus.
     *
     * @return a value among Y_I2CVOLTAGELEVEL_OFF, Y_I2CVOLTAGELEVEL_3V3 and Y_I2CVOLTAGELEVEL_1V8
     * corresponding to the voltage level used on the I2C bus
     *
     * On failure, throws an exception or returns Y_I2CVOLTAGELEVEL_INVALID.
     */
    Y_I2CVOLTAGELEVEL_enum get_i2cVoltageLevel(void);

    inline Y_I2CVOLTAGELEVEL_enum i2cVoltageLevel(void)
    { return this->get_i2cVoltageLevel(); }

    /**
     * Changes the voltage level used on the I2C bus.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     *
     * @param newval : a value among Y_I2CVOLTAGELEVEL_OFF, Y_I2CVOLTAGELEVEL_3V3 and
     * Y_I2CVOLTAGELEVEL_1V8 corresponding to the voltage level used on the I2C bus
     *
     * @return YAPI_SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_i2cVoltageLevel(Y_I2CVOLTAGELEVEL_enum newval);
    inline int      setI2cVoltageLevel(Y_I2CVOLTAGELEVEL_enum newval)
    { return this->set_i2cVoltageLevel(newval); }

    /**
     * Returns the I2C port communication parameters, as a string such as
     * "400kbps,2000ms,NoRestart". The string includes the baud rate, the
     * recovery delay after communications errors, and if needed the option
     * NoRestart to use a Stop/Start sequence instead of the
     * Restart state when performing read on the I2C bus.
     *
     * @return a string corresponding to the I2C port communication parameters, as a string such as
     *         "400kbps,2000ms,NoRestart"
     *
     * On failure, throws an exception or returns Y_I2CMODE_INVALID.
     */
    string              get_i2cMode(void);

    inline string       i2cMode(void)
    { return this->get_i2cMode(); }

    /**
     * Changes the I2C port communication parameters, with a string such as
     * "400kbps,2000ms". The string includes the baud rate, the
     * recovery delay after communications errors, and if needed the option
     * NoRestart to use a Stop/Start sequence instead of the
     * Restart state when performing read on the I2C bus.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     *
     * @param newval : a string corresponding to the I2C port communication parameters, with a string such as
     *         "400kbps,2000ms"
     *
     * @return YAPI_SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_i2cMode(const string& newval);
    inline int      setI2cMode(const string& newval)
    { return this->set_i2cMode(newval); }

    /**
     * Retrieves an I2C port for a given identifier.
     * The identifier can be specified using several formats:
     * <ul>
     * <li>FunctionLogicalName</li>
     * <li>ModuleSerialNumber.FunctionIdentifier</li>
     * <li>ModuleSerialNumber.FunctionLogicalName</li>
     * <li>ModuleLogicalName.FunctionIdentifier</li>
     * <li>ModuleLogicalName.FunctionLogicalName</li>
     * </ul>
     *
     * This function does not require that the I2C port is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method YI2cPort.isOnline() to test if the I2C port is
     * indeed online at a given time. In case of ambiguity when looking for
     * an I2C port by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     *
     * If a call to this object's is_online() method returns FALSE although
     * you are certain that the matching device is plugged, make sure that you did
     * call registerHub() at application initialization time.
     *
     * @param func : a string that uniquely characterizes the I2C port, for instance
     *         YI2CMK01.i2cPort.
     *
     * @return a YI2cPort object allowing you to drive the I2C port.
     */
    static YI2cPort*    FindI2cPort(string func);

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
    virtual int         registerValueCallback(YI2cPortValueCallback callback);
    using YFunction::registerValueCallback;

    virtual int         _invokeValueCallback(string value);

    virtual int         sendCommand(string text);

    /**
     * Reads a single line (or message) from the receive buffer, starting at current stream position.
     * This function is intended to be used when the serial port is configured for a message protocol,
     * such as 'Line' mode or frame protocols.
     *
     * If data at current stream position is not available anymore in the receive buffer,
     * the function returns the oldest available line and moves the stream position just after.
     * If no new full line is received, the function returns an empty line.
     *
     * @return a string with a single line of text
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual string      readLine(void);

    /**
     * Searches for incoming messages in the serial port receive buffer matching a given pattern,
     * starting at current position. This function will only compare and return printable characters
     * in the message strings. Binary protocols are handled as hexadecimal strings.
     *
     * The search returns all messages matching the expression provided as argument in the buffer.
     * If no matching message is found, the search waits for one up to the specified maximum timeout
     * (in milliseconds).
     *
     * @param pattern : a limited regular expression describing the expected message format,
     *         or an empty string if all messages should be returned (no filtering).
     *         When using binary protocols, the format applies to the hexadecimal
     *         representation of the message.
     * @param maxWait : the maximum number of milliseconds to wait for a message if none is found
     *         in the receive buffer.
     *
     * @return an array of strings containing the messages found, if any.
     *         Binary messages are converted to hexadecimal representation.
     *
     * On failure, throws an exception or returns an empty array.
     */
    virtual vector<string> readMessages(string pattern,int maxWait);

    /**
     * Changes the current internal stream position to the specified value. This function
     * does not affect the device, it only changes the value stored in the API object
     * for the next read operations.
     *
     * @param absPos : the absolute position index for next read operations.
     *
     * @return nothing.
     */
    virtual int         read_seek(int absPos);

    /**
     * Returns the current absolute stream position pointer of the API object.
     *
     * @return the absolute position index for next read operations.
     */
    virtual int         read_tell(void);

    /**
     * Returns the number of bytes available to read in the input buffer starting from the
     * current absolute stream position pointer of the API object.
     *
     * @return the number of bytes available to read
     */
    virtual int         read_avail(void);

    /**
     * Sends a text line query to the serial port, and reads the reply, if any.
     * This function is intended to be used when the serial port is configured for 'Line' protocol.
     *
     * @param query : the line query to send (without CR/LF)
     * @param maxWait : the maximum number of milliseconds to wait for a reply.
     *
     * @return the next text line received after sending the text query, as a string.
     *         Additional lines can be obtained by calling readLine or readMessages.
     *
     * On failure, throws an exception or returns an empty string.
     */
    virtual string      queryLine(string query,int maxWait);

    /**
     * Sends a binary message to the serial port, and reads the reply, if any.
     * This function is intended to be used when the serial port is configured for
     * Frame-based protocol.
     *
     * @param hexString : the message to send, coded in hexadecimal
     * @param maxWait : the maximum number of milliseconds to wait for a reply.
     *
     * @return the next frame received after sending the message, as a hex string.
     *         Additional frames can be obtained by calling readHex or readMessages.
     *
     * On failure, throws an exception or returns an empty string.
     */
    virtual string      queryHex(string hexString,int maxWait);

    /**
     * Saves the job definition string (JSON data) into a job file.
     * The job file can be later enabled using selectJob().
     *
     * @param jobfile : name of the job file to save on the device filesystem
     * @param jsonDef : a string containing a JSON definition of the job
     *
     * @return YAPI_SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         uploadJob(string jobfile,string jsonDef);

    /**
     * Load and start processing the specified job file. The file must have
     * been previously created using the user interface or uploaded on the
     * device filesystem using the uploadJob() function.
     *
     * @param jobfile : name of the job file (on the device filesystem)
     *
     * @return YAPI_SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         selectJob(string jobfile);

    /**
     * Clears the serial port buffer and resets counters to zero.
     *
     * @return YAPI_SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         reset(void);

    /**
     * Sends a one-way message (provided as a a binary buffer) to a device on the I2C bus.
     * This function checks and reports communication errors on the I2C bus.
     *
     * @param slaveAddr : the 7-bit address of the slave device (without the direction bit)
     * @param buff : the binary buffer to be sent
     *
     * @return YAPI_SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         i2cSendBin(int slaveAddr,string buff);

    /**
     * Sends a one-way message (provided as a list of integer) to a device on the I2C bus.
     * This function checks and reports communication errors on the I2C bus.
     *
     * @param slaveAddr : the 7-bit address of the slave device (without the direction bit)
     * @param values : a list of data bytes to be sent
     *
     * @return YAPI_SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         i2cSendArray(int slaveAddr,vector<int> values);

    /**
     * Sends a one-way message (provided as a a binary buffer) to a device on the I2C bus,
     * then read back the specified number of bytes from device.
     * This function checks and reports communication errors on the I2C bus.
     *
     * @param slaveAddr : the 7-bit address of the slave device (without the direction bit)
     * @param buff : the binary buffer to be sent
     * @param rcvCount : the number of bytes to receive once the data bytes are sent
     *
     * @return a list of bytes with the data received from slave device.
     *
     * On failure, throws an exception or returns an empty binary buffer.
     */
    virtual string      i2cSendAndReceiveBin(int slaveAddr,string buff,int rcvCount);

    /**
     * Sends a one-way message (provided as a list of integer) to a device on the I2C bus,
     * then read back the specified number of bytes from device.
     * This function checks and reports communication errors on the I2C bus.
     *
     * @param slaveAddr : the 7-bit address of the slave device (without the direction bit)
     * @param values : a list of data bytes to be sent
     * @param rcvCount : the number of bytes to receive once the data bytes are sent
     *
     * @return a list of bytes with the data received from slave device.
     *
     * On failure, throws an exception or returns an empty array.
     */
    virtual vector<int> i2cSendAndReceiveArray(int slaveAddr,vector<int> values,int rcvCount);

    /**
     * Sends a text-encoded I2C code stream to the I2C bus, as is.
     * An I2C code stream is a string made of hexadecimal data bytes,
     * but that may also include the I2C state transitions code:
     * "{S}" to emit a start condition,
     * "{R}" for a repeated start condition,
     * "{P}" for a stop condition,
     * "xx" for receiving a data byte,
     * "{A}" to ack a data byte received and
     * "{N}" to nack a data byte received.
     * If a newline ("\n") is included in the stream, the message
     * will be terminated and a newline will also be added to the
     * receive stream.
     *
     * @param codes : the code stream to send
     *
     * @return YAPI_SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         writeStr(string codes);

    /**
     * Sends a text-encoded I2C code stream to the I2C bus, and terminate
     * the message en relâchant le bus.
     * An I2C code stream is a string made of hexadecimal data bytes,
     * but that may also include the I2C state transitions code:
     * "{S}" to emit a start condition,
     * "{R}" for a repeated start condition,
     * "{P}" for a stop condition,
     * "xx" for receiving a data byte,
     * "{A}" to ack a data byte received and
     * "{N}" to nack a data byte received.
     * At the end of the stream, a stop condition is added if missing
     * and a newline is added to the receive buffer as well.
     *
     * @param codes : the code stream to send
     *
     * @return YAPI_SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         writeLine(string codes);

    /**
     * Sends a single byte to the I2C bus. Depending on the I2C bus state, the byte
     * will be interpreted as an address byte or a data byte.
     *
     * @param code : the byte to send
     *
     * @return YAPI_SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         writeByte(int code);

    /**
     * Sends a byte sequence (provided as a hexadecimal string) to the I2C bus.
     * Depending on the I2C bus state, the first byte will be interpreted as an
     * address byte or a data byte.
     *
     * @param hexString : a string of hexadecimal byte codes
     *
     * @return YAPI_SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         writeHex(string hexString);

    /**
     * Sends a binary buffer to the I2C bus, as is.
     * Depending on the I2C bus state, the first byte will be interpreted
     * as an address byte or a data byte.
     *
     * @param buff : the binary buffer to send
     *
     * @return YAPI_SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         writeBin(string buff);

    /**
     * Sends a byte sequence (provided as a list of bytes) to the I2C bus.
     * Depending on the I2C bus state, the first byte will be interpreted as an
     * address byte or a data byte.
     *
     * @param byteList : a list of byte codes
     *
     * @return YAPI_SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         writeArray(vector<int> byteList);


    inline static YI2cPort *Find(string func)
    { return YI2cPort::FindI2cPort(func); }

    /**
     * Continues the enumeration of I2C ports started using yFirstI2cPort().
     * Caution: You can't make any assumption about the returned I2C ports order.
     * If you want to find a specific an I2C port, use I2cPort.findI2cPort()
     * and a hardwareID or a logical name.
     *
     * @return a pointer to a YI2cPort object, corresponding to
     *         an I2C port currently online, or a NULL pointer
     *         if there are no more I2C ports to enumerate.
     */
           YI2cPort        *nextI2cPort(void);
    inline YI2cPort        *next(void)
    { return this->nextI2cPort();}

    /**
     * Starts the enumeration of I2C ports currently accessible.
     * Use the method YI2cPort.nextI2cPort() to iterate on
     * next I2C ports.
     *
     * @return a pointer to a YI2cPort object, corresponding to
     *         the first I2C port currently online, or a NULL pointer
     *         if there are none.
     */
           static YI2cPort *FirstI2cPort(void);
    inline static YI2cPort *First(void)
    { return YI2cPort::FirstI2cPort();}
#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of YI2cPort accessors declaration)
};

//--- (YI2cPort functions declaration)

/**
 * Retrieves an I2C port for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
 *
 * This function does not require that the I2C port is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method YI2cPort.isOnline() to test if the I2C port is
 * indeed online at a given time. In case of ambiguity when looking for
 * an I2C port by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the I2C port, for instance
 *         YI2CMK01.i2cPort.
 *
 * @return a YI2cPort object allowing you to drive the I2C port.
 */
inline YI2cPort *yFindI2cPort(const string& func)
{ return YI2cPort::FindI2cPort(func);}
/**
 * Starts the enumeration of I2C ports currently accessible.
 * Use the method YI2cPort.nextI2cPort() to iterate on
 * next I2C ports.
 *
 * @return a pointer to a YI2cPort object, corresponding to
 *         the first I2C port currently online, or a NULL pointer
 *         if there are none.
 */
inline YI2cPort *yFirstI2cPort(void)
{ return YI2cPort::FirstI2cPort();}

//--- (end of YI2cPort functions declaration)

#ifdef YOCTOLIB_NAMESPACE
// end of namespace definition
}
#endif

#endif
