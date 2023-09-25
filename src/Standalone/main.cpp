#ifndef NO_QTMODE
#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#endif

#include "commandlinemode.h"
#include "../Include/Config.h"
#include "PathUtils.h"

void TranslateString(void){
#ifndef NO_QTMODE
    TranslationString& translation = TranslationString::GetString();
    translation.SetString();
#endif
}

int main(int argc, char *argv[])
{
    std::string strDst = EPSON_INSTALL_PATH;
#ifndef NO_QTMODE
#ifdef AKBMODE
    ES_CMN_FUNCS::PATH::ES_CombinePath(strDst, strDst, "Resources/locale/");
    ES_CMN_FUNCS::PATH::ES_CombinePath(strDst, strDst, DRIVER_NAME);
    strDst = strDst + "_";
#else
    ES_CMN_FUNCS::PATH::ES_CombinePath(strDst, strDst, "Resources/locale/");
    ES_CMN_FUNCS::PATH::ES_CombinePath(strDst, strDst, "epsonscan2_");
#endif
#endif

    if(argc > 1){
        //コマンドライン起動によるオプション解釈
        CommandLineMode* cmd_mode = new CommandLineMode();
        if(cmd_mode == NULL){
            return 0;
        }
        if(!cmd_mode->SelectMode(argc, (const char**)argv)){
#ifndef NO_QTMODE
            //--editモード解釈
            if(cmd_mode->SelectEditMode(argc, (const char**)argv)){
                //Edit ModeでStandaloneを起動する
                QApplication a(argc, argv);

                //ローカライズ対応
                QTranslator myappTranslator;
                //動作環境言語の翻訳ファイルの読み込み
                if(!myappTranslator.load(strDst.c_str() + QLocale::system().name())){
                    //言語ファイルが見つからない場合はデフォルトの英語表示になる
                    myappTranslator.load((strDst + "en").c_str());
                }
                a.installTranslator(&myappTranslator);

                TranslateString();

                MainWindow w(argv[2]);
                w.show();

                return a.exec();
            }
#endif
        }

        int device_status = cmd_mode->IsGetStatus();

        delete cmd_mode;
        cmd_mode = NULL;

        return device_status;
    }else {
#ifndef NO_QTMODE
        QApplication a(argc, argv);

        //ローカライズ対応
        QTranslator myappTranslator;
        //動作環境言語の翻訳ファイルの読み込み
        if(!myappTranslator.load(strDst.c_str() + QLocale::system().name())){
            //言語ファイルが見つからない場合はデフォルトの英語表示になる
            myappTranslator.load((strDst + "en").c_str());
        }
        //myappTranslator.load((strDst + "fr_CA").c_str());
        a.installTranslator(&myappTranslator);

        TranslateString();

        MainWindow w("");
        w.show();

        return a.exec();
#else
        std::cout << "ERROR : Please specify options and setting(SF2) files." << std::endl;
#endif
    }
}

