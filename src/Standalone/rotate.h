#ifndef ROTATE_H
#define ROTATE_H

#include <QComboBox>
#include "supervisor.h"

class RotateComb : public QComboBox
{

public:
    bool changed;

    RotateComb(QWidget *parent = 0);

    void initialize();

    void focusOutEvent(QFocusEvent* event);

    void update_ui(INT_SETTING_VALUE rotate_para);

    void item_event(int value, INT_SETTING_VALUE* rotate_para);

    Orientation getCurrentItem(void);

private:

    void set_enabled(bool enabled);

    void add_item(int32_t item_list[], int32_t list_size);

    void select_item(INT_SETTING_VALUE rotate_para);
};

#endif // ROTATE_H
