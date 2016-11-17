#include <iostream>
#include <set>
#include "yocto_api.h"

using namespace std;


static std::set<string> KnownHubs;

static void HubDiscovered(const string& serial, const string& url)
{
  // The call-back can be called several times for the same hub
  // (the discovery technique is based on a periodic broadcast)
  // So we use a dictionnary to avoid duplicates
  if (KnownHubs.find(serial) != KnownHubs.end()) return;

  cout << "hub found: " << serial << " (" << url << ")" << endl;

  // connect to the hub
  string msg;
  YAPI::RegisterHub(url, msg);

  //  find the hub module
  YModule *hub = YModule::FindModule(serial);

  // iterate on all functions on the module and find the ports
  int fctCount =  hub->functionCount();
  for (int i = 0; i < fctCount; i++) {
    // retreive the hardware name of the ith function
    string fctHwdName = hub->functionId(i);
    if (fctHwdName.length() > 7 && fctHwdName.substr(0, 7) == "hubPort") {
      // The port logical name is always the serial#
      // of the connected device
      string deviceid =  hub->functionName(i);
      cout << "  " << fctHwdName << " : " << deviceid << endl;
    }
  }
  // add the hub to the dictionnary so we won't have to
  // process is again.
  KnownHubs.insert(serial);

  // disconnect from the hub
  YAPI::UnregisterHub(url);
}

int main(int argc, const char * argv[])
{
  string errmsg;

  cout << "Waiting for hubs to signal themselves..." << endl;

  // register the callback: HubDiscovered will be
  // invoked each time a hub signals its presence
  YAPI::RegisterHubDiscoveryCallback(HubDiscovered);

  // wait for 30 seconds, doing nothing.
  for (int i = 0 ; i < 30; i++) {
    YAPI::UpdateDeviceList(errmsg);
    YAPI::Sleep(1000, errmsg);
  }
}
