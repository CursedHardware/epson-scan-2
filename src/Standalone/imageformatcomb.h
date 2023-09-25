#ifndef IMAGEFORMATCOMB_H
#define IMAGEFORMATCOMB_H

#include <QComboBox>
#include "supervisor.h"
#include "imageformatsettingsdialog.h"

class ImageFormatComb : public QComboBox
{
public:
    ImageFormatComb(QWidget *parent = 0);

    ImageFormat current_item;

    void initialize(IMAGE_FORMAT_SETTINGS_VALUE imageformat_settings_para);

    void focusOutEvent(QFocusEvent* event);

    void update_ui(IMAGE_FORMAT_SETTINGS_VALUE imageformat_settings_para, bool isLFPModel);

    void item_event(int value, IMAGE_FORMAT_SETTINGS_VALUE* imageformat_settings_para);

private:
    ImageFormatSettingsDialog* imageformat_dialog;

    bool changed;

    void set_enabled(bool enabled);

    void add_item(int32_t item_list[], int32_t list_size, SDISupportLevel mTiff_support_lv, bool isLFPModel, SDISupportLevel OFD_support);

    void add_option(int list_size);

    void select_item();

    void item_event_options(IMAGE_FORMAT_SETTINGS_VALUE* imageformat_settings_para);
};

#endif // IMAGEFORMATCOMB_H
