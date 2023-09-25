#ifndef BEHAVIORWHENDOUBLEFEEDCOMBOBOX_H
#define BEHAVIORWHENDOUBLEFEEDCOMBOBOX_H

#include <QComboBox>
#include "supervisor.h"

class BehaviorWhenDoubleFeedCombobox : public QComboBox
{
public:
    BehaviorWhenDoubleFeedCombobox(QWidget *parent = 0);

    void initialize(BehaviorWhenDoubleFeed default_item);

    void update_ui(INT_SETTING_VALUE behavior_when_double_feed_para);

    void item_event(int value);

    void set_enabled(bool enabled);

    BehaviorWhenDoubleFeed get_current_item(void);

private:
    bool changed;

    bool enabled;

    BehaviorWhenDoubleFeed current_item;

    void add_item(int32_t item_list[], int32_t list_size);

    void select_item(void);
};

#endif // BEHAVIORWHENDOUBLEFEEDCOMBOBOX_H
