#ifndef AFMTIMEOUTCOMBOBOX_H
#define AFMTIMEOUTCOMBOBOX_H

#include <QComboBox>
#include "supervisor.h"

class AfmTimeoutCombobox : public QComboBox
{
public:
    AfmTimeoutCombobox(QWidget *parent = 0);

    AutomaticModeTimeout current_item;

    void initialize(INT_SETTING_VALUE afm_para);

    void update_ui();

    void item_event(int value);

private:

    void set_enabled(bool enabled);

    void add_item();

    void select_item();
};

#endif // AFMTIMEOUTCOMBOBOX_H
