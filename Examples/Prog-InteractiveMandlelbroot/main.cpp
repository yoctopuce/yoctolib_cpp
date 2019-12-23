/*
 * This demo allows to navigate into an Mandelbroot set,
 * You need
 * A Display
 * 2 anButton connected to swiches  called b1 and b2 (Zoom In/Out)
 * 2 anButton connected to swiches called by and by (Pan Hrt/Vrt)
 */

#include "yocto_api.h"
#include "yocto_display.h"
#include "yocto_anbutton.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

static int zoomInPressed, zoomOutPressed;
static double vx, vy;

static void b1change(YAnButton *b, const string& value)
{
  cout << "b1(" << value << ")" << endl;
  zoomInPressed = atoi(value.c_str()) < 500;
}

static void b2change(YAnButton *b, const string& value)
{
  cout << "b2(" << value << ")" << endl;
  zoomOutPressed = atoi(value.c_str()) < 500;
}

static void bxchange(YAnButton *b, const string& value)
{
  cout << "bx(" << value << ")" << endl;
  int v = (int)(0.5 + (atoi(value.c_str()) - 500) / 50);
  vx = v / 50.0;
}

static void bychange(YAnButton *b, const string& value)
{
  cout << "bx(" << value << ")" << endl;
  int v = (int)(0.5 + (atoi(value.c_str()) - 500) / 50);
  vy = v / 50.0;
}


int main(int argc, const char * argv[])
{
  string         errmsg;
  string         target;
  YDisplay      *disp;
  YDisplayLayer *l0;
  YAnButton     *b1, *b2, *bx, *by;
  int w, h;



  // Setup the API to use local USB devices
  if (YAPI::RegisterHub("usb", errmsg) != YAPI::SUCCESS) {
    cerr << "RegisterHub error: " << errmsg << endl;
    return 1;
  }

  disp = YDisplay::FirstDisplay();
  b1 = YAnButton::FindAnButton("b1");
  b2 = YAnButton::FindAnButton("b2");
  bx = YAnButton::FindAnButton("x");
  by = YAnButton::FindAnButton("y");

  if (!disp) {
    cout << "Module is offline (check USB cable)" << endl;
    return 1;
  }

  if (!b1->isOnline()) {
    cout << "no anbutton call b1" << endl;
    return 1;
  }

  if (!b2->isOnline()) {
    cout << "no anbutton call b2" << endl;
    return 1;
  }

  if (!bx->isOnline()) {
    cout << "no anbutton call bx" << endl;
    return 1;
  }

  if (!by->isOnline()) {
    cout << "no anbutton call by" << endl;
    return 1;
  }


  b1->registerValueCallback(b1change);
  b2->registerValueCallback(b2change);
  by->registerValueCallback(bychange);
  bx->registerValueCallback(bxchange);

  disp->resetAll();
  // retreive the display size
  w = disp->get_displayWidth();
  h = disp->get_displayHeight();

  // reteive the first layer
  l0 = disp->get_displayLayer(0);
  int bytesPerLines = w / 8;

  std::vector<unsigned char> data(h * bytesPerLines);


  int max_iteration = 50;
  int iteration;
  long double  xtemp;
  long double  centerX = 0;
  long double  centerY = 0;
  long double  x, y, x0, y0;
  long double  zoom    = 1;

  while (1) {
    for(int i = 0 ; i < (int)data.size(); i++)  data[i] = 0;
    if (zoomInPressed)   zoom = zoom * 1.1;
    if (zoomOutPressed)   zoom = zoom / 1.1;

    centerX =  centerX - (vx / zoom);
    centerY =  centerY + (vy / zoom);
    max_iteration = (int)floor(0.5 + max_iteration  + sqrt(zoom) );
    if (max_iteration > 1500)  max_iteration = 1500;

    for ( int j = 0 ; j < h; j++)
      for(int i = 0 ; i < w; i++) {
        x0 = (((i - w / 2.0) / (w / 8)) / zoom) - centerX;
        y0 = (((j - h / 2.0) / (w / 8)) / zoom) - centerY;

        x = 0;
        y = 0;

        iteration = 0;

        while ( (x * x + y * y < 4)  && (iteration < max_iteration )) {
          xtemp = x * x - y * y + x0;
          y = 2 * x * y + y0;
          x = xtemp;
          iteration += 1;
        }

        if (iteration >= max_iteration)
          data[j * bytesPerLines + (i >> 3)] |=  (128 >> (i % 8));

      }
    l0->drawBitmap(0, 0, w, data, 0);
    YAPI::HandleEvents(errmsg);
  }
}
