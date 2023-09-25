#include "mainwindow.h"

Document_Size::Document_Size(QWidget *parent) :
    QComboBox(parent),
    doumentsize_dialog(NULL),
    documentsize_setting_dialog(NULL),
    changed(false),
    current_item(A4),
    current_pixel_x(0),
    current_pixel_y(0),
    auto_detect(AutoSizeNone),
    base_resolution(100),
    list_size (0),
    user_define_width(0),
    user_define_length(0),
    m_marquee_width(0),
    m_marquee_length(0),
    user_define_det_paper_length(0),
    max_pixel_x(0),
    max_pixel_y(0)
{
    memset(&doc_list, 0, sizeof(DocumentSize)*DOCUMENT_LIST_SIZE);
}

void Document_Size::initialize(SCAN_AREA_VALUE document_size_para, int cur_resolution, ScannerKind kind, INT_SETTING_VALUE* isInscribed){
    m_kind = kind;
    changed = true;
    current_item = document_size_para.FixedSize;
    base_resolution = cur_resolution;
    list_size = 0;
    user_define_width = document_size_para.UserDefine_Width;
    user_define_length = document_size_para.UserDefine_Length;
    user_define_det_paper_length = document_size_para.PaperEndDetectionMemory;
    auto_detect = (AutoSizeType)document_size_para.AutoSize.select;
    max_pixel_x = document_size_para.MaxScanAreaWidth.select;
    max_pixel_y = document_size_para.MaxScanAreaHeight.select;

    //Initialize dialog
    if(!doumentsize_dialog){
        doumentsize_dialog = new DocumentSizeDialog;
        if(doumentsize_dialog == NULL){
            return;
        }
    }

    doumentsize_dialog->Initialize(document_size_para, m_kind);

    //Initialize setting dialog
    if(!documentsize_setting_dialog){
        documentsize_setting_dialog = new DocumentSizeSettingDialog(isInscribed);
        if(documentsize_setting_dialog == NULL)
        {
            return;
        }
    }
    documentsize_setting_dialog->Initialize(isInscribed);

    this->CreateComboBoxToolTip(current_item, document_size_para);
}

void Document_Size::update_ui(SCANPARA scan_area_para, int cur_resolution, bool isLFPModel){

    this->base_resolution = cur_resolution;
    if(this->itemData(this->currentIndex()) != Marquee){
        //this->base_resolution = cur_resolution;
        if(changed == true){
            this->clear();
            this->create_list(scan_area_para, isLFPModel);
            this->select_item();
            this->CreateComboBoxToolTip(current_item, scan_area_para.ScanArea);
            this->set_enabled(true);
            changed = false;
        }
    }
}

void Document_Size::update_base_resolution(int cur_resolution)
{
    this->base_resolution = cur_resolution;
}

void Document_Size::set_enabled(bool enabled)
{
    this->setEnabled(enabled);
}

void Document_Size::ExistMarquee(void)
{
    this->clear();
    this->setToolTip("");
    this->insertItem(0, TranslationString::GetString().translation_str.SIZELIST_NOSIZE_032, Marquee);
}

