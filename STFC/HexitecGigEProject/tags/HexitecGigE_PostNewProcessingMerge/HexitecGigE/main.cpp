#include <QApplication>
#include <QSplashScreen>
#include "mainwindow.h"


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

