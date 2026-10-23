/*********************************************************************
 *
 * $Id:yocto_display.h version 2.1.16635 (build 76635) $
 *
 * Declares yFindDisplay(), the high-level API for Display functions
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


#ifndef YOCTO_DISPLAY_H
#define YOCTO_DISPLAY_H

#include <cfloat>
#include <cmath>

#include "yocto_api.h"

#ifdef YOCTOLIB_NAMESPACE
namespace YOCTOLIB_NAMESPACE
{
#endif
//--- (generated code: YDisplay definitions)
class YDisplay; // forward declaration

typedef void (*YDisplayValueCallback)(YDisplay *func, const string& functionValue);
#ifndef _Y_ENABLED_ENUM
#define _Y_ENABLED_ENUM
typedef enum {
    Y_ENABLED_FALSE = 0,
    Y_ENABLED_TRUE = 1,
    Y_ENABLED_INVALID = -1,
} Y_ENABLED_enum;
#endif
#ifndef _Y_ORIENTATION_ENUM
#define _Y_ORIENTATION_ENUM
typedef enum {
    Y_ORIENTATION_LEFT = 0,
    Y_ORIENTATION_UP = 1,
    Y_ORIENTATION_RIGHT = 2,
    Y_ORIENTATION_DOWN = 3,
    Y_ORIENTATION_INVALID = -1,
} Y_ORIENTATION_enum;
#endif
#ifndef _Y_DISPLAYTYPE_ENUM
#define _Y_DISPLAYTYPE_ENUM
typedef enum {
    Y_DISPLAYTYPE_MONO = 0,
    Y_DISPLAYTYPE_EPAPER_BW = 1,
    Y_DISPLAYTYPE_EPAPER_BWR = 2,
    Y_DISPLAYTYPE_EPAPER_BWRY = 3,
    Y_DISPLAYTYPE_INVALID = -1,
} Y_DISPLAYTYPE_enum;
#endif
    #ifndef _Y_FASTREFRESH
    #define _Y_FASTREFRESH
    typedef enum {
        Y_FASTREFRESH_WHENEVER_POSSIBLE = 0 ,
        Y_FASTREFRESH_WHENEVER_SUPPORTED = 1 ,
        Y_FASTREFRESH_NEVER = 2 ,
        Y_FASTREFRESH_INVALID = 3
    } Y_FASTREFRESH;
    #endif

    #ifndef _Y_REGENERATE
    #define _Y_REGENERATE
    typedef enum {
        Y_REGENERATE_ON_REQUEST_ONLY = 0 ,
        Y_REGENERATE_EVERY_DAY = 1 ,
        Y_REGENERATE_EVERY_12H = 2 ,
        Y_REGENERATE_EVERY_6H = 3 ,
        Y_REGENERATE_EVERY_3H = 4 ,
        Y_REGENERATE_EVERY_2H = 5 ,
        Y_REGENERATE_EVERY_HOUR = 6 ,
        Y_REGENERATE_EVERY_30MIN = 7 ,
        Y_REGENERATE_EVERY_15MIN = 8 ,
        Y_REGENERATE_EVERY_480 = 9 ,
        Y_REGENERATE_EVERY_432 = 10 ,
        Y_REGENERATE_EVERY_360 = 11 ,
        Y_REGENERATE_EVERY_288 = 12 ,
        Y_REGENERATE_EVERY_240 = 13 ,
        Y_REGENERATE_EVERY_192 = 14 ,
        Y_REGENERATE_EVERY_144 = 15 ,
        Y_REGENERATE_EVERY_96 = 16 ,
        Y_REGENERATE_EVERY_48 = 17 ,
        Y_REGENERATE_EVERY_36 = 18 ,
        Y_REGENERATE_EVERY_24 = 19 ,
        Y_REGENERATE_EVERY_12 = 20 ,
        Y_REGENERATE_EVERY_10 = 21 ,
        Y_REGENERATE_EVERY_8 = 22 ,
        Y_REGENERATE_EVERY_6 = 23 ,
        Y_REGENERATE_EVERY_4 = 24 ,
        Y_REGENERATE_ALWAYS = 25 ,
        Y_REGENERATE_INVALID = 26
    } Y_REGENERATE;
    #endif

    #ifndef _Y_DISPLAYSTATE
    #define _Y_DISPLAYSTATE
    typedef enum {
        Y_DISPLAYSTATE_FAILURE = 0 ,
        Y_DISPLAYSTATE_OFF = 1 ,
        Y_DISPLAYSTATE_POWERING = 2 ,
        Y_DISPLAYSTATE_IDLE = 3 ,
        Y_DISPLAYSTATE_REFRESHING = 4 ,
        Y_DISPLAYSTATE_INVALID = 5
    } Y_DISPLAYSTATE;
    #endif

#define Y_STARTUPSEQ_INVALID            (YAPI_INVALID_STRING)
#define Y_BRIGHTNESS_INVALID            (YAPI_INVALID_UINT)
#define Y_AUTOINVERTDELAY_INVALID       (YAPI_INVALID_UINT)
#define Y_DISPLAYPANEL_INVALID          (YAPI_INVALID_STRING)
#define Y_DISPLAYWIDTH_INVALID          (YAPI_INVALID_UINT)
#define Y_DISPLAYHEIGHT_INVALID         (YAPI_INVALID_UINT)
#define Y_LAYERWIDTH_INVALID            (YAPI_INVALID_UINT)
#define Y_LAYERHEIGHT_INVALID           (YAPI_INVALID_UINT)
#define Y_LAYERCOUNT_INVALID            (YAPI_INVALID_UINT)
#define Y_COMMAND_INVALID               (YAPI_INVALID_STRING)
//--- (end of generated code: YDisplay definitions)

//--- (generated code: YDisplayLayer definitions)
    #ifndef _Y_ALIGN
    #define _Y_ALIGN
    typedef enum {
        Y_ALIGN_TOP_LEFT = 0 ,
        Y_ALIGN_CENTER_LEFT = 1 ,
        Y_ALIGN_BASELINE_LEFT = 2 ,
        Y_ALIGN_BOTTOM_LEFT = 3 ,
        Y_ALIGN_TOP_CENTER = 4 ,
        Y_ALIGN_CENTER = 5 ,
        Y_ALIGN_BASELINE_CENTER = 6 ,
        Y_ALIGN_BOTTOM_CENTER = 7 ,
        Y_ALIGN_TOP_DECIMAL = 8 ,
        Y_ALIGN_CENTER_DECIMAL = 9 ,
        Y_ALIGN_BASELINE_DECIMAL = 10 ,
        Y_ALIGN_BOTTOM_DECIMAL = 11 ,
        Y_ALIGN_TOP_RIGHT = 12 ,
        Y_ALIGN_CENTER_RIGHT = 13 ,
        Y_ALIGN_BASELINE_RIGHT = 14 ,
        Y_ALIGN_BOTTOM_RIGHT = 15
    } Y_ALIGN;
    #endif

    #ifndef _Y_QRMODE
    #define _Y_QRMODE
    typedef enum {
        Y_QRMODE_UPCASE_ECCL = 0 ,
        Y_QRMODE_UPCASE_ECCM = 1 ,
        Y_QRMODE_UPCASE_ECCQ = 2 ,
        Y_QRMODE_UPCASE_ECCH = 3 ,
        Y_QRMODE_BYTES_ECCL = 4 ,
        Y_QRMODE_BYTES_ECCM = 5 ,
        Y_QRMODE_BYTES_ECCQ = 6 ,
        Y_QRMODE_BYTES_ECCH = 7
    } Y_QRMODE;
    #endif

//--- (end of generated code: YDisplayLayer definitions)

class YDisplay;

//--- (generated code: YDisplayLayer declaration)
/**
 * YDisplayLayer Class: Interface for drawing into display layers, obtained by calling display.get_displayLayer.
 *
 * Each DisplayLayer represents an image layer containing objects
 * to display (bitmaps, text, etc.). The content is displayed only when
 * the layer is active on the screen (and not masked by other
 * overlapping layers).
 */
