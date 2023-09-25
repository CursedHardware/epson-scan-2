#ifndef DOCUMENTSIZE_H
#define DOCUMENTSIZE_H

#define DOCUMENT_LIST_SIZE 100

#include <QComboBox>
#include "supervisor.h"
#include "documentsizedialog.h"
#include "documentsizesettingdialog.h"

class Document_Size : public QComboBox
{

public:
    Document_Size(QWidget *parent = 0);

    DocumentSize current_item;
    double current_pixel_x;
    double current_pixel_y;
    AutoSizeType auto_detect;

    bool changed;

    int base_resolution;

    void initialize(SCAN_AREA_VALUE document_size_para, int cur_resolution, ScannerKind kind, INT_SETTING_VALUE* isInscribed);

    void update_ui(SCANPARA document_size_para, int cur_resolution, bool isLFPModel);
    void update_base_resolution(int cur_resolution);

    void item_event(int value, SCAN_AREA_VALUE* scan_area_para, INT_SETTING_VALUE* isInscribed);

    double calculate_pixel_size(double value, int res=0);
    double calculate_pixel_size_inchi(double value);

    void update_scanarea(SCAN_AREA_VALUE* scan_area_para);

    void ExistMarquee(void);

    void setMarqueeSize(QRect Marquee_Size);
    void setMarqueeOffset(QRect Marquee_Size);
    QPoint getMarqueeOffset(void);

    float previewOffsetXAsInch(bool ViewPreview, GuidePosition GuidePosition, float previewWidth, FunctionalUnit select_funcunit);

private:
    ScannerKind m_kind;
    DocumentSizeDialog* doumentsize_dialog;
    DocumentSizeSettingDialog* documentsize_setting_dialog;

    //QVariant old_select;

    DocumentSize doc_list[DOCUMENT_LIST_SIZE];
    int list_size;

    double user_define_width;
    double user_define_length;
    double m_marquee_width;
    double m_marquee_length;
    int m_marquee_x;
    int m_marquee_y;
    bool user_define_det_paper_length;
    bool needSettingOption;

    double max_pixel_x;
    double max_pixel_y;

    void create_list(SCANPARA scan_area_para, bool isLFPModel);

    void set_enabled(bool enabled);

    void add_option();
    void add_setting_option(INT_SETTING_VALUE setting_para);

    void select_item();

    void CreateComboBoxToolTip(DocumentSize current_item, SCAN_AREA_VALUE scan_area_para);
    void create_custom_size(QString* strDir);
    void create_maximum_size(QString* strDir, SCAN_AREA_VALUE scan_area_para);

    void item_event_options(SCAN_AREA_VALUE document_size_para);
    void item_event_settings(INT_SETTING_VALUE* document_size_para);

    void append_custom_size(QString* strDir); 

    void StandardSizeToPixel();
};

#endif // DOCUMENTSIZE_H
