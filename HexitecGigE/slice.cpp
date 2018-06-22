/*
Slice class for 2Easy code
- this defines a slice of voxels
S D M Jacques 24 Feb 2011
*/

#include <QVector>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDataStream>
#include <math.h>
#include <QProgressDialog>
#include <gridsizequery.h>
//#include <matlab.h>
#include <cfloat>
#include <QDebug>

#include "slice.h"

const QString Slice::simpleSuffixes = QString(".dat.ezd.hif.hxt");
const QString Slice::multipleSuffixes = QString(".sb.xmy.xy.txt");

/* Constructs a Slice with a name but no data.
  */
Slice::Slice(QString name, QObject *parent) : QObject(parent)
{
//   qDebug() << "::Slice(QStr, QObj)";
   preDataInit(name);
}

/* Returns a title suitable for using on e.g. images of the slice.
  */

QString Slice::getTitle()
{
    return QString(objectName()  + ", max value: " + QString::number(maxData));
}

QString Slice::getFileName()
{
    return fileName;
}

/* Constructs a Slice from a single file of type ezd, exd or dat.
  */
Slice::Slice(QString name, QString fileName)
{
//    qDebug() << "::Slice(QStr, QStr)  ";

   preDataInit(name);
   bool status = false;
   QString fileRoot = QFileInfo(fileName).fileName();

   if (fileRoot.contains("hxt"))
   {
      status = readHXT(fileName);
   }

   if (status)
   {
      this->fileName = fileName;
      postDataInit();
   }
   else
   {
      throw QString("Cannot read file " + fileName);
   }
}

/*
  Constructs a slice from a set of files of type sb, xy, xmy or txt.
  */
Slice::Slice(QString name, QStringList fileNameList)
{
//    qDebug() << "::Slice(QStr, QStrList)";
   preDataInit(name);

   this->fileName = fileNameList[0];
   postDataInit();

}

Slice::Slice(QString name, unsigned short* buffer, QString fileName)
{
//    qDebug() << "::Slice(QStr, us buff, QStr)";
   preDataInit(name);

   readHXT(buffer);
   this->fileName = fileName;

   postDataInit(fileName);
}

/* Does the pre-data reading initializing which is common to all the constructors.
  */
void Slice::preDataInit(QString name)
{
   setObjectName(name);
   voxelDataLen = 0;
   gridSizeX = 0;
   gridSizeY = 0;
   minData = 0.0;
   maxData = 0.0;
   meanData = 0.0;
   contentVoxel.resize(0);
   commonX.resize(0);
   zeroStats();
   xType = NONE;
   connect(this, SIGNAL(writeMessage(QString)), ApplicationOutput::instance(), SLOT(writeMessage(QString)));
   connect(this, SIGNAL(writeWarning(QString)), ApplicationOutput::instance(), SLOT(writeWarning(QString)));
   connect(this, SIGNAL(writeError(QString)), ApplicationOutput::instance(), SLOT(writeError(QString)));
}

/* Does the post-data reading initializing which is common to all the constructors.
   Initializing has to be split like this or types which take a long time to read
   such as 'XY' do not get displayed properly in Workspace.
  */
void Slice::postDataInit()
{
   addParameters();

   QVector<QVariant> sliceData;
   sliceData << objectName() << "Slice" << "" << "";
   QModelIndex parentIndex = DataModel::instance()->getItemIndex("myVolume");
   TreeItem::init(sliceData, &parentIndex, objectName(), TreeItem::SLICE);

   attach();
   setProperty("objectName", objectName());
   stats();
}

void Slice::postDataInit(QString fileName)
{
   sliceToReplace = -1;
   addParameters();

   QVector<QVariant> sliceData;
   sliceData << objectName() << "Slice" << "" << "";
   QModelIndex parentIndex = DataModel::instance()->getItemIndex("myVolume");
   sliceToReplace = TreeItem::init(sliceData, &parentIndex, objectName(), fileName);

   if (sliceToReplace >= 0)
   {
      roleBackSliceName();
      replace(sliceToReplace);
   }
   else
   {
      attach();
      setProperty("objectName", objectName());
   }

   stats();
}

