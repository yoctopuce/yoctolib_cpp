/*********************************************************************
 *
 *  $Id: main.cpp 44664 2021-04-23 09:45:05Z seb $
 *
 *  An example that show how to use a  Yocto-MaxiBuzzer
 *
 *  You can find more information on our web site:
 *   Yocto-MaxiBuzzer documentation:
 *      https://www.yoctopuce.com/EN/products/yocto-maxibuzzer/doc.html
 *   C++ V2 API Reference:
 *      https://www.yoctopuce.com/EN/doc/reference/yoctolib-cpp-EN.html
 *
 *********************************************************************/

#include "yocto_api.h"
#include "yocto_buzzer.h"
#include "yocto_anbutton.h"
#include "yocto_colorled.h"
#include <iostream>
#include <ctype.h>
#include <stdlib.h>

using namespace std;

static void usage(void)
{
  cout << "usage: demo <serial_number> " << endl;
  cout << "       demo any  (use any discovered device)" << endl;
  u64 now = yGetTickCount();
  while (yGetTickCount() - now < 3000) {
    // wait 3 sec to show the message
  }
  exit(1);
}

int main(int argc, const char * argv[])
{
  string  errmsg;
  string  target, serial;
  YBuzzer  *buz;
  YColorLed *led;
  YAnButton *button1, *button2;
  int i;
  int frequency;
  int volume;
  int color;

  if (argc < 2) {
    usage();
  }
  target = (string) argv[1];

  // Setup the API to use local USB devices
  if (YAPI::RegisterHub("usb", errmsg) != YAPI_SUCCESS) {
    cerr << "RegisterHub error: " << errmsg << endl;
    return 1;
  }

  if (target == "any") {
    buz = YBuzzer::FirstBuzzer();
    if (buz == NULL) {
      cout << "No module connected (check USB cable)" << endl;
      return 1;
    }
  } else {
    buz = YBuzzer::FindBuzzer(target + ".buzzer");
  }

  if (!buz->isOnline()) {
    cout << "Module not connected (check identification and USB cable)" << endl;
    return 1;
  }
  frequency = 1000;
  serial    = buz->get_module()->get_serialNumber();
  led       = YColorLed::FindColorLed(serial + ".colorLed");
  button1   = YAnButton::FindAnButton(serial + ".anButton1");
  button2   = YAnButton::FindAnButton(serial + ".anButton2");

  cout << "press a test button or hit Ctrl-C" << endl;
  while (button1->isOnline()) {
    int b1 = button1->isPressed();
    int b2 = button2->isPressed();
    if (b1 || b2 ) {
      if (b1) {
        volume = 60;
        frequency = 1500;
        color = 0xff0000;
      } else {
        volume = 30;
        color = 0x00ff00;
        frequency = 750;
      }

      led->resetBlinkSeq();
      led->addRgbMoveToBlinkSeq(color, 100);
      led->addRgbMoveToBlinkSeq(0, 100);
      led->startBlinkSeq();
      buz->set_volume(volume);
      for (i = 0; i < 5; i++) {
        // this could be done using a sequence as well
        buz->set_frequency(frequency);
        buz->freqMove(2 * frequency, 250);
        YAPI::Sleep(250, errmsg);
      }
      buz->set_frequency(0);
      led->stopBlinkSeq();
      led->set_rgbColor(0);
    }
  }
  YAPI::FreeAPI();
}

