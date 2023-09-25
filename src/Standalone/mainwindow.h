#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QValidator>
#include <QFocusEvent>
#include "ui_mainwindow.h"
#include "documentsource.h"
#include "scanningside.h"
#include "documentsize.h"
#include "imagetype.h"
#include "resolution.h"
#include "rotate.h"
#include "deskewcheckbox.h"
#include "addpagescheckbox.h"
#include "skipblankpagescomb.h"
#include "detectdoublefeedcomb.h"

#include "noneradiobutton.h"
#include "removebackcombobox.h"
#include "dropoutcombobox.h"
#include "brightnessslider.h"
#include "contrastslider.h"
#include "gammaslider.h"
#include "gammadoublespinbox.h"
#include "thresholdslider.h"

#include "imageformatcomb.h"
#include "foldercomb.h"
#include "prefixlineedit.h"

#include "widthdoublespinbox.h"
#include "lengthdoublespinbox.h"
#include "detectpapercheckbox.h"

#include "skipblankpagesdetlvslider.h"

#include "paperthicknesscombobox.h"
#include "detectionareamindoublespinbox.h"
#include "detectionareamaxdoublespinbox.h"

#include "configurationdialog.h"
#include "configuration.h"

#include "unitscombobox.h"
#include "lastusedcheckbox.h"
#include "transferdataaftercompcheckbox.h"
#include "openimgfolderafterscancheckbox.h"
#include "sleeptimerspinbox.h"
#include "acpowercombobox.h"
#include "batterypowercombobox.h"

#include "scanbutton.h"

#include "deviceselectorcombobox.h"
#include "initdeviceselectcombobox.h"
#include "imageformatsettingscombobox.h"

#include "lastusedsettings.h"

#include "translationstring.h"
#include "deviceconnectiondialog.h"

#include "custommessagebox.h"

#include "backgroundcolorcombobox.h"
#include "automaticfeedingmodecombobox.h"

#include "previewwindow.h"
#include "qualitypreviewcheckbox.h"
#include "previewbutton.h"

#include "filenamesettingdialog.h"

#include "sensorglassdirtsensitivitycombobox.h"
#include "adfpaperprotectioncombobox.h"
#include "behaviorwhendoublefeedcombobox.h"

#include "nonconnectpoweroffcombobox.h"
#include "directpoweroncombobox.h"

#include "textenhancecombobox.h"

#include "removepunchholes.h"

#include "edgecorrection.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>
#include <QJsonArray>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const std::string edit_output_path, QWidget *parent = 0);
    ~MainWindow();

    void updateUI();

    bool m_viewPreviewWindow;

protected:
    void closeEvent(QCloseEvent* event);

    //Ui::MainWindow *ui;

private slots:
    void on_Scan_Button_pressed();
    void on_Preview_Button_pressed();

    void onValueChanged(int value);
    void onValueChangedQstr(QString value);
    void onValueChangedBool(bool check);
    void onSave_Button_pressed();
    void onConfiguration_Button_pressed();
    void onFolder_Button_pressed();
    void onFileNameSetting_Button_pressed();
    void onAbout_Epson_Scan2_pressed();
    void onHelp_Button_pressed();

    void onTextEnhanceSetting_Button_pressed();

    void EditFinishText(void);
    bool SelectionRestrictions(int value);
    bool CheckLongPaperMode(void);

    void MarqueeCreated(bool exist, QRect MarqueeSize);

private:
    Ui::MainWindow *ui;
    ConfigurationDialog* configuration_dialog;
    Supervisor *sv;
    LastUsedSettings* m_last_used_settings_controller;

    CustomMessageBox* m_messageBox;

    PreviewWindow* m_preview_window;

    bool m_DoNotClose;

    void SetupMainWindow(Configuration config);
    void CloseWindow();

    void showEvent( QShowEvent* event );

    bool search_item_list(int item, int32_t item_list[], int32_t list_size);

    bool ConnectToDevice(void);
    bool SelectDevice(void);
    void Initialize(void);
    void SetAllLabelEnabled(bool enabled);
    void Check2in1Difference();

    void ChangeExtension(ImageFormat imgformat);

    void ChangeWidget(SDIInt type);

    void CreateComboBoxToolTip(QComboBox* obj, QString message);
    void CreateRadioButtonToolTip(QRadioButton* obj, QString message);

    void InitializeImageOption(SDIInt select);
    //排他処理
    void ControlImageOption();
    void ControlDropoutAvailable(SDIInt type);
    void ControlRemoveBackGroundAvailable(SDIInt type);
    void ControlTextEnhanceAvailable(SDIInt type);
    void ControlPreviewButtonAvailable(SDIInt type);
    void ControlAutoLongPaperDropOutAvailable(SDIInt type);
    void ControlDetDoubleFeedAvailable(SDIInt type);
    void ControlSetDocumentAvailable(SDIInt type);
    void ControlSkipBlankPagesAvailable(SDIInt type);
    void ControlScanningSideAvailable(void);
    void ControlDocumentSizeAvailable(void);
    void ControlBackgroundColorAvailable(void);
    void ControlAutomaticFeedingModeAvailable(void);
    void ControlGammaAvabilable(void);
    void AvailableControlLFPModel(void);
    void ControlDocumentTypeAvailable(void);
    void ControlImageType(void);
    void ControlRemovePunchHoleAvailable(SDIInt type);
    void ControlEdgeCorrection(SDIInt type);

    //bool CustomMessageBox(QString message, QString button_message, IconType type, bool use_doNotShow=false);
    bool CheckAutoFunctionalUnit(bool isPreview);
    bool CheckBeforeScan(bool isPreview);
    qint32 GetHeightLimitLongPaper(int current_resolution);

    void open_folder();

    bool UserDefineSizeRestrictions(void);

    void isManualResolution(int resolution=0);

    void UpdateMarqueeSize_resolution(void);

    void ValidateOverRollerLifeLimit();

    void CheckMarqueeEnebled(void);

    void ChangePreviewParameter(bool start);

    void AllItemReset(void);

};

#endif // MAINWINDOW_H
