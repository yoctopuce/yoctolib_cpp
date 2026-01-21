/*********************************************************************
 *
 * $Id: yocto_display.cpp 71207 2026-01-07 18:17:59Z mvuilleu $
 *
 * Implements yFindDisplay(), the high-level API for Display functions
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


#define _CRT_SECURE_NO_DEPRECATE //do not use windows secure crt
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "yocto_display.h"
#include "yapi/yproto.h"
#include "yapi/yjson.h"
#include "yapi/yapi.h"
#define  __FILE_ID__  "display"

#ifdef YOCTOLIB_NAMESPACE
using namespace YOCTOLIB_NAMESPACE;
#endif

YDisplayLayer::YDisplayLayer(YDisplay *parent, int id):
//--- (generated code: YDisplayLayer initialization)
//--- (end of generated code: YDisplayLayer initialization)
_display(parent),_id(id),_cmdbuff(""),_hidden(false)
{}


int YDisplayLayer::flush_now(void)
{
    int res = YAPI_SUCCESS;
    if(_cmdbuff.length() > 0) {
        res = _display->sendCommand(_cmdbuff);
        _cmdbuff="";
    }
    return res;
}


// internal function to send a command for this layer
int YDisplayLayer::command_push(string cmd)
{
    int res = YAPI_SUCCESS;

    if(_cmdbuff.length() + cmd.length() >= 100) {
        // force flush before, to prevent overflow
        res = flush_now();
    }
    if(_cmdbuff.length() == 0) {
        // always prepend layer ID first
        _cmdbuff.append(YapiWrapper::ysprintf("%d",_id));
    }
    _cmdbuff.append(cmd);
    return res;
}

// internal function to send a command for this layer
int YDisplayLayer::command_flush(string cmd)
{
    int  res = command_push(cmd);
    if(_hidden) {
        return res;
    }
    return flush_now();
}

int YDisplayLayer::drawBitmap(int x,int y,int w,const std::vector<unsigned char>& data,int bgcol)
{
	int size = (int)data.size();
	char *arr = new char[size];
	for (int i=0;i<size;i++) arr[i] = data[i];
	string strval = string(arr,size);
	return this->drawBitmap(x,y,w,strval,bgcol);
}


//--- (generated code: YDisplayLayer implementation)
// static attributes


/**
 * Reverts the layer to its initial state (fully transparent, default settings).
 * Reinitializes the drawing pointer to the upper left position,
 * and selects the most visible pen color. If you only want to erase the layer
 * content, use the method clear() instead.
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplayLayer::reset(void)
{
    _hidden = false;
    return this->command_flush("X");
}

/**
 * Erases the whole content of the layer (makes it fully transparent).
 * This method does not change any other attribute of the layer.
 * To reinitialize the layer attributes to defaults settings, use the method
 * reset() instead.
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplayLayer::clear(void)
{
    return this->command_flush("x");
}

/**
 * Selects the pen color for all subsequent drawing functions,
 * including text drawing. The pen color is provided as an RGB value.
 * For grayscale or monochrome displays, the value is
 * automatically converted to the proper range.
 *
 * @param color : the desired pen color, as a 24-bit RGB value
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplayLayer::selectColorPen(int color)
{
    return this->command_push(YapiWrapper::ysprintf("c%06x",color));
}

/**
 * Selects the pen gray level for all subsequent drawing functions,
 * including text drawing. The gray level is provided as a number between
 * 0 (black) and 255 (white, or whichever the lightest color is).
 * For monochrome displays (without gray levels), any value
 * lower than 128 is rendered as black, and any value equal
 * or above to 128 is non-black.
 *
 * @param graylevel : the desired gray level, from 0 to 255
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplayLayer::selectGrayPen(int graylevel)
{
    return this->command_push(YapiWrapper::ysprintf("g%d",graylevel));
}

/**
 * Selects an eraser instead of a pen for all subsequent drawing functions,
 * except for bitmap copy functions. Any point drawn using the eraser
 * becomes transparent (as when the layer is empty), showing the other
 * layers beneath it.
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplayLayer::selectEraser(void)
{
    return this->command_push("e");
}

/**
 * Enables or disables anti-aliasing for drawing oblique lines and circles.
 * Anti-aliasing provides a smoother aspect when looked from far enough,
 * but it can add fuzziness when the display is looked from very close.
 * At the end of the day, it is your personal choice.
 * Anti-aliasing is enabled by default on grayscale and color displays,
 * but you can disable it if you prefer. This setting has no effect
 * on monochrome displays.
 *
 * @param mode : true to enable anti-aliasing, false to
 *         disable it.
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplayLayer::setAntialiasingMode(bool mode)
{
    return this->command_push(YapiWrapper::ysprintf("a%d",mode));
}

/**
 * Draws a single pixel at the specified position.
 *
 * @param x : the distance from left of layer, in pixels
 * @param y : the distance from top of layer, in pixels
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplayLayer::drawPixel(int x,int y)
{
    return this->command_flush(YapiWrapper::ysprintf("P%d,%d",x,y));
}

/**
 * Draws an empty rectangle at a specified position.
 *
 * @param x1 : the distance from left of layer to the left border of the rectangle, in pixels
 * @param y1 : the distance from top of layer to the top border of the rectangle, in pixels
 * @param x2 : the distance from left of layer to the right border of the rectangle, in pixels
 * @param y2 : the distance from top of layer to the bottom border of the rectangle, in pixels
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplayLayer::drawRect(int x1,int y1,int x2,int y2)
{
    return this->command_flush(YapiWrapper::ysprintf("R%d,%d,%d,%d",x1,y1,x2,y2));
}

/**
 * Draws a filled rectangular bar at a specified position.
 *
 * @param x1 : the distance from left of layer to the left border of the rectangle, in pixels
 * @param y1 : the distance from top of layer to the top border of the rectangle, in pixels
 * @param x2 : the distance from left of layer to the right border of the rectangle, in pixels
 * @param y2 : the distance from top of layer to the bottom border of the rectangle, in pixels
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplayLayer::drawBar(int x1,int y1,int x2,int y2)
{
    return this->command_flush(YapiWrapper::ysprintf("B%d,%d,%d,%d",x1,y1,x2,y2));
}

/**
 * Draws an empty circle at a specified position.
 *
 * @param x : the distance from left of layer to the center of the circle, in pixels
 * @param y : the distance from top of layer to the center of the circle, in pixels
 * @param r : the radius of the circle, in pixels
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplayLayer::drawCircle(int x,int y,int r)
{
    return this->command_flush(YapiWrapper::ysprintf("C%d,%d,%d",x,y,r));
}

/**
 * Draws a filled disc at a given position.
 *
 * @param x : the distance from left of layer to the center of the disc, in pixels
 * @param y : the distance from top of layer to the center of the disc, in pixels
 * @param r : the radius of the disc, in pixels
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplayLayer::drawDisc(int x,int y,int r)
{
    return this->command_flush(YapiWrapper::ysprintf("D%d,%d,%d",x,y,r));
}

/**
 * Selects a font to use for the next text drawing functions, by providing the name of the
 * font file. You can use a built-in font as well as a font file that you have previously
 * uploaded to the device built-in memory. If you experience problems selecting a font
 * file, check the device logs for any error message such as missing font file or bad font
 * file format.
 *
 * @param fontname : the font file name, embedded fonts are 8x8.yfm, Small.yfm, Medium.yfm, Large.yfm
 * (not available on Yocto-MiniDisplay).
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplayLayer::selectFont(string fontname)
{
    return this->command_push(YapiWrapper::ysprintf("&%s%c",fontname.c_str(),27));
}

/**
 * Draws a text string at the specified position. The point of the text that is aligned
 * to the specified pixel position is called the anchor point, and can be chosen among
 * several options. Text is rendered from left to right, without implicit wrapping.
 *
 * @param x : the distance from left of layer to the text anchor point, in pixels
 * @param y : the distance from top of layer to the text anchor point, in pixels
 * @param anchor : the text anchor point, chosen among the YDisplayLayer::ALIGN enumeration:
 *         YDisplayLayer::ALIGN_TOP_LEFT,         YDisplayLayer::ALIGN_CENTER_LEFT,
 *         YDisplayLayer::ALIGN_BASELINE_LEFT,    YDisplayLayer::ALIGN_BOTTOM_LEFT,
 *         YDisplayLayer::ALIGN_TOP_CENTER,       YDisplayLayer::ALIGN_CENTER,
 *         YDisplayLayer::ALIGN_BASELINE_CENTER,  YDisplayLayer::ALIGN_BOTTOM_CENTER,
 *         YDisplayLayer::ALIGN_TOP_DECIMAL,      YDisplayLayer::ALIGN_CENTER_DECIMAL,
 *         YDisplayLayer::ALIGN_BASELINE_DECIMAL, YDisplayLayer::ALIGN_BOTTOM_DECIMAL,
 *         YDisplayLayer::ALIGN_TOP_RIGHT,        YDisplayLayer::ALIGN_CENTER_RIGHT,
 *         YDisplayLayer::ALIGN_BASELINE_RIGHT,   YDisplayLayer::ALIGN_BOTTOM_RIGHT.
 * @param text : the text string to draw
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplayLayer::drawText(int x,int y,Y_ALIGN anchor,string text)
{
    return this->command_flush(YapiWrapper::ysprintf("T%d,%d,%d,%s%c",x,y,anchor,text.c_str(),27));
}

/**
 * Draws a GIF image at the specified position. The GIF image must have been previously
 * uploaded to the device built-in memory. If you experience problems using an image
 * file, check the device logs for any error message such as missing image file or bad
 * image file format.
 *
 * @param x : the distance from left of layer to the left of the image, in pixels
 * @param y : the distance from top of layer to the top of the image, in pixels
 * @param imagename : the GIF file name
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplayLayer::drawImage(int x,int y,string imagename)
{
    return this->command_flush(YapiWrapper::ysprintf("*%d,%d,%s%c",x,y,imagename.c_str(),27));
}

/**
 * Draws a bitmap at the specified position. The bitmap is provided as a binary object,
 * where each pixel maps to a bit, from left to right and from top to bottom.
 * The most significant bit of each byte maps to the leftmost pixel, and the least
 * significant bit maps to the rightmost pixel. Bits set to 1 are drawn using the
 * layer selected pen color. Bits set to 0 are drawn using the specified background
 * gray level, unless -1 is specified, in which case they are not drawn at all
 * (as if transparent).
 *
 * @param x : the distance from left of layer to the left of the bitmap, in pixels
 * @param y : the distance from top of layer to the top of the bitmap, in pixels
 * @param w : the width of the bitmap, in pixels
 * @param bitmap : a binary object
 * @param bgcol : the background gray level to use for zero bits (0 = black,
 *         255 = white), or -1 to leave the pixels unchanged
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplayLayer::drawBitmap(int x,int y,int w,string bitmap,int bgcol)
{
    string destname;
    destname = YapiWrapper::ysprintf("layer%d:%d,%d@%d,%d",_id,w,bgcol,x,y);
    return _display->upload(destname,bitmap);
}

/**
 * Moves the drawing pointer of this layer to the specified position.
 *
 * @param x : the distance from left of layer, in pixels
 * @param y : the distance from top of layer, in pixels
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplayLayer::moveTo(int x,int y)
{
    return this->command_push(YapiWrapper::ysprintf("@%d,%d",x,y));
}

/**
 * Draws a line from current drawing pointer position to the specified position.
 * The specified destination pixel is included in the line. The pointer position
 * is then moved to the end point of the line.
 *
 * @param x : the distance from left of layer to the end point of the line, in pixels
 * @param y : the distance from top of layer to the end point of the line, in pixels
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplayLayer::lineTo(int x,int y)
{
    return this->command_flush(YapiWrapper::ysprintf("-%d,%d",x,y));
}

/**
 * Outputs a message in the console area, and advances the console pointer accordingly.
 * The console pointer position is automatically moved to the beginning
 * of the next line when a newline character is met, or when the right margin
 * is hit. When the new text to display extends below the lower margin, the
 * console area is automatically scrolled up.
 *
 * @param text : the message to display
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplayLayer::consoleOut(string text)
{
    return this->command_flush(YapiWrapper::ysprintf("!%s%c",text.c_str(),27));
}

/**
 * Sets up display margins for the consoleOut function.
 *
 * @param x1 : the distance from left of layer to the left margin, in pixels
 * @param y1 : the distance from top of layer to the top margin, in pixels
 * @param x2 : the distance from left of layer to the right margin, in pixels
 * @param y2 : the distance from top of layer to the bottom margin, in pixels
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplayLayer::setConsoleMargins(int x1,int y1,int x2,int y2)
{
    return this->command_push(YapiWrapper::ysprintf("m%d,%d,%d,%d",x1,y1,x2,y2));
}

/**
 * Sets up the background color used by the clearConsole function and by
 * the console scrolling feature.
 *
 * @param bgcol : the background gray level to use when scrolling (0 = black,
 *         255 = white), or -1 for transparent
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplayLayer::setConsoleBackground(int bgcol)
{
    return this->command_push(YapiWrapper::ysprintf("b%d",bgcol));
}

/**
 * Sets up the wrapping behavior used by the consoleOut function.
 *
 * @param wordwrap : true to wrap only between words,
 *         false to wrap on the last column anyway.
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplayLayer::setConsoleWordWrap(bool wordwrap)
{
    return this->command_push(YapiWrapper::ysprintf("w%d",wordwrap));
}

/**
 * Blanks the console area within console margins, and resets the console pointer
 * to the upper left corner of the console.
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplayLayer::clearConsole(void)
{
    return this->command_flush("^");
}

/**
 * Sets the position of the layer relative to the display upper left corner.
 * When smooth scrolling is used, the display offset of the layer is
 * automatically updated during the next milliseconds to animate the move of the layer.
 *
 * @param x : the distance from left of display to the upper left corner of the layer
 * @param y : the distance from top of display to the upper left corner of the layer
 * @param scrollTime : number of milliseconds to use for smooth scrolling, or
 *         0 if the scrolling should be immediate.
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplayLayer::setLayerPosition(int x,int y,int scrollTime)
{
    return this->command_flush(YapiWrapper::ysprintf("#%d,%d,%d",x,y,scrollTime));
}

/**
 * Hides the layer. The state of the layer is preserved but the layer is not displayed
 * on the screen until the next call to unhide(). Hiding the layer can positively
 * affect the drawing speed, since it postpones the rendering until all operations are
 * completed (double-buffering).
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplayLayer::hide(void)
{
    this->command_push("h");
    _hidden = true;
    return this->flush_now();
}

/**
 * Shows the layer. Shows the layer again after a hide command.
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplayLayer::unhide(void)
{
    _hidden = false;
    return this->command_flush("s");
}

/**
 * Gets parent YDisplay. Returns the parent YDisplay object of the current YDisplayLayer::
 *
 * @return an YDisplay object
 */
