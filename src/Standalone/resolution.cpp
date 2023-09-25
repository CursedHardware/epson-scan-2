#include "mainwindow.h"

Resolution::Resolution(QWidget *parent) :
    QComboBox(parent),
    changed(false),
    current_item(res_100),
    current_value(100),
    validator(NULL)
{
}

void Resolution::focusOutEvent(QFocusEvent* event){
    event->accept();
    this->validate_input();

    if(ValueToId(this->currentText().toInt()) != other){
        current_item = (ResolutionList)this->currentIndex();
    }else {
        current_item = other;
    }
    current_value = this->currentText().toInt();
}

void Resolution::initialize(INT_SETTING_VALUE resolution_para){

    validator = new QIntValidator(50, resolution_para.capability.allMaxValue, this);
    this->setValidator(validator);

    changed = true;
    current_value = resolution_para.select;
    current_item = (ResolutionList)ValueToId(current_value);
}

void Resolution::update_ui(INT_SETTING_VALUE resolution_para, bool isLFPModel){

    if(changed == true){
        this->clear();
        if(resolution_para.capability.supportLevel == kSDISupportLevelAvailable){
            this->set_enabled(true);
            this->create_list(resolution_para, isLFPModel);
            this->select_item();

            //手入力された解像度を表示させる
            if(current_item == other){
                QLineEdit *displayedText = this->lineEdit();
                displayedText->setText(QString(QString::number(current_value)));
            }
        }else {
            this->set_enabled(false);
        }
        changed = false;
    }
}

void Resolution::set_enabled(bool enabled){

    if(enabled == true){
        this->setEnabled(true);
    }else {
        this->setEnabled(false);
    }
}

void Resolution::create_list(INT_SETTING_VALUE resolution_para, bool isLFPModel){

    int i = 0;
    //LFPデバイスの場合は表示させる解像度を制限する
    if(!isLFPModel){
        if(resolution_para.capability.maxValue >= 75){
            this->insertItem(i, "75", res_75);
            i++;
        }
        if(resolution_para.capability.maxValue >= 100){
            this->insertItem(i, "100", res_100);
            i++;
        }
        if(resolution_para.capability.maxValue >= 150){
            this->insertItem(i, "150", res_150);
            i++;
        }
    }
    if(resolution_para.capability.maxValue >= 200){
        this->insertItem(i, "200", res_200);
        i++;
    }
    if(!isLFPModel){
        if(resolution_para.capability.maxValue >= 240){
            this->insertItem(i, "240", res_240);
            i++;
        }
    }
    if(resolution_para.capability.maxValue >= 300){
        this->insertItem(i, "300", res_300);
        i++;
    }
    if(resolution_para.capability.maxValue >= 400){
        this->insertItem(i, "400", res_400);
        i++;
    }
    if(resolution_para.capability.maxValue >= 600){
        this->insertItem(i, "600", res_600);
        i++;
    }
    if(!isLFPModel){
        if(resolution_para.capability.maxValue >= 1200){
            this->insertItem(i, "1200", res_1200);
            i++;
        }
    }
}

void Resolution::select_item()
{
    int item = this->findData(current_item);
    if(item != -1){
        this->setCurrentIndex(item);
    }
}

void Resolution::validate_input(){
    QString cur_text = this->currentText();
    int position = 0;

    QValidator::State state = validator->validate(cur_text, position);

    switch(state) {
    case QValidator::Acceptable:
        break;
    case QValidator::Intermediate:
        if(this->currentText().isEmpty()){
            this->setCurrentText("200"); //default
        }else {
            this->setCurrentText("50"); //minimum
        }
        break;
    case QValidator::Invalid:
        break;
    }
}

void Resolution::item_event(int value, INT_SETTING_VALUE* resolution_para)
{
    current_item = (ResolutionList)value;
    current_value = this->IdToValue(current_item);
    if(current_value == 0){
        current_value = std::atoi((this->currentText().toLocal8Bit()).constData());
    }
    resolution_para->select = current_value;
    changed = true;
}

int Resolution::IdToValue(ResolutionList item){

    int res;

    if(item == res_75){
        res = 75;
    }else if(item == res_100){
        res = 100;
    }else if(item == res_150){
        res = 150;
    }else if(item == res_200){
        res = 200;
    }else if(item == res_240){
        res = 240;
    }else if(item == res_300){
        res = 300;
    }else if(item == res_400){
        res = 400;
    }else if(item == res_600){
        res = 600;
    }else if(item == res_1200){
        res = 1200;
    }else {
        res = 0;
    }

    return res;
}

ResolutionList Resolution::ValueToId(int res){

    ResolutionList item;

    if(res == 75){
        item = res_75;
    }else if(res == 100){
        item = res_100;
    }else if(res == 150){
        item = res_150;
    }else if(res == 200){
        item = res_200;
    }else if(res == 240){
        item = res_240;
    }else if(res == 300){
        item = res_300;
    }else if(res == 400){
        item = res_400;
    }else if(res == 600){
        item = res_600;
    }else if(res == 1200){
        item = res_1200;
    }else {
        item = other;
    }

    return item;
}
