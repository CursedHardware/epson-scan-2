#ifndef DROPOUTCOMBOBOX_H
#define DROPOUTCOMBOBOX_H

#include <QComboBox>
#include "supervisor.h"

class DropoutCombobox : public QComboBox
{
public:
    bool changed;

    DropoutCombobox(QWidget *parent = 0);

    DropoutColor current_item;

    void initialize(INT_SETTING_VALUE dropout_para);

    void update_ui(INT_SETTING_VALUE dropout_para);

    void item_event(int value, INT_SETTING_VALUE* dropout_para);

    void set_enabled(bool enabled);

private:

    void add_item(int32_t item_list[], int32_t list_size);

    void select_item();

};

#endif // DROPOUTCOMBOBOX_H
