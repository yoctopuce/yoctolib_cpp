#include <iostream>

#include "yocto_api.h"
#include "yocto_temperature.h"
#include "yocto_anbutton.h"
#include "yocto_lightsensor.h"

using namespace std;


static void CustomLog(const string& log)
{
    cout << "LOG:"<<log;
}

static void anButtonChangeCallBack(YAnButton *fct, const string& value)
{
    cout << "Position change    :" << fct->describe() << " = " <<value<<endl;
}

static void temperatureChangeCallBack(YTemperature *fct, const string& value)
{
    cout << "Temperature change :" << fct->describe() << " = " <<value <<"°C"<<endl;
}

static void lightSensorChangeCallBack(YLightSensor *fct, const string& value)
{
    cout << "Light change       :" << fct->describe() << " = " << value << "lx"<<endl;
}


static void deviceArrival(YModule *m)
{
    cout << "Device arrival          : " << m->describe() <<endl;
    int fctcount = m->functionCount();
    string fctName, fctFullName;

    for (int i = 0; i < fctcount; i++) {
        fctName = m->functionId(i);
        fctFullName = m->get_serialNumber() + "." + fctName;

        // register call back for anbuttons
        if (fctName.find("anButton")==0) { 
            YAnButton *bt = YAnButton::FindAnButton(fctFullName);
            //fixme: registerValueCallback should call callback with first value?
            bt->registerValueCallback(anButtonChangeCallBack);
            cout << "Callback registered for : " << fctFullName<<endl;
        }

        // register call back for temperature sensors
        if (fctName.find("temperature")==0) { 
            YTemperature *t = YTemperature::FindTemperature(fctFullName);
            t->registerValueCallback(temperatureChangeCallBack);
            cout << "Callback registered for : " << fctFullName << endl;
        }

        // register call back for light sensors
        if (fctName.find("lightSensor")==0) { 
            YLightSensor *l = YLightSensor::FindLightSensor(fctFullName);
            l->registerValueCallback(lightSensorChangeCallBack);
            cout << "Callback registered for : " << fctFullName<<endl;
        }
        // and so on for other sensor type.....
    }
}

static void deviceRemoval(YModule *m)
{
  cout << "Device removal          : " << m->get_serialNumber()<<endl;
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
        cerr << "RegisterHub error : " << errmsg<<endl;
        return 1;
    }

    cout << "Hit Ctrl-C to Stop " << endl;

    while (true) {
        YAPI::UpdateDeviceList(errmsg); // traps plug/unplug events
        YAPI::Sleep(500, errmsg);   // traps others events
    } 
}
