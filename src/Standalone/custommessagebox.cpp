#include "custommessagebox.h"
#include "continuousscandialog.h"
#include "translationstring.h"
#include "../Include/Config.h"
#include <QCheckBox>

CustomMessageBox::CustomMessageBox(QWidget* parent)
{
    parent_dialog = parent;
}

bool CustomMessageBox::CreateMessageBox(QString message, QString button_message, IconType type, bool use_doNotShow)
{
    QMessageBox msgBox(parent_dialog);
    msgBox.setText(message + "\n");
#ifdef AKBMODE
    msgBox.setWindowTitle(DRIVER_TITLE);
#else
    msgBox.setWindowTitle(TranslationString::GetString().translation_str.MENU_001);
#endif
    msgBox.setStandardButtons(QMessageBox::Yes);
    msgBox.setDefaultButton(QMessageBox::Yes);

    //ボタンのテキスト変更
    msgBox.setButtonText(QMessageBox::Yes, button_message);

    switch (type){
        case Question:
            msgBox.setIcon(QMessageBox::Question);
            break;
        case Information:
            msgBox.setIcon(QMessageBox::Information);
            break;
        case Warning:
            msgBox.setIcon(QMessageBox::Warning);
            break;
        case Critical:
            msgBox.setIcon(QMessageBox::Critical);
            break;
    }

    if(use_doNotShow){
        QCheckBox *doNotShowCheckBox = new QCheckBox(TranslationString::GetString().translation_str.LINUX_C_UI_002, parent_dialog);
        doNotShowCheckBox->setGeometry(10, 30, 0, 300);

        msgBox.setCheckBox(doNotShowCheckBox);

        msgBox.exec();

        if(doNotShowCheckBox->checkState() == Qt::Checked ){
            return true;
        }
    }else {
        msgBox.exec();
    }

    return false;
}

bool CustomMessageBox::CreateSelectableMessageBox(QString message, IconType type)
{
    QMessageBox msgBox(parent_dialog);

    msgBox.setText(message + "\n");
#ifdef AKBMODE
    msgBox.setWindowTitle(DRIVER_TITLE);
#else
    msgBox.setWindowTitle(TranslationString::GetString().translation_str.MENU_001);
#endif

    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);

    //ボタンのテキスト変更
    msgBox.setButtonText(QMessageBox::Yes, TranslationString::GetString().translation_str.ES2U_MAIN_052);
    msgBox.setButtonText(QMessageBox::No, TranslationString::GetString().translation_str.ES2U_MAIN_053);

    switch (type){
        case Question:
            msgBox.setIcon(QMessageBox::Question);
            break;
        case Information:
            msgBox.setIcon(QMessageBox::Information);
            break;
        case Warning:
            msgBox.setIcon(QMessageBox::Warning);
            break;
        case Critical:
            msgBox.setIcon(QMessageBox::Critical);
            break;
    }

    int res = msgBox.exec();

    if (res == QMessageBox::Yes){
        return true;
    } else if (res == QMessageBox::No) {
        return false;
    }
    return false;
}

