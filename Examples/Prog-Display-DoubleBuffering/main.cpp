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

// this is the recusive function to draw 1/3nd of the Von Koch flake

static void recursiveLine(YDisplayLayer *layer, double x0, double y0, double x1,
                          double y1, int deep)
{
  double dx, dy, mx, my;
  if (deep <= 0) {
    layer->moveTo((int) (x0 + 0.5), (int) (y0 + 0.5));
    layer->lineTo((int) (x1 + 0.5), (int) (y1 + 0.5));
  } else {
    dx = (x1 - x0) / 3;
    dy = (y1 - y0) / 3;
    mx = ((x0 + x1) / 2) + (0.87 * (y1 - y0) / 3);
    my = ((y0 + y1) / 2) - (0.87 * (x1 - x0) / 3);
    recursiveLine(layer, x0, y0, x0 + dx, y0 + dy, deep - 1);
    recursiveLine(layer, x0 + dx, y0 + dy, mx, my, deep - 1);
    recursiveLine(layer, mx, my, x1 - dx, y1 - dy, deep - 1);
    recursiveLine(layer, x1 - dx, y1 - dy, x1, y1, deep - 1);
  }
}

int main(int argc, const char * argv[])
{
  string errmsg;
  string target;
  YDisplay *disp;
  YDisplayLayer *l1, *l2;

  if (argc < 2) usage();

  // Setup the API to use local USB devices
  if (YAPI::RegisterHub("usb", errmsg) != YAPI::SUCCESS) {
    cerr << "RegisterHub error: " << errmsg << endl;
    usage();
    return 1;
  }

  target = (string) argv[1];
  if (target == "any") {
    disp = YDisplay::FirstDisplay();
    if (disp == NULL) {
      cout << "No module connected (check USB cable)" << endl;
      usage();
      return 1;
    }
  } else {
    disp = YDisplay::FindDisplay(target + ".display");
  }

  if (!disp->isOnline()) {
    cout << "Module is offline (check USB cable)" << endl;
    usage();
    return 1;
  }

  // display clean up
  disp->resetAll();

  l1 = disp->get_displayLayer(1);
  l2 = disp->get_displayLayer(2);
  l1->hide(); // L1 is hidden, l2 stay visible
  double centerX = disp->get_displayWidth() / 2;
  double centerY = disp->get_displayHeight() / 2;
  double radius = disp->get_displayHeight() / 2;
  double a = 0;

  while (true) {
    // we draw in the hidden layer
    l1->clear();
    for (int i = 0; i < 3; i++)
      recursiveLine(l1, centerX + radius * cos(a + i * 2.094),
                    centerY + radius * sin(a + i * 2.094),
                    centerX + radius * cos(a + (i + 1) * 2.094),
                    centerY + radius * sin(a + (i + 1) * 2.094), 2);
    // then we swap contents with the visible layer

    disp->swapLayerContent(1, 2);
    // change the flake angle
    a += 0.1257;
  }
}
