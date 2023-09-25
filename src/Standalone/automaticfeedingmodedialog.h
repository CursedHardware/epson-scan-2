#ifndef AUTOMATICFEEDINGMODEDIALOG_H
#define AUTOMATICFEEDINGMODEDIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include <QTimer>

#include "supervisor.h"
#include "custommessagebox.h"

namespace Ui {
class AutomaticFeedingModeDialog;
}

class AutomaticFeedingModeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AutomaticFeedingModeDialog(Supervisor* sv, CustomMessageBox* msg_box, QWidget *parent = 0);
    ~AutomaticFeedingModeDialog();

    int getPagecount();

protected:
    void closeEvent(QCloseEvent* event);
    void ScanInitialize(void);

private:
    Ui::AutomaticFeedingModeDialog *ui;
    Supervisor *sv;
    int page_count;

    QTimer* m_timer;
    bool do_cancel;

    void PlacePaper();
    void EmptyPaper();
    void ButtonAction();

    CustomMessageBox* m_messageBox;

private slots:
    void Update();
    void Cancel();
};

#endif // AUTOMATICFEEDINGMODEDIALOG_H
