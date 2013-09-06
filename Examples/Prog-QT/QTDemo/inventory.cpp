#include "inventory.h"
#include "ui_inventory.h"
#include <QDebug>
#include <QTimer>
#include "yocto_api.h"

Inventory::Inventory(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Inventory)
{
    ui->setupUi(this);
    updateInventory();
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateInventory()));
    timer->start(1000);
}

Inventory::~Inventory()
{
    delete ui;
}

void Inventory::on_listWidget_doubleClicked(const QModelIndex &index)
{
    QListWidgetItem *item = ui->listWidget->item(index.row());
    YModule *module = YModule::FindModule(item->text().toStdString());
    if (module) {
        if(module->get_beacon() == YModule::BEACON_ON){
            module->setBeacon(YModule::BEACON_OFF);
            item->setTextColor(Qt::black);
        } else {
            module->setBeacon(YModule::BEACON_ON);
            item->setTextColor(Qt::blue);
        }
    }
}

void Inventory::updateInventory()
{
    string errmsg;
    YAPI::UpdateDeviceList(errmsg);
    ui->listWidget->clear();
    YModule *module = YModule::FirstModule();
    while(module){
        QListWidgetItem *item = new QListWidgetItem(QString(module->get_serialNumber().c_str()));
        if(module->get_beacon() == YModule::BEACON_ON)
            item->setTextColor(Qt::blue);
        else
            item->setTextColor(Qt::black);

        ui->listWidget->addItem(item);
        module = module->nextModule();
    }
}
