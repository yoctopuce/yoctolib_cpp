/*********************************************************************
 *
 *  $Id: svn_id $
 *
 *  Declares yFindRfidReader(), the high-level API for RfidReader functions
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


#ifndef YOCTO_RFIDREADER_H
#define YOCTO_RFIDREADER_H

#include <cfloat>
#include <cmath>

#include "yocto_api.h"

#ifdef YOCTOLIB_NAMESPACE
namespace YOCTOLIB_NAMESPACE
{
#endif

//--- (generated code: YRfidTagInfo return codes)
//--- (end of generated code: YRfidTagInfo return codes)
//--- (generated code: YRfidTagInfo yapiwrapper)
//--- (end of generated code: YRfidTagInfo yapiwrapper)
//--- (generated code: YRfidTagInfo definitions)
//--- (end of generated code: YRfidTagInfo definitions)

//--- (generated code: YRfidTagInfo declaration)
/**
 * YRfidTagInfo Class: RFID tag description, used by class YRfidReader
 *
 * YRfidTagInfo objects are used to describe RFID tag attributes,
 * such as the tag type and its storage size. These objects are returned by
 * method get_tagInfo() of class YRfidReader.
 */
class YOCTO_CLASS_EXPORT YRfidTagInfo {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of generated code: YRfidTagInfo declaration)
protected:
    //--- (generated code: YRfidTagInfo attributes)
    // Attributes (function value cache)
    string          _tagId;
    int             _tagType;
    string          _typeStr;
    int             _size;
    int             _usable;
    int             _blksize;
    int             _fblk;
    int             _lblk;
    //--- (end of generated code: YRfidTagInfo attributes)

public:
    YRfidTagInfo();
    virtual ~YRfidTagInfo();
    //--- (generated code: YRfidTagInfo accessors declaration)

    static const int IEC_15693 = 1;
    static const int IEC_14443 = 2;
    static const int IEC_14443_MIFARE_ULTRALIGHT = 3;
    static const int IEC_14443_MIFARE_CLASSIC1K = 4;
    static const int IEC_14443_MIFARE_CLASSIC4K = 5;
    static const int IEC_14443_MIFARE_DESFIRE = 6;
    static const int IEC_14443_NTAG_213 = 7;
    static const int IEC_14443_NTAG_215 = 8;
    static const int IEC_14443_NTAG_216 = 9;
    static const int IEC_14443_NTAG_424_DNA = 10;

    /**
     * Returns the RFID tag identifier.
     *
     * @return a string with the RFID tag identifier.
     */
    virtual string      get_tagId(void);

    /**
     * Returns the type of the RFID tag, as a numeric constant.
     * (IEC_14443_MIFARE_CLASSIC1K, ...).
     *
     * @return an integer corresponding to the RFID tag type
     */
    virtual int         get_tagType(void);

    /**
     * Returns the type of the RFID tag, as a string.
     *
     * @return a string corresponding to the RFID tag type
     */
    virtual string      get_tagTypeStr(void);

    /**
     * Returns the total memory size of the RFID tag, in bytes.
     *
     * @return the total memory size of the RFID tag
     */
    virtual int         get_tagMemorySize(void);

    /**
     * Returns the usable storage size of the RFID tag, in bytes.
     *
     * @return the usable storage size of the RFID tag
     */
    virtual int         get_tagUsableSize(void);

    /**
     * Returns the block size of the RFID tag, in bytes.
     *
     * @return the block size of the RFID tag
     */
    virtual int         get_tagBlockSize(void);

    /**
     * Returns the index of the block available for data storage on the RFID tag.
     * Some tags have special block used to configure the tag behavior, these
     * blocks must be handled with precaution. However, the  block return by
     * get_tagFirstBlock() can be locked, use get_tagLockState()
     * to find out  which block are locked.
     *
     * @return the index of the first usable storage block on the RFID tag
     */
    virtual int         get_tagFirstBlock(void);

    /**
     * Returns the index of the last last black available for data storage on the RFID tag,
     * However, this block can be locked, use get_tagLockState() to find out
     * which block are locked.
     *
     * @return the index of the last usable storage block on the RFID tag
     */
    virtual int         get_tagLastBlock(void);

    virtual void        imm_init(string tagId,int tagType,int size,int usable,int blksize,int fblk,int lblk);

#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of generated code: YRfidTagInfo accessors declaration)
};

//--- (generated code: YRfidTagInfo functions declaration)
//--- (end of generated code: YRfidTagInfo functions declaration)


//--- (generated code: YRfidOptions definitions)
//--- (end of generated code: YRfidOptions definitions)

//--- (generated code: YRfidOptions declaration)
/**
 * YRfidOptions Class: Additional parameters for operations on RFID tags.
 *
 * The YRfidOptions objects are used to specify additional
 * optional parameters to RFID commands that interact with tags,
 * including security keys. When instantiated,the parameters of
 * this object are pre-initialized to a value  which corresponds
 * to the most common usage.
 */
class YOCTO_CLASS_EXPORT YRfidOptions {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of generated code: YRfidOptions declaration)

