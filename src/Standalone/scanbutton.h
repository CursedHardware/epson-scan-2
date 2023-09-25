#ifndef SCANBUTTON_H
#define SCANBUTTON_H

#include <QPushButton>
#include "capitem.h"
#include "supervisor.h"
#include "custommessagebox.h"

class  QMainWindow;

class ScanButton : public QPushButton
{
public:
    ScanButton(QWidget *parent = 0);

    void initialize(CustomMessageBox* msg_box, QMainWindow* mainWindow = nullptr);

    void Button_pressed(Supervisor* sv, bool Add_Pages);

private:

    CustomMessageBox* m_messageBox;
    QMainWindow* m_mainWindow;
};


#endif // SCANBUTTON_H
