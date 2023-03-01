#include <stdlib.h>
#include <iostream>

#include "yocto_api.h"
#include "yocto_hubport.h"
#include "main.h"

using namespace std;



YoctoShield::YoctoShield(string serial)
{
  _serial = serial;
}


YoctoShield::~YoctoShield()
{
}


string YoctoShield::getSerial()
{
  return _serial;
}

bool YoctoShield::addSubdevice(string serial)
{
  char buffer[2] = "1";
  for (int i = 1; i <= 4; i++, (*buffer)++) {
    YHubPort *p = YHubPort::FindHubPort(_serial + ".hubPort" + string(buffer));
    if (p->get_logicalName() == serial) {
      _subDevices.push_back(serial);
      return true;
    }

  }
  return false;
}

void YoctoShield::removeSubDevice(string serial)
{
  for (long i = _subDevices.size() - 1; i >= 0; i--) {
    if (_subDevices[i] == serial) {
      _subDevices.erase(_subDevices.begin() + i);
    }
  }
}

void YoctoShield::describe()
{
  cout << "  " << _serial << endl;
  for (unsigned long i = 0; i < _subDevices.size(); i++) {
    cout << "    " << _subDevices[i] << endl;
  }
}

RootDevice::RootDevice(string serialnumber, string url)
{
  _serial = serialnumber;
  _url = url;
}


RootDevice::~RootDevice()
{
}



string RootDevice::getSerial()
{
  return _serial;
}

void RootDevice::addSubDevice(string serial)
{

  if (serial.substr(0, 7) == "YHUBSHL") {
    _shields.push_back(YoctoShield(serial));
  } else {
    // Device to plug look if the device is plugged on a shield
    for (unsigned int i = 0; i < _shields.size(); i++) {
      if (_shields[i].addSubdevice(serial)) {
        return;
      }
    }
    _subDevices.push_back(serial);
  }
}


void RootDevice::removeSubDevice(string serial)
{
  for (long i = _subDevices.size() - 1; i >= 0; i--) {
    if (_subDevices[i] == serial) {
      _subDevices.erase(_subDevices.begin() + i);
    }
  }
  for (long i = _shields.size() - 1; i >= 0; i--) {
    if (_shields[i].getSerial() == serial) {
      _shields.erase(_shields.begin() + 1);
    } else {
      _shields[i].removeSubDevice(serial);
    }
  }
}

void RootDevice::describe()
{
  unsigned int i;
  cout << _serial << " (" << _url << ")" << endl;
  for (i = 0; i < _subDevices.size(); i++) {
    cout << "  " << _subDevices[i] << endl;
  }
  for (i = 0; i < _shields.size(); i++) {
    _shields[i].describe();
  }
}

std::vector<RootDevice*> __rootDevices;

static RootDevice* getYoctoHub(string serial)
{
  unsigned int  i;
  for (i = 0; i < __rootDevices.size(); i++) {
    if (__rootDevices[i]->getSerial() == serial) {
      return __rootDevices[i];
    }
  }
  return NULL;
}

static RootDevice* addRootDevice(string serial, string url)
{
  unsigned int  i;
  for (i = 0; i < __rootDevices.size(); i++) {
    if (__rootDevices[i]->getSerial() == serial) {
      return __rootDevices[i];
    }
  }
  RootDevice *rootDevice = new RootDevice(serial, url);
  __rootDevices.push_back(rootDevice);
  return rootDevice;

}

static void showNetwork()
{
  unsigned int  i;
  cout << "**** device inventory *****" << endl;
  for (i = 0; i < __rootDevices.size(); i++) {
    __rootDevices[i]->describe();
  }
}




static void deviceArrival(YModule *module)
{
  string serial = module->get_serialNumber();
  string parentHub = module->get_parentHub();
  if (parentHub  == "") {
    // root device (
    string url = module->get_url();
    addRootDevice(serial, url);
  } else {
    RootDevice *hub = getYoctoHub(parentHub);
    if (hub != NULL) {
      hub->addSubDevice(serial);
    }
  }
}

static void deviceRemoval(YModule *module)
{
  string serial = module->get_serialNumber();
  for (long i = __rootDevices.size() - 1; i >= 0; i--) {
    __rootDevices[i]->removeSubDevice(serial);
    if (__rootDevices[i]->getSerial() == serial) {
      free(__rootDevices[i]);
      __rootDevices.erase(__rootDevices.begin() + i);
    }
  }
}

int main(int argc, const char * argv[])
{
  string errmsg;

  if (YAPI::RegisterHub("usb", errmsg) != YAPI::SUCCESS) {
    cerr << "RegisterHub error : " << errmsg << endl;
    return 1;
  }
  if (YAPI::RegisterHub("net", errmsg) != YAPI::SUCCESS) {
    cerr << "RegisterHub error : " << errmsg << endl;
    return 1;
  }

  // each time a new device is connected/discovered
  // arrivalCallback will be called.
  YAPI::RegisterDeviceArrivalCallback(deviceArrival);
  // each time a device is disconnected/removed
  // arrivalCallback will be called.
  YAPI::RegisterDeviceRemovalCallback(deviceRemoval);

  cout << "Waiting for hubs to signal themselves..." << endl;
  // wait for 5 seconds, doing nothing.
  //noinspection InfiniteLoopStatement
  while (true) {
    YAPI::UpdateDeviceList(errmsg); // traps plug/unplug events
    YAPI::Sleep(1000, errmsg);   // traps others events
    showNetwork();
  }
}