void Document_Size::create_list(SCANPARA scan_area_para, bool isLFPModel){

    int resolution = 100;
    int item_num = 0;

    double min_x_pixel;
    double min_y_pixel;
    //FunctionalUnit is Auto
    if(scan_area_para.FunctionalUnit_Auto){
        min_x_pixel = 1;
        min_y_pixel = 1;
    }else {
        //Duplex ADF
        if(scan_area_para.DuplexType.select && scan_area_para.FunctionalUnit.select == ADF){
            min_x_pixel = scan_area_para.ScanArea.ADFWidthMin.select;
            if(scan_area_para.ScanArea.ADFDuplexHeightMin.select != 0){
                min_y_pixel = scan_area_para.ScanArea.ADFDuplexHeightMin.select;
            }else {
                min_y_pixel = scan_area_para.ScanArea.ADFHeightMin.select;
            }
        //ADF
        }else if(scan_area_para.FunctionalUnit.select == ADF){
            min_x_pixel = scan_area_para.ScanArea.ADFWidthMin.select;
            min_y_pixel = scan_area_para.ScanArea.ADFHeightMin.select;
        //Flatbed
        }else {
            min_x_pixel = 1;
            min_y_pixel = 1;
        }
    }
    max_pixel_x = scan_area_para.ScanArea.MaxScanAreaWidth.select;
    max_pixel_y = scan_area_para.ScanArea.MaxScanAreaHeight.select;

    double max_x_inchi = max_pixel_x / resolution;
    double max_y_inchi = max_pixel_y / resolution;
    double min_x_inchi = min_x_pixel / resolution;
    double min_y_inchi = min_y_pixel / resolution;

    for(int x = 0; x < scan_area_para.ScanArea.AutoSize.capability.countOfList; x++){
        if(scan_area_para.ScanArea.AutoSize.capability.list[x] == AutoSizeStandard){
            this->insertItem(item_num, TranslationString::GetString().translation_str.SIZELIST_NOSIZE_028, Auto_Detect);
            doc_list[item_num] = Auto_Detect;
            item_num++;
        }else if(scan_area_para.ScanArea.AutoSize.capability.list[x] == AutoSizeLongPaper){
            this->insertItem(item_num, TranslationString::GetString().translation_str.SIZELIST_NOSIZE_030, Auto_Detect_long);
            doc_list[item_num] = Auto_Detect_long;
            item_num++;
        }
    }

    if(min_x_inchi <= 33.11 && 33.11 <= max_x_inchi){
        if(min_y_inchi <= 46.81 && 46.81 <= max_y_inchi){
            this->insertItem(item_num, TranslationString::GetString().translation_str.SIZELIST_NOSIZE_038, A0);
            doc_list[item_num] = A0;
            item_num++;
        }
    }

    if(min_x_inchi <= 23.39 && 23.39 <= max_x_inchi){
        if(min_y_inchi <= 33.11 && 33.11 <= max_y_inchi){
            this->insertItem(item_num, TranslationString::GetString().translation_str.SIZELIST_NOSIZE_039, A1);
            doc_list[item_num] = A1;
            item_num++;
        }
    }

    if(min_x_inchi <= 16.54 && 16.54 <= max_x_inchi){
        if(min_y_inchi <= 23.39 && 23.39 <= max_y_inchi){
            this->insertItem(item_num, TranslationString::GetString().translation_str.SIZELIST_NOSIZE_040, A2);
            doc_list[item_num] = A2;
            item_num++;
        }
    }

    if(min_x_inchi <= 11.69 && 11.69 <= max_x_inchi){
        if(min_y_inchi <= 16.54 && 16.54 <= max_y_inchi){
            this->insertItem(item_num, TranslationString::GetString().translation_str.SIZELIST_NOSIZE_025, A3);
            doc_list[item_num] = A3;
            item_num++;
        }
    }

    if(min_x_inchi <= 8.27 && 8.27 <= max_x_inchi){
        if(min_y_inchi <= 11.69 && 11.69 <= max_y_inchi){
            this->insertItem(item_num, TranslationString::GetString().translation_str.SIZELIST_NOSIZE_017, A4);
            doc_list[item_num] = A4;
            item_num++;
        }
    }

    if(min_x_inchi <= 11.69 && 11.69 <= max_x_inchi){
        if(min_y_inchi <= 8.27 && 8.27 <= max_y_inchi){
            this->insertItem(item_num, TranslationString::GetString().translation_str.SIZELIST_NOSIZE_018, A4_Landscape);
            doc_list[item_num] = A4_Landscape;
            item_num++;
        }
    }

    if(min_x_inchi <= 5.83 && 5.83 <= max_x_inchi){
        if(min_y_inchi <= 8.27 && 8.27 <= max_y_inchi){
            this->insertItem(item_num, TranslationString::GetString().translation_str.SIZELIST_NOSIZE_011, A5);
            doc_list[item_num] = A5;
            item_num++;
        }
    }

    if(min_x_inchi <= 8.27 && 8.27 <= max_x_inchi){
        if(min_y_inchi <= 5.83 && 5.83 <= max_y_inchi){
            this->insertItem(item_num, TranslationString::GetString().translation_str.SIZELIST_NOSIZE_012, A5_Landscape);
            doc_list[item_num] = A5_Landscape;
            item_num++;
        }
    }

    if(min_x_inchi <= 4.13 && 4.13 <= max_x_inchi){
        if(min_y_inchi <= 5.83 && 5.83 <= max_y_inchi){
            this->insertItem(item_num, TranslationString::GetString().translation_str.SIZELIST_NOSIZE_009, A6);
            doc_list[item_num] = A6;
            item_num++;
        }
    }


    if(min_x_inchi <= 5.83 && 5.83 <= max_x_inchi){
        if(min_y_inchi <= 4.13 && 4.13 <= max_y_inchi){
            this->insertItem(item_num, TranslationString::GetString().translation_str.SIZELIST_NOSIZE_010, A6_Landscape);
            doc_list[item_num] = A6_Landscape;
            item_num++;
        }
    }

    if(min_x_inchi <= 2.05 && 2.05 <= max_x_inchi){
        if(min_y_inchi <= 2.91 && 2.91 <= max_y_inchi){
            this->insertItem(item_num, TranslationString::GetString().translation_str.SIZELIST_NOSIZE_007, A8);
            doc_list[item_num] = A8;
            item_num++;
        }
    }

    if(min_x_inchi <= 2.91 && 2.91 <= max_x_inchi){
        if(min_y_inchi <= 2.05 && 2.05 <= max_y_inchi){
            this->insertItem(item_num, TranslationString::GetString().translation_str.SIZELIST_NOSIZE_008, A8_Landscape);
            doc_list[item_num] = A8_Landscape;
            item_num++;
        }
    }

    if(min_x_inchi <= 28.66 && 28.66 <= max_x_inchi){
        if(min_y_inchi <= 40.55 && 40.55 <= max_y_inchi){
            this->insertItem(item_num, TranslationString::GetString().translation_str.SIZELIST_NOSIZE_041, B1_JIS);
            doc_list[item_num] = B1_JIS;
            item_num++;
        }
    }

    if(min_x_inchi <= 20.28 && 20.28 <= max_x_inchi){
        if(min_y_inchi <= 28.66 && 28.66 <= max_y_inchi){
            this->insertItem(item_num, TranslationString::GetString().translation_str.SIZELIST_NOSIZE_042, B2_JIS);
            doc_list[item_num] = B2_JIS;
            item_num++;
        }
    }

    if(min_x_inchi <= 14.33 && 14.33 <= max_x_inchi){
        if(min_y_inchi <= 20.28 && 20.28 <= max_y_inchi){
            this->insertItem(item_num, TranslationString::GetString().translation_str.SIZELIST_NOSIZE_043, B3_JIS);
            doc_list[item_num] = B3_JIS;
            item_num++;
        }
    }

    if(min_x_inchi <= 10.12 && 10.12 <= max_x_inchi){
        if(min_y_inchi <= 14.33 && 14.33 <= max_y_inchi){
            this->insertItem(item_num, TranslationString::GetString().translation_str.SIZELIST_NOSIZE_023, B4_JIS);
            doc_list[item_num] = B4_JIS;
            item_num++;
        }
    }

    if(min_x_inchi <= 14.33 && 14.33 <= max_x_inchi){
        if(min_y_inchi <= 10.12 && 10.12 <= max_y_inchi){
            this->insertItem(item_num, TranslationString::GetString().translation_str.SIZELIST_NOSIZE_024, B4_JIS_Landscape);
            doc_list[item_num] = B4_JIS;
            item_num++;
        }
    }

    if(min_x_inchi <= 7.17 && 7.17 <= max_x_inchi){
        if(min_y_inchi <= 10.12 && 10.12 <= max_y_inchi){
            this->insertItem(item_num, TranslationString::GetString().translation_str.SIZELIST_NOSIZE_013, B5_JIS);
            doc_list[item_num] = B5_JIS;
            item_num++;
        }
    }

    if(min_x_inchi <= 10.12 && 10.12 <= max_x_inchi){
        if(min_y_inchi <= 7.17 && 7.17 <= max_y_inchi){
            this->insertItem(item_num, TranslationString::GetString().translation_str.SIZELIST_NOSIZE_014, B5_JIS_Landscape);
            doc_list[item_num] = B5_JIS_Landscape;
            item_num++;
        }
    }

    if(min_x_inchi <= 8.5 && 8.5 <= max_x_inchi){
        if(min_y_inchi <= 14 && 14 <= max_y_inchi){
            this->insertItem(item_num, TranslationString::GetString().translation_str.SIZELIST_NOSIZE_019, Legal);
            doc_list[item_num] = Legal;
            item_num++;
        }
    }

    if(min_x_inchi <= 8.5 && 8.5 <= max_x_inchi){
        if(min_y_inchi <= 11 && 11 <= max_y_inchi){
            this->insertItem(item_num, TranslationString::GetString().translation_str.SIZELIST_NOSIZE_015, Letter);
            doc_list[item_num] = Letter;
            item_num++;
        }
    }

    if(min_x_inchi <= 11 && 11 <= max_x_inchi){
        if(min_y_inchi <= 8.5 && 8.5 <= max_y_inchi){
            this->insertItem(item_num, TranslationString::GetString().translation_str.SIZELIST_NOSIZE_016, Letter_Landscape);
            doc_list[item_num] = Letter_Landscape;
            item_num++;
        }
    }

    if(min_x_inchi <= 11 && 11 <= max_x_inchi){
        if(min_y_inchi <= 17 && 17 <= max_y_inchi){
            this->insertItem(item_num, TranslationString::GetString().translation_str.SIZELIST_NOSIZE_021, Tabloid);
            doc_list[item_num] = Tabloid;
            item_num++;
        }
    }

    if(min_x_inchi <= 3.94 && 3.94 <= max_x_inchi){
        if(min_y_inchi <= 5.83 && 5.83 <= max_y_inchi){
            this->insertItem(item_num, TranslationString::GetString().translation_str.SIZELIST_NOSIZE_001, Postcard);
            doc_list[item_num] = Postcard;
            item_num++;
        }
    }

    if(min_x_inchi <= 5.83 && 5.83 <= max_x_inchi){
        if(min_y_inchi <= 3.94 && 3.94 <= max_y_inchi){
            this->insertItem(item_num, TranslationString::GetString().translation_str.SIZELIST_NOSIZE_002, Postcard_Landscape);
            doc_list[item_num] = Postcard_Landscape;
            item_num++;
        }
    }

    if(min_x_inchi <= 2.13 && 2.13 <= max_x_inchi){
        if(min_y_inchi <= 3.37 && 3.37 <= max_y_inchi){
            if(scan_area_para.FunctionalUnit.select == Flatbed){
            }else if(scan_area_para.ScanArea.ADFCard.select == ADFCardSupport || scan_area_para.ScanArea.ADFCard.select == ADFCardPortrait){
                this->insertItem(item_num, TranslationString::GetString().translation_str.SIZELIST_NOSIZE_003, PlasticCard);
                doc_list[item_num] = PlasticCard;
                item_num++;
            }
        }
    }

    if(min_x_inchi <= 3.37 && 3.37 <= max_x_inchi){
        if(min_y_inchi <= 2.13 && 2.13 <= max_y_inchi){
            if(scan_area_para.FunctionalUnit.select == Flatbed || scan_area_para.FunctionalUnit_Auto){
                this->insertItem(item_num, TranslationString::GetString().translation_str.SIZELIST_NOSIZE_003, PlasticCard_Landscape);
                doc_list[item_num] = PlasticCard_Landscape;
                item_num++;
            }else if(scan_area_para.ScanArea.ADFCard.select == ADFCardSupport){
                this->insertItem(item_num, TranslationString::GetString().translation_str.SIZELIST_NOSIZE_004, PlasticCard_Landscape);
                doc_list[item_num] = PlasticCard_Landscape;
                item_num++;
            }else if(scan_area_para.ScanArea.ADFCard.select == ADFCardLandscape){
                this->insertItem(item_num, TranslationString::GetString().translation_str.SIZELIST_NOSIZE_003, PlasticCard_Landscape);
                doc_list[item_num] = PlasticCard_Landscape;
                item_num++;
            }
        }
    }

    if(!isLFPModel){
        this->insertItem(item_num, TranslationString::GetString().translation_str.SIZELIST_NOSIZE_027, Maximum_Size);
        doc_list[item_num] = Maximum_Size;
        item_num++;
    }


    this->add_option();
    this->add_setting_option(scan_area_para.AutoCroppingInscribed);
    list_size = item_num;
}

