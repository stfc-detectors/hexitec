/*
Slice class for 2Easy code
- this defines a slice of voxels
S D M Jacques 24 Feb 2011
*/

#ifndef SLICE_H
#define SLICE_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QStringList>

#include "sarray.h"
#include "treeitem.h"
#include "volume.h"
#include "voxel.h"
#include "datamodel.h"
#include "reservable.h"
#include "applicationoutput.h"

#include "Dense"
using namespace Eigen;

static const int MAX_BINS = 1000;
static const int MAX_ROWS = 80;
static const int MAX_COLS = 80;
static const int MAX_SPECTRUM_SIZE = MAX_BINS * MAX_ROWS * MAX_COLS + MAX_BINS;

class Slice : public QObject, public TreeItem, public Reservable
{
   Q_OBJECT

public:
   // Constructors
   Slice(QString name, QObject *parent = 0);
   Slice(QString name, QString fileName);
   Slice(QString name, QStringList fileNameList);
   Slice(QString name, unsigned short* buffer, QString fileName);
//   Slice(QString name, QString varName, int dummy);
   Slice(QString name, int rows, int cols, int depth, double value);

   ~Slice();

   // Methods (in alphabetical order)
   Slice *add(Slice *anOtherSlice);
   Slice *add(double value);
//   Slice *backProject();
//   Slice *eigenImageSlice();
//   Slice *prinComp();

//   void calculatePrinComps();
   static bool fileNameListValid(QStringList fileNameList);
   SArray<double> channelImage(double value);
   void detach();
   double *getSummedImageY();
   int getGridSizeX();
   int getGridSizeY();
   int getNumberOfBins();
   QString getTitle();
   QString getFileName();
   int getSliceToReplace();
   int getVoxelDataLen();
   QVector<double> getXData(int xPix, int yPix);
   QVector<double> getYData(int xPix, int yPix);
   bool isCompatible(Slice *anOtherSlice);
   Q_INVOKABLE QObject *mask(double *theMask);
   Slice *multiply(double value);
   Q_INVOKABLE QObject *ones(int rows, int cols, int depth);
   Q_INVOKABLE QObject *plus(QObject *anotherSlice);
   Q_INVOKABLE QObject *plus(double value);
   static QVector <Slice *> readFileNameList(QStringList fileNameList);
   static Slice *readFileBuffer(unsigned short* buffer, QString fileName);
   void resize(int, int);
//   Q_INVOKABLE void sendToMatlab();
//   Q_INVOKABLE void sendToMatlab(QString varName);
   void setVoxelDataLen(int voxelDataLen);
   SArray<double> sumImage();
   SArray<double> sumImage(int start, int end);
   Q_INVOKABLE void testInfo() {writeMessage("This is a test");}
   Q_INVOKABLE QObject *times(double value);
   int valueToChannel(double value);
   Slice *veil(double *theMask);
   bool writeXY(QString);
   bool writeXMY(QString, QString);
   void writeEZD(QString);
   void writeHXT(QString);
   void writeHIF(QString);
   Q_INVOKABLE QObject *zeros(int rows, int cols, int depth);
//   void setData(double *data);
signals:
   void writeError(QString);
   void writeMessage(QString);
   void writeWarning(QString);
   void initializeSlice(Slice *);

private:
   enum XType {NONE, COMMON, UNIQUE};
   struct HxtBuffer {
       char hxtLabel[8];
       quint64 hxtVersion;
       int motorPositions[9];
       int filePrefixLength;
       char filePrefix[100];
       char dataTimeStamp[16];
       quint32 nRows;
       quint32 nCols;
       quint32 nBins;
       double *allData;
//       double channel[MAX_BINS];
//       double *spectrum;
//       double spectrum[64000];
//       double allData[6401000];
/* This is calculated from the maximum possible bins, rows and columns as follows:
 * max bins = 1000, max rows = 80, max cols = 80 therefore:
 * max channel data = 1000
 * max spectrum = max bins * max rows * max cols
 *     double channel[1000];
 *     double spectrum[6400000];
 * where these arrays start and end will be determined on reading the
 * values from the buffer (proobably - tbc!!!)
 * */
   };

   // Variables
   Voxel *voxels;
   QVector <double> binVoltage;
   QVector <double> commonX;
   QVector < QVector <Voxel *> > contentVoxel;
   double *summedImageY;
   MatrixXd eigenWeights;
   MatrixXd eigenSpectra;
   QString fileName;
   int sliceToReplace;
   int gridSizeX;
   int gridSizeY;
   int numberOfBins;
   double maxData;
   double meanData;
   double minData;
   static const QString multipleSuffixes;
   static const QString simpleSuffixes;  
   XType xType;
   int voxelDataLen;

   // Methods
   void addParameters();
   void attach();
   void replace(int sliceToReplace);
   Slice *clone(QString name);
   static QString nextSliceName();
   static QString roleBackSliceName();
//   bool makeCommonX(double);
//   void myFFT(int, int, QVector <double> &, QVector <double> &);
//   double *getData();
   void postDataInit();
   void postDataInit(QString fileName);
   void preDataInit(QString name);
   bool readDAT(QString);
   bool readEZD(QString);
   bool readHXT(QString);
   bool readHXT(unsigned short *buffer);
   bool readHIF(QString);
   bool readXMY(QStringList);
   bool readXY(QStringList);
   bool squeezeX();
   void stats();
   void zeroStats();
};

#endif // SLICE_H
