#ifndef DOCUMENTSOURCE_H
#define DOCUMENTSOURCE_H

#include <QComboBox>
#include "supervisor.h"
#include "documentsourcesettingsdialog.h"

class Document_Source : public QComboBox
{

public:
    FunctionalUnit current_item;

    Document_Source(QWidget *parent = 0);

    void initialize(INT_SETTING_VALUE document_source_para, bool functionalunit_auto);

    void update_ui(INT_SETTING_VALUE document_source_para, SDISupportLevel AFM_supported_level);

    void item_event(int value, SCANPARA* doc_source_para);

    bool changed;

private:

    DocumentSourceSettingsDialog* m_documentsourcesettings_dialog;

    void set_enabled(bool enabled);

    void add_item(int32_t item_list[], int32_t list_size, bool AFM_supported);

    void select_item();

    void item_event_options(INT_SETTING_VALUE* afm_timeout_para);

    bool search_item_list(int item, int32_t item_list[], int32_t list_size);

};

#endif // DOCUMENTSOURCE_H
