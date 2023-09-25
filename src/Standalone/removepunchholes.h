#ifndef REMOVEPUNCHHOLES_H
#define REMOVEPUNCHHOLES_H

#include <QComboBox>
#include "supervisor.h"

class RemovePunchHoles : public QComboBox
{

public:
    bool changed;

    RemovePunchHoles(QWidget *parent = 0);

    void initialize();

    void update_ui(INT_SETTING_VALUE remove_punchhole_para, QLabel* label1, QLabel* label2);

    void item_event(int value, INT_SETTING_VALUE* remove_punchhole_para);

private:

    void add_item(int32_t item_list[], int32_t list_size);

    void select_item(INT_SETTING_VALUE remove_punchhole_para);

};

#endif // REMOVEPUNCHHOLES_H
