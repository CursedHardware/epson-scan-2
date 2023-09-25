#ifndef LENGTHDOUBLESPINBOX_H
#define LENGTHDOUBLESPINBOX_H

#include <QDoubleSpinBox>
#include "supervisor.h"

class LengthDoubleSpinBox : public QDoubleSpinBox
{

public:
    LengthDoubleSpinBox(QWidget *parent = 0);

    double current_value_inchi;
    double current_value_mm;
    double base_value;

    void initialize(SCAN_AREA_VALUE default_value);
    void Finalize(void);

    void CreateBaseToValue(void);
    double CreateDisplayValue(double value);
    bool isUnit_inchi(void);

private:

    void SetSpinBoxParam(SCAN_AREA_VALUE default_value);
    void CreateValueToBase(void);

    void set_enabled(bool enabled);

};

#endif // LENGTHDOUBLESPINBOX_H
