#ifndef SENSORGLASSDIRTSENSITIVITYCOMBOBOX_H
#define SENSORGLASSDIRTSENSITIVITYCOMBOBOX_H

#include <QComboBox>
#include "supervisor.h"

class SensorGlassDirtSensitivityCombobox : public QComboBox
{
public:
    SensorGlassDirtSensitivityCombobox(QWidget *parent = 0);

    void initialize(SensorGlassDirtSensitivity default_item);

    void update_ui(INT_SETTING_VALUE sensor_glass_para);

    void item_event(int value);

    void set_enabled(bool enabled);

    SensorGlassDirtSensitivity get_current_item(void);

private:
    bool changed;

    bool enabled;

    SensorGlassDirtSensitivity current_item;

    void add_item(int32_t item_list[], int32_t list_size);

    void select_item(void);
};

#endif // SENSORGLASSDIRTSENSITIVITYCOMBOBOX_H