YDisplay* YDisplayLayer::get_display(void)
{
    return _display;
}

/**
 * Returns the display width, in pixels.
 *
 * @return an integer corresponding to the display width, in pixels
 *
 * On failure, throws an exception or returns YDisplayLayer::DISPLAYWIDTH_INVALID.
 */
int YDisplayLayer::get_displayWidth(void)
{
    return _display->get_displayWidth();
}

/**
 * Returns the display height, in pixels.
 *
 * @return an integer corresponding to the display height, in pixels
 *
 * On failure, throws an exception or returns YDisplayLayer::DISPLAYHEIGHT_INVALID.
 */
int YDisplayLayer::get_displayHeight(void)
{
    return _display->get_displayHeight();
}

/**
 * Returns the width of the layers to draw on, in pixels.
 *
 * @return an integer corresponding to the width of the layers to draw on, in pixels
 *
 * On failure, throws an exception or returns YDisplayLayer::LAYERWIDTH_INVALID.
 */
int YDisplayLayer::get_layerWidth(void)
{
    return _display->get_layerWidth();
}

/**
 * Returns the height of the layers to draw on, in pixels.
 *
 * @return an integer corresponding to the height of the layers to draw on, in pixels
 *
 * On failure, throws an exception or returns YDisplayLayer::LAYERHEIGHT_INVALID.
 */
