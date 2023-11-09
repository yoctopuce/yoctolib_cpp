/*********************************************************************
 *
 *  $Id: svn_id $
 *
 *  Implements yFindRfidReader(), the high-level API for RfidReader functions
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


#define _CRT_SECURE_NO_DEPRECATE //do not use windows secure crt
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "yocto_rfidreader.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#define  __FILE_ID__  "rfidreader"

#ifdef YOCTOLIB_NAMESPACE
using namespace YOCTOLIB_NAMESPACE;
#endif

YRfidTagInfo::YRfidTagInfo():
//--- (generated code: YRfidTagInfo initialization)
    _tagType(0)
    ,_size(0)
    ,_usable(0)
    ,_blksize(0)
    ,_fblk(0)
    ,_lblk(0)
//--- (end of generated code: YRfidTagInfo initialization)
{
}

YRfidTagInfo::~YRfidTagInfo()
{
//--- (generated code: YRfidTagInfo cleanup)
//--- (end of generated code: YRfidTagInfo cleanup)
}
//--- (generated code: YRfidTagInfo implementation)
// static attributes


/**
 * Returns the RFID tag identifier.
 *
 * @return a string with the RFID tag identifier.
 */
string YRfidTagInfo::get_tagId(void)
{
    return _tagId;
}

/**
 * Returns the type of the RFID tag, as a numeric constant.
 * (IEC_14443_MIFARE_CLASSIC1K, ...).
 *
 * @return an integer corresponding to the RFID tag type
 */
int YRfidTagInfo::get_tagType(void)
{
    return _tagType;
}

/**
 * Returns the type of the RFID tag, as a string.
 *
 * @return a string corresponding to the RFID tag type
 */
string YRfidTagInfo::get_tagTypeStr(void)
{
    return _typeStr;
}

/**
 * Returns the total memory size of the RFID tag, in bytes.
 *
 * @return the total memory size of the RFID tag
 */
int YRfidTagInfo::get_tagMemorySize(void)
{
    return _size;
}

/**
 * Returns the usable storage size of the RFID tag, in bytes.
 *
 * @return the usable storage size of the RFID tag
 */
int YRfidTagInfo::get_tagUsableSize(void)
{
    return _usable;
}

/**
 * Returns the block size of the RFID tag, in bytes.
 *
 * @return the block size of the RFID tag
 */
int YRfidTagInfo::get_tagBlockSize(void)
{
    return _blksize;
}

/**
 * Returns the index of the first usable storage block on the RFID tag.
 *
 * @return the index of the first usable storage block on the RFID tag
 */
int YRfidTagInfo::get_tagFirstBlock(void)
{
    return _fblk;
}

/**
 * Returns the index of the last usable storage block on the RFID tag.
 *
 * @return the index of the last usable storage block on the RFID tag
 */
int YRfidTagInfo::get_tagLastBlock(void)
{
    return _lblk;
}

void YRfidTagInfo::imm_init(string tagId,int tagType,int size,int usable,int blksize,int fblk,int lblk)
{
    string typeStr;
    typeStr = "unknown";
    if (tagType == YRfidTagInfo::IEC_15693) {
        typeStr = "IEC 15693";
    }
    if (tagType == YRfidTagInfo::IEC_14443) {
        typeStr = "IEC 14443";
    }
    if (tagType == YRfidTagInfo::IEC_14443_MIFARE_ULTRALIGHT) {
        typeStr = "MIFARE Ultralight";
    }
    if (tagType == YRfidTagInfo::IEC_14443_MIFARE_CLASSIC1K) {
        typeStr = "MIFARE Classic 1K";
    }
    if (tagType == YRfidTagInfo::IEC_14443_MIFARE_CLASSIC4K) {
        typeStr = "MIFARE Classic 4K";
    }
    if (tagType == YRfidTagInfo::IEC_14443_MIFARE_DESFIRE) {
        typeStr = "MIFARE DESFire";
    }
    if (tagType == YRfidTagInfo::IEC_14443_NTAG_213) {
        typeStr = "NTAG 213";
    }
    if (tagType == YRfidTagInfo::IEC_14443_NTAG_215) {
        typeStr = "NTAG 215";
    }
    if (tagType == YRfidTagInfo::IEC_14443_NTAG_216) {
        typeStr = "NTAG 216";
    }
    if (tagType == YRfidTagInfo::IEC_14443_NTAG_424_DNA) {
        typeStr = "NTAG 424 DNA";
    }
    _tagId = tagId;
    _tagType = tagType;
    _typeStr = typeStr;
    _size = size;
    _usable = usable;
    _blksize = blksize;
    _fblk = fblk;
    _lblk = lblk;
}
//--- (end of generated code: YRfidTagInfo implementation)


YRfidOptions::YRfidOptions():
//--- (generated code: YRfidOptions initialization)
    KeyType(0)
    ,ForceSingleBlockAccess(0)
    ,ForceMultiBlockAccess(0)
    ,EnableRawAccess(0)
    ,DisableBoundaryChecks(0)
    ,EnableDryRun(0)
//--- (end of generated code: YRfidOptions initialization)
{
}

YRfidOptions::~YRfidOptions()
{
//--- (generated code: YRfidOptions cleanup)
//--- (end of generated code: YRfidOptions cleanup)
}
//--- (generated code: YRfidOptions implementation)
// static attributes


string YRfidOptions::imm_getParams(void)
{
    int opt = 0;
    string res;
    if (ForceSingleBlockAccess) {
        opt = 1;
    } else {
        opt = 0;
    }
    if (ForceMultiBlockAccess) {
        opt = ((opt) | (2));
    }
    if (EnableRawAccess) {
        opt = ((opt) | (4));
    }
    if (DisableBoundaryChecks) {
        opt = ((opt) | (8));
    }
    if (EnableDryRun) {
        opt = ((opt) | (16));
    }
    res = YapiWrapper::ysprintf("&o=%d",opt);
    if (KeyType != 0) {
        res = YapiWrapper::ysprintf("%s&k=%02x:%s", res.c_str(), KeyType,HexKey.c_str());
    }
    return res;
}
//--- (end of generated code: YRfidOptions implementation)


