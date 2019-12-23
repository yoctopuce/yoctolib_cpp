/*********************************************************************
 *
 *  $Id: main.cpp 38820 2019-12-18 18:01:14Z seb $
 *
 *  An example that show how to use a  Yocto-Demo
 *
 *  You can find more information on our web site:
 *   Yocto-Demo documentation:
 *      https://www.yoctopuce.com/EN/products/yocto-demo/doc.html
 *   C++ API Reference:
 *      https://www.yoctopuce.com/EN/doc/reference/yoctolib-cpp-EN.html
 *
 *********************************************************************/

#include "yocto_api.h"
#include "yocto_led.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

static void usage(void)
{
  cout << "usage: demo <serial_number>  [ on | off ]" << endl;
  cout << "       demo <logical_name> [ on | off ]" << endl;
  cout << "       demo any [ on | off ] (use any discovered device)" << endl;
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
  YLed   *led;
  string on_off;

  if(argc < 3) {
    usage();
  }
  target    = (string) argv[1];
  on_off = (string) argv[2];

  // Setup the API to use local USB devices
  if(YAPI::RegisterHub("usb", errmsg) != YAPI::SUCCESS) {
    cerr << "RegisterHub error: " << errmsg << endl;
    return 1;
  }

  if(target == "any") {
    led =  YLed::FirstLed();
  } else {
    led =  YLed::FindLed(target + ".led");
  }
  if (led && led->isOnline()) {
    led->set_power(on_off == "on" ? Y_POWER_ON : Y_POWER_OFF);
  } else {
    cout << "Module not connected (check identification and USB cable)" << endl;
  }
  YAPI::FreeAPI();

  return 0;
}