void CustomMessageBox::ErrorMessageBox(Supervisor* sv, bool Preview)
{
    QString error_message;
    IconType type = Warning;

    if(sv->m_LastError != kSDIErrorNone){
        if(sv->m_LastError == kSDIErrorPaperEmpty){
            //原稿なしエラー時のダイアログ
            error_message = TranslationString::GetString().translation_str.E_SCAN_101;
        }else if(sv->m_LastError == kSDIErrorPaperJam){
            if(Preview){
                error_message = TranslationString::GetString().translation_str.E_SCAN_103;
                sv->scanning_status = CANCEL;
            }else {
                //紙ジャムエラー時の復帰ダイアログ
                this->PaperJamDialog(sv, TranslationString::GetString().translation_str.CONFIRMCONTINUE_008);
                return;
            }
        }else if(sv->m_LastError == kSDIErrorPaperDoubleFeed){
            if(Preview){
                error_message = TranslationString::GetString().translation_str.E_SCAN_104;
                sv->scanning_status = CANCEL;
            }else {
                //重送エラー時の復帰ダイアログ
                this->PaperJamDialog(sv, TranslationString::GetString().translation_str.CONFIRMCONTINUE_002);
                return;
            }
        }else if(sv->m_LastError == kSDIErrorPaperDoubleFeed2){
            if(Preview){
                error_message = TranslationString::GetString().translation_str.E_SCAN_104;
                sv->scanning_status = CANCEL;
            }else {
                //重送エラー(排紙後モード)時の復帰ダイアログ
                this->PaperJamDialog(sv, TranslationString::GetString().translation_str.CONFIRMCONTINUE_013, AFTER_EJECT);
                return;
            }
        }else if(sv->m_LastError == kSDIErrorPaperProtect){
            if(Preview){
                error_message = TranslationString::GetString().translation_str.E_SCAN_103;
                sv->scanning_status = CANCEL;
            }else {
                //原稿保護エラー時の復帰ダイアログ
                this->PaperJamDialog(sv, TranslationString::GetString().translation_str.CONFIRMCONTINUE_011);
                return;
            }
        }else  if(sv->m_LastError == kSDIErrorDeviceInBusy){
            error_message = TranslationString::GetString().translation_str.E_COMM_001;
        }else  if(sv->m_LastError == kSDIErrorCoverOpen){
            //ADFカバーオープンエラー時のダイアログ
            error_message = TranslationString::GetString().translation_str.E_SCAN_102;
        }else  if(sv->m_LastError == kSDIErrorTrayClose){
            //給紙トレークローズエラー時のダイアログ
            error_message = TranslationString::GetString().translation_str.E_SCAN_105;
        }else  if(sv->m_LastError == kSDIErrorInvalidResponse){
            error_message = TranslationString::GetString().translation_str.E_SCAN_003;
        }else  if(sv->m_LastError == kSDIErrorNoMemory){
            //メモリ不足エラー時のダイアログ
            type = Critical;
            error_message = TranslationString::GetString().translation_str.E_ENV_001;
            sv->scanning_status = ERROR;
        }else if(sv->m_LastError == kSDITransferEventTypeCancel){
            return;
        }else if(sv->m_LastError == kSDIErrorDataSend){
            //データ送信エラーのダイアログ
            type = Critical;
            error_message = TranslationString::GetString().translation_str.E_COMM_103;
            sv->scanning_status = ERROR;
        }else if(sv->m_LastError == kSDIErrorDataReceive){
            //データ受信エラーのダイアログ
            type = Critical;
            error_message = TranslationString::GetString().translation_str.E_COMM_102;
            sv->scanning_status = ERROR;
        }else if(sv->m_LastError == kSDIErrorDeviceInUse){
            //スキャナーが使用中または使用できないエラーのダイアログ
            type = Critical;
            error_message = TranslationString::GetString().translation_str.E_COMM_001;
            sv->scanning_status = ERROR;
        }else  if(sv->m_LastError == kSDIErrorWriteFile){
            //ファイルの書き込みエラー時のダイアログ
            error_message = TranslationString::GetString().translation_str.E_SAVE_001;
        }else if(sv->m_LastError == kSDIErrorUnknownError){
            //予期せぬエラー・不明なエラー時のダイアログ
            type = Critical;
            error_message = TranslationString::GetString().translation_str.E_SCAN_001;
            sv->scanning_status = ERROR;
        }else if(sv->m_LastError == kSDIErrorDeviceFormChangedInterruptedly){
            if(Preview){
                error_message = TranslationString::GetString().translation_str.E_SCAN_110;
                sv->scanning_status = CANCEL;
            }else {
                //デバイスの姿勢がスキャン中に変更されたエラー時のダイアログ
                this->PaperJamDialog(sv, TranslationString::GetString().translation_str.E_SCAN_111);
                return;
            }
        }else if(sv->m_LastError == kSDIErrorDeviceFormUnstable){
            //デバイスの姿勢が不定状態エラー時のダイアログ
            type = Critical;
            error_message = TranslationString::GetString().translation_str.E_SCAN_109;
            sv->scanning_status = ERROR;
        }else if(sv->m_LastError == kSDIErrorSepLeverChangedInterruptedly){
            if(Preview){
                error_message = TranslationString::GetString().translation_str.E_SCAN_113;
                sv->scanning_status = CANCEL;
            }else {
                //原稿分離レバーがスキャン中に変更されたエラー時のダイアログ
                this->PaperJamDialog(sv, TranslationString::GetString().translation_str.E_SCAN_114);
                return;
            }
        }else if(sv->m_LastError == kSDIErrorUnscannableDeviceConfig1){
            //スキャナの姿勢と分離レバー組み合わせエラー時のダイアログ
            type = Critical;
            error_message = TranslationString::GetString().translation_str.E_SCAN_107;
            sv->scanning_status = ERROR;
        }else if(sv->m_LastError == kSDIErrorETSensorError || sv->m_LastError == kSDIErrorPESensorError){
            //ET Sensor error時のダイアログ
            type = Critical;
            error_message = TranslationString::GetString().translation_str.A_UNIT_007;
            sv->scanning_status = ERROR;
        }else if(sv->m_LastError == kSDIErrorUserAuthEnabled){
            //本体認証モードエラー
            type = Critical;
            error_message = TranslationString::GetString().translation_str.E_UNIT_008;
            sv->scanning_status = ERROR;
        }else if(sv->m_LastError == kSDIErrorPaperRemoveError){
            //PRMWエラー
            error_message = TranslationString::GetString().translation_str.E_SCAN_117;
            sv->scanning_status = CANCEL;
        }else if(sv->m_LastError == kSDIErrorSkewDetectError){
            //本体のスキュー角度検知失敗エラー
            error_message = TranslationString::GetString().translation_str.E_SCAN_118;
            sv->scanning_status = CANCEL;
        }else if(sv->m_LastError == kSDIErrorDeviceFatalError){
            //本体のフェータルエラー
            error_message = TranslationString::GetString().translation_str.E_SCAN_003 + "\n(E425-" + QString::number(sv->CheckDeviceErrorStatus()) + ")";
        }else {
            //予期せぬエラー・不明なエラー時のダイアログ
            type = Critical;
            error_message = TranslationString::GetString().translation_str.E_SCAN_001;
            sv->scanning_status = ERROR;
        }
    }else {
        return;
    }

    CreateMessageBox(error_message, TranslationString::GetString().translation_str.VERSION_003, Warning);
}