Slice::Slice(QString name, int rows, int cols, int depth, double value)
{
//    qDebug() << "::Slice(QStr, int rows, int cols, .., ..)";
   preDataInit(name);

   resize(rows, cols);

   for (int i = 0; i < rows; i++)
   {
      for (int j = 0; j < cols; j++)
      {
         contentVoxel[i][j] = new Voxel(depth, value);
      }
   }
   postDataInit();
}

Slice::~Slice()
{
//   qDebug() << "Slice::~Slice() DTOR called";
   int iRow, iCol;
   for (iRow = 0; iRow < gridSizeX; iRow++)
   {
      for (iCol = 0; iCol < gridSizeY; iCol++)
      {
           delete contentVoxel[iRow][iCol];
      }
   }

   commonX.clear();
   free(summedImageY);
}

/* These are the parameters which are displayed in the Workspace tree.
  */
void Slice::addParameters()
{
   addParameter("File Name", fileName);
   addParameter("Width", gridSizeX);
   addParameter("Height", gridSizeY);
   addParameter("Length", voxelDataLen);
}

int Slice::getGridSizeX()
{
   return gridSizeX;
}

int Slice::getGridSizeY()
{
   return gridSizeY;
}

int Slice::getNumberOfBins()
{
   return numberOfBins;
}

int Slice::getVoxelDataLen()
{
   return voxelDataLen;
}

void Slice::setVoxelDataLen(int voxelDataLen)
{
   this->voxelDataLen = voxelDataLen;
}

/* The variables in this method are only used in the commented out part of stats().
  */
void Slice::zeroStats()
{
   return;
}

/* Resets contentVoxel to be a rows * cols array of (Voxel *)
  */
void Slice::resize(int rows, int cols)
{
   for (int i = 0; i < rows; ++i)
   {
      contentVoxel.push_back( QVector <Voxel *> () );
      for (int j = 0 ; j < cols ; ++j)
      {
         Voxel *v = new Voxel;
         contentVoxel[i].push_back(v);
      }
   }
   gridSizeX = rows;
   gridSizeY = cols;
   return;
}


/* Calculates the statistics for this Slice.
  */
void Slice::stats()
{
   double sumData = 0;

   minData = DBL_MAX;
   maxData = -DBL_MAX;

   for (int row = 0; row < gridSizeX ; ++row )
   {
      for (int col = 0; col < gridSizeY ; ++col )
      {
         contentVoxel[row][col]->stats();
         maxData = std::max(maxData, contentVoxel[row][col]->getMax());
         minData = std::min(minData, contentVoxel[row][col]->getMin());
         sumData += contentVoxel[row][col]->getSum();
      }
   }
   meanData = sumData/(1.0 * gridSizeX * gridSizeY);
}

void Slice::writeHXT(QString fileName)
{
   if (!xType == COMMON)
   {
      emit writeMessage("Currently only implemented for slice data with common X scale");
      return;
   }
   if (voxelDataLen != commonX.size())
   {
      emit writeMessage("VoxelDataLen != commonX.size()");
      return;
   }

   QFile ofile(fileName);
   if (!ofile.open(QIODevice::WriteOnly))
   {
      emit writeMessage("Can not write file : " + fileName);
      return;
   }
   QDataStream bout(&ofile);
   char myChar;
   quint64 hxtVersion;
   quint32 nRows, nCols, nBins;
   hxtVersion = 1;
   nRows = (quint32) gridSizeX;
   nCols = (quint32) gridSizeY;
   nBins = (quint32) voxelDataLen;
   QString hxtLabel = "HEXITECH";
   for (int i = 0; i < 8; ++i)
   {
      myChar = hxtLabel.at(i).toLatin1();
      bout.writeRawData((char *) &myChar, sizeof(myChar));
      hxtLabel += myChar;
   }
   bout.writeRawData((char *) &hxtVersion, sizeof(hxtVersion));
   bout.writeRawData((char *) &nRows, sizeof(nRows));
   bout.writeRawData((char *) &nCols, sizeof(nRows));
   bout.writeRawData((char *) &nBins, sizeof(nRows));

   quint32 iBin, iRow, iCol;
   // Read Channels
   QVector <double> channels(nBins);
   for (iBin = 0 ; iBin < nBins ; ++iBin)
   {
      bout.writeRawData((char *) &commonX[iBin], sizeof(commonX[iBin]));
   }
   // Read data
   QVector <double> spectrum(nBins);
   QString fileStem;
   for (iRow = 0; iRow < nRows; ++iRow)
   {
      for (iCol = 0; iCol < nCols; ++iCol)
      {
         for (iBin = 0 ; iBin < nBins ; ++iBin)
         {
            bout.writeRawData((char *) &contentVoxel[iRow][iCol]->contentYData[iBin], sizeof(double));
         }
      }
   }
   emit writeMessage("data successfully writen");  // should comment this out
   ofile.close();
}

