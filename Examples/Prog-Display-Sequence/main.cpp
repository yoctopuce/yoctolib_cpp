#include "yocto_api.h"
#include "yocto_display.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

#define COUNT 8

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
  int coord[(COUNT * 2) + 1];
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

  //reteive the first layer
  l0 = disp->get_displayLayer(0);


  // precompute the "leds" position
  int ledwidth = (w / COUNT);
  for (int i = 0; i < COUNT; i++) {
    coord[i] = i * ledwidth;
    coord[2 * COUNT - i - 2] = coord[i];
  }

  int framesCount = 2 * COUNT - 2;

  // start recording
  disp->newSequence();

  // build one loop for recording
  for (int i = 0; i < framesCount; i++) {
    l0->selectColorPen(0);
    l0->drawBar(coord[(i + framesCount - 1) % framesCount], h - 1,
                coord[(i + framesCount - 1) % framesCount] + ledwidth, h - 4);
    l0->selectColorPen(0xffffff);
    l0->drawBar(coord[i], h - 1, coord[i] + ledwidth, h - 4);
    disp->pauseSequence(50); // records a 50ms pause.
  }
  // self-call : causes an endless looop
  disp->playSequence("K2000.seq");
  // stop recording and save to device filesystem
  disp->saveSequence("K2000.seq");

  // play the sequence
  disp->playSequence("K2000.seq");

  cout << "This animation is running in background." << endl;
}
