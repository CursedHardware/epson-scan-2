#ifndef CONFIGURATIONDIALOG_H
#define CONFIGURATIONDIALOG_H

#include <QDialog>
#include "supervisor.h"

typedef enum {Information_tab = 0, General_tab, Counter_tab, Power_Saving_tab, Maintenace_tab, Scanner_Setting_tab, Other_tab} TabNameNo;

namespace Ui {
class ConfigurationDialog;
}

class ConfigurationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigurationDialog(Supervisor* sv, QWidget *parent = 0);
    ~ConfigurationDialog();

    void Initialize();
    void UpdateInformationDisplay(SCANPARA device_data);
    void UpdateCounterDisplay(CONFIGURE_COUNTER_VALUE ConfigureCounter);
    void DisplayItem(void);
    bool Get_ResetFlag();
    bool Get_CounterResetFlag();

    bool CheckOverRollerKitLifeLimit(CONFIGURE_COUNTER_VALUE ConfigureCounter, bool* AlertAlready);
    bool CheckNearRollerKitLifeLimit(CONFIGURE_COUNTER_VALUE ConfigureCounter, bool* AlertAlready);
    bool CheckOverRollerLifeLimit(CONFIGURE_COUNTER_VALUE ConfigureCounter, bool* AlertAlready);
    bool CheckNearRollerLifeLimit(CONFIGURE_COUNTER_VALUE ConfigureCounter, bool* AlertAlready);
    bool CheckOverRetardRollerLifeLimit(CONFIGURE_COUNTER_VALUE ConfigureCounter, bool* AlertAlready);
    bool CheckNearRetardRollerLifeLimit(CONFIGURE_COUNTER_VALUE ConfigureCounter, bool* AlertAlready);

    void Check_AdminPassword(void);
    bool CheckUnlockButton_Enabled(void);

public slots:
    void onValueChanged(int value);
    void onValueChangedBool(bool value);
    void PushButtonAction(void);

private:
    Ui::ConfigurationDialog *config_ui;

    void updateUI(SCANPARA device_data);

    bool ResetMessageBox(void);
    bool CounterResetMessageBox(QString message);

    bool bReset;
    bool bCountReset;

    Supervisor* m_sv;

    void hideTab(const QString &tabName);
};

#endif // CONFIGURATIONDIALOG_H
