/*********************************************************************
 *
 *  $Id: main.cpp 38820 2019-12-18 18:01:14Z seb $
 *
 *  An example that show how to use a  Yocto-MaxiThermistor
 *
 *  You can find more information on our web site:
 *   Yocto-MaxiThermistor documentation:
 *      https://www.yoctopuce.com/EN/products/yocto-maxithermistor/doc.html
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
  string errmsg, target, serial;
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

  serial = tsensor->get_module()->get_serialNumber();
  YTemperature *t1 = YTemperature::FindTemperature(serial + ".temperature1");
  YTemperature *t2 = YTemperature::FindTemperature(serial + ".temperature2");
  YTemperature *t3 = YTemperature::FindTemperature(serial + ".temperature3");
  YTemperature *t4 = YTemperature::FindTemperature(serial + ".temperature4");
  YTemperature *t5 = YTemperature::FindTemperature(serial + ".temperature5");
  YTemperature *t6 = YTemperature::FindTemperature(serial + ".temperature6");

  while (1) {
    if (!t1->isOnline() || !t2->isOnline()) {
      cout << "Module not connected (check identification and USB cable)";
      break;
    }

    cout << " | 1: " << t1->get_currentValue();
    cout << " | 2: " << t2->get_currentValue();
    cout << " | 3: " << t3->get_currentValue();
    cout << " | 4: " << t4->get_currentValue();
    cout << " | 5: " << t5->get_currentValue();
    cout << " | 6: " << t6->get_currentValue();

    cout <<  " |  deg C |" << endl;

    YAPI::Sleep(1000, errmsg);
  };
  YAPI::FreeAPI();

  return 0;
}
