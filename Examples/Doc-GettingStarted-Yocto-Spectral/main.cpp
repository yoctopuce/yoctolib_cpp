/*********************************************************************
 *
 *  $Id: main.cpp 63864 2024-12-23 11:55:34Z tiago $
 *
 *  An example that show how to use a  Yocto-Spectral
 *
 *  You can find more information on our web site:
 *   Yocto-Spectral documentation:
 *      https://www.yoctopuce.com/EN/products/yocto-spectral/doc.html
 *   C++ V2 API Reference:
 *      https://www.yoctopuce.com/EN/doc/reference/yoctolib-cpp-EN.html
 *
 *********************************************************************/

#include "yocto_api.h"
#include "yocto_spectralsensor.h"
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
  YSpectralSensor *spectralSensor;

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
      spectralSensor = YSpectralSensor::FirstSpectralSensor();
    if (spectralSensor == NULL) {
      cerr << "No module connected (check USB cable)" << endl;
      return 1;
    }
  } else {
    target = (string) argv[1];
    spectralSensor = YSpectralSensor::FindSpectralSensor(target + ".spectralSensor");
    if (!spectralSensor->isOnline()) {
      cerr << "Module not connected (check USB cable)" << endl;
      return 1;
    }
  }
  // sample code configure Yocto-Spectral
  spectralSensor->set_gain(6);
  spectralSensor->set_integrationTime(150);
  spectralSensor->set_ledCurrent(6);

  cout << "Near color: " << spectralSensor->get_nearSimpleColor() << endl;
  //cout << "Color HEX: " << spectralSensor->get_estimatedRGB() << endl;
  u64 now = YAPI::GetTickCount();
  while (YAPI::GetTickCount() - now < 3000) {
    // wait 3 sec to show the message
  }

  YAPI::FreeAPI();
  return 0;
}