void Document_Size::add_option(){

    QString strDir;
    //this->append_custom_size(&strDir);

    if(user_define_width != 0 || user_define_length != 0){
        this->append_custom_size(&strDir);
        //this->insertSeparator(this->count());
        this->insertItem(this->count(), strDir, USER_DEFINE);
        this->insertSeparator(this->count()-1);
    }
    this->insertItem(this->count(), TranslationString::GetString().translation_str.SIZELIST_NOSIZE_029, OPTIONS);
    this->insertSeparator(this->count()-1);
}

void Document_Size::add_setting_option(INT_SETTING_VALUE setting_para){
    if(setting_para.capability.supportLevel == kSDISupportLevelAvailable)
    {
        this->insertItem(this->count(), TranslationString::GetString().translation_str.SIZELIST_NOSIZE_031, SETTING);
        this->insertSeparator(this->count()-1);
    }
}



void Document_Size::select_item()
{
    int index = this->findData(current_item);
    if(index != -1){
        this->setCurrentIndex(index);
    }else {
        //キーが見つからなかった場合デフォルトに戻す
        if(this->findData(Auto_Detect) != -1){
            current_item = Auto_Detect;
        }else {
            if(QLocale::system().name() == "en_US"){
                current_item = Letter;
            }else {
                current_item = A4;
            }
        }
        index = this->findData(current_item);
        this->setCurrentIndex(index);
    }
}

