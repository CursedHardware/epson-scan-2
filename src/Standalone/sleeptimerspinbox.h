#ifndef SLEEPTIMERSPINBOX_H
#define SLEEPTIMERSPINBOX_H

#include <QFocusEvent>
#include <QSpinBox>
#include "supervisor.h"

class SleepTimerSpinBox : public QSpinBox
{
public:
    SleepTimerSpinBox(QWidget *parent = 0);

    void initialize(INT_SETTING_VALUE sleep_time_para);

    void update_ui(void);

    void item_event(int value);

    int get_current_item(void);

private:
    bool changed;
    bool enabled;

    int current_value;

    void set_enabled(bool enabled);

    void select_item(int select);

};

#endif // SLEEPTIMERSPINBOX_H
