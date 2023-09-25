#ifndef FOLDERCOMB_H
#define FOLDERCOMB_H

#include <QComboBox>
#include <QStandardPaths>
#include "supervisor.h"
#include "custommessagebox.h"

class FolderComb : public QComboBox
{
public:
    FolderComb(QWidget *parent = 0);

    Folder current_item;

    void initialize(SCANPARA dev_data, CustomMessageBox* msg_box);

    void focusOutEvent(QFocusEvent* event);

    void update_ui();

    void item_event(int value, SCANPARA* path_data);

    void select_current_path(SCANPARA* path_data);

    QString user_define_path;

private:
    CustomMessageBox* m_messageBox;
    bool changed;

    QVariant old_select;

    void set_enabled(bool enabled);

    void add_item();

    void add_option(int list_size);

    void add_userdefine();

    void select_item();

    void item_event_options(void);
};

#endif // FOLDERCOMB_H