class YOCTO_CLASS_EXPORT YDisplayLayer {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of generated code: YDisplayLayer declaration)
    //--- (generated code: YDisplayLayer attributes)
    // Attributes (function value cache)
    string          _cmdbuff;
    bool            _hidden;
    int             _polyPrevX;
    int             _polyPrevY;
    //--- (end of generated code: YDisplayLayer attributes)
    //--- (generated code: YDisplayLayer constructor)

    //--- (end of generated code: YDisplayLayer constructor)
    //--- (generated code: YDisplayLayer initialization)
    //--- (end of generated code: YDisplayLayer initialization)

    YDisplay *_display;
    int    _id;

public:
    virtual ~YDisplayLayer(){};
    YDisplayLayer(YDisplay *parent, int id);
    //--- (generated code: YDisplayLayer accessors declaration)

    static const int NO_INK = -1;
    static const int BG_INK = -2;
    static const int FG_INK = -3;
    static const Y_ALIGN ALIGN_TOP_LEFT = Y_ALIGN_TOP_LEFT;
    static const Y_ALIGN ALIGN_CENTER_LEFT = Y_ALIGN_CENTER_LEFT;
    static const Y_ALIGN ALIGN_BASELINE_LEFT = Y_ALIGN_BASELINE_LEFT;
    static const Y_ALIGN ALIGN_BOTTOM_LEFT = Y_ALIGN_BOTTOM_LEFT;
    static const Y_ALIGN ALIGN_TOP_CENTER = Y_ALIGN_TOP_CENTER;
    static const Y_ALIGN ALIGN_CENTER = Y_ALIGN_CENTER;
    static const Y_ALIGN ALIGN_BASELINE_CENTER = Y_ALIGN_BASELINE_CENTER;
    static const Y_ALIGN ALIGN_BOTTOM_CENTER = Y_ALIGN_BOTTOM_CENTER;
    static const Y_ALIGN ALIGN_TOP_DECIMAL = Y_ALIGN_TOP_DECIMAL;
    static const Y_ALIGN ALIGN_CENTER_DECIMAL = Y_ALIGN_CENTER_DECIMAL;
    static const Y_ALIGN ALIGN_BASELINE_DECIMAL = Y_ALIGN_BASELINE_DECIMAL;
    static const Y_ALIGN ALIGN_BOTTOM_DECIMAL = Y_ALIGN_BOTTOM_DECIMAL;
    static const Y_ALIGN ALIGN_TOP_RIGHT = Y_ALIGN_TOP_RIGHT;
    static const Y_ALIGN ALIGN_CENTER_RIGHT = Y_ALIGN_CENTER_RIGHT;
    static const Y_ALIGN ALIGN_BASELINE_RIGHT = Y_ALIGN_BASELINE_RIGHT;
    static const Y_ALIGN ALIGN_BOTTOM_RIGHT = Y_ALIGN_BOTTOM_RIGHT;
    static const Y_QRMODE QRMODE_UPCASE_ECCL = Y_QRMODE_UPCASE_ECCL;
    static const Y_QRMODE QRMODE_UPCASE_ECCM = Y_QRMODE_UPCASE_ECCM;
    static const Y_QRMODE QRMODE_UPCASE_ECCQ = Y_QRMODE_UPCASE_ECCQ;
    static const Y_QRMODE QRMODE_UPCASE_ECCH = Y_QRMODE_UPCASE_ECCH;
    static const Y_QRMODE QRMODE_BYTES_ECCL = Y_QRMODE_BYTES_ECCL;
    static const Y_QRMODE QRMODE_BYTES_ECCM = Y_QRMODE_BYTES_ECCM;
    static const Y_QRMODE QRMODE_BYTES_ECCQ = Y_QRMODE_BYTES_ECCQ;
    static const Y_QRMODE QRMODE_BYTES_ECCH = Y_QRMODE_BYTES_ECCH;

    virtual bool        must_be_flushed(void);

    virtual int         resetHiddenFlag(void);

    virtual int         flush_now(void);

    virtual int         command_push(string cmd);

    virtual int         command_flush(string cmd);

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
    virtual int         reset(void);

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
    virtual int         clear(void);

    /**
     * Selects the color to be used for all subsequent drawing functions,
     * for filling as well as for line and text drawing.
     * To select a different fill and outline color, use
     * selectFillColor and selectLineColor.
     * The pen color is provided as an RGB value.
     * For grayscale or monochrome displays, the value is
     * automatically converted to the proper range.
     *
     * @param color : the desired pen color, as a 24-bit RGB value
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         selectColorPen(int color);

    /**
     * Selects the pen gray level for all subsequent drawing functions,
     * for filling as well as for line and text drawing.
     * To select a different fill and outline color, use
     * selectFillColor and selectLineColor.
     * The gray level is provided as a number between
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
    virtual int         selectGrayPen(int graylevel);

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
    virtual int         selectEraser(void);

    /**
     * Selects the color to be used for filling rectangular bars,
     * discs and polygons. The color is provided as an RGB value.
     * For grayscale or monochrome displays, the value is
     * automatically converted to the proper range.
     * You can also use the constants FG_INK to use the
     * default drawing colour, BG_INK to use the default
     * background colour, and NO_INK to disable filling.
     *
     * @param color : the desired drawing color, as a 24-bit RGB value,
     *         or one of the constants NO_INK, FG_INK
     *         or BG_INK
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         selectFillColor(int color);

    /**
     * Selects the color to be used for drawing the outline of rectangular
     * bars, discs and polygons, as well as for drawing lines and text.
     * The color is provided as an RGB value.
     * For grayscale or monochrome displays, the value is
     * automatically converted to the proper range.
     * You can also use the constants FG_INK to use the
     * default drawing colour, BG_INK to use the default
     * background colour, and NO_INK to disable outline drawing.
     *
     * @param color : the desired drawing color, as a 24-bit RGB value,
     *         or one of the constants NO_INK, FG_INK
     *         or BG_INK
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         selectLineColor(int color);

    /**
     * Selects the line width for drawing the outline of rectangular
     * bars, discs and polygons, as well as for drawing lines.
     *
     * @param width : the desired line width, in pixels
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         selectLineWidth(int width);

    virtual int         setAntialiasingMode(bool mode);

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
    virtual int         drawPixel(int x,int y);

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
    virtual int         drawRect(int x1,int y1,int x2,int y2);

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
    virtual int         drawBar(int x1,int y1,int x2,int y2);

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
    virtual int         drawCircle(int x,int y,int r);

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
    virtual int         drawDisc(int x,int y,int r);

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
    virtual int         selectFont(string fontname);

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
    virtual int         drawText(int x,int y,Y_ALIGN anchor,string text);

    /**
     * Draws an image previously uploaded to the device filesystem, at the specified position.
     * At present time, GIF images are the only supported image format. If you experience
     * problems using an image file, check the device logs for any error message such as
     * missing image file or bad image file format.
     *
     * @param x : the distance from left of layer to the left of the image, in pixels
     * @param y : the distance from top of layer to the top of the image, in pixels
     * @param imagename : the GIF file name
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         drawImage(int x,int y,string imagename);

    /**
     * Draws a GIF image provided as a binary buffer at the specified position.
     * If the image drawing must be included in an animation sequence, save it
     * in the device filesystem first and use drawImage instead.
     *
     * @param x : the distance from left of layer to the left of the image, in pixels
     * @param y : the distance from top of layer to the top of the image, in pixels
     * @param gifimage : a binary object with the content of a GIF file
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         drawGIF(int x,int y,string gifimage);

    /**
     * Displays a QR code at the specified position on the layer. You can choose from several
     * QR code representation modes, depending on the desired content type (restricted alphabet
     * vs. all characters) and the error correction level (ECC level, which can be Low, Medium,
     * Quartile, or High). A code with a full character set and a higher error correction level is
     * larger than a code with a limited character set and a low error correction level. You can
     * precalculate the size of the QR code before displaying it by calling this function with
     * a zoom factor of zero.
     *
     * @param x : the distance in pixels from the left edge of the layer to the left edge of the QR code
     * @param y : the distance in pixels from the top edge of the layer to the top edge of the QR code
     * @param mode : the QR code encoding mode, selected from the enumeration YDisplayLayer::QRMODE:
     *         YDisplayLayer::QRMODE_BYTES_ECCH, YDisplayLayer::QRMODE_BYTES_ECCQ,
     *         YDisplayLayer::QRMODE_BYTES_ECCM, YDisplayLayer::QRMODE_BYTES_ECCL,
     *         YDisplayLayer::QRMODE_UPCASE_ECCH, YDisplayLayer::QRMODE_UPCASE_ECCQ,
     *         YDisplayLayer::QRMODE_UPCASE_ECCM, YDisplayLayer::QRMODE_UPCASE_ECCL.
     * @param zoom : the magnification factor, between 0 (not displayed) and 16
     *         (each point of the QR code is represented by 16x16 pixels on the screen).
     * @param text : the text to encode in the QR code
     *
     * @return the size of one side of the QR code in pixels if successful.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         drawQRCode(int x,int y,Y_QRMODE mode,int zoom,string text);

    /**
     * Draws a bitmap at the specified position. The bitmap is provided as a binary object,
     * where each pixel maps to a bit, from left to right and from top to bottom.
     * The most significant bit of each byte maps to the leftmost pixel, and the least
     * significant bit maps to the rightmost pixel. Bits set to 1 are drawn using the
     * layer selected pen color. Bits set to 0 are drawn using the specified background
     * color, unless NO_INK (-1) is specified, in which case they are not
     * drawn at all (as if transparent).
     *
     * @param x : the distance from left of layer to the left of the bitmap, in pixels
     * @param y : the distance from top of layer to the top of the bitmap, in pixels
     * @param w : the width of the bitmap, in pixels
     * @param bitmap : a binary object
     * @param bgcol : the RGB background color to use for zero bits, as a 24-bit RGB value,
     *         or one of the constants NO_INK, FG_INK or BG_INK
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         drawBitmap(int x,int y,int w,string bitmap,int bgcol);

    /**
     * Draws a color pixmap at the specified position. The pixmap is provided as a binary
     * object, where each byte maps to one pixel. The 24 bit RGB value corresponding to each
     * byte value is defined in the palette provided as extra argument.
     * The palette maximal size is 8, and it is recommended to use the smallest possible
     * palette size to optimize the size of data to be sent to the display.
     * The height of the pixmap is implicitely given by the pixmap buffer size.
     *
     * @param x : the distance from left of layer to the left of the pixmap, in pixels
     * @param y : the distance from top of layer to the top of the pixmap, in pixels
     * @param w : the width of the pixmap, in pixels
     * @param pixmap : a binary buffer where each byte maps to one pixel
     * @param palette : an array of 24-bit RGB values, defining the color for each byte value in pixmap
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         drawPixmap(int x,int y,int w,string pixmap,vector<int> palette);

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
    virtual int         moveTo(int x,int y);

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
    virtual int         lineTo(int x,int y);

    /**
     * Starts drawing a polygon with the first corner at the specified position.
     *
     * @param x : the distance from left of layer, in pixels
     * @param y : the distance from top of layer, in pixels
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         polygonStart(int x,int y);

    /**
     * Adds a point to the currently open polygon, previously opened using
     * polygonStart.
     *
     * @param x : the distance from left of layer to the new point, in pixels
     * @param y : the distance from top of layer to the new point, in pixels
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         polygonAdd(int x,int y);

    /**
     * Closes the currently open polygon, fill its content the fill color currently
     * selected for the layer, and draw its outline using the selected line color.
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         polygonEnd(void);

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
    virtual int         consoleOut(string text);

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
    virtual int         setConsoleMargins(int x1,int y1,int x2,int y2);

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
    virtual int         setConsoleBackground(int bgcol);

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
    virtual int         setConsoleWordWrap(bool wordwrap);

    /**
     * Blanks the console area within console margins, and resets the console pointer
     * to the upper left corner of the console.
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         clearConsole(void);

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
    virtual int         setLayerPosition(int x,int y,int scrollTime);

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
    virtual int         hide(void);

    /**
     * Shows the layer. Shows the layer again after a hide command.
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         unhide(void);

    /**
     * Gets parent YDisplay. Returns the parent YDisplay object of the current YDisplayLayer::
     *
     * @return an YDisplay object
     */
    virtual YDisplay*   get_display(void);

    /**
     * Returns the display width, in pixels.
     *
     * @return an integer corresponding to the display width, in pixels
     *
     * On failure, throws an exception or returns YDisplayLayer::DISPLAYWIDTH_INVALID.
     */
    virtual int         get_displayWidth(void);

    /**
     * Returns the display height, in pixels.
     *
     * @return an integer corresponding to the display height, in pixels
     *
     * On failure, throws an exception or returns YDisplayLayer::DISPLAYHEIGHT_INVALID.
     */
    virtual int         get_displayHeight(void);

    /**
     * Returns the width of the layers to draw on, in pixels.
     *
     * @return an integer corresponding to the width of the layers to draw on, in pixels
     *
     * On failure, throws an exception or returns YDisplayLayer::LAYERWIDTH_INVALID.
     */
    virtual int         get_layerWidth(void);

    /**
     * Returns the height of the layers to draw on, in pixels.
     *
     * @return an integer corresponding to the height of the layers to draw on, in pixels
     *
     * On failure, throws an exception or returns YDisplayLayer::LAYERHEIGHT_INVALID.
     */
    virtual int         get_layerHeight(void);

