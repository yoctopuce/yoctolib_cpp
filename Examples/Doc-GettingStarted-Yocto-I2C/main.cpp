/*********************************************************************
 *
 *  $Id: main.cpp 52593 2023-01-03 15:21:36Z seb $
 *
 *  An example that show how to use a  Yocto-I2C
 *
 *  You can find more information on our web site:
 *   Yocto-I2C documentation:
 *      https://www.yoctopuce.com/EN/products/yocto-i2c/doc.html
 *   C++ V2 API Reference:
 *      https://www.yoctopuce.com/EN/doc/reference/yoctolib-cpp-EN.html
 *
 *********************************************************************/

#include "yocto_api.h"
#include "yocto_i2cport.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

static void usage(void)
{
  cout << "usage: demo <serial_number>" << endl;
  cout << "       demo <logical_name>" << endl;
  cout << "       demo any           (use any discovered device)" << endl;
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
  YI2cPort *i2cPort;

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
    i2cPort = YI2cPort::FirstI2cPort();
    if (i2cPort == NULL) {
      cerr << "No module connected (check USB cable)" << endl;
      return 1;
    }
  } else {
    target = (string) argv[1];
    i2cPort = YI2cPort::FindI2cPort(target + ".i2cPort");
    if (!i2cPort->isOnline()) {
      cerr << "Module not connected (check USB cable)" << endl;
      return 1;
    }
  }

  // sample code reading MCP9804 temperature sensor
  i2cPort->set_i2cMode("100kbps");
  i2cPort->set_i2cVoltageLevel(YI2cPort::I2CVOLTAGELEVEL_3V3);
  i2cPort->reset();
  // do not forget to configure the powerOutput and
  // of the Yocto-I2C as well if used
  cout << "****************************" << endl;
  cout << "* make sure voltage levels *" << endl;
  cout << "* are properly configured  *" << endl;
  cout << "****************************" << endl;

  std::vector<int> toSend(1), received;
  toSend.at(0) = 0x05;       // request ambient temperature register
  received = i2cPort->i2cSendAndReceiveArray(0x1f, toSend, 2);
  short int tempReg = (received.at(0) << 8) + received.at(1);
  if (tempReg & 0x1000) {
    tempReg |= 0xf000;    // perform sign extension
  } else {
    tempReg &= 0x0fff;    // clear status bits
  }
  cout << "Ambiant temperature: " << (tempReg / 16.0) << endl;
  u64 now = YAPI::GetTickCount();
  while (YAPI::GetTickCount() - now < 3000) {
    // wait 3 sec to show the message
  }

  YAPI::FreeAPI();
  return 0;
}
