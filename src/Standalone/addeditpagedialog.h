#ifndef ADDEDITPAGEDIALOG_H
#define ADDEDITPAGEDIALOG_H

#include <QDialog>
#include <QCloseEvent>

#include "supervisor.h"
#include "imageeditdialog.h"

namespace Ui {
class addeditpageDialog;
}

class addeditpageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit addeditpageDialog(ScanningStatus* scanning_status, QWidget *parent = 0);
    ~addeditpageDialog();

    bool EditDialog(std::list<imageEditInfo> *result_list);

private slots:
    void onButtonPressed();

protected:
    void closeEvent(QCloseEvent* event);

private:
    Ui::addeditpageDialog *ui;

    bool CancelDialog(void);

    ScanningStatus* scanning_status;

    ImageEditDialog *m_jumpImageEditDialog = NULL;
};

#endif // ADDEDITPAGEDIALOG_H