#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of generated code: YDisplayLayer accessors declaration)
    int drawBitmap(int x,int y,int w,const std::vector<unsigned char>& data,int bgcol);
};


//--- (generated code: YDisplay declaration)
/**
 * YDisplay Class: display control interface, available for instance in the Yocto-Display, the
 * Yocto-MaxiDisplay, the Yocto-MaxiDisplay-G or the Yocto-MiniDisplay
 *
 * The YDisplay class allows to drive Yoctopuce displays.
 * Yoctopuce display interface has been designed to easily
 * show information and images. The device provides built-in
 * multi-layer rendering. Layers can be drawn offline, individually,
 * and freely moved on the display. It can also replay recorded
 * sequences (animations).
 *
 * In order to draw on the screen, you should use the
 * display.get_displayLayer method to retrieve the layer(s) on
 * which you want to draw, and then use methods defined in
 * YDisplayLayer to draw on the layers.
 */
class YOCTO_CLASS_EXPORT YDisplay: public YFunction {
#ifdef __BORLANDC__
#pragma option push -w-8022
#endif
//--- (end of generated code: YDisplay declaration)
    //--- (generated code: YDisplay attributes)
    // Attributes (function value cache)
    Y_ENABLED_enum  _enabled;
    string          _startupSeq;
    int             _brightness;
    int             _autoInvertDelay;
    Y_ORIENTATION_enum _orientation;
    string          _displayPanel;
    int             _displayWidth;
    int             _displayHeight;
    Y_DISPLAYTYPE_enum _displayType;
    int             _layerWidth;
    int             _layerHeight;
    int             _layerCount;
    string          _command;
    YDisplayValueCallback _valueCallbackDisplay;
    vector<YDisplayLayer*> _allDisplayLayers;
    u64             _frozenUntil;
    bool            _recording;
    string          _sequence;

