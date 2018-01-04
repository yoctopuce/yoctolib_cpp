#include "yocto_api.h"
#include "yocto_pwminput.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

static void usage(void)
{
  cout << "usage: demo <serial_number> " << endl;
  cout << "       demo <logical_name>" << endl;
  cout << "       demo any                 (use any discovered device)" << endl;
  u64 now = yGetTickCount();
  while (yGetTickCount() - now < 3000) {
    // wait 3 sec to show the message
  }
  exit(1);
}

int main(int argc, const char * argv[])
{
  string       errmsg;
  string       target;
  YPwmInput   *pwm;
  YPwmInput   *pwm1;
  YPwmInput   *pwm2;
  YModule     *m;

  if (argc < 2) {
    usage();
  }
  target = (string) argv[1];

  YAPI::DisableExceptions();

  // Setup the API to use local USB devices
  if (YAPI::RegisterHub("usb", errmsg) != YAPI_SUCCESS) {
    cerr << "RegisterHub error: " << errmsg << endl;
    return 1;
  }

  if (target == "any") {
    // retreive any pwm input available
    pwm = YPwmInput::FirstPwmInput();
    if (pwm == NULL) {
      cerr << "No module connected (Check cable)" << endl;
      exit(1);
    }
  } else {
    // retreive the first pwm input from the device given on command line
    pwm = YPwmInput::FindPwmInput(target + ".pwmInput1");
  }

  // we need to retreive both channels from the device.
  if (pwm->isOnline()) {
    m = pwm->get_module();
    pwm1 = YPwmInput::FindPwmInput(m->get_serialNumber() + ".pwmInput1");
    pwm2 = YPwmInput::FindPwmInput(m->get_serialNumber() + ".pwmInput2");
  } else {
    cerr << "No module connected (Check cable)" << endl;
    exit(1);
  }
  while (pwm1->isOnline()) {
    cout << "PWM1 : " << pwm1->get_frequency() << " Hz " << pwm1->get_dutyCycle()
         << " %  " << pwm1->get_pulseCounter() << "pulses edges" << endl;
    cout << "PWM2 : " << pwm2->get_frequency() << "  Hz " << pwm2->get_dutyCycle()
         << " %  " << pwm2->get_pulseCounter() << " pulses edges" << endl;

    cout << "  (press Ctrl-C to exit)" << endl;
    YAPI::Sleep(1000, errmsg);
  }
  cout << "Module disconnected" << endl;
  yFreeAPI();
  return 0;
}
