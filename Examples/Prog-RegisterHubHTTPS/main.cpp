/*********************************************************************
 *
 *  $Id: main.cpp 62776 2024-09-27 06:43:05Z seb $
 *
 *  Hello world exampel
 *
 *  You can find more information on our web site:
 *   C++ V2 API Reference:
 *      https://www.yoctopuce.com/EN/doc/reference/yoctolib-cpp-EN.html
 *
 *********************************************************************/
#define _CRT_SECURE_NO_WARNINGS 
#include <fstream>
#include <iostream>

#include "yocto_api.h"

using namespace std;

static string load_cert_from_file(const string &host)
{
    FILE *fd;
    long cert_size;
    char *cert_data;
    string path = host + ".crt";
    long size;

    fd = fopen(path.c_str(), "r");
    if (!fd) {
        return "";
    }
    fseek(fd, 0, SEEK_END);
    cert_size = ftell(fd);
    fseek(fd, 0, SEEK_SET);
    cert_data = (char*)malloc(cert_size + 1);
    size = (long) fread(cert_data, 1, cert_size, fd);
    if (size != cert_size) {
        return "";
    }
    fclose(fd);
    return string(cert_data, cert_size);

}

static void save_cert_to_file(const string &host, const string &cert)
{
    string path = host + ".crt";
    std::ofstream out(path);
    out << cert;
    out.close();
}

int main(int argc, const char *argv[])
{
    string errmsg = "";
    string username = "admin";
    string password = "1234";
    string host = "localhost";
    string url = "secure://" + username + ":" + password + "@" + host;

    // load known TLS certificate into the API
    string trusted_cert = load_cert_from_file(host);
    if (trusted_cert != "") {
        string error = YAPI::AddTrustedCertificates(trusted_cert);
        if (error != "") {
            cerr << error << endl;
            return 1;
        }
    }
    // test connection with VirtualHub
    int res = YAPI::TestHub(url, 1000, errmsg);
    if (res == YAPI::SSL_UNK_CERT) {
        // remote TLS certificate is unknown ask user what to do
        cout << "Remote SSL/TLS certificate is unknown" << endl;
        cout << "You can..." << endl;
        cout << " -(A)dd certificate to the API" << endl;
        cout << " -(I)gnore this error and continue" << endl;
        cout << " -(E)xit" << endl;
        cout << "Your choice: ";
        string line;
        cin >> line;
        if (tolower(line[0]) == 'a') {
            // download remote certificate and save it locally
            trusted_cert = YAPI::DownloadHostCertificate(url, 5000);
            if (trusted_cert.find("error") == 0) {
                cerr << trusted_cert << endl;
                return 1;
            }
            save_cert_to_file(host, trusted_cert);
            string error = YAPI::AddTrustedCertificates(trusted_cert);
            if (error != "") {
                cerr << error << endl;
                return 1;
            }
        } else if (tolower(line[0]) == 'i') {
            YAPI::SetNetworkSecurityOptions(YAPI::NO_HOSTNAME_CHECK | YAPI::NO_TRUSTED_CA_CHECK |
                YAPI::NO_EXPIRATION_CHECK);
        } else {
            return 1;
        }
    } else if (res != YAPI::SUCCESS) {
        cerr << "YAPI::TestHub failed:" + errmsg << endl;
        return 1;
    }


    if (YAPI::RegisterHub(url, errmsg) != YAPI::SUCCESS) {
        cerr << "YAPI::RegisterHub failed:" + errmsg << endl;
        return 1;
    }

    cout << "Device list" << endl;

    YModule *module = YModule::FirstModule();
    while (module != NULL) {
        cout << module->get_serialNumber() << " ";
        cout << module->get_productName() << endl;
        module = module->nextModule();
    }
    YAPI::FreeAPI();
    return 0;
}
