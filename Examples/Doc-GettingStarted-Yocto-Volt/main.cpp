#include "yocto_api.h"
#include "yocto_voltage.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

static void usage(void)
{
    cout << "usage: demo <serial_number> " << endl;
    cout << "       demo <logical_name>" << endl;
    cout << "       demo any                 (use any discovered device)" << endl;
    u64 now = yGetTickCount();    // dirty active wait loop 
	while (yGetTickCount()-now<3000);
    exit(1);
}

static void die(const char* errmsg)
{
    cout << errmsg << " (Check cable)"<< endl;
    exit(1);
}

int main(int argc, const char * argv[])
{
    string       errmsg;
    string       target;
    YVoltage    *sensor;
    YVoltage    *sensorAC;
    YVoltage    *sensorDC;
    YModule     *m;

    if (argc < 2) {
        usage();
    }
    target = (string) argv[1];
    
    // Setup the API to use local USB devices
    if (yRegisterHub("usb", errmsg) != YAPI_SUCCESS) {
        cerr << "RegisterHub error: " << errmsg << endl;
        return 1;
    }

    if (target == "any") {
        // retreive any voltage sensor (can be AC or DC)
        sensor = yFirstVoltage();
        if (sensor==NULL)
            die ("No module connected");
    } else {
        sensor = yFindVoltage(target + ".voltage1");
    }
    
    // we need to retreive both DC and AC voltage from the device.    
    if (sensor->isOnline())  {
        m = sensor->get_module();
        sensorDC = yFindVoltage(m->get_serialNumber() + ".voltage1");
        sensorAC = yFindVoltage(m->get_serialNumber() + ".voltage2");
      } else {
        die("Module not connected");
      }
    while(1) {
        if (!m->isOnline())  die("Module not connected");        
        cout << "Voltage,  DC : " << sensorDC->get_currentValue() << " v";
        cout << "   AC : " << sensorAC->get_currentValue() << " v";
        cout << "  (press Ctrl-C to exit)" << endl;
        ySleep(1000,errmsg);
    };
        
    return 0;
}
