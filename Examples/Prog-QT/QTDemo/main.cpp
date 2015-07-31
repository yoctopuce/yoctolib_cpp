#include "inventory.h"
#include <QApplication>
#include <QDebug>
#include "yocto_api.h"

void yocto_debug_log_callback(const string& message)
{
    qDebug(message.c_str());
}
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    YAPI::RegisterLogFunction(yocto_debug_log_callback);
    string errmsg;
    if (YAPI::InitAPI(YAPI::DETECT_USB,errmsg)!= YAPI::SUCCESS){
        qDebug("Unable to start Yoctopuce API");
        qDebug(errmsg.c_str());
        exit(1);
    }
    Inventory w;
    w.show();
    return a.exec();
}
