#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <iomanip>
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "yocto_api.h"
#ifdef WINDOWS_API
#define SAFE_SPRINTF         sprintf_s
#else
#define SAFE_SPRINTF         snprintf
#endif
using namespace std;

int main(int argc, const char *argv[])
{
  string errmsg, line;
  YSensor *sensor;
  vector<YSensor*> sensorList = vector<YSensor*>();
  vector<double> record;
  YConsolidatedDataSet *data;
  char buffer[80];

  // Setup the API to use local devices
  if (YAPI::RegisterHub("usb", errmsg) != YAPI::SUCCESS) {
    cerr << "yInitAPI failed: " << errmsg << endl;
    return -1;
  }

  // Enumerate all connected sensors
  sensor = YSensor::FirstSensor();
  while (sensor != NULL) {
    sensorList.push_back(sensor);
    sensor = sensor->nextSensor();
  }
  if (sensorList.size() == 0) {
    cerr << "No Yoctopuce sensor connected (check USB cable)" << endl;
    return 1;
  }

  // Generate consolidated CSV output for all sensors
  data = new YConsolidatedDataSet(0, 0, sensorList);
  while (data->nextRecord(record) < 100) {
    int avail = sizeof(buffer);
    time_t secs = (time_t)(record.at(0));
    struct tm *timeinfo = localtime(&secs);
    strftime(buffer, 80, "%Y-%m-%dT%H:%M:", timeinfo);
    avail -= strlen(buffer);
    SAFE_SPRINTF(buffer + strlen(buffer), avail, "%03.3f", fmod(record.at(0), 60));
    line = string(buffer);
    for (size_t idx = 1; idx < record.size(); idx++) {
      SAFE_SPRINTF(buffer, sizeof(buffer), ";%.3f", record.at(idx));
      line += string(buffer);
    }
    cout << line << endl;
  }
  u64 now = YAPI::GetTickCount();
  while (YAPI::GetTickCount() - now < 15000);
  YAPI::FreeAPI();
  cout << "done." << endl;

  return 0;
}
