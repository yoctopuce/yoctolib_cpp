#include "yocto_api.h"
#include "yocto_current.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

static void usage(const char * execname)
{   cout << "usage: " << endl;
    cout << execname << " <serial_number> " << endl;
    cout << execname << " <logical_name>" << endl;
    cout << execname << " any (use any discovered device)" << endl;
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
    YCurrent    *sensor;
    YCurrent    *sensorAC;
    YCurrent    *sensorDC;
    YModule     *m;

    if (argc < 2)  usage(argv[0]);
    
    target = (string) argv[1];
    
    // Setup the API to use local USB devices
    if (yRegisterHub("usb", errmsg) != YAPI_SUCCESS) {
        cerr << "RegisterHub error: " << errmsg << endl;
        return 1;
    }

    if (target == "any") {
        // retreive any voltage sensor (can be AC or DC)
        sensor = yFirstCurrent();
        if (sensor==NULL)
            die ("No module connected");
    } else {
        sensor = yFindCurrent(target + ".current1");
    }
    
    // we need to retreive both DC and AC voltage from the device.    
    if (sensor->isOnline())  {
        m = sensor->get_module();
        sensorDC = yFindCurrent(m->get_serialNumber() + ".current1");
        sensorAC = yFindCurrent(m->get_serialNumber() + ".current2");
      } else {
        die("Module not connected");
      }
    while(1) {
        if (!m->isOnline())  die("Module not connected");        
        cout << "Current,  DC : " << sensorDC->get_currentValue() << " mA";
        cout << "   AC : " << sensorAC->get_currentValue() << " mA";
        cout << "  (press Ctrl-C to exit)" << endl;
        ySleep(1000,errmsg);
    };
        
    return 0;
}
