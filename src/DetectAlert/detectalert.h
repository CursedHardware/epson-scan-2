#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>

typedef enum {Question=0, Information, Warning, Critical}IconType;

class DetectAlert : public QMainWindow
{
    Q_OBJECT

public:
    DetectAlert(QWidget *parent = 0);
    ~DetectAlert();

    void CustomMessageBox(QString dialog_title, QString message, QString button_message, std::string identifier, std::string dir_num);

private:
    int CheckExistDir();
};

#endif // MAINWINDOW_H