YRfidStatus::YRfidStatus():
//--- (generated code: YRfidStatus initialization)
    _errCode(0)
    ,_errBlk(0)
    ,_yapierr(0)
    ,_fab(0)
    ,_lab(0)
//--- (end of generated code: YRfidStatus initialization)
{
}

YRfidStatus::~YRfidStatus()
{
//--- (generated code: YRfidStatus cleanup)
//--- (end of generated code: YRfidStatus cleanup)
}
//--- (generated code: YRfidStatus implementation)
// static attributes


/**
 * Returns RFID tag identifier related to the status.
 *
 * @return a string with the RFID tag identifier.
 */
string YRfidStatus::get_tagId(void)
{
    return _tagId;
}

/**
 * Returns the detailled error code, or 0 if no error happened.
 *
 * @return a numeric error code
 */
int YRfidStatus::get_errorCode(void)
{
    return _errCode;
}

/**
 * Returns the RFID tag memory block number where the error was encountered, or -1 if the
 * error is not specific to a memory block.
 *
 * @return an RFID tag block number
 */
int YRfidStatus::get_errorBlock(void)
{
    return _errBlk;
}

/**
 * Returns a string describing precisely the RFID commande result.
 *
 * @return an error message string
 */
string YRfidStatus::get_errorMessage(void)
{
    return _errMsg;
}

int YRfidStatus::get_yapiError(void)
{
    return _yapierr;
}

/**
 * Returns the block number of the first RFID tag memory block affected
 * by the operation. Depending on the type of operation and on the tag
 * memory granularity, this number may be smaller than the requested
 * memory block index.
 *
 * @return an RFID tag block number
 */
int YRfidStatus::get_firstAffectedBlock(void)
{
    return _fab;
}

/**
 * Returns the block number of the last RFID tag memory block affected
 * by the operation. Depending on the type of operation and on the tag
 * memory granularity, this number may be bigger than the requested
 * memory block index.
 *
 * @return an RFID tag block number
 */
int YRfidStatus::get_lastAffectedBlock(void)
{
    return _lab;
}

