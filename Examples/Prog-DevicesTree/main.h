#ifndef _MAIN_H
#define _MAIN_H
#include <string>
#include <vector>
using std::string;
using std::vector;


class YoctoShield
{
private:
    string _serial;
    vector<string> _subDevices;

public:
    YoctoShield(string serial);
    ~YoctoShield();

    string getSerial();
    bool addSubdevice(string serial);
    void removeSubDevice(string serial);
    void describe();
};

class RootDevice
{
private:
    string _serial;
    string _url;
    vector<YoctoShield> _shields;
    vector<string> _subDevices;
public:
    RootDevice(string serialnumber, string url);
    ~RootDevice();
    string getSerial();
    void addSubDevice(string serial);
    void removeSubDevice(string serial);
    void describe();
};

#endif
