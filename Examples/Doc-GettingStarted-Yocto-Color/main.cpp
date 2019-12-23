/*********************************************************************
 *
 *  $Id: main.cpp 38820 2019-12-18 18:01:14Z seb $
 *
 *  An example that show how to use a  Yocto-Color
 *
 *  You can find more information on our web site:
 *   Yocto-Color documentation:
 *      https://www.yoctopuce.com/EN/products/yocto-color/doc.html
 *   C++ API Reference:
 *      https://www.yoctopuce.com/EN/doc/reference/yoctolib-cpp-EN.html
 *
 *********************************************************************/

#include "yocto_api.h"
#include "yocto_colorled.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

static void usage(void)
{
  cout << "usage: demo <serial_number>  [ color | rgb ]" << endl;
  cout << "       demo <logical_name> [ color | rgb ]" << endl;
  cout << "       demo any  [ color | rgb ]" <<
       endl;
  cout << "Eg." << endl;
  cout << "   demo any FF1493 " << endl;
  cout << "   demo YRGBLED1-123456 red" << endl;
  u64 now = YAPI::GetTickCount();
  while (YAPI::GetTickCount() - now < 3000) {
    // wait 3 sec to show the message
  }
  exit(1);
}

int main(int argc, const char * argv[])
{
  string       errmsg;
  string       target;
  YColorLed    *led1;
  YColorLed    *led2;
  string       color_str;
  unsigned int color;

  if(argc < 3) {
    usage();
  }
  target     = (string) argv[1];
  color_str  = (string) argv[2];
  if (color_str == "red")
    color = 0xFF0000;
  else if ( color_str == "green")
    color = 0x00FF00;
  else if (color_str == "blue")
    color = 0x0000FF;
  else
    color = (unsigned int)strtoul(color_str.c_str(), NULL, 16);

  // Setup the API to use local USB devices
  if (YAPI::RegisterHub("usb", errmsg) != YAPI::SUCCESS) {
    cerr << "RegisterHub error: " << errmsg << endl;
    return 1;
  }

  if (target == "any") {
    led1 =  YColorLed::FirstColorLed();
    if (led1 == NULL) {
      cout << "No module connected (check USB cable)" << endl;
      return 1;
    }
    led2 =  led1->nextColorLed();
  } else {
    led1 =  YColorLed::FindColorLed(target + ".colorLed1");
    led2 =  YColorLed::FindColorLed(target + ".colorLed2");
  }

  if (led1->isOnline()) {
    led1->set_rgbColor(color);  // immediate switch
    led2->rgbMove(color, 1000); // smooth transition
  } else {
    cout << "Module not connected (check identification and USB cable)" << endl;
  }
  YAPI::FreeAPI();

  return 0;
}
