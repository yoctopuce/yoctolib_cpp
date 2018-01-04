#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <iomanip>
#include <time.h>
#include <stdio.h>
#include "yocto_api.h"

using namespace std;

static void dumpSensor(YSensor *sensor)
{
  const char * fmt = "%d %b %Y %H:%M:%S";
  char buffer[80];
  struct tm * timeinfo;

  cout << "Using DataLogger of " << sensor->get_friendlyName() << endl;
  YDataSet dataset = sensor->get_recordedData(0, 0);
  cout << "loading summary... " << endl;
  dataset.loadMore();
  YMeasure summary = dataset.get_summary();
  timeinfo = localtime(summary.get_startTimeUTC_asTime_t(NULL));
  buffer[0] = 0;
  strftime(buffer, 80, fmt, timeinfo);
  printf("from %s ", buffer);
  timeinfo = localtime(summary.get_endTimeUTC_asTime_t(NULL));
  strftime (buffer, 80, fmt, timeinfo);
  printf("to %s : min=%.3f%s avg=%.3f%s  max=%.3f%s\n",
         buffer, summary.get_minValue(), sensor->get_unit().c_str(),
         summary.get_averageValue(), sensor->get_unit().c_str(),
         summary.get_maxValue(), sensor->get_unit().c_str());
  cout << "Loading details :   0%" << flush;
  int progress = 0;
  do {
    progress = dataset.loadMore();
    cout << '\b' << '\b' << '\b' << '\b' << setw(4) << progress << flush;
  } while(progress < 100);
  cout << endl;
  vector<YMeasure> details = dataset.get_measures();
  for(vector<YMeasure>::iterator m = details.begin(); m != details.end(); ++m ) {
    timeinfo = localtime (m->get_startTimeUTC_asTime_t(NULL));
    strftime (buffer, 80, fmt, timeinfo);
    printf("from %s ", buffer);
    timeinfo = localtime (m->get_endTimeUTC_asTime_t(NULL));
    strftime (buffer, 80, fmt, timeinfo);
    printf("to %s : min=%.3f%s avg=%.3f%s  max=%.3f%s\n",
           buffer, m->get_minValue(), sensor->get_unit().c_str(),
           m->get_averageValue(), sensor->get_unit().c_str(),
           m->get_maxValue(), sensor->get_unit().c_str());
  }
}



int main(int argc, const char * argv[])
{
  string      errmsg;
  YSensor   *sensor;

  // No exception please
  YAPI::DisableExceptions();

  if(YAPI::RegisterHub("usb", errmsg) != YAPI::SUCCESS) {
    cerr << "yInitAPI failed: " << errmsg << endl;
    return -1;
  }

  if (argc == 1 || string(argv[1]) == "any") {
    sensor = YSensor::FirstSensor();
    if (sensor == NULL) {
      cerr << "No module connected (check USB cable)" << endl;
      return 1;
    }
  } else {
    sensor = YSensor::FindSensor(string(argv[1]));
    if (!sensor->isOnline()) {
      cerr << "Sensor " << sensor->get_hardwareId() << " is not connected (check USB cable)" <<
           endl;
      return 1;
    }
  }
  dumpSensor(sensor);
  YAPI::FreeAPI();
  cout << "done." << endl;

  return 0;
}

