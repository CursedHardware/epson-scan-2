#include "deviceselectdialog.h"
#include "ui_deviceselectdialog.h"
#include "devicefinder.h"
#include "PathUtils.h"
#include "devicelist.h"

#include <QIcon>

DeviceSelectDialog::DeviceSelectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceSelectDialog),
    sv(nullptr)
{
    ui->setupUi(this);
    this->setFixedSize(this->size());
    terminate_flag = false;

#ifdef AKBMODE
    this->setWindowTitle(DRIVER_TITLE);
#endif

#ifdef NOUSEQRC
    std::string strIconPath = EPSON_INSTALL_PATH;
    ES_CMN_FUNCS::PATH::ES_CombinePath(strIconPath, strIconPath, "Resources/Icons/update4.png");
    ui->Update_toolButton->setIcon(QIcon(strIconPath.c_str()));
#endif
#ifdef RPMBUILD
    ui->Update_toolButton->setIcon(this->style()->standardIcon(QStyle::SP_BrowserReload));
#endif
}

DeviceSelectDialog::~DeviceSelectDialog()
{
    //delete ui;
}

void DeviceSelectDialog::closeEvent(QCloseEvent* event){
    //このダイアログで×ボタンを押下された場合アプリを終了する
    //event->ignore();
    this->done(QDialog::Rejected);
    terminate_flag = true;
}

bool DeviceSelectDialog::Initialize(Supervisor* sv)
{
    this->sv = sv;
    if(!ui->Select_Device_comboBox->initialize()){
        return false;
    }

    this->updateUI();

    return true;
}

void DeviceSelectDialog::updateUI()
{
    ui->Select_Device_comboBox->update_device_list();

    //デバイスリストに１つもデバイスがないときは接続ボタンをグレーアウトする
    int device_sum = DeviceList::manu_network_device_list.size() + DeviceList::device_list.size();
    if(device_sum == 0){
        ui->Connecting_pushButton->setEnabled(false);
    }else {
        ui->Connecting_pushButton->setEnabled(true);
    }
}

void DeviceSelectDialog::onValueChanged(int value)
{
    QObject* obj = sender();

    if(obj == ui->Select_Device_comboBox){
        ui->Select_Device_comboBox->item_event(value);
    }
    this->updateUI();
}

void DeviceSelectDialog::PushEventButton(void)
{
    QObject* obj = sender();

    //Add...が選択中はConnectingボタンを無効化
    if(obj == ui->Connecting_pushButton){
        if(ui->Select_Device_comboBox->itemData(ui->Select_Device_comboBox->currentIndex()) != OPTIONS){
            this->reject();
        }
    }else if(obj == ui->Update_toolButton){
        DeviceFinder* dev_finder = new DeviceFinder();
        if(dev_finder == NULL){
            return;
        }

        dev_finder->finder(this);
        if(!dev_finder->ManuNetfinder()){
            //TBD
        }

        delete dev_finder;
        dev_finder = nullptr;

        this->Initialize(this->sv);
    }
    this->updateUI();
}
