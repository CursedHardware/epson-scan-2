#include "tiffcompressionmonocombobox.h"
#include "mainwindow.h"

TiffCompressionMonoCombobox::TiffCompressionMonoCombobox(QWidget *parent) :
    QComboBox(parent),
    current_item(TiffCompressionNone),
    changed(false)
{
}

void TiffCompressionMonoCombobox::initialize(TiffCompression default_value){
    changed = true;
    current_item = default_value;
}

void TiffCompressionMonoCombobox::update_ui(INT_SETTING_VALUE tiff_compression_para)
{
    if(changed == true){
        this->clear();
        if(tiff_compression_para.capability.supportLevel == kSDISupportLevelAvailable){
            this->set_enabled(true);
            this->add_item(tiff_compression_para.capability.list, tiff_compression_para.capability.countOfList);
            this->select_item();
        }else {
            this->set_enabled(false);
        }
        changed = false;
    }
}

void TiffCompressionMonoCombobox::set_enabled(bool enabled)
{
    this->setEnabled(enabled);
}

void TiffCompressionMonoCombobox::add_item(int32_t item_list[], int32_t list_size)
{
    int i = 0;
    while(i < list_size){
        if(item_list[i] == TiffCompressionNone){
            this->insertItem(i, TranslationString::GetString().translation_str.IMAGEFORMATOPTION_TIF_011, TiffCompressionNone);
        }else if(item_list[i] == TiffCompressionCCITFAX4){
            this->insertItem(i, TranslationString::GetString().translation_str.IMAGEFORMATOPTION_TIF_012, TiffCompressionCCITFAX4);
        }
        i++;
    }
}

void TiffCompressionMonoCombobox::select_item()
{
    this->setCurrentIndex(this->findData(current_item));
}

void TiffCompressionMonoCombobox::item_event(int value, INT_SETTING_VALUE* tiff_compression_para)
{
    QVariant select_item = this->itemData(value);

    if(select_item == TiffCompressionNone){
        tiff_compression_para->select = current_item = TiffCompressionNone;
    }else if(select_item == TiffCompressionCCITFAX4){
        tiff_compression_para->select = current_item = TiffCompressionCCITFAX4;
    }
    changed = true;
}
