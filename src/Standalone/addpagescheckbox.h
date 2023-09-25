#ifndef ADDPAGESCHECKBOX_H
#define ADDPAGESCHECKBOX_H

#include <QCheckBox>
#include "capitem.h"

class AddpagesCheckbox : public QCheckBox
{
public:
    AddpagesCheckbox(QWidget *parent = 0); 

    void initialize(bool default_val);

    void focusOutEvent(QFocusEvent* event);

    void update_ui();

    void item_event(bool value, bool* add_pages);

    bool get_current_item();

private:
    bool current_item;
    bool changed;

    void set_enabled(bool enabled);

    void select_item(bool select);
};

#endif // ADDPAGESCHECKBOX_H
