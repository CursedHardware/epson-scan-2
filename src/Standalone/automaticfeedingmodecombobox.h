#ifndef AUTOMATICFEEDINGMODECOMBOBOX_H
#define AUTOMATICFEEDINGMODECOMBOBOX_H

#include <QComboBox>
#include "supervisor.h"

class AutomaticFeedingModeCombobox : public QComboBox
{
public:
    AutomaticFeedingModeCombobox(QWidget *parent = 0);

    bool changed;

    bool current_item;

    void initialize(INT_SETTING_VALUE afm_para, INT_SETTING_VALUE cafm_para);

    void update_ui(INT_SETTING_VALUE afm_para, bool use_continuousAutoFeedingMode);

    void item_event(int value, INT_SETTING_VALUE* afm_para, bool* use_continuousAutoFeedingMode);

    void set_enabled(bool enabled);

private:

    void add_item();

    void select_item(INT_SETTING_VALUE afm_para, bool use_continuousAutoFeedingMode);

    bool supported_cafm;

};

#endif // AUTOMATICFEEDINGMODECOMBOBOX_H
