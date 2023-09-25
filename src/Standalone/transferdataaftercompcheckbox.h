#ifndef TRANSFERDATAAFTERCOMPCHECKBOX_H
#define TRANSFERDATAAFTERCOMPCHECKBOX_H

#include <QCheckBox>
#include "supervisor.h"

class TransferDataAfterCompCheckBox : public QCheckBox
{
public:
    TransferDataAfterCompCheckBox(QWidget *parent = 0);

    bool current_item;

    void initialize(bool default_item);

    void update_ui(void);

    bool item_event(bool value);

private:
    bool changed;

    void set_enabled(bool enabled);

    void select_item(bool select);

};

#endif // TRANSFERDATAAFTERCOMPCHECKBOX_H