void YRfidStatus::imm_init(string tagId,int errCode,int errBlk,int fab,int lab)
{
    string errMsg;
    if (errCode == 0) {
        _yapierr = YAPI_SUCCESS;
        errMsg = "Success (no error)";
    } else {
        if (errCode < 0) {
            if (errCode > -50) {
                _yapierr = errCode;
                errMsg = YapiWrapper::ysprintf("YoctoLib error %d",errCode);
            } else {
                _yapierr = YAPI_RFID_HARD_ERROR;
                errMsg = YapiWrapper::ysprintf("Non-recoverable RFID error %d",errCode);
            }
        } else {
            if (errCode > 1000) {
                _yapierr = YAPI_RFID_SOFT_ERROR;
                errMsg = YapiWrapper::ysprintf("Recoverable RFID error %d",errCode);
            } else {
                _yapierr = YAPI_RFID_HARD_ERROR;
                errMsg = YapiWrapper::ysprintf("Non-recoverable RFID error %d",errCode);
            }
        }
        if (errCode == YRfidStatus::TAG_NOTFOUND) {
            errMsg = "Tag not found";
        }
        if (errCode == YRfidStatus::TAG_JUSTLEFT) {
            errMsg = "Tag left during operation";
        }
        if (errCode == YRfidStatus::TAG_LEFT) {
            errMsg = "Tag not here anymore";
        }
        if (errCode == YRfidStatus::READER_BUSY) {
            errMsg = "Reader is busy";
        }
        if (errCode == YRfidStatus::INVALID_CMD_ARGUMENTS) {
            errMsg = "Invalid command arguments";
        }
        if (errCode == YRfidStatus::UNKNOWN_CAPABILITIES) {
            errMsg = "Unknown capabilities";
        }
        if (errCode == YRfidStatus::MEMORY_NOT_SUPPORTED) {
            errMsg = "Memory no present";
        }
        if (errCode == YRfidStatus::INVALID_BLOCK_INDEX) {
            errMsg = "Invalid block index";
        }
        if (errCode == YRfidStatus::MEM_SPACE_UNVERRUN_ATTEMPT) {
            errMsg = "Tag memory space overrun attempt";
        }
        if (errCode == YRfidStatus::COMMAND_NOT_SUPPORTED) {
            errMsg = "The command is not supported";
        }
        if (errCode == YRfidStatus::COMMAND_NOT_RECOGNIZED) {
            errMsg = "The command is not recognized";
        }
        if (errCode == YRfidStatus::COMMAND_OPTION_NOT_RECOGNIZED) {
            errMsg = "The command option is not supported.";
        }
        if (errCode == YRfidStatus::COMMAND_CANNOT_BE_PROCESSED_IN_TIME) {
            errMsg = "The command cannot be processed in time";
        }
        if (errCode == YRfidStatus::UNDOCUMENTED_ERROR) {
            errMsg = "Error with no information given";
        }
        if (errCode == YRfidStatus::BLOCK_NOT_AVAILABLE) {
            errMsg = "Block is not available";
        }
        if (errCode == YRfidStatus::BLOCK_ALREADY_LOCKED) {
            errMsg = "Block is already locked and thus cannot be locked again.";
        }
        if (errCode == YRfidStatus::BLOCK_LOCKED) {
            errMsg = "Block is locked and its content cannot be changed";
        }
        if (errCode == YRfidStatus::BLOCK_NOT_SUCESSFULLY_PROGRAMMED) {
            errMsg = "Block was not successfully programmed";
        }
        if (errCode == YRfidStatus::BLOCK_NOT_SUCESSFULLY_LOCKED) {
            errMsg = "Block was not successfully locked";
        }
        if (errCode == YRfidStatus::BLOCK_IS_PROTECTED) {
            errMsg = "Block is protected";
        }
        if (errCode == YRfidStatus::CRYPTOGRAPHIC_ERROR) {
            errMsg = "Generic cryptographic error";
        }
        if (errCode == YRfidStatus::BROWNOUT_DETECTED) {
            errMsg = "BrownOut detected (BOD)";
        }
        if (errCode == YRfidStatus::BUFFER_OVERFLOW) {
            errMsg = "Buffer Overflow (BOF)";
        }
        if (errCode == YRfidStatus::CRC_ERROR) {
            errMsg = "Communication CRC Error (CCE)";
        }
        if (errCode == YRfidStatus::COLLISION_DETECTED) {
            errMsg = "Collision Detected (CLD/CDT)";
        }
        if (errCode == YRfidStatus::COMMAND_RECEIVE_TIMEOUT) {
            errMsg = "Command Receive Timeout (CRT)";
        }
        if (errCode == YRfidStatus::DID_NOT_SLEEP) {
            errMsg = "Did Not Sleep (DNS)";
        }
        if (errCode == YRfidStatus::ERROR_DECIMAL_EXPECTED) {
            errMsg = "Error Decimal Expected (EDX)";
        }
        if (errCode == YRfidStatus::HARDWARE_FAILURE) {
            errMsg = "Error Hardware Failure (EHF)";
        }
        if (errCode == YRfidStatus::ERROR_HEX_EXPECTED) {
            errMsg = "Error Hex Expected (EHX)";
        }
        if (errCode == YRfidStatus::FIFO_LENGTH_ERROR) {
            errMsg = "FIFO length error (FLE)";
        }
        if (errCode == YRfidStatus::FRAMING_ERROR) {
            errMsg = "Framing error (FER)";
        }
        if (errCode == YRfidStatus::NOT_IN_CNR_MODE) {
            errMsg = "Not in CNR Mode (NCM)";
        }
        if (errCode == YRfidStatus::NUMBER_OU_OF_RANGE) {
            errMsg = "Number Out of Range (NOR)";
        }
        if (errCode == YRfidStatus::NOT_SUPPORTED) {
            errMsg = "Not Supported (NOS)";
        }
        if (errCode == YRfidStatus::NO_RF_FIELD_ACTIVE) {
            errMsg = "No RF field active (NRF)";
        }
        if (errCode == YRfidStatus::READ_DATA_LENGTH_ERROR) {
            errMsg = "Read data length error (RDL)";
        }
        if (errCode == YRfidStatus::WATCHDOG_RESET) {
            errMsg = "Watchdog reset (SRT)";
        }
        if (errCode == YRfidStatus::TAG_COMMUNICATION_ERROR) {
            errMsg = "Tag Communication Error (TCE)";
        }
        if (errCode == YRfidStatus::TAG_NOT_RESPONDING) {
            errMsg = "Tag Not Responding (TNR)";
        }
        if (errCode == YRfidStatus::TIMEOUT_ERROR) {
            errMsg = "TimeOut Error (TOE)";
        }
        if (errCode == YRfidStatus::UNKNOW_COMMAND) {
            errMsg = "Unknown Command (UCO)";
        }
        if (errCode == YRfidStatus::UNKNOW_ERROR) {
            errMsg = "Unknown error (UER)";
        }
        if (errCode == YRfidStatus::UNKNOW_PARAMETER) {
            errMsg = "Unknown Parameter (UPA)";
        }
        if (errCode == YRfidStatus::UART_RECEIVE_ERROR) {
            errMsg = "UART Receive Error (URE)";
        }
        if (errCode == YRfidStatus::WRONG_DATA_LENGTH) {
            errMsg = "Wrong Data Length (WDL)";
        }
        if (errCode == YRfidStatus::WRONG_MODE) {
            errMsg = "Wrong Mode (WMO)";
        }
        if (errCode == YRfidStatus::UNKNOWN_DWARFxx_ERROR_CODE) {
            errMsg = "Unknown DWARF15 error code";
        }
        if (errCode == YRfidStatus::UNEXPECTED_TAG_ID_IN_RESPONSE) {
            errMsg = "Unexpected Tag id in response";
        }
        if (errCode == YRfidStatus::UNEXPECTED_TAG_INDEX) {
            errMsg = "internal error : unexpected TAG index";
        }
        if (errCode == YRfidStatus::TRANSFER_CLOSED) {
            errMsg = "transfer closed";
        }
        if (errCode == YRfidStatus::WRITE_DATA_MISSING) {
            errMsg = "Missing write data";
        }
        if (errCode == YRfidStatus::WRITE_TOO_MUCH_DATA) {
            errMsg = "Attempt to write too much data";
        }
        if (errCode == YRfidStatus::COULD_NOT_BUILD_REQUEST) {
            errMsg = "Could not not request";
        }
        if (errCode == YRfidStatus::INVALID_OPTIONS) {
            errMsg = "Invalid transfer options";
        }
        if (errCode == YRfidStatus::UNEXPECTED_RESPONSE) {
            errMsg = "Unexpected Tag response";
        }
        if (errCode == YRfidStatus::AFI_NOT_AVAILABLE) {
            errMsg = "AFI not available";
        }
        if (errCode == YRfidStatus::DSFID_NOT_AVAILABLE) {
            errMsg = "DSFID not available";
        }
        if (errCode == YRfidStatus::TAG_RESPONSE_TOO_SHORT) {
            errMsg = "Tag's response too short";
        }
        if (errCode == YRfidStatus::DEC_EXPECTED) {
            errMsg = "Error Decimal value Expected, or is missing";
        }
        if (errCode == YRfidStatus::HEX_EXPECTED) {
            errMsg = "Error Hexadecimal value Expected, or is missing";
        }
        if (errCode == YRfidStatus::NOT_SAME_SECOR) {
            errMsg = "Input and Output block are not in the same Sector";
        }
        if (errCode == YRfidStatus::MIFARE_AUTHENTICATED) {
            errMsg = "No chip with MIFARE Classic technology Authenticated";
        }
        if (errCode == YRfidStatus::NO_DATABLOCK) {
            errMsg = "No Data Block";
        }
        if (errCode == YRfidStatus::KEYB_IS_READABLE) {
            errMsg = "Key B is Readable";
        }
        if (errCode == YRfidStatus::OPERATION_NOT_EXECUTED) {
            errMsg = "Operation Not Executed, would have caused an overflow";
        }
        if (errCode == YRfidStatus::BLOK_MODE_ERROR) {
            errMsg = "Block has not been initialized as a 'value block'";
        }
        if (errCode == YRfidStatus::BLOCK_NOT_WRITABLE) {
            errMsg = "Block Not Writable";
        }
        if (errCode == YRfidStatus::BLOCK_ACCESS_ERROR) {
            errMsg = "Block Access Error";
        }
        if (errCode == YRfidStatus::BLOCK_NOT_AUTHENTICATED) {
            errMsg = "Block Not Authenticated";
        }
        if (errCode == YRfidStatus::ACCESS_KEY_BIT_NOT_WRITABLE) {
            errMsg = "Access bits or Keys not Writable";
        }
        if (errCode == YRfidStatus::USE_KEYA_FOR_AUTH) {
            errMsg = "Use Key B for authentication";
        }
        if (errCode == YRfidStatus::USE_KEYB_FOR_AUTH) {
            errMsg = "Use Key A for authentication";
        }
        if (errCode == YRfidStatus::KEY_NOT_CHANGEABLE) {
            errMsg = "Key(s) not changeable";
        }
        if (errCode == YRfidStatus::BLOCK_TOO_HIGH) {
            errMsg = "Block index is too high";
        }
        if (errCode == YRfidStatus::AUTH_ERR) {
            errMsg = "Authentication Error (i.e. wrong key)";
        }
        if (errCode == YRfidStatus::NOKEY_SELECT) {
            errMsg = "No Key Select, select a temporary or a static key";
        }
        if (errCode == YRfidStatus::CARD_NOT_SELECTED) {
            errMsg = " Card is Not Selected";
        }
        if (errCode == YRfidStatus::BLOCK_TO_READ_NONE) {
            errMsg = "Number of Blocks to Read is 0";
        }
        if (errCode == YRfidStatus::NO_TAG) {
            errMsg = "No Tag detected";
        }
        if (errCode == YRfidStatus::TOO_MUCH_DATA) {
            errMsg = "Too Much Data (i.e. Uart input buffer overflow)";
        }
        if (errCode == YRfidStatus::CON_NOT_SATISFIED) {
            errMsg = "Conditions Not Satisfied";
        }
        if (errCode == YRfidStatus::BLOCK_IS_SPECIAL) {
            errMsg = "Bad parameter: block is a special block";
        }
        if (errCode == YRfidStatus::READ_BEYOND_ANNOUNCED_SIZE) {
            errMsg = "Attempt to read more than announced size.";
        }
        if (errCode == YRfidStatus::BLOCK_ZERO_IS_RESERVED) {
            errMsg = "Block 0 is reserved and cannot be used";
        }
        if (errCode == YRfidStatus::VALUE_BLOCK_BAD_FORMAT) {
            errMsg = "One value block is not properly initialized";
        }
        if (errCode == YRfidStatus::ISO15693_ONLY_FEATURE) {
            errMsg = "Feature available on ISO 15693 only";
        }
        if (errCode == YRfidStatus::ISO14443_ONLY_FEATURE) {
            errMsg = "Feature available on ISO 14443 only";
        }
        if (errCode == YRfidStatus::MIFARE_CLASSIC_ONLY_FEATURE) {
            errMsg = "Feature available on ISO 14443 MIFARE Classic only";
        }
        if (errCode == YRfidStatus::BLOCK_MIGHT_BE_PROTECTED) {
            errMsg = "Block might be protected";
        }
        if (errCode == YRfidStatus::NO_SUCH_BLOCK) {
            errMsg = "No such block";
        }
        if (errCode == YRfidStatus::COUNT_TOO_BIG) {
            errMsg = "Count parameter is too large";
        }
        if (errCode == YRfidStatus::UNKNOWN_MEM_SIZE) {
            errMsg = "Tag memory size is unknown";
        }
        if (errCode == YRfidStatus::MORE_THAN_2BLOCKS_MIGHT_NOT_WORK) {
            errMsg = "Writing more than two blocks at once might not be supported by this tag";
        }
        if (errCode == YRfidStatus::READWRITE_NOT_SUPPORTED) {
            errMsg = "Read/write operation not supported for this tag";
        }
        if (errCode == YRfidStatus::UNEXPECTED_VICC_ID_IN_RESPONSE) {
            errMsg = "Unexpected VICC ID in response";
        }
        if (errCode == YRfidStatus::LOCKBLOCK_NOT_SUPPORTED) {
            errMsg = "This tag does not support the Lock block function";
        }
        if (errCode == YRfidStatus::INTERNAL_ERROR_SHOULD_NEVER_HAPPEN) {
            errMsg = "Yoctopuce RFID code ran into an unexpected state, please contact support";
        }
        if (errCode == YRfidStatus::INVLD_BLOCK_MODE_COMBINATION) {
            errMsg = "Invalid combination of block mode options";
        }
        if (errCode == YRfidStatus::INVLD_ACCESS_MODE_COMBINATION) {
            errMsg = "Invalid combination of access mode options";
        }
        if (errCode == YRfidStatus::INVALID_SIZE) {
            errMsg = "Invalid data size parameter";
        }
        if (errCode == YRfidStatus::BAD_PASSWORD_FORMAT) {
            errMsg = "Bad password format or type";
        }
        if (errBlk >= 0) {
            errMsg = YapiWrapper::ysprintf("%s (block %d)", errMsg.c_str(),errBlk);
        }
    }
    _tagId = tagId;
    _errCode = errCode;
    _errBlk = errBlk;
    _errMsg = errMsg;
    _fab = fab;
    _lab = lab;
}
//--- (end of generated code: YRfidStatus implementation)


