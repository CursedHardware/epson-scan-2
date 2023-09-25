#include "mainwindow.h"
#include <QStandardItemModel>

Image_Type::Image_Type(QWidget *parent) :
    QComboBox(parent),
    m_ImageTypeSetting_Dialog(nullptr)
{
    changed = false;
}

void Image_Type::initialize(INT_SETTING_VALUE* document_type_para){
    m_document_type_para = document_type_para;
    changed = true;
}

void Image_Type::update_ui(INT_SETTING_VALUE image_type_para){

    if(changed == true){
        this->clear();
        if(image_type_para.capability.supportLevel == kSDISupportLevelAvailable){
            this->set_enabled(true);
            this->add_item(image_type_para.capability.list, image_type_para.capability.countOfList);
            this->select_item(image_type_para);
        }else {
            this->set_enabled(false);
        }
        changed = false;
    }
}

void Image_Type::set_enabled(bool enabled){

    if(enabled == true){
        this->setEnabled(true);
    }else {
        this->setEnabled(false);
    }
}

void Image_Type::add_item(int32_t item_list[], int32_t list_size){

    int i = 0;

    bool hasAuto = false;

    while(i < list_size){
        if(item_list[i] == ColorTypeAuto){
            this->insertItem(i, TranslationString::GetString().translation_str.MAINSETTING_017, ColorTypeAuto);
            if( m_document_type_para->select != Reflective)
            {
            QStandardItemModel *model =
                qobject_cast<QStandardItemModel *>(this->model());
            bool disabled = true;
            QStandardItem *item = model->item(i);
            item->setFlags(disabled ? item->flags() & ~Qt::ItemIsEnabled
                                    : item->flags() | Qt::ItemIsEnabled);
            }
            hasAuto = true;
        }else if(item_list[i] == ColorTypeRGB24){ //ColorType
            this->insertItem(i, TranslationString::GetString().translation_str.MAINSETTING_014, ColorTypeRGB24);
        }else if(item_list[i] == ColorTypeMono8){
            this->insertItem(i, TranslationString::GetString().translation_str.MAINSETTING_015, ColorTypeMono8);
        }else if(item_list[i] == ColorTypeMono1){
            this->insertItem(i, TranslationString::GetString().translation_str.MAINSETTING_016, ColorTypeMono1);
            if( m_document_type_para->select != Reflective)
            {
            QStandardItemModel *model =
                qobject_cast<QStandardItemModel *>(this->model());
            bool disabled = true;
            QStandardItem *item = model->item(i);
            item->setFlags(disabled ? item->flags() & ~Qt::ItemIsEnabled
                                    : item->flags() | Qt::ItemIsEnabled);
            }
        }else if(item_list[i] == ColorTypeRGB48){
            this->insertItem(i, TranslationString::GetString().translation_str.PHOTO_MAIN_019, ColorTypeRGB48);
        }else if(item_list[i] == ColorTypeMono16){
            this->insertItem(i, TranslationString::GetString().translation_str.PHOTO_MAIN_020, ColorTypeMono16);
        }
        i++;
    }

    //イメージタイプ自動の設定を追加する
    if(hasAuto){//TBD
        this->insertItem(i, TranslationString::GetString().translation_str.CONFIG_DOC_010, ColorTypeSettings);
        this->insertSeparator(i);
    }
}

void Image_Type::select_item(INT_SETTING_VALUE image_type_para){

    int list_size = image_type_para.capability.countOfList;

    int i = 0;
    while(i < list_size){
        if(image_type_para.capability.list[i] == image_type_para.select){
            this->setCurrentIndex(i);
        }
        i++;
    }
}

void Image_Type::item_event(int value, INT_SETTING_VALUE* image_type_para, Supervisor* sv)
{
    QVariant select_item = this->itemData(value);

    if(select_item == ColorTypeAuto){
        image_type_para->select = ColorTypeAuto;
    }else if(select_item == ColorTypeRGB24){
        image_type_para->select = ColorTypeRGB24;
    }else if(select_item == ColorTypeMono8){
        image_type_para->select = ColorTypeMono8;
    }else if(select_item == ColorTypeMono1){
        image_type_para->select = ColorTypeMono1;
    }else if(select_item == ColorTypeRGB48){
        image_type_para->select = ColorTypeRGB48;
    }else if(select_item == ColorTypeMono16){
        image_type_para->select = ColorTypeMono16;
    }else if(select_item == ColorTypeSettings){
        this->item_event_options(sv);
    }
    changed = true;
}

void Image_Type::item_event_options(Supervisor* sv)
{
    //Initialize dialog
    if(!m_ImageTypeSetting_Dialog){
        m_ImageTypeSetting_Dialog = new ImageTypeSettingsDialog;
    }

    m_ImageTypeSetting_Dialog->Initialize(sv);

    if(m_ImageTypeSetting_Dialog->isHidden()){
        m_ImageTypeSetting_Dialog->exec();
    }else {
        m_ImageTypeSetting_Dialog->activateWindow();
    }

    sv->device_data.AutoColorPixelType.select = m_ImageTypeSetting_Dialog->get_currentItem();
    sv->device_data.AutoColorPixelLevel.select = m_ImageTypeSetting_Dialog->get_currentValue();
}
