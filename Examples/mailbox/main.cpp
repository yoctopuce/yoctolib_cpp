#include <iostream>
// includes
#include "yocto_api.h"
#include "yocto_colorled.h"
#include "yocto_anbutton.h"

using namespace std;

#define RGB_GREEN   0x00FF00
#define RGB_RED     0xFF0000


int main(int argc, const char * argv[])
{
    string      errmsg;
    s64        lastPostman;
    s64        lastColect;

    YAPI::InitAPI(Y_DETECT_USB, errmsg);

    YAnButton *postmanSensor = YAnButton::FindAnButton("YBUTTON1-99999.anButton5");
    YAnButton *colectSensor  = YAnButton::FindAnButton("YBUTTON1-99999.anButton4");
     
    YColorLed *led1 =  YColorLed::FindColorLed("YRGBLED1-001A8.colorLed1");
    YColorLed *led2 =  YColorLed::FindColorLed("YRGBLED1-001A8.colorLed2");
    if (!led1->isOnline() || !led2->isOnline()) {
        cout << "the led "<< led1->describe() << " is not connected" << endl;
        return 1;
    }

    led1->set_rgbColor(RGB_GREEN); 
    led2->set_rgbColor(RGB_GREEN);  

    if (!postmanSensor->isOnline() || !colectSensor->isOnline()) {
        cout << "the mailbox sensor is not connected" << endl;
        return 1;
    }
    lastPostman = postmanSensor->get_lastTimePressed();
    lastColect =  colectSensor->get_lastTimeReleased();
    while(1){
        s64 newPostman =  postmanSensor->get_lastTimePressed();
        if(lastPostman !=  newPostman){           
            lastPostman =  newPostman;
            cout << "Postman Deteected"<< endl;
            led1->set_rgbColor(RGB_RED);
            led2->set_rgbColor(RGB_RED);  
        }
        s64 newColect =  colectSensor->get_lastTimeReleased();
        if(lastColect !=  newColect){           
            lastColect =  newColect;
            cout << "Colect Deteected"<< endl;
            led1->set_rgbColor(RGB_GREEN); 
            led2->set_rgbColor(RGB_GREEN);  
        }
        YAPI::Sleep(300,errmsg);
    }

    return 0;
}
