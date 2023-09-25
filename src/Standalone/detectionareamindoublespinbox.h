#ifndef DETECTIONAREAMINDOUBLESPINBOX_H
#define DETECTIONAREAMINDOUBLESPINBOX_H

#include <QDoubleSpinBox>
#include "supervisor.h"

class DetectionAreaMinDoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT

public:
    DetectionAreaMinDoubleSpinBox(QWidget *parent = 0);

    double current_value_inchi;
    double current_value_mm;
    double base_value;

    void initialize(SDIInt default_value);
    void Finalize(void);

    void CreateBaseToValue(void);
    double CreateDisplayValue(double value);
    bool isUnit_inchi(void);

public slots:
    void setMaxValue(double value);

private:

    void SetSpinBoxParam(void);
    void CreateValueToBase(void);

    void set_enabled(bool enabled);

};


#endif // DETECTIONAREAMINDOUBLESPINBOX_H
