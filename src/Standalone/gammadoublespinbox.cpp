#include "mainwindow.h"

GammaDoubleSpinBox::GammaDoubleSpinBox(QWidget *parent) :
    QDoubleSpinBox(parent)
{

}

void GammaDoubleSpinBox::setGamma(int value){

    this->setValue((double)value / 10);
}
