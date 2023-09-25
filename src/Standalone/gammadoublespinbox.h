#ifndef GAMMADOUBLESPINBOX_H
#define GAMMADOUBLESPINBOX_H

#include <QDoubleSpinBox>

class GammaDoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT

public:

    GammaDoubleSpinBox(QWidget *parent = 0);

public slots:
    void setGamma(int value);

};

#endif // GAMMADOUBLESPINBOX_H
