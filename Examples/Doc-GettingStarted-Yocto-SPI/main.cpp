#include "yocto_api.h"
#include "yocto_spiport.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

static void usage(void)
{
  cout << "usage: demo <serial_number> <value>" << endl;
  cout << "       demo <logical_name>  <value>" << endl;
  cout << "       demo any  <value>   (use any discovered device)" << endl;
  u64 now = yGetTickCount();
  while (yGetTickCount() - now < 3000) {
    // wait 3 sec to show the message
  }
  exit(1);
}

int main(int argc, const char * argv[])
{
  string errmsg;
  string target;
  int    value;
  YSpiPort *spiPort;

  if (argc < 3) {
    usage();
  }
  target = (string) argv[1];
  value = (int)atol(argv[2]);

  // Setup the API to use local USB devices
  if (yRegisterHub("usb", errmsg) != YAPI_SUCCESS) {
    cerr << "RegisterHub error: " << errmsg << endl;
    return 1;
  }

  if (target == "any") {
    spiPort = yFirstSpiPort();
    if (spiPort == NULL) {
      cerr << "No module connected (check USB cable)" << endl;
      return 1;
    }
  } else {
    target = (string) argv[1];
    spiPort = yFindSpiPort(target + ".spiPort");
    if (!spiPort->isOnline()) {
      cerr << "Module not connected (check USB cable)" << endl;
      return 1;
    }
  }

  // sample code driving MAX7219 7-segment display driver
  // such as SPI7SEGDISP8.56 from www.embedded-lab.com
  spiPort->set_spiMode("250000,3,msb");
  spiPort->set_ssPolarity(Y_SSPOLARITY_ACTIVE_LOW);
  spiPort->set_protocol("Frame:5ms");
  spiPort->reset();
  // do not forget to configure the powerOutput of the Yocto-SPI
  // ( for SPI7SEGDISP8.56 powerOutput need to be set at 5v )
  cout << "****************************" << endl;
  cout << "* make sure voltage levels *" << endl;
  cout << "* are properly configured  *" << endl;
  cout << "****************************" << endl;

  spiPort->writeHex("0c01"); // Exit from shutdown state
  spiPort->writeHex("09ff"); // Enable BCD for all digits
  spiPort->writeHex("0b07"); // Enable digits 0-7 (=8 in total)
  spiPort->writeHex("0a0a"); // Set medium brightness
  for(int i = 1; i <= 8; i++) {
    std::vector<int> bytes(2);
    bytes.at(0) = i;          // digit position
    bytes.at(1) = value % 10; // digit value
    spiPort->writeArray(bytes);
    value = value / 10;
  }

  yFreeAPI();
  return 0;
}
