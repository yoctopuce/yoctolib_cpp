/*********************************************************************
 *
 *  $Id: main.cpp 38820 2019-12-18 18:01:14Z seb $
 *
 *  An example that show how to use a  Yocto-RangeFinder
 *
 *  You can find more information on our web site:
 *   Yocto-RangeFinder documentation:
 *      https://www.yoctopuce.com/EN/products/yocto-rangefinder/doc.html
 *   C++ API Reference:
 *      https://www.yoctopuce.com/EN/doc/reference/yoctolib-cpp-EN.html
 *
 *********************************************************************/

#include "yocto_api.h"
#include "yocto_rangefinder.h"
#include "yocto_lightsensor.h"
#include "yocto_temperature.h"

#include <iostream>
#include <stdlib.h>

using namespace std;

static void usage(void)
{
  cout << "usage: demo <serial_number> " << endl;
  cout << "       demo <logical_name>" << endl;
  cout << "       demo any" << endl;
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
  YRangeFinder *rf;
  YLightSensor *ir;
  YTemperature *tmp;

  if (argc < 2) {
    usage();
  }
  target = (string) argv[1];

  // Setup the API to use local USB devices
  if (YAPI::RegisterHub("usb", errmsg) != YAPI::SUCCESS) {
    cerr << "RegisterHub error: " << errmsg << endl;
    return 1;
  }

  if (target == "any") {
    rf = YRangeFinder::FirstRangeFinder();
    if (rf == NULL) {
      cout << "No module connected (check USB cable)" << endl;
      return 1;
    }
    target = rf->get_module()->get_serialNumber();
  } else {
    rf = YRangeFinder::FindRangeFinder(target + ".rangeFinder1");
  }
  ir = YLightSensor::FindLightSensor(target + ".lightSensor1");
  tmp = YTemperature::FindTemperature(target + ".temperature1");

  while(1) {
    if (!rf->isOnline()) {
      cout << "Module not connected (check identification and USB cable)";
      break;
    }
    cout << "Distance    : " << rf->get_currentValue() << endl;
    cout << "Ambient IR  : " << ir->get_currentValue() << endl;
    cout << "Temperature : " << tmp->get_currentValue() << endl;
    cout << "  (press Ctrl-C to exit)" << endl;
    YAPI::Sleep(1000, errmsg);
  };
  YAPI::FreeAPI();

  return 0;
}
