#include <iostream>
#include "yocto_api.h"
#include "yocto_anbutton.h"
#include "yocto_colorled.h"
#include <stdlib.h>

using namespace std;

class ColorMixer
{
private:
  // current Color
  unsigned            _color;
  // vector that contain all registered leds
  vector<YColorLed*>  _leds;

  // internal method that will update the color on all registered leds
  void refreshColor(void)
  {
    unsigned i;
    for (i = 0 ; i < _leds.size() ; i++)
      _leds[i]->set_rgbColor(_color);
  }

  static void redCallback(YAnButton *button, const string& calibratedValue)
  {
    // calculate the red component by scaling the calibratedValue
    // from 0..1000 to 0..255

    unsigned int value  =  atoi(calibratedValue.c_str());
    unsigned char red  =  ( value * 255 / 1000 ) & 0xff;
    // we used the userData to get the pointer to the instance of ColorMixer
    ColorMixer *mixer  =  (ColorMixer*) button->get_userData();
    // update the color
    mixer->changeRed(red);
  }

  static void greenCallback(YAnButton *button, const string& calibratedValue)
  {
    // calculate the green component by scaling the calibratedValue
    // from 0..1000 to 0..255
    unsigned int value  =  atoi(calibratedValue.c_str());
    unsigned char green =  ( value * 255 / 1000 ) & 0xff;
    // we used the userData to get the pointer to the instance of ColorMixer
    ColorMixer *mixer  =  (ColorMixer*) button->get_userData();
    // update the color
    mixer->changeGreen(green);
  }

  static void blueCallback(YAnButton *button, const string& calibratedValue)
  {
    // calculate the blue component by scaling the calibratedValue
    // from 0..1000 to 0..255
    unsigned int value  =  atoi(calibratedValue.c_str());
    unsigned char blue =   (value * 255 / 1000 ) & 0xff;
    // we used the userData to get the pointer to the instance of ColorMixer
    ColorMixer *mixer  =  (ColorMixer*) button->get_userData();
    // update the color
    mixer->changeBlue(blue);
  }



public:
  // constructor
  ColorMixer(void): _color(0) {}

  // register a YoctoLed
  void addLED(YColorLed *led)
  {
    _leds.push_back(led);
  }

  // update only red component
  void changeRed(unsigned char red)
  {
    _color = (_color & 0xffff) | ( red << 16);
    refreshColor();
  }

  // update only geen component
  void changeGreen(unsigned char green)
  {
    _color = (_color & 0xff00ff) | ( green << 8);
    refreshColor();
  }

  // update only blue component
  void changeBlue(unsigned char blue )
  {
    _color = (_color & 0xffff00) |  blue ;
    refreshColor();
  }

  void assignRedButton(YAnButton *button)
  {
    // we store a pointer to the current instance of ColorMixer into
    // the userData Field
    button->set_userData(this);
    // and we register our static method to change red color as callback
    button->registerValueCallback(redCallback);
  }

  void assignGreenButton(YAnButton *button)
  {
    // we store a pointer to the current instance of ColorMixer into
    // the userData Field
    button->set_userData(this);
    // and we register our static method to change green color as callback
    button->registerValueCallback(greenCallback);

  }
  void assignBlueButton(YAnButton *button)
  {
    // we store a pointer to the current instance of ColorMixer into
    // the userData Field
    button->set_userData(this);
    // and we register our static method to change blue color as callback
    button->registerValueCallback(blueCallback);
  }
};




int main(int argc, const char * argv[])
{
  string      errmsg;
  int         i;
  int         nbled = 0;

  cout << "ColorMixer" << endl;
  if(argc < 2) {
    cerr << "usage: " << argv[0] << " [usb | ip_address]" << endl;
    return 1;
  }

  for (i = 1; i < argc ; i++) {
    // Setup the API to use local USB devices
    if(YAPI::RegisterHub(argv[i], errmsg) != YAPI::SUCCESS) {
      cerr << "Unable to get acces to devices on " << argv[i] << endl;
      cerr << "error: " << errmsg << endl;
      return 1;
    }
  }

  YAPI::DisableExceptions();
  // create our ColorMixer Object
  ColorMixer mixer = ColorMixer();

  // get our pointer on our 3 knob
  // we use will reference the 3 knob by the logical name
  // that we have configured using the VirtualHub
  YAnButton* knobRed  = YAnButton::FindAnButton("Red");
  YAnButton* knobGreen = YAnButton::FindAnButton("Green");
  YAnButton* knobBlue  = YAnButton::FindAnButton("Blue");

  // register these 3 knob to the mixer
  mixer.assignRedButton( knobRed );
  mixer.assignGreenButton( knobGreen );
  mixer.assignBlueButton( knobBlue );

  // display a warning if we miss a knob
  if ( ! knobRed->isOnline() )
    cout << "Warning: knob \"" << knobRed->describe() << "\" is not connected" << endl;
  if ( ! knobGreen->isOnline() )
    cout << "Warning: knob \"" << knobGreen->describe() << "\" is not connected" << endl;
  if ( !knobBlue->isOnline() )
    cout << "Warning: knob \"" << knobBlue->describe() << "\" is not connected" << endl;

  // register all led that is connected to our "network"
  YColorLed* led = YColorLed::FirstColorLed();
  while(led) {
    mixer.addLED(led);
    nbled++;
    led = led->nextColorLed();
  }
  cout << nbled << " Color Led detected" << endl;
  // never hanling loop that will..
  while(1) {
    // ... handle all event durring 5000ms without using lots of CPU ...
    YAPI::Sleep(1000, errmsg);
    // ... and check for device plug/unplug
    YAPI::UpdateDeviceList(errmsg);
  }
  return 0;
}