int YDisplayLayer::get_layerHeight(void)
{
    return _display->get_layerHeight();
}

int YDisplayLayer::resetHiddenFlag(void)
{
    _hidden = false;
    return YAPI_SUCCESS;
}
//--- (end of generated code: YDisplayLayer implementation)


YDisplay::YDisplay(const string& func): YFunction(func)
//--- (generated code: YDisplay initialization)
    ,_enabled(ENABLED_INVALID)
    ,_startupSeq(STARTUPSEQ_INVALID)
    ,_brightness(BRIGHTNESS_INVALID)
    ,_autoInvertDelay(AUTOINVERTDELAY_INVALID)
    ,_orientation(ORIENTATION_INVALID)
    ,_displayPanel(DISPLAYPANEL_INVALID)
    ,_displayWidth(DISPLAYWIDTH_INVALID)
    ,_displayHeight(DISPLAYHEIGHT_INVALID)
    ,_displayType(DISPLAYTYPE_INVALID)
    ,_layerWidth(LAYERWIDTH_INVALID)
    ,_layerHeight(LAYERHEIGHT_INVALID)
    ,_layerCount(LAYERCOUNT_INVALID)
    ,_command(COMMAND_INVALID)
    ,_valueCallbackDisplay(NULL)
//--- (end of generated code: YDisplay initialization)
            ,_allDisplayLayers(0)
            ,_recording(false)
            ,_sequence("")
{
    _className ="Display";
}

