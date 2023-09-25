#ifndef INITDEVICESELECTCOMBOBOX_H
#define INITDEVICESELECTCOMBOBOX_H

#include <QComboBox>
#include "supervisor.h"
#include "networkdeviceselectdialog.h"

class InitDeviceSelectCombobox : public QComboBox
{
public:
    InitDeviceSelectCombobox(QWidget *parent = 0);

    bool initialize();
    void update_device_list();
    void add_device();
    void CreateDisplayName(char displayName[MAX_DISPLAYNAME], const char* connection, int num, int index);
    void item_event(int value);
    void item_event_options(void);

private:
    NetworkDeviceSelectDialog* networkdeviceselect_dialog;

    bool changed;

    void add_option();
    void add_manu_ip();

    void select_item();
};

#endif // INITDEVICESELECTCOMBOBOX_H
