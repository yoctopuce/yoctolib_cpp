/*********************************************************************
 *
 * $Id: pic24config.php 9668 2013-02-04 12:36:11Z martinm $
 *
 * Declares yFindFiles(), the high-level API for Files functions
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
 *    THE SOFTWARE AND DOCUMENTATION ARE PROVIDED 'AS IS' WITHOUT
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


#ifndef YOCTO_FILES_H
#define YOCTO_FILES_H

#include "yocto_api.h"
#include <cfloat>
#include <cmath>
#include <map>

//--- (generated code: return codes)
//--- (end of generated code: return codes)
//--- (generated code: YFiles definitions)
class YFiles; //forward declaration

typedef void (*YFilesUpdateCallback)(YFiles *func, const string& functionValue);
#define Y_LOGICALNAME_INVALID           (YAPI::INVALID_STRING)
#define Y_ADVERTISEDVALUE_INVALID       (YAPI::INVALID_STRING)
#define Y_FILESCOUNT_INVALID            (0xffffffff)
#define Y_FREESPACE_INVALID             (0xffffffff)
//--- (end of generated code: YFiles definitions)


//--- (generated code: YFileRecord definitions)
//--- (end of generated code: YFileRecord definitions)


    //--- (generated code: YFileRecord declaration)
/**
 * YFileRecord Class: Description of a file on the device filesystem
 * 
 * 
 */
class YFileRecord {
protected:
    // Attributes (function value cache)
    //--- (end of generated code: YFileRecord declaration)
    string _name;
    int    _size;
    int    _crc;
    
public:
    YFileRecord(const string& json);
    //--- (generated code: YFileRecord accessors declaration)


    string             get_name();

    int             get_size();

    int             get_crc();

    string             name();

    int             size();

    int             crc();

    //--- (end of generated code: YFileRecord accessors declaration)
};





//--- (generated code: YFiles declaration)
/**
 * YFiles Class: Files function interface
 * 
 * The filesystem interface makes it possible to store files
 * on some devices, for instance to design a custom web UI
 * (for networked devices) or to add fonts (on display
 * devices).
 */
class YFiles: public YFunction {
protected:
    // Attributes (function value cache)
    YFilesUpdateCallback _callback;
    string          _logicalName;
    string          _advertisedValue;
    unsigned        _filesCount;
    unsigned        _freeSpace;
    // Static function object cache
    static std::map<string,YFiles*> _FilesCache;

    friend YFiles *yFindFiles(const string& func);
    friend YFiles *yFirstFiles(void);

    // Function-specific method for parsing of JSON output and caching result
    int             _parse(yJsonStateMachine& j);
    //--- (end of generated code: YFiles declaration)

    //--- (YFiles generated code: constructor)
    // Constructor is protected, use yFindFiles factory function to instantiate
    YFiles(const string& func): YFunction("Files", func)
    //--- (end of generated code: YFiles constructor)
    //--- (Files initialization)
            ,_callback(NULL)
            ,_logicalName(Y_LOGICALNAME_INVALID)
            ,_advertisedValue(Y_ADVERTISEDVALUE_INVALID)
            ,_filesCount(Y_FILESCOUNT_INVALID)
            ,_freeSpace(Y_FREESPACE_INVALID)
    //--- (end of generated code: Files initialization)
    {};

public:
    //--- (generated code: YFiles accessors declaration)

    static const string LOGICALNAME_INVALID;
    static const string ADVERTISEDVALUE_INVALID;
    static const unsigned FILESCOUNT_INVALID = 0xffffffff;
    static const unsigned FREESPACE_INVALID = 0xffffffff;

    /**
     * Returns the logical name of the filesystem.
     * 
     * @return a string corresponding to the logical name of the filesystem
     * 
     * On failure, throws an exception or returns Y_LOGICALNAME_INVALID.
     */
           string          get_logicalName(void);
    inline string          logicalName(void)
    { return this->get_logicalName(); }

