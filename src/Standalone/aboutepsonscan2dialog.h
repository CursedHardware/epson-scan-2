#ifndef ABOUTEPSONSCAN2DIALOG_H
#define ABOUTEPSONSCAN2DIALOG_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class AboutEpsonScan2Dialog;
}

class AboutEpsonScan2Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutEpsonScan2Dialog(QWidget *parent = 0);
    ~AboutEpsonScan2Dialog();

    void Initialize();

protected:
    void closeEvent(QCloseEvent* event);

private:
    Ui::AboutEpsonScan2Dialog *ui;

    void ReadVersionFile(const char* version_path);
};

#endif // ABOUTEPSONSCAN2DIALOG_H