void Document_Size::CreateComboBoxToolTip(DocumentSize current_item, SCAN_AREA_VALUE scan_area_para)
{
    QString message;
    Configuration& config = Configuration::GetConfiguration();
    Units units = config.config_para.units;

    if(current_item == Auto_Detect){
        message = "";
    }else if(current_item == Auto_Detect_long){
        message = "";
    }else if(current_item == OPTIONS){
        message = "";
    }else if(current_item == SETTING){
        message = "";
    }else if(current_item == USER_DEFINE){
        create_custom_size(&message);
    }else if(current_item == Maximum_Size){
        create_maximum_size(&message, scan_area_para);
    }else {
        if(units == unit_inchi){
            if(current_item == A0){
                message = "33.11 x 46.81";
            }else if(current_item == A1){
                message = "23.39 x 33.11";
            }else if(current_item == A2){
                message = "16.54 x 23.39";
            }else if(current_item == A3){
                message = "11.69 x 16.54";
            }else if(current_item == A4){
                message = "8.27 x 11.69";
            }else if(current_item == A4_Landscape){
                message = "11.69 x 8.27";
            }else if(current_item == A5){
                message = "5.83 x 8.27";
            }else if(current_item == A5_Landscape){
                message = "8.27 x 5.83";
            }else if(current_item == A6){
                message = "4.13 x 5.83";
            }else if(current_item == A6_Landscape){
                message = "5.83 x 4.13";
            }else if(current_item == A8){
                message = "2.05 x 2.91";
            }else if(current_item == A8_Landscape){
                message = "2.91 x 2.05";
            }else if(current_item == B1_JIS){
                message = "28.66 x 40.55";
            }else if(current_item == B2_JIS){
                message = "20.28 x 28.66";
            }else if(current_item == B3_JIS){
                message = "14.33 x 20.28";
            }else if(current_item == B4_JIS){
                message = "10.12 x 14.33";
            }else if(current_item == B4_JIS_Landscape){
                message = "14.33 x 10.12";
            }else if(current_item == B5_JIS){
                message = "7.17 x 10.12";
            }else if(current_item == B5_JIS_Landscape){
                message = "10.12 x 7.17";
            }else if(current_item == s8K){
                message = "10.63 x 15.35";
            }else if(current_item == s16K){
                message = "7.67 x 10.63";
            }else if(current_item == s16K_Landscape){
                message = "10.63 x 7.67";
            }else if(current_item == Letter){
                message = "8.50 x 11.00";
            }else if(current_item == Letter_Landscape){
                message = "11.00 x 8.50";
            }else if(current_item == Legal){
                message = "8.50 x 14.00";
            }else if(current_item == Legal_Landscape){
                message = "14.00 x 8.50";
            }else if(current_item == Postcard){
                message = "3.94 x 5.83";
            }else if(current_item == Postcard_Landscape){
                message = "5.83 x 3.94";
            }else if(current_item == PlasticCard){
                message = "2.13 x 3.37";
            }else if(current_item == PlasticCard_Landscape){
                message = "3.37 x 2.13";
            }else if(current_item == Tabloid){
                message = "11.00 x 17.00";
            }else if(current_item == Tabloid_Landscape){
                message = "17.00 x 11.00";
            }
        }else if(units == unit_mm){
            if(current_item == A0){
                message = "841.0 x 1189.0";
            }else if(current_item == A1){
                message = "594.0 x 841.0";
            }else if(current_item == A2){
                message = "420.0 x 594.0";
            }else if(current_item == A3){
                message = "297.0 x 420.0";
            }else if(current_item == A4){
                message = "210.0 x 297.0";
            }else if(current_item == A4_Landscape){
                message = "297.0 x 210.0";
            }else if(current_item == A5){
                message = "148.0 x 210.0";
            }else if(current_item == A5_Landscape){
                message = "210.0 x 148.0";
            }else if(current_item == A6){
                message = "105.0 x 148.0";
            }else if(current_item == A6_Landscape){
                message = "148.0 x 105.0";
            }else if(current_item == A8){
                message = "52.0 x 74.0";
            }else if(current_item == A8_Landscape){
                message = "74.0 x 52.0";
            }else if(current_item == B1_JIS){
                message = "728.0 x 1030.0";
            }else if(current_item == B2_JIS){
                message = "515.0 x 728.0";
            }else if(current_item == B3_JIS){
                message = "364.0 x 515.0";
            }else if(current_item == B4_JIS){
                message = "257.0 x 364.0";
            }else if(current_item == B4_JIS_Landscape){
                message = "364.0 x 257.0";
            }else if(current_item == B5_JIS){
                message = "182.0 x 257.0";
            }else if(current_item == B5_JIS_Landscape){
                message = "257.0 x 182.0";
            }else if(current_item == s8K){
                message = "270.0 x 390.0";
            }else if(current_item == s16K){
                message = "195.0 x 270.0";
            }else if(current_item == s16K_Landscape){
                message = "270.0 x 195.0";
            }else if(current_item == Letter){
                message = "215.9 x 279.4";
            }else if(current_item == Letter_Landscape){
                message = "279.4 x 215.9";
            }else if(current_item == Legal){
                message = "215.9 x 355.6";
            }else if(current_item == Legal_Landscape){
                message = "355.6 x 215.9";
            }else if(current_item == Postcard){
                message = "100.0 x 148.0";
            }else if(current_item == Postcard_Landscape){
                message = "148.0 x 100.0";
            }else if(current_item == PlasticCard){
                message = "54.0 x 85.6";
            }else if(current_item == PlasticCard_Landscape){
                message = "85.6 x 54.0";
            }else if(current_item == Tabloid){
                message = "279.4 x 431.8";
            }else if(current_item == Tabloid_Landscape){
                message = "431.8 x 279.4";
            }
        }
        if(units == unit_inchi){
            message.append(" ");
            message.append(TranslationString::GetString().translation_str.CUSTOMSIZE_009);
        }else if(units == unit_mm){
            message.append(" ");
            message.append(TranslationString::GetString().translation_str.CUSTOMSIZE_008);
        }
    }

    this->setToolTip(message);
}