public:
    YRfidOptions();
    virtual ~YRfidOptions();

    //--- (generated code: YRfidOptions attributes)

    // Public Attributes
    /**
     * Type of security key to be used to access the RFID tag.
     * For MIFARE Classic tags, allowed values are
     * Y_MIFARE_KEY_A or Y_MIFARE_KEY_B.
     * The default value is Y_NO_RFID_KEY, in that case
     * the reader will use the most common default key for the
     * tag type.
     * When a security key is required, it must be provided
     * using property HexKey.
     */
    int             KeyType;
    /**
     * Security key to be used to access the RFID tag, as an
     * hexadecimal string. The key will only be used if you
     * also specify which type of key it is, using property
     * KeyType.
     */
    string          HexKey;
    /**
     * Forces the use of single-block commands to access RFID tag memory blocks.
     * By default, the Yoctopuce library uses the most efficient access strategy
     * generally available for each tag type, but you can force the use of
     * single-block commands if the RFID tags you are using do not support
     * multi-block commands. If operation speed is not a priority, choose
     * single-block mode as it will work with any mode.
     */
    bool            ForceSingleBlockAccess;
    /**
     * Forces the use of multi-block commands to access RFID tag memory blocks.
     * By default, the Yoctopuce library uses the most efficient access strategy
     * generally available for each tag type, but you can force the use of
     * multi-block commands if you know for sure that the RFID tags you are using
     * do support multi-block commands. Be  aware that even if a tag allows multi-block
     * operations, the maximum number of blocks that can be written or read at the same
     * time can be (very) limited. If the tag does not support multi-block mode
     * for the wanted operation, the option will be ignored.
     */
    bool            ForceMultiBlockAccess;
    /**
     * Enables direct access to RFID tag control blocks.
     * By default, Yoctopuce library read and write functions only work
     * on data blocks and automatically skip special blocks, as specific functions are provided
     * to configure security parameters found in control blocks.
     * If you need to access control blocks in your own way using
     * read/write functions, enable this option.  Use this option wisely,
     * as overwriting a special block migth very well irreversibly alter your
     * tag behavior.
     */
    bool            EnableRawAccess;
    /**
     * Disables the tag memory overflow test. By default, the Yoctopuce
     * library's read/write functions detect overruns and do not run
     * commands that are likely to fail. If you nevertheless wish to
     * try to access more memory than the tag announces, you can try to use
     * this option.
     */
    bool            DisableBoundaryChecks;
    /**
     * Enables simulation mode to check the affected block range as well
     * as access rights. When this option is active, the operation is
     * not fully applied to the RFID tag but the affected block range
     * is determined and the optional access key is tested on these blocks.
     * The access key rights are not tested though. This option applies to
     * write / configure operations only, it is ignored for read operations.
     */
    bool            EnableDryRun;

    // Constants used as attribute values
    static const int NO_RFID_KEY = 0;
    static const int MIFARE_KEY_A = 1;
    static const int MIFARE_KEY_B = 2;

    //--- (end of generated code: YRfidOptions attributes)

    //--- (generated code: YRfidOptions accessors declaration)

    virtual string      imm_getParams(void);

#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of generated code: YRfidOptions accessors declaration)
};

//--- (generated code: YRfidOptions functions declaration)
//--- (end of generated code: YRfidOptions functions declaration)


//--- (generated code: YRfidStatus return codes)
//--- (end of generated code: YRfidStatus return codes)
//--- (generated code: YRfidStatus yapiwrapper)
//--- (end of generated code: YRfidStatus yapiwrapper)
//--- (generated code: YRfidStatus definitions)
//--- (end of generated code: YRfidStatus definitions)

//--- (generated code: YRfidStatus declaration)
/**
 * YRfidStatus Class: Detailled information about the result of RFID tag operations, allowing to find
 * out what happened exactly after a tag operation failure.
 *
 * YRfidStatus objects provide additional information about
 * operations on RFID tags, including the range of blocks affected
 * by read/write operations and possible errors when communicating
 * with RFID tags.
 * This makes it possible, for example, to distinguish communication
 * errors that can be recovered by an additional attempt, from
 * security or other errors on the tag.
 * Combined with the EnableDryRun option in RfidOptions,
 * this structure can be used to predict which blocks will be affected
 * by a write operation.
 */
class YOCTO_CLASS_EXPORT YRfidStatus {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of generated code: YRfidStatus declaration)
protected:
    //--- (generated code: YRfidStatus attributes)
    // Attributes (function value cache)
    string          _tagId;
    int             _errCode;
    int             _errBlk;
    string          _errMsg;
    int             _yapierr;
    int             _fab;
    int             _lab;
    //--- (end of generated code: YRfidStatus attributes)

