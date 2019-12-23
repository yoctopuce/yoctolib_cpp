/*********************************************************************
 *
 *  $Id: main.cpp 38820 2019-12-18 18:01:14Z seb $
 *
 *  An example that show how to use a  Yocto-MaxiDisplay
 *
 *  You can find more information on our web site:
 *   Yocto-MaxiDisplay documentation:
 *      https://www.yoctopuce.com/EN/products/yocto-maxidisplay/doc.html
 *   C++ API Reference:
 *      https://www.yoctopuce.com/EN/doc/reference/yoctolib-cpp-EN.html
 *
 *********************************************************************/

#include "yocto_api.h"
#include "yocto_display.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

static void usage(void)
{
  cout << "Wrong command line arguments" << endl;
  cout << "usage: demo <serial_number>" << endl;
  cout << "       demo <logical_name>" << endl;
  cout << "       demo any (use any discovered device)" << endl;
  u64 now = YAPI::GetTickCount();
  while (YAPI::GetTickCount() - now < 3000) {
    // wait 3 sec to show the message
  }
  exit(1);
}

int main(int argc, const char * argv[])
{
  string         errmsg;
  string         target;
  YDisplay      *disp;
  YDisplayLayer *l0, *l1;
  int w, h, x, y, vx, vy;

  if(argc < 2) {
    usage();
  }

  // Setup the API to use local USB devices
  if (YAPI::RegisterHub("usb", errmsg) != YAPI::SUCCESS) {
    cerr << "RegisterHub error: " << errmsg << endl;
    usage();
    return 1;
  }

  target     = (string) argv[1];
  if (target == "any") {
    disp =  YDisplay::FirstDisplay();
    if (disp == NULL) {
      cout << "No module connected (check USB cable)" << endl;
      usage();
      return 1;
    }
  } else {
    disp =  YDisplay::FindDisplay(target + ".display");
  }

  if (!disp->isOnline()) {
    cout << "Module is offline (check USB cable)" << endl;
    usage();
    return 1;
  }

  disp->resetAll();
  // retreive the display size
  w = disp->get_displayWidth();
  h = disp->get_displayHeight();

  // reteive the first layer
  l0 = disp->get_displayLayer(0);
  l0->clear();

  // display a text in the middle of the screen
  l0->drawText(w / 2, h / 2, YDisplayLayer::ALIGN_CENTER, "Hello world!" );
  // visualize each corner
  l0->moveTo(0, 5);
  l0->lineTo(0, 0);
  l0->lineTo(5, 0);
  l0->moveTo(0, h - 6);
  l0->lineTo(0, h - 1);
  l0->lineTo(5, h - 1);
  l0->moveTo(w - 1, h - 6);
  l0->lineTo(w - 1, h - 1);
  l0->lineTo(w - 6, h - 1);
  l0->moveTo(w - 1, 5);
  l0->lineTo(w - 1, 0);
  l0->lineTo(w - 6, 0);

  // draw a circle in the top left corner of layer 1
  l1 = disp->get_displayLayer(1);
  l1->clear();
  l1->drawCircle(h / 8, h / 8, h / 8);

  // and animate the layer
  cout << "Use Ctrl-C to stop";
  x = 0;
  y = 0;
  vx = 1;
  vy = 1;
  while (disp->isOnline()) {
    x += vx;
    y += vy;
    if ((x < 0) || (x > w - (h / 4)))
      vx = -vx;
    if ((y < 0) || (y > h - (h / 4)))
      vy = -vy;
    l1->setLayerPosition(x, y, 0);
    YAPI::Sleep(5, errmsg);
  }
  YAPI::FreeAPI();
  return 0;
}
