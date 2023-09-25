#ifndef CONTINUOUSSCANDIALOG_H
#define CONTINUOUSSCANDIALOG_H

#include <QDialog>
#include <QGraphicsScene>
#include <QCloseEvent>

#include "supervisor.h"

namespace Ui {
class ContinuousScanDialog;
}

class ContinuousScanDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ContinuousScanDialog(Supervisor* sv, QWidget *parent = 0);
    ~ContinuousScanDialog();

    void Initialize(QString message, DoubleFeedMode dialog_type=IMMEDIATE);
    ScanningStatus GetScanningStatus();

protected:
    void closeEvent(QCloseEvent* event);

private slots:
    void onButtonPressed();

private:
    Ui::ContinuousScanDialog *ui;

    QGraphicsScene Scene_;
    QImage m_image;

    ScanningStatus m_scanning_status;
    DoubleFeedMode m_dialog_type;

    Supervisor* m_sv;

    void LoadScanImage();
    void ChangeStrings(DoubleFeedMode dialog_type, QString message);
    void AddADFSpecialErrorMessageForPassportStrings(QString message);
};

#endif // CONTINUOUSSCANDIALOG_H