void Document_Size::create_custom_size(QString* strDir)
{
    doumentsize_dialog->SetSize();
    doumentsize_dialog->GetSize(&user_define_width, &user_define_length, &user_define_det_paper_length);

    //current_pixel_x = user_define_width;
    //current_pixel_y = user_define_length;

    Configuration& config = Configuration::GetConfiguration();
    Units units = config.config_para.units;

    strDir->append(QString(QString::number(user_define_width)));
    strDir->append(" x ");
    if(user_define_det_paper_length == true){
      strDir->append(TranslationString::GetString().translation_str.CUSTOMSIZE_010);
    }else {
        strDir->append(QString(QString::number(user_define_length)));
        if(units == unit_inchi){
            strDir->append(" ");
            strDir->append(TranslationString::GetString().translation_str.CUSTOMSIZE_009);
        }else if(units == unit_mm){
            strDir->append(" ");
            strDir->append(TranslationString::GetString().translation_str.CUSTOMSIZE_008);
        }
    }
}

void Document_Size::create_maximum_size(QString* strDir, SCAN_AREA_VALUE scan_area_para)
{
    Configuration& config = Configuration::GetConfiguration();
    Units units = config.config_para.units;

    if(units == unit_inchi){
        strDir->append(QString(QString::number(max_pixel_x/100)));
        strDir->append(" x ");
        strDir->append(QString(QString::number(max_pixel_y/100)));
        strDir->append(" ");
        strDir->append(TranslationString::GetString().translation_str.CUSTOMSIZE_009);
    }else if(units == unit_mm){
        strDir->append(QString(QString::number(max_pixel_x*25.4/100)));
        strDir->append(" x ");
        strDir->append(QString(QString::number(max_pixel_y*25.4/100)));
        strDir->append(" ");
        strDir->append(TranslationString::GetString().translation_str.CUSTOMSIZE_008);
    }
}

