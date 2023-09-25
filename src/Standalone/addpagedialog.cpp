#include "addpagedialog.h"
#include "ui_addpagedialog.h"
#include "mainwindow.h"

#include "common_utility.h"

#include <QMessageBox>

AddPageDialog::AddPageDialog(ScanningStatus* scanning_status, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddPageDialog)
{
    ui->setupUi(this);
    this->setFixedSize(this->size());

    this->scanning_status = scanning_status;
}

AddPageDialog::~AddPageDialog()
{
    delete ui;
}

void AddPageDialog::closeEvent(QCloseEvent* event){
    event->ignore();
}

void AddPageDialog::onButtonPressed()
{
    QObject* obj = sender();

    if(obj == ui->Add_pushButton){
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

bool AddPageDialog::CancelDialog(void)
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
