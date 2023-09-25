#ifndef SELECTABLEDETECTDOUBLEFEEDSETTINGSDIALOG_H
#define SELECTABLEDETECTDOUBLEFEEDSETTINGSDIALOG_H

#include <QDialog>
#include "supervisor.h"

namespace Ui {
class SelectableDetectDoubleFeedSettingsDialog;
}

class SelectableDetectDoubleFeedSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectableDetectDoubleFeedSettingsDialog(QWidget *parent = 0);
    ~SelectableDetectDoubleFeedSettingsDialog();

    void Initialize(INT_SETTING_VALUE DoubleFeedDetectionLevel);

    void SetValue(void);
    void Finalize(void);
    void GetValue(double *min, double *max, SDIInt *pt);

public slots:
    void onValueChanged(int value);
    void onValueChangedBool(bool check);

private:
    Ui::SelectableDetectDoubleFeedSettingsDialog *det_double_feed_ui;

    bool m_ultrasonic_sensor;

    DoubleFeedDetection paper_thickness;

    double detection_area_min;
    double detection_area_max;

    bool m_scanned_image;

    bool m_paper_protection;

    INT_SETTING_VALUE DoubleFeedDetectionLevel_item;

    void updateUI(void);

    void select_units(void);

    void WarningMessageBox(void);
};

#endif // SELECTABLEDETECTDOUBLEFEEDSETTINGSDIALOG_H