void CustomMessageBox::PaperJamDialog(Supervisor* sv, QString message, DoubleFeedMode dialog_type)
{
    ContinuousScanDialog* continuous_dialog = new ContinuousScanDialog(sv);

    if(continuous_dialog == NULL){
        return;
    }

    do{
        continuous_dialog->Initialize(message, dialog_type);

        if(continuous_dialog->isHidden()){
            continuous_dialog->exec();
        }else {
            continuous_dialog->activateWindow();
        }
        sv->scanning_status = continuous_dialog->GetScanningStatus();

        if(dialog_type == AFTER_EJECT && !sv->device_data.AutomaticFeedingMode.select){
            if(sv->scanning_status == DISCARD || sv->scanning_status == CONTINUE){
                //事前紙なしエラーCheck
                if(sv->device_data.FunctionalUnit.select == kSDIFunctionalUnitDocumentFeeder){
                    if(!sv->GetPaperLoadedStatus()){
                        //紙なしエラー
                        sv->m_LastError = kSDIErrorPaperEmpty;
                        if(sv->scanning_status == DISCARD){
                            this->ErrorMessageBox(sv);
                        }
                    }else {
                        sv->m_LastError = sv->CheckDeviceErrorStatus();
                    }
                }else {
                    sv->m_LastError = sv->CheckDeviceErrorStatus();
                }
            }
        }
        if(sv->device_data.AutomaticFeedingMode.select){
           break;
        }
    }while(sv->m_LastError == kSDIErrorPaperEmpty && sv->scanning_status == DISCARD);

    if(sv->m_LastError == kSDIErrorCoverOpen){
        if(sv->scanning_status == CONTINUE || sv->scanning_status == DISCARD){
            this->ErrorMessageBox(sv);
        }
    }
}
