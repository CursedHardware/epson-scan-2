#ifndef SKIPBLANKPAGESDETLVSLIDER_H
#define SKIPBLANKPAGESDETLVSLIDER_H

#include <QSlider>
#include "capitem.h"

class SkipBlankPagesDetLvSlider : public QSlider
{
public:
    SkipBlankPagesDetLvSlider(QWidget *parent = 0);

    int current_value;

    void initialize(int default_item);

    void update_ui(void);

    void item_event(int value);

private:
    bool changed;
    bool enabled;

    void set_enabled(bool enabled);

};

#endif // SKIPBLANKPAGESDETLVSLIDER_H
