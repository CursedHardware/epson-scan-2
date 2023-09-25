#ifndef DETECTPAPERCHECKBOX_H
#define DETECTPAPERCHECKBOX_H

#include <QCheckBox>
#include "capitem.h"

class DetectPaperCheckBox : public QCheckBox
{
public:
    DetectPaperCheckBox(QWidget *parent = 0);

    bool current_item;

    void initialize(bool default_item);

    void update_ui(void);

    void item_event(bool value);

private:
    bool changed;

    bool enabled;

    void set_enabled(bool enabled);

    void select_item(bool select);
};

#endif // DETECTPAPERCHECKBOX_H
