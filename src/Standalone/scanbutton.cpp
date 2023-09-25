#include <stdio.h>
#include <iostream>
#include <boost/format.hpp>
#include <QDesktopServices>

#include "scanbutton.h"
#include "mainwindow.h"
#include "scanningdialog.h"
#include "addpagedialog.h"
#include "addeditpagedialog.h"
#include "saveimagepathlist.h"
#include "PathUtils.h"
#include "savedestfolder.h"

#include "automaticfeedingmodedialog.h"

#include "rotateimage.h"
#include "supervisor.h"

#include <QtWidgets/QProgressDialog>


ScanButton::ScanButton(QWidget *parent) :
    QPushButton(parent)
{
    m_messageBox = NULL;
    m_mainWindow = NULL;
}

void ScanButton::initialize(CustomMessageBox* msg_box, QMainWindow* mainWindow)
{
    this->setEnabled(true);

    m_messageBox = msg_box;
    m_mainWindow = mainWindow;
}
 
 void MyProgessCallBack(int, int)
 {
    QCoreApplication::processEvents();

 }


void ScanButton::Button_pressed(Supervisor* sv, bool Add_Pages)
{
    ScanningDialog* scanning_dialog = NULL;
    sv->scanning_status = SAVE;
    bool first_image_save = false;
    bool edit_mode = false;
    QProgressDialog* saveProgressDialog = nullptr;

    //scanning roop
    do{
        //Initialize dialog///////////////////////////////////////////////////////////////////////////
        //AFM時は専用ダイアログを起動
        if(sv->device_data.AutomaticFeedingMode.select){
            AutomaticFeedingModeDialog* afm_dialog = new AutomaticFeedingModeDialog(sv, m_messageBox);
            if(afm_dialog == NULL){
                sv->scanning_status = ERROR;
                sv->m_LastError = kSDIErrorUnknownError;
                m_messageBox->ErrorMessageBox(sv);
                break;
            }
            afm_dialog->setAttribute(Qt::WA_DeleteOnClose);
            if(sv->m_LastError != kSDIErrorNone){
                m_messageBox->ErrorMessageBox(sv);
                break;
            }else {
                if(afm_dialog->isHidden()){
                    afm_dialog->exec();
                }else {
                    afm_dialog->activateWindow();
                }
            }
        //通常スキャン時
        }else {
            scanning_dialog = new ScanningDialog(sv);
            scanning_dialog->ScanInitialize(TranslationString::GetString().translation_str.PROGRESS_007);
            if(scanning_dialog == NULL){
                sv->scanning_status = ERROR;
                sv->m_LastError = kSDIErrorUnknownError;
                m_messageBox->ErrorMessageBox(sv);
                break;
            }
            scanning_dialog->setAttribute(Qt::WA_DeleteOnClose);
            if(sv->m_LastError != kSDIErrorNone){
                m_messageBox->ErrorMessageBox(sv);
                break;
            }else {
                if(scanning_dialog->isHidden()){
                    scanning_dialog->exec();
                }else {
                    scanning_dialog->activateWindow();
                }
            }
        }
        //////////////////////////////////////////////////////////////////////////////////////////////

        if(!sv->device_data.AutomaticFeedingMode.select){
        //エラー処理//////////////////////////////////////////////////////////////////////////////////
            //スキャン中のキャンセル処理があるか確認
            if(sv->outEventType == kSDITransferEventTypeCancel){
                m_messageBox->ErrorMessageBox(sv);
                sv->scanning_status = CANCEL;
                //キャンセルしたときも最終的にkSDITransferEventTypeCompleteを受け取ってから終了しなければならない
                //while(sv->outEventType != kSDITransferEventTypeComplete){
                //    sv->Scanning();
                //}
            }else
            //スキャン中のエラーをチェックしエラーダイアログを表示する
            if(sv->m_LastError != kSDIErrorNone){
                m_messageBox->ErrorMessageBox(sv);
                //紙なしエラー時はメインＵＩに戻る
                if(sv->m_LastError == kSDIErrorPaperEmpty){
                    if(sv->scanning_status == CONTINUE){
                        sv->scanning_status = SAVE;
                    }else {
                        sv->scanning_status = CANCEL;
                    }
                }else if(sv->m_LastError == kSDIErrorCoverOpen){
                    sv->scanning_status = CANCEL;
                }else
                if(sv->m_LastError == kSDIErrorNone){
                    //継続エラーダイアログ時にエラーでなければステータスは変更しない
                    ;
                }else
                //紙ジャム・重送エラー時はスキャン処理の継続はダイアログで選択されたアクションに任せる
                if(!(sv->m_LastError == kSDIErrorPaperJam ||
                     sv->m_LastError == kSDIErrorPaperDoubleFeed ||
                     sv->m_LastError == kSDIErrorPaperDoubleFeed2 ||
                     sv->m_LastError == kSDIErrorPaperProtect ||
                     sv->m_LastError == kSDIErrorDeviceFormChangedInterruptedly ||
                     sv->m_LastError == kSDIErrorPaperRemoveError ||
                     sv->m_LastError == kSDIErrorSkewDetectError ||
                     sv->m_LastError == kSDIErrorSepLeverChangedInterruptedly)){
                    //それ以外のエラーは終了
                    sv->scanning_status = ERROR;
                }
            }else {
                sv->scanning_status = SAVE;
            }
          //////////////////////////////////////////////////////////////////////////////////////////////

          //ページ追加処理//////////////////////////////////////////////////////////////////////////////
            if(sv->scanning_status == SAVE || (sv->scanning_status == CANCEL && first_image_save)){ //ページ追加時に1度もスキャンしていない状態でキャンセルされた場合メインウィンドウに戻る
                //スキャン後のページ追加フラグ処理
                if(Add_Pages){
                    do{
                        //Initialize dialog
                        if(sv->device_data.ImageFormatSettings.ImageFormat.select == ImageFormatTIFF){
                            //TIFFのときはページ追加ダイアログのみサポート（編集は別プラグインがいるため非サポートに）
                            AddPageDialog* add_page_dialog = new AddPageDialog(&sv->scanning_status);
                            if(add_page_dialog == NULL){
                                sv->scanning_status = ERROR;
                                sv->m_LastError = kSDIErrorUnknownError;
                                m_messageBox->ErrorMessageBox(sv);
                                break;
                            }
                            if(!edit_mode){
                                if(add_page_dialog->isHidden()){
                                    add_page_dialog->exec();
                                }else {
                                    add_page_dialog->activateWindow();
                                }
                            }
                            delete(add_page_dialog);
                            add_page_dialog = NULL;
                        }else {
                            addeditpageDialog* add_page_dialog = new addeditpageDialog(&sv->scanning_status);

                            if(add_page_dialog == NULL){
                                sv->scanning_status = ERROR;
                                sv->m_LastError = kSDIErrorUnknownError;
                                m_messageBox->ErrorMessageBox(sv);
                                break;
                            }
                            if(!edit_mode){
                                if(add_page_dialog->isHidden()){
                                    add_page_dialog->exec();
                                }else {
                                    add_page_dialog->activateWindow();
                                }
                            }else {
                                sv->scanning_status = EDIT;
                            }
                            if(sv->scanning_status == EDIT){
                                edit_mode = add_page_dialog->EditDialog(&SaveImagePathList::save_image_path_list);
                            }
                            delete(add_page_dialog);
                            add_page_dialog = NULL;
                        }
                        //事前紙なしエラーCheck
                        if(sv->device_data.FunctionalUnit.select == kSDIFunctionalUnitDocumentFeeder &&
                                sv->scanning_status == CONTINUE){
                            if(!sv->GetPaperLoadedStatus()){
                                //紙なしエラー
                                sv->m_LastError = kSDIErrorPaperEmpty;
                                m_messageBox->ErrorMessageBox(sv);
                            }else {
                                sv->m_LastError = kSDIErrorNone;
                                first_image_save = true;
                            }
                        }else {
                            sv->m_LastError = kSDIErrorNone;
                            first_image_save  = true;
                        }
                    }while(sv->m_LastError == kSDIErrorPaperEmpty);
                }
            }
        }
        //////////////////////////////////////////////////////////////////////////////////////////////

        if(sv->scanning_status == SAVE){
            //画像を保存する前にtempフォルダ内で回転させる
            RotateImage* rotateImage = new RotateImage((ColorType)sv->device_data.ColorType.select);
            auto save_path = SaveImagePathList::save_image_path_list.begin();
            for(int i = 0; i < (int)SaveImagePathList::save_image_path_list.size(); i++)
            {
                imageEditInfo temp_edit_info = (imageEditInfo)*save_path;
                int angle = temp_edit_info.angle % 360;
                if(angle != 0){
                    rotateImage->Rotate(temp_edit_info.file_path, angle, (ImageFormat)sv->device_data.ImageFormatSettings.ImageFormat.select, sv->device_data.ImageFormatSettings.MultiTiffEnabled.select, sv->device_data.ImageFormatSettings.OFDEnabled.select);
                }
                ++save_path;
            }


            ProgessCallBack progressCallBack = nullptr; 

            if (sv->device_data.ImageFormatSettings.ImageFormat.select == kSDIImageFormatPNM &&
                sv->device_data.ImageFormatSettings.OFDEnabled.select)
            {   
                std::string message = (TranslationString::GetString().translation_str.PROGRESS_003.toLocal8Bit()).constData();

                saveProgressDialog = new QProgressDialog(message.c_str(),NULL, 0, 0, m_mainWindow);
                saveProgressDialog->setWindowModality(Qt::WindowModal);
                saveProgressDialog->setWindowFlags(Qt::WindowTitleHint | Qt::Window | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint) ;
                saveProgressDialog->setCancelButton(NULL);
                saveProgressDialog->setModal(true);
                saveProgressDialog->open();

                progressCallBack = MyProgessCallBack;
                        
                QCoreApplication::processEvents();
                sleep(1);
                QCoreApplication::processEvents();
            }
  

            if(sv->Save_Image2(progressCallBack)){
                //保存先ディレクトリを表示するフラグがたっているか？
                Configuration& config = Configuration::GetConfiguration();
                if(config.config_para.open_img_folder){
                    SaveDestFolder* folder = new SaveDestFolder();
                    if(folder){
                        if(!folder->open_folder(sv)){
                            m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_SAVE_001, TranslationString::GetString().translation_str.VERSION_003, Critical);
                        }
                        delete folder;
                        folder = NULL;
                    }
                }else {
                    //保存先のディレクトリが存在しているか確認する
                    const bool result = ES_CMN_FUNCS::PATH::ES_IsExistFile(sv->device_data.SelectPath);
                    if (!result) {
                        m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_SAVE_001, TranslationString::GetString().translation_str.VERSION_003, Critical);
                    }
                }
            }else {
                if(sv->m_LastError == kSDIErrorWriteFile){
                    //書き込み失敗エラー
                    m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.E_SAVE_001, TranslationString::GetString().translation_str.VERSION_003, Critical);
                }else {
                    //すべて白紙ページエラー
                    if(sv->m_DoneBlanckPage && !sv->m_ExistsPage){
                        m_messageBox->CreateMessageBox(TranslationString::GetString().translation_str.A_SCAN_001, TranslationString::GetString().translation_str.VERSION_003, Warning);
                    }
                }
            }
            delete rotateImage;
            rotateImage = NULL;
        }

        if(sv->scanning_status == CANCEL){
            //キャンセルしたときも最終的にkSDITransferEventTypeCompleteを受け取ってから終了しなければならない
            while(sv->outEventType != kSDITransferEventTypeComplete){
                if(sv->CheckNextTransferEvent(false))
                {
                    sv->Scanning();
                    sv->Dispose_Ptr();
                }else{
                    sv->outEventType = kSDITransferEventTypeComplete;
                }
            }
            SaveImagePathList::save_image_path_list.clear();
        }

        if(sv->scanning_status == DISCARD){
            //最後にスキャンした画像を破棄してスキャンを続ける
            auto save_path = SaveImagePathList::save_image_path_list.end();
            save_path--;
            imageEditInfo temp_edit_info = (imageEditInfo)*save_path;
            std::string temp_str = temp_edit_info.file_path;
            if(temp_str.c_str()){
                SaveImagePathList::save_image_path_list.erase(save_path);
            }
            sv->scanning_status = CONTINUE;
        }
    }while(sv->scanning_status == CONTINUE); //スキャン継続フラグがたっている場合次のスキャンに移行する

    if (saveProgressDialog)
    {
        delete saveProgressDialog;
        saveProgressDialog = nullptr;
    }
}


