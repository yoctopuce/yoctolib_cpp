#include "yocto_api.h"
#include "yocto_temperature.h"
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
    string errmsg, target, serial;
    YTemperature *tsensor;

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
        tsensor = yFirstTemperature();
        if (tsensor == NULL) {
            cout << "No module connected (check USB cable)" << endl;
            return 1;
        }
    } else {
        tsensor = yFindTemperature(target + ".temperature1");
    }

    serial = tsensor->get_module()->get_serialNumber();
    YTemperature *t1 = yFindTemperature(serial + ".temperature1");
    YTemperature *t2 = yFindTemperature(serial + ".temperature2");
    YTemperature *t3 = yFindTemperature(serial + ".temperature3");
    YTemperature *t4 = yFindTemperature(serial + ".temperature4");
    YTemperature *t5 = yFindTemperature(serial + ".temperature5");
    YTemperature *t6 = yFindTemperature(serial + ".temperature6");

    while (1) {
        if (!t1->isOnline() || !t2->isOnline()) {
            cout << "Module not connected (check identification and USB cable)";
            break;
        }

        cout << " | 1: " << t1->get_currentValue();
        cout << " | 2: " << t2->get_currentValue();
        cout << " | 3: " << t3->get_currentValue();
        cout << " | 4: " << t4->get_currentValue();
        cout << " | 5: " << t5->get_currentValue();
        cout << " | 6: " << t6->get_currentValue();

        cout <<  " |  deg C |" << endl;

        ySleep(1000, errmsg);
    };
    yFreeAPI();

    return 0;
}
