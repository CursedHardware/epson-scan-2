#ifndef NONCONNECTPOWEROFFCOMBOBOX_H
#define NONCONNECTPOWEROFFCOMBOBOX_H

#include <QComboBox>
#include "supervisor.h"

class NonConnectPowerOffCombobox : public QComboBox
{
public:
    NonConnectPowerOffCombobox(QWidget *parent = 0);

    void initialize(NonConnectPowerOff default_item);

    void update_ui(INT_SETTING_VALUE non_connect_power_off_para);

    void item_event(int value);

    void set_enabled(bool enabled);

    NonConnectPowerOff get_current_item(void);

private:
    bool changed;

    bool enabled;

    NonConnectPowerOff current_item;

    void add_item(int32_t item_list[], int32_t list_size);

    void select_item(void);
};

#endif // NONCONNECTPOWEROFFCOMBOBOX_H
