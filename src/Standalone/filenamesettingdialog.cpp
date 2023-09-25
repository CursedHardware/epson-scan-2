#include "mainwindow.h"
#include "ui_filenamesettingdialog.h"

FileNameSettingDialog::FileNameSettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileNameSettingDialog)
{
    ui->setupUi(this);
    this->setFixedSize(this->size());
}

FileNameSettingDialog::~FileNameSettingDialog()
{
    delete ui;
}

void FileNameSettingDialog::Initialize(SCANPARA filename_para)
{
    m_FileName_OverWrite = filename_para.FileName_OverWrite;
    m_FileName_Counter = filename_para.FileName_Counter;

    ui->Samename_Overwrite_checkBox->setChecked(m_FileName_OverWrite);
    ui->File_Counter_checkBox->setChecked(m_FileName_Counter);
}

void FileNameSettingDialog::onValueChangedBool(bool value)
{
    QObject* obj = sender();

    if(obj == ui->Samename_Overwrite_checkBox){
        m_FileName_OverWrite = value;
    }else if(obj == ui->File_Counter_checkBox){
        m_FileName_Counter = value;
    }
}

bool FileNameSettingDialog::getFileName_OverWrite()
{
    return m_FileName_OverWrite;
}

bool FileNameSettingDialog::getFileName_Counter()
{
    return m_FileName_Counter;
}