bool Slice::readHXT(unsigned short *buffer)
{
    struct HxtBuffer hxtBuffer;
    hxtBuffer.allData = (double*) malloc (MAX_SPECTRUM_SIZE * sizeof(double));
    double *allDataPointer;
    allDataPointer = hxtBuffer.allData;
    unsigned short *tmpBuffer;

    unsigned int bufferSize = sizeof(hxtBuffer) - sizeof(double *);
    memcpy((void *) &hxtBuffer, (void *) buffer, bufferSize);

    tmpBuffer = buffer + bufferSize/sizeof(unsigned short);
    bufferSize = ((hxtBuffer.nBins * hxtBuffer.nRows * hxtBuffer.nCols) + hxtBuffer.nBins) * sizeof(double);

    unsigned long long dataAddress;
    memcpy((void *) &dataAddress, (void *) tmpBuffer, 8);

    memcpy((void *) allDataPointer, (void *) (dataAddress), bufferSize);

    gridSizeX = hxtBuffer.nRows;
    gridSizeY = hxtBuffer.nCols;
    numberOfBins = hxtBuffer.nBins;

    commonX.resize(hxtBuffer.nBins);
    memcpy((void *) &commonX[0], (void *) (allDataPointer), hxtBuffer.nBins * sizeof(double));

    resize(gridSizeX, gridSizeY);
    summedImageY = (double*) calloc (numberOfBins, sizeof(double)); // freed()'d in DTOR

    allDataPointer += numberOfBins;
    int currentVoxel = 0;
    int iRow;
    int iCol;
    int iBin;
    for (iRow = 0; iRow < gridSizeX; iRow++)
    {
       contentVoxel[iRow].resize(gridSizeY);
       for (iCol = 0; iCol < gridSizeY; iCol++)
       {
           contentVoxel[iRow][iCol]->contentXData.resize(numberOfBins);
           contentVoxel[iRow][iCol]->contentYData.resize(numberOfBins);
           memcpy((void *) &(contentVoxel[iRow][iCol]->contentYData[0]), (void *) (allDataPointer), numberOfBins * sizeof(double));
           for (iBin = 0; iBin < numberOfBins; iBin++)
           {
              summedImageY[iBin] += contentVoxel[iRow][iCol]->contentYData[iBin];
           }
           currentVoxel++;
           allDataPointer += numberOfBins;
       }
    }

    voxelDataLen = numberOfBins;
    zeroStats();
    xType = COMMON;

    free(hxtBuffer.allData);
    return (true);
}

