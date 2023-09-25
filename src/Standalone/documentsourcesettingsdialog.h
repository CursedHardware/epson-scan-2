#ifndef DOCUMENTSOURCESETTINGSDIALOG_H
#define DOCUMENTSOURCESETTINGSDIALOG_H

#include <QDialog>
#include "supervisor.h"

namespace Ui {
class DocumentSourceSettingsDialog;
}

class DocumentSourceSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DocumentSourceSettingsDialog(QWidget *parent = 0);
    ~DocumentSourceSettingsDialog();

    void Initialize(INT_SETTING_VALUE afm_para);
    int GetCurrentValue();

public slots:
    void onValueChanged(int value);

private:
    Ui::DocumentSourceSettingsDialog *ui;
};

#endif // DOCUMENTSOURCESETTINGSDIALOG_H
