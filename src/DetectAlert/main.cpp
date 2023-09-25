#include "detectalert.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DetectAlert alert;

    if(argc != 6){
        return -1;
    }

    alert.CustomMessageBox(argv[1], argv[2], argv[3], argv[4], argv[5]);

    return 1;
}
