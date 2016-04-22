#include "yocto_api.h"
#include "yocto_lightsensor.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

static void usage(void)
{
    cout << "usage: demo <serial_number> " << endl;
    cout << "       demo <logical_name>" << endl;
    cout << "       demo any                 (use any discovered device)" << endl;
    u64 now = yGetTickCount();
	while (yGetTickCount()-now<3000) {
        // wait 3 sec to show the message
    }
    exit(1);
}

int main(int argc, const char * argv[])
{
    string errmsg, target;
    YLightSensor    *hsensor;
    
    cout << "RegisterHub"<<  endl;

    // Setup the API to use local USB devices
    if (yRegisterHub("usb", errmsg) != YAPI_SUCCESS) {
        cerr << "RegisterHub error: " << errmsg << endl;
        return 1;
    }

    hsensor = yFirstLightSensor();
    if (!hsensor->isOnline()) {
        cout << "Module not connected (check identification and USB cable)";
        return -1;
    }

    cout << "Current : " << hsensor->get_currentValue() << " lx" << endl;

    cout << "  (press Ctrl-C to exit)" << endl;
    ySleep(100, errmsg);
    yUnregisterHub("usb");
    yFreeAPI();

    return 0;
}
