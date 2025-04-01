/*********************************************************************
 *
 *  $Id: main.cpp 65287 2025-03-24 13:18:15Z seb $
 *
 *  An example that shows how to use a  Yocto-Spectral
 *
 *  You can find more information on our web site:
 *   Yocto-Spectral documentation:
 *      https://www.yoctopuce.com/EN/products/yocto-spectral/doc.html
 *   C++ API Reference:
 *      https://www.yoctopuce.com/EN/doc/reference/yoctolib-cpp-EN.html
 *
 *********************************************************************/

#include "yocto_api.h"
#include "yocto_colorsensor.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

static void usage(void)
{
  cout << "usage: demo <serial_number>" << endl;
  cout << "       demo <logical_name>" << endl;
  cout << "       demo any           (use any discovered device)" << endl;
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
  YColorSensor *colorSensor;

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
      colorSensor = YColorSensor::FirstColorSensor();
    if (colorSensor == NULL) {
      cerr << "No module connected (check USB cable)" << endl;
      return 1;
    }
  } else {
    target = (string) argv[1];
    colorSensor = YColorSensor::FindColorSensor(target + ".colorSensor");
    if (!colorSensor->isOnline()) {
      cerr << "Module not connected (check USB cable)" << endl;
      return 1;
    }
  }
  // sample code configure Yocto-Spectral
  colorSensor->set_workingMode(Y_WORKINGMODE_AUTO); // Set Working Mode Auto
  colorSensor->set_estimationModel(Y_ESTIMATIONMODEL_REFLECTION); // Set Prediction Model Reflexion
  cout << "Press Enter to stop..." << endl;
  while (colorSensor->isOnline()) {
	  cout << "Near color: " << colorSensor->get_nearSimpleColor() << endl;
	  cout << "RGB HEX: " << hex << colorSensor->get_estimatedRGB() << endl;
	  cout << "------------------------------------" << endl;
      cout << "  (press Ctrl-C to exit)" << endl;
      YAPI::Sleep(2000, errmsg);
  }

  YAPI::FreeAPI();
  return 0;
}
