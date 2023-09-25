#include "imageeditdialog.h"
#include "ui_imageeditdialog.h"

#include "mainwindow.h"

#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QFileDialog>
#include <QMessageBox>
//#include <math.h>
#include <QMatrix>
#include <QTransform>
#include <QMimeData>
#include <QAbstractScrollArea>
#include <QTimer>
#include <list>
#include <vector>
#include <iostream>

#include "PathUtils.h"

using namespace std;

ImageEditDialog::ImageEditDialog(std::list<imageEditInfo> imgpath_list, ScanningStatus* scanning_status, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImageEditDialog)
{
    ui->setupUi(this);

    ui->graphicsView->Initialize(this);
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    setFixedSize(1214,673);
    ui->graphicsView->setBackGround();
    grayOut(false);
    //saveButtonGrayOut(false);
    ui->graphicsView->integrateLoadFunction(imgpath_list, &m_returnInfoList);

    this->scanning_status = scanning_status;

    ui->removeButton->setEnabled(false);

    ui->saveButton->setFocus();

#ifdef NOUSEQRC
    std::string strRotateLeftIconPath = EPSON_INSTALL_PATH;
    ES_CMN_FUNCS::PATH::ES_CombinePath(strRotateLeftIconPath, strRotateLeftIconPath, "Resources/Icons/btn_multipage_rotate_left90@2x.png");
    ui->rotateLeft->setIcon(QIcon(strRotateLeftIconPath.c_str()));

    std::string strRotateRightIconPath = EPSON_INSTALL_PATH;
    ES_CMN_FUNCS::PATH::ES_CombinePath(strRotateRightIconPath, strRotateRightIconPath, "Resources/Icons/btn_multipage_rotate_right90@2x.png");
    ui->rotateRight->setIcon(QIcon(strRotateRightIconPath.c_str()));

    std::string strRemoveIconPath = EPSON_INSTALL_PATH;
    ES_CMN_FUNCS::PATH::ES_CombinePath(strRemoveIconPath, strRemoveIconPath, "Resources/Icons/btn_multipage_delete@2x.png");
    ui->removeButton->setIcon(QIcon(strRemoveIconPath.c_str()));
#endif
}

void ImageEditDialog::closeEvent(QCloseEvent* event){
    if(*this->scanning_status != CONTINUE){
        if(this->CancelDialog()){
            *this->scanning_status = CANCEL;
            this->reject();
        }else {
            event->ignore();
        }
    }
}

ImageEditDialog::~ImageEditDialog()
{
    delete ui;
}

void ImageEditDialog::onButtonPressed()
{
    QObject* obj = sender();

    if(obj == ui->addButton){
        ui->graphicsView->pdfDialog();
        *this->scanning_status = CONTINUE;
        m_returnStatus = ImageEditStatusIsSave;
    }else if(obj == ui->cancelButton){
        if(this->CancelDialog()){
            *this->scanning_status = CANCEL;
            this->reject();
        }
    }else if(obj == ui->saveButton){
        *this->scanning_status = SAVE;
    }
}

bool ImageEditDialog::CancelDialog(void)
{
    QMessageBox msgBox(this);
    std::string message = (TranslationString::GetString().translation_str.C_UI_102.toLocal8Bit()).constData();
    msgBox.setText(message.c_str());
    msgBox.setWindowTitle(TranslationString::GetString().translation_str.MENU_001);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);

    //ボタンのテキスト変更
    msgBox.setButtonText(QMessageBox::Yes, TranslationString::GetString().translation_str.ES2U_MAIN_052);
    msgBox.setButtonText(QMessageBox::No, TranslationString::GetString().translation_str.ES2U_MAIN_053);

    msgBox.setIcon(QMessageBox::Warning);

    int res = msgBox.exec();

    if (res == QMessageBox::Yes){
        return true;
    } else if (res == QMessageBox::No) {
        return false;
    }else {
        return false;
    }
}

int ImageEditDialog::showRemoveMessage(){
    QMessageBox msgBox(this);
    msgBox.setText(TranslationString::GetString().translation_str.C_UI_101);
    msgBox.setWindowTitle(TranslationString::GetString().translation_str.MENU_001);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);

    //ボタンのテキスト変更
    msgBox.setButtonText(QMessageBox::Yes, TranslationString::GetString().translation_str.ES2U_MAIN_052);
    msgBox.setButtonText(QMessageBox::No, TranslationString::GetString().translation_str.ES2U_MAIN_053);

    int res = msgBox.exec();
    return res;
}


void ImageEditDialog::on_rotateLeft_clicked()
{
    ui->graphicsView->rotateLeft();
}


void ImageEditDialog::on_rotateRight_clicked()
{
    ui->graphicsView->rotateRight();
}

void ImageEditDialog::on_removeButton_clicked()
{
    if(showRemoveMessage() == QMessageBox::Yes)
    {
        ui->graphicsView->removeItem();
        if(ui->graphicsView->getItemCount() <= 0){
            saveButtonGrayOut(false);
        }
        else {
            saveButtonGrayOut(true);
        }

        if(ui->graphicsView->imageIsSelected() == false)
        {
            grayOut(false);
        }
        else
        {
            grayOut(true);
        }
        ui->removeButton->setEnabled(false);
    }
}


void ImageEditDialog::on_saveButton_clicked()
{
    ui->graphicsView->pdfDialog();
    *this->scanning_status = CONTINUE;
    this->close();
    *this->scanning_status = SAVE;
    m_returnStatus = ImageEditStatusIsSave;
}

void ImageEditDialog::grayOut(bool existImage)
{
    ui->rotateLeft->setEnabled(existImage);
    ui->rotateRight->setEnabled(existImage);
    if(ui->graphicsView->getItemCount() > 1){
        ui->removeButton->setEnabled(existImage);
    }
}

void ImageEditDialog::saveButtonGrayOut(bool existOneImage)
{
    ui->saveButton->setEnabled(existOneImage);
}

void ImageEditDialog::getInfoList(std::list<imageEditInfo> *ListA){
    //qDebug("getInfoList string address:%p", ListA);
    std::list<imageEditInfo> inf;
    inf = m_returnInfoList;

    if(inf.size() != 0){
        for (int i = 0;i < ui->graphicsView->getItemCount();i++)
        {
            ListA->push_back(inf.front());
            inf.pop_front();
        }
    }
}

void ImageEditDialog::reject(){
    m_returnStatus = ImageEditStatusIsReturn;
    QDialog::reject();
}
