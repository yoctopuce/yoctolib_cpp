/*********************************************************************
 *
 *  $Id: main.cpp 32716 2018-10-19 15:54:48Z seb $
 *
 *  An example that show how to use a  Yocto-MaxiCoupler
 *
 *  You can find more information on our web site:
 *   Yocto-MaxiCoupler documentation:
 *      https://www.yoctopuce.com/EN/products/yocto-maxicoupler/doc.html
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

static void usage(const char* execname)
{
  cout  << "usage:" << endl;
  cout  << execname << " serial_number> <channel>  [ ON | OFF ]" << endl;
  cout  << execname << " <logical_name> <channel>[ ON | OFF ]" << endl;
  cout  << execname << " any <channel> [ ON | OFF ]" << endl;
  cout  << "Example"   << endl;
  cout  << execname << " any 2 ON" << endl;
  exit(1);
}

int main(int argc, const char * argv[])
{
  string  errmsg;
  string  target;
  string  channel;
  YRelay  *relay;
  string  state;

  if (argc < 3) usage(argv[0]);

  target  = (string) argv[1];
  channel = (string) argv[2];
  state   = (string) argv[3];

  // Setup the API to use local USB devices
  if (yRegisterHub("usb", errmsg) != YAPI_SUCCESS) {
    cerr << "RegisterHub error: " << errmsg << endl;
    return 1;
  }

  if (target == "any") {
    relay =  yFirstRelay();
    if (relay == NULL) {
      cout << "No module connected (check USB cable)" << endl;
      return 1;
    }
    target = relay->get_module()->get_serialNumber();
  }

  cout << "Using " << target << endl;
  relay =  yFindRelay((string)target + ".relay" + channel);

  if (relay->isOnline()) {
    relay->set_output(state == "ON" ? Y_OUTPUT_ON : Y_OUTPUT_OFF);
  } else {
    cout << "Module not connected (check identification and USB cable)" << endl;
  }
  yFreeAPI();
  return 0;
}