YRfidReader::YRfidReader(const string& func): YFunction(func)
//--- (generated code: YRfidReader initialization)
    ,_nTags(NTAGS_INVALID)
    ,_refreshRate(REFRESHRATE_INVALID)
    ,_valueCallbackRfidReader(NULL)
    ,_eventCallback(NULL)
    ,_isFirstCb(0)
    ,_prevCbPos(0)
    ,_eventPos(0)
    ,_eventStamp(0)
//--- (end of generated code: YRfidReader initialization)
{
    _className="RfidReader";
}

YRfidReader::~YRfidReader()
{
//--- (generated code: YRfidReader cleanup)
//--- (end of generated code: YRfidReader cleanup)
}
//--- (generated code: YRfidReader implementation)
void YRfidReader::yInternalEventCallback(YRfidReader *obj, const string& value)
{
    obj->_internalEventHandler(value);
}

// static attributes

int YRfidReader::_parseAttr(YJSONObject *json_val)
{
    if(json_val->has("nTags")) {
        _nTags =  json_val->getInt("nTags");
    }
    if(json_val->has("refreshRate")) {
        _refreshRate =  json_val->getInt("refreshRate");
    }
    return YFunction::_parseAttr(json_val);
}


/**
 * Returns the number of RFID tags currently detected.
 *
 * @return an integer corresponding to the number of RFID tags currently detected
 *
 * On failure, throws an exception or returns YRfidReader::NTAGS_INVALID.
 */
