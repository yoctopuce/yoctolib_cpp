/*********************************************************************
 *
 *  $Id: svn_id $
 *
 *  An example that show how to use a  Yocto-MaxiKnob
 *
 *  You can find more information on our web site:
 *   Yocto-MaxiKnob documentation:
 *      https://www.yoctopuce.com/EN/products/yocto-maxiknob/doc.html
 *   C++ V2 API Reference:
 *      https://www.yoctopuce.com/EN/doc/reference/yoctolib-cpp-EN.html
 *
 *********************************************************************/

#include "yocto_api.h"
#include "yocto_buzzer.h"
#include "yocto_anbutton.h"
#include "yocto_colorledcluster.h"
#include "yocto_quadraturedecoder.h"
#include <iostream>

using namespace std;

static void usage(void)
{
  cout << "usage: demo <serial_number> " << endl;
  cout << "       demo any  (use any discovered device)" << endl;
  u64 now = yGetTickCount();
  while (yGetTickCount() - now < 3000) {
    // wait 3 sec to show the message
  }
  exit(1);
}

static int notefreq(int note)
{
  return (int)( 220.0 * exp(note * log(2.0) / 12));
}

int main(int argc, const char* argv[])
{
  string errmsg;
  string target, serial;
  YBuzzer* buz;
  YColorLedCluster* leds;
  YAnButton* button;
  YQuadratureDecoder* qd;
  double lastPos;

  if (argc < 2) {
    usage();
  }
  target = (string)argv[1];

  // Setup the API to use local USB devices
  if (YAPI::RegisterHub("usb", errmsg) != YAPI_SUCCESS) {
    cerr << "RegisterHub error: " << errmsg << endl;
    return 1;
  }

  if (target == "any") {
    buz = YBuzzer::FirstBuzzer();
    if (buz == NULL) {
      cout << "No module connected (check USB cable)" << endl;
      return 1;
    }
  } else {
    buz = YBuzzer::FindBuzzer(target + ".buzzer");
  }

  if (!buz->isOnline()) {
    cout << "Module not connected (check identification and USB cable)" << endl;
    return 1;
  }

  serial = buz->get_module()->get_serialNumber();
  leds = YColorLedCluster::FindColorLedCluster(serial + ".colorLedCluster");
  button = YAnButton::FindAnButton(serial + ".anButton1");
  qd = YQuadratureDecoder::FindQuadratureDecoder(serial + ".quadratureDecoder1");

  if ((!button->isOnline()) || (!qd->isOnline())) {
    cout << "Make sure the Yocto-MaxiKnob is configured with at least one AnButton and One Quadrature decoder."
         << endl;
    return 1;
  }

  cout << "press a test button, or turn the encoder or hit Ctrl-C" << endl;
  lastPos = (int) qd->get_currentValue();
  buz->set_volume(75);
  while (button->isOnline()) {
    if (button->isPressed() && (lastPos != 0)) {
      lastPos = 0;
      qd->set_currentValue(0);
      buz->playNotes("'E32 C8");
      leds->set_rgbColor(0, 1, 0x000000);
    } else {
      int p = (int) qd->get_currentValue();
      if (lastPos != p) {
        lastPos = p;
        buz->pulse(notefreq(p), 500);
        leds->set_hslColor(0, 1, 0x00FF7f | (p % 255) << 16);
      }
    }
  }
  YAPI::FreeAPI();
}
