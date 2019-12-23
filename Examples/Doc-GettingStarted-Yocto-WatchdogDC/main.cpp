/*********************************************************************
 *
 *  $Id: main.cpp 38820 2019-12-18 18:01:14Z seb $
 *
 *  An example that show how to use a  Yocto-WatchdogDC
 *
 *  You can find more information on our web site:
 *   Yocto-WatchdogDC documentation:
 *      https://www.yoctopuce.com/EN/products/yocto-watchdogdc/doc.html
 *   C++ API Reference:
 *      https://www.yoctopuce.com/EN/doc/reference/yoctolib-cpp-EN.html
 *
 *********************************************************************/

#include "yocto_api.h"
#include "yocto_watchdog.h"
#include <iostream>
#include <ctype.h>
#include <stdlib.h>

using namespace std;

static void usage(void)
{
  cout << "usage: demo <serial_number>  [ on | off | reset ]" << endl;
  cout << "       demo <logical_name> [ on | off | reset ]" << endl;
  cout << "       demo any [ on | off | reset] (use any discovered device)" << endl;
  exit(1);
}

int main(int argc, const char * argv[])
{
  string  errmsg;
  string  target;
  YWatchdog  *watchdog;
  string    state;

  if (argc < 3) {
    usage();
  }
  target = (string) argv[1];
  state = argv[2];

  // Setup the API to use local USB devices
  if (YAPI::RegisterHub("usb", errmsg) != YAPI::SUCCESS) {
    cerr << "RegisterHub error: " << errmsg << endl;
    return 1;
  }

  if (target == "any") {
    watchdog = YWatchdog::FirstWatchdog();
    if (watchdog == NULL) {
      cout << "No module connected (check USB cable)" << endl;
      return 1;
    }
  } else {
    watchdog =  YWatchdog::FindWatchdog(target + ".watchdog1");
  }

  if (watchdog->isOnline()) {
    if (state == "on")    watchdog->set_running(Y_RUNNING_ON);
    if (state == "off")   watchdog->set_running(Y_RUNNING_OFF);
    if (state == "reset") watchdog->resetWatchdog();
  } else {
    cout << "Module not connected (check identification and USB cable)" << endl;
  }
  YAPI::FreeAPI();

  return 0;
}