bool Slice::readHXT(QString fileName)
{
//    qDebug() << "Slice::readHXT(Qstr)  !";
   QFile file(fileName);
   if (!file.open(QIODevice::ReadOnly))
   {
      emit writeMessage("Cannot open file " + fileName + ": Skipping this file");
      return(false);
   }

   char myChar;
   quint64 hxtVersion;
   quint32 nRows, nCols, nBins;
   // Additional variables required for format version 2
   int mSSX, mSSY, mSSZ, mSSROT, mTimer, mGALX, mGALY, mGALZ, mGALROT;
   std::string filePrefix;
   int filePrefixLength;
   std::string dataTimeStamp;

   bool jj;
   QString hxtLabel;
   for (int i = 0; i < 8; ++i)
   {
      jj   = file.getChar(&myChar);
      hxtLabel += myChar;
   }

   if (hxtLabel.contains("HEXITECH"))
   {
      emit writeMessage("Type: Hexitec Spectral File");
   }
   else
   {
      emit writeMessage("<B>Format not recognised !</B>");
      file.close();
      return(false);
   }
   file.read((char *) &hxtVersion, sizeof(hxtVersion));

   /// Check what format version the file has
   if (hxtVersion >= 2)
   {
       // Version 2, read in additional header entries
       file.read((char*)&mSSX, sizeof(mSSX));
       file.read((char*)&mSSY, sizeof(mSSY));
       file.read((char*)&mSSZ, sizeof(mSSZ));
       file.read((char*)&mSSROT, sizeof(mSSROT));
       file.read((char*)&mTimer, sizeof(mTimer));
       file.read((char*)&mGALX, sizeof(mGALX));
       file.read((char*)&mGALY, sizeof(mGALY));
       file.read((char*)&mGALZ, sizeof(mGALZ));
       file.read((char*)&mGALROT, sizeof(mGALROT));
       file.read((char*) &filePrefixLength, sizeof(filePrefixLength));

        // Read file prefix character by character into stringstream object
        std::stringstream ss;

        int charsToRead = filePrefixLength;
        int timeStampLength = 13;
        if (hxtVersion == 3)
        {
           charsToRead = 100;
           timeStampLength = 16;
        }

        for(int k = 0; k < charsToRead; k++)
        {
            //read string
            char c;
            file.read((char*)&c, sizeof(char));
            ss << c;
        }
        filePrefix = ss.str();

        // Read timestamp character by character into stringstream object
        std::stringstream timeStampStream;
        for (int l= 0; l < timeStampLength; l++)
        {
            char c;
            file.read((char*)&c, sizeof(char));
            timeStampStream << c;
        }
        dataTimeStamp = timeStampStream.str();

   }
   file.read((char *) &nRows, sizeof(nRows));
   file.read((char *) &nCols, sizeof(nCols));
   file.read((char *) &nBins, sizeof(nBins));

   emit writeMessage("Version: " + QString::number(hxtVersion));
   emit writeMessage("Number of rows: " + QString::number(nRows));
   emit writeMessage("Number of columns: " + QString::number(nCols));
   emit writeMessage("Number of bins: " + QString::number(nBins));
   ///
   emit writeMessage("mSSX: " + QString::number(mSSX) + " mSSY: " + QString::number(mSSY) +
                     " mSSZ: " + QString::number(mSSZ));

   emit writeMessage("mSSROT: " + QString::number(mSSROT) + " mTimer: " + QString::number(mTimer) +
                     " mGALX: " + QString::number(mGALX));

   emit writeMessage("mGALY: " + QString::number(mGALY) + " mGALZ: " + QString::number(mGALZ) +
                     " mGALROT: " + QString::number(mGALROT));

   emit writeMessage("filePrefix: " + QString(filePrefix.c_str()));
   emit writeMessage("dataTimeStamp: " + QString(dataTimeStamp.c_str()));

   resize(nRows,nCols);

   quint32 iRow, iCol, iBin;
   double d;

   // Read Channels
 ///////////////////////////////  QVector <double> channels(nBins);
   for (iBin = 0 ; iBin < nBins ; ++iBin)
   {
      file.read((char *) &d, sizeof(d));
      commonX.push_back(d);
   }
   // Read data
 ///////////////////////////////  QVector <double> spectrum(nBins);
 ///////////////////////////////  QString fileStem;

   numberOfBins = nBins;
   summedImageY = (double*) calloc (numberOfBins, sizeof(double)); // freed()'d in DTOR
   for (iRow = 0; iRow < nRows; ++iRow)
   {
      for (iCol = 0; iCol < nCols; ++iCol)
      {
         for (iBin = 0 ; iBin < nBins ; ++iBin)
         {
            file.read((char *) &d, sizeof(d));
            summedImageY[iBin] += d;
            contentVoxel[iRow][iCol]->contentYData.push_back(d);
         }
      }
   }

   file.close();
   voxelDataLen = nBins;
   zeroStats();
   xType = COMMON;
   return(true);
}

bool Slice::squeezeX()
{
   // Squeeze X axis
   if (!(xType == UNIQUE))
      return(false);

   // this is the case where all pixels have idenitcal x
   commonX.resize(voxelDataLen);
   for (int k = 0; k < voxelDataLen ; ++k)
      commonX[k] = contentVoxel[0][0]->contentXData[k];
   for (int i = 0; i < gridSizeX; ++i)
   {
      for (int j = 0; j < gridSizeY; ++j)
      {
         contentVoxel[i][j]->contentXData.resize(0);;
      }
   }
   xType = COMMON;

   return(true);
}

/* Converts a double value to a channel number for this slice.
  */
int Slice::valueToChannel(double value)
{
   int n;
   if (xType == NONE)
      n = (int) floor(value+0.5);
   if (xType == COMMON)
   {
      for (n = 0; n < commonX.size(); ++n)
         if (value < commonX[n])
            break;
      if (n > 0)
         n--;
   }
   return n;
}

