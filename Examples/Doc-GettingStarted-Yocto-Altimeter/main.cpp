/*********************************************************************
 *
 *  $Id: main.cpp 38820 2019-12-18 18:01:14Z seb $
 *
 *  An example that show how to use a  Yocto-Altimeter
 *
 *  You can find more information on our web site:
 *   Yocto-Altimeter documentation:
 *      https://www.yoctopuce.com/EN/products/yocto-altimeter/doc.html
 *   C++ API Reference:
 *      https://www.yoctopuce.com/EN/doc/reference/yoctolib-cpp-EN.html
 *
 *********************************************************************/

#include "yocto_api.h"
#include "yocto_altitude.h"
#include "yocto_temperature.h"
#include "yocto_pressure.h"
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
  YAltitude    *asensor;
  YTemperature *tsensor;
  YPressure    *psensor;

  if(argc < 2) {
    usage();
  }
  target = (string) argv[1];

  // Setup the API to use local USB devices
  if(YAPI::RegisterHub("usb", errmsg) != YAPI::SUCCESS) {
    cerr << "RegisterHub error: " << errmsg << endl;
    return 1;
  }

  if(target == "any") {
    asensor = YAltitude::FirstAltitude();
    tsensor = YTemperature::FirstTemperature();
    psensor = YPressure::FirstPressure();
    if (asensor == NULL || tsensor == NULL || psensor == NULL) {
      cout << "No module connected (check USB cable)" << endl;
      return 1;
    }
  } else {
    asensor = YAltitude::FindAltitude(target + ".altitude");
    tsensor = YTemperature::FindTemperature(target + ".temperature");
    psensor = YPressure::FindPressure(target + ".pressure");
  }

  while(1) {
    if(!asensor->isOnline()) {
      cout << "Module not connected (check identification and USB cable)";
      break;
    }

    cout << "Current altitude: "    << asensor->get_currentValue() << " m (QNH= " <<
         asensor->get_qnh() << " hPa)" << endl;
    cout << "Current temperature: " << tsensor->get_currentValue() << " °C" << endl;
    cout << "Current pressure: "    << psensor->get_currentValue() << " hPa" << endl;

    cout << "  (press Ctrl-C to exit)" << endl;
    YAPI::Sleep(1000, errmsg);
  };

  YAPI::FreeAPI();
  return 0;
}