    friend YDisplay *yFindDisplay(const string& func);
    friend YDisplay *yFirstDisplay(void);

    // Function-specific method for parsing of JSON output and caching result
    virtual int     _parseAttr(YJSONObject *json_val);

    // Constructor is protected, use yFindDisplay factory function to instantiate
    YDisplay(const string& func);
    //--- (end of generated code: YDisplay attributes)

    //--- (generated code: YDisplay initialization)
    //--- (end of generated code: YDisplay initialization)

public:
    ~YDisplay();
    //--- (generated code: YDisplay accessors declaration)

    static const Y_ENABLED_enum ENABLED_FALSE = Y_ENABLED_FALSE;
    static const Y_ENABLED_enum ENABLED_TRUE = Y_ENABLED_TRUE;
    static const Y_ENABLED_enum ENABLED_INVALID = Y_ENABLED_INVALID;
    static const string STARTUPSEQ_INVALID;
    static const int BRIGHTNESS_INVALID = YAPI_INVALID_UINT;
    static const int AUTOINVERTDELAY_INVALID = YAPI_INVALID_UINT;
    static const Y_ORIENTATION_enum ORIENTATION_LEFT = Y_ORIENTATION_LEFT;
    static const Y_ORIENTATION_enum ORIENTATION_UP = Y_ORIENTATION_UP;
    static const Y_ORIENTATION_enum ORIENTATION_RIGHT = Y_ORIENTATION_RIGHT;
    static const Y_ORIENTATION_enum ORIENTATION_DOWN = Y_ORIENTATION_DOWN;
    static const Y_ORIENTATION_enum ORIENTATION_INVALID = Y_ORIENTATION_INVALID;
    static const string DISPLAYPANEL_INVALID;
    static const int DISPLAYWIDTH_INVALID = YAPI_INVALID_UINT;
    static const int DISPLAYHEIGHT_INVALID = YAPI_INVALID_UINT;
    static const Y_DISPLAYTYPE_enum DISPLAYTYPE_MONO = Y_DISPLAYTYPE_MONO;
    static const Y_DISPLAYTYPE_enum DISPLAYTYPE_EPAPER_BW = Y_DISPLAYTYPE_EPAPER_BW;
    static const Y_DISPLAYTYPE_enum DISPLAYTYPE_EPAPER_BWR = Y_DISPLAYTYPE_EPAPER_BWR;
    static const Y_DISPLAYTYPE_enum DISPLAYTYPE_EPAPER_BWRY = Y_DISPLAYTYPE_EPAPER_BWRY;
    static const Y_DISPLAYTYPE_enum DISPLAYTYPE_INVALID = Y_DISPLAYTYPE_INVALID;
    static const int LAYERWIDTH_INVALID = YAPI_INVALID_UINT;
    static const int LAYERHEIGHT_INVALID = YAPI_INVALID_UINT;
    static const int LAYERCOUNT_INVALID = YAPI_INVALID_UINT;
    static const string COMMAND_INVALID;
    static const Y_FASTREFRESH FASTREFRESH_WHENEVER_POSSIBLE = Y_FASTREFRESH_WHENEVER_POSSIBLE;
    static const Y_FASTREFRESH FASTREFRESH_WHENEVER_SUPPORTED = Y_FASTREFRESH_WHENEVER_SUPPORTED;
    static const Y_FASTREFRESH FASTREFRESH_NEVER = Y_FASTREFRESH_NEVER;
    static const Y_FASTREFRESH FASTREFRESH_INVALID = Y_FASTREFRESH_INVALID;
    static const Y_REGENERATE REGENERATE_ON_REQUEST_ONLY = Y_REGENERATE_ON_REQUEST_ONLY;
    static const Y_REGENERATE REGENERATE_EVERY_DAY = Y_REGENERATE_EVERY_DAY;
    static const Y_REGENERATE REGENERATE_EVERY_12H = Y_REGENERATE_EVERY_12H;
    static const Y_REGENERATE REGENERATE_EVERY_6H = Y_REGENERATE_EVERY_6H;
    static const Y_REGENERATE REGENERATE_EVERY_3H = Y_REGENERATE_EVERY_3H;
    static const Y_REGENERATE REGENERATE_EVERY_2H = Y_REGENERATE_EVERY_2H;
    static const Y_REGENERATE REGENERATE_EVERY_HOUR = Y_REGENERATE_EVERY_HOUR;
    static const Y_REGENERATE REGENERATE_EVERY_30MIN = Y_REGENERATE_EVERY_30MIN;
    static const Y_REGENERATE REGENERATE_EVERY_15MIN = Y_REGENERATE_EVERY_15MIN;
    static const Y_REGENERATE REGENERATE_EVERY_480 = Y_REGENERATE_EVERY_480;
    static const Y_REGENERATE REGENERATE_EVERY_432 = Y_REGENERATE_EVERY_432;
    static const Y_REGENERATE REGENERATE_EVERY_360 = Y_REGENERATE_EVERY_360;
    static const Y_REGENERATE REGENERATE_EVERY_288 = Y_REGENERATE_EVERY_288;
    static const Y_REGENERATE REGENERATE_EVERY_240 = Y_REGENERATE_EVERY_240;
    static const Y_REGENERATE REGENERATE_EVERY_192 = Y_REGENERATE_EVERY_192;
    static const Y_REGENERATE REGENERATE_EVERY_144 = Y_REGENERATE_EVERY_144;
    static const Y_REGENERATE REGENERATE_EVERY_96 = Y_REGENERATE_EVERY_96;
    static const Y_REGENERATE REGENERATE_EVERY_48 = Y_REGENERATE_EVERY_48;
    static const Y_REGENERATE REGENERATE_EVERY_36 = Y_REGENERATE_EVERY_36;
    static const Y_REGENERATE REGENERATE_EVERY_24 = Y_REGENERATE_EVERY_24;
    static const Y_REGENERATE REGENERATE_EVERY_12 = Y_REGENERATE_EVERY_12;
    static const Y_REGENERATE REGENERATE_EVERY_10 = Y_REGENERATE_EVERY_10;
    static const Y_REGENERATE REGENERATE_EVERY_8 = Y_REGENERATE_EVERY_8;
    static const Y_REGENERATE REGENERATE_EVERY_6 = Y_REGENERATE_EVERY_6;
    static const Y_REGENERATE REGENERATE_EVERY_4 = Y_REGENERATE_EVERY_4;
    static const Y_REGENERATE REGENERATE_ALWAYS = Y_REGENERATE_ALWAYS;
    static const Y_REGENERATE REGENERATE_INVALID = Y_REGENERATE_INVALID;
    static const Y_DISPLAYSTATE DISPLAYSTATE_FAILURE = Y_DISPLAYSTATE_FAILURE;
    static const Y_DISPLAYSTATE DISPLAYSTATE_OFF = Y_DISPLAYSTATE_OFF;
    static const Y_DISPLAYSTATE DISPLAYSTATE_POWERING = Y_DISPLAYSTATE_POWERING;
    static const Y_DISPLAYSTATE DISPLAYSTATE_IDLE = Y_DISPLAYSTATE_IDLE;
    static const Y_DISPLAYSTATE DISPLAYSTATE_REFRESHING = Y_DISPLAYSTATE_REFRESHING;
    static const Y_DISPLAYSTATE DISPLAYSTATE_INVALID = Y_DISPLAYSTATE_INVALID;

