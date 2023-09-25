#ifndef NETWORKDEVICESELECTDIALOG_H
#define NETWORKDEVICESELECTDIALOG_H

#include "supervisor.h"
#include <QDialog>

#include <fstream>

namespace Ui {
class NetworkDeviceSelectDialog;
}

class NetworkDeviceSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NetworkDeviceSelectDialog(QWidget *parent = 0);
    ~NetworkDeviceSelectDialog();

    int Get_IP_ListTable_Size();
    void InsertDevice();

    bool UpdateNetworkSettingFile();

public slots:
    void LineEditFinished();
    void PushButtonAction();
    void AvailableDeleteButton();

private:
    Ui::NetworkDeviceSelectDialog *ui;

    void RemoveNetworkScanner(QString target_ip);
    bool DeleteMessageBox(QString target_ip);

    bool CreateNewNetworkSettingFile(int device_list_size);

    void closeEvent(QCloseEvent* event);
};

#endif // NETWORKDEVICESELECTDIALOG_H
