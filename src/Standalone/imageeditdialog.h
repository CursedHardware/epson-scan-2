#ifndef IMAGEEDITDIALOG_H
#define IMAGEEDITDIALOG_H

#include <QDialog>

#include "supervisor.h"

#include "imageeditview.h"

namespace Ui {
class ImageEditDialog;
}

class ImageEditDialog : public QDialog
{
    Q_OBJECT

protected:
    void closeEvent(QCloseEvent* event);

public:

    explicit ImageEditDialog(std::list<imageEditInfo> imgpath_list, ScanningStatus* scanning_status, QWidget *parent = 0);
    ~ImageEditDialog();

    void grayOut(bool existImage);

    void saveButtonGrayOut(bool existOneImage);

    std::list<imageEditInfo> m_returnInfoList;
    void getInfoList(std::list<imageEditInfo> *List);

    enum ImageEditStatusFlag{
        ImageEditStatusIsNotGot = 0x0,
        ImageEditStatusIsSave = 0x1,
        ImageEditStatusIsReturn = 0x2
    };

    ImageEditStatusFlag m_returnStatus = ImageEditStatusIsNotGot;

private slots:

    void onButtonPressed();

    void on_rotateLeft_clicked();

    void on_rotateRight_clicked();

    void on_removeButton_clicked();

    void on_saveButton_clicked();

private:
    Ui::ImageEditDialog *ui;
    void reject();
    int showRemoveMessage();

    bool CancelDialog(void);

    ScanningStatus* scanning_status;
};

#endif // IMAGEEDITDIALOG_H
