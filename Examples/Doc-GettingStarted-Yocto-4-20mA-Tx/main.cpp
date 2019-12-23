/*********************************************************************
 *
 *  $Id: main.cpp 38820 2019-12-18 18:01:14Z seb $
 *
 *  An example that show how to use a  Yocto-4-20mA-Tx
 *
 *  You can find more information on our web site:
 *   Yocto-4-20mA-Tx documentation:
 *      https://www.yoctopuce.com/EN/products/yocto-4-20ma-tx/doc.html
 *   C++ API Reference:
 *      https://www.yoctopuce.com/EN/doc/reference/yoctolib-cpp-EN.html
 *
 *********************************************************************/

#include "yocto_api.h"
#include "yocto_currentloopoutput.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

static void usage(void)
{
  cout << "Wrong command line arguments" << endl;
  cout << "usage: demo <serial_number> value" << endl;
  cout << "       demo <logical_name> value" << endl;
  cout << "       demo any  value  (use any discovered device)" << endl;
  cout << "Eg." << endl;
  cout << "   demo any 12 " << endl;
  cerr << "   demo TX420MA1-123456 20" << endl;
  u64 now = YAPI::GetTickCount();
  while (YAPI::GetTickCount() - now < 3000) {
    // wait 3 sec to show the message
  }
  exit(1);
}

int main(int argc, const char * argv[])
{
  string       errmsg;
  string       target;
  YCurrentLoopOutput    *loop;

  double      value;

  if (argc < 3) {
    usage();
  }
  target = (string)argv[1];
  value = atof(argv[2]);

  // Setup the API to use local USB devices
  if (YAPI::RegisterHub("usb", errmsg) != YAPI::SUCCESS) {
    cerr << "RegisterHub error: " << errmsg << endl;
    return 1;
  }

  if (target == "any") {
    loop = YCurrentLoopOutput::FirstCurrentLoopOutput();
    if (loop == NULL) {
      cout << "No module connected (check USB cable)" << endl;
      return 1;
    }
  } else {
    loop = YCurrentLoopOutput::FindCurrentLoopOutput(target + ".currentLoopOutput");
  }

  if (loop->isOnline()) {
    loop->set_current(value);  // smooth transition
    Y_LOOPPOWER_enum loopPower = loop->get_loopPower();
    if (loopPower == Y_LOOPPOWER_NOPWR) {
      cout << "Current loop not powered" << endl;
      return 1;
    }
    if (loopPower == Y_LOOPPOWER_LOWPWR) {
      cout << "Insufficient voltage on current loop" << endl;
      return 1;
    }
    cout << "current loop set to " << value << " mA" << endl;

  } else {
    cout << "Module not connected (check identification and USB cable)" << endl;
  }
  YAPI::FreeAPI();
  return 0;
}
