#ifndef DESKEWCHECKBOX_H
#define DESKEWCHECKBOX_H

#include <QCheckBox>
#include "supervisor.h"

class DeskewCheckbox : public QCheckBox
{

public:
    bool changed;

    DeskewCheckbox(QWidget *parent = 0);

    void initialize();

    void focusOutEvent(QFocusEvent* event);

    void update_ui(INT_SETTING_VALUE deskew_para);

    void item_event(bool value, INT_SETTING_VALUE* deskew_para);

private:

    void set_enabled(bool enabled);

    void select_item(bool select);
};

#endif // DESKEWCHECKBOX_H