int YRfidReader::get_nTags(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YRfidReader::NTAGS_INVALID;
                }
            }
        }
        res = _nTags;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the tag list refresh rate, measured in Hz.
 *
 * @return an integer corresponding to the tag list refresh rate, measured in Hz
 *
 * On failure, throws an exception or returns YRfidReader::REFRESHRATE_INVALID.
 */
int YRfidReader::get_refreshRate(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YRfidReader::REFRESHRATE_INVALID;
                }
            }
        }
        res = _refreshRate;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the present tag list refresh rate, measured in Hz. The reader will do
 * its best to respect it. Note that the reader cannot detect tag arrival or removal
 * while it is  communicating with a tag.  Maximum frequency is limited to 100Hz,
 * but in real life it will be difficult to do better than 50Hz.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : an integer corresponding to the present tag list refresh rate, measured in Hz
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YRfidReader::set_refreshRate(int newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("refreshRate", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Retrieves a RFID reader for a given identifier.
 * The identifier can be specified using several formats:
 * <ul>
 * <li>FunctionLogicalName</li>
 * <li>ModuleSerialNumber.FunctionIdentifier</li>
 * <li>ModuleSerialNumber.FunctionLogicalName</li>
 * <li>ModuleLogicalName.FunctionIdentifier</li>
 * <li>ModuleLogicalName.FunctionLogicalName</li>
 * </ul>
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
YRfidReader* YRfidReader::FindRfidReader(string func)
{
    YRfidReader* obj = NULL;
    int taken = 0;
    if (YAPI::_apiInitialized) {
        yEnterCriticalSection(&YAPI::_global_cs);
        taken = 1;
    }try {
        obj = (YRfidReader*) YFunction::_FindFromCache("RfidReader", func);
        if (obj == NULL) {
            obj = new YRfidReader(func);
            YFunction::_AddToCache("RfidReader", func, obj);
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
int YRfidReader::registerValueCallback(YRfidReaderValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackRfidReader = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YRfidReader::_invokeValueCallback(string value)
{
    if (_valueCallbackRfidReader != NULL) {
        _valueCallbackRfidReader(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

int YRfidReader::_chkerror(string tagId,string json,YRfidStatus& status)
{
    string jsonStr;
    int errCode = 0;
    int errBlk = 0;
    int fab = 0;
    int lab = 0;
    int retcode = 0;

    if ((int)(json).size() == 0) {
        errCode = this->get_errorType();
        errBlk = -1;
        fab = -1;
        lab = -1;
    } else {
        jsonStr = json;
        errCode = atoi((this->_json_get_key(json, "err")).c_str());
        errBlk = atoi((this->_json_get_key(json, "errBlk")).c_str())-1;
        if (_ystrpos(jsonStr, "\"fab\":") >= 0) {
            fab = atoi((this->_json_get_key(json, "fab")).c_str())-1;
            lab = atoi((this->_json_get_key(json, "lab")).c_str())-1;
        } else {
            fab = -1;
            lab = -1;
        }
    }
    status.imm_init(tagId, errCode, errBlk, fab, lab);
    retcode = status.get_yapiError();
    if (!(retcode == YAPI_SUCCESS)) {
        _throw((YRETCODE)(retcode), status.get_errorMessage());
        return retcode;
    }
    return YAPI_SUCCESS;
}

int YRfidReader::reset(void)
{
    string json;
    YRfidStatus status;
    status = YRfidStatus();

    json = this->_download("rfid.json?a=reset");
    return this->_chkerror("", json, status);
}

/**
 * Returns the list of RFID tags currently detected by the reader.
 *
 * @return a list of strings, corresponding to each tag identifier.
 *
 * On failure, throws an exception or returns an empty list.
 */
vector<string> YRfidReader::get_tagIdList(void)
{
    string json;
    vector<string> jsonList;
    vector<string> taglist;

    json = this->_download("rfid.json?a=list");
    taglist.clear();
    if ((int)(json).size() > 3) {
        jsonList = this->_json_get_array(json);
        for (unsigned ii = 0; ii < jsonList.size(); ii++) {
            taglist.push_back(this->_json_get_string(jsonList[ii]));
        }
    }
    return taglist;
}

/**
 * Retourne la description des propriétés d'un tag RFID présent.
 * Cette fonction peut causer des communications avec le tag.
 *
 * @param tagId : identifier of the tag to check
 * @param status : an RfidStatus object that will contain
 *         the detailled status of the operation
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns an empty YRfidTagInfo objact.
 * When it happens, you can get more information from the status object.
 */
YRfidTagInfo YRfidReader::get_tagInfo(string tagId,YRfidStatus& status)
{
    string url;
    string json;
    int tagType = 0;
    int size = 0;
    int usable = 0;
    int blksize = 0;
    int fblk = 0;
    int lblk = 0;
    YRfidTagInfo res;
    url = YapiWrapper::ysprintf("rfid.json?a=info&t=%s",tagId.c_str());

    json = this->_download(url);
    this->_chkerror(tagId, json, status);
    tagType = atoi((this->_json_get_key(json, "type")).c_str());
    size = atoi((this->_json_get_key(json, "size")).c_str());
    usable = atoi((this->_json_get_key(json, "usable")).c_str());
    blksize = atoi((this->_json_get_key(json, "blksize")).c_str());
    fblk = atoi((this->_json_get_key(json, "fblk")).c_str());
    lblk = atoi((this->_json_get_key(json, "lblk")).c_str());
    res = YRfidTagInfo();
    res.imm_init(tagId, tagType, size, usable, blksize, fblk, lblk);
    return res;
}

/**
 * Change an RFID tag configuration to prevents any further write to
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
int YRfidReader::tagLockBlocks(string tagId,int firstBlock,int nBlocks,YRfidOptions options,YRfidStatus& status)
{
    string optstr;
    string url;
    string json;
    optstr = options.imm_getParams();
    url = YapiWrapper::ysprintf("rfid.json?a=lock&t=%s&b=%d&n=%d%s",tagId.c_str(),firstBlock,nBlocks,optstr.c_str());

    json = this->_download(url);
    return this->_chkerror(tagId, json, status);
}

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
vector<bool> YRfidReader::get_tagLockState(string tagId,int firstBlock,int nBlocks,YRfidOptions options,YRfidStatus& status)
{
    string optstr;
    string url;
    string json;
    string binRes;
    vector<bool> res;
    int idx = 0;
    int val = 0;
    bool isLocked = 0;
    optstr = options.imm_getParams();
    url = YapiWrapper::ysprintf("rfid.json?a=chkl&t=%s&b=%d&n=%d%s",tagId.c_str(),firstBlock,nBlocks,optstr.c_str());

    json = this->_download(url);
    this->_chkerror(tagId, json, status);
    if (status.get_yapiError() != YAPI_SUCCESS) {
        return res;
    }
    binRes = YAPI::_hexStr2Bin(this->_json_get_key(json, "bitmap"));
    idx = 0;
    while (idx < nBlocks) {
        val = ((u8)binRes[((idx) >> (3))]);
        isLocked = (((val) & (((1) << (((idx) & (7)))))) != 0);
        res.push_back(isLocked);
        idx = idx + 1;
    }
    return res;
}

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
vector<bool> YRfidReader::get_tagSpecialBlocks(string tagId,int firstBlock,int nBlocks,YRfidOptions options,YRfidStatus& status)
{
    string optstr;
    string url;
    string json;
    string binRes;
    vector<bool> res;
    int idx = 0;
    int val = 0;
    bool isLocked = 0;
    optstr = options.imm_getParams();
    url = YapiWrapper::ysprintf("rfid.json?a=chks&t=%s&b=%d&n=%d%s",tagId.c_str(),firstBlock,nBlocks,optstr.c_str());

    json = this->_download(url);
    this->_chkerror(tagId, json, status);
    if (status.get_yapiError() != YAPI_SUCCESS) {
        return res;
    }
    binRes = YAPI::_hexStr2Bin(this->_json_get_key(json, "bitmap"));
    idx = 0;
    while (idx < nBlocks) {
        val = ((u8)binRes[((idx) >> (3))]);
        isLocked = (((val) & (((1) << (((idx) & (7)))))) != 0);
        res.push_back(isLocked);
        idx = idx + 1;
    }
    return res;
}

/**
 * Reads data from an RFID tag memory, as an hexadecimal string.
 * The read operation may span accross multiple blocks if the requested
 * number of bytes is larger than the RFID tag block size. By default
 * firstBlock cannot be a special block, and any special block encountered
 * in the middle of the read operation will be skipped automatically.
 * If you rather want to read special blocks, use EnableRawAccess option.
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
string YRfidReader::tagReadHex(string tagId,int firstBlock,int nBytes,YRfidOptions options,YRfidStatus& status)
{
    string optstr;
    string url;
    string json;
    string hexbuf;
    optstr = options.imm_getParams();
    url = YapiWrapper::ysprintf("rfid.json?a=read&t=%s&b=%d&n=%d%s",tagId.c_str(),firstBlock,nBytes,optstr.c_str());

    json = this->_download(url);
    this->_chkerror(tagId, json, status);
    if (status.get_yapiError() == YAPI_SUCCESS) {
        hexbuf = this->_json_get_key(json, "res");
    } else {
        hexbuf = "";
    }
    return hexbuf;
}

/**
 * Reads data from an RFID tag memory, as a binary buffer. The read operation
 * may span accross multiple blocks if the requested number of bytes
 * is larger than the RFID tag block size.  By default
 * firstBlock cannot be a special block, and any special block encountered
 * in the middle of the read operation will be skipped automatically.
 * If you rather want to read special blocks, use EnableRawAccess option.
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
string YRfidReader::tagReadBin(string tagId,int firstBlock,int nBytes,YRfidOptions options,YRfidStatus& status)
{
    return YAPI::_hexStr2Bin(this->tagReadHex(tagId, firstBlock, nBytes, options, status));
}

/**
 * Reads data from an RFID tag memory, as a byte list. The read operation
 * may span accross multiple blocks if the requested number of bytes
 * is larger than the RFID tag block size.  By default
 * firstBlock cannot be a special block, and any special block encountered
 * in the middle of the read operation will be skipped automatically.
 * If you rather want to read special blocks, use EnableRawAccess option.
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
vector<int> YRfidReader::tagReadArray(string tagId,int firstBlock,int nBytes,YRfidOptions options,YRfidStatus& status)
{
    string blk;
    int idx = 0;
    int endidx = 0;
    vector<int> res;
    blk = this->tagReadBin(tagId, firstBlock, nBytes, options, status);
    endidx = (int)(blk).size();
    idx = 0;
    while (idx < endidx) {
        res.push_back(((u8)blk[idx]));
        idx = idx + 1;
    }
    return res;
}

/**
 * Reads data from an RFID tag memory, as a text string. The read operation
 * may span accross multiple blocks if the requested number of bytes
 * is larger than the RFID tag block size.  By default
 * firstBlock cannot be a special block, and any special block encountered
 * in the middle of the read operation will be skipped automatically.
 * If you rather want to read special blocks, use EnableRawAccess option.
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
string YRfidReader::tagReadStr(string tagId,int firstBlock,int nChars,YRfidOptions options,YRfidStatus& status)
{
    return this->tagReadBin(tagId, firstBlock, nChars, options, status);
}

/**
 * Writes data provided as a binary buffer to an RFID tag memory.
 * The write operation may span accross multiple blocks if the
 * number of bytes to write is larger than the RFID tag block size.
 * By default firstBlock cannot be a special block, and any special block
 * encountered in the middle of the write operation will be skipped
 * automatically. If you rather want to rewrite special blocks as well,
 * use EnableRawAccess option.
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
int YRfidReader::tagWriteBin(string tagId,int firstBlock,string buff,YRfidOptions options,YRfidStatus& status)
{
    string optstr;
    string hexstr;
    int buflen = 0;
    string fname;
    string json;
    buflen = (int)(buff).size();
    if (buflen <= 16) {
        // short data, use an URL-based command
        hexstr = YAPI::_bin2HexStr(buff);
        return this->tagWriteHex(tagId, firstBlock, hexstr, options, status);
    } else {
        // long data, use an upload command
        optstr = options.imm_getParams();
        fname = YapiWrapper::ysprintf("Rfid:t=%s&b=%d&n=%d%s",tagId.c_str(),firstBlock,buflen,optstr.c_str());
        json = this->_uploadEx(fname, buff);
        return this->_chkerror(tagId, json, status);
    }
}

/**
 * Writes data provided as a list of bytes to an RFID tag memory.
 * The write operation may span accross multiple blocks if the
 * number of bytes to write is larger than the RFID tag block size.
 * By default firstBlock cannot be a special block, and any special block
 * encountered in the middle of the write operation will be skipped
 * automatically. If you rather want to rewrite special blocks as well,
 * use EnableRawAccess option.
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
int YRfidReader::tagWriteArray(string tagId,int firstBlock,vector<int> byteList,YRfidOptions options,YRfidStatus& status)
{
    int bufflen = 0;
    string buff;
    int idx = 0;
    int hexb = 0;
    bufflen = (int)byteList.size();
    buff = string(bufflen, (char)0);
    idx = 0;
    while (idx < bufflen) {
        hexb = byteList[idx];
        buff[idx] = (char)(hexb);
        idx = idx + 1;
    }

    return this->tagWriteBin(tagId, firstBlock, buff, options, status);
}

/**
 * Writes data provided as an hexadecimal string to an RFID tag memory.
 * The write operation may span accross multiple blocks if the
 * number of bytes to write is larger than the RFID tag block size.
 * By default firstBlock cannot be a special block, and any special block
 * encountered in the middle of the write operation will be skipped
 * automatically. If you rather want to rewrite special blocks as well,
 * use EnableRawAccess option.
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
int YRfidReader::tagWriteHex(string tagId,int firstBlock,string hexString,YRfidOptions options,YRfidStatus& status)
{
    int bufflen = 0;
    string optstr;
    string url;
    string json;
    string buff;
    int idx = 0;
    int hexb = 0;
    bufflen = (int)(hexString).length();
    bufflen = ((bufflen) >> (1));
    if (bufflen <= 16) {
        // short data, use an URL-based command
        optstr = options.imm_getParams();
        url = YapiWrapper::ysprintf("rfid.json?a=writ&t=%s&b=%d&w=%s%s",tagId.c_str(),firstBlock,hexString.c_str(),optstr.c_str());
        json = this->_download(url);
        return this->_chkerror(tagId, json, status);
    } else {
        // long data, use an upload command
        buff = string(bufflen, (char)0);
        idx = 0;
        while (idx < bufflen) {
            hexb = (int)YAPI::_hexStr2Long((hexString).substr(2 * idx, 2));
            buff[idx] = (char)(hexb);
            idx = idx + 1;
        }
        return this->tagWriteBin(tagId, firstBlock, buff, options, status);
    }
}

/**
 * Writes data provided as an ASCII string to an RFID tag memory.
 * The write operation may span accross multiple blocks if the
 * number of bytes to write is larger than the RFID tag block size.
 * By default firstBlock cannot be a special block, and any special block
 * encountered in the middle of the write operation will be skipped
 * automatically. If you rather want to rewrite special blocks as well,
 * use EnableRawAccess option.
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
int YRfidReader::tagWriteStr(string tagId,int firstBlock,string text,YRfidOptions options,YRfidStatus& status)
{
    string buff;
    buff = text;

    return this->tagWriteBin(tagId, firstBlock, buff, options, status);
}

/**
 * Returns a string with last tag arrival/removal events observed.
 * This method return only events that are still buffered in the device memory.
 *
 * @return a string with last events observed (one per line).
 *
 * On failure, throws an exception or returns  YAPI::INVALID_STRING.
 */
string YRfidReader::get_lastEvents(void)
{
    string content;

    content = this->_download("events.txt");
    return content;
}

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
int YRfidReader::registerEventCallback(YEventCallback callback)
{
    _eventCallback = callback;
    _isFirstCb = true;
    if (callback != NULL) {
        this->registerValueCallback(yInternalEventCallback);
    } else {
        this->registerValueCallback((YRfidReaderValueCallback) NULL);
    }
    return 0;
}

int YRfidReader::_internalEventHandler(string cbVal)
{
    int cbPos = 0;
    int cbDPos = 0;
    int cbNtags = 0;
    int searchTags = 0;
    string url;
    string content;
    string contentStr;
    vector<string> currentTags;
    vector<string> eventArr;
    int arrLen = 0;
    vector<int> lastEvents;
    string lenStr;
    int arrPos = 0;
    string eventStr;
    int eventLen = 0;
    string hexStamp;
    int typePos = 0;
    int dataPos = 0;
    int evtStamp = 0;
    string evtType;
    string evtData;
    int tagIdx = 0;
    // detect possible power cycle of the reader to clear event pointer
    cbPos = atoi((cbVal).c_str());
    cbNtags = ((cbPos) % (1000));
    cbPos = ((cbPos) / (1000));
    cbDPos = ((cbPos - _prevCbPos) & (0x7ffff));
    _prevCbPos = cbPos;
    if (cbDPos > 16384) {
        _eventPos = 0;
    }
    if (!(_eventCallback != NULL)) {
        return YAPI_SUCCESS;
    }
    // load all events since previous call
    url = YapiWrapper::ysprintf("events.txt?pos=%d",_eventPos);

    content = this->_download(url);
    contentStr = content;
    eventArr = _strsplit(contentStr,'\n');
    arrLen = (int)eventArr.size();
    if (!(arrLen > 0)) {
        _throw((YRETCODE)(YAPI_IO_ERROR), "fail to download events");
        return YAPI_IO_ERROR;
    }
    // last element of array is the new position preceeded by '@'
    arrLen = arrLen - 1;
    lenStr = eventArr[arrLen];
    lenStr = (lenStr).substr(1, (int)(lenStr).length()-1);
    // update processed event position pointer
    _eventPos = atoi((lenStr).c_str());
    if (_isFirstCb) {
        // first emulated value callback caused by registerValueCallback:
        // attempt to retrieve arrivals of all tags present to emulate arrival
        _isFirstCb = false;
        _eventStamp = 0;
        if (cbNtags == 0) {
            return YAPI_SUCCESS;
        }
        currentTags = this->get_tagIdList();
        cbNtags = (int)currentTags.size();
        searchTags = cbNtags;
        lastEvents.clear();
        arrPos = arrLen - 1;
        while ((arrPos >= 0) && (searchTags > 0)) {
            eventStr = eventArr[arrPos];
            typePos = _ystrpos(eventStr, ":")+1;
            if (typePos > 8) {
                dataPos = _ystrpos(eventStr, "=")+1;
                evtType = (eventStr).substr(typePos, 1);
                if ((dataPos > 10) && evtType == "+") {
                    evtData = (eventStr).substr(dataPos, (int)(eventStr).length()-dataPos);
                    tagIdx = searchTags - 1;
                    while (tagIdx >= 0) {
                        if (evtData == currentTags[tagIdx]) {
                            lastEvents.push_back(0+arrPos);
                            currentTags[tagIdx] = "";
                            while ((searchTags > 0) && currentTags[searchTags-1] == "") {
                                searchTags = searchTags - 1;
                            }
                            tagIdx = -1;
                        }
                        tagIdx = tagIdx - 1;
                    }
                }
            }
            arrPos = arrPos - 1;
        }
        // If we have any remaining tags without a known arrival event,
        // create a pseudo callback with timestamp zero
        tagIdx = 0;
        while (tagIdx < searchTags) {
            evtData = currentTags[tagIdx];
            if (!(evtData == "")) {
                _eventCallback(this, 0, "+", evtData);
            }
            tagIdx = tagIdx + 1;
        }
    } else {
        // regular callback
        lastEvents.clear();
        arrPos = arrLen - 1;
        while (arrPos >= 0) {
            lastEvents.push_back(0+arrPos);
            arrPos = arrPos - 1;
        }
    }
    // now generate callbacks for each selected event
    arrLen = (int)lastEvents.size();
    arrPos = arrLen - 1;
    while (arrPos >= 0) {
        tagIdx = lastEvents[arrPos];
        eventStr = eventArr[tagIdx];
        eventLen = (int)(eventStr).length();
        if (eventLen >= 1) {
            hexStamp = (eventStr).substr(0, 8);
            evtStamp = (int)YAPI::_hexStr2Long(hexStamp);
            typePos = _ystrpos(eventStr, ":")+1;
            if ((evtStamp >= _eventStamp) && (typePos > 8)) {
                _eventStamp = evtStamp;
                dataPos = _ystrpos(eventStr, "=")+1;
                evtType = (eventStr).substr(typePos, 1);
                evtData = "";
                if (dataPos > 10) {
                    evtData = (eventStr).substr(dataPos, eventLen-dataPos);
                }
                _eventCallback(this, evtStamp, evtType, evtData);
            }
        }
        arrPos = arrPos - 1;
    }
    return YAPI_SUCCESS;
}

YRfidReader *YRfidReader::nextRfidReader(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YRfidReader::FindRfidReader(hwid);
}

YRfidReader *YRfidReader::FirstRfidReader(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("RfidReader", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YRfidReader::FindRfidReader(serial+"."+funcId);
}

//--- (end of generated code: YRfidReader implementation)

//--- (generated code: YRfidReader functions)
//--- (end of generated code: YRfidReader functions)