public:
    YRfidStatus();
    virtual ~YRfidStatus();
    //--- (generated code: YRfidStatus accessors declaration)

    static const int SUCCESS = 0;
    static const int COMMAND_NOT_SUPPORTED = 1;
    static const int COMMAND_NOT_RECOGNIZED = 2;
    static const int COMMAND_OPTION_NOT_RECOGNIZED = 3;
    static const int COMMAND_CANNOT_BE_PROCESSED_IN_TIME = 4;
    static const int UNDOCUMENTED_ERROR = 15;
    static const int BLOCK_NOT_AVAILABLE = 16;
    static const int BLOCK_ALREADY_LOCKED = 17;
    static const int BLOCK_LOCKED = 18;
    static const int BLOCK_NOT_SUCESSFULLY_PROGRAMMED = 19;
    static const int BLOCK_NOT_SUCESSFULLY_LOCKED = 20;
    static const int BLOCK_IS_PROTECTED = 21;
    static const int CRYPTOGRAPHIC_ERROR = 64;
    static const int READER_BUSY = 1000;
    static const int TAG_NOTFOUND = 1001;
    static const int TAG_LEFT = 1002;
    static const int TAG_JUSTLEFT = 1003;
    static const int TAG_COMMUNICATION_ERROR = 1004;
    static const int TAG_NOT_RESPONDING = 1005;
    static const int TIMEOUT_ERROR = 1006;
    static const int COLLISION_DETECTED = 1007;
    static const int INVALID_CMD_ARGUMENTS = -66;
    static const int UNKNOWN_CAPABILITIES = -67;
    static const int MEMORY_NOT_SUPPORTED = -68;
    static const int INVALID_BLOCK_INDEX = -69;
    static const int MEM_SPACE_UNVERRUN_ATTEMPT = -70;
    static const int BROWNOUT_DETECTED = -71     ;
    static const int BUFFER_OVERFLOW = -72;
    static const int CRC_ERROR = -73;
    static const int COMMAND_RECEIVE_TIMEOUT = -75;
    static const int DID_NOT_SLEEP = -76;
    static const int ERROR_DECIMAL_EXPECTED = -77;
    static const int HARDWARE_FAILURE = -78;
    static const int ERROR_HEX_EXPECTED = -79;
    static const int FIFO_LENGTH_ERROR = -80;
    static const int FRAMING_ERROR = -81;
    static const int NOT_IN_CNR_MODE = -82;
    static const int NUMBER_OU_OF_RANGE = -83;
    static const int NOT_SUPPORTED = -84;
    static const int NO_RF_FIELD_ACTIVE = -85;
    static const int READ_DATA_LENGTH_ERROR = -86;
    static const int WATCHDOG_RESET = -87;
    static const int UNKNOW_COMMAND = -91;
    static const int UNKNOW_ERROR = -92;
    static const int UNKNOW_PARAMETER = -93;
    static const int UART_RECEIVE_ERROR = -94;
    static const int WRONG_DATA_LENGTH = -95;
    static const int WRONG_MODE = -96;
    static const int UNKNOWN_DWARFxx_ERROR_CODE = -97;
    static const int RESPONSE_SHORT = -98;
    static const int UNEXPECTED_TAG_ID_IN_RESPONSE = -99;
    static const int UNEXPECTED_TAG_INDEX = -100;
    static const int READ_EOF = -101;
    static const int READ_OK_SOFAR = -102;
    static const int WRITE_DATA_MISSING = -103;
    static const int WRITE_TOO_MUCH_DATA = -104;
    static const int TRANSFER_CLOSED = -105;
    static const int COULD_NOT_BUILD_REQUEST = -106;
    static const int INVALID_OPTIONS = -107;
    static const int UNEXPECTED_RESPONSE = -108;
    static const int AFI_NOT_AVAILABLE = -109;
    static const int DSFID_NOT_AVAILABLE = -110;
    static const int TAG_RESPONSE_TOO_SHORT = -111;
    static const int DEC_EXPECTED = -112 ;
    static const int HEX_EXPECTED = -113;
    static const int NOT_SAME_SECOR = -114;
    static const int MIFARE_AUTHENTICATED = -115;
    static const int NO_DATABLOCK = -116;
    static const int KEYB_IS_READABLE = -117;
    static const int OPERATION_NOT_EXECUTED = -118;
    static const int BLOK_MODE_ERROR = -119;
    static const int BLOCK_NOT_WRITABLE = -120;
    static const int BLOCK_ACCESS_ERROR = -121;
    static const int BLOCK_NOT_AUTHENTICATED = -122;
    static const int ACCESS_KEY_BIT_NOT_WRITABLE = -123;
    static const int USE_KEYA_FOR_AUTH = -124;
    static const int USE_KEYB_FOR_AUTH = -125;
    static const int KEY_NOT_CHANGEABLE = -126;
    static const int BLOCK_TOO_HIGH = -127;
    static const int AUTH_ERR = -128;
    static const int NOKEY_SELECT = -129;
    static const int CARD_NOT_SELECTED = -130;
    static const int BLOCK_TO_READ_NONE = -131;
    static const int NO_TAG = -132;
    static const int TOO_MUCH_DATA = -133;
    static const int CON_NOT_SATISFIED = -134;
    static const int BLOCK_IS_SPECIAL = -135;
    static const int READ_BEYOND_ANNOUNCED_SIZE = -136;
    static const int BLOCK_ZERO_IS_RESERVED = -137;
    static const int VALUE_BLOCK_BAD_FORMAT = -138;
    static const int ISO15693_ONLY_FEATURE = -139;
    static const int ISO14443_ONLY_FEATURE = -140;
    static const int MIFARE_CLASSIC_ONLY_FEATURE = -141;
    static const int BLOCK_MIGHT_BE_PROTECTED = -142;
    static const int NO_SUCH_BLOCK = -143;
    static const int COUNT_TOO_BIG = -144;
    static const int UNKNOWN_MEM_SIZE = -145;
    static const int MORE_THAN_2BLOCKS_MIGHT_NOT_WORK = -146;
    static const int READWRITE_NOT_SUPPORTED = -147;
    static const int UNEXPECTED_VICC_ID_IN_RESPONSE = -148;
    static const int LOCKBLOCK_NOT_SUPPORTED = -150;
    static const int INTERNAL_ERROR_SHOULD_NEVER_HAPPEN = -151;
    static const int INVLD_BLOCK_MODE_COMBINATION = -152;
    static const int INVLD_ACCESS_MODE_COMBINATION = -153;
    static const int INVALID_SIZE = -154;
    static const int BAD_PASSWORD_FORMAT = -155;
    static const int RADIO_IS_OFF = -156;

    /**
     * Returns RFID tag identifier related to the status.
     *
     * @return a string with the RFID tag identifier.
     */
    virtual string      get_tagId(void);

    /**
     * Returns the detailled error code, or 0 if no error happened.
     *
     * @return a numeric error code
     */
    virtual int         get_errorCode(void);

    /**
     * Returns the RFID tag memory block number where the error was encountered, or -1 if the
     * error is not specific to a memory block.
     *
     * @return an RFID tag block number
     */
    virtual int         get_errorBlock(void);

    /**
     * Returns a string describing precisely the RFID commande result.
     *
     * @return an error message string
     */
    virtual string      get_errorMessage(void);

    virtual int         get_yapiError(void);

    /**
     * Returns the block number of the first RFID tag memory block affected
     * by the operation. Depending on the type of operation and on the tag
     * memory granularity, this number may be smaller than the requested
     * memory block index.
     *
     * @return an RFID tag block number
     */
    virtual int         get_firstAffectedBlock(void);

    /**
     * Returns the block number of the last RFID tag memory block affected
     * by the operation. Depending on the type of operation and on the tag
     * memory granularity, this number may be bigger than the requested
     * memory block index.
     *
     * @return an RFID tag block number
     */
    virtual int         get_lastAffectedBlock(void);

    virtual void        imm_init(string tagId,int errCode,int errBlk,int fab,int lab);

