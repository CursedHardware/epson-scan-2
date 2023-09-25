#ifndef EDGECORRECTIONSETTINGSDIALOG_H
#define EDGECORRECTIONSETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class EdgeCorrectionSettingsDialog;
}

class EdgeCorrectionSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EdgeCorrectionSettingsDialog(QWidget *parent = 0);
    ~EdgeCorrectionSettingsDialog();

private:
    Ui::EdgeCorrectionSettingsDialog *ui;
};

#endif // EDGECORRECTIONSETTINGSDIALOG_H
