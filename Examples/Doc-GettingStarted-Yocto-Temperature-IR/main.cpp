/*********************************************************************
 *
 *  $Id: svn_id $
 *
 *  An example that show how to use a  Yocto-Temperature-IR
 *
 *  You can find more information on our web site:
 *   Yocto-Temperature-IR documentation:
 *      https://www.yoctopuce.com/EN/products/yocto-temperature-ir/doc.html
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
  u64 now = yGetTickCount();
  while (yGetTickCount() - now < 3000) {
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
  if (yRegisterHub("usb", errmsg) != YAPI_SUCCESS) {
    cerr << "RegisterHub error: " << errmsg << endl;
    return 1;
  }

  if (target == "any") {
    tsensor = yFirstTemperature();
    if (tsensor == NULL) {
      cout << "No module connected (check USB cable)" << endl;
      return 1;
    }
  } else {
    tsensor = yFindTemperature(target + ".temperature1");
  }

  YTemperature *t1 = yFindTemperature(tsensor->get_module()->get_serialNumber() +
                                      ".temperature1");
  YTemperature *t2 = yFindTemperature(tsensor->get_module()->get_serialNumber() +
                                      ".temperature2");

  while (1) {
    if (!t1->isOnline() || !t2->isOnline()) {
      cout << "Module not connected (check identification and USB cable)";
      break;
    }
    cout << "Ambiant temperature  : " << t1->get_currentValue() << "  deg C" << endl;
    cout << "Infrared temperature : " << t2->get_currentValue() << "  deg C" << endl;
    cout << "  (press Ctrl-C to exit)" << endl;
    ySleep(1000, errmsg);
  };
  yFreeAPI();

  return 0;
}
