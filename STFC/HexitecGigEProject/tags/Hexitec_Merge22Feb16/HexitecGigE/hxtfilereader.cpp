#include "hxtfilereader.h"

HXTFileReader::HXTFileReader(QObject *parent) :
        QObject(parent)
{
    hxtVersion = 0;
    nRows = 0;
    nCols = 0;
    nBins = 0;
}

HXTFileReader::HXTFileReader(QString fileName)
{
}

bool HXTFileReader::openFile(QString fileName)
{
    file.setFileName(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        writeMessage("Cannot open file " + fileName);
        return(false);
    }
    else
    {
         HXTFileName = fileName;
         return(true);
    }

}

bool HXTFileReader::readHeader()
{
   if (!file.isOpen())
       return(false);

   if (file.pos()!=0)
       file.seek(0);

   char myChar;
   bool jj;
   QString hxtLabel;

   for (int i = 0; i < 8; ++i)
   {
       jj   = file.getChar(&myChar);
       hxtLabel += myChar;
   }

   if (hxtLabel.contains("HEXITECH"))
   {
       writeMessage("Type: Hexitec Spectral File");
   }
   else
   {
       writeMessage("<B>Format not recognised !</B>");
       return(false);
   }
   file.read((char *) &hxtVersion, sizeof(hxtVersion));
   file.read((char *) &nRows, sizeof(nRows));
   file.read((char *) &nCols, sizeof(nRows));
   file.read((char *) &nBins, sizeof(nRows));
   writeMessage("Version: " + QString::number(hxtVersion));
   writeMessage("Number of rows: " + QString::number(nRows));
   writeMessage("Number of columns: " + QString::number(nCols));
   writeMessage("Number of bins: " + QString::number(nBins));
   writeMessage(QString::number(file.pos()));
   return(true);
}


bool HXTFileReader::readXData()
{
   if (!file.isOpen())
       return(false);
   if (nRows == 0 || nCols == 0)
       return(false);
   file.seek(28); // if (hxtversion == 1)

   double d;
   quint64 doubleSize = sizeof(d);
   X.resize(0);
   for (int i = 0 ; i < (int) nBins ; ++i)
   {
       file.read((char *) &d, doubleSize);
       X.push_back(d);
       writeMessage(QString::number(X[i]));
   }
	return(true);
}

bool HXTFileReader::readSingleSpectrum(int Row, int Col)
{
    double d;
    quint64 doubleSize = sizeof(d);
    quint64 pos = doubleSize*(quint64)(Row*nCols+Col+1)*nBins;
    file.seek(28+pos); // if (hxtversion == 1)
    Y.resize(0);
    for (int i = 0 ; i < (int) nBins ; ++i)
    {
        file.read((char *) &d, doubleSize);
        Y.push_back(d);
        writeMessage(QString::number(Y[i]));
    }
	return(true);
}

