#ifndef FILENAMESETTINGDIALOG_H
#define FILENAMESETTINGDIALOG_H

#include <QDialog>

#include "supervisor.h"

namespace Ui {
class FileNameSettingDialog;
}

class FileNameSettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileNameSettingDialog(QWidget *parent = 0);
    ~FileNameSettingDialog();

    void Initialize(SCANPARA filename_para);

    bool getFileName_OverWrite();
    bool getFileName_Counter();

private:
    Ui::FileNameSettingDialog *ui;

    bool m_FileName_OverWrite;
    bool m_FileName_Counter;


public slots:
    void onValueChangedBool(bool value);
};

#endif // FILENAMESETTINGDIALOG_H
