#ifndef IMAGEFORMATSETTINGSDIALOG_H
#define IMAGEFORMATSETTINGSDIALOG_H

#include <QDialog>
#include "supervisor.h"

#define IMAGE_FORMAT_LIST_SIZE 10

namespace Ui {
class ImageFormatSettingsDialog;
}

class ImageFormatSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImageFormatSettingsDialog(QWidget *parent = 0);
    ~ImageFormatSettingsDialog();

    //setting parameter
    ImageFormat img_format_list[IMAGE_FORMAT_LIST_SIZE];
    ImageFormat select_format;

    void initialize(IMAGE_FORMAT_SETTINGS_VALUE* imageformat_settings);

public slots:
    void onValueChanged(int value);
    void updateUI();

private:
    Ui::ImageFormatSettingsDialog *ui;

    IMAGE_FORMAT_SETTINGS_VALUE* imageformat_settings_para;

    void ChangeWidget(ImageFormat format);

    void set_parameter();

    Supervisor* sv;
};

#endif // IMAGEFORMATSETTINGSDIALOG_H
