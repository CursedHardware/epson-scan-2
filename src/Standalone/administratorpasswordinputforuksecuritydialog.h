#ifndef AdministratorPasswordInputForUKSecurityDialog_H
#define AdministratorPasswordInputForUKSecurityDialog_H

#include <QDialog>

namespace Ui {
class AdministratorPasswordInputForUKSecurityDialog;
}

class AdministratorPasswordInputForUKSecurityDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AdministratorPasswordInputForUKSecurityDialog(QWidget *parent = 0);
    ~AdministratorPasswordInputForUKSecurityDialog();

    QString GetPassword();

public slots:
    void PushButtonAction(void);
    void LineEditFinished();

private:
    Ui::AdministratorPasswordInputForUKSecurityDialog *ui;

    QString m_password;
};

#endif // AdministratorPasswordInputForUKSecurityDialog_H
