#ifndef DEVICECONNECTIONDIALOG_H
#define DEVICECONNECTIONDIALOG_H

#include <QDialog>
#include <QTimer>
#include "supervisor.h"

struct CONNECTINFO{
    QDialog* dialog;
    Supervisor* sv;
    SDIDeviceInfo select_Device;
    bool use_lastsettings;
    bool connection_result;
    bool terminate_connection;
};

namespace Ui {
class DeviceConnectionDialog;
}

class DeviceConnectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeviceConnectionDialog(Supervisor* sv, SDIDeviceInfo select_Device, bool use_lastsettings, QWidget *parent = 0);
    ~DeviceConnectionDialog();

    static void* ConnectToDevice(void* vpContext);
    void Initialize(void);
    bool GetConnectionResult(void);

private:
    Ui::DeviceConnectionDialog *ui;

    void ResetOnlyStandaloneVal(Supervisor* sv);
    void closeEvent(QCloseEvent* event);

    CONNECTINFO m_connect_info;

    QTimer* m_timer;

private slots:
    void CheckConnection();
};

#endif // DEVICECONNECTIONDIALOG_H
