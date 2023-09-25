#ifndef SCANNINGDIALOG_H
#define SCANNINGDIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include <QTimer>

#include "supervisor.h"

namespace Ui {
class ScanningDialog;
}

class ScanningDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScanningDialog(Supervisor* sv, QWidget *parent = 0);
    ~ScanningDialog();

    int getPagecount();

    void ScanInitialize(QString progress_message);

protected:
    void closeEvent(QCloseEvent* event);

private:
    Ui::ScanningDialog *ui;
    Supervisor *sv;
    int page_count;

    QTimer* m_timer;
    bool do_cancel;

private slots:
    void Update();
    void Cancel();
};

#endif // SCANNINGDIALOG_H
