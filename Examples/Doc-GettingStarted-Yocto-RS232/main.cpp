/*********************************************************************
 *
 *  $Id: main.cpp 59576 2024-02-29 09:36:30Z seb $
 *
 *  An example that show how to use a  Yocto-RS232
 *
 *  You can find more information on our web site:
 *   Yocto-RS232 documentation:
 *      https://www.yoctopuce.com/EN/products/yocto-rs232/doc.html
 *   C++ V2 API Reference:
 *      https://www.yoctopuce.com/EN/doc/reference/yoctolib-cpp-EN.html
 *
 *********************************************************************/

#include "yocto_api.h"
#include "yocto_serialport.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

int main(int argc, const char * argv[])
{
  string errmsg;
  string target;
  YSerialPort *serialPort;

  // Setup the API to use local USB devices
  if (YAPI::RegisterHub("usb", errmsg) != YAPI::SUCCESS) {
    cerr << "RegisterHub error: " << errmsg << endl;
    return 1;
  }

  if (argc > 1) {
    target = (string) argv[1];
    serialPort = YSerialPort::FindSerialPort(target + ".serialPort");
  } else {
    serialPort = YSerialPort::FirstSerialPort();
    if (serialPort == NULL) {
      cerr << "No module connected (check USB cable)" << endl;
      return 1;
    }
  }

  serialPort->set_serialMode("9600,8N1");
  serialPort->set_protocol("Line");
  serialPort->reset();

  string line;
  do {
    YAPI::Sleep(500, errmsg);
    do {
      line = serialPort->readLine();
      if(line != "") {
        cout << "Received: " << line << endl;
      }
    } while(line != "");

    cout << "Type line to send, or Ctrl-C to exit:";
    cin >> line;
    serialPort->writeLine(line);
  } while(line != "");

  YAPI::FreeAPI();
  return 0;
}