    /**
     * Changes the logical name of the filesystem. You can use yCheckLogicalName()
     * prior to this call to make sure that your parameter is valid.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     * 
     * @param newval : a string corresponding to the logical name of the filesystem
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_logicalName(const string& newval);
    inline int      setLogicalName(const string& newval)
    { return this->set_logicalName(newval); }

    /**
     * Returns the current value of the filesystem (no more than 6 characters).
     * 
     * @return a string corresponding to the current value of the filesystem (no more than 6 characters)
     * 
     * On failure, throws an exception or returns Y_ADVERTISEDVALUE_INVALID.
     */
           string          get_advertisedValue(void);
    inline string          advertisedValue(void)
    { return this->get_advertisedValue(); }

    /**
     * Returns the number of files currently loaded in the filesystem.
     * 
     * @return an integer corresponding to the number of files currently loaded in the filesystem
     * 
     * On failure, throws an exception or returns Y_FILESCOUNT_INVALID.
     */
           unsigned        get_filesCount(void);
    inline unsigned        filesCount(void)
    { return this->get_filesCount(); }

    /**
     * Returns the free space for uploading new files to the filesystem, in bytes.
     * 
     * @return an integer corresponding to the free space for uploading new files to the filesystem, in bytes
     * 
     * On failure, throws an exception or returns Y_FREESPACE_INVALID.
     */
           unsigned        get_freeSpace(void);
    inline unsigned        freeSpace(void)
    { return this->get_freeSpace(); }

    string             sendCommand(string command);

    /**
     * Reinitializes the filesystem to its clean, unfragmented, empty state.
     * All files previously uploaded are permanently lost.
     * 
     * @return YAPI_SUCCESS if the call succeeds.
     * 
     * On failure, throws an exception or returns a negative error code.
     */
    int             format_fs();

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
    vector<YFileRecord*>             get_list(string pattern);

    /**
     * Downloads the requested file and returns a binary buffer with its content.
     * 
     * @param pathname : path and name of the new file to load
     * 
     * @return a binary buffer with the file content
     * 
     * On failure, throws an exception or returns an empty content.
     */
    string             download(string pathname);

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
    int             upload(string pathname,string content);

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
    int             remove(string pathname);


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
    void registerValueCallback(YFilesUpdateCallback callback);

    void advertiseValue(const string& value);

    /**
     * Continues the enumeration of filesystems started using yFirstFiles().
     * 
     * @return a pointer to a YFiles object, corresponding to
     *         a filesystem currently online, or a null pointer
     *         if there are no more filesystems to enumerate.
     */
           YFiles          *nextFiles(void);
    inline YFiles          *next(void)
    { return this->nextFiles();}

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
           static YFiles* FindFiles(const string& func);
    inline static YFiles* Find(const string& func)
    { return YFiles::FindFiles(func);}
    /**
     * Starts the enumeration of filesystems currently accessible.
     * Use the method YFiles.nextFiles() to iterate on
     * next filesystems.
     * 
     * @return a pointer to a YFiles object, corresponding to
     *         the first filesystem currently online, or a null pointer
     *         if there are none.
     */
           static YFiles* FirstFiles(void);
    inline static YFiles* First(void)
    { return YFiles::FirstFiles();}
    //--- (end of generated code: YFiles accessors declaration)
};

//--- (generated code: Files functions declaration)

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
inline YFiles* yFindFiles(const string& func)
{ return YFiles::FindFiles(func);}
/**
 * Starts the enumeration of filesystems currently accessible.
 * Use the method YFiles.nextFiles() to iterate on
 * next filesystems.
 * 
 * @return a pointer to a YFiles object, corresponding to
 *         the first filesystem currently online, or a null pointer
 *         if there are none.
 */
inline YFiles* yFirstFiles(void)
{ return YFiles::FirstFiles();}

//--- (end of generated code: Files functions declaration)

#endif
