#ifndef CONTRASTSLIDER_H
#define CONTRASTSLIDER_H

#include <QSlider>
#include "supervisor.h"

class ContrastSlider : public QSlider
{
public:
    ContrastSlider(QWidget *parent = 0);

    int current_item;

    void initialize(int default_item);

    void update_ui();

    void item_event(int value, INT_SETTING_VALUE* contrast_para);

private:
    bool changed;

    void set_enabled(bool enabled);

};

#endif // CONTRASTSLIDER_H
