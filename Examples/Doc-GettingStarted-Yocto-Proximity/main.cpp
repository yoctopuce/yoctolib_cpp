/*********************************************************************
 *
 *  $Id: main.cpp 59576 2024-02-29 09:36:30Z seb $
 *
 *  An example that show how to use a  Yocto-Proximity
 *
 *  You can find more information on our web site:
 *   Yocto-Proximity documentation:
 *      https://www.yoctopuce.com/EN/products/yocto-proximity/doc.html
 *   C++ V2 API Reference:
 *      https://www.yoctopuce.com/EN/doc/reference/yoctolib-cpp-EN.html
 *
 *********************************************************************/

#include "yocto_api.h"
#include "yocto_lightsensor.h"
#include "yocto_proximity.h"

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
  YLightSensor *al, *ir;
  YProximity   *p;

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
    p = YProximity::FirstProximity();
    if (p == NULL) {
      cout << "No module connected (check USB cable)" << endl;
      return 1;
    }
    target = p->get_module()->get_serialNumber();
  } else {
    p = YProximity::FindProximity(target + ".proximity1");
  }
  al = YLightSensor::FindLightSensor(target + ".lightSensor1");
  ir = YLightSensor::FindLightSensor(target + ".lightSensor2");

  while(1) {
    if (!p->isOnline()) {
      cout << "Module not connected (check identification and USB cable)";
      break;
    }

    cout << " Proximity: " << p->get_currentValue();
    cout << " Ambient: " << al->get_currentValue();
    cout << " IR: " << ir->get_currentValue();

    cout << "  (press Ctrl-C to exit)" << endl;
    YAPI::Sleep(1000, errmsg);
  };
  YAPI::FreeAPI();

  return 0;
}
