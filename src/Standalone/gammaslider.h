#ifndef GAMMASLIDER_H
#define GAMMASLIDER_H

#include <QSlider>
#include "supervisor.h"

class GammaSlider : public QSlider
{
    Q_OBJECT

public:
    GammaSlider(QWidget *parent = 0);

    double current_item;

    void initialize(double default_item);

    void update_ui();

    void item_event(double value, INT_SETTING_VALUE* gamma_para);

private slots:
    void setGammaSlider(double value);

private:
    bool changed;

    void set_enabled(bool enabled);

};

#endif // GAMMASLIDER_H
