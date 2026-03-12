/*********************************************************************
 *
 *  $Id: main.cpp 59576 2024-02-29 09:36:30Z seb $
 *
 *  An example that show how to send and receive SMS
 *
 *  You can find more information on our web site:
 *   YoctoHub-GSM-4G documentation:
 *      https://www.yoctopuce.com/EN/products/yoctohub-gsm-4g/doc.html
 *   C++ V2 API Reference:
 *      https://www.yoctopuce.com/EN/doc/reference/yoctolib-cpp-EN.html
 *
 *********************************************************************/

#include "yocto_api.h"
#include "yocto_messagebox.h"
#include <iostream>
#include <stdlib.h>

using namespace std;

void smsCallback(YMessageBox *mbox, YSms sms)
{
    cout << "- dated " << sms.get_timestamp() << endl;
    cout << "  from " + sms.get_sender() << endl;
    cout << "  '" + sms.get_textData() << "'" << endl;
    sms.deleteFromSIM();
}

int main(int argc, const char * argv[])
{
    string errmsg;
    string target;
    YMessageBox *mbox;

    // Setup the API to use local USB devices
    if (YAPI::RegisterHub("usb", errmsg) != YAPI::SUCCESS) {
        cerr << "RegisterHub error: " << errmsg << endl;
        return 1;
    }

    // find the GSM hub according to command line parameters
    if (argc > 1) {
        target = (string)argv[1];
        mbox = YMessageBox::FindMessageBox(target + ".messageBox");
    } else {
        mbox = YMessageBox::FirstMessageBox();
        if (mbox == NULL) {
            cerr << "No GSM hub connected (check USB cable)" << endl;
            return 1;
        }
    }

    // list messages found on the device
    cout << "Messages found on the SIM card:" << endl;
    vector<YSms> messages = mbox->get_messages();
    if (messages.size() == 0) {
        cout << "* None" << endl;
    }
    for (size_t i = 0; i < messages.size(); i++) {
        YSms sms = messages[i];
        cout << "- dated " << sms.get_timestamp() << endl;
        cout << "  from " + sms.get_sender() << endl;
        cout << "  '" + sms.get_textData() << "'" << endl;
    }

    // register a callback to receive any new message
    mbox->registerSmsCallback(smsCallback);

    // offer to send a new message
    cout << "To test sending SMS, provide message recipient (+xxxxxxx)." << endl;
    cout << "To skip sending, leave empty and press Enter." << endl;
    string number;
    cin >> number;
    if (number != "") {
        mbox->sendTextMessage(number, "Hello from YoctoHub-GSM !");
    }

    cout << "Waiting to receive SMS, press Ctrl-C to quit" << endl;
    while (1) {
        YAPI::Sleep(500, errmsg);
    }

    YAPI::FreeAPI();
    return 0;
}
