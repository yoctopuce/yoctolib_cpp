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
  string errmsg;
  string target;
  YDisplay *disp;
  YDisplayLayer *l0;
  int w, h;

  if (argc < 2) {
    usage();
  }

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
  long double xtemp;
  long double centerX = 0;
  long double centerY = 0;
  long double targetX = 0.834555980181972;
  long double targetY = 0.204552998862566;
  long double x, y, x0, y0;
  long double zoom = 1;
  long double distance = 1;

  while (1) {
    for (int i = 0; i < (int) data.size(); i++) data[i] = 0;
    distance = distance * 0.95;
    centerX = targetX * (1 - distance);
    centerY = targetY * (1 - distance);
    max_iteration = (int) floor(0.5 + max_iteration + sqrt(zoom));
    if (max_iteration > 1500) max_iteration = 1500;
    for (int j = 0; j < h; j++)
      for (int i = 0; i < w; i++) {
        x0 = (((i - w / 2.0) / (w / 8)) / zoom) - centerX;
        y0 = (((j - h / 2.0) / (w / 8)) / zoom) - centerY;

        x = 0;
        y = 0;

        iteration = 0;

        while ((x * x + y * y < 4) && (iteration < max_iteration)) {
          xtemp = x * x - y * y + x0;
          y = 2 * x * y + y0;
          x = xtemp;
          iteration += 1;
        }

        if (iteration >= max_iteration)
          data[j * bytesPerLines + (i >> 3)] |= (128 >> (i % 8));

      }
    l0->drawBitmap(0, 0, w, data, 0);
    zoom = zoom / 0.95;
  }
}
