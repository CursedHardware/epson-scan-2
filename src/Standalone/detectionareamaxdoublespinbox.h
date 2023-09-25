#ifndef DETECTIONAREAMAXDOUBLESPINBOX_H
#define DETECTIONAREAMAXDOUBLESPINBOX_H

#include <QDoubleSpinBox>
#include "supervisor.h"

class DetectionAreaMaxDoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT

public:
    DetectionAreaMaxDoubleSpinBox(QWidget *parent = 0);

    double current_value_inchi;
    double current_value_mm;
    double base_value;

    void initialize(DOUBLE_FEED_DETECTION_VALUE detectdoublefeed_para);
    void Finalize(void);

    void CreateBaseToValue(void);
    double CreateDisplayValue(double value);
    bool isUnit_inchi(void);

public slots:
    void setMinValue(double value);

private:

    void SetSpinBoxParam(DOUBLE_FEED_DETECTION_VALUE detectdoublefeed_para);
    void CreateValueToBase(void);

    void set_enabled(bool enabled);

};

#endif // DETECTIONAREAMAXDOUBLESPINBOX_H