YDisplay::~YDisplay()
{
    unsigned int i;
    for (i=0;i<_allDisplayLayers.size();i++){
        delete _allDisplayLayers[i];
    }
    _allDisplayLayers.clear();
    //--- (generated code: YDisplay cleanup)
//--- (end of generated code: YDisplay cleanup)
}


//--- (generated code: YDisplay implementation)
// static attributes
const string YDisplay::STARTUPSEQ_INVALID = YAPI_INVALID_STRING;
const string YDisplay::DISPLAYPANEL_INVALID = YAPI_INVALID_STRING;
const string YDisplay::COMMAND_INVALID = YAPI_INVALID_STRING;

int YDisplay::_parseAttr(YJSONObject *json_val)
{
    if(json_val->has("enabled")) {
        _enabled =  (Y_ENABLED_enum)json_val->getInt("enabled");
    }
    if(json_val->has("startupSeq")) {
        _startupSeq =  json_val->getString("startupSeq");
    }
    if(json_val->has("brightness")) {
        _brightness =  json_val->getInt("brightness");
    }
    if(json_val->has("autoInvertDelay")) {
        _autoInvertDelay =  json_val->getInt("autoInvertDelay");
    }
    if(json_val->has("orientation")) {
        _orientation =  (Y_ORIENTATION_enum)json_val->getInt("orientation");
    }
    if(json_val->has("displayPanel")) {
        _displayPanel =  json_val->getString("displayPanel");
    }
    if(json_val->has("displayWidth")) {
        _displayWidth =  json_val->getInt("displayWidth");
    }
    if(json_val->has("displayHeight")) {
        _displayHeight =  json_val->getInt("displayHeight");
    }
    if(json_val->has("displayType")) {
        _displayType =  (Y_DISPLAYTYPE_enum)json_val->getInt("displayType");
    }
    if(json_val->has("layerWidth")) {
        _layerWidth =  json_val->getInt("layerWidth");
    }
    if(json_val->has("layerHeight")) {
        _layerHeight =  json_val->getInt("layerHeight");
    }
    if(json_val->has("layerCount")) {
        _layerCount =  json_val->getInt("layerCount");
    }
    if(json_val->has("command")) {
        _command =  json_val->getString("command");
    }
    return YFunction::_parseAttr(json_val);
}


/**
 * Returns true if the screen is powered, false otherwise.
 *
 * @return either YDisplay::ENABLED_FALSE or YDisplay::ENABLED_TRUE, according to true if the screen is
 * powered, false otherwise
 *
 * On failure, throws an exception or returns YDisplay::ENABLED_INVALID.
 */
Y_ENABLED_enum YDisplay::get_enabled(void)
{
    Y_ENABLED_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YDisplay::ENABLED_INVALID;
                }
            }
        }
        res = _enabled;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the power state of the display.
 *
 * @param newval : either YDisplay::ENABLED_FALSE or YDisplay::ENABLED_TRUE, according to the power
 * state of the display
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplay::set_enabled(Y_ENABLED_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = (newval>0 ? "1" : "0");
        res = _setAttr("enabled", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the name of the sequence to play when the displayed is powered on.
 *
 * @return a string corresponding to the name of the sequence to play when the displayed is powered on
 *
 * On failure, throws an exception or returns YDisplay::STARTUPSEQ_INVALID.
 */
string YDisplay::get_startupSeq(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YDisplay::STARTUPSEQ_INVALID;
                }
            }
        }
        res = _startupSeq;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the name of the sequence to play when the displayed is powered on.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : a string corresponding to the name of the sequence to play when the displayed is powered on
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplay::set_startupSeq(const string& newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = newval;
        res = _setAttr("startupSeq", rest_val);
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
 * On failure, throws an exception or returns YDisplay::BRIGHTNESS_INVALID.
 */
int YDisplay::get_brightness(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YDisplay::BRIGHTNESS_INVALID;
                }
            }
        }
        res = _brightness;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the brightness of the display. The parameter is a value between 0 and
 * 100. Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : an integer corresponding to the brightness of the display
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplay::set_brightness(int newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("brightness", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the interval between automatic display inversions, or 0 if automatic
 * inversion is disabled. Using the automatic inversion mechanism reduces the
 * burn-in that occurs on OLED screens over long periods when the same content
 * remains displayed on the screen.
 *
 * @return an integer corresponding to the interval between automatic display inversions, or 0 if automatic
 *         inversion is disabled
 *
 * On failure, throws an exception or returns YDisplay::AUTOINVERTDELAY_INVALID.
 */
int YDisplay::get_autoInvertDelay(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YDisplay::AUTOINVERTDELAY_INVALID;
                }
            }
        }
        res = _autoInvertDelay;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the interval between automatic display inversions.
 * The parameter is the number of seconds, or 0 to disable automatic inversion.
 * Using the automatic inversion mechanism reduces the burn-in that occurs on OLED
 * screens over long periods when the same content remains displayed on the screen.
 * Remember to call the saveToFlash() method of the module if the
 * modification must be kept.
 *
 * @param newval : an integer corresponding to the interval between automatic display inversions
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplay::set_autoInvertDelay(int newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("autoInvertDelay", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the currently selected display orientation.
 *
 * @return a value among YDisplay::ORIENTATION_LEFT, YDisplay::ORIENTATION_UP,
 * YDisplay::ORIENTATION_RIGHT and YDisplay::ORIENTATION_DOWN corresponding to the currently selected
 * display orientation
 *
 * On failure, throws an exception or returns YDisplay::ORIENTATION_INVALID.
 */
Y_ORIENTATION_enum YDisplay::get_orientation(void)
{
    Y_ORIENTATION_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YDisplay::ORIENTATION_INVALID;
                }
            }
        }
        res = _orientation;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the display orientation. Remember to call the saveToFlash()
 * method of the module if the modification must be kept.
 *
 * @param newval : a value among YDisplay::ORIENTATION_LEFT, YDisplay::ORIENTATION_UP,
 * YDisplay::ORIENTATION_RIGHT and YDisplay::ORIENTATION_DOWN corresponding to the display orientation
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplay::set_orientation(Y_ORIENTATION_enum newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        char buf[32]; SAFE_SPRINTF(buf, 32, "%d", newval); rest_val = string(buf);
        res = _setAttr("orientation", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the exact model of the display panel.
 *
 * @return a string corresponding to the exact model of the display panel
 *
 * On failure, throws an exception or returns YDisplay::DISPLAYPANEL_INVALID.
 */
string YDisplay::get_displayPanel(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YDisplay::DISPLAYPANEL_INVALID;
                }
            }
        }
        res = _displayPanel;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Changes the model of display to match the connected display panel.
 * This function has no effect if the module does not support the selected
 * display panel.
 * Remember to call the saveToFlash()
 * method of the module if the modification must be kept.
 *
 * @param newval : a string corresponding to the model of display to match the connected display panel
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplay::set_displayPanel(const string& newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = newval;
        res = _setAttr("displayPanel", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the display width, in pixels.
 *
 * @return an integer corresponding to the display width, in pixels
 *
 * On failure, throws an exception or returns YDisplay::DISPLAYWIDTH_INVALID.
 */
int YDisplay::get_displayWidth(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration == 0) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YDisplay::DISPLAYWIDTH_INVALID;
                }
            }
        }
        res = _displayWidth;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the display height, in pixels.
 *
 * @return an integer corresponding to the display height, in pixels
 *
 * On failure, throws an exception or returns YDisplay::DISPLAYHEIGHT_INVALID.
 */
