/*********************************************************************
 *
 *  $Id: main.cpp 38820 2019-12-18 18:01:14Z seb $
 *
 *  An example that show how to use a  Yocto-Volt
 *
 *  You can find more information on our web site:
 *   Yocto-Volt documentation:
 *      https://www.yoctopuce.com/EN/products/yocto-volt/doc.html
 *   C++ API Reference:
 *      https://www.yoctopuce.com/EN/doc/reference/yoctolib-cpp-EN.html
 *
 *********************************************************************/

#include "yocto_api.h"
#include "yocto_voltage.h"
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
  YVoltage    *sensor;
  YVoltage    *sensorAC;
  YVoltage    *sensorDC;
  YModule     *m;

  if (argc < 2) {
    usage();
  }
  target = (string) argv[1];

  YAPI::DisableExceptions();

  // Setup the API to use local USB devices
  if (YAPI::RegisterHub("usb", errmsg) != YAPI::SUCCESS) {
    cerr << "RegisterHub error: " << errmsg << endl;
    return 1;
  }

  if (target == "any") {
    // retreive any voltage sensor (can be AC or DC)
    sensor = YVoltage::FirstVoltage();
    if (sensor == NULL) {
      cerr << "No module connected (Check cable)" << endl;
      exit(1);
    }
  } else {
    sensor = YVoltage::FindVoltage(target + ".voltage1");
  }

  // we need to retreive both DC and AC voltage from the device.
  if (sensor->isOnline())  {
    m = sensor->get_module();
    sensorDC = YVoltage::FindVoltage(m->get_serialNumber() + ".voltage1");
    sensorAC = YVoltage::FindVoltage(m->get_serialNumber() + ".voltage2");
  } else {
    cerr << "No module connected (Check cable)" << endl;
    exit(1);
  }
  while(1) {
    if (!sensorDC->isOnline())  {
      cout << "Module disconnected" << endl;
      break;
    }
    cout << "Voltage,  DC : " << sensorDC->get_currentValue() << " v";
    cout << "   AC : " << sensorAC->get_currentValue() << " v";
    cout << "  (press Ctrl-C to exit)" << endl;
    YAPI::Sleep(1000, errmsg);
  };
  YAPI::FreeAPI();

  return 0;
}
