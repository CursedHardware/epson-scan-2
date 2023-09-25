#ifndef DIRECTPOWERONCOMBOBOX_H
#define DIRECTPOWERONCOMBOBOX_H

#include <QComboBox>
#include "supervisor.h"

class DirectPowerOnCombobox : public QComboBox
{
public:
    DirectPowerOnCombobox(QWidget *parent = 0);

    void initialize(DirectPowerOn default_item);

    void update_ui(INT_SETTING_VALUE direct_power_on_para);

    void item_event(int value);

    void set_enabled(bool enabled);

    DirectPowerOn get_current_item(void);

private:
    bool changed;

    bool enabled;

    DirectPowerOn current_item;

    void add_item(int32_t item_list[], int32_t list_size);

    void select_item(void);
};

#endif // DIRECTPOWERONCOMBOBOX_H
