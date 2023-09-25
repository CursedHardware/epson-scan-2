#ifndef DETECTDOUBLEFEEDCOMB_H
#define DETECTDOUBLEFEEDCOMB_H

#include <QComboBox>
#include "supervisor.h"
#include "detectdoublefeedsettingsdialog.h"
#include "selectabledetectdoublefeedsettingsdialog.h"

class DetectDoubleFeedComb : public QComboBox
{
public:
    DetectDoubleFeedComb(QWidget *parent = 0);

    bool changed;

    double max_value;
    double min_value;
    INT_SETTING_VALUE pt_value;

    void initialize(INT_SETTING_VALUE detectdoublefeed_para);

    void focusOutEvent(QFocusEvent* event);

    void update_ui(DOUBLE_FEED_DETECTION_VALUE detectdoublefeed_para);

    void item_event(int value, DOUBLE_FEED_DETECTION_VALUE* detectdoublefeed_para);
    void item_event_options(DOUBLE_FEED_DETECTION_VALUE* detectdoublefeed_para);

private:
    DetectDoubleFeedSettingsDialog* detectdoublefeedsettings_dialog;
    SelectableDetectDoubleFeedSettingsDialog* selectdetectdoublefeedsettings_dialog;

    void set_enabled(bool enabled);

    void add_item(SDICapability capability);

    void select_item(INT_SETTING_VALUE detectdoublefeed_para);
};

#endif // DETECTDOUBLEFEEDCOMB_H
