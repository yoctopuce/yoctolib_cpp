#include <iostream>

using namespace std;

// include files for accessing Yoctopuce sensors
#include "yocto_api.h"

// include files for cross-platform TCP/IP support
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#else
#include <netdb.h>
#endif
#include <iostream>
#include <string.h>
#include <stdlib.h>

// UDP socket to stream values
struct  sockaddr_in udp_addr;
int     udpsock;

// Function invoked each time the tilt sensor changes
//
static void sensorValueChangeCallback(YSensor *fct, const string& value)
{
  // stream the value in ASCII null-terminated string
  sendto(udpsock, value.c_str(), value.length() + 1, 0, (struct sockaddr*) &udp_addr,
         sizeof(udp_addr));
}

// Function invoked each time a new Yoctopuce device is found
//
static void deviceArrival(YModule *m)
{
  string serial = m->get_serialNumber();
  cout << "Sensor connected : " << serial << endl;
  // If sensor is a kind of Yocto-3D, use its X tilt sensor
  if(serial.substr(0, 3) == "Y3D") {
    YSensor *sensor = YSensor::FindSensor(serial + ".tilt1");
    sensor->registerValueCallback(sensorValueChangeCallback);
  }
}

int main(int argc, const char * argv[])
{
  struct addrinfo *infos, *p;
  string errmsg;

  if(argc < 4) {
    cerr << "Usage: " << argv[0] << " <YoctoHubIP> <UDPServerIP> <UDPServerPort>" << endl;
    return 1;
  }

  // Initialize Yoctopuce library in fault-tolerant mode
  YAPI::RegisterDeviceArrivalCallback(deviceArrival);
  YAPI::DisableExceptions();
  YAPI::PreregisterHub(argv[1], errmsg);

  // Prepare to stream UDP packets to selected host
  memset((char *) &udp_addr, 0, sizeof(udp_addr));
  udp_addr.sin_family = AF_INET;
  udp_addr.sin_port = htons(atoi(argv[3]));
  if (getaddrinfo(argv[2], NULL, NULL, &infos) != 0) {
    cerr << "Unable to resolve UDP server " << argv[2] << endl;
    return 1;
  }
  for (p = infos; p != NULL; p = p->ai_next) {
    if (p->ai_family == AF_INET) {
      udp_addr.sin_addr = ((struct sockaddr_in *)p->ai_addr)->sin_addr;
      break;
    }
  }
  freeaddrinfo(infos);
  udpsock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

  // Handle incoming events from Yoctopuce library lazily
  while (true) {
    YAPI::UpdateDeviceList(errmsg); // handle plug/unplug events
    YAPI::Sleep(500, errmsg);       // handle others events
  }
}

