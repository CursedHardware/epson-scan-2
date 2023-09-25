#include "documenttype.h"

Document_Type::Document_Type(QWidget *parent) :
    QComboBox(parent),
    changed(false)
{
}

void Document_Type::initialize(DocumentType type){
    changed = true;
}

void Document_Type::update_ui(INT_SETTING_VALUE* document_type_para)
{
    if(document_type_para->capability.supportLevel == kSDISupportLevelAvailable){
        this->set_enabled(true);
        changed = true;
        this->show();
    }else if(document_type_para->capability.supportLevel == kSDISupportLevelUnavailable){
        this->set_enabled(false);
        changed = true;
        this->show();
    }else if(document_type_para->capability.supportLevel == kSDISupportLevelNone){
        this->hide();
    }

    if(changed == true){
        this->clear();
        this->add_item(document_type_para->capability.list, document_type_para->capability.countOfAllList);
        this->select_item(*document_type_para);
        changed = false;
    }
}

void Document_Type::set_enabled(bool enabled){

    if(enabled == true){
        this->setEnabled(true);
    }else {
        this->setEnabled(false);
    }
}

void Document_Type::add_item(int32_t item_list[], int32_t list_size){

    int i = 0;
    while(i < list_size){
        if(item_list[i] == Reflective){
            this->insertItem(i, TranslationString::GetString().translation_str.PHOTO_MAIN_008, Reflective);
        }else if(item_list[i] == Color_Positive_Film){
            this->insertItem(i, TranslationString::GetString().translation_str.PHOTO_MAIN_009, Color_Positive_Film );
        }else if(item_list[i] == Color_Negative_Film){
            this->insertItem(i, TranslationString::GetString().translation_str.PHOTO_MAIN_010, Color_Negative_Film );
        }else if(item_list[i] == Mono_Negative_Film){
            this->insertItem(i, TranslationString::GetString().translation_str.PHOTO_MAIN_013, Mono_Negative_Film );
        }
        i++;
    }
}

void Document_Type::select_item(INT_SETTING_VALUE document_type_para){

    int list_size = document_type_para.capability.countOfAllList;

    int i = 0;
    while(i < list_size){
        if(document_type_para.capability.list[i] == document_type_para.select){
            this->setCurrentIndex(i);
        }
        i++;
    }
}

void Document_Type::item_event(int value, INT_SETTING_VALUE* document_type_para)
{
    QVariant select_item = this->itemData(value);

    if(select_item == Reflective){
        document_type_para->select = Reflective;
    }else if(select_item == Color_Positive_Film){
        document_type_para->select = Color_Positive_Film;
    }else if(select_item == Color_Negative_Film){
        document_type_para->select = Color_Negative_Film;
    }else if(select_item == Mono_Negative_Film){
        document_type_para->select = Mono_Negative_Film;
    }
    changed = true;
}