    /**
     * Returns true if the screen is powered, false otherwise.
     *
     * @return either YDisplay::ENABLED_FALSE or YDisplay::ENABLED_TRUE, according to true if the screen is
     * powered, false otherwise
     *
     * On failure, throws an exception or returns YDisplay::ENABLED_INVALID.
     */
    Y_ENABLED_enum      get_enabled(void);

    inline Y_ENABLED_enum enabled(void)
    { return this->get_enabled(); }

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
    int             set_enabled(Y_ENABLED_enum newval);
    inline int      setEnabled(Y_ENABLED_enum newval)
    { return this->set_enabled(newval); }

    /**
     * Returns the name of the sequence to play when the displayed is powered on.
     *
     * @return a string corresponding to the name of the sequence to play when the displayed is powered on
     *
     * On failure, throws an exception or returns YDisplay::STARTUPSEQ_INVALID.
     */
    string              get_startupSeq(void);

    inline string       startupSeq(void)
    { return this->get_startupSeq(); }

    /**
     * Changes the name of the sequence to play when the display is powered on.
     * Remember to call the saveToFlash() method of the module if the
     * modification must be kept.
     *
     * @param newval : a string corresponding to the name of the sequence to play when the display is powered on
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_startupSeq(const string& newval);
    inline int      setStartupSeq(const string& newval)
    { return this->set_startupSeq(newval); }

    /**
     * Returns the luminosity of the  module informative LEDs (from 0 to 100).
     *
     * @return an integer corresponding to the luminosity of the  module informative LEDs (from 0 to 100)
     *
     * On failure, throws an exception or returns YDisplay::BRIGHTNESS_INVALID.
     */
    int                 get_brightness(void);

