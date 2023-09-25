#ifndef TIFFCOMPRESSIONMONOCOMBOBOX_H
#define TIFFCOMPRESSIONMONOCOMBOBOX_H

#include <QComboBox>
#include "supervisor.h"

class TiffCompressionMonoCombobox : public QComboBox
{
public:
    TiffCompressionMonoCombobox(QWidget *parent = 0);

    TiffCompression current_item;

    void initialize(TiffCompression default_value);

    void update_ui(INT_SETTING_VALUE tiff_compression_para);

    void item_event(int value, INT_SETTING_VALUE* tiff_compression_para);

private:
    bool changed;

    void set_enabled(bool enabled);

    void add_item(int32_t item_list[], int32_t list_size);

    void select_item();
};

#endif // TIFFCOMPRESSIONMONOCOMBOBOX_H
