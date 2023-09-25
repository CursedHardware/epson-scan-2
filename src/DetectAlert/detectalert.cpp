#include "detectalert.h"
#include "translationstring.h"

#include <QDesktopWidget>
#include <QGridLayout>
#include <QSpacerItem>
#include <QDir>
#include <QDirIterator>

#include "../Include/Config.h"

DetectAlert::DetectAlert(QWidget *parent)
    : QMainWindow(parent)
{
}

DetectAlert::~DetectAlert()
{

}

static const int kMessageBoxWidth = 500;
static const int kMessageBoxHeight = 100;

void DetectAlert::CustomMessageBox(QString dialog_title, QString message, QString button_message, std::string identifier, std::string dir_num)
{
#ifdef AKBMODE
    std::string dir_name = "epsonscan2alert";
#else
    std::string dir_name = "epsonscan2alert";
#endif
    dir_name = dir_name + identifier;

    int num = std::atoi(dir_num.c_str());

    QDir tmpdir("/tmp");

        //IconType icon = Information;
        QMessageBox msgBox(this);

        QDesktopWidget desktop_info;

        msgBox.setIconPixmap(QPixmap(":/escan2_app.icon"));
        msgBox.setText(message);
        msgBox.setWindowTitle(dialog_title);
        msgBox.setStandardButtons(QMessageBox::Yes);
        msgBox.setDefaultButton(QMessageBox::Yes);


        //ボタンのテキスト変更
        msgBox.setButtonText(QMessageBox::Yes, button_message);

        // 20 is margin
        if(num == 1){
            msgBox.setGeometry(desktop_info.width() - kMessageBoxWidth - 20, desktop_info.height() - kMessageBoxHeight,  0, 0);
        }else if(num == 2){
            msgBox.setGeometry(desktop_info.width() - kMessageBoxWidth - 20, desktop_info.height() - kMessageBoxHeight - (kMessageBoxHeight * 1 + (1 * 50)),  0, 0);
        }


        QSpacerItem* horizontalSpacer = new QSpacerItem(kMessageBoxWidth  , kMessageBoxHeight, QSizePolicy::Minimum, QSizePolicy::Expanding);
        QGridLayout* layout = (QGridLayout*)msgBox.layout();
        layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1,layout->columnCount());

        msgBox.setIcon(QMessageBox::Information);
        /*switch (icon){
            case Question:
                msgBox.setIcon(QMessageBox::Question);
                break;
            case Information:
                msgBox.setIcon(QMessageBox::Information);
                break;
            case Warning:
                msgBox.setIcon(QMessageBox::Warning);
                break;
            case Critical:
                msgBox.setIcon(QMessageBox::Critical);
                break;
        }*/

        msgBox.exec();

        tmpdir.rmdir(dir_name.c_str());
}

int DetectAlert::CheckExistDir()
{
    QDir tmpdir("/tmp");

    QStringList nameFilters;
#ifdef AKBMODE
    std::string strDst = DRIVER_NAME;
    strDst = strDst + "alert*";
    nameFilters << strDst.c_str();
#else
    nameFilters << "epsonscan2alert*";
#endif

    QStringList dirlist = tmpdir.entryList(nameFilters, QDir::Dirs);
    return dirlist.count();
}