void Document_Size::item_event(int value, SCAN_AREA_VALUE* scan_area_para, INT_SETTING_VALUE* isInscribed)
{
    QVariant select_item = this->itemData(value);

    if(select_item == Auto_Detect){
        current_item = Auto_Detect;
    }else if(select_item == Auto_Detect_long){
        current_item = Auto_Detect_long;
    }else if(select_item == A0){
        current_item = A0;
    }else if(select_item == A1){
        current_item = A1;
    }else if(select_item == A2){
        current_item = A2;
    }else if(select_item == A3){
        current_item = A3;
    }else if(select_item == A4){
        current_item = A4;
    }else if(select_item == A4_Landscape){
        current_item = A4_Landscape;
    }else if(select_item == A5){
        current_item = A5;
    }else if(select_item == A5_Landscape){
        current_item = A5_Landscape;
    }else if(select_item == A6){
        current_item = A6;
    }else if(select_item == A6_Landscape){
        current_item = A6_Landscape;
    }else if(select_item == A8){
        current_item = A8;
    }else if(select_item == A8_Landscape){
        current_item = A8_Landscape;
    }else if(select_item == B1_JIS){
        current_item = B1_JIS;
    }else if(select_item == B2_JIS){
        current_item = B2_JIS;
    }else if(select_item == B3_JIS){
        current_item = B3_JIS;
    }else if(select_item == B4_JIS){
        current_item = B4_JIS;
    }else if(select_item == B4_JIS_Landscape){
        current_item = B4_JIS_Landscape;
    }else if(select_item == B5_JIS){
        current_item = B5_JIS;
    }else if(select_item == B5_JIS_Landscape){
        current_item = B5_JIS_Landscape;
    }else if(select_item == s8K){
        current_item = s8K;
    }else if(select_item == s16K){
        current_item = s16K;
    }else if(select_item == s16K_Landscape){
        current_item = s16K_Landscape;
    }else if(select_item == Letter){
        current_item = Letter;
    }else if(select_item == Letter_Landscape){
        current_item = Letter_Landscape;
    }else if(select_item == Legal){
        current_item = Legal;
    }else if(select_item == Legal_Landscape){
        current_item = Legal_Landscape;
    }else if(select_item == Postcard){
        current_item = Postcard;
    }else if(select_item == Postcard_Landscape){
        current_item = Postcard_Landscape;
    }else if(select_item == PlasticCard){
        current_item = PlasticCard;
    }else if(select_item == PlasticCard_Landscape){
        current_item = PlasticCard_Landscape;
    }else if(select_item == Tabloid){
        current_item = Tabloid;
    }else if(select_item == Tabloid_Landscape){
        current_item = Tabloid_Landscape;
    }else if(select_item == Maximum_Size){
        current_item = Maximum_Size;
    }else if(select_item == USER_DEFINE){
        current_item = (DocumentSize)USER_DEFINE;
    }else if(select_item == OPTIONS){
        this->item_event_options(*scan_area_para);
    }else if(select_item == SETTING){
        this->item_event_settings(isInscribed);
    }

    else {
        if(user_define_width != 0 || user_define_length != 0){
            doumentsize_dialog->SetSize();
            doumentsize_dialog->GetSize(&user_define_width, &user_define_length, &user_define_det_paper_length);
        }
    }

    this->update_scanarea(scan_area_para);

    this->CreateComboBoxToolTip(current_item, *scan_area_para);

    changed = true;
}

void Document_Size::update_scanarea(SCAN_AREA_VALUE* scan_area_para)
{
    this->StandardSizeToPixel();
    scan_area_para->ScanAreaWidth.select = current_pixel_x;
    scan_area_para->ScanAreaHeight.select = current_pixel_y;
    scan_area_para->AutoSize.select = auto_detect;
    scan_area_para->FixedSize = (DocumentSize)current_item;
    scan_area_para->UserDefine_Width = user_define_width;
    scan_area_para->UserDefine_Length = user_define_length;
    scan_area_para->PaperEndDetectionMemory = user_define_det_paper_length;
    if(current_item == (DocumentSize)USER_DEFINE && scan_area_para->PaperEndDetectionMemory){
        scan_area_para->PaperEndDetection.select = true;
    }else {
        scan_area_para->PaperEndDetection.select = false;
    }
}

void Document_Size::item_event_options(SCAN_AREA_VALUE document_size_para){

    QString strDir;

    //Show dialog
    doumentsize_dialog->Initialize(document_size_para, m_kind);
    if(doumentsize_dialog->isHidden()){
        doumentsize_dialog->exec();
    }else {
        doumentsize_dialog->activateWindow();
    }
    doumentsize_dialog->Finalize();

    this->append_custom_size(&strDir);

    current_item = (DocumentSize)USER_DEFINE;
}

void Document_Size::item_event_settings(INT_SETTING_VALUE* document_size_para){
    //Show dialog
    documentsize_setting_dialog->Initialize(document_size_para);
    if(documentsize_setting_dialog->isHidden() && document_size_para->capability.supportLevel == kSDISupportLevelAvailable){
        documentsize_setting_dialog->exec();
    }else {
        documentsize_setting_dialog->activateWindow();
    }
}