/* Produces an SArray (to use as an image) for a single channel.
  */
SArray<double> Slice::channelImage(double value)
{
   // Convert the value to a channel number
   int channel = valueToChannel(value);

   SArray <double> imageData = SArray<double>();
   imageData.resize(gridSizeX, gridSizeY);

   // FIX ME - check the channel is valid at this point
   // if (channel is valid)
   int iRow, iCol;
   for (iRow = 0; iRow < gridSizeX; ++iRow)
      for (iCol = 0; iCol < gridSizeY; ++iCol)
         imageData[iRow][iCol] = contentVoxel[iRow][iCol]->contentYData[channel];

   return imageData;
}

/* Produces an SArray (to use as an image) for the data summed over all channels.
   Note that since this is most common sumContentYData is calculated on reading in so
   there is no point in this sumImage calling the other one.
  */
SArray<double> Slice::sumImage()
{
   SArray <double> imageData = SArray<double>();
   imageData.resize(gridSizeX, gridSizeY);
   int iRow, iCol;
   for (iRow = 0; iRow < gridSizeX; ++iRow)
      for (iCol = 0; iCol < gridSizeY; ++iCol)
         imageData[iRow][iCol] = contentVoxel[iRow][iCol]->getSum();

   return imageData;
}

/* Produces an SArray (to use as an image) for the data summed between channel 'start' and
   channel 'end'.
   */
SArray<double> Slice::sumImage(int start, int end)
{
   SArray <double> imageData = SArray<double>();
   imageData.resize(gridSizeX, gridSizeY);

   // This should actually check that end is less than the minimum of all the voxel contentYData.sizes instead
   // of assuming that they are all the same.
   if (start >= 0 && end < contentVoxel[0][0]->contentYData.size())
   {
      int iRow, iCol;
      for (iRow = 0; iRow < gridSizeX; ++iRow)
      {
         for (iCol = 0; iCol < gridSizeY; ++iCol)
         {
            // The old code used to count how many times end exceeded contentYData.size() but
            // the value was never used.
            imageData[iRow][iCol] = 0;
            for (int j = start ; (j < end) && j < contentVoxel[iRow][iCol]->contentYData.size() ;  ++j)
               imageData[iRow][iCol] += contentVoxel[iRow][iCol]->contentYData[j];
         }
      }
   }
   return imageData;
}

/* Attaches the Slice to its parent Volume's data.
  */
void Slice::attach()
{
   if (TreeItem::parent()->getType() == TreeItem::VOLUME)
   {
      Volume *volume = static_cast<Volume *>(TreeItem::parent());
      volume->addSlice(this);
   }
}

void Slice::replace(int sliceToReplace)
{
   if (TreeItem::parent()->getType() == TreeItem::VOLUME)
   {
      Volume *volume = static_cast<Volume *>(TreeItem::parent());
      Slice *sliceToDelete = volume->sliceAt(sliceToReplace);
      volume->replaceSlice(sliceToReplace, this);
      delete(sliceToDelete);
   }
}

/* Removes the Slice from its parent Volume's data.
  */
void Slice::detach()
{
   if (TreeItem::parent()->getType() == TreeItem::VOLUME)
   {
      Volume *volume = static_cast<Volume *>(TreeItem::parent());
      volume->removeSlice(this);
   }
}

double *Slice::getSummedImageY()
{
   return summedImageY;
}

/* This takes over responsibilities which were previously in MainWindow.
   It is static because it may create more than one Slice and the Slices
   created still need some mediation from MainWindow's initializeSlice.
   */
QVector<Slice *> Slice::readFileNameList(QStringList fileNameList)
{
   QVector<Slice *> slices = QVector<Slice *>();
   // If the extension of the first file contained sb xmy xy or txt then you pass ALL the
   // file names to the Slice constructor to construct a single Slice.

   QString fileSuffix = QFileInfo(fileNameList[0]).suffix();

   if (fileSuffix.contains("sb") || fileSuffix.contains("xmy") ||
       fileSuffix.contains("xy") || fileSuffix.contains("txt"))
   {
      slices.append(new Slice(nextSliceName(), fileNameList));
   }
   // otherwise try to create a new Slice from each specified file name
   else
   {
      QProgressDialog progress("Reading files...", "Stop read", 0, fileNameList.size());
      progress.setWindowModality(Qt::WindowModal);
      progress.show();
      for (int i = 0; i < fileNameList.size() ; ++i)
      {
         slices.append(new Slice(nextSliceName(), fileNameList[i]));
         progress.setValue(i + 1);
         if (progress.wasCanceled())
            break;
      }
      progress.update();
      progress.deleteLater();

   }
   return slices;
}

