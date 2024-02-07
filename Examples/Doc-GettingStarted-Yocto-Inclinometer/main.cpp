/*********************************************************************
 *
 *  $Id: main.cpp 44664 2021-04-23 09:45:05Z seb $
 *
 *  An example that show how to use a  Yocto-Inclinometer
 *
 *  You can find more information on our web site:
 *   Yocto-Inclinometer documentation:
 *      https://www.yoctopuce.com/EN/products/yocto-inclinometer/doc.html
 *   C++ V2 API Reference:
 *      https://www.yoctopuce.com/EN/doc/reference/yoctolib-cpp-EN.html
 *
 *********************************************************************/

#include "yocto_api.h"
#include "yocto_tilt.h"
#include <iostream>
#include <stdlib.h>
#include <iostream>
#include <iomanip>

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
  YTilt  *anytilt, *tilt1, *tilt2 , *tilt3;

  if(argc < 2) {
    usage();
  }
  target = (string) argv[1];

  // Setup the API to use local USB devices
  if(YAPI::RegisterHub("usb", errmsg) != YAPI::SUCCESS) {
    cerr << "RegisterHub error: " << errmsg << endl;
    return 1;
  }

  // try to find a valid serial number
  if(target == "any") {
    anytilt = YTilt::FirstTilt();
    if (anytilt == NULL) {
      cout << "No module connected (check USB cable)" << endl;
      return 1;
    }
  } else {
    anytilt = YTilt::FindTilt(target + ".tilt1");
    if (!anytilt->isOnline()) {
      cout << "Module not connected (check identification and USB cable)" << endl;
      return 1;
    }
  }
  string serial = anytilt->get_module()->get_serialNumber();

  // retrieve all sensors on the device matching the serial
  tilt1 = YTilt::FindTilt(serial + ".tilt1");
  tilt2 = YTilt::FindTilt(serial + ".tilt2");
  tilt3 = YTilt::FindTilt(serial + ".tilt3");

  int count = 0;

  while(1) {
    if(!tilt1->isOnline()) {
      cout << "device disconnected";
      break;
    }
    if ((count % 10) == 0) {
      cout << "tilt1\ttilt2\ttilt3" << endl;
    }
    cout <<  std::setprecision(2) << std::setw(8)
         << tilt1->get_currentValue() << "\t"
         << tilt2->get_currentValue() << "\t"
         << tilt3->get_currentValue()  << endl;
    count++;
    YAPI::Sleep(250, errmsg);
  }
  YAPI::FreeAPI();
  return 0;
}
