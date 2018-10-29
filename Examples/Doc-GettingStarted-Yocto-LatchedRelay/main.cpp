/*********************************************************************
 *
 *  $Id: main.cpp 32716 2018-10-19 15:54:48Z seb $
 *
 *  An example that show how to use a  Yocto-LatchedRelay
 *
 *  You can find more information on our web site:
 *   Yocto-LatchedRelay documentation:
 *      https://www.yoctopuce.com/EN/products/yocto-latchedrelay/doc.html
 *   C++ API Reference:
 *      https://www.yoctopuce.com/EN/doc/reference/yoctolib-cpp-EN.html
 *
 *********************************************************************/

#include "yocto_api.h"
#include "yocto_relay.h"
#include <iostream>
#include <ctype.h>
#include <stdlib.h>

using namespace std;

static void usage(void)
{
  cout << "usage: demo <serial_number>  [ A | B ]" << endl;
  cout << "       demo <logical_name> [ A | B ]" << endl;
  cout << "       demo any [ A | B ]" << endl;
  u64 now = yGetTickCount();
  while (yGetTickCount() - now < 3000) {
    // wait 3 sec to show the message
  }
  exit(1);
}

int main(int argc, const char * argv[])
{
  string  errmsg;
  string  target;
  YRelay  *relay;
  char    state;

  if (argc < 3) {
    usage();
  }
  target = (string) argv[1];
  state = toupper(argv[2][0]);

  // Setup the API to use local USB devices
  if (yRegisterHub("usb", errmsg) != YAPI_SUCCESS) {
    cerr << "RegisterHub error: " << errmsg << endl;
    return 1;
  }

  if (target == "any") {
    relay = yFirstRelay();
    if (relay == NULL) {
      cout << "No module connected (check USB cable)" << endl;
      return 1;
    }
  } else {
    relay =  yFindRelay(target + ".relay1");
  }

  if (relay->isOnline()) {
    relay->set_state(state == 'A' ? Y_STATE_A : Y_STATE_B);
  } else {
    cout << "Module not connected (check identification and USB cable)" << endl;
  }
  yFreeAPI();

  return 0;
}
