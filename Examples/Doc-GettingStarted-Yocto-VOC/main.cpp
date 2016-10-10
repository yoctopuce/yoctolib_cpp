#include "yocto_api.h"
#include "yocto_voc.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

static void usage(void)
{
    cout << "usage: demo <serial_number> " << endl;
    cout << "       demo <logical_name>" << endl;
    cout << "       demo any   (use any discovered device)" << endl;
    u64 now = yGetTickCount();
    while (yGetTickCount() - now < 3000) {
        // wait 3 sec to show the message
    }
    exit(1);
}

int main(int argc, const char * argv[])
{
    string errmsg, target;
    YVoc *vocSensor;

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
        vocSensor = yFirstVoc();
        if (vocSensor == NULL) {
            cout << "No module connected (check USB cable)" << endl;
            return 1;
        }
    } else {
        vocSensor = yFindVoc(target + ".voc");
    }

    while (1) {
        if (!vocSensor->isOnline()) {
            cout << "Module not connected (check identification and USB cable)";
            break;
        }
        cout << "VOC: " << vocSensor->get_currentValue() << " ppm" << endl;
        cout << "  (press Ctrl-C to exit)" << endl;
        ySleep(1000, errmsg);
    };
    yFreeAPI();

    return 0;
}
