#ifndef SKIPBLANKPAGESSETTINGSDIALOG_H
#define SKIPBLANKPAGESSETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SkipBlankPagesSettingsDialog;
}

class SkipBlankPagesSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SkipBlankPagesSettingsDialog(int default_lv, QWidget *parent = 0);
    ~SkipBlankPagesSettingsDialog();

    void SetValue(void);
    void GetValue(int *level);

public slots:
    void onValueChanged(int value);

private:
    Ui::SkipBlankPagesSettingsDialog *det_skip_ui;

    int detection_level;

    void updateUI(void);
};

#endif // SKIPBLANKPAGESSETTINGSDIALOG_H
