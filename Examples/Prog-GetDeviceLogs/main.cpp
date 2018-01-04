#include <iostream>

#include "yocto_api.h"
#include "yocto_anbutton.h"

using namespace std;


static void log(const string &log)
{
  cout << "YAPI : " << log << endl;
}


static void logfun(YModule *m, const string &log)
{
  cout << m->get_serialNumber() << " : " << log << endl;
}

static void deviceArrival(YModule *m)
{
  string serial = m->get_serialNumber();
  cout << "Device arrival : " << serial << endl;
  m->registerLogCallback(logfun);
}


int main(int argc, const char * argv[])
{
  string errmsg;

  YAPI::RegisterLogFunction(log);
  YAPI::RegisterDeviceArrivalCallback(deviceArrival);
  YAPI::DisableExceptions();

  if (YAPI::RegisterHub("usb", errmsg) != YAPI::SUCCESS) {
    cerr << "RegisterHub error : " << errmsg << endl;
    return 1;
  }
  cout << "Hit Ctrl-C to Stop " << endl;

  while (true) {
    YAPI::UpdateDeviceList(errmsg); // traps plug/unplug events
    YAPI::Sleep(500, errmsg);   // traps others events
  }
}
