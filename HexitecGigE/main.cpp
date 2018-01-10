#include <QApplication>
#include <QSplashScreen>
#include "mainwindow.h"

// Necessary includes and defines for memory leak detection:
//#ifdef _MSC_VER
//#include <vld.h>
//#endif // _MSC_VER

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    //Q_INIT_RESOURCE(2Easy);
    Q_INIT_RESOURCE(HexitecGigE);

    QSplashScreen *splash = new QSplashScreen;
    splash->setPixmap(QPixmap(":/images/Hexitec_Logo50.png"));
    splash->show();

    MainWindow mainWin;
    mainWin.show();
    splash->finish(&mainWin);
    return app.exec();
}

