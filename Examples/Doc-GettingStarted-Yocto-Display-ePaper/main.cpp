/*********************************************************************
 *
 *  $Id: main.cpp 75051 2026-07-02 09:51:19Z seb $
 *
 *  An example that shows how to use a  Yocto-Display-ePaper
 *
 *  You can find more information on our web site:
 *   Yocto-Display-ePaper documentation:
 *      https://www.yoctopuce.com/EN/products/yocto-display-epaper/doc.html
 *   C++ API Reference:
 *      https://www.yoctopuce.com/EN/doc/reference/yoctolib-cpp-EN.html
 *
 *********************************************************************/

#include "yocto_api.h"
#include "yocto_display.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

static void usage(void)
{
  cout << "Wrong command line arguments" << endl;
  cout << "usage: demo <serial_number>" << endl;
  cout << "       demo <logical_name>" << endl;
  cout << "       demo any (use any discovered device)" << endl;
  u64 now = YAPI::GetTickCount();
  while (YAPI::GetTickCount() - now < 3000) {
    // wait 3 sec to show the message
  }
  exit(1);
}

int main(int argc, const char * argv[])
{
  string         errmsg;
  string         target;
  YDisplay      *disp;
 
  if(argc < 2) usage(); 

  // Setup the API to use local USB devices
  if (YAPI::RegisterHub("usb", errmsg) != YAPI::SUCCESS) {
    cerr << "RegisterHub error: " << errmsg << endl;
    usage();
    return 1;
  }

  target     = (string) argv[1];
  if (target == "any") {
    disp =  YDisplay::FirstDisplay();
    if (disp == NULL) {
      cout << "No module connected (check USB cable)" << endl;
      usage();
      return 1;
    }
  } else {
    disp =  YDisplay::FindDisplay(target + ".display");
  }

  if (!disp->isOnline()) {
    cout << "Module is offline (check USB cable)" << endl;
    usage();
    return 1;
  }

  u32 colors[] = { 0xFFFFFF, 0x000000, 0xFF0000, 0xFFFF00 };

// Makes sure the Panel type is set
string paneltype = disp->get_displayPanel();
if (paneltype== "NOT_SET")
{   cout << "Use the virtual to Configure the panel first";
    YAPI::Sleep(3000, errmsg);
    exit(1);
}

// retrieve the display size
int w = disp->get_displayWidth();
int h = disp->get_displayHeight();
int middleX = (int)(w / 2);
int middleY = (int)(h / 2);
cout << "Using device " << disp->get_serialNumber() << " (panel: " << paneltype << " " << w << "x" << h << "pixels)\n";
disp->resetAll();

// retrieve the first layer
YDisplayLayer* l0 = disp->get_displayLayer(0);
l0->selectFont("medium.yfm");
int interations = 0;
bool animation = true;

while (animation)
{
  interations++;
  // prevent refreshing for 2 sec
  disp->postponeRefresh(2000);
  l0->clear();
  // draw a few circle
  for (int i = 0; i < 15; i++)
  {
    int cx = rand() % w;
    int  cy = rand() % h;
    int  r = (int)(h / 20) + rand() % (int)(h / 10);
    l0->selectFillColor(colors[rand() % 4]);
    l0->drawDisc(cx, cy, r);
    l0->drawCircle(cx, cy, r);
  }
  // draw a rectangle with panel type in it
  l0->selectFillColor(0xffffff);
  l0->drawBar(middleX - 75, middleY - 10, middleX + 75, middleY + 12);
  l0->drawRect(middleX - 75, middleY - 10, middleX + 75, middleY + 12);
  l0->drawText(middleX, middleY, YDisplayLayer::ALIGN_CENTER, paneltype);
  // forces a full refresh only the 1rst time
  if (interations == 1) disp->regenerateDisplay();
  disp->triggerRefresh(); // display is allowed to refresh  again
  YAPI::Sleep(1000, errmsg);
  // if no fast refresh available, don't even try to run animations
  if (paneltype.find("KS") < 0)  animation = false;
  }

YAPI::FreeAPI();
}

 