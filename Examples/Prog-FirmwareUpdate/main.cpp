#include <iostream>
#include <stdlib.h>

#include "yocto_api.h"
using namespace std;


static int upgradeSerialList(vector<string> allserials)
{
    string errmsg;


    for (std::vector<string>::iterator it = allserials.begin(); it != allserials.end();
         ++it) {
        string serial = *it;
        YModule* module = YModule::FindModule(serial);
        string path = "www.yoctopuce.com";
        YFirmwareUpdate update;
        if (module->isOnline()) {
            string product = module->get_productName();
            string current = module->get_firmwareRelease();

            // check if a new firmare is available on yoctopuce.com
            string newfirm = module->checkFirmware(path, true);
            if (newfirm == "") {
                cout << product << " " << serial << "(rev=" << current << ") is up to date" << endl;
                continue;
            } else {
                cout << product << " " << serial << "(rev=" << current <<
                    ") need be updated with firmare : " << endl;
                cout << "    " << newfirm << endl;
                // execute the firmware upgrade
                update = module->updateFirmware(newfirm);
            }
        } else {
            update = YFirmwareUpdate(serial, path, "", true);
        }
        int status = update.startUpdate();
        do {
            int newstatus = update.get_progress();
            if (newstatus != status)
                cout << newstatus << "% " << update.get_progressMessage() << endl;
            YAPI::Sleep(500, errmsg);
            status = newstatus;
        } while (status < 100 && status >= 0);
        if (status < 0) {
            cout << "Firmware Update failed: " << update.get_progressMessage() << endl;
            exit(1);
        } else {
            if (module->isOnline()) {
                cout << status << "% Firmware Updated Successfully!" << endl;
            } else {
                cout << status << " Firmware Update failed: module " << serial << " is not online" <<
                    endl;
                exit(1);
            }
        }
    }

    return 0;
}

int main(int argc, const char* argv[])
{
    int i;
    string errmsg;
    vector<string> hubs;
    vector<string> shield;
    vector<string> devices;

    // Setup the API to use local USB devices
    if (YAPI::RegisterHub("usb", errmsg) != YAPI::SUCCESS) {
        cerr << "RegisterHub error: " << errmsg << endl;
        return 1;
    }


    for (i = 1; i < argc; i++) {
        string hub_url = string(argv[i]);
        cout << "Update module connected to hub " << hub_url << endl;
        if (YAPI::RegisterHub(hub_url, errmsg) != YAPI::SUCCESS) {
            cerr << "RegisterHub error: " << errmsg << endl;
            return 1;
        }
    }

    vector<string> all_boot_loaders = YFirmwareUpdate::GetAllBootLoaders();
    for (i = 0; i < (int) all_boot_loaders.size(); i++) {
        devices.push_back(all_boot_loaders[i]);
    }

    //first step construct the list of all hub /shield and devices connected
    YModule* module = YModule::FirstModule();
    while (module) {
        string product = module->get_productName();
        string serial = module->get_serialNumber();
        if (product == "YoctoHub-Shield") {
            shield.push_back(serial);
        } else if (product.substr(0, 9) == "YoctoHub-") {
            hubs.push_back(serial);
        } else if (product != "VirtualHub") {
            devices.push_back(serial);
        }
        module = module->nextModule();
    }
    // first upgrades all Hubs...
    upgradeSerialList(hubs);
    // ... then all shield..
    upgradeSerialList(shield);
    // ... and finaly all devices
    upgradeSerialList(devices);
    cout << "All devices are now up to date" << endl;
    YAPI::FreeAPI();
    return 0;
}
