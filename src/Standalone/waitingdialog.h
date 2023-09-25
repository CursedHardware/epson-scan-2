#ifndef WAITINGDIALOG_H
#define WAITINGDIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include <QTimer>

namespace Ui {
class WaitingDialog;
}

class WaitingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WaitingDialog(const char* message, int wait_time, QWidget *parent = 0);
    ~WaitingDialog();

    int wait_time;

protected:
    void closeEvent(QCloseEvent* event);

private:
    Ui::WaitingDialog *ui;

    QTimer* m_timer;
    int time_count;

private slots:
    void Update();
};

#endif // WAITINGDIALOG_H