#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of generated code: YRfidStatus accessors declaration)
};

//--- (generated code: YRfidStatus functions declaration)
//--- (end of generated code: YRfidStatus functions declaration)


//--- (generated code: YRfidReader return codes)
//--- (end of generated code: YRfidReader return codes)
//--- (generated code: YRfidReader yapiwrapper)
//--- (end of generated code: YRfidReader yapiwrapper)
//--- (generated code: YRfidReader definitions)
class YRfidReader; // forward declaration

typedef void (*YRfidReaderValueCallback)(YRfidReader *func, const string& functionValue);
typedef void (*YEventCallback)(YRfidReader *obj, double timestamp, const string& eventType, const string& eventData);

#define Y_NTAGS_INVALID                 (YAPI_INVALID_UINT)
#define Y_REFRESHRATE_INVALID           (YAPI_INVALID_UINT)
//--- (end of generated code: YRfidReader definitions)

//--- (generated code: YRfidReader declaration)
/**
 * YRfidReader Class: RfidReader function interface
 *
 * The YRfidReader class allows you to detect RFID tags, as well as
 * read and write on these tags if the security settings allow it.
 *
 * Short reminder:
 *
 * - A tag's memory is generally organized in fixed-size blocks.
 * - At tag level, each block must be read and written in its entirety.
 * - Some blocks are special configuration blocks, and may alter the tag's behavior
 * if they are rewritten with arbitrary data.
 * - Data blocks can be set to read-only mode, but on many tags, this operation is irreversible.
 *
 *
 * By default, the RfidReader class automatically manages these blocks so that
 * arbitrary size data  can be manipulated of  without risk and without knowledge of
 * tag architecture.
 */
class YOCTO_CLASS_EXPORT YRfidReader: public YFunction {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of generated code: YRfidReader declaration)
protected:
    static void yInternalEventCallback(YRfidReader *obj, const string& value);

    //--- (generated code: YRfidReader attributes)
    // Attributes (function value cache)
    int             _nTags;
    int             _refreshRate;
    YRfidReaderValueCallback _valueCallbackRfidReader;
    YEventCallback  _eventCallback;
    bool            _isFirstCb;
    int             _prevCbPos;
    int             _eventPos;
    int             _eventStamp;

    friend YRfidReader *yFindRfidReader(const string& func);
    friend YRfidReader *yFirstRfidReader(void);

    // Function-specific method for parsing of JSON output and caching result
    virtual int     _parseAttr(YJSONObject *json_val);

    // Constructor is protected, use yFindRfidReader factory function to instantiate
    YRfidReader(const string& func);
    //--- (end of generated code: YRfidReader attributes)

