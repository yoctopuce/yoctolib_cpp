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
  u64 now = yGetTickCount();
  while (yGetTickCount() - now < 3000) {
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
  if (yRegisterHub("usb", errmsg) != YAPI_SUCCESS) {
    cerr << "RegisterHub error: " << errmsg << endl;
    return 1;
  }

  if (target == "any") {
    YVoltageOutput *vout = yFirstVoltageOutput();
    if (vout == NULL) {
      cout << "No module connected (check USB cable)" << endl;
      return 1;
    }
    target = vout->module()->get_serialNumber();
  }
  vout1 = yFindVoltageOutput(target + ".voltageOutput1");
  vout2 = yFindVoltageOutput(target + ".voltageOutput2");

  if (vout1->isOnline()) {
    // output 1 : immediate change
    vout1->set_currentVoltage(voltage);
    // output 2 : smooth change
    vout2->voltageMove(voltage, 3000);
  } else {
    cout << "Module not connected (check identification and USB cable)" << endl;
  }
  yFreeAPI();

  return 0;
}
