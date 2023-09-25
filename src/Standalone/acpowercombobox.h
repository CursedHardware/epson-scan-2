#ifndef ACPOWERCOMBOBOX_H
#define ACPOWERCOMBOBOX_H

#include <QComboBox>
#include "supervisor.h"

class ACPowerComboBox : public QComboBox
{
public:
    ACPowerComboBox(QWidget *parent = 0);

    void initialize(Time default_item);

    void update_ui(INT_SETTING_VALUE power_of_time_para);

    void item_event(int value);

    void set_enabled(bool enabled);

    Time get_current_item(void);

private:
    bool changed;

    bool enabled;

    Time current_item;

    void add_item(int32_t item_list[], int32_t list_size);

    void select_item(Time select);

};

#endif // ACPOWERCOMBOBOX_H
