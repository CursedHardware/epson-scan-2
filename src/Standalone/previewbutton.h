#ifndef PREVIEWBUTTON_H
#define PREVIEWBUTTON_H

#include <QPushButton>
#include "capitem.h"
#include "supervisor.h"
#include "custommessagebox.h"

class PreviewButton : public QPushButton
{
public:
    PreviewButton(QWidget *parent = 0);

    void initialize(CustomMessageBox* msg_box);

    void Button_pressed(Supervisor* sv);

private:

    CustomMessageBox* m_messageBox;
};

#endif // PREVIEWBUTTON_H
