#include "previewbutton.h"

#include <stdio.h>
#include <iostream>
#include <boost/format.hpp>
#include <QDesktopServices>

#include "mainwindow.h"
#include "scanningdialog.h"
#include "addpagedialog.h"
#include "saveimagepathlist.h"
#include "PathUtils.h"
#include "savedestfolder.h"

PreviewButton::PreviewButton(QWidget *parent) :
    QPushButton(parent)
{
    m_messageBox = NULL;
}

void PreviewButton::initialize(CustomMessageBox* msg_box)
{
    this->setEnabled(true);

    m_messageBox = msg_box;
}

void PreviewButton::Button_pressed(Supervisor* sv)
{
    ScanningDialog* scanning_dialog = NULL;
    sv->scanning_status = SAVE;


    //Initialize dialog///////////////////////////////////////////////////////////////////////////
    scanning_dialog = new ScanningDialog(sv);
    scanning_dialog->ScanInitialize(TranslationString::GetString().translation_str.PROGRESS_005);

    if(scanning_dialog == NULL){
        sv->scanning_status = ERROR;
        sv->m_LastError = kSDIErrorUnknownError;
        m_messageBox->ErrorMessageBox(sv, true);
        return;
    }
    scanning_dialog->setAttribute(Qt::WA_DeleteOnClose);
    if(sv->m_LastError != kSDIErrorNone){
        m_messageBox->ErrorMessageBox(sv, true);
        return;
    }else {
        if(scanning_dialog->isHidden()){
            scanning_dialog->exec();
        }else {
            scanning_dialog->activateWindow();
        }
    }

    //エラー処理//////////////////////////////////////////////////////////////////////////////////
        //スキャン中のキャンセル処理があるか確認
        if(sv->outEventType == kSDITransferEventTypeCancel){
            m_messageBox->ErrorMessageBox(sv, true);
            sv->scanning_status = CANCEL;
        }else
        //スキャン中のエラーをチェックしエラーダイアログを表示する
        if(sv->m_LastError != kSDIErrorNone){
            m_messageBox->ErrorMessageBox(sv, true);
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
}


