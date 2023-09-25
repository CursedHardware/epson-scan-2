#ifndef UNITSCOMBOBOX_H
#define UNITSCOMBOBOX_H

#include <QComboBox>
#include "capitem.h"

class UnitsComboBox : public QComboBox
{
public:
    UnitsComboBox(QWidget *parent = 0);

    void initialize(Units default_item);

    void update_ui(void);

    void item_event(int value);

    void set_enabled(bool enabled);

    Units get_current_item(void);

private:
    bool changed;
    bool enabled;

    std::list<Units> units_list;

    void add_item(void);

    void select_item(Units select);

    Units current_item;
};

#endif // UNITSCOMBOBOX_H
