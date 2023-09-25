#include "mainwindow.h"
#include "devicelist.h"

InitDeviceSelectCombobox::InitDeviceSelectCombobox(QWidget *parent) :
    QComboBox(parent),
    changed(false),
    networkdeviceselect_dialog(NULL)
{
}

bool InitDeviceSelectCombobox::initialize()
{
    //Initialize dialog
    if(!networkdeviceselect_dialog){
        networkdeviceselect_dialog = new NetworkDeviceSelectDialog;
        if(networkdeviceselect_dialog == NULL){
            return false;
        }
    }
    changed = true;
    DeviceList::select_device = 0;
    return true;
}

void InitDeviceSelectCombobox::update_device_list(){
    if(changed == true){
        this->clear();
        this->add_device();
        this->select_item();
        changed = false;
    }
}

void InitDeviceSelectCombobox::add_device(){
    int list_size = DeviceList::device_list.size();

    int i = 0;
    const char* connection;
    auto device = DeviceList::device_list.begin();

    while(i < list_size){
        auto qString = TranslationString::GetString().translation_str.SCANNERSETTING_005.toLocal8Bit();

        if(device->ipAddress[0] == '\0'){
            connection = qString.constData();
        }else {
            connection = device->ipAddress;//"(Network)";
        }
        this->CreateDisplayName(device->displayName, connection, i, i);
        ++device;
        i++;
    }
    this->add_manu_ip();
    this->add_option();
}

void InitDeviceSelectCombobox::add_option()
{
    this->insertSeparator(this->count());
    this->insertItem(this->count(), TranslationString::GetString().translation_str.SCANNERSETTING_006, OPTIONS);
}


void InitDeviceSelectCombobox::add_manu_ip()
{
    if(DeviceList::manu_network_device_list.size() > 0){
        //セパレーターがコンボボックスのサイズに含まれてしまうためindexを別に用意する
        int list_size = DeviceList::manu_network_device_list.size() + DeviceList::device_list.size();
        auto device = DeviceList::manu_network_device_list.begin();
        this->insertSeparator(this->count());
        int i = this->count();
        int dev_index = DeviceList::device_list.size();
        while(i <= list_size){
            this->CreateDisplayName("Network Scanner", device->ipAddress, i, dev_index); //TBD
            ++device;
            i++;
            dev_index++;
        }
    }
}

void InitDeviceSelectCombobox::CreateDisplayName(char displayName[MAX_DISPLAYNAME], const char* connection, int num, int index)
{
    char *displayDeviceName = NULL;
    char temp_displayName[MAX_DISPLAYNAME];

    memcpy_s(temp_displayName, MAX_DISPLAYNAME, displayName, strlen(displayName)+1);
    std::string connection_display = connection;
    if(strncmp(connection, "USB", 3) == 0){
        if(strtok(temp_displayName, ":")){
            std::string bus_no = strtok(NULL, ":");
            std::string device_no = strtok(NULL, ":");
            connection_display = connection_display + ":" + bus_no + ":" + device_no;
        }
    }
#ifdef AKBMODE
    int connection_str_size = strlen(temp_displayName) + strlen(" (") + strlen(connection_display.c_str()) + strlen(")") + 1;
#else
    int connection_str_size = strlen("EPSON ") + strlen(temp_displayName) + strlen(" (") + strlen(connection_display.c_str()) + strlen(")") + 1;
#endif
    if(!displayDeviceName){
        displayDeviceName = (char *)malloc(connection_str_size);
        if(displayDeviceName){
           memset(displayDeviceName, 0, connection_str_size);

           //表示する名前を作成する
#ifdef AKBMODE
           sprintf(displayDeviceName, "%s (%s)", temp_displayName, connection_display.c_str());
#else
           sprintf(displayDeviceName, "EPSON %s (%s)", temp_displayName, connection_display.c_str());
#endif
           this->insertItem(num, displayDeviceName, index);

           free(displayDeviceName);
           displayDeviceName = NULL;
        }
    }
}

void InitDeviceSelectCombobox::item_event(int value)
{
    int list_size = this->count();
    QVariant select_device = this->itemData(value);

    int i = 0;
    while(i < list_size){
        if(select_device == i){
            DeviceList::select_device = i;
            break;
        }
        i++;
    }
    if(select_device == OPTIONS){
        this->item_event_options();
    }
    changed = true;
}

void InitDeviceSelectCombobox::item_event_options(void){

    //手動で追加したIPアドレスがtableWidgetにない場合は追加する
    if(DeviceList::manu_network_device_list.size() != 0 && networkdeviceselect_dialog->Get_IP_ListTable_Size() == 0){
        networkdeviceselect_dialog->InsertDevice();
    }

    if(networkdeviceselect_dialog->isHidden()){
        networkdeviceselect_dialog->exec();
    }else {
        networkdeviceselect_dialog->activateWindow();
    }
    if(!networkdeviceselect_dialog->UpdateNetworkSettingFile()){
        //設定ファイルが不正ですエラーダイアログ表示
    }

    if(DeviceList::select_manu_ip_address >= 0){
        if(DeviceList::device_list.size() == 0){
            DeviceList::select_device = (DeviceList::manu_network_device_list.size() - 1) - DeviceList::select_manu_ip_address;
        }else {
            DeviceList::select_device = ((DeviceList::device_list.size() + DeviceList::manu_network_device_list.size()) - 1) - DeviceList::select_manu_ip_address;
        }
    }else {
        DeviceList::select_device = 0;
    }
}

void InitDeviceSelectCombobox::select_item()
{
    // デバイスが一つも見つからなかったとき
    if(this->findData(OPTIONS) == 1){
        this->setCurrentIndex(this->findData(OPTIONS));
    }else {
        this->setCurrentIndex(this->findData(DeviceList::select_device));
    }
}