int YDisplay::get_displayHeight(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration == 0) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YDisplay::DISPLAYHEIGHT_INVALID;
                }
            }
        }
        res = _displayHeight;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the display type: monochrome, gray levels or full color.
 *
 * @return a value among YDisplay::DISPLAYTYPE_MONO, YDisplay::DISPLAYTYPE_GRAY,
 * YDisplay::DISPLAYTYPE_RGB and YDisplay::DISPLAYTYPE_EPAPER corresponding to the display type:
 * monochrome, gray levels or full color
 *
 * On failure, throws an exception or returns YDisplay::DISPLAYTYPE_INVALID.
 */
Y_DISPLAYTYPE_enum YDisplay::get_displayType(void)
{
    Y_DISPLAYTYPE_enum res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration == 0) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YDisplay::DISPLAYTYPE_INVALID;
                }
            }
        }
        res = _displayType;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the width of the layers to draw on, in pixels.
 *
 * @return an integer corresponding to the width of the layers to draw on, in pixels
 *
 * On failure, throws an exception or returns YDisplay::LAYERWIDTH_INVALID.
 */
int YDisplay::get_layerWidth(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration == 0) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YDisplay::LAYERWIDTH_INVALID;
                }
            }
        }
        res = _layerWidth;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the height of the layers to draw on, in pixels.
 *
 * @return an integer corresponding to the height of the layers to draw on, in pixels
 *
 * On failure, throws an exception or returns YDisplay::LAYERHEIGHT_INVALID.
 */
int YDisplay::get_layerHeight(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration == 0) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YDisplay::LAYERHEIGHT_INVALID;
                }
            }
        }
        res = _layerHeight;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Returns the number of available layers to draw on.
 *
 * @return an integer corresponding to the number of available layers to draw on
 *
 * On failure, throws an exception or returns YDisplay::LAYERCOUNT_INVALID.
 */
int YDisplay::get_layerCount(void)
{
    int res = 0;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration == 0) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YDisplay::LAYERCOUNT_INVALID;
                }
            }
        }
        res = _layerCount;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

