#ifndef INVENTORY_H
#define INVENTORY_H

#include <QMainWindow>
#include <QModelIndex>
namespace Ui {
class Inventory;
}

class Inventory : public QMainWindow
{
    Q_OBJECT

public:
    explicit Inventory(QWidget *parent = 0);
    ~Inventory();

private slots:
    void on_listWidget_doubleClicked(const QModelIndex &index);

private:
    Ui::Inventory *ui;

public slots:
    void updateInventory();
};

#endif // INVENTORY_H
