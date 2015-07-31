#include "yocto_api.h"
#include "yocto_pwmoutput.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

static void usage(void)
{
    cout << "usage: demo <serial_number>  <frequency> <dutyCycle>" << endl;
    cout << "       demo <logical_name> <frequency> <dutyCycle>" << endl;
    cout << "       demo any  <frequency> <dutyCycle>   (use any discovered device)" << endl;
    cout << "       <frequency>: integer between 1Hz and 1000000Hz" << endl;
    cout << "       <dutyCycle>: floating point number between 0.0 and 100.0" << endl;
    u64 now = yGetTickCount();
	while (yGetTickCount()-now<3000) {
        // wait 3 sec to show the message
    }
    exit(1);
}

int main(int argc, const char * argv[])
{
    string errmsg;
    string target;
    YPwmOutput *pwmoutput1;
    YPwmOutput *pwmoutput2;
    int    frequency;
    double dutyCycle;

    if (argc < 4) {
        usage();
    }
    target = (string) argv[1];
    frequency = (int) atol(argv[2]);
    dutyCycle = atof(argv[3]);
    
    // Setup the API to use local USB devices
    if (yRegisterHub("usb", errmsg) != YAPI_SUCCESS) {
        cerr << "RegisterHub error: " << errmsg << endl;
        return 1;
    }

    if (target == "any") {
        YPwmOutput *pwmoutput = yFirstPwmOutput();
        if (pwmoutput==NULL) {
            cout << "No module connected (check USB cable)" << endl;
            return 1;
        }
        target = pwmoutput->module()->get_serialNumber();
    }   
    pwmoutput1 =  yFindPwmOutput(target + ".pwmOutput1");
    pwmoutput2 =  yFindPwmOutput(target + ".pwmOutput2");

    if (pwmoutput1->isOnline()) {
        // output 1 : immediate change
        pwmoutput1->set_frequency(frequency);
        pwmoutput1->set_enabled(Y_ENABLED_TRUE);
        pwmoutput1->set_dutyCycle(dutyCycle);
        // output 2 : smooth change
        pwmoutput2->set_frequency(frequency);
        pwmoutput2->set_enabled(Y_ENABLED_TRUE);
        pwmoutput2->dutyCycleMove(dutyCycle,3000);
    } else {
        cout << "Module not connected (check identification and USB cable)" << endl;
    }
        
    return 0;
}
