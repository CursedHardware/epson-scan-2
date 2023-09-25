#include "automaticfeedingmodedialog.h"
#include "ui_automaticfeedingmodedialog.h"
#include "unistd.h"
#include "mainwindow.h"

#include "common_utility.h"
#include "PathUtils.h"

AutomaticFeedingModeDialog::AutomaticFeedingModeDialog(Supervisor* sv, CustomMessageBox* msg_box, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AutomaticFeedingModeDialog)
{
    ui->setupUi(this);
    this->setFixedSize(this->size());
    ui->progress_label->setText(QStringLiteral(""));

#ifdef NOUSEQRC
    std::string strIconPath = EPSON_INSTALL_PATH;
    ES_CMN_FUNCS::PATH::ES_CombinePath(strIconPath, strIconPath, "Resources/Icons/bmp_mss_image@2x.png");
    ui->Icon_toolButton->setIcon(QIcon(strIconPath.c_str()));
#endif
    setWindowFlags(Qt::WindowTitleHint | Qt::Window | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint) ;

    this->sv = sv;
    do_cancel = false;

    m_timer = NULL;
    m_messageBox = msg_box;

    ScanInitialize();
}

AutomaticFeedingModeDialog::~AutomaticFeedingModeDialog()
{
    //継続でそのまま終了したときに画像を保存するためSAVEに変更する
    if(sv->scanning_status == CONTINUE){
        sv->scanning_status = SAVE;
    }

    sv->Scan_Terminate();

    if(m_timer){
        if(m_timer->isActive()){
            m_timer->stop();
        }

        delete m_timer;
        m_timer = NULL;
    }
    delete ui;
}

void AutomaticFeedingModeDialog::closeEvent(QCloseEvent* event){
    event->ignore();
}

void AutomaticFeedingModeDialog::ScanInitialize(){

    page_count = 0;

    bool success;
    if(sv->device_data.use_continuousAutoFeedingMode){
        success = sv->Scan_Start(kSDIOperationTypeStartAFMC);
    }else {
        success = sv->Scan_Start(kSDIOperationTypeStartAFM);
    }
    if(success){
        //スキャン処理の監視のための定期実行QTimerの準備
        m_timer = new QTimer(this);
        if(m_timer == NULL){
            return;
        }
        m_timer->setInterval(100);
        m_timer->setSingleShot(false);
        connect(m_timer, SIGNAL(timeout()), this, SLOT(Update()));
        m_timer->start();
    }
}

void AutomaticFeedingModeDialog::Update()
{
    if(do_cancel || sv->scanning_status == CANCEL){
        //ダイアログのキャンセルボタン処理
        sv->Cancel_Scanning();
        sv->outEventType = kSDITransferEventTypeCancel;
        do_cancel = false;
    }else {
        if(page_count != 0){
            std::string message = (TranslationString::GetString().translation_str.PROGRESS_008.toLocal8Bit()).constData();
            std::string progress_message = common_utility.ReplaceString((char *)message.c_str(), "%d", std::to_string(page_count).c_str());
            ui->progress_label->setText(progress_message.c_str());
        }
        if(sv->CheckNextTransferEvent(false)){
            if(sv->Scanning()){
                if(sv->outEventType == kSDITransferEventTypeStartContinuousScanInAFM){
                    this->PlacePaper();
                }else
                if(sv->outEventType == kSDITransferEventTypeStopContinuousScanInAFM){
                    this->EmptyPaper();
                }else
                if(sv->outEventType == kSDITrasnferEventTypeImage){
                    sv->Get_Image();
                    sv->Save_Path();
                    page_count++;
                }
            }else {//scanning error
                if(sv->outEventType != kSDITransferEventTypeCancel){
                    sv->Get_Image();
                    sv->Scan_Terminate();
                    this->EmptyPaper();
                    //本体のエラーを解除しないままAFM待機に入れないため解除するまでループ
                    do{
                        m_messageBox->ErrorMessageBox(sv);
                    }while(sv->scanning_status == CONTINUE && (sv->CheckDeviceErrorStatus() != kSDIErrorNone && sv->CheckDeviceErrorStatus() != kSDIErrorPaperEmpty));
                    this->ButtonAction();
                }
            }
            sv->Dispose_Ptr();
        }else if(sv->outEventType == kSDITransferEventTypeComplete){
            m_timer->stop();
            this->reject();
        }
    }
}

void AutomaticFeedingModeDialog::Cancel()
{
    //ダイアログのキャンセルボタン処理
    do_cancel = true;
}

void AutomaticFeedingModeDialog::PlacePaper()
{
    QString progress_message = TranslationString::GetString().translation_str.PROGRESS_007;
    ui->progress_label->setText(progress_message);
    ui->cancel_pushButton->setEnabled(true);
    ui->Finish_pushButton->setEnabled(false);
    ui->scan_progressBar->setMaximum(0);
}

void AutomaticFeedingModeDialog::EmptyPaper()
{
    ui->progress_label->setText(QStringLiteral(""));
    ui->cancel_pushButton->setEnabled(false);
    ui->Finish_pushButton->setEnabled(true);
    ui->scan_progressBar->setMaximum(100);
}

void AutomaticFeedingModeDialog::ButtonAction()
{
    if(sv->scanning_status == SAVE){
        m_timer->stop();
        this->reject();
    }else if(sv->scanning_status == CANCEL){
        m_timer->stop();
        this->reject();
    }else if(sv->scanning_status == DISCARD){

    }else if(sv->scanning_status == CONTINUE){
        sv->scanning_status = SAVE; //default
        if(sv->device_data.use_continuousAutoFeedingMode){
            sv->Scan_Start(kSDIOperationTypeStartAFMC);
        }else {
            sv->Scan_Start(kSDIOperationTypeStartAFM);
        }
    }
}
