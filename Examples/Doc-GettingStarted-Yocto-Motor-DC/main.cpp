#include "yocto_api.h"
#include "yocto_motor.h"
#include "yocto_temperature.h"
#include "yocto_current.h"
#include "yocto_voltage.h"
#include <iostream>
#include <ctype.h>
#include <stdlib.h>

using namespace std;

static void usage(void)
{
    cout << "usage: demo <serial_number> power" << endl;
    cout << "       demo <logical_name>  power" << endl;
    cout << "       demo any power       (use any discovered device)" << endl;
    cout << "       power is a integer between -100 and 100%%" << endl;
    cout << "Example:" << endl;
    cout << "       demo any 75" << endl;

    u64 now = yGetTickCount();
    while (yGetTickCount() - now < 3000) {
        // wait 3 sec to show the message
    }
    exit(1);
}

int main(int argc, const char * argv[])
{
    string  target, errmsg;
    int power;
    YMotor *motor;
    YCurrent *current;
    YVoltage *voltage;
    YTemperature *temperature;

    // parse command line
    if (argc < 3) {
        usage();
    }
    target = (string) argv[1];
    power = atoi(argv[2]);

    // Setup the API to use local USB devices
    if (YAPI::RegisterHub("usb", errmsg) != YAPI_SUCCESS) {
        cerr << "RegisterHub error: " << errmsg << endl;
        return 1;
    }

    if (target == "any") {
        // find the serial# of the first available motor
        motor =  YMotor::FirstMotor();
        if (motor == NULL) {
            cout << "No module connected (check USB cable)" << endl;
            return 1;
        }
        target = motor->get_module()->get_serialNumber();
    }

    // retreive motor, current, voltage and temperature features from the device
    motor       = YMotor::FindMotor(target + ".motor");
    current     = YCurrent::FindCurrent(target + ".current");
    voltage     = YVoltage::FindVoltage(target + ".voltage");
    temperature = YTemperature::FindTemperature(target + ".temperature");

    // lets start the motor
    if (motor->isOnline()) {
        // if motor is in error state, reset it.
        if (motor->get_motorStatus() >= YMotor::MOTORSTATUS_LOVOLT) {
            motor->resetStatus();
        }
        motor->drivingForceMove(power, 2000); // ramp up to power in 2 seconds
        while (motor->isOnline()) {
            // display motor status
            cout << "Status=" << motor->get_advertisedValue() << "  "
                 << "Voltage=" << voltage->get_currentValue() << "V  "
                 << "Current=" << current->get_currentValue() / 1000 << "A  "
                 << "Temp=" << temperature->get_currentValue() << "deg C" << endl;
            YAPI::Sleep(1000, errmsg); // wait for one second
        }
    } else {
        cout << "Module not connected (check identification and USB cable)" << endl;
    }
    yFreeAPI();
    return 0;
}
