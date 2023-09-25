#ifndef CUSTOMMESSAGEBOX_H
#define CUSTOMMESSAGEBOX_H

#include <supervisor.h>
#include "QMessageBox"

typedef enum {Question=0, Information, Warning, Critical}IconType;


class CustomMessageBox
{
public:

    CustomMessageBox(QWidget* parent);

    bool CreateMessageBox(QString message, QString button_message, IconType type, bool use_doNotShow=false);
    bool CreateSelectableMessageBox(QString message, IconType type);

    void ErrorMessageBox(Supervisor* sv, bool Preview=false);

private:
    QWidget* parent_dialog;

    void PaperJamDialog(Supervisor* sv, QString message, DoubleFeedMode dialog_type=IMMEDIATE);
};

#endif // CUSTOMMESSAGEBOX_H
