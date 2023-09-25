#ifndef ADFPAPERPROTECTIONCOMBOBOX_H
#define ADFPAPERPROTECTIONCOMBOBOX_H

#include <QComboBox>
#include "supervisor.h"

class ADFPaperProtectionCombobox : public QComboBox
{
public:
    ADFPaperProtectionCombobox(QWidget *parent = 0);

    void initialize(ADFPaperProtection default_item);

    void update_ui(INT_SETTING_VALUE paper_protection_para);

    void item_event(int value);

    void set_enabled(bool enabled);

    ADFPaperProtection get_current_item(void);

private:
    bool changed;

    bool enabled;

    ADFPaperProtection current_item;

    void add_item(int32_t item_list[], int32_t list_size);

    void select_item();
};

#endif // ADFPAPERPROTECTIONCOMBOBOX_H
