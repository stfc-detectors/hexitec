/* Following necessary as a workaround for qdatetime errors at Qt 5.0.2.
 * See for instance http://qt-project.org/forums/viewthread/22133 */
#define NOMINMAX

#include "chargesharing.h"
#include "ui_chargesharing.h"
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include "windows.h"
#include "slice.h"

ChargeSharing::ChargeSharing(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::ChargeSharing)
{
    ui->setupUi(this);
    ui->startValue->setValue(0);
    ui->endValue->setValue(4000);
    ui->noOfBins->setValue(400);
    ui->threshold->setValue(99);
    ui->chargeShare->setChecked(0);
    ui->segment->setChecked(1);
    ui->openAfter->setChecked(0);
    model = new QDirModel(this);
    ui->treeView->setModel(model);
    ui->treeView->setExpandsOnDoubleClick(false);
    ui->treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

ChargeSharing::~ChargeSharing()
{
    delete ui;
}

void ChargeSharing::externalCall()
{

}

void ChargeSharing::on_buttonBox_accepted()
{
    QStringList fileNames;
    fileNames = QFileDialog::getOpenFileNames(this,
                                              tr("Open Image"), "", tr("Data Files (*.hxt *.ezd *.dat *.xy)"));
    if (fileNames.isEmpty())
        return;

    QString fileName;
    QString preamble = "Hexitech.exe";
    preamble += " -s " + QString::number(ui->startValue->value());
    preamble += " -e " + QString::number(ui->endValue->value()) ;
    preamble += " -n " + QString::number(ui->noOfBins->value()) ;
    preamble += " -gt " + QString::number(ui->threshold->value()) ;

    //    preamble = "Hexitech.exe -s 0 -e 4000 -n 400 -gt 99 -o ";

    QFileInfo fi(fileNames[0]);
    QStringList fileNameParts = fi.baseName().split("reduced_");
    if (fileNameParts.size() != 2)
    {
        writeMessage("error in HEXITEC dat filename");
        return;
    }

    QStringList fileGroup;
    fileGroup.push_back(fileNames[0]);

    QString csSettings = "_" + QString::number(ui->startValue->value()) + "_" + QString::number(ui->endValue->value()) + "_" + QString::number(ui->noOfBins->value());

    if (ui->chargeShare->isChecked())
    {
        preamble += " -o ";
        int k = 0;
        int kk = 0;
        int i;
        for (i = 0; i < fileNames.size()-1; ++i)
        {
            QStringList fileNameParts0 = fileNames[i].split("reduced_");
            QStringList fileNameParts1 = fileNames[i+1].split("reduced_");
            if (fileNameParts1[0] == fileNameParts0[0])
                continue;
            else
            {
                kk = i + 1;
                // do Hexitec bit
                QString hxtWorkingDirectory = "";
                QStringList fileGroupParts = fileNames[k].split("reduced_");
                QString outputFileName = "\"" + fileGroupParts[0] + "CS" + csSettings + ".hxt" + "\"";
                QString myCommand = "";
                for (int j = k ; j < kk ; ++j)
                    myCommand =  myCommand  + "\"" + fileNames[j] + "\"" + " ";//+ fileGroup.at(j)
                myCommand = hxtWorkingDirectory + preamble + outputFileName + " " + myCommand;
                system(myCommand.toLatin1());
                k = kk;
                if (ui->openAfter->isChecked())
                    readData(fileGroupParts[0] + "CS"  + csSettings + ".hxt");
            }
        }
        kk = i + 1;
        // do Hexitec bit
        QString hxtWorkingDirectory = "";
        QStringList fileGroupParts = fileNames[k].split("reduced_");
        QString outputFileName = "\"" + fileGroupParts[0] + "CS"  + csSettings + ".hxt" + "\"";
        QString myCommand = "";
        for (int j = k ; j < fileNames.size() ; ++j)
            myCommand =  myCommand  + "\"" + fileNames[j] + "\"" + " ";
        myCommand = hxtWorkingDirectory + preamble + outputFileName + " " + myCommand;
        system(myCommand.toLatin1());
        if (ui->openAfter->isChecked())
            readData(fileGroupParts[0] + "CS"  + csSettings + ".hxt");
    }
    else
    {
        preamble += " -nc ";
        preamble += " -o ";
        int k = 0;
        int kk = 0;
        int i;
        for (i = 0; i < fileNames.size()-1; ++i)
        {
            QStringList fileNameParts0 = fileNames[i].split("reduced_");
            QStringList fileNameParts1 = fileNames[i+1].split("reduced_");
            if (fileNameParts1[0] == fileNameParts0[0])
                continue;
            else
            {
                kk = i + 1;
                // do Hexitec bit
                QString hxtWorkingDirectory = "";
                QStringList fileGroupParts = fileNames[k].split("reduced_");
                QString outputFileName = "\"" + fileGroupParts[0] + "NOCS" + ".hxt" + "\"";
                QString myCommand = "";
                for (int j = k ; j < kk ; ++j)
                    myCommand =  myCommand  + "\"" + fileNames[j] + "\"" + " ";//+ fileGroup.at(j)
                myCommand = hxtWorkingDirectory + preamble + outputFileName + " " + myCommand;
                system(myCommand.toLatin1());
                k = kk;
                if (ui->openAfter->isChecked())
                    readData(fileGroupParts[0] + "NOCS" + ".hxt");
            }
        }
        kk = i + 1;
        // do Hexitec bit
        QString hxtWorkingDirectory = "";
        QStringList fileGroupParts = fileNames[k].split("reduced_");
        QString outputFileName = "\"" + fileGroupParts[0] + "NOCS" +".hxt" + "\"";
        QString myCommand = "";
        for (int j = k ; j < fileNames.size() ; ++j)
            myCommand =  myCommand  + "\"" + fileNames[j] + "\"" + " ";
        myCommand = hxtWorkingDirectory + preamble + outputFileName + " " + myCommand;
        system(myCommand.toLatin1());
        if (ui->openAfter->isChecked())
            readData(fileGroupParts[0] + "NOCS" + ".hxt");
    }
/*
    else //don't do charge sharing
    {
        int k = 0;
        int kk = 0;
        int i;

        double xStep = (double) (ui->endValue->value() - ui->startValue->value()) / (double) ui->noOfBins->value();
        Slice *slice = new Slice();
        connect(slice, SIGNAL(writeMessage(QString)), this, SLOT(externalMessage(QString)));

        for (i = 0; i < fileNames.size()-1; ++i)
        {
            QStringList fileNameParts0 = fileNames[i].split("reduced_");
            QStringList fileNameParts1 = fileNames[i+1].split("reduced_");
            if (fileNameParts1[0] == fileNameParts0[0])
                continue;
            else
            {
                kk = i + 1;
                QStringList fileGroupParts = fileNames[k].split("reduced_");
                QString outputFileName = fileGroupParts[0] + "_NOCS.hxt";
                QString myCommand = "";
                writeMessage("Summing the following files into one slice:");
                for (int j = k ; j < kk ; ++j)
                {
                    if (j == k)
                        slice->clear();
                    writeMessage("[" + QString::number(j)+ "]" + "..." + fileNames[j].right(50));
                    slice->readDAT(fileNames[j]);
                }
                k = kk;
                writeMessage("Attempting to write:" + outputFileName);
                slice->makeCommonX(xStep);
                slice->writeHXT(outputFileName);
                if (ui->openAfter->isChecked())
                    readData(outputFileName);
            }
        }
        kk = i + 1;
        QStringList fileGroupParts = fileNames[k].split("reduced_");
        QString outputFileName = fileGroupParts[0] + "_NOCS.hxt";
        writeMessage("Summing the following files into one slice:");
        for (int j = k ; j < fileNames.size() ; ++j)
        {
            writeMessage("[" + QString::number(j)+ "]" + "..." + fileNames[j].right(50));
            if (j == k)
                slice->clear();
            slice->readDAT(fileNames[j]);
        }
         writeMessage("Attempting to write:" + outputFileName);
         slice->makeCommonX(xStep);
         slice->writeHXT(outputFileName);
         if (ui->openAfter->isChecked())
            readData(outputFileName);
         delete slice;
      }
      */
}

void ChargeSharing::externalMessage(QString Message)
{
    writeMessage(Message);
}
