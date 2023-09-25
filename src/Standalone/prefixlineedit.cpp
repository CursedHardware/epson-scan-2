#include "mainwindow.h"

PrefixLineedit::PrefixLineedit(QWidget *parent) :
    QLineEdit(parent),
    changed(false),
    current_prefix("")
{
}

void PrefixLineedit::initialize(std::string default_prefix){
    changed = true;
    current_prefix = default_prefix.c_str();
    this->setEnabled(true);
    this->setText(current_prefix);
}

QString PrefixLineedit::set_prefix(QString prefix){

    //入力制限文字チェック
    QRegExp r1("^.*[/:.*?\"<>|].*");
    if(r1.exactMatch(prefix) || prefix.indexOf("\\") != -1){
        BanMessageBox();
        current_prefix = DEFAULT_PREFIX;
        this->setText(DEFAULT_PREFIX);
    }else if(prefix.isEmpty()){
        current_prefix = DEFAULT_PREFIX;
    }else {
        current_prefix = prefix;
    }
    return current_prefix;
}

void PrefixLineedit::get_prefix(std::string* prefix){
    *prefix = (std::string)current_prefix.toLocal8Bit();
}

void PrefixLineedit::BanMessageBox(void)
{
    QMessageBox msgBox(this);
    msgBox.setText(TranslationString::GetString().translation_str.E_UI_103);
    msgBox.setWindowTitle(TranslationString::GetString().translation_str.MENU_001);
    msgBox.setStandardButtons(QMessageBox::Yes);
    msgBox.setDefaultButton(QMessageBox::Yes);

    //ボタンのテキスト変更
    msgBox.setButtonText(QMessageBox::Yes, TranslationString::GetString().translation_str.VERSION_003);

    msgBox.setIcon(QMessageBox::Warning);

    msgBox.exec();
}
