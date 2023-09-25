#include "edgecorrectionsettingsdialog.h"
#include "ui_edgecorrectionsettingsdialog.h"

EdgeCorrectionSettingsDialog::EdgeCorrectionSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EdgeCorrectionSettingsDialog)
{
    ui->setupUi(this);
}

EdgeCorrectionSettingsDialog::~EdgeCorrectionSettingsDialog()
{
    delete ui;
}
