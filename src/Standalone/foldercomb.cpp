#include "mainwindow.h"

FolderComb::FolderComb(QWidget *parent) :
    QComboBox(parent),
    m_messageBox(NULL),
    changed(false),
    current_item(Document)
{
}

void FolderComb::initialize(SCANPARA dev_data, CustomMessageBox* msg_box){
    changed = true;
    current_item = (Folder)dev_data.Folder;
    user_define_path = dev_data.UserDefinePath.c_str();
    if(current_item == USER_DEFINE){
        QString qStr = user_define_path;
        this->setToolTip(qStr);
    }
    old_select = current_item;

    m_messageBox = msg_box;
}

void FolderComb::focusOutEvent(QFocusEvent* event){
    event->accept();
}

void FolderComb::update_ui(){

    if(changed == true){
        this->clear();
        this->set_enabled(true);
        this->add_item();
        this->select_item();
        changed = false;
    }
}

void FolderComb::set_enabled(bool enabled){

    if(enabled == true){
        this->setEnabled(true);
    }else {
        this->setEnabled(false);
    }
}

void FolderComb::add_item()
{
    int i = 0;
    this->insertItem(i, TranslationString::GetString().translation_str.LINUX_FILESAVESETTING_008, Document);
    i++;
    this->insertItem(i, TranslationString::GetString().translation_str.LINUX_FILESAVESETTING_007, Picture);
    i++;
    this->insertItem(i, TranslationString::GetString().translation_str.LINUX_FILESAVESETTING_006, Desktop);
    i++;
    this->add_option(i);
}

void FolderComb::add_option(int list_size)
{
    this->insertItem(list_size, TranslationString::GetString().translation_str.FILESAVESETTING_016, OPTIONS);
    this->insertSeparator(list_size);

    this->add_userdefine();
}

void FolderComb::add_userdefine()
{
    if(!user_define_path.isEmpty()){
        this->setCurrentIndex(0);
        int temp = this->findData(USER_DEFINE);
        if(temp >= 0){
            this->removeItem(this->findData(USER_DEFINE)-1);
            this->removeItem(this->findData(USER_DEFINE));
            this->insertItem(this->findData(OPTIONS), user_define_path, USER_DEFINE);
            this->insertSeparator(this->findData(OPTIONS));
        }else {
            this->insertItem(this->count()-1, user_define_path, USER_DEFINE);
            this->insertSeparator(this->count()-1);
        }
    }
}

void FolderComb::select_item()
{
    this->setCurrentIndex(this->findData(current_item));
}

void FolderComb::item_event(int value, SCANPARA* path_data)
{
    QString qStr;
    QVariant select_item = this->itemData(value);

    if(select_item == Document){
        current_item = Document;
        qStr = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        this->setToolTip("");
    }else if(select_item == Picture){
        current_item = Picture;
        qStr = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
        this->setToolTip("");
    }else if(select_item == Desktop){
        current_item = Desktop;
        qStr = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
        this->setToolTip("");
    }else if(select_item == USER_DEFINE){
        current_item = (Folder)USER_DEFINE;
        qStr = user_define_path;
        path_data->UserDefinePath = (user_define_path.toLocal8Bit()).constData();
        this->setToolTip(qStr);
    }else if(select_item == OPTIONS){
        this->item_event_options();
        if(current_item == USER_DEFINE){
            qStr = user_define_path;
            path_data->UserDefinePath = (user_define_path.toLocal8Bit()).constData();
            select_item = old_select;
            this->setToolTip(qStr);
        }else {
            if(old_select == Document){
                qStr = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
            }else if(old_select == Picture){
                qStr = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
            }else if(old_select == Desktop){
                qStr = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
            }
            select_item = old_select;
            this->setToolTip("");
        }
    }
    path_data->SelectPath = (std::string)qStr.toLocal8Bit();
    old_select = select_item;
    changed = true;
}

#include "PathUtils.h"

void FolderComb::item_event_options(void){

    QFileDialog::Options options = QFileDialog::DontResolveSymlinks | QFileDialog::ShowDirsOnly;
    QString strDir = QFileDialog::getExistingDirectory(this, TranslationString::GetString().translation_str.FILESAVESETTING_012, "Dir", options);
    if(!strDir.isEmpty()){
        std::string full_path = (strDir.toLocal8Bit()).constData();
        std::string dirname;
        ES_CMN_FUNCS::PATH::ES_GetFileName(dirname, (strDir.toLocal8Bit()).constData(), FALSE);

        //ディレクトリ名を含めたフルパスが1023バイトまで
        //ディレクトリ名が255バイトまで許容する
        if(full_path.length() > 1023 || dirname.length() > 255){
            m_messageBox->CreateMessageBox("ERROR : Selection failed because the directory name is too long.", TranslationString::GetString().translation_str.VERSION_003, Critical);
            return;
        }
        user_define_path = strDir;
        current_item = (Folder)USER_DEFINE;
        old_select = (Folder)USER_DEFINE;
    }else {
        this->setCurrentIndex(this->findData(old_select));
        current_item = (Folder)old_select.toInt();
    }
}

void FolderComb::select_current_path(SCANPARA* path_data)
{
    if(path_data->Folder == Document){
        path_data->SelectPath = (QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation).toLocal8Bit()).constData();
    }else if(path_data->Folder == Picture){
        path_data->SelectPath = (QStandardPaths::writableLocation(QStandardPaths::PicturesLocation).toLocal8Bit()).constData();
    }else if(path_data->Folder == Desktop){
        path_data->SelectPath = (QStandardPaths::writableLocation(QStandardPaths::DesktopLocation).toLocal8Bit()).constData();
    }else if(path_data->Folder == USER_DEFINE){
        path_data->SelectPath = path_data->UserDefinePath;
    }
}
