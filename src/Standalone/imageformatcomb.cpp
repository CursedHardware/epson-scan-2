#include "mainwindow.h"

ImageFormatComb::ImageFormatComb(QWidget *parent) :
    QComboBox(parent),
    current_item(ImageFormatJPEG),
    imageformat_dialog(NULL),
    changed(false)
{
    //Initialize dialog
    imageformat_dialog = new ImageFormatSettingsDialog(parent);
}

void ImageFormatComb::initialize(IMAGE_FORMAT_SETTINGS_VALUE imageformat_settings_para){
    changed = true;
    if(imageformat_settings_para.ImageFormat.select == ImageFormatPNM && imageformat_settings_para.MultiTiffEnabled.select){
        current_item = ImageFormatMultiTIFF;
    }
    else if(imageformat_settings_para.ImageFormat.select == ImageFormatPNM && imageformat_settings_para.OFDEnabled.select){
        current_item = ImageFormatOFD;
    }
    else {
        current_item = (ImageFormat)imageformat_settings_para.ImageFormat.select; //default value
    }
}

void ImageFormatComb::focusOutEvent(QFocusEvent* event){
    event->accept();
}

void ImageFormatComb::update_ui(IMAGE_FORMAT_SETTINGS_VALUE imageformat_settings_para, bool isLFPModel)
{
    if(changed == true){
        this->clear();
        if(imageformat_settings_para.ImageFormat.capability.supportLevel == kSDISupportLevelAvailable){
            this->set_enabled(true);
            this->add_item(imageformat_settings_para.ImageFormat.capability.list, imageformat_settings_para.ImageFormat.capability.countOfList, imageformat_settings_para.MultiTiffEnabled.capability.supportLevel, isLFPModel,imageformat_settings_para.OFDEnabled.capability.supportLevel);
            this->select_item();
        }else {
            this->set_enabled(false);
        }
        changed = false;
    }
}

void ImageFormatComb::set_enabled(bool enabled){

    if(enabled == true){
        this->setEnabled(true);
    }else {
        this->setEnabled(false);
    }
}

void ImageFormatComb::add_item(int32_t item_list[], int32_t list_size, SDISupportLevel mTiff_support_lv, bool isLFPModel, SDISupportLevel OFD_support_lv)
{
    int i = 0;
    int item_num = 0;
    while(i < list_size){
        if(item_list[i] == ImageFormatJPEG){
            this->insertItem(item_num, TranslationString::GetString().translation_str.FILESAVESETTING_003, ImageFormatJPEG);
            imageformat_dialog->img_format_list[item_num] = ImageFormatJPEG;
            item_num++;
        }else if(item_list[i] == ImageFormatTIFF){
            this->insertItem(item_num, TranslationString::GetString().translation_str.FILESAVESETTING_005, ImageFormatTIFF);
            imageformat_dialog->img_format_list[item_num] = ImageFormatTIFF;
            item_num++;
            if(!isLFPModel){//LFPデバイスの場合はMulti-Tiffを制限する
                if(mTiff_support_lv == kSDISupportLevelAvailable){
                    this->insertItem(item_num, TranslationString::GetString().translation_str.FILESAVESETTING_006, ImageFormatMultiTIFF);
                    imageformat_dialog->img_format_list[item_num] = ImageFormatMultiTIFF;
                    item_num++;
                }
            }
        }else if(item_list[i] == ImageFormatColorJpegMonoRaw){
            this->insertItem(item_num, TranslationString::GetString().translation_str.FILESAVESETTING_007, ImageFormatColorJpegMonoRaw);
            imageformat_dialog->img_format_list[item_num] = ImageFormatColorJpegMonoRaw;
            item_num++;
        }else if(item_list[i] == ImageFormatPNG){
            this->insertItem(item_num, TranslationString::GetString().translation_str.FILESAVESETTING_004, ImageFormatPNG);
            imageformat_dialog->img_format_list[item_num] = ImageFormatPNG;
            item_num++;
        }else if(item_list[i] == ImageFormatPNM){
            this->insertItem(item_num, TranslationString::GetString().translation_str.LINUX_FILESAVESETTING_009, ImageFormatPNM);
            imageformat_dialog->img_format_list[item_num] = ImageFormatPNM;
            item_num++;
        }
        i++;
    }
    if(OFD_support_lv == kSDISupportLevelAvailable){
        this->insertItem(item_num, "OFD", ImageFormatOFD);
        imageformat_dialog->img_format_list[item_num] = ImageFormatOFD;
        item_num++;
    }

    this->add_option(item_num);
}

void ImageFormatComb::add_option(int list_size){

    this->insertItem(list_size, TranslationString::GetString().translation_str.FILESAVESETTING_009, OPTIONS);
    this->insertSeparator(list_size);
    imageformat_dialog->img_format_list[list_size] = (ImageFormat)OPTIONS;
}

void ImageFormatComb::select_item()
{
      this->setCurrentIndex(this->findData(current_item));
}

void ImageFormatComb::item_event(int value, IMAGE_FORMAT_SETTINGS_VALUE* imageformat_settings_para)
{   
    QVariant select_item = this->itemData(value);

    imageformat_settings_para->OFDEnabled.select = false;
    imageformat_settings_para->MultiTiffEnabled.select = false;
    if(select_item == ImageFormatJPEG){
        imageformat_settings_para->ImageFormat.select = current_item = ImageFormatJPEG;
    }else if(select_item == ImageFormatTIFF){
        imageformat_settings_para->ImageFormat.select = current_item = ImageFormatTIFF;
    }else if(select_item == ImageFormatMultiTIFF){
        current_item = ImageFormatMultiTIFF;
        imageformat_settings_para->ImageFormat.select = ImageFormatPNM;
        imageformat_settings_para->MultiTiffEnabled.select = true;
    }else if(select_item == ImageFormatOFD){
        current_item = ImageFormatOFD;
        imageformat_settings_para->ImageFormat.select = ImageFormatPNM;
        imageformat_settings_para->OFDEnabled.select = true;
    }else if(select_item == ImageFormatColorJpegMonoRaw){
        imageformat_settings_para->ImageFormat.select = current_item = ImageFormatColorJpegMonoRaw;
    }else if(select_item == ImageFormatPNG){
        imageformat_settings_para->ImageFormat.select = current_item = ImageFormatPNG;
    }else if(select_item == ImageFormatPNM){
        imageformat_settings_para->ImageFormat.select = current_item = ImageFormatPNM;
    }
    else if(select_item == OPTIONS){
        this->item_event_options(imageformat_settings_para);
        if(current_item == ImageFormatMultiTIFF){
            imageformat_settings_para->ImageFormat.select = ImageFormatPNM;
            imageformat_settings_para->MultiTiffEnabled.select = true;
        }else if(current_item == ImageFormatOFD){
            imageformat_settings_para->ImageFormat.select = ImageFormatPNM;
            imageformat_settings_para->OFDEnabled.select = true;
        }
        else {
            imageformat_settings_para->ImageFormat.select = current_item;
        }
    }
    changed = true;
}

void ImageFormatComb::item_event_options(IMAGE_FORMAT_SETTINGS_VALUE* imageformat_settings_para){

    imageformat_dialog->select_format = current_item;
    imageformat_dialog->initialize(imageformat_settings_para);

    //Show dialog
    if(imageformat_dialog->isHidden()){
        imageformat_dialog->exec();
    }else {
        imageformat_dialog->activateWindow();
    }
    current_item = imageformat_dialog->select_format;
}
