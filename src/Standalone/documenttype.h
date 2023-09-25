#ifndef DOCUMENT_TYPE_H
#define DOCUMENT_TYPE_H

#include <QComboBox>
#include "supervisor.h"
#include "translationstring.h"

class Document_Type : public QComboBox
{
public:
    bool changed;

    Document_Type(QWidget *parent = 0);

    void initialize(DocumentType type);

    void update_ui(INT_SETTING_VALUE* document_type_para);

    void item_event(int value, INT_SETTING_VALUE* document_type_para);

private:

    void set_enabled(bool enabled);

    void add_item(int32_t item_list[], int32_t list_size);

    void select_item(INT_SETTING_VALUE document_type_para);
};

#endif // DOCUMENT_TYPE_H