Slice *Slice::readFileBuffer(unsigned short* buffer, QString fileName)
{
   Slice *slice;
   // If the extension of the first file contained sb xmy xy or txt then you pass ALL the
   // file names to the Slice constructor to construct a single Slice.
         slice = new Slice(nextSliceName(), buffer, fileName);
//        progress.setValue(i + 1);
//         if (progress.wasCanceled())
//            break;

//      progress.update();
//      progress.deleteLater();

   return slice;
}

int Slice::getSliceToReplace()
{
   return sliceToReplace;
}

/* Returns true if the fileNameList is valid. This is taken to mean that one of:
   a) it contains only files of ONE of the 'many files per slice type' e.g. all are .sb
   b) it contains only files of the 'single file per slice type' e.g. a .ezd and a .dat
   */
bool Slice::fileNameListValid(QStringList fileNameList)
{
   bool valid = true;
   QString suffix;
   QString fixedSuffix = "";

   suffix = QFileInfo(fileNameList[0]).suffix();
   if (Slice::multipleSuffixes.contains(suffix))
   {
      fixedSuffix = suffix;
   }

   for (int i = 0; i < fileNameList.size(); i++)
   {
      suffix = QFileInfo(fileNameList[i]).suffix();
      if ((fixedSuffix != "" && suffix != fixedSuffix) ||
          (fixedSuffix == "" && !Slice::simpleSuffixes.contains(suffix))
          )
      {
         valid = false;
         break;
      }
   }
   return valid;
}

char sliceAliasChar = '@';

/* Returns a suitable name for the next slice.
  */
QString Slice::nextSliceName()
{
   sliceAliasChar++;
   // Step over the gap between 'Z' and 'a'
   if (sliceAliasChar == '[')
      sliceAliasChar = 'a';

   return QString("Image_" + QString::fromLatin1(&sliceAliasChar, 1));
}

QString Slice::roleBackSliceName()
{

   // Step over the gap between 'Z' and 'a'
   if (sliceAliasChar == 'a')
   {
      sliceAliasChar = 'Z';
   }
   else
   {
      sliceAliasChar--;
   }
   return QString("Image_" + QString::fromLatin1(&sliceAliasChar, 1));

}

/* Returns true if anOtherSlice is compatible with 'this' for operations such as 'add'.
  */
bool Slice::isCompatible(Slice *anOtherSlice)
{
   bool compatible = false;
   if (anOtherSlice->gridSizeX == gridSizeX &&
       anOtherSlice->gridSizeY == gridSizeY &&
       anOtherSlice->xType == xType)
   {
      compatible = true;
   }
   return compatible;
}

/* Returns the xData for the given position in the Slice.
  */
QVector<double> Slice::getXData(int xPix, int yPix)
{
   QVector<double> xData;
   QVector<double> yData;
   switch (xType)
   {
   case NONE:
      yData = contentVoxel[xPix][yPix]->contentYData;
      xData.resize(yData.size());
      for (int i = 0 ; i < yData.size(); ++i )
      {
         xData[i] = (double) i;
      }
      break;
   case COMMON:
      xData = commonX;
      break;
   case UNIQUE:
      xData =  contentVoxel[xPix][yPix]->contentXData;
      break;
   default:
      break;
   }
   return xData;
}

/* Returns the yData for the given position in the Slice.
  */
QVector<double> Slice::getYData(int xPix, int yPix)
{
   return contentVoxel[xPix][yPix]->contentYData;
}

/* Creates a new Slice by adding 'this' to 'anOtherSlice'. See Q_INVOKABLE counterpart plus().
 */
