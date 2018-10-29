/*********************************************************************
 *
 *  $Id: main.cpp 32716 2018-10-19 15:54:48Z seb $
 *
 *  An example that show how to use a  Yocto-CO2
 *
 *  You can find more information on our web site:
 *   Yocto-CO2 documentation:
 *      https://www.yoctopuce.com/EN/products/yocto-co2/doc.html
 *   C++ API Reference:
 *      https://www.yoctopuce.com/EN/doc/reference/yoctolib-cpp-EN.html
 *
 *********************************************************************/

#include "yocto_api.h"
#include "yocto_carbondioxide.h"
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
  YCarbonDioxide *co2sensor;

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
    co2sensor = yFirstCarbonDioxide();
    if (co2sensor == NULL) {
      cout << "No module connected (check USB cable)" << endl;
      return 1;
    }
  } else {
    co2sensor = yFindCarbonDioxide(target + ".carbonDioxide");
  }

  while (1) {
    if (!co2sensor->isOnline()) {
      cout << "Module not connected (check identification and USB cable)";
      break;
    }
    cout << "CO2: " << co2sensor->get_currentValue() << " ppm" << endl;
    cout << "  (press Ctrl-C to exit)" << endl;
    ySleep(1000, errmsg);
  };

  yFreeAPI();
  return 0;
}
