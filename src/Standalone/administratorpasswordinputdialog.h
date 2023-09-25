#ifndef ADMINISTRATORPASSWORDINPUTDIALOG_H
#define ADMINISTRATORPASSWORDINPUTDIALOG_H

#include <QDialog>

namespace Ui {
class AdministratorPasswordInputDialog;
}

class AdministratorPasswordInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AdministratorPasswordInputDialog(QWidget *parent = 0);
    ~AdministratorPasswordInputDialog();

    QString GetPassword();

public slots:
    void PushButtonAction(void);
    void LineEditFinished();

private:
    Ui::AdministratorPasswordInputDialog *ui;

    QString m_password;
};

#endif // ADMINISTRATORPASSWORDINPUTDIALOG_H
