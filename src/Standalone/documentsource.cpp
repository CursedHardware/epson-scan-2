#include "mainwindow.h"

Document_Source::Document_Source(QWidget *parent) :
    QComboBox(parent),
    changed(false),
    m_documentsourcesettings_dialog(NULL),
    current_item(ADF)
{
}

void Document_Source::initialize(INT_SETTING_VALUE document_source_para, bool functionalunit_auto){
    changed = true;
    if(functionalunit_auto && this->search_item_list(kSDIFunctionalUnitFlatbed, document_source_para.capability.list, document_source_para.capability.countOfList)){
        current_item = Auto;
    }else {
        current_item = (FunctionalUnit)document_source_para.select;
    }
}

void Document_Source::update_ui(INT_SETTING_VALUE document_source_para, SDISupportLevel AFM_supported_level)
{
    if(document_source_para.capability.supportLevel == kSDISupportLevelAvailable){
        this->set_enabled(true);
    }else {
        this->set_enabled(false);
    }

    bool AFM_supported = false;
    if(AFM_supported_level != kSDISupportLevelNone){
        AFM_supported = true;
    }

    if(changed == true){
        this->clear();
        this->add_item(document_source_para.capability.list, document_source_para.capability.countOfList, AFM_supported);
        this->select_item();

        changed = false;
    }
}

void Document_Source::set_enabled(bool enabled)
{
    this->setEnabled(enabled);
}

void Document_Source::add_item(int32_t item_list[], int32_t list_size, bool AFM_supported){

    int i = 0;

    while(i < list_size){
        if(item_list[i] == kSDIFunctionalUnitFlatbed){
            this->insertItem(i, TranslationString::GetString().translation_str.MAINSETTING_004, Flatbed);
        }else if(item_list[i] == kSDIFunctionalUnitDocumentFeeder){
            this->insertItem(i, TranslationString::GetString().translation_str.MAINSETTING_005, ADF);
        }else if(item_list[i] == kSDIFunctionalUnitCarrierSheet){
            this->insertItem(i, TranslationString::GetString().translation_str.MAINSETTING_006, Carrier_Sheet);
        }
        i++;
    }
    //FBとADFを両方サポートしている場合Autoを追加する
    if(this->findData(Flatbed) != -1 && this->findData(ADF) != -1){
        this->insertItem(i, TranslationString::GetString().translation_str.MAINSETTING_003, Auto);
        i++;
    }
    //AFMをサポートしている場合はAFMのオプションを追加する
    if(AFM_supported){
        this->insertItem(i, TranslationString::GetString().translation_str.CONFIG_DOC_010, Unit_Settings);
        this->insertSeparator(i);
    }
}

void Document_Source::select_item()
{
    this->setCurrentIndex(this->findData(current_item));
}

void Document_Source::item_event(int value, SCANPARA* doc_source_para)
{
    QVariant select_item = this->itemData(value);

    if(select_item == Auto){
        doc_source_para->FunctionalUnit_Auto = true;
        current_item = Auto;
    }else if(select_item == Flatbed){
        doc_source_para->FunctionalUnit_Auto = false;
        current_item = Flatbed;
         doc_source_para->FunctionalUnit.select = Flatbed;
    }else if(select_item == ADF){
        doc_source_para->FunctionalUnit_Auto = false;
        current_item = ADF;
        doc_source_para->FunctionalUnit.select = ADF;
    }else if(select_item == Carrier_Sheet){
        doc_source_para->FunctionalUnit_Auto = false;
        current_item = Carrier_Sheet;
        doc_source_para->FunctionalUnit.select = Carrier_Sheet;
    }else if(select_item == Unit_Settings){
        this->item_event_options(&doc_source_para->AFMTimeout);
    }
    changed = true;
}

void Document_Source::item_event_options(INT_SETTING_VALUE* afm_timeout_para)
{
    //Initialize dialog
    if(!m_documentsourcesettings_dialog){
        m_documentsourcesettings_dialog = new DocumentSourceSettingsDialog;
    }

    m_documentsourcesettings_dialog->Initialize(*afm_timeout_para);

    if(m_documentsourcesettings_dialog->isHidden()){
        m_documentsourcesettings_dialog->exec();
    }else {
        m_documentsourcesettings_dialog->activateWindow();
    }
   afm_timeout_para->select = m_documentsourcesettings_dialog->GetCurrentValue();

    //detectdoublefeedsettings_dialog->SetValue();
    //detectdoublefeedsettings_dialog->GetValue(&min_value, &max_value, &pt_value.select);
}

bool Document_Source::search_item_list(int item, int32_t item_list[], int32_t list_size)
{
    int i = 0;
    while(i < list_size){
        if(item_list[i] == item){
            return true;
        }
        i++;
    }
    return false;
}
