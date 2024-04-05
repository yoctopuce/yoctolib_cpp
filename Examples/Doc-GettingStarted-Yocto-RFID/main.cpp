/*********************************************************************
 *
 *  $Id: main.cpp 59991 2024-03-18 17:25:52Z seb $
 *
 *  An example that shows how to use a  Yocto-RFID-15693
 *
 *  You can find more information on our web site:
 *   Yocto-RFID-15693 documentation:
 *      https://www.yoctopuce.com/EN/products/yocto-rfid-15693/doc.html
 *   C++ API Reference:
 *      https://www.yoctopuce.com/EN/doc/reference/yoctolib-cpp-EN.html
 *
 *********************************************************************/

#include "yocto_api.h"
#include "yocto_rfidreader.h"
#include "yocto_buzzer.h"
#include "yocto_anbutton.h"
#include "yocto_colorledcluster.h"

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

int main(int argc, const char* argv[])
{
  string errmsg;
  string target, serial;
  YBuzzer* buz;
  YColorLedCluster* leds;
  YAnButton* button;
  YRfidReader* reader;

  if (argc < 2) { usage(); }
  target = (string)argv[1];

  // Setup the API to use local USB devices
  if (YAPI::RegisterHub("usb", errmsg) != YAPI_SUCCESS) 
  { cerr << "RegisterHub error: " << errmsg << endl;
    return 1;
  }

  if (target == "any") 
   {reader = YRfidReader::FirstRfidReader();
    if (reader == NULL)
    { cout << "No module connected (check USB cable)" << endl;
      return 1;
    }
  } else { reader = YRfidReader::FindRfidReader(target + ".rfidReader");}

  if (!reader->isOnline()) 
  { cout << "Module not connected (check identification and USB cable)" << endl;
    return 1;
  }

  serial = reader->get_module()->get_serialNumber();
  leds = YColorLedCluster::FindColorLedCluster(serial + ".colorLedCluster");
  button = YAnButton::FindAnButton(serial + ".anButton1");
  buz   = YBuzzer::FindBuzzer(serial + ".buzzer");

  leds->set_rgbColor(0, 1, 0x000000);
  buz->set_volume(75);
  cout << "Place a RFID tag near the antenna" << endl;
  vector<string> tagList ;
  
  do
  { tagList = reader->get_tagIdList();
  }
  while (tagList.size() <= 0);
  
  string tagId = tagList[0];
  YRfidStatus opStatus =  YRfidStatus();
  YRfidOptions  options =  YRfidOptions();

  YRfidTagInfo  taginfo = reader->get_tagInfo(tagId, opStatus);
  int  blocksize = taginfo.get_tagBlockSize();
  int   firstBlock = taginfo.get_tagFirstBlock();
  cout << "Tag ID          = " << taginfo.get_tagId() << endl;
  cout << "Tag Memory size = " << taginfo.get_tagMemorySize()<< endl;
  cout << "Tag Block  size = "<< taginfo.get_tagBlockSize() << endl;

  string  data = reader->tagReadHex(tagId, firstBlock, 3 * blocksize, options, opStatus);
  if (opStatus.get_errorCode() == YRfidStatus::SUCCESS) 
     { cout << "First 3 blocks  = " <<  data << endl;
       leds->set_rgbColor(0, 1, 0x00FF00);
       buz->pulse(1000, 100);
     }
  else
  {
    cout << "Cannot read tag contents (" << opStatus.get_errorMessage() << ")" << endl;
    leds->set_rgbColor(0, 1, 0xFF0000);
  }
  leds->rgb_move(0, 1, 0x000000, 200);
  YAPI::FreeAPI();
}
