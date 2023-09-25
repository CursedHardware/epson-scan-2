#include "administratorpasswordinputdialog.h"
#include "ui_administratorpasswordinputdialog.h"
#include <QTimer>

AdministratorPasswordInputDialog::AdministratorPasswordInputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdministratorPasswordInputDialog)
{
    m_password = "";
    ui->setupUi(this);
    this->setFixedSize(this->size());

    QTimer::singleShot(0, ui->Password_Input_lineEdit, SLOT(setFocus()));
}

AdministratorPasswordInputDialog::~AdministratorPasswordInputDialog()
{
    delete ui;
}

void AdministratorPasswordInputDialog::PushButtonAction(void)
{
    QObject* obj = sender();

    if(obj == ui->OK_pushButton){
        m_password = ui->Password_Input_lineEdit->text();
        this->reject();
    }else if(obj == ui->Cancel_pushButton){
        m_password = "";
        this->reject();
    }
}

void AdministratorPasswordInputDialog::LineEditFinished()
{
    //アドレスのエディットボックスが空のとき追加ボタンをグレーアウト
    if(ui->Password_Input_lineEdit->text() != ""){
        ui->OK_pushButton->setEnabled(true);
    }else {
        ui->OK_pushButton->setEnabled(false);
    }
}

QString AdministratorPasswordInputDialog::GetPassword()
{
    return m_password;
}
