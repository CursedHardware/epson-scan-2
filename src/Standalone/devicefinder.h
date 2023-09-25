#ifndef DEVICEFINDER_H
#define DEVICEFINDER_H

#include "supervisor.h"
#ifndef NO_QTMODE
#include "waitingdialog.h"
#endif

class DeviceFinder
{
public:
    DeviceFinder();

    void finder(QWidget *parent = 0);
    bool ManuNetfinder();

private:
    WaitingDialog* wait_dialog;
};

#endif // DEVICEFINDER_H
