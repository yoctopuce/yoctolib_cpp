#include <iostream>

#include "yocto_api.h"
#include "yocto_anbutton.h"

using namespace std;

static void anButtonValueChangeCallBack(YAnButton *fct, const string& value)
{
  cout << fct->get_hardwareId() << " : " << value << " (new value)" << endl;
}

static void sensorValueChangeCallBack(YSensor *fct, const string& value)
{
  cout << fct->get_hardwareId() << " : " << value << " " << fct->get_unit() << " (new value)" << endl;
}

static void sensorTimedReportCallBack(YSensor *fct, YMeasure measure)
{
  cout << fct->get_hardwareId() << " : " << measure.get_averageValue() << " " << fct->get_unit() << " (timed report)" << endl;
}

static void deviceLog(YModule *module, const string& logline)
{
  cout << "log:" << module->get_hardwareId() << ":" << logline << endl;
}

static void deviceArrival(YModule *m)
{
  string serial = m->get_serialNumber();
  cout << "Device arrival : " << serial << endl;
  m->registerLogCallback(deviceLog);
  string hardwareId;

  // First solution: look for a specific type of function (eg. anButton)
  int fctcount = m->functionCount();
  for (int i = 0; i < fctcount; i++) {
    hardwareId = serial + "." + m->functionId(i);
    if (hardwareId.find(".anButton") != string::npos) {
      cout << "- " << hardwareId << endl;
      YAnButton *bt = YAnButton::FindAnButton(hardwareId);
      bt->registerValueCallback(anButtonValueChangeCallBack);
    }
  }

  // Alternate solution: register any kind of sensor on the device
  YSensor *sensor = YSensor::FirstSensor();
  while(sensor) {
    if(sensor->get_module()->get_serialNumber() == serial) {
      hardwareId = sensor->get_hardwareId();
      cout << "- " << hardwareId << endl;
      sensor->registerValueCallback(sensorValueChangeCallBack);
      sensor->registerTimedReportCallback(sensorTimedReportCallBack);
    }
    sensor = sensor->nextSensor();
  }
}

static void deviceRemoval(YModule *m)
{
  cout << "Device removal : " << m->get_serialNumber() << endl;
}

static void log(const string& val)
{
  cout << val;
}

int main(int argc, const char * argv[])
{
  string errmsg;

  YAPI::RegisterLogFunction(log);
  YAPI::RegisterDeviceArrivalCallback(deviceArrival);
  YAPI::RegisterDeviceRemovalCallback(deviceRemoval);
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
