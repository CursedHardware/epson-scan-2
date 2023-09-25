#include "administratorpasswordinputforuksecuritydialog.h"
#include "ui_administratorpasswordinputforuksecuritydialog.h"
#include <QTimer>

AdministratorPasswordInputForUKSecurityDialog::AdministratorPasswordInputForUKSecurityDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdministratorPasswordInputForUKSecurityDialog)
{
    m_password = "";
    ui->setupUi(this);
    this->setFixedSize(this->size());

    QTimer::singleShot(0, ui->Password_Input_lineEdit, SLOT(setFocus()));
}

AdministratorPasswordInputForUKSecurityDialog::~AdministratorPasswordInputForUKSecurityDialog()
{
    delete ui;
}

void AdministratorPasswordInputForUKSecurityDialog::PushButtonAction(void)
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

void AdministratorPasswordInputForUKSecurityDialog::LineEditFinished()
{
    //アドレスのエディットボックスが空のとき追加ボタンをグレーアウト
    if(ui->Password_Input_lineEdit->text() != ""){
        ui->OK_pushButton->setEnabled(true);
    }else {
        ui->OK_pushButton->setEnabled(false);
    }
}

QString AdministratorPasswordInputForUKSecurityDialog::GetPassword()
{
    return m_password;
}
