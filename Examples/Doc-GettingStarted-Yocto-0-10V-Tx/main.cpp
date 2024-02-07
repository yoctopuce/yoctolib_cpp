/*********************************************************************
 *
 *  $Id: main.cpp 42585 2020-11-19 11:10:49Z seb $
 *
 *  An example that show how to use a  Yocto-0-10V-Tx
 *
 *  You can find more information on our web site:
 *   Yocto-0-10V-Tx documentation:
 *      https://www.yoctopuce.com/EN/products/yocto-0-10v-tx/doc.html
 *   C++ API Reference:
 *      https://www.yoctopuce.com/EN/doc/reference/yoctolib-cpp-EN.html
 *
 *********************************************************************/

#include "yocto_api.h"
#include "yocto_voltageoutput.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

static void usage(void)
{
  cout << "usage: demo <serial_number> <value>" << endl;
  cout << "       demo <logical_name>  <value>" << endl;
  cout << "       demo any  <value>   (use any discovered device)"
       << endl;
  cout << "       <value>: floating point number between 0.0 and 10.000" << endl;
  u64 now = YAPI::GetTickCount();
  while (YAPI::GetTickCount() - now < 3000) {
    // wait 3 sec to show the message
  }
  exit(1);
}

int main(int argc, const char * argv[])
{
  string errmsg;
  string target;
  YVoltageOutput *vout1;
  YVoltageOutput *vout2;
  double voltage;

  if (argc < 4) {
    usage();
  }
  target = (string) argv[1];
  voltage = atof(argv[2]);

  // Setup the API to use local USB devices
  if (YAPI::RegisterHub("usb", errmsg) != YAPI::SUCCESS) {
    cerr << "RegisterHub error: " << errmsg << endl;
    return 1;
  }

  if (target == "any") {
    YVoltageOutput *vout = YVoltageOutput::FirstVoltageOutput();
    if (vout == NULL) {
      cout << "No module connected (check USB cable)" << endl;
      return 1;
    }
    target = vout->module()->get_serialNumber();
  }
  vout1 = YVoltageOutput::FindVoltageOutput(target + ".voltageOutput1");
  vout2 = YVoltageOutput::FindVoltageOutput(target + ".voltageOutput2");

  if (vout1->isOnline()) {
    // output 1 : immediate change
    vout1->set_currentVoltage(voltage);
    // output 2 : smooth change
    vout2->voltageMove(voltage, 3000);
  } else {
    cout << "Module not connected (check identification and USB cable)" << endl;
  }
  YAPI::FreeAPI();

  return 0;
}
