#ifndef PAPERTHICKNESSCOMBOBOX_H
#define PAPERTHICKNESSCOMBOBOX_H

#include <QComboBox>
#include "supervisor.h"

class PaperThicknessComboBox : public QComboBox
{
public:
    PaperThicknessComboBox(QWidget *parent = 0);

    DoubleFeedDetection current_item;

    void initialize(DoubleFeedDetection default_item);

    void update_ui(INT_SETTING_VALUE DoubleFeedDetectionLevel);

    void item_event(int value);

    void set_enabled(bool enabled);

private:
    bool changed;
    bool enabled;

    void add_item(int32_t item_list[], int32_t list_size);

    void select_item();
};

#endif // PAPERTHICKNESSCOMBOBOX_H
