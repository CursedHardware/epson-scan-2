#ifndef BACKGROUNDCOLORCOMBOBOX_H
#define BACKGROUNDCOLORCOMBOBOX_H

#include <QComboBox>
#include "supervisor.h"

class BackgroundColorCombobox : public QComboBox
{
public:
    BackgroundColorCombobox(QWidget *parent = 0);

    int current_item;
    bool changed;

    void initialize(SCANPARA backgroundcolor_para);

    void update_ui(INT_SETTING_VALUE backgroundcolor_para);

    void item_event(int value, INT_SETTING_VALUE* backgroundcolor_para);

private:

    void set_enabled(bool enabled);

    void add_item();

    void select_item();

};

#endif // BACKGROUNDCOLORCOMBOBOX_H
