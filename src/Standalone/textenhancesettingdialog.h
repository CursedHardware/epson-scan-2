#ifndef TEXTENHANCESETTINGDIALOG_H
#define TEXTENHANCESETTINGDIALOG_H

#include <QDialog>
#include "supervisor.h"

namespace Ui {
class TextEnhanceSettingDialog;
}

class TextEnhanceSettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TextEnhanceSettingDialog(SCANPARA device_data, QWidget *parent = 0);
    ~TextEnhanceSettingDialog();

    void SetValue(void);
    void GetValue(SCANPARA* device_data);

public slots:
    void onValueChanged(int value);

private:
    Ui::TextEnhanceSettingDialog *text_enhance_ui;

    int m_textEnhanceSensitivity;
    int m_thresholdAdjustment;
    int m_textEnhanceNoiseReductionLevel;
    bool m_textEnhancePaperCreaseReduction;

    void updateUI(void);
};

#endif // TEXTENHANCESETTINGDIALOG_H
