/*********************************************************************
 *
 *  $Id: main.cpp 59576 2024-02-29 09:36:30Z seb $
 *
 *  An example that show how to use a  Yocto-MaxiBridge
 *
 *  You can find more information on our web site:
 *   Yocto-MaxiBridge documentation:
 *      https://www.yoctopuce.com/EN/products/yocto-maxibridge/doc.html
 *   C++ V2 API Reference:
 *      https://www.yoctopuce.com/EN/doc/reference/yoctolib-cpp-EN.html
 *
 *********************************************************************/

#include "yocto_api.h"
#include "yocto_multicellweighscale.h"
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

int main(int argc, const char* argv[])
{
  string errmsg, target;
  YMultiCellWeighScale* sensor;

  if (argc < 2) {
    usage();
  }
  target = (string)argv[1];

  // Setup the API to use local USB devices
  if (YAPI::RegisterHub("usb", errmsg) != YAPI::SUCCESS) {
    cerr << "RegisterHub error: " << errmsg << endl;
    return 1;
  }

  if (target == "any") {
    sensor = YMultiCellWeighScale::FirstMultiCellWeighScale();
    if (sensor == NULL) {
      cout << "No module connected (check USB cable)" << endl;
      return 1;
    }
  } else {
    sensor = YMultiCellWeighScale::FindMultiCellWeighScale(
               target + ".multiCellWeighScale");
  }

  string unit;

  if (sensor->isOnline()) {
    // On startup, enable excitation and tare weigh scale
    cout << "Resetting tare weight..." << endl;
    sensor->set_excitation(YMultiCellWeighScale::EXCITATION_AC);
    YAPI::Sleep(3000, errmsg);
    sensor->tare();
    unit = sensor->get_unit();
  }

  // Show measured weight continuously
  while (sensor->isOnline()) {
    cout << "Weight : " << sensor->get_currentValue() << unit;
    cout << "  (press Ctrl-C to exit)" << endl;
    YAPI::Sleep(1000, errmsg);
  };
  YAPI::FreeAPI();
  cout << "Module not connected (check identification and USB cable)";
  return 0;
}
