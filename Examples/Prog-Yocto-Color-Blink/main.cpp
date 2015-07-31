#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <iomanip>
#include <time.h>
#include <stdio.h>
#include "yocto_api.h"
#include "yocto_colorled.h"
using namespace std;




int main(int argc, const char * argv[])
{
    string      errmsg;
    YColorLed   *led;

    // No exception please
    YAPI::DisableExceptions();
    
    if(YAPI::RegisterHub("usb", errmsg) != YAPI::SUCCESS) {
        cerr << "yInitAPI failed: " << errmsg << endl;
        return -1;
    }

  
    led = YColorLed::FirstColorLed();
	if (!led)
		{ cerr << "no color led found (check USB cable)"<<endl;
          return 1;
        }
  
    led->resetBlinkSeq();                       // cleans the sequence
    led->addRgbMoveToBlinkSeq(0x00FF00,500);     // move to green in 500 ms
    led->addRgbMoveToBlinkSeq(0x000000,   0);    // switch to black instantaneously
    led->addRgbMoveToBlinkSeq(0x000000,  250);   // stays black for 250ms
    led->addRgbMoveToBlinkSeq(0x0000FF,    0);   // switch to blue instantaneously
    led->addRgbMoveToBlinkSeq(0x0000FF,  100);   // stays blue for 100ms
    led->addRgbMoveToBlinkSeq(0x000000,   0);    // switch to black instantaneously
    led->addRgbMoveToBlinkSeq(0x000000,  250);   // stays black for 250ms
    led->addRgbMoveToBlinkSeq(0xFF0000,    0);   // switch to red instantaneously
    led->addRgbMoveToBlinkSeq(0xFF0000,  100);   // stays red for 100ms
    led->addRgbMoveToBlinkSeq(0x000000,    0);   // switch to black instantaneously
    led->addRgbMoveToBlinkSeq(0x000000, 1000);   // stays black for 1s
    led->startBlinkSeq();                       // starts sequence 
    
	
	cout << "done." << endl;
    
    return 0;
}

