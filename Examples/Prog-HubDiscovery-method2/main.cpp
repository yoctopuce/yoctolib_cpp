#include <iostream>

#include "yocto_api.h"
#include "yocto_anbutton.h"

using namespace std;

// called each time a new device (networked or not) is detected
static void arrivalCallback(YModule *dev)
{
  bool isAHub = false;
  // iterate on all functions on the module and find the ports
  int fctCount =  dev->functionCount();
  for (int i = 0; i < fctCount; i++) {
    // retreive the hardware name of the ith function
    string fctHwdName = dev->functionId(i);
    if (fctHwdName.length() > 7 &&  fctHwdName.substr(0, 7) == "hubPort") {
      // the device contains a  hubPortx function, so it's a hub
      if (!isAHub) {
        cout << "hub found : " << dev->get_friendlyName() << endl;
        isAHub = true;
      }
      // The port logical name is always the serial#
      // of the connected device
      string deviceid = dev->functionName(i);
      cout << " " << fctHwdName << " : " << deviceid << endl;
    }

  }
}

int main(int argc, const char * argv[])
{
  string errmsg;

  cout << "Waiting for hubs to signal themselves..." << endl;

  if (YAPI::RegisterHub("net", errmsg) != YAPI::SUCCESS) {
    cerr << "RegisterHub error : " << errmsg << endl;
    return 1;
  }

  // each time a new device is connected/discovered
  // arrivalCallback will be called.
  YAPI::RegisterDeviceArrivalCallback(arrivalCallback);

  // wait for 30 seconds, doing nothing.
  for (int i = 0; i < 30; i++) {
    YAPI::UpdateDeviceList(errmsg); // traps plug/unplug events
    YAPI::Sleep(1000, errmsg);   // traps others events
  }
}
