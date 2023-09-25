#ifndef SCANNINGSIDE_H
#define SCANNINGSIDE_H

#include <QComboBox>
#include "supervisor.h"

class Scanning_Side : public QComboBox
{

public:
    bool changed;

    Scanning_Side(QWidget *parent = 0);

    void initialize(ScanningSide type);

    void update_ui(INT_SETTING_VALUE* scanning_side_para);

    void item_event(int value, INT_SETTING_VALUE* scanning_side_para);

private:

    void set_enabled(bool enabled);

    void add_item(int32_t item_list[], int32_t list_size);

    void select_item(INT_SETTING_VALUE scanning_side_para);
};

#endif // SCANNINGSIDE_H