void Document_Size::append_custom_size(QString* strDir)
{
    doumentsize_dialog->SetSize();
    doumentsize_dialog->GetSize(&user_define_width, &user_define_length, &user_define_det_paper_length);

    current_pixel_x = user_define_width;
    current_pixel_y = user_define_length;

    Configuration& config = Configuration::GetConfiguration();
    Units units = config.config_para.units;

    *strDir = TranslationString::GetString().translation_str.LINUX_SIZELIST_NOSIZE_001;
    strDir->append(" (");

    strDir->append(QString(QString::number(user_define_width)));
    strDir->append(" x ");
    if(user_define_det_paper_length == true){
      strDir->append(TranslationString::GetString().translation_str.CUSTOMSIZE_010);
    }else {
        strDir->append(QString(QString::number(user_define_length)));
        if(units == unit_inchi){
            strDir->append(" ");
            strDir->append(TranslationString::GetString().translation_str.CUSTOMSIZE_009);
        }else if(units == unit_mm){
            strDir->append(" ");
            strDir->append(TranslationString::GetString().translation_str.CUSTOMSIZE_008);
        }
    }
    strDir->append(")");
}

double Document_Size::calculate_pixel_size(double value, int res)
{
    if(res == 0){
        res = base_resolution;
    }
    //mm -> pixel
    double mpp = 25.4 / (double)res;
    return value / mpp;
}

double Document_Size::calculate_pixel_size_inchi(double value)
{
    //inchi -> pixel
    return value * base_resolution;
}

#include "common_utility.h"

void Document_Size::setMarqueeSize(QRect Marquee_Size)
{
    //50dpiで計算されているピクセル数を一度mmに戻して現在の解像度で計算しなおす
    //m_marquee_width = this->calculate_pixel_size(common_utility.PixelToMm(Marquee_Size.width(), Configuration::GetConfiguration().config_para.preview_resolution), base_resolution);
    //m_marquee_length = this->calculate_pixel_size(common_utility.PixelToMm(Marquee_Size.height(), Configuration::GetConfiguration().config_para.preview_resolution), base_resolution);

    m_marquee_width = Marquee_Size.width() * (base_resolution / Configuration::GetConfiguration().config_para.preview_resolution);
    m_marquee_length = Marquee_Size.height() * (base_resolution / Configuration::GetConfiguration().config_para.preview_resolution);

}

void Document_Size::setMarqueeOffset(QRect Marquee_Size)
{
    //50dpiで計算されているピクセル数を一度mmに戻して現在の解像度で計算しなおす
    //m_marquee_x = this->calculate_pixel_size(common_utility.PixelToMm(Marquee_Size.x(), Configuration::GetConfiguration().config_para.preview_resolution), base_resolution);
    //m_marquee_y = this->calculate_pixel_size(common_utility.PixelToMm(Marquee_Size.y(), Configuration::GetConfiguration().config_para.preview_resolution), base_resolution);

    m_marquee_x = Marquee_Size.x() * (base_resolution / Configuration::GetConfiguration().config_para.preview_resolution);
    m_marquee_y = Marquee_Size.y() * (base_resolution / Configuration::GetConfiguration().config_para.preview_resolution);
}

QPoint Document_Size::getMarqueeOffset(void)
{
    return QPoint(m_marquee_x, m_marquee_y);
}

float Document_Size::previewOffsetXAsInch(bool ViewPreview, GuidePosition GuidePosition, float previewWidth, FunctionalUnit select_funcunit)
{
    float offsetX = 0;

    if (ViewPreview) { // プレビュー画像がある場合

        if(select_funcunit == Flatbed){
            offsetX = 0;
            return offsetX;
        }

        float previewWidth_inchi = common_utility.PixelToInchi(previewWidth, Configuration::GetConfiguration().config_para.preview_resolution);
        float maxWidth = common_utility.PixelToInchi(max_pixel_x / 100 * this->base_resolution, base_resolution);

        switch (GuidePosition) {
            case GuidePositionCenter:
            case GuidePositionCenterManual:
                offsetX = (maxWidth - previewWidth_inchi) / 2;
                break;

            case GuidePositionRight:
                offsetX = maxWidth - previewWidth_inchi;
                break;

            case GuidePositionLeft:
            default:
                offsetX = 0;
                break;
        }
    }

    return offsetX;
}

