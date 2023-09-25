#ifndef DEVICESELECTORCOMBOBOX_H
#define DEVICESELECTORCOMBOBOX_H

#include <QComboBox>
#include "supervisor.h"
#include "networkdeviceselectdialog.h"
#include "waitingdialog.h"

class DeviceSelectorCombobox : public QComboBox
{
public:
    DeviceSelectorCombobox(QWidget *parent = 0);

    void initialize();
    void finder(void);
    void update_device_list();
    void add_device();
    void CreateDisplayName(char displayName[MAX_DISPLAYNAME], const char* connection, int num, int index);
    void item_event(int value);
    void item_event_options();
    SDIDeviceInfo get_cur_device();

private:
    NetworkDeviceSelectDialog* networkdeviceselect_dialog;
    WaitingDialog* wait_dialog;

    bool changed;

    void add_option();
    void add_manu_ip();

    void select_item();
};

#endif // DEVICESELECTORCOMBOBOX_H
