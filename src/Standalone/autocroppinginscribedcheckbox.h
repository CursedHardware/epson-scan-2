#ifndef AUTOCROPPINGINSCRIBEDCHECKBOX_H
#define AUTOCROPPINGINSCRIBEDCHECKBOX_H

#include <QCheckBox>
#include "supervisor.h"
#include <QFocusEvent>

class AutoCroppingInscribedCheckBox : public QCheckBox
{

public:

    AutoCroppingInscribedCheckBox(QWidget *parent = 0);

    void initialize(bool default_val);

    void focusOutEvent(QFocusEvent* event);

    void update_ui(INT_SETTING_VALUE inscribed_para);

    void item_event(bool value, INT_SETTING_VALUE* inscribed_para);

private:

    bool current_item;
    bool changed;

    void set_enabled(bool enabled);

    void select_item(bool select);
};

#endif // AUTOCROPPINGINSCRIBEDCHECKBOX_H
