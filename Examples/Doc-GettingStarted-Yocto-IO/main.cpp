/*********************************************************************
 *
 *  $Id: main.cpp 62191 2024-08-19 12:19:27Z seb $
 *
 *  An example that shows how to use a  Yocto-IO
 *
 *  You can find more information on our web site:
 *   Yocto-IO documentation:
 *      https://www.yoctopuce.com/EN/products/yocto-io/doc.html
 *   C++ API Reference:
 *      https://www.yoctopuce.com/EN/doc/reference/yoctolib-cpp-EN.html
 *
 *********************************************************************/

#include "yocto_api.h"
#include "yocto_digitalio.h"
#include <iostream>
#include <ctype.h>
#include <stdlib.h>

using namespace std;

static void usage(void)
{
  cout << "usage: demo <serial_number>  " << endl;
  cout << "       demo <logical_name> " << endl;
  cout << "       demo any           (use any discovered device)" << endl;
  u64 now = YAPI::GetTickCount();
  while (YAPI::GetTickCount() - now < 3000) {
    // wait 3 sec to show the message
  }
  exit(1);
}

int main(int argc, const char * argv[])
{
  string  errmsg;
  string  target;
  YDigitalIO  *io;

  if (argc < 2) {
    usage();
  }
  target = (string) argv[1];

  // Setup the API to use local USB devices
  if (YAPI::RegisterHub("usb", errmsg) != YAPI::SUCCESS) {
    cerr << "RegisterHub error: " << errmsg << endl;
    return 1;
  }

  if (target == "any") {
    // try to find the first available digitial IO  feature
    io =  YDigitalIO::FirstDigitalIO();
    if (io == NULL) {
      cout << "No module connected (check USB cable)" << endl;
      return 1;
    }
  } else {
    io =  YDigitalIO::FindDigitalIO(target + ".digitalIO");
  }

  // make sure the device is here
  if (!io->isOnline()) {
    cout << "Module not connected (check identification and USB cable)" << endl;
    return 1;
  }

  // lets configure the channels direction
  // bits 0 and 1 as output
  // bits 2 and 3 as input

  io->set_portDirection(0x03);
  io->set_portPolarity(0); // polarity set to regular
  io->set_portOpenDrain(0); // No open drain

  cout << "Channels 0..1 are configured as outputs and channels 2..3" << endl;
  cout << "are configred as inputs, you can connect some inputs to" << endl;
  cout << "ouputs and see what happens" << endl;

  int  outputdata = 0;
  while (io->isOnline()) {
    int inputdata = io->get_portState(); // read port values
    string line = ""; // display port value as binary
    for (int i = 0; i < 4 ; i++) {
      if  (inputdata & (8 >> i))
        line = line + '1';
      else
        line = line + '0';
    }
    cout << "port value = " << line << endl;
    outputdata = (outputdata + 1) % 4; // cycle ouput 0..3
    io->set_portState(outputdata); // We could have used set_bitState as well
    YAPI::Sleep(1000, errmsg);
  }
  cout << "Module disconnected" << endl;
  YAPI::FreeAPI();
}
