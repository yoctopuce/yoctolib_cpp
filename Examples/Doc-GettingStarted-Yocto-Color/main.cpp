#include "yocto_api.h"
#include "yocto_colorled.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

static void usage(void)
{
    cout << "usage: demo <serial_number>  [ color | rgb ]" << endl;
    cout << "       demo <logical_name> [ color | rgb ]" << endl;
    cout << "       demo any  [ color | rgb ]                (use any discovered device)" << endl;
    cout << "Eg." << endl;
    cout << "   demo any FF1493 " << endl;
    cerr << "   demo YRGBLED1-123456 red" << endl;
    u64 now = yGetTickCount();    // dirty active wait loop 
	while (yGetTickCount()-now<3000);
    exit(1);
}

int main(int argc, const char * argv[])
{
    string       errmsg;
    string       target;
    YColorLed    *led1;
    YColorLed    *led2; 
    string       color_str;
    unsigned int color;
 
    if(argc < 3) {
        usage();
    }
    target     = (string) argv[1];
    color_str  = (string) argv[2];
    if (color_str == "red")
        color = 0xFF0000;
    else if ( color_str == "green")            
        color = 0x00FF00;
    else if (color_str == "blue")
        color = 0x0000FF;
    else 
        color = (unsigned int)strtoul(color_str.c_str(),NULL, 16);

    // Setup the API to use local USB devices
    if (yRegisterHub("usb", errmsg) != YAPI_SUCCESS) {
        cerr << "RegisterHub error: " << errmsg << endl;
        return 1;
    }

    if (target == "any") {
        led1 =  yFirstColorLed();        
        if (led1==NULL) {
            cout << "No module connected (check USB cable)" << endl;
            return 1;
        }
        led2 =  led1->nextColorLed();
    } else {
        led1 =  yFindColorLed(target + ".colorLed1");
        led2 =  yFindColorLed(target + ".colorLed2");
    }
    
    if (led1->isOnline()) {
        led1->set_rgbColor(color);  // immediate switch
        led2->rgbMove(color,1000);  // smooth transition  
    } else {
        cout << "Module not connected (check identification and USB cable)" << endl;
    }
        
    return 0;
}
