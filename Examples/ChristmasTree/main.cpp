#include <iostream>
// includes
#include "yocto_api.h"
#include "yocto_colorled.h"


using namespace std;

#define RGB_BLACK   0x000000
#define RGB_BLUE    0x0000FF
#define RGB_GREEN   0x00FF00
#define RGB_RED     0xFF0000


static void ylog(const string& log)
{
  cout << "LOG:" << log << endl;
}

u64 into_set_rgbColor = 0;
u64 nb_set_rgbColor = 0;
static void SetGlobalColor(vector<YColorLed*> &leds, unsigned color)
{
  for (vector<YColorLed*>::iterator it = leds.begin(); it != leds.end(); ++it) {
    u64 start, stop;
    start = yGetTickCount();
    (*it)->set_rgbColor(color);
    stop = yGetTickCount();
    into_set_rgbColor += stop - start;
    nb_set_rgbColor++;
    it++;
    if(it == leds.end())
      break;
  }
  vector<YColorLed*>::iterator it = leds.begin();
  for (++it; it != leds.end(); ++it) {
    u64 start, stop;
    start = yGetTickCount();
    (*it)->set_rgbColor(color);
    stop = yGetTickCount();
    into_set_rgbColor += stop - start;
    nb_set_rgbColor++;
    it++;
    if(it == leds.end())
      break;
  }

}


int main(int argc, const char * argv[])
{

  string          errmsg;
  vector<YColorLed*> all_leds;
  int             i;

  if(YISERR(yInitAPI(Y_DETECT_USB, errmsg))) {
    cerr << errmsg << endl;
    return 0;
  }
  yRegisterLogFunction(ylog);
  cout << "Do Enumeration" << endl;
  if(YISERR(yUpdateDeviceList(errmsg))) {
    cerr << errmsg << endl;
    return 0;
  }
  YColorLed *led =  yFirstColorLed();

  while(led) {
    all_leds.push_back(led);
    led = led->nextColorLed();
  };
  if(all_leds.size() == 0) {
    cout << "no rgb led connected" << endl;
    return 1;
  }

  cout << all_leds.size() << " rgb leds found" << endl;
  int nb_iterations = 100;

  u64 start = yGetTickCount();
  for(i = 0; i < nb_iterations ; i++) {
    SetGlobalColor(all_leds, i & 1 ? RGB_GREEN : RGB_RED);
  }
  u64 stop = yGetTickCount();
  cout << nb_set_rgbColor << " set rgb take " << (stop - start) / 1000 << " sec + ";
  cout << (stop - start) % 1000 << " ms (" << (stop - start) << ")" << endl;
  cout << "nb ms into yocto cpp :" << into_set_rgbColor << " ms" << endl;
  cout << "we need " << into_set_rgbColor / nb_set_rgbColor << " ms per set" << endl;
  yFreeAPI();
  return 0;
}
