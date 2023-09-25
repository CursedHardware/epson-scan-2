#ifndef IMAGETYPESETTINGSDIALOG_H
#define IMAGETYPESETTINGSDIALOG_H

#include <QDialog>
#include "supervisor.h"

namespace Ui {
class ImageTypeSettingsDialog;
}

class ImageTypeSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    bool changed;

    explicit ImageTypeSettingsDialog(QWidget *parent = 0);
    ~ImageTypeSettingsDialog();

    bool Initialize(Supervisor* sv);

    void select_item(INT_SETTING_VALUE image_type_auto_para);
    void select_level(INT_SETTING_VALUE image_type_auto_lv_para);
    void item_event(int value);
    void item_event_slider(int value);

    AutoColorPixelType get_currentItem();
    int get_currentValue();

private slots:
    void onValueChanged(int value);

private:
    Ui::ImageTypeSettingsDialog *ui;

    AutoColorPixelType current_item;
    int current_value;

    void add_item(int32_t item_list[], int32_t list_size);
};

#endif // IMAGETYPESETTINGSDIALOG_H
