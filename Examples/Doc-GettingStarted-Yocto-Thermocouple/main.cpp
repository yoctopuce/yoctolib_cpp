/*********************************************************************
 *
 *  $Id: main.cpp 38864 2019-12-19 14:50:03Z seb $
 *
 *  An example that show how to use a  Yocto-Thermocouple
 *
 *  You can find more information on our web site:
 *   Yocto-Thermocouple documentation:
 *      https://www.yoctopuce.com/EN/products/yocto-thermocouple/doc.html
 *   C++ API Reference:
 *      https://www.yoctopuce.com/EN/doc/reference/yoctolib-cpp-EN.html
 *
 *********************************************************************/

#include "yocto_api.h"
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
  string errmsg, target;
  YTemperature *tsensor;

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
    tsensor = YTemperature::FirstTemperature();
    if (tsensor == NULL) {
      cout << "No module connected (check USB cable)" << endl;
      return 1;
    }
  } else {
    tsensor = YTemperature::FindTemperature(target + ".temperature1");
  }
  string serial = tsensor->get_module()->get_serialNumber();
  YTemperature *t1 = YTemperature::FindTemperature(serial + ".temperature1");
  YTemperature *t2 = YTemperature::FindTemperature(serial + ".temperature2");

  while (1) {
    if (!t1->isOnline() || !t2->isOnline()) {
      cout << "Module not connected (check identification and USB cable)";
      break;
    }
    cout << "Channel 1 : " << t1->get_currentValue() << "  deg C" << endl;
    cout << "Channel 2 : " << t2->get_currentValue() << "  deg C" << endl;
    cout << "  (press Ctrl-C to exit)" << endl;
    YAPI::Sleep(1000, errmsg);
  };
  YAPI::FreeAPI();

  return 0;
}
