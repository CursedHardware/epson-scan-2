#ifndef DEVICESELECTDIALOG_H
#define DEVICESELECTDIALOG_H

#include <QDialog>
#include "supervisor.h"
#include <QCloseEvent>

namespace Ui {
class DeviceSelectDialog;
}

class DeviceSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeviceSelectDialog(QWidget *parent = 0);
    ~DeviceSelectDialog();

    bool Initialize(Supervisor* sv);

    bool terminate_flag;

protected:
    void closeEvent(QCloseEvent* event);

public slots:
    void onValueChanged(int value);
    void PushEventButton(void);

private:
    Ui::DeviceSelectDialog *ui;
    Supervisor* sv;

    void updateUI(void);
};

#endif // DEVICESELECTDIALOG_H
