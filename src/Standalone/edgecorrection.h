#ifndef EDGECORRECTION_H
#define EDGECORRECTION_H

#include <QComboBox>
#include "supervisor.h"
#include "edgecorrectionsettingsdialog.h"

class EdgeCorrection : public QComboBox
{
public:
    bool changed;
    bool current_item;

    EdgeCorrection(QWidget *parent = 0);

    void initialize(SCANPARA edge_correction_para);

    void update_ui(INT_SETTING_VALUE edge_correction_para, QLabel* label1, QLabel* label2);

    void item_event(int value, INT_SETTING_VALUE* edge_correction_para);

    void item_event_options(void);

private:
    EdgeCorrectionSettingsDialog* edgeCorrectionSettingsDialog;

    void add_item();

    void select_item(void);

};

#endif // EDGECORRECTION_H