    inline int          brightness(void)
    { return this->get_brightness(); }

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
    int             set_brightness(int newval);
    inline int      setBrightness(int newval)
    { return this->set_brightness(newval); }

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
    int                 get_autoInvertDelay(void);

    inline int          autoInvertDelay(void)
    { return this->get_autoInvertDelay(); }

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
    int             set_autoInvertDelay(int newval);
    inline int      setAutoInvertDelay(int newval)
    { return this->set_autoInvertDelay(newval); }

    /**
     * Returns the currently selected display orientation. The orientation is defined as the side of the
     * screen where the
     * USB connector (for OLED displays) or the ribbon cable (for ePaper panels) is located when the
     * display is up straight.
     *
     * @return a value among YDisplay::ORIENTATION_LEFT, YDisplay::ORIENTATION_UP,
     * YDisplay::ORIENTATION_RIGHT and YDisplay::ORIENTATION_DOWN corresponding to the currently selected
     * display orientation
     *
     * On failure, throws an exception or returns YDisplay::ORIENTATION_INVALID.
     */
    Y_ORIENTATION_enum  get_orientation(void);

    inline Y_ORIENTATION_enum orientation(void)
    { return this->get_orientation(); }

    /**
     * Changes the display orientation. he orientation is defined as the side of the screen where the
     * USB connector (for OLED displays) or the ribbon cable (for ePaper panels) is located when the
     * display is up straight. Remember to call the saveToFlash()
     * method of the module if the modification must be kept.
     *
     * @param newval : a value among YDisplay::ORIENTATION_LEFT, YDisplay::ORIENTATION_UP,
     * YDisplay::ORIENTATION_RIGHT and YDisplay::ORIENTATION_DOWN corresponding to the display orientation
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_orientation(Y_ORIENTATION_enum newval);
    inline int      setOrientation(Y_ORIENTATION_enum newval)
    { return this->set_orientation(newval); }

    /**
     * Returns the exact model of the display panel.
     *
     * @return a string corresponding to the exact model of the display panel
     *
     * On failure, throws an exception or returns YDisplay::DISPLAYPANEL_INVALID.
     */
    string              get_displayPanel(void);

    inline string       displayPanel(void)
    { return this->get_displayPanel(); }

    /**
     * Changes the model of display to match the connected display panel.
     * Value examples:  "E2_266_KS0_C1" , "E2_290_QS_0F22 , "E2_370_KS_0C1", "E2_417_KS_0D0" etc..
     * This function has no effect if the module does not support the selected
     * display panel. Remember to call the saveToFlash()
     * method of the module if the modification must be kept.
     *
     * @param newval : a string corresponding to the model of display to match the connected display panel
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    int             set_displayPanel(const string& newval);
    inline int      setDisplayPanel(const string& newval)
    { return this->set_displayPanel(newval); }

    /**
     * Returns the display width, in pixels.
     *
     * @return an integer corresponding to the display width, in pixels
     *
     * On failure, throws an exception or returns YDisplay::DISPLAYWIDTH_INVALID.
     */
    int                 get_displayWidth(void);

    inline int          displayWidth(void)
    { return this->get_displayWidth(); }

    /**
     * Returns the display height, in pixels.
     *
     * @return an integer corresponding to the display height, in pixels
     *
     * On failure, throws an exception or returns YDisplay::DISPLAYHEIGHT_INVALID.
     */
    int                 get_displayHeight(void);

    inline int          displayHeight(void)
    { return this->get_displayHeight(); }

    /**
     * Returns the display type: monochrome OLED, black and white ePaper, color ePaper, and so on.
     *
     * @return a value among YDisplay::DISPLAYTYPE_MONO, YDisplay::DISPLAYTYPE_EPAPER_BW,
     * YDisplay::DISPLAYTYPE_EPAPER_BWR and YDisplay::DISPLAYTYPE_EPAPER_BWRY corresponding to the display
     * type: monochrome OLED, black and white ePaper, color ePaper, and so on
     *
     * On failure, throws an exception or returns YDisplay::DISPLAYTYPE_INVALID.
     */
    Y_DISPLAYTYPE_enum  get_displayType(void);

