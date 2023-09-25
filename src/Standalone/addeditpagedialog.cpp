#include "addeditpagedialog.h"
#include "ui_addeditpagedialog.h"
#include "mainwindow.h"
#include "saveimagepathlist.h"
#include "common_utility.h"

#include <QMessageBox>

addeditpageDialog::addeditpageDialog(ScanningStatus* scanning_status, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addeditpageDialog)
{
    ui->setupUi(this);
    this->setFixedSize(this->size());

    this->scanning_status = scanning_status;
}

addeditpageDialog::~addeditpageDialog()
{
    delete ui;
}

void addeditpageDialog::closeEvent(QCloseEvent* event){
    if(this->CancelDialog()){
        *this->scanning_status = CANCEL;
        this->reject();
    }else {
        event->ignore();
    }
}

void addeditpageDialog::onButtonPressed()
{
    QObject* obj = sender();

    if(obj == ui->Edit_pushButton){
        *this->scanning_status = EDIT;
        this->reject();
    }else if(obj == ui->Add_pushButton){
        *this->scanning_status = CONTINUE;
    }else if(obj == ui->Cancel_pushButton){
        if(this->CancelDialog()){
            *this->scanning_status = CANCEL;
            this->reject();
        }
    }else if(obj == ui->Save_pushButton){
        *this->scanning_status = SAVE;
    }
}

bool addeditpageDialog::CancelDialog(void)
{
    QMessageBox msgBox(this);
    std::string message = (TranslationString::GetString().translation_str.C_UI_102.toLocal8Bit()).constData();
    msgBox.setText(message.c_str());
    msgBox.setWindowTitle(TranslationString::GetString().translation_str.MENU_001);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);

    //ボタンのテキスト変更
    msgBox.setButtonText(QMessageBox::Yes, TranslationString::GetString().translation_str.ES2U_MAIN_052);
    msgBox.setButtonText(QMessageBox::No, TranslationString::GetString().translation_str.ES2U_MAIN_053);

    msgBox.setIcon(QMessageBox::Warning);

    int res = msgBox.exec();

    if (res == QMessageBox::Yes){
        return true;
    } else if (res == QMessageBox::No) {
        return false;
    }else {
        return false;
    }
}

bool addeditpageDialog::EditDialog(std::list<imageEditInfo> *result_list)
{
    //ImageEditDialog::ImageEditStatusFlag status = ImageEditDialog::ImageEditStatusIsNotGot;

    m_jumpImageEditDialog = new ImageEditDialog(*result_list, this->scanning_status, this);
    if(!m_jumpImageEditDialog){
        return false;
    }
    m_jumpImageEditDialog->exec();

    std::list<imageEditInfo> temp_list = {};
    m_jumpImageEditDialog->getInfoList(&temp_list);

    *result_list = temp_list;

    return true;
}
