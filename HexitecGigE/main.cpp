#include <QApplication>
#include <QSplashScreen>
#include "mainwindow.h"

/// HexitecGigE Addition; Static linking?
//#include <QtPlugin>
//Q_IMPORT_PLUGIN(qt5gui)
//Q_IMPORT_PLUGIN(Qt5Widgets)
//Q_IMPORT_PLUGIN(Qt5ExtSerialPort1)
///

int main(int argc, char *argv[])
{
    qDebug() << "int main()";
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

