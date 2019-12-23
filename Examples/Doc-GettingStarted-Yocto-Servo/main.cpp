/*********************************************************************
 *
 *  $Id: main.cpp 38820 2019-12-18 18:01:14Z seb $
 *
 *  An example that show how to use a  Yocto-Servo
 *
 *  You can find more information on our web site:
 *   Yocto-Servo documentation:
 *      https://www.yoctopuce.com/EN/products/yocto-servo/doc.html
 *   C++ API Reference:
 *      https://www.yoctopuce.com/EN/doc/reference/yoctolib-cpp-EN.html
 *
 *********************************************************************/

#include "yocto_api.h"
#include "yocto_servo.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

static void usage(void)
{
  cout << "usage: demo <serial_number>  [ -1000 | ... | 1000 ]" << endl;
  cout << "       demo <logical_name> [ -1000 | ... | 1000 ]" << endl;
  cout << "       demo any  [ -1000 | ... | 1000 ] (use any discovered device)"
       << endl;
  u64 now = YAPI::GetTickCount();
  while (YAPI::GetTickCount() - now < 3000) {
    // wait 3 sec to show the message
  }
  exit(1);
}

int main(int argc, const char * argv[])
{
  string errmsg;
  string target;
  YServo *servo1;
  YServo *servo5;
  int    pos;

  if (argc < 3) {
    usage();
  }
  target = (string) argv[1];
  pos = (int) atol(argv[2]);

  // Setup the API to use local USB devices
  if (YAPI::RegisterHub("usb", errmsg) != YAPI::SUCCESS) {
    cerr << "RegisterHub error: " << errmsg << endl;
    return 1;
  }

  if (target == "any") {
    YServo *servo = YServo::FirstServo();
    if (servo == NULL) {
      cout << "No module connected (check USB cable)" << endl;
      return 1;
    }
    target = servo->module()->get_serialNumber();
  }
  servo1 =  YServo::FindServo(target + ".servo1");
  servo5 =  YServo::FindServo(target + ".servo5");

  if (servo1->isOnline()) {
    servo1->set_position(pos);  // immediate switch
    servo5->move(pos, 3000);    // smooth transition
  } else {
    cout << "Module not connected (check identification and USB cable)" << endl;
  }
  YAPI::FreeAPI();

  return 0;
}
