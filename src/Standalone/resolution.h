#ifndef RESOLUTION_H
#define RESOLUTION_H

#include <QComboBox>
#include "supervisor.h"

class Resolution : public QComboBox
{

public:
    Resolution(QWidget *parent = 0);

    bool changed;
    ResolutionList current_item;
    int current_value;

    void focusOutEvent(QFocusEvent* event);

    void initialize(INT_SETTING_VALUE resolution_para);

    void update_ui(INT_SETTING_VALUE resolution_para, bool isLFPModel);

    void validate_input();

    void item_event(int value, INT_SETTING_VALUE* resolution_para);

private:

    QIntValidator* validator;

    void set_enabled(bool enabled);

    void create_list(INT_SETTING_VALUE resolution_para, bool isLFPModel);

    void select_item();

    int IdToValue(ResolutionList item);
    ResolutionList ValueToId(int res);

};

#endif // RESOLUTION_H
