/*********************************************************************
 *
 *  $Id: main.cpp 32619 2018-10-10 12:22:50Z seb $
 *
 *  An example that show how to use a  Yocto-Serial
 *
 *  You can find more information on our web site:
 *   Yocto-Serial documentation:
 *      https://www.yoctopuce.com/EN/products/yocto-serial/doc.html
 *   C++ API Reference:
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
  if (yRegisterHub("usb", errmsg) != YAPI_SUCCESS) {
    cerr << "RegisterHub error: " << errmsg << endl;
    return 1;
  }

  if (argc > 1) {
    target = (string) argv[1];
    serialPort = yFindSerialPort(target + ".serialPort");
  } else {
    serialPort = yFirstSerialPort();
    if (serialPort == NULL) {
      cerr << "No module connected (check USB cable)" << endl;
      return 1;
    }
  }

  cout << "****************************" << endl;
  cout << "* make sure voltage levels *" << endl;
  cout << "* are properly configured  *" << endl;
  cout << "****************************" << endl;

  serialPort->set_serialMode("9600,8N1");
  serialPort->set_protocol("Line");
  serialPort->reset();

  string line;
  do {
    ySleep(500, errmsg);
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

  yFreeAPI();
  return 0;
}