Slice *Slice::add(Slice *anOtherSlice)
{
   Slice *newSlice;

   if (isCompatible(anOtherSlice))
   {
      int i, j;
      newSlice = clone(name + "_+_" + anOtherSlice->objectName());

      // Deal with the Voxels (and the x values for uniqueX)
      for (i = 0; i < gridSizeX; i++)
      {
         for (j = 0; j < gridSizeY; j++)
         {
            newSlice->contentVoxel[i][j] = contentVoxel[i][j]->add(anOtherSlice->contentVoxel[i][j]);
         }
      }
      newSlice->postDataInit();
   }
   else
   {
      throw QString("Slices to be added must have equal grid sizes.");
   }
   return newSlice;
}

/* Creates a new Slice by adding value to each Voxel. See Q_INVOKABLE counterpart plus().
 */
Slice *Slice::add(double value)
{
   int i, j;
   Slice *newSlice = clone(name + "_+_" + QString::number(value));

   for (i = 0; i < gridSizeX; i++)
   {
      for (j = 0; j < gridSizeY; j++)
      {
         newSlice->contentVoxel[i][j] = contentVoxel[i][j]->add(value);
      }
   }
   newSlice->postDataInit();

   return newSlice;
}

/* Creates a new Slice by multiplying all the Voxels of 'this' by value. See Q_INVOKABLE counterpart times().
  */
Slice *Slice::multiply(double value)
{
   int i, j;
   Slice *newSlice = clone(name + "_x_" + QString::number(value));

   for (i = 0; i < gridSizeX; i++)
   {
      for (j = 0; j < gridSizeY; j++)
      {
         newSlice->contentVoxel[i][j] = contentVoxel[i][j]->multiply(value);
      }
   }
   newSlice->postDataInit();

   return newSlice;
}

/* This is a Q_INVOKABLE wrapper for add() for use in scripts. As well as creating the new slice it
   also emits the signal to add it to the object list and returns it as a (QObject *) rather than a (Slice *)
  */
QObject *Slice::plus(QObject *anotherSlice)
{
   Slice *newSlice = add((Slice *)anotherSlice);
   emit initializeSlice(newSlice);
   return newSlice;
}

/* This is a Q_INVOKABLE wrapper for add() for use in scripts. As well as creating the new slice it
   also emits the signal to add it to the object list and returns it as a (QObject *) rather than a (Slice *)
  */
QObject *Slice::plus(double value)
{
   Slice *newSlice = add(value);
   emit initializeSlice(newSlice);
   return newSlice;
}

/* This is a Q_INVOKABLE wrapper for multiply() for use in scripts. As well as creating the new slice it
   also emits the signal to add it to the object list and returns it as a (QObject *) rather than a (Slice *)
  */
QObject *Slice::times(double value)
{
   Slice *newSlice = multiply(value);
   emit initializeSlice(newSlice);
   return newSlice;
}

/* This is a Q_INVOKABLE wrapper for veil() for use in scripts. As well as creating the new slice it
   also emits the signal to add it to the object list and returns it as a (QObject *) rather than a (Slice *)
  */
QObject *Slice::mask(double *theMask)
{
   Slice *newSlice = veil(theMask);
   emit initializeSlice(newSlice);
   return newSlice;
}

/* Creates a new Slice by masking the data of this one. Each contentVoxel is multiplied by the value
   in theMask at the same position. NB it is called veil() to distinguish it from its Q_INVOKABLE
   counterpart mask() cf add()/plus(), multiply()/times().
   */
Slice *Slice::veil(double *theMask)
{
   int i, j;
   Slice *newSlice = clone(name + "_MASKED");

   for (i = 0; i < gridSizeX; i++)
   {
      for (j = 0; j < gridSizeY; j++)
      {
         newSlice->contentVoxel[i][j] = contentVoxel[i][j]->multiply(theMask[i * gridSizeX + j]);
      }
   }
   newSlice->postDataInit();

   return newSlice;
}

/* Makes a PARTIAL clone of this Slice. This is used by the operation functions. It creates a new slice
   which is based on this one has not called postDataInit. Normally this kind of
   thing would be done by a copy constructor but Q_Objects are not allowed to have one.
   */
Slice *Slice::clone(QString name)
{
   int i;
   Slice *newSlice = new Slice(name);
   newSlice->resize(gridSizeX, gridSizeY);
   newSlice->xType = xType;
   if (xType == COMMON)
   {
      newSlice->voxelDataLen = voxelDataLen;
      for (i = 0; i < voxelDataLen; i++)
      {
         newSlice->commonX.append(commonX.at(i));
      }
   }

   return newSlice;
}



