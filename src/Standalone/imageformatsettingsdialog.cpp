#include "imageformatsettingsdialog.h"
#include "ui_imageformatsettingsdialog.h"

ImageFormatSettingsDialog::ImageFormatSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImageFormatSettingsDialog)
{
    memset(&img_format_list, 0, sizeof(ImageFormat)*IMAGE_FORMAT_LIST_SIZE);
    select_format = ImageFormatJPEG;
    imageformat_settings_para = NULL;
    sv = NULL;

    ui->setupUi(this);
    this->setFixedSize(this->size());
}

ImageFormatSettingsDialog::~ImageFormatSettingsDialog()
{
    delete ui;
}

void ImageFormatSettingsDialog::initialize(IMAGE_FORMAT_SETTINGS_VALUE* imageformat_settings)
{
    imageformat_settings_para = imageformat_settings;

    ui->Image_Format_Settings_comboBox->initialize(select_format);
    //JPEG
    ui->Image_Quality_spinBox->setValue(imageformat_settings_para->JpegQuality_forJPEG);
    ui->Standard_radioButton->setChecked(!imageformat_settings_para->JpegProgressive.select);
    ui->Progressive_radioButton->setChecked(imageformat_settings_para->JpegProgressive.select);
    //TIFF
    ui->Black_White_comboBox->initialize((TiffCompression)imageformat_settings_para->TiffCompression.select);
    //PDF
    ui->All_Page_radioButton->setChecked(imageformat_settings_para->PDFImgFormatOption.AllPage);
    ui->Select_Page_radioButton->setChecked(!imageformat_settings_para->PDFImgFormatOption.AllPage);
    ui->Select_Page_spinBox->setValue(imageformat_settings_para->PDFImgFormatOption.page);
    ui->Image_Quality_spinBox_2->setValue(imageformat_settings_para->JpegQuality_forPDF);

    this->updateUI();
}

void ImageFormatSettingsDialog::onValueChanged(int value)
{
    QObject* obj = sender();

    if(obj == ui->Image_Format_Settings_comboBox){
        ui->Image_Format_Settings_comboBox->item_event(value);
    }else if(obj == ui->Black_White_comboBox){
        ui->Black_White_comboBox->item_event(value, &imageformat_settings_para->TiffCompression);
    }

    this->updateUI();
}

void ImageFormatSettingsDialog::updateUI()
{
    ui->Image_Format_Settings_comboBox->get_current_item(&select_format);
    this->ChangeWidget(select_format);
    ui->Image_Format_Settings_comboBox->update_ui(img_format_list);
    ui->Black_White_comboBox->update_ui(imageformat_settings_para->TiffCompression);

    this->set_parameter();
}

void ImageFormatSettingsDialog::set_parameter()
{
    //Jpeg Encoding
    if(ui->Standard_radioButton->isChecked()){
        imageformat_settings_para->JpegProgressive.select = false;
    }else if(ui->Progressive_radioButton->isChecked()){
        imageformat_settings_para->JpegProgressive.select = true;
    }
    //imageformat_settings_para->JpegQuality.select = ui->Image_Quality_spinBox->value();
    imageformat_settings_para->JpegQuality_forJPEG = ui->Image_Quality_spinBox->value();

    //PDF
    if(ui->All_Page_radioButton->isChecked()){
        imageformat_settings_para->PDFImgFormatOption.AllPage = true;
    }else if(ui->Select_Page_radioButton->isChecked()){
        imageformat_settings_para->PDFImgFormatOption.AllPage = false;
    }
    imageformat_settings_para->PDFImgFormatOption.page = ui->Select_Page_spinBox->value();
    imageformat_settings_para->JpegQuality_forPDF = ui->Image_Quality_spinBox_2->value();
}

void ImageFormatSettingsDialog::ChangeWidget(ImageFormat format){

    if(format == ImageFormatPNM){
        ui->Image_Format_stackedWidget->setCurrentIndex(0);
    }else if(format == ImageFormatJPEG){
        ui->Image_Format_stackedWidget->setCurrentIndex(1);
    }else if(format == ImageFormatPNG){
        ui->Image_Format_stackedWidget->setCurrentIndex(2);
    }else if(format == ImageFormatTIFF){
        ui->Image_Format_stackedWidget->setCurrentIndex(3);
    }else if(format == ImageFormatMultiTIFF){
        ui->Image_Format_stackedWidget->setCurrentIndex(3);
    }else if(format == ImageFormatColorJpegMonoRaw){
        ui->Image_Format_stackedWidget->setCurrentIndex(4);
    }else if(format == ImageFormatOFD) {
        ui->Image_Format_stackedWidget->setCurrentIndex(5);
    }
}
