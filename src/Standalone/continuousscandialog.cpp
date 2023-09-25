#include "continuousscandialog.h"
#include "ui_continuousscandialog.h"
#include "translationstring.h"
#include "saveimagepathlist.h"
#include <QStyle>

ContinuousScanDialog::ContinuousScanDialog(Supervisor* sv, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContinuousScanDialog),
    m_dialog_type(IMMEDIATE),
    m_scanning_status(CONTINUE)
{
    m_sv = sv;

    ui->setupUi(this);
    this->setFixedSize(this->size());
}

ContinuousScanDialog::~ContinuousScanDialog()
{
    delete ui;
}

void ContinuousScanDialog::Initialize(QString message, DoubleFeedMode dialog_type)
{
    this->m_dialog_type = dialog_type;

    ui->graphicsView->hide();
    ui->Icon_toolButton->setIcon(this->style()->standardIcon(QStyle::SP_MessageBoxWarning));

    //画像表示領域の背景色変更
    ui->frame->setObjectName("frame");
    ui->frame->setStyleSheet("#frame {background-color:dimgray;}");

    //文字色を変更
    QPalette pal = ui->noimage_label->palette();
    pal.setColor(QPalette::Foreground, QColor("#FFFFFF"));
    ui->noimage_label->setPalette(pal);

    this->ChangeStrings(dialog_type, message);

    if(m_sv->device_data.ADFSpecialErrorMessageForPassport.select){
        this->AddADFSpecialErrorMessageForPassportStrings(TranslationString::GetString().translation_str.CONFIRMCONTINUE_015);
    }

    this->LoadScanImage();
}

void ContinuousScanDialog::closeEvent(QCloseEvent* event){
    event->ignore();
}

void ContinuousScanDialog::LoadScanImage()
{
    //if(m_image.load("/home/build/workspace/PreviewWindow/sample_image50.png")){
    if(SaveImagePathList::save_image_path_list.size() > 0)
    { 
        auto save_path = SaveImagePathList::save_image_path_list.end();
        save_path--;
        imageEditInfo temp_edit_info = (imageEditInfo)*save_path;
        std::string temp_str = temp_edit_info.file_path;
        if(m_image.load((const char*)temp_str.c_str())){

            ui->graphicsView->resize(ui->frame->width(), ui->frame->height());
            ui->graphicsView->show();

            //graphicsViewにScene_を設定。
            ui->graphicsView->setScene(&Scene_);
            //Scene_をクリア
            Scene_.clear();
            //Scene_に登録するpixmapを作成
            QPixmap pixmap=QPixmap::fromImage(m_image);
            //Scene_に画像を登録
            Scene_.addPixmap(pixmap.scaled(ui->graphicsView->width()-5, ui->graphicsView->height()-5, Qt::KeepAspectRatio,Qt::FastTransformation));
        }
    }
}

void ContinuousScanDialog::onButtonPressed()
{
    QObject* obj = sender();

    if(obj == ui->Continue_pushButton){
        this->m_scanning_status = CONTINUE;
    }else if(obj == ui->Stop_pushButton){
        this->m_scanning_status = CANCEL;
    }else if(obj == ui->Save_pushButton){
        if(this->m_dialog_type == IMMEDIATE){
            this->m_scanning_status = SAVE;
        }else {
            this->m_scanning_status = DISCARD;
        }
    }
    this->reject();
}

ScanningStatus ContinuousScanDialog::GetScanningStatus()
{
    return this->m_scanning_status;
}

void ContinuousScanDialog::ChangeStrings(DoubleFeedMode dialog_type, QString message)
{

    ui->continuous_label->setText(message);

    //ダイアログのタイプによって表示する文字列を変更する
    if(dialog_type == AFTER_EJECT){
        ui->Save_pushButton->setText(TranslationString::GetString().translation_str.CONFIRMCONTINUE_014);
        //ui->continuous_label->setText(TranslationString::GetString().translation_str.CONFIRMCONTINUE_013);
    }else if(dialog_type == IMMEDIATE){
        ui->Save_pushButton->setText(TranslationString::GetString().translation_str.CONFIRMCONTINUE_004);
        //ui->continuous_label->setText(TranslationString::GetString().translation_str.CONFIRMCONTINUE_002);
    }
}

void ContinuousScanDialog::AddADFSpecialErrorMessageForPassportStrings(QString message)
{

    ui->ADFSpecialErrorMessageForPassport_label->setText(message);
    /*
    //ダイアログのタイプによって表示する文字列を変更する
    if(dialog_type == AFTER_EJECT){
        ui->Save_pushButton->setText(TranslationString::GetString().translation_str.CONFIRMCONTINUE_014);
        //ui->continuous_label->setText(TranslationString::GetString().translation_str.CONFIRMCONTINUE_013);
    }else if(dialog_type == IMMEDIATE){
        ui->Save_pushButton->setText(TranslationString::GetString().translation_str.CONFIRMCONTINUE_004);
        //ui->continuous_label->setText(TranslationString::GetString().translation_str.CONFIRMCONTINUE_002);
    }
    */
}