string YDisplay::get_command(void)
{
    string res;
    yEnterCriticalSection(&_this_cs);
    try {
        if (_cacheExpiration <= YAPI::GetTickCount()) {
            if (this->_load_unsafe(YAPI::_yapiContext.GetCacheValidity()) != YAPI_SUCCESS) {
                {
                    yLeaveCriticalSection(&_this_cs);
                    return YDisplay::COMMAND_INVALID;
                }
            }
        }
        res = _command;
    } catch (std::exception &) {
        yLeaveCriticalSection(&_this_cs);
        throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

int YDisplay::set_command(const string& newval)
{
    string rest_val;
    int res;
    yEnterCriticalSection(&_this_cs);
    try {
        rest_val = newval;
        res = _setAttr("command", rest_val);
    } catch (std::exception &) {
         yLeaveCriticalSection(&_this_cs);
         throw;
    }
    yLeaveCriticalSection(&_this_cs);
    return res;
}

/**
 * Retrieves a display for a given identifier.
 * The identifier can be specified using several formats:
 *
 * - FunctionLogicalName
 * - ModuleSerialNumber.FunctionIdentifier
 * - ModuleSerialNumber.FunctionLogicalName
 * - ModuleLogicalName.FunctionIdentifier
 * - ModuleLogicalName.FunctionLogicalName
 *
 *
 * This function does not require that the display is online at the time
 * it is invoked. The returned object is nevertheless valid.
 * Use the method isOnline() to test if the display is
 * indeed online at a given time. In case of ambiguity when looking for
 * a display by logical name, no error is notified: the first instance
 * found is returned. The search is performed first by hardware name,
 * then by logical name.
 *
 * If a call to this object's is_online() method returns FALSE although
 * you are certain that the matching device is plugged, make sure that you did
 * call registerHub() at application initialization time.
 *
 * @param func : a string that uniquely characterizes the display, for instance
 *         YD128X32.display.
 *
 * @return a YDisplay object allowing you to drive the display.
 */
YDisplay* YDisplay::FindDisplay(string func)
{
    YDisplay* obj = NULL;
    int taken = 0;
    if (YAPI::_apiInitialized) {
        yEnterCriticalSection(&YAPI::_global_cs);
        taken = 1;
    }try {
        obj = (YDisplay*) YFunction::_FindFromCache("Display", func);
        if (obj == NULL) {
            obj = new YDisplay(func);
            YFunction::_AddToCache("Display", func, obj);
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
int YDisplay::registerValueCallback(YDisplayValueCallback callback)
{
    string val;
    if (callback != NULL) {
        YFunction::_UpdateValueCallbackList(this, true);
    } else {
        YFunction::_UpdateValueCallbackList(this, false);
    }
    _valueCallbackDisplay = callback;
    // Immediately invoke value callback with current value
    if (callback != NULL && this->isOnline()) {
        val = _advertisedValue;
        if (!(val == "")) {
            this->_invokeValueCallback(val);
        }
    }
    return 0;
}

int YDisplay::_invokeValueCallback(string value)
{
    if (_valueCallbackDisplay != NULL) {
        _valueCallbackDisplay(this, value);
    } else {
        YFunction::_invokeValueCallback(value);
    }
    return 0;
}

/**
 * Clears the display screen and resets all display layers to their default state.
 * Using this function in a sequence will kill the sequence play-back. Don't use that
 * function to reset the display at sequence start-up.
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplay::resetAll(void)
{
    this->flushLayers();
    this->resetHiddenLayerFlags();
    return this->sendCommand("Z");
}

/**
 * Forces an ePaper screen to perform a regenerative update using the slow
 * update method. Periodic use of the slow method (total panel update with
 * multiple inversions) prevents ghosting effects and improves contrast.
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplay::regenerateDisplay(void)
{
    return this->sendCommand("z");
}

/**
 * Disables screen refresh for a short period of time. The combination of
 * postponeRefresh and triggerRefresh can be used as an
 * alternative to double-buffering to avoid flickering during display updates.
 *
 * @param duration : duration of deactivation in milliseconds (max. 30 seconds)
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplay::postponeRefresh(int duration)
{
    return this->sendCommand(YapiWrapper::ysprintf("t%d",duration));
}

/**
 * Trigger an immediate screen refresh. The combination of
 * postponeRefresh and triggerRefresh can be used as an
 * alternative to double-buffering to avoid flickering during display updates.
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplay::triggerRefresh(void)
{
    return this->sendCommand("t0");
}

/**
 * Smoothly changes the brightness of the screen to produce a fade-in or fade-out
 * effect.
 *
 * @param brightness : the new screen brightness
 * @param duration : duration of the brightness transition, in milliseconds.
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplay::fade(int brightness,int duration)
{
    this->flushLayers();
    return this->sendCommand(YapiWrapper::ysprintf("+%d,%d",brightness,duration));
}

/**
 * Starts to record all display commands into a sequence, for later replay.
 * The name used to store the sequence is specified when calling
 * saveSequence(), once the recording is complete.
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplay::newSequence(void)
{
    this->flushLayers();
    _sequence = "";
    _recording = true;
    return YAPI_SUCCESS;
}

/**
 * Stops recording display commands and saves the sequence into the specified
 * file on the display internal memory. The sequence can be later replayed
 * using playSequence().
 *
 * @param sequenceName : the name of the newly created sequence
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplay::saveSequence(string sequenceName)
{
    this->flushLayers();
    _recording = false;
    this->_upload(sequenceName, _sequence);
    //We need to use YPRINTF("") for Objective-C
    _sequence = YapiWrapper::ysprintf("");
    return YAPI_SUCCESS;
}

/**
 * Replays a display sequence previously recorded using
 * newSequence() and saveSequence().
 *
 * @param sequenceName : the name of the newly created sequence
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplay::playSequence(string sequenceName)
{
    this->flushLayers();
    return this->sendCommand(YapiWrapper::ysprintf("S%s",sequenceName.c_str()));
}

/**
 * Waits for a specified delay (in milliseconds) before playing next
 * commands in current sequence. This method can be used while
 * recording a display sequence, to insert a timed wait in the sequence
 * (without any immediate effect). It can also be used dynamically while
 * playing a pre-recorded sequence, to suspend or resume the execution of
 * the sequence. To cancel a delay, call the same method with a zero delay.
 *
 * @param delay_ms : the duration to wait, in milliseconds
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplay::pauseSequence(int delay_ms)
{
    this->flushLayers();
    return this->sendCommand(YapiWrapper::ysprintf("W%d",delay_ms));
}

/**
 * Stops immediately any ongoing sequence replay.
 * The display is left as is.
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplay::stopSequence(void)
{
    this->flushLayers();
    return this->sendCommand("S");
}

/**
 * Uploads an arbitrary file (for instance a GIF file) to the display, to the
 * specified full path name. If a file already exists with the same path name,
 * its content is overwritten.
 *
 * @param pathname : path and name of the new file to create
 * @param content : binary buffer with the content to set
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplay::upload(string pathname,string content)
{
    return this->_upload(pathname, content);
}

/**
 * Copies the whole content of a layer to another layer. The color and transparency
 * of all the pixels from the destination layer are set to match the source pixels.
 * This method only affects the displayed content, but does not change any
 * property of the layer object.
 * Note that layer 0 has no transparency support (it is always completely opaque).
 *
 * @param srcLayerId : the identifier of the source layer (a number in range 0..layerCount-1)
 * @param dstLayerId : the identifier of the destination layer (a number in range 0..layerCount-1)
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplay::copyLayerContent(int srcLayerId,int dstLayerId)
{
    this->flushLayers();
    return this->sendCommand(YapiWrapper::ysprintf("o%d,%d",srcLayerId,dstLayerId));
}

/**
 * Swaps the whole content of two layers. The color and transparency of all the pixels from
 * the two layers are swapped. This method only affects the displayed content, but does
 * not change any property of the layer objects. In particular, the visibility of each
 * layer stays unchanged. When used between one hidden layer and a visible layer,
 * this method makes it possible to easily implement double-buffering.
 * Note that layer 0 has no transparency support (it is always completely opaque).
 *
 * @param layerIdA : the first layer (a number in range 0..layerCount-1)
 * @param layerIdB : the second layer (a number in range 0..layerCount-1)
 *
 * @return YAPI::SUCCESS if the call succeeds.
 *
 * On failure, throws an exception or returns a negative error code.
 */
int YDisplay::swapLayerContent(int layerIdA,int layerIdB)
{
    this->flushLayers();
    return this->sendCommand(YapiWrapper::ysprintf("E%d,%d",layerIdA,layerIdB));
}

/**
 * Returns a YDisplayLayer object that can be used to draw on the specified
 * layer. The content is displayed only when the layer is active on the
 * screen (and not masked by other overlapping layers).
 *
 * @param layerId : the identifier of the layer (a number in range 0..layerCount-1)
 *
 * @return an YDisplayLayer object
 *
 * On failure, throws an exception or returns NULL.
 */
YDisplayLayer* YDisplay::get_displayLayer(int layerId)
{
    int layercount = 0;
    int idx = 0;
    layercount = this->get_layerCount();
    if (!((layerId >= 0) && (layerId < layercount))) {
        _throw((YRETCODE)(YAPI_INVALID_ARGUMENT), "invalid DisplayLayer index");
        return NULL;
    }
    if ((int)_allDisplayLayers.size() == 0) {
        idx = 0;
        while (idx < layercount) {
            _allDisplayLayers.push_back(new YDisplayLayer(this,idx));
            idx = idx + 1;
        }
    }
    return _allDisplayLayers[layerId];
}

/**
 * Returns a color image with the current content of the display.
 * The image is returned as a binary object, where each byte represents a pixel,
 * from left to right and from top to bottom. The palette used to map byte
 * values to RGB colors is filled into the list provided as argument.
 * In all cases, the first palette entry (value 0) corresponds to the
 * screen default background color.
 * The image dimensions are given by the display width and height.
 *
 * @param palette : a list to be filled with the image palette
 *
 * @return a binary object if the call succeeds.
 *
 * On failure, throws an exception or returns an empty binary object.
 */
string YDisplay::readDisplay(vector<int> palette)
{
    string zipmap;
    int zipsize = 0;
    int zipwidth = 0;
    int zipheight = 0;
    int ziprotate = 0;
    int zipcolors = 0;
    int zipcol = 0;
    int zipbits = 0;
    int zipmask = 0;
    int srcpos = 0;
    int endrun = 0;
    int srcpat = 0;
    int srcbit = 0;
    int srcval = 0;
    int srcx = 0;
    int srcy = 0;
    int srci = 0;
    int incx = 0;
    string pixmap;
    int pixcount = 0;
    int pixval = 0;
    int pixpos = 0;
    string rotmap;
    pixmap = string(0, (char)0);
    // Check if the display firmware has autoInvertDelay and pixels.bin support

    if (this->get_autoInvertDelay() < 0) {
        // Old firmware, use uncompressed GIF output to rebuild pixmap
        zipmap = this->_download("display.gif");
        zipsize = (int)(zipmap).size();
        if (zipsize == 0) {
            return pixmap;
        }
        if (!(zipsize >= 32)) {
            _throw((YRETCODE)(YAPI_IO_ERROR), "not a GIF image");
            return pixmap;
        }
        if (!((((u8)zipmap[0]) == 71) && (((u8)zipmap[2]) == 70))) {
            _throw((YRETCODE)(YAPI_INVALID_ARGUMENT), "not a GIF image");
            return pixmap;
        }
        zipwidth = ((u8)zipmap[6]) + 256 * ((u8)zipmap[7]);
        zipheight = ((u8)zipmap[8]) + 256 * ((u8)zipmap[9]);
        palette.clear();
        zipcol = ((u8)zipmap[13]) * 65536 + ((u8)zipmap[14]) * 256 + ((u8)zipmap[15]);
        palette.push_back(zipcol);
        zipcol = ((u8)zipmap[16]) * 65536 + ((u8)zipmap[17]) * 256 + ((u8)zipmap[18]);
        palette.push_back(zipcol);
        pixcount = zipwidth * zipheight;
        pixmap = string(pixcount, (char)0);
        pixpos = 0;
        srcpos = 30;
        zipsize = zipsize - 2;
        while (srcpos < zipsize) {
            // load next run size
            endrun = srcpos + 1 + ((u8)zipmap[srcpos]);
            srcpos = srcpos + 1;
            while (srcpos < endrun) {
                srcval = ((u8)zipmap[srcpos]);
                srcpos = srcpos + 1;
                srcbit = 8;
                while (srcbit != 0) {
                    if (srcbit < 3) {
                        srcval = srcval + (((u8)zipmap[srcpos]) << srcbit);
                        srcpos = srcpos + 1;
                    }
                    pixval = (srcval & 7);
                    srcval = (srcval >> 3);
                    if (!((pixval > 1) && (pixval != 4))) {
                        _throw((YRETCODE)(YAPI_INVALID_ARGUMENT), "unexpected encoding");
                        return pixmap;
                    }
                    pixmap[pixpos] = (char)(pixval);
                    pixpos = pixpos + 1;
                    srcbit = srcbit - 3;
                }
            }
        }
        return pixmap;
    }
    // New firmware, use compressed pixels.bin
    zipmap = this->_download("pixels.bin");
    zipsize = (int)(zipmap).size();
    if (zipsize == 0) {
        return pixmap;
    }
    if (!(zipsize >= 16)) {
        _throw((YRETCODE)(YAPI_IO_ERROR), "not a pixmap");
        return pixmap;
    }
    if (!((((u8)zipmap[0]) == 80) && (((u8)zipmap[2]) == 88))) {
        _throw((YRETCODE)(YAPI_INVALID_ARGUMENT), "not a pixmap");
        return pixmap;
    }
    zipwidth = ((u8)zipmap[4]) + 256 * ((u8)zipmap[5]);
    zipheight = ((u8)zipmap[6]) + 256 * ((u8)zipmap[7]);
    ziprotate = ((u8)zipmap[8]);
    zipcolors = ((u8)zipmap[9]);
    palette.clear();
    srcpos = 10;
    srci = 0;
    while (srci < zipcolors) {
        zipcol = ((u8)zipmap[srcpos]) * 65536 + ((u8)zipmap[srcpos+1]) * 256 + ((u8)zipmap[srcpos+2]);
        palette.push_back(zipcol);
        srcpos = srcpos + 3;
        srci = srci + 1;
    }
    zipbits = 1;
    while ((1 << zipbits) < zipcolors) {
        zipbits = zipbits + 1;
    }
    zipmask = (1 << zipbits) - 1;
    pixcount = zipwidth * zipheight;
    pixmap = string(pixcount, (char)0);
    srcx = 0;
    srcy = 0;
    incx = (8 / zipbits);
    srcval = 0;
    while (srcpos < zipsize) {
        // load next compression pattern byte
        srcpat = ((u8)zipmap[srcpos]);
        srcpos = srcpos + 1;
        srcbit = 7;
        while (srcbit >= 0) {
            // get next bitmap byte
            if ((srcpat & 128) != 0) {
                srcval = ((u8)zipmap[srcpos]);
                srcpos = srcpos + 1;
            }
            srcpat = (srcpat << 1);
            pixpos = srcy * zipwidth + srcx;
            // produce 8 pixels (or 4, if bitmap uses 2 bits per pixel)
            srci = 8 - zipbits;
            while (srci >= 0) {
                pixval = ((srcval >> srci) & zipmask);
                pixmap[pixpos] = (char)(pixval);
                pixpos = pixpos + 1;
                srci = srci - zipbits;
            }
            srcy = srcy + 1;
            if (srcy >= zipheight) {
                srcy = 0;
                srcx = srcx + incx;
                // drop last bytes if image is not a multiple of 8
                if (srcx >= zipwidth) {
                    srcbit = 0;
                }
            }
            srcbit = srcbit - 1;
        }
    }
    // rotate pixmap to match display orientation
    if (ziprotate == 0) {
        return pixmap;
    }
    if ((ziprotate & 2) != 0) {
        // rotate buffer 180 degrees by swapping pixels
        srcpos = 0;
        pixpos = pixcount - 1;
        while (srcpos < pixpos) {
            pixval = ((u8)pixmap[srcpos]);
            pixmap[srcpos] = (char)(((u8)pixmap[pixpos]));
            pixmap[pixpos] = (char)(pixval);
            srcpos = srcpos + 1;
            pixpos = pixpos - 1;
        }
    }
    if ((ziprotate & 1) == 0) {
        return pixmap;
    }
    // rotate 90 ccw: first pixel is bottom left
    rotmap = string(pixcount, (char)0);
    srcx = 0;
    srcy = zipwidth - 1;
    srcpos = 0;
    while (srcpos < pixcount) {
        pixval = ((u8)pixmap[srcpos]);
        pixpos = srcy * zipheight + srcx;
        rotmap[pixpos] = (char)(pixval);
        srcy = srcy - 1;
        if (srcy < 0) {
            srcx = srcx + 1;
            srcy = zipwidth - 1;
        }
        srcpos = srcpos + 1;
    }
    return rotmap;
}

YDisplay *YDisplay::nextDisplay(void)
{
    string  hwid;

    if(YISERR(_nextFunction(hwid)) || hwid=="") {
        return NULL;
    }
    return YDisplay::FindDisplay(hwid);
}

YDisplay *YDisplay::FirstDisplay(void)
{
    vector<YFUN_DESCR>   v_fundescr;
    YDEV_DESCR             ydevice;
    string              serial, funcId, funcName, funcVal, errmsg;

    if(YISERR(YapiWrapper::getFunctionsByClass("Display", 0, v_fundescr, sizeof(YFUN_DESCR), errmsg)) ||
       v_fundescr.size() == 0 ||
       YISERR(YapiWrapper::getFunctionInfo(v_fundescr[0], ydevice, serial, funcId, funcName, funcVal, errmsg))) {
        return NULL;
    }
    return YDisplay::FindDisplay(serial+"."+funcId);
}

//--- (end of generated code: YDisplay implementation)

int YDisplay::flushLayers(void)
{
    for(unsigned i = 0; i < _allDisplayLayers.size(); i++) {
        _allDisplayLayers[i]->flush_now();
    }
    return YAPI_SUCCESS;
}

// internal function to clear hidden flag during resetAll
void YDisplay::resetHiddenLayerFlags(void)
{
    for(unsigned i = 0; i < _allDisplayLayers.size(); i++) {
        _allDisplayLayers[i]->resetHiddenFlag();
    }
}

int YDisplay::sendCommand(string cmd)
{
    if(!_recording) {
        return this->set_command(cmd);
    }
    _sequence += cmd+"\n";
    return YAPI_SUCCESS;
}


//--- (generated code: YDisplay functions)
//--- (end of generated code: YDisplay functions)