public:
    virtual ~YRfidReader();
    //--- (generated code: YRfidReader accessors declaration)

    static const int NTAGS_INVALID = YAPI_INVALID_UINT;
    static const int REFRESHRATE_INVALID = YAPI_INVALID_UINT;

    /**
     * Returns the number of RFID tags currently detected.
     *
     * @return an integer corresponding to the number of RFID tags currently detected
     *
     * On failure, throws an exception or returns YRfidReader::NTAGS_INVALID.
     */
    int                 get_nTags(void);

    inline int          nTags(void)
    { return this->get_nTags(); }

    /**
     * Returns the tag list refresh rate, measured in Hz.
     *
     * @return an integer corresponding to the tag list refresh rate, measured in Hz
     *
     * On failure, throws an exception or returns YRfidReader::REFRESHRATE_INVALID.
     */
    int                 get_refreshRate(void);

    inline int          refreshRate(void)
    { return this->get_refreshRate(); }

    /**
     * Changes the present tag list refresh rate, measured in Hz. The reader will do
     * its best to respect it. Note that the reader cannot detect tag arrival or removal
     * while it is  communicating with a tag.  Maximum frequency is limited to 100Hz,
     * but in real life it will be difficult to do better than 50Hz.  A zero value
     * will power off the device radio.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     *
     * @param newval : an integer corresponding to the present tag list refresh rate, measured in Hz
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_refreshRate(int newval);
    inline int      setRefreshRate(int newval)
    { return this->set_refreshRate(newval); }

    /**
     * Retrieves a RFID reader for a given identifier.
     * The identifier can be specified using several formats:
     *
     * - FunctionLogicalName
     * - ModuleSerialNumber.FunctionIdentifier
     * - ModuleSerialNumber.FunctionLogicalName
     * - ModuleLogicalName.FunctionIdentifier
     * - ModuleLogicalName.FunctionLogicalName
     *
     *
     * This function does not require that the RFID reader is online at the time
     * it is invoked. The returned object is nevertheless valid.
     * Use the method isOnline() to test if the RFID reader is
     * indeed online at a given time. In case of ambiguity when looking for
     * a RFID reader by logical name, no error is notified: the first instance
     * found is returned. The search is performed first by hardware name,
     * then by logical name.
     *
     * If a call to this object's is_online() method returns FALSE although
     * you are certain that the matching device is plugged, make sure that you did
     * call registerHub() at application initialization time.
     *
     * @param func : a string that uniquely characterizes the RFID reader, for instance
     *         MyDevice.rfidReader.
     *
     * @return a YRfidReader object allowing you to drive the RFID reader.
     */
    static YRfidReader* FindRfidReader(string func);

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
    virtual int         registerValueCallback(YRfidReaderValueCallback callback);
    using YFunction::registerValueCallback;

    virtual int         _invokeValueCallback(string value);

    virtual int         _chkerror(string tagId,string json,YRfidStatus& status);

    virtual int         reset(void);

    /**
     * Returns the list of RFID tags currently detected by the reader.
     *
     * @return a list of strings, corresponding to each tag identifier (UID).
     *
     * On failure, throws an exception or returns an empty list.
     */
    virtual vector<string> get_tagIdList(void);

    /**
     * Returns a description of the properties of an existing RFID tag.
     * This function can cause communications with the tag.
     *
     * @param tagId : identifier of the tag to check
     * @param status : an RfidStatus object that will contain
     *         the detailled status of the operation
     *
     * @return a YRfidTagInfo object.
     *
     * On failure, throws an exception or returns an empty YRfidTagInfo objact.
     * When it happens, you can get more information from the status object.
     */
    virtual YRfidTagInfo get_tagInfo(string tagId,YRfidStatus& status);

    /**
     * Changes an RFID tag configuration to prevents any further write to
     * the selected blocks. This operation is definitive and irreversible.
     * Depending on the tag type and block index, adjascent blocks may become
     * read-only as well, based on the locking granularity.
     *
     * @param tagId : identifier of the tag to use
     * @param firstBlock : first block to lock
     * @param nBlocks : number of blocks to lock
     * @param options : an YRfidOptions object with the optional
     *         command execution parameters, such as security key
     *         if required
     * @param status : an RfidStatus object that will contain
     *         the detailled status of the operation
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code. When it
     * happens, you can get more information from the status object.
     */
    virtual int         tagLockBlocks(string tagId,int firstBlock,int nBlocks,YRfidOptions options,YRfidStatus& status);

    /**
     * Reads the locked state for RFID tag memory data blocks.
     * FirstBlock cannot be a special block, and any special
     * block encountered in the middle of the read operation will be
     * skipped automatically.
     *
     * @param tagId : identifier of the tag to use
     * @param firstBlock : number of the first block to check
     * @param nBlocks : number of blocks to check
     * @param options : an YRfidOptions object with the optional
     *         command execution parameters, such as security key
     *         if required
     * @param status : an RfidStatus object that will contain
     *         the detailled status of the operation
     *
     * @return a list of booleans with the lock state of selected blocks
     *
     * On failure, throws an exception or returns an empty list. When it
     * happens, you can get more information from the status object.
     */
    virtual vector<bool> get_tagLockState(string tagId,int firstBlock,int nBlocks,YRfidOptions options,YRfidStatus& status);

    /**
     * Tells which block of a RFID tag memory are special and cannot be used
     * to store user data. Mistakely writing a special block can lead to
     * an irreversible alteration of the tag.
     *
     * @param tagId : identifier of the tag to use
     * @param firstBlock : number of the first block to check
     * @param nBlocks : number of blocks to check
     * @param options : an YRfidOptions object with the optional
     *         command execution parameters, such as security key
     *         if required
     * @param status : an RfidStatus object that will contain
     *         the detailled status of the operation
     *
     * @return a list of booleans with the lock state of selected blocks
     *
     * On failure, throws an exception or returns an empty list. When it
     * happens, you can get more information from the status object.
     */
    virtual vector<bool> get_tagSpecialBlocks(string tagId,int firstBlock,int nBlocks,YRfidOptions options,YRfidStatus& status);

    /**
     * Reads data from an RFID tag memory, as an hexadecimal string.
     * The read operation may span accross multiple blocks if the requested
     * number of bytes is larger than the RFID tag block size. By default
     * firstBlock cannot be a special block, and any special block encountered
     * in the middle of the read operation will be skipped automatically.
     * If you rather want to read special blocks, use the EnableRawAccess
     * field from the options parameter.
     *
     * @param tagId : identifier of the tag to use
     * @param firstBlock : block number where read should start
     * @param nBytes : total number of bytes to read
     * @param options : an YRfidOptions object with the optional
     *         command execution parameters, such as security key
     *         if required
     * @param status : an RfidStatus object that will contain
     *         the detailled status of the operation
     *
     * @return an hexadecimal string if the call succeeds.
     *
     * On failure, throws an exception or returns an empty binary buffer. When it
     * happens, you can get more information from the status object.
     */
    virtual string      tagReadHex(string tagId,int firstBlock,int nBytes,YRfidOptions options,YRfidStatus& status);

    /**
     * Reads data from an RFID tag memory, as a binary buffer. The read operation
     * may span accross multiple blocks if the requested number of bytes
     * is larger than the RFID tag block size.  By default
     * firstBlock cannot be a special block, and any special block encountered
     * in the middle of the read operation will be skipped automatically.
     * If you rather want to read special blocks, use the EnableRawAccess
     * field frrm the options parameter.
     *
     * @param tagId : identifier of the tag to use
     * @param firstBlock : block number where read should start
     * @param nBytes : total number of bytes to read
     * @param options : an YRfidOptions object with the optional
     *         command execution parameters, such as security key
     *         if required
     * @param status : an RfidStatus object that will contain
     *         the detailled status of the operation
     *
     * @return a binary object with the data read if the call succeeds.
     *
     * On failure, throws an exception or returns an empty binary buffer. When it
     * happens, you can get more information from the status object.
     */
    virtual string      tagReadBin(string tagId,int firstBlock,int nBytes,YRfidOptions options,YRfidStatus& status);

    /**
     * Reads data from an RFID tag memory, as a byte list. The read operation
     * may span accross multiple blocks if the requested number of bytes
     * is larger than the RFID tag block size.  By default
     * firstBlock cannot be a special block, and any special block encountered
     * in the middle of the read operation will be skipped automatically.
     * If you rather want to read special blocks, use the EnableRawAccess
     * field from the options parameter.
     *
     * @param tagId : identifier of the tag to use
     * @param firstBlock : block number where read should start
     * @param nBytes : total number of bytes to read
     * @param options : an YRfidOptions object with the optional
     *         command execution parameters, such as security key
     *         if required
     * @param status : an RfidStatus object that will contain
     *         the detailled status of the operation
     *
     * @return a byte list with the data read if the call succeeds.
     *
     * On failure, throws an exception or returns an empty list. When it
     * happens, you can get more information from the status object.
     */
    virtual vector<int> tagReadArray(string tagId,int firstBlock,int nBytes,YRfidOptions options,YRfidStatus& status);

    /**
     * Reads data from an RFID tag memory, as a text string. The read operation
     * may span accross multiple blocks if the requested number of bytes
     * is larger than the RFID tag block size.  By default
     * firstBlock cannot be a special block, and any special block encountered
     * in the middle of the read operation will be skipped automatically.
     * If you rather want to read special blocks, use the EnableRawAccess
     * field from the options parameter.
     *
     * @param tagId : identifier of the tag to use
     * @param firstBlock : block number where read should start
     * @param nChars : total number of characters to read
     * @param options : an YRfidOptions object with the optional
     *         command execution parameters, such as security key
     *         if required
     * @param status : an RfidStatus object that will contain
     *         the detailled status of the operation
     *
     * @return a text string with the data read if the call succeeds.
     *
     * On failure, throws an exception or returns an empty string. When it
     * happens, you can get more information from the status object.
     */
    virtual string      tagReadStr(string tagId,int firstBlock,int nChars,YRfidOptions options,YRfidStatus& status);

    /**
     * Writes data provided as a binary buffer to an RFID tag memory.
     * The write operation may span accross multiple blocks if the
     * number of bytes to write is larger than the RFID tag block size.
     * By default firstBlock cannot be a special block, and any special block
     * encountered in the middle of the write operation will be skipped
     * automatically. The last data block affected by the operation will
     * be automatically padded with zeros if neccessary.  If you rather want
     * to rewrite special blocks as well,
     * use the EnableRawAccess field from the options parameter.
     *
     * @param tagId : identifier of the tag to use
     * @param firstBlock : block number where write should start
     * @param buff : the binary buffer to write
     * @param options : an YRfidOptions object with the optional
     *         command execution parameters, such as security key
     *         if required
     * @param status : an RfidStatus object that will contain
     *         the detailled status of the operation
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code. When it
     * happens, you can get more information from the status object.
     */
    virtual int         tagWriteBin(string tagId,int firstBlock,string buff,YRfidOptions options,YRfidStatus& status);

    /**
     * Writes data provided as a list of bytes to an RFID tag memory.
     * The write operation may span accross multiple blocks if the
     * number of bytes to write is larger than the RFID tag block size.
     * By default firstBlock cannot be a special block, and any special block
     * encountered in the middle of the write operation will be skipped
     * automatically. The last data block affected by the operation will
     * be automatically padded with zeros if neccessary.
     * If you rather want to rewrite special blocks as well,
     * use the EnableRawAccess field from the options parameter.
     *
     * @param tagId : identifier of the tag to use
     * @param firstBlock : block number where write should start
     * @param byteList : a list of byte to write
     * @param options : an YRfidOptions object with the optional
     *         command execution parameters, such as security key
     *         if required
     * @param status : an RfidStatus object that will contain
     *         the detailled status of the operation
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code. When it
     * happens, you can get more information from the status object.
     */
    virtual int         tagWriteArray(string tagId,int firstBlock,vector<int> byteList,YRfidOptions options,YRfidStatus& status);

    /**
     * Writes data provided as an hexadecimal string to an RFID tag memory.
     * The write operation may span accross multiple blocks if the
     * number of bytes to write is larger than the RFID tag block size.
     * By default firstBlock cannot be a special block, and any special block
     * encountered in the middle of the write operation will be skipped
     * automatically. The last data block affected by the operation will
     * be automatically padded with zeros if neccessary.
     * If you rather want to rewrite special blocks as well,
     * use the EnableRawAccess field from the options parameter.
     *
     * @param tagId : identifier of the tag to use
     * @param firstBlock : block number where write should start
     * @param hexString : a string of hexadecimal byte codes to write
     * @param options : an YRfidOptions object with the optional
     *         command execution parameters, such as security key
     *         if required
     * @param status : an RfidStatus object that will contain
     *         the detailled status of the operation
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code. When it
     * happens, you can get more information from the status object.
     */
    virtual int         tagWriteHex(string tagId,int firstBlock,string hexString,YRfidOptions options,YRfidStatus& status);

    /**
     * Writes data provided as an ASCII string to an RFID tag memory.
     * The write operation may span accross multiple blocks if the
     * number of bytes to write is larger than the RFID tag block size.
     * Note that only the characters present in the provided string
     * will be written, there is no notion of string length. If your
     * string data have variable length, you'll have to encode the
     * string length yourself, with a terminal zero for instannce.
     *
     * This function only works with ISO-latin characters, if you wish to
     * write strings encoded with alternate character sets, you'll have to
     * use tagWriteBin() function.
     *
     * By default firstBlock cannot be a special block, and any special block
     * encountered in the middle of the write operation will be skipped
     * automatically. The last data block affected by the operation will
     * be automatically padded with zeros if neccessary.
     * If you rather want to rewrite special blocks as well,
     * use the EnableRawAccess field from the options parameter
     * (definitely not recommanded).
     *
     * @param tagId : identifier of the tag to use
     * @param firstBlock : block number where write should start
     * @param text : the text string to write
     * @param options : an YRfidOptions object with the optional
     *         command execution parameters, such as security key
     *         if required
     * @param status : an RfidStatus object that will contain
     *         the detailled status of the operation
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code. When it
     * happens, you can get more information from the status object.
     */
    virtual int         tagWriteStr(string tagId,int firstBlock,string text,YRfidOptions options,YRfidStatus& status);

    /**
     * Reads an RFID tag AFI byte (ISO 15693 only).
     *
     * @param tagId : identifier of the tag to use
     * @param options : an YRfidOptions object with the optional
     *         command execution parameters, such as security key
     *         if required
     * @param status : an RfidStatus object that will contain
     *         the detailled status of the operation
     *
     * @return the AFI value (0...255)
     *
     * On failure, throws an exception or returns a negative error code. When it
     * happens, you can get more information from the status object.
     */
    virtual int         tagGetAFI(string tagId,YRfidOptions options,YRfidStatus& status);

    /**
     * Changes an RFID tag AFI byte (ISO 15693 only).
     *
     * @param tagId : identifier of the tag to use
     * @param afi : the AFI value to write (0...255)
     * @param options : an YRfidOptions object with the optional
     *         command execution parameters, such as security key
     *         if required
     * @param status : an RfidStatus object that will contain
     *         the detailled status of the operation
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code. When it
     * happens, you can get more information from the status object.
     */
    virtual int         tagSetAFI(string tagId,int afi,YRfidOptions options,YRfidStatus& status);

    /**
     * Locks the RFID tag AFI byte (ISO 15693 only).
     * This operation is definitive and irreversible.
     *
     * @param tagId : identifier of the tag to use
     * @param options : an YRfidOptions object with the optional
     *         command execution parameters, such as security key
     *         if required
     * @param status : an RfidStatus object that will contain
     *         the detailled status of the operation
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code. When it
     * happens, you can get more information from the status object.
     */
    virtual int         tagLockAFI(string tagId,YRfidOptions options,YRfidStatus& status);

    /**
     * Reads an RFID tag DSFID byte (ISO 15693 only).
     *
     * @param tagId : identifier of the tag to use
     * @param options : an YRfidOptions object with the optional
     *         command execution parameters, such as security key
     *         if required
     * @param status : an RfidStatus object that will contain
     *         the detailled status of the operation
     *
     * @return the DSFID value (0...255)
     *
     * On failure, throws an exception or returns a negative error code. When it
     * happens, you can get more information from the status object.
     */
    virtual int         tagGetDSFID(string tagId,YRfidOptions options,YRfidStatus& status);

    /**
     * Changes an RFID tag DSFID byte (ISO 15693 only).
     *
     * @param tagId : identifier of the tag to use
     * @param dsfid : the DSFID value to write (0...255)
     * @param options : an YRfidOptions object with the optional
     *         command execution parameters, such as security key
     *         if required
     * @param status : an RfidStatus object that will contain
     *         the detailled status of the operation
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code. When it
     * happens, you can get more information from the status object.
     */
    virtual int         tagSetDSFID(string tagId,int dsfid,YRfidOptions options,YRfidStatus& status);

    /**
     * Locks the RFID tag DSFID byte (ISO 15693 only).
     * This operation is definitive and irreversible.
     *
     * @param tagId : identifier of the tag to use
     * @param options : an YRfidOptions object with the optional
     *         command execution parameters, such as security key
     *         if required
     * @param status : an RfidStatus object that will contain
     *         the detailled status of the operation
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code. When it
     * happens, you can get more information from the status object.
     */
    virtual int         tagLockDSFID(string tagId,YRfidOptions options,YRfidStatus& status);

    /**
     * Returns a string with last tag arrival/removal events observed.
     * This method return only events that are still buffered in the device memory.
     *
     * @return a string with last events observed (one per line).
     *
     * On failure, throws an exception or returns  YAPI::INVALID_STRING.
     */
    virtual string      get_lastEvents(void);

    /**
     * Registers a callback function to be called each time that an RFID tag appears or
     * disappears. The callback is invoked only during the execution of
     * ySleep or yHandleEvents. This provides control over the time when
     * the callback is triggered. For good responsiveness, remember to call one of these
     * two functions periodically. To unregister a callback, pass a NULL pointer as argument.
     *
     * @param callback : the callback function to call, or a NULL pointer.
     *         The callback function should take four arguments:
     *         the YRfidReader object that emitted the event, the
     *         UTC timestamp of the event, a character string describing
     *         the type of event ("+" or "-") and a character string with the
     *         RFID tag identifier.
     *         On failure, throws an exception or returns a negative error code.
     */
    virtual int         registerEventCallback(YEventCallback callback);

    virtual int         _internalEventHandler(string cbVal);


    inline static YRfidReader *Find(string func)
    { return YRfidReader::FindRfidReader(func); }

    /**
     * Continues the enumeration of RFID readers started using yFirstRfidReader().
     * Caution: You can't make any assumption about the returned RFID readers order.
     * If you want to find a specific a RFID reader, use RfidReader.findRfidReader()
     * and a hardwareID or a logical name.
     *
     * @return a pointer to a YRfidReader object, corresponding to
     *         a RFID reader currently online, or a NULL pointer
     *         if there are no more RFID readers to enumerate.
     */
           YRfidReader     *nextRfidReader(void);
    inline YRfidReader     *next(void)
    { return this->nextRfidReader();}

    /**
     * Starts the enumeration of RFID readers currently accessible.
     * Use the method YRfidReader::nextRfidReader() to iterate on
     * next RFID readers.
     *
     * @return a pointer to a YRfidReader object, corresponding to
     *         the first RFID reader currently online, or a NULL pointer
     *         if there are none.
     */
           static YRfidReader *FirstRfidReader(void);
    inline static YRfidReader *First(void)
    { return YRfidReader::FirstRfidReader();}