    inline Y_DISPLAYTYPE_enum displayType(void)
    { return this->get_displayType(); }

    /**
     * Returns the width of the layers to draw on, in pixels.
     *
     * @return an integer corresponding to the width of the layers to draw on, in pixels
     *
     * On failure, throws an exception or returns YDisplay::LAYERWIDTH_INVALID.
     */
    int                 get_layerWidth(void);

    inline int          layerWidth(void)
    { return this->get_layerWidth(); }

    /**
     * Returns the height of the layers to draw on, in pixels.
     *
     * @return an integer corresponding to the height of the layers to draw on, in pixels
     *
     * On failure, throws an exception or returns YDisplay::LAYERHEIGHT_INVALID.
     */
    int                 get_layerHeight(void);

    inline int          layerHeight(void)
    { return this->get_layerHeight(); }

    /**
     * Returns the number of available layers to draw on.
     *
     * @return an integer corresponding to the number of available layers to draw on
     *
     * On failure, throws an exception or returns YDisplay::LAYERCOUNT_INVALID.
     */
    int                 get_layerCount(void);

    inline int          layerCount(void)
    { return this->get_layerCount(); }

    string              get_command(void);

    inline string       command(void)
    { return this->get_command(); }

    int             set_command(const string& newval);
    inline int      setCommand(const string& newval)
    { return this->set_command(newval); }

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
    static YDisplay*    FindDisplay(string func);

    /**
     * Registers the callback function that is invoked on every change of advertised value.
     * The callback is then invoked only during the execution of ySleep or yHandleEvents.
     * This provides control over the time when the callback is triggered. For good responsiveness,
     * remember to call one of these two functions periodically. The callback is called once juste after beeing
     * registered, passing the current advertised value  of the function, provided that it is not an empty string.
     * To unregister a callback, pass a NULL pointer as argument.
     *
     * @param callback : the callback function to call, or a NULL pointer. The callback function should take two
     *         arguments: the function object of which the value has changed, and the character string describing
     *         the new advertised value.
     * @noreturn
     */
    virtual int         registerValueCallback(YDisplayValueCallback callback);
    using YFunction::registerValueCallback;

    virtual int         _invokeValueCallback(string value);

    virtual int         sendCommand(string cmd);

    virtual int         flushLayers(void);

    virtual int         resetHiddenLayerFlags(void);

    virtual bool        isFrozen(void);

    /**
     * Returns the fast refresh usage policy in use (ePaper displays only).
     * This setting is combined with the regenerate policy to determine when the screen
     * should be updated using a fast update versus or regenerated using a slower,
     * flickering full refresh.
     *
     * @return a value among the YDisplay::FASTREFRESH enumeration
     *         (YDisplay::FASTREFRESH_WHENEVER_POSSIBLE,
     *         YDisplay::FASTREFRESH_WHENEVER_SUPPORTED,
     *         YDisplay::FASTREFRESH_NEVER).
     *
     * On failure, throws an exception or returns YDisplay::FASTREFRESH_INVALID.
     */
    virtual Y_FASTREFRESH get_fastRefreshPolicy(void);

    /**
     * Returns the display regeneration minimal frequency (ePaper displays only).
     * This setting is combined with the fast refresh usage policy to determine
     * when the screen should be updated using a fast update versus or regenerated
     * using a slower, flickering full refresh. To change the display regeneration minimal
     * frequency, use methode set_fastRefreshPolicy().
     *
     * @return a value among the YDisplay::REGENERATE enumeration
     *         (YDisplay::REGENERATE_ON_REQUEST_ONLY,
     *         YDisplay::REGENERATE_EVERY_DAY, YDisplay::REGENERATE_EVERY_12H,
     *         YDisplay::REGENERATE_EVERY_6H, YDisplay::REGENERATE_EVERY_3H,
     *         YDisplay::REGENERATE_EVERY_2H, YDisplay::REGENERATE_EVERY_HOUR,
     *         YDisplay::REGENERATE_EVERY_30MIN, YDisplay::REGENERATE_EVERY_15MIN,
     *         YDisplay::REGENERATE_EVERY_480, YDisplay::REGENERATE_EVERY_432,
     *         YDisplay::REGENERATE_EVERY_360, YDisplay::REGENERATE_EVERY_288,
     *         YDisplay::REGENERATE_EVERY_240, YDisplay::REGENERATE_EVERY_192,
     *         YDisplay::REGENERATE_EVERY_144, YDisplay::REGENERATE_EVERY_96,
     *         YDisplay::REGENERATE_EVERY_48, YDisplay::REGENERATE_EVERY_36,
     *         YDisplay::REGENERATE_EVERY_24, YDisplay::REGENERATE_EVERY_12,
     *         YDisplay::REGENERATE_EVERY_10, YDisplay::REGENERATE_EVERY_8,
     *         YDisplay::REGENERATE_EVERY_6, YDisplay::REGENERATE_EVERY_4,
     *         YDisplay::REGENERATE_ALWAYS).
     *
     * On failure, throws an exception or returns YDisplay::REGENERATE_INVALID.
     */
    virtual Y_REGENERATE get_regeneratePolicy(void);

