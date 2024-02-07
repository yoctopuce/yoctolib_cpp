/*********************************************************************
 *
 *  $Id: svn_id $
 *
 *  An example that show how to use a  Yocto-SDI12
 *
 *  You can find more information on our web site:
 *   Yocto-SDI12 documentation:
 *      https://www.yoctopuce.com/EN/products/yocto-sdi12/doc.html
 *   C++ V2 API Reference:
 *      https://www.yoctopuce.com/EN/doc/reference/yoctolib-cpp-EN.html
 *
 *********************************************************************/

#include "yocto_api.h"
#include "yocto_sdi12port.h"
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
  YSdi12Port *sdi12Port;

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
    sdi12Port = YSdi12Port::FirstSdi12Port();
    if (sdi12Port == NULL) {
      cerr << "No module connected (check USB cable)" << endl;
      return 1;
    }
  } else {
    target = (string) argv[1];
    sdi12Port = YSdi12Port::FindSdi12Port(target + ".sdi12Port");
    if (!sdi12Port->isOnline()) {
      cerr << "Module not connected (check USB cable)" << endl;
      return 1;
    }
  }
  sdi12Port->reset();

  YSdi12Sensor singleSensor = sdi12Port->discoverSingleSensor();
  printf("%-30s %s \n", "Sensor address :", singleSensor.get_sensorAddress().c_str());
  printf("%-30s %s \n", "Sensor SDI-12 compatibility : ",
         singleSensor.get_sensorProtocol().c_str());
  printf("%-30s %s \n", "Sensor company name : ", singleSensor.get_sensorVendor().c_str());
  printf("%-30s %s \n", "Sensor model number : ", singleSensor.get_sensorModel().c_str());
  printf("%-30s %s \n", "Sensor version : ", singleSensor.get_sensorVersion().c_str());
  printf("%-30s %s \n", "Sensor serial number : ", singleSensor.get_sensorSerial().c_str());

  // of the Yocto-SDI12 as well if used
  vector<double> valSensor = sdi12Port->readSensor(singleSensor.get_sensorAddress(), "M",
                             5000);
  for (unsigned i = 0; i < valSensor.size(); i++) {
    if (singleSensor.get_measureCount() > 1) {
      printf("%s %-8.2f%-8s (%s) \n", singleSensor.get_measureSymbol(i).c_str(), valSensor[i],
             singleSensor.get_measureUnit(i).c_str(), singleSensor.get_measureDescription(i).c_str());
    } else {
      printf("%.2f \n", valSensor[i]);
    }

  }

  YAPI::FreeAPI();
  return 0;
}
