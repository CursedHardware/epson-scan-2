#ifndef DOCUMENTSIZEDIALOG_H
#define DOCUMENTSIZEDIALOG_H

#include <QDialog>

#include "supervisor.h"

namespace Ui {
class DocumentSizeDialog;
}

class DocumentSizeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DocumentSizeDialog(QWidget *parent = 0);
    ~DocumentSizeDialog();

    void Initialize(SCAN_AREA_VALUE document_size_para, ScannerKind kind);
    void Finalize();

    void SetSize(void);
    void GetSize(double *user_width, double *user_length, bool *user_det_paper_len);

    void select_units(void);

public slots:
    void onValueChangedBool(bool value);

private:
    Ui::DocumentSizeDialog *d_size_ui;

    double width;
    double length;
    bool det_paper_length;

    void updateUI(void);

    void ChangeDetPaperCheck(bool checked);

};

#endif // DOCUMENTSIZEDIALOG_H
