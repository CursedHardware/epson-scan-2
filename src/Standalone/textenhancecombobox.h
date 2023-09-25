#ifndef TEXTENHANCECOMBOBOX_H
#define TEXTENHANCECOMBOBOX_H

#include <QComboBox>
#include "supervisor.h"

class TextEnhanceCombobox : public QComboBox
{
public:
    bool changed;

    TextEnhanceCombobox(QWidget *parent = 0);

    TextEnhance current_item;

    void initialize(INT_SETTING_VALUE textenhance_para);

    void update_ui(INT_SETTING_VALUE textenhance_para);

    void item_event(int value, INT_SETTING_VALUE* textenhance_para);

    void set_enabled(bool enabled);

private:

    void add_item(int32_t item_list[], int32_t list_size);

    void set_visible(SDISupportLevel level);

    void select_item();

};

#endif // TEXTENHANCECOMBOBOX_H