#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of generated code: YRfidReader accessors declaration)
};

//--- (generated code: YRfidReader functions declaration)

/**
 * Retrieves a RFID reader for a given identifier.
 * The identifier can be specified using several formats:
 *
 * - FunctionLogicalName
 * - ModuleSerialNumber.FunctionIdentifier
 * - ModuleSerialNumber.FunctionLogicalName
 * - ModuleLogicalName.FunctionIdentifier
 * - ModuleLogicalName.FunctionLogicalName
 *
 *
 * This function does not require that the RFID reader is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the RFID reader is
 * indeed online at a given time. In case of ambiguity when looking for
 * a RFID reader by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the RFID reader, for instance
 *         MyDevice.rfidReader.
 *
 * @return a YRfidReader object allowing you to drive the RFID reader.
 */
inline YRfidReader *yFindRfidReader(const string& func)
{ return YRfidReader::FindRfidReader(func);}
/**
 * Starts the enumeration of RFID readers currently accessible.
 * Use the method YRfidReader::nextRfidReader() to iterate on
 * next RFID readers.
 *
 * @return a pointer to a YRfidReader object, corresponding to
 *         the first RFID reader currently online, or a NULL pointer
 *         if there are none.
 */
inline YRfidReader *yFirstRfidReader(void)
{ return YRfidReader::FirstRfidReader();}

//--- (end of generated code: YRfidReader functions declaration)

#ifdef YOCTOLIB_NAMESPACE
// end of namespace definition
}
#endif

#endif