    /**
     * Changes the fast refresh usage policy and display regeneration minimal frequency
     * (ePaper displays only). These settings jointly determine when the screen should be
     * updated using a fast update versus or regenerated using a slower, flickering full
     * refresh.
     *
     * @param fastRefresh : a value among the YDisplay::FASTREFRESH enumeration
     *         (YDisplay::FASTREFRESH_WHENEVER_POSSIBLE,
     *         YDisplay::FASTREFRESH_WHENEVER_SUPPORTED,
     *         YDisplay::FASTREFRESH_NEVER),
     *         corresponding to the policy for using fast refresh.
     * @param regenerate : a value among the enumeration YRefFrame.REGENERATE
     *         (YDisplay::REGENERATE_ON_REQUEST_ONLY,
     *         YDisplay::REGENERATE_EVERY_DAY, YDisplay::REGENERATE_EVERY_12H,
     *         YDisplay::REGENERATE_EVERY_6H, YDisplay::REGENERATE_EVERY_3H,
     *         YDisplay::REGENERATE_EVERY_2H, YDisplay::REGENERATE_EVERY_HOUR,
     *         YDisplay::REGENERATE_EVERY_30MIN, YDisplay::REGENERATE_EVERY_15MIN,
     *         YDisplay::REGENERATE_EVERY_480, YDisplay::REGENERATE_EVERY_432,
     *         YDisplay::REGENERATE_EVERY_360, YDisplay::REGENERATE_EVERY_288,
     *         YDisplay::REGENERATE_EVERY_240, YDisplay::REGENERATE_EVERY_192,
     *         YDisplay::REGENERATE_EVERY_144, YDisplay::REGENERATE_EVERY_96,
     *         YDisplay::REGENERATE_EVERY_48, YDisplay::REGENERATE_EVERY_36,
     *         YDisplay::REGENERATE_EVERY_24, YDisplay::REGENERATE_EVERY_12,
     *         YDisplay::REGENERATE_EVERY_10, YDisplay::REGENERATE_EVERY_8,
     *         YDisplay::REGENERATE_EVERY_6, YDisplay::REGENERATE_EVERY_4,
     *         YDisplay::REGENERATE_ALWAYS),
     *         corresponding to the display minimal regeneration frequency.
     *
     * Remember to call the saveToFlash()
     * method of the module if the modification must be kept.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         set_fastRefreshPolicy(Y_FASTREFRESH fastRefresh,Y_REGENERATE regenerate);

    /**
     * Clears the display screen and resets all display layers to their default state.
     * Using this function in a sequence will kill the sequence play-back. Do not use that
     * function to reset the display at sequence start-up.
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         resetAll(void);

    /**
     * Forces an ePaper screen to perform a regenerative update using the slow
     * update method. Periodic use of the slow method (total panel update with
     * multiple inversions) prevents ghosting effects and improves contrast.
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         regenerateDisplay(void);

    /**
     * Returns the current state of an ePaper display, specifically to
     * determine whether an update is in progress or whether a
     * configuration issue has been detected. If a display configuration
     * error has been detected, the error message can be retrieved.
     *
     * @param errmsg : a string passed by reference to receive the error message.
     *
     * @return a value among the enumeration YDisplay::DISPLAYSTATE
     *         (YDisplay::DISPLAYSTATE_FAILURE, YDisplay::DISPLAYSTATE_OFF,
     *         YDisplay::DISPLAYSTATE_POWERING, YDisplay::DISPLAYSTATE_IDLE,
     *         YDisplay::DISPLAYSTATE_REFRESHING)
     *         corresponding to the current display state.
     */
    virtual Y_DISPLAYSTATE get_ePaperState(string& errmsg);

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
    virtual int         postponeRefresh(int duration);

    /**
     * Triggers an immediate screen refresh. The combination of
     * postponeRefresh and triggerRefresh can be used as an
     * alternative to double-buffering to avoid flickering during display updates.
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         triggerRefresh(void);

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
    virtual int         fade(int brightness,int duration);

    /**
     * Starts to record all display commands into a sequence, for later replay.
     * The name used to store the sequence is specified when calling
     * saveSequence(), once the recording is complete.
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         newSequence(void);

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
    virtual int         saveSequence(string sequenceName);

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
    virtual int         playSequence(string sequenceName);

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
    virtual int         pauseSequence(int delay_ms);

    /**
     * Stops immediately any ongoing sequence replay.
     * The display is left as is.
     *
     * @return YAPI::SUCCESS if the call succeeds.
     *
     * On failure, throws an exception or returns a negative error code.
     */
    virtual int         stopSequence(void);

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
    virtual int         upload(string pathname,string content);

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
    virtual int         copyLayerContent(int srcLayerId,int dstLayerId);

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
    virtual int         swapLayerContent(int layerIdA,int layerIdB);

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
    virtual YDisplayLayer* get_displayLayer(int layerId);

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
    virtual string      readDisplay(vector<int> palette);

    virtual string      gifEncode(string pixmap,vector<int> palette,int w,bool shortHdr);


    inline static YDisplay *Find(string func)
    { return YDisplay::FindDisplay(func); }

    /**
     * Continues the enumeration of displays started using yFirstDisplay().
     * Caution: You can't make any assumption about the returned displays order.
     * If you want to find a specific a display, use Display.findDisplay()
     * and a hardwareID or a logical name.
     *
     * @return a pointer to a YDisplay object, corresponding to
     *         a display currently online, or a NULL pointer
     *         if there are no more displays to enumerate.
     */
           YDisplay        *nextDisplay(void);
    inline YDisplay        *next(void)
    { return this->nextDisplay();}

    /**
     * Starts the enumeration of displays currently accessible.
     * Use the method YDisplay::nextDisplay() to iterate on
     * next displays.
     *
     * @return a pointer to a YDisplay object, corresponding to
     *         the first display currently online, or a NULL pointer
     *         if there are none.
     */
           static YDisplay *FirstDisplay(void);
    inline static YDisplay *First(void)
    { return YDisplay::FirstDisplay();}
#ifdef __BORLANDC__
#pragma option pop
#endif
    //--- (end of generated code: YDisplay accessors declaration)

};

//--- (generated code: YDisplay functions declaration)

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
inline YDisplay *yFindDisplay(const string& func)
{ return YDisplay::FindDisplay(func);}
/**
 * Starts the enumeration of displays currently accessible.
 * Use the method YDisplay::nextDisplay() to iterate on
 * next displays.
 *
 * @return a pointer to a YDisplay object, corresponding to
 *         the first display currently online, or a NULL pointer
 *         if there are none.
 */
inline YDisplay *yFirstDisplay(void)
{ return YDisplay::FirstDisplay();}

//--- (end of generated code: YDisplay functions declaration)

#ifdef YOCTOLIB_NAMESPACE
}
#endif

#endif
