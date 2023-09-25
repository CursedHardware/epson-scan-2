#ifndef DETECTDOUBLEFEEDSETTINGSDIALOG_H
#define DETECTDOUBLEFEEDSETTINGSDIALOG_H

#include <QDialog>
#include "supervisor.h"

namespace Ui {
class DetectDoubleFeedSettingsDialog;
}

class DetectDoubleFeedSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DetectDoubleFeedSettingsDialog(QWidget *parent = 0);
    ~DetectDoubleFeedSettingsDialog();

    void Initialize(DOUBLE_FEED_DETECTION_VALUE detectdoublefeed_para);

    void SetValue(void);
    void Finalize(void);
    void GetValue(double *min, double *max, SDIInt *pt);

public slots:
    void onValueChanged(int value);

private:
    Ui::DetectDoubleFeedSettingsDialog *det_double_feed_ui;

    DoubleFeedDetection paper_thickness;

    double detection_area_min;
    double detection_area_max;

    INT_SETTING_VALUE DoubleFeedDetectionLevel_item;

    void updateUI(void);

    void select_units(void);
};

#endif // DETECTDOUBLEFEEDSETTINGSDIALOG_H
