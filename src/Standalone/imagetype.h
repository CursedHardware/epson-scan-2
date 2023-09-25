#ifndef IMAGETYPE_H
#define IMAGETYPE_H

#include <QComboBox>
#include "supervisor.h"
#include "imagetypesettingsdialog.h"

class Image_Type : public QComboBox
{

public:
    bool changed;

    Image_Type(QWidget *parent = 0);

    void initialize(INT_SETTING_VALUE* document_type_para);

    void update_ui(INT_SETTING_VALUE image_type_para);

    void item_event(int value, INT_SETTING_VALUE* image_type_para, Supervisor* sv);

private:

    void set_enabled(bool enabled);

    void add_item(int32_t item_list[], int32_t list_size);

    void select_item(INT_SETTING_VALUE image_type_para);

    void item_event_options(Supervisor* sv);

    ImageTypeSettingsDialog* m_ImageTypeSetting_Dialog;

    INT_SETTING_VALUE* m_document_type_para;

};

#endif // IMAGETYPE_H
