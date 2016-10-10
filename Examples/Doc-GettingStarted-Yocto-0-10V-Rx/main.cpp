#include "yocto_api.h"
#include "yocto_genericsensor.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

static void usage(void)
{
    cout << "usage: demo <serial_number> " << endl;
    cout << "       demo <logical_name>" << endl;
    cout << "       demo any                 (use any discovered device)" << endl;
    u64 now = yGetTickCount();
    while (yGetTickCount() - now < 3000) {
        // wait 3 sec to show the message
    }
    exit(1);
}

int main(int argc, const char * argv[])
{
    string errmsg, target;
    YGenericSensor *sensor;

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
        sensor = yFirstGenericSensor();
        if (sensor == NULL) {
            cout << "No module connected (check USB cable)" << endl;
            return 1;
        }
    } else {
        sensor = yFindGenericSensor(target + ".genericSensor1");
    }

    YGenericSensor *s1 = yFindGenericSensor(sensor->get_module()->get_serialNumber() + ".genericSensor1");
    YGenericSensor *s2 = yFindGenericSensor(sensor->get_module()->get_serialNumber() + ".genericSensor2");

    string unitSensor1, unitSensor2;

    if (s1->isOnline()) unitSensor1 = s1->get_unit();
    if (s2->isOnline()) unitSensor2 = s2->get_unit();

    while (s1->isOnline() && s2->isOnline())  {
        double value = s1->get_currentValue();
        cout << "Channel 1 : " << s1->get_currentValue() << unitSensor1;
        value = s2->get_currentValue();
        cout << "  Channel 2 : " << s2->get_currentValue() << unitSensor2;
        cout << "  (press Ctrl-C to exit)" << endl;
        ySleep(1000, errmsg);
    };
    yFreeAPI();
    cout << "Module not connected (check identification and USB cable)";
    return 0;
}
