#ifndef ADDPAGEDIALOG_H
#define ADDPAGEDIALOG_H

#include <QDialog>
#include <QCloseEvent>

#include "supervisor.h"

namespace Ui {
class AddPageDialog;
}

class AddPageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddPageDialog(ScanningStatus* scanning_status, QWidget *parent = 0);
    ~AddPageDialog();

private slots:
    void onButtonPressed();

protected:
    void closeEvent(QCloseEvent* event);

private:
    Ui::AddPageDialog *ui;

    bool CancelDialog(void);

    ScanningStatus* scanning_status;
};

#endif // ADDPAGEDIALOG_H
