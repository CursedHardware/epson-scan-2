#ifndef FOCUSPOSITIONSETTINGDIALOG_H
#define FOCUSPOSITIONSETTINGDIALOG_H

#include <QDialog>
#include "supervisor.h"

namespace Ui {
class FocusPositionSettingDialog;
}

class FocusPositionSettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FocusPositionSettingDialog(SCANPARA* device_data, QWidget *parent = 0);
    ~FocusPositionSettingDialog();
    void SetValue(void);
    void GetValue(SCANPARA* device_data);

public slots:
    void onValueChanged(int value);
    void PushButtonAction(void);

private:
    Ui::FocusPositionSettingDialog *focus_position_ui;

    int m_focusPositionLevel;

    void updateUI(void);
};

#endif // FOCUSPOSITIONSETTINGDIALOG_H
