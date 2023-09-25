#ifndef DOCUMENTSIZESETTINGDIALOG_H
#define DOCUMENTSIZESETTINGDIALOG_H

#include <QDialog>

#include "supervisor.h"

namespace Ui {
class DocumentSizeSettingDialog;
}

class DocumentSizeSettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DocumentSizeSettingDialog(INT_SETTING_VALUE* inscribed_para, QWidget *parent = 0);
    ~DocumentSizeSettingDialog();

    void Initialize(INT_SETTING_VALUE* document_size_setting);

    //INT_SETTING_VALUE current_item;

public slots:
    void onValueChangedBool(bool value);

private:
    Ui::DocumentSizeSettingDialog *ui;

    void updateUI(void);

    void ChangeDetPaperCheck(bool checked);

    INT_SETTING_VALUE* m_inscribed_para;
};

#endif // DOCUMENTSIZESETTINGDIALOG_H
