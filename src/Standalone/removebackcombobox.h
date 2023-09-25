#ifndef REMOVEBACKCOMBOBOX_H
#define REMOVEBACKCOMBOBOX_H

#include <QComboBox>
#include "supervisor.h"

class RemoveBackComboBox : public QComboBox
{
public:
    bool changed;

    RemoveBackComboBox(QWidget *parent = 0);

    BackgroundRemoval current_item;

    void initialize(INT_SETTING_VALUE remove_background_para);

    void update_ui(INT_SETTING_VALUE remove_background_para);

    void item_event(int value, INT_SETTING_VALUE* remove_background_para);

    void set_enabled(bool enabled);

private:

    void set_visible(SDISupportLevel level);
    void add_item(int32_t item_list[], int32_t list_size);

    void select_item();

};

#endif // REMOVEBACKCOMBOBOX_H
