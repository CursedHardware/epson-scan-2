#ifndef SKIPBLANKPAGESCOMB_H
#define SKIPBLANKPAGESCOMB_H

#include <QComboBox>
#include "supervisor.h"
#include "skipblankpagessettingsdialog.h"

class SkipblankpagesComb : public QComboBox
{
public:
    SkipblankpagesComb(QWidget *parent = 0);
    bool current_item;
    int current_level;
    bool changed;

    void initialize(SCANPARA skipblankpageslv_para);

    void focusOutEvent(QFocusEvent* event);

    void update_ui(INT_SETTING_VALUE* skipblankpages_para);

    void item_event(int value, SCANPARA* skipblankpageslv_para);
    void item_event_options(void);

private:
    SkipBlankPagesSettingsDialog* skipblankpagessettings_dialog;

    void set_enabled(bool enabled);

    void add_item();

    void select_item(void);

};

#endif // SKIPBLANKPAGESCOMB_H
