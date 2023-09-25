#ifndef IMAGEFORMATSETTINGSCOMBOBOX_H
#define IMAGEFORMATSETTINGSCOMBOBOX_H

#include <QComboBox>
#include "supervisor.h"
#include "imageformatcomb.h"

class ImageFormatSettingsCombobox : public QComboBox
{
public:
    ImageFormatSettingsCombobox(QWidget *parent = 0);

    ImageFormat current_item;

    void initialize(ImageFormat default_item);

    void item_event(int value);

    void get_current_item(ImageFormat* select);

    void update_ui(ImageFormat img_format_list[]);

private:

    bool changed;

    void set_enabled(bool enabled);

    void add_item(ImageFormat img_format_list[]);

    void select_item();
};

#endif // IMAGEFORMATSETTINGSCOMBOBOX_H
