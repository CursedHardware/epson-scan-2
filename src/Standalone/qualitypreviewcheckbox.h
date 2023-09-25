#ifndef QUALITYPREVIEWCHECKBOX_H
#define QUALITYPREVIEWCHECKBOX_H

#include <QCheckBox>
#include "capitem.h"

class QualityPreviewCheckbox : public QCheckBox
{
public:
public:
    QualityPreviewCheckbox(QWidget *parent = 0);

    bool current_item;

    void initialize(bool default_item);

    void update_ui(void);

    bool item_event(bool value);

private:
    bool changed;

    bool enabled;

    void set_enabled(bool enabled);

    void select_item(bool select);

};

#endif // QUALITYPREVIEWCHECKBOX_H
