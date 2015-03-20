/*
Slice class for 2Easy code
- this defines a slice of voxels
S D M Jacques 24 Feb 2011
*/

#ifndef SLICE_H
#define SLICE_H

#include <QObject>
#include <QVector>
#include <voxel.h>
#include <QString>
#include <QStringList>

#include "sarray.h"
#include "treeitem.h"
#include "volume.h"
#include "voxel.h"
#include "datamodel.h"
#include "reservable.h"

#include "Dense"
using namespace Eigen;

class Slice : public QObject, public TreeItem, public Reservable
{
   Q_OBJECT

public:
   // Constructors
   Slice(QString name, QObject *parent = 0);
   Slice(QString name, QString fileName);
   Slice(QString name, QStringList fileNameList);
   Slice(QString name, QString varName, int dummy);
   Slice(QString name, int rows, int cols, int depth, double value);

   // Methods (in alphabetical order)
   Slice *add(Slice *anOtherSlice);
   Slice *add(double value);
   Slice *backProject();
   Slice *eigenImageSlice();
   Slice *prinComp();

   void calculatePrinComps();
   static bool fileNameListValid(QStringList fileNameList);
   SArray<double> channelImage(double value);
   void detach();
   int getGridSizeX();
   int getGridSizeY();
   QString getTitle();
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
   void resize(int, int);
   Q_INVOKABLE void sendToMatlab();
   Q_INVOKABLE void sendToMatlab(QString varName);
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
   void setData(double *data);
signals:
   void writeError(QString);
   void writeMessage(QString);
   void writeWarning(QString);
   void initializeSlice(Slice *);

private:
   enum XType {NONE, COMMON, UNIQUE};

   // Variables
   QVector <double> binVoltage;
   QVector <double> commonX;
   QVector < QVector <Voxel *> > contentVoxel;
   MatrixXd eigenWeights;
   MatrixXd eigenSpectra;
   QString fileName;
   int gridSizeX;
   int gridSizeY;
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
   Slice *clone(QString name);
   static QString nextSliceName();
   bool makeCommonX(double);
   void myFFT(int, int, QVector <double> &, QVector <double> &);
   double *getData();
   void postDataInit();
   void preDataInit(QString name);
   bool readDAT(QString);
   bool readEZD(QString);
   bool readHXT(QString);
   bool readHIF(QString);
   bool readXMY(QStringList);
   bool readXY(QStringList);
   bool squeezeX();
   void stats();
   void zeroStats();
};

#endif // SLICE_H