void Document_Size::StandardSizeToPixel()
{
    auto_detect = AutoSizeNone;

    if(this->itemData(this->currentIndex()) == Marquee){
        current_pixel_x = m_marquee_width;
        current_pixel_y = m_marquee_length;
    }else {

        if(current_item == Auto_Detect){
            auto_detect = AutoSizeStandard;
            //取り込み装置自動時のサイズ自動が非サポート時用にA4サイズを入れておく
            current_pixel_x = calculate_pixel_size(210);
            current_pixel_y = calculate_pixel_size(297);
        }else if(current_item == Auto_Detect_long){
            auto_detect = AutoSizeLongPaper;
        }else if(current_item == A0){
            current_pixel_x = calculate_pixel_size(841);
            current_pixel_y = calculate_pixel_size(1189);
        }else if(current_item == A1){
            current_pixel_x = calculate_pixel_size(594);
            current_pixel_y = calculate_pixel_size(841);
        }else if(current_item == A2){
            current_pixel_x = calculate_pixel_size(420);
            current_pixel_y = calculate_pixel_size(594);
        }else if(current_item == A3){
            current_pixel_x = calculate_pixel_size(297);
            current_pixel_y = calculate_pixel_size(420);
        }else if(current_item == A4){
            current_pixel_x = calculate_pixel_size(210);
            current_pixel_y = calculate_pixel_size(297);
        }else if(current_item == A4_Landscape){
            current_pixel_x = calculate_pixel_size(297);
            current_pixel_y = calculate_pixel_size(210);
        }else if(current_item == A5){
            current_pixel_x = calculate_pixel_size(148);
            current_pixel_y = calculate_pixel_size(210);
        }else if(current_item == A5_Landscape){
            current_pixel_x = calculate_pixel_size(210);
            current_pixel_y = calculate_pixel_size(148);
        }else if(current_item == A6){
            current_pixel_x = calculate_pixel_size(105);
            current_pixel_y = calculate_pixel_size(148);
        }else if(current_item == A6_Landscape){
            current_pixel_x = calculate_pixel_size(148);
            current_pixel_y = calculate_pixel_size(105);
        }else if(current_item == A8){
            current_pixel_x = calculate_pixel_size(52);
            current_pixel_y = calculate_pixel_size(74);
        }else if(current_item == A8_Landscape){
            current_pixel_x = calculate_pixel_size(74);
            current_pixel_y = calculate_pixel_size(52);
        }else if(current_item == B1_JIS){
            current_pixel_x = calculate_pixel_size(728);
            current_pixel_y = calculate_pixel_size(1030);
        }else if(current_item == B2_JIS){
            current_pixel_x = calculate_pixel_size(515);
            current_pixel_y = calculate_pixel_size(728);
        }else if(current_item == B3_JIS){
            current_pixel_x = calculate_pixel_size(364);
            current_pixel_y = calculate_pixel_size(515);
        }else if(current_item == B4_JIS){
            current_pixel_x = calculate_pixel_size(257);
            current_pixel_y = calculate_pixel_size(364);
        }else if(current_item == B4_JIS_Landscape){
            current_pixel_x = calculate_pixel_size(364);
            current_pixel_y = calculate_pixel_size(257);
        }else if(current_item == B5_JIS){
            current_pixel_x = calculate_pixel_size(182);
            current_pixel_y = calculate_pixel_size(257);
        }else if(current_item == B5_JIS_Landscape){
            current_pixel_x = calculate_pixel_size(257);
            current_pixel_y = calculate_pixel_size(182);
        }else if(current_item == s8K){
            current_pixel_x = calculate_pixel_size(270);
            current_pixel_y = calculate_pixel_size(390);
        }else if(current_item == s16K){
            current_pixel_x = calculate_pixel_size(195);
            current_pixel_y = calculate_pixel_size(270);
        }else if(current_item == s16K_Landscape){
            current_pixel_x = calculate_pixel_size(270);
            current_pixel_y = calculate_pixel_size(195);
        }else if(current_item == Letter){
            current_pixel_x = calculate_pixel_size(216);
            current_pixel_y = calculate_pixel_size(279);
        }else if(current_item == Letter_Landscape){
            current_pixel_x = calculate_pixel_size(279);
            current_pixel_y = calculate_pixel_size(216);
        }else if(current_item == Legal){
            current_pixel_x = calculate_pixel_size(216);
            current_pixel_y = calculate_pixel_size(356);
        }else if(current_item == Legal_Landscape){
            current_pixel_x = calculate_pixel_size(356);
            current_pixel_y = calculate_pixel_size(216);
        }else if(current_item == Postcard){
            current_pixel_x = calculate_pixel_size(100);
            current_pixel_y = calculate_pixel_size(148);
        }else if(current_item == Postcard_Landscape){
            current_pixel_x = calculate_pixel_size(148);
            current_pixel_y = calculate_pixel_size(100);
        }else if(current_item == PlasticCard){
            current_pixel_x = calculate_pixel_size(85.6);
            current_pixel_y = calculate_pixel_size(54);
        }else if(current_item == PlasticCard_Landscape){
            current_pixel_x = calculate_pixel_size(85.6);
            current_pixel_y = calculate_pixel_size(54);
        }else if(current_item == Tabloid){
            current_pixel_x = calculate_pixel_size(279);
            current_pixel_y = calculate_pixel_size(432);
        }else if(current_item == Tabloid_Landscape){
            current_pixel_x = calculate_pixel_size(432);
            current_pixel_y = calculate_pixel_size(279);
        }else if(current_item == Maximum_Size){
            current_pixel_x = max_pixel_x / 100 * this->base_resolution;
            current_pixel_y = max_pixel_y / 100 * this->base_resolution;
        }else if(current_item == USER_DEFINE){
            Configuration& config = Configuration::GetConfiguration();
            Units units = config.config_para.units;

            if(units == unit_inchi){
                if(user_define_det_paper_length){
                    current_pixel_x = calculate_pixel_size_inchi(user_define_width);
                }else {
                    current_pixel_x = calculate_pixel_size_inchi(user_define_width);
                    current_pixel_y = calculate_pixel_size_inchi(user_define_length);
                }
            }else if(units == unit_mm){
                if(user_define_det_paper_length){
                    current_pixel_x = calculate_pixel_size(user_define_width);
                }else {
                    current_pixel_x = calculate_pixel_size(user_define_width);
                    current_pixel_y = calculate_pixel_size(user_define_length);
                }
            }
        }
    }
}
