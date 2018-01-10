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
   qDebug() << "::Slice(QStr, QObj)";
   preDataInit(name);
}

/* Constructs a Slice from a matlabVariable.
  */
/// Redundant now that Matlab's taken out altogether?
//Slice::Slice(QString name, QString varName, int dummy)
//{
//   preDataInit(name);
//   matlab *matlab = matlab::instance();
//   if (matlab->getSliceFromMatlab(this, varName) == matlab->SUCCESS)
//   {
//      QVector <double> xScale;
//      if (matlab->getVectorFromMatlab(xScale, varName + "XScale") == matlab->SUCCESS)
//      {
//         if (xScale.size() == voxelDataLen)
//         {
//            for (int i = 0 ; i < voxelDataLen; ++i)
//               commonX.push_back(xScale[i]);
//            xType = COMMON;
//         }
//         else
//         {
//            emit writeWarning("activeSliceXScale size is not consistent with activeSlice - ignoring activeSliceXscale");
//         }
//         postDataInit();
//      }
//      else
//      {
//         emit writeWarning("activeSliceXScale could not be read");
//      }
//   }
//   else
//   {
//      throw QString("Failed to construct Slice from matlab variable: \"" + varName + "\"");
//   }
//}



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
    qDebug() << "::Slice(QStr, QStr)  ";

   preDataInit(name);
   bool status = false;
   QString fileRoot = QFileInfo(fileName).fileName();

   if (fileRoot.contains("hxt"))
   {
      status = readHXT(fileName);
   }
   else if (fileRoot.contains("hif"))
   {
       status = readHIF(fileName);
   }
   else if (fileRoot.contains("ezd"))
   {
      status = readEZD(fileName);
   }
   else if (fileRoot.contains("dat"))
   {
      status = readDAT(fileName);
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
    qDebug() << "::Slice(QStr, QStrList)";
   preDataInit(name);

   QString fileSuffix = QFileInfo(fileNameList[0]).suffix();
   // This check is done twice
   if (fileSuffix.contains("sb") || fileSuffix.contains("xmy"))
      readXMY(fileNameList);
   if (fileSuffix.contains("xy") || fileSuffix.contains("txt"))
      readXY(fileNameList);

   this->fileName = fileNameList[0];
   postDataInit();

}

Slice::Slice(QString name, unsigned short* buffer, QString fileName)
{
    qDebug() << "::Slice(QStr, us buff, QStr)";
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
   // Make voxels a NULL ptr
   voxels = NULL;
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
    qDebug() << "::Slice(QStr, int rows, int cols, .., ..)";
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
   if (voxels != NULL)
      delete[](voxels);
   commonX.clear();
   free(summedImageY);
}

/*
  This method and ones() are really meant for use in scripts and they should be static. However
  static methods can't easily be called from QML so you need some other instance of Slice to use
  them: newslice = any_old_slice.zeros(20,20,20)
  */
QObject *Slice::zeros(int rows, int cols, int depth)
{
   Slice *newSlice = new Slice("zeros", rows, cols, depth, 0.0);
   emit initializeSlice(newSlice);
   return newSlice;
}

QObject *Slice::ones(int rows, int cols, int depth)
{
   Slice *newSlice = new Slice("ones", rows, cols, depth, 1.0);
   emit initializeSlice(newSlice);
   return newSlice;
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

#include "SVD"

//Slice *Slice::prinComp()
//{

//    Slice *newSlice = new Slice(name + "_Eigen_Images");
//    newSlice->resize(gridSizeX, gridSizeY);

//    int nComps = eigenWeights.rows();
//    if (nComps == 0)
//    {
//        calculatePrinComps();
//    }

//    // Construct a slice based on existing stored weights and spectra within (this) instance
//    for (int iComp = 0 ; iComp < nComps ; ++iComp)
//    {
//        newSlice->commonX.push_back(iComp);
//    }
//    newSlice->voxelDataLen = nComps;
//    int iPix;
//    for (int iCol = 0; iCol < gridSizeY; ++iCol)
//    {
//       for (int iRow = 0; iRow < gridSizeX; ++iRow)
//       {
//          iPix = iCol*gridSizeX+iRow;
//          // Create new voxel
//          Voxel *v = new Voxel;
//          contentVoxel[iRow].push_back(v);
//          for (int iComp= 0 ; iComp < nComps ; ++iComp)
//          {
//              newSlice->contentVoxel[iRow][iCol]->contentYData.push_back(eigenWeights(iComp,iPix));
//          }
//       }
//    }

//    newSlice->zeroStats();
//    newSlice->xType = COMMON;
//    newSlice->postDataInit();
//    return newSlice;
//}

//void Slice::calculatePrinComps()
//{
//    // Calculate the components
//    int nRC = gridSizeX*gridSizeY;
//    MatrixXd MAT;
//    MAT.resize(nRC,voxelDataLen);
//    // Repack the data
//    int iPix;
//    for (int i = 0; i < gridSizeY ; ++i)
//    {
//        for (int j = 0; j < gridSizeX; ++j)
//        {
//            iPix = i*gridSizeX+j;
//            for (int k = 0 ; k < voxelDataLen; ++k)
//            {
//                MAT(iPix,k) = contentVoxel[j][i]->contentYData[k];
//            }
//        }
//    }
//    writeMessage("Calculating principle components");
//    JacobiSVD<MatrixXd> svd(MAT, ComputeThinU | ComputeThinV);
//    writeMessage("Done");
//    writeMessage("MAT size : " + QString::number(MAT.rows())+ ","+QString::number(MAT.cols()));
//    writeMessage("SVD U size : " + QString::number(svd.matrixU().rows())+ ","+QString::number(svd.matrixU().cols()));
//    writeMessage("SVD V size : " + QString::number(svd.matrixV().rows())+ ","+QString::number(svd.matrixV().cols()));
//    eigenWeights = svd.singularValues().asDiagonal()*svd.matrixU().transpose();
//    eigenSpectra = svd.matrixV();
//    //eigenWeights.transposeInPlace();
//   // eigenSpectra.transposeInPlace();

//}

//Slice *Slice::eigenImageSlice()
//{
//    // Now superceded by Slice::prinComp()- method needs removing
//    // Constructs a new slice from the eigen weights stored in an existing (this) slice
//    int nComps = eigenWeights.rows();

//    Slice *newSlice = new Slice(name + "_Eigen_Images");
//    newSlice->resize(gridSizeX, gridSizeY);

//    for (int iComp = 0 ; iComp < nComps ; ++iComp)
//    {
//        newSlice->commonX.push_back(iComp);
//    }
//    newSlice->voxelDataLen = nComps;
//    int iPix;
//    for (int iCol = 0; iCol < gridSizeY; ++iCol)
//    {
//       for (int iRow = 0; iRow < gridSizeX; ++iRow)
//       {
//          iPix = iCol*gridSizeX+iRow;
//          // Create new voxel
//          Voxel *v = new Voxel;
//          contentVoxel[iRow].push_back(v);
//          for (int iComp= 0 ; iComp < nComps ; ++iComp)
//          {
//              newSlice->contentVoxel[iRow][iCol]->contentYData.push_back(eigenWeights(iComp,iPix));
//          }
//       }
//    }
//    newSlice->zeroStats();
//    newSlice->xType = COMMON;
//    newSlice->postDataInit();
//    return newSlice;
//}

///* Moved here from main window, modified where marked. There definitely was a bug which is now fixed so that
//   it will produce a new Slice. However the contents of the new Slice may be wrong.
//   Used to 'return' on error from several places, these now 'return NULL'.
//*/
//Slice *Slice::backProject()
//{
//   // temporary driver part
//   bool bPRotTraFlag = true;
//   int bPFilterTypeNum = 1;
//   double bPStartAngle = 0;
//   double bPStopAngle = 180;
//   double PI = 3.14159265;
//   //
//   int rotSide, traSide;
//   if(bPRotTraFlag)
//   {
//      rotSide = this->gridSizeY;
//      traSide = this->gridSizeX;
//      emit writeMessage("gridSizeY is rotation: " + QString::number(this->gridSizeY));
//   }
//   else
//   {
//      traSide = this->gridSizeY;
//      rotSide = this->gridSizeX;
//      emit writeMessage("gridSizeX is rotation: " + QString::number(this->gridSizeX));
//   }
//   if(rotSide > traSide)
//   {
//      emit writeError("The algorithm currently work only when rotation side <= translation side.");
//      return NULL;
//   }

//   double bPStepAngle = (bPStopAngle-bPStartAngle)/(rotSide-1);
//   QVector <double> anglesVec;

//   for(int a=0; a<rotSide; ++a)
//      anglesVec.push_back((bPStartAngle+a*bPStepAngle)*PI/180.0);

//   if(anglesVec.size() != rotSide)
//   {
//      emit writeError("Number of angles not equal to side size.");
//      return NULL;
//   }

//   int nn2 = 2 * pow(2, ceil(log(1.0*traSide)/log(2.0)));
//   QVector <double> dataVec;
//   dataVec.resize(nn2);
//   QVector <double> Filt;

//   if(bPFilterTypeNum == 0)
//   {
//      emit writeError( "Filter should be chosen to proceed.");
//      return NULL;
//   }
//   else if(bPFilterTypeNum == 1)
//   {
//      for(int uu=0; uu<traSide; ++uu)
//         Filt.push_back(fabs(sin(-PI+uu*(2*PI/traSide))));
//   }
//   else if(bPFilterTypeNum == 2)
//   {
//      for(int uu=0; uu<traSide; ++uu)
//         Filt.push_back(fabs((-PI+uu*(2*PI/traSide))));
//   }

//   emit writeMessage("Back projection will be carried out for all channels");

//   QVector < QVector < QVector <double> > > allVec;
//   double midIndex = (1.0*traSide+1.0)/2.0;
//   QVector < QVector <double> > xpr, ypr, filtIndex, BPIa;
//   xpr.resize(traSide);
//   ypr.resize(traSide);
//   filtIndex.resize(traSide);
//   BPIa.resize(traSide);
//   for(int b=0; b<traSide; ++b)
//      for(int c=0; c<traSide; ++c)
//      {
//         xpr[b].push_back(1.0*c-midIndex+1.0);
//         ypr[b].push_back(1.0*b-midIndex+1.0);
//         filtIndex[b].push_back(0.0);
//         BPIa[b].push_back(0.0);
//      }
//   QVector < int > tempI;
//   QVector < QPair <int, int> > pairI;
//   QVector < QVector < QPair <int, int> > > spota;
//   QVector < QVector < int > > nfi;
//   for (int mm = 0 ; mm < anglesVec.size() ; mm++)
//   {
//      QVector < QVector <int> > F;
//      spota.push_back (pairI);
//      nfi.push_back (tempI);
//      for (int i = 0 ; i < traSide ; i++)
//      {
//         F.push_back(tempI);
//         for (int j = 0 ; j < traSide ; j ++)
//         {
//            double filtidx = floor(0.5+(midIndex+xpr[i][j]*sin(anglesVec[mm])-ypr[i][j]*cos(anglesVec[mm])));
//            F[i].push_back(filtidx);
//         }
//      }

//      for (int k = 0  ; k < traSide; k ++)
//      {
//         for (int l = 0 ; l < traSide ; l ++)
//         {
//            if (F[l][k] > 0 && F[l][k] <= traSide)
//            {
//               QPair <int,int> tem;
//               tem.first = l;
//               tem.second = k;
//               spota[mm].push_back(tem);
//               nfi[mm].push_back(F[l][k]-1);
//            }
//         }
//      }
//   }
//   int numberOfChannels = this->commonX.size();
//   QProgressDialog progress("Back projection will be carried out for all channels...", "Stop read", 0, numberOfChannels-1);
//   progress.setWindowModality(Qt::WindowModal);
//   progress.show();
//   unsigned long int count = 0;
//   for(int oo=0; oo < numberOfChannels; ++oo) // commonX specific
//   {
//      QVector < QVector <double> > BPI;
//      BPI.resize(traSide);
//      for(int dd=0; dd<traSide; ++dd)
//         for(int zz=0; zz<traSide; ++zz)
//            BPI[dd].push_back(0.0);

//      QVector < QVector <double> > filtPR;
//      filtPR.resize(rotSide);
//      if(bPRotTraFlag)
//      {
//         for(int aa=0; aa<rotSide; ++aa)
//            for(int bb=0; bb<traSide; ++bb)
//               filtPR[aa].push_back(this->contentVoxel[aa][bb]->contentYData[oo]);
//      }
//      else
//      {
//         for(int aa=0; aa<traSide; ++aa)
//            for(int bb=0; bb<rotSide; ++bb)
//               filtPR[aa].push_back(this->contentVoxel[aa][bb]->contentYData[oo]);
//      }

//      for(int hh=0; hh<filtPR.size(); ++hh)
//      {
//         QVector <double>  x1, y1;
//         x1.resize(filtPR[hh].size());
//         y1.resize(filtPR[hh].size());
//         int gg;
//         for(gg=0; gg<filtPR[hh].size(); ++gg)
//         {
//            x1[gg]   = filtPR[hh][gg];
//            y1[gg]   = 0.0;
//         }

//         myFFT(1, filtPR[hh].size(), x1, y1);

//         for(gg=0; gg<filtPR[hh].size(); ++gg)
//         {
//            x1[gg]   = x1[gg] * Filt[gg];
//            y1[gg]   = y1[gg] * Filt[gg];
//         }

//         myFFT(-1, filtPR[hh].size(), x1, y1);

//         for(gg=0; gg<filtPR[hh].size(); ++gg)
//         {
//            filtPR[hh][gg] = x1[gg];
//         }
//      }

//      for (int ii = 0  ; ii < rotSide ; ii++)
//      {
//         for (int jj = 0; jj <  spota[ii].size(); ++jj)
//         {
//            int r = spota[ii][jj].first;
//            int c = spota[ii][jj].second;
//            int ll = nfi[ii][jj];
//            BPI[r][c] += filtPR[ii][ll];
//         }
//      }

//      for(int x=0; x<BPI.size(); ++x)
//      {
//         for(int y=0; y<BPI[x].size(); ++y)
//         {
//            BPI[x][y] = BPI[x][y]/(anglesVec.size()*1.0);
//         }
//      }
//      allVec.push_back(BPI);
//      count++;
//      progress.setValue(count);
//      progress.update();
//      if(progress.wasCanceled())
//         return NULL;
//   }

//   progress.update();
//   progress.deleteLater();

//   Slice *newSlice = new Slice("back projected");

//   newSlice->resize(traSide, traSide);
//   newSlice->voxelDataLen = allVec.size();
//   for(int ee=0; ee<allVec.size(); ++ee)
//   {
//      // This was originally copying the new commonX to the new commonX
//      newSlice->commonX.push_back(this->commonX[ee]);
//   }
//   for(int ww=0; ww<traSide; ++ww)
//      for(int xx=0; xx<traSide; ++xx)
//      {
//         for(int dd=0; dd<allVec.size(); ++dd)
//         {
//            newSlice->contentVoxel[ww][xx]->contentYData.push_back(allVec[dd][ww][xx]);
//         }
//      }

//   newSlice->zeroStats();
//   newSlice->xType = COMMON;
//   newSlice->postDataInit();
////   emit initializeSlice(newSlice);
//   return newSlice;
//}

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
   /*
   minChannelValue.resize(0);
   maxChannelValue.resize(0);
   meanChannelValue.resize(0);
   variance.resize(0);
   standardDeviation.resize(0);
   for (int i = 0; i < voxelDataLen; ++i)
   {
      minChannelValue.push_back(0.0);
      maxChannelValue.push_back(0.0);
      meanChannelValue.push_back(0.0);
      variance.push_back(0.0);
      standardDeviation.push_back(0.0);
   }*/
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
   /*
   QVector <double> minChannelValue;
   QVector <double> maxChannelValue;
   QVector <double> meanChannelValue;
   QVector <double> variance;
   QVector <double> standardDeviation;

    for (int iBin = 0 ; iBin < voxelDataLen ; ++iBin)
    {
        minChannelValue[iBin] = contentVoxel[0][0]->contentYData[iBin];
        meanChannelValue[iBin] = 0.0;
        maxChannelValue[iBin] = 0.0;
        for (int row = 0; row < gridSizeX ; ++row )
        {
            for (int col = 0; col < gridSizeY ; ++col )
            {
                if (contentVoxel[row][col]->contentYData[iBin] > maxChannelValue[iBin])
                    maxChannelValue[iBin] = contentVoxel[row][col]->contentYData[iBin];
                if (contentVoxel[row][col]->contentYData[iBin] < minChannelValue[iBin])
                    minChannelValue[iBin] = contentVoxel[row][col]->contentYData[iBin];
                meanChannelValue[iBin] += contentVoxel[row][col]->contentYData[iBin];
            }
        }
        meanChannelValue[iBin] = meanChannelValue[iBin] / (gridSizeX * gridSizeY);
    }

    // Now work out the variance
    double diff;
    for (int iBin = 0 ; iBin < voxelDataLen ; ++iBin)
    {
        variance[iBin] = 0;
         for (int row = 0; row < gridSizeX ; ++row )
        {
            for (int col = 0; col < gridSizeY ; ++col )
            {
                diff = (meanChannelValue[iBin] - contentVoxel[row][col]->contentYData[iBin]);
                variance[iBin] += diff*diff;
            }
        }
        variance[iBin] = variance[iBin] / (gridSizeX * gridSizeY);
        standardDeviation[iBin] = sqrt(variance[iBin]);
    }
    */
}


void Slice::writeEZD(QString fileName)
{
   if (xType == UNIQUE)  // this is a big fix
   {
      emit writeMessage("This is a message from the Slice class");
      squeezeX();
   }

   QFile ofile(fileName);
   if (!ofile.open(QIODevice::WriteOnly))
      emit writeMessage("Can not write file : " + fileName);

   QTextStream out(&ofile);
   QDataStream bout(&ofile);

   out << "#FILE EZD (2Easy data file)" << endl;
   out << "#VERSION 1" << endl;
   out << "#HIST " << gridSizeX << " x " << gridSizeY << endl;
   if (xType == NONE)
   {
      out << "#TYPE MY" << endl;
      out << "#YDATA "  << gridSizeX*gridSizeY << " x " << voxelDataLen << " DOUBLE" << endl;
   }
   if (xType == COMMON)
   {
      out << "#TYPE SXMY" << endl;
      out << "#XDATA "  << "1" << " x " << voxelDataLen << " DOUBLE" << endl;
      out << "#YDATA "  << gridSizeX*gridSizeY << " x " << voxelDataLen << " DOUBLE" << endl;
   }
   if (xType == UNIQUE)
   {
      out << "#TYPE MXY" << endl;
      out << "#XYDATA "  << gridSizeX*gridSizeY << " x " << voxelDataLen << " DOUBLE" << endl;
   }

   out << "#EOH "  << endl;

   if (xType == COMMON)
   {
      // write X data
      for (int i = 0; i < voxelDataLen; ++i)
         bout << (double) commonX[i];
   }
   if ((xType == NONE) | (xType == COMMON))
   {
      // write Y data
      for (int i = 0; i < gridSizeX; ++i)
      {
         for (int j = 0; j < gridSizeY; ++j)
         {
            for (int k = 0 ; k < voxelDataLen;  ++k)
               bout << (double) contentVoxel[i][j]->contentYData[k];
         }
      }
   }
   if (xType == UNIQUE)
   {
      // write XY data
      for (int i = 0; i < gridSizeX; ++i)
      {
         for (int j = 0; j < gridSizeY; ++j)
         {
            for (int k = 0 ; k < voxelDataLen;  ++k)
            {
               bout << (double) contentVoxel[i][j]->contentXData[k];
               bout << (double) contentVoxel[i][j]->contentYData[k];
            }
         }
      }
   }
   emit writeMessage("data successfully writen");  // should comment this out
   ofile.close();
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



bool Slice::readXY(QStringList fileNames)
{
   gridSizeQuery gridQuery;
   gridQuery.exec();
   if (fileNames.size() != gridQuery.numberOfRows*gridQuery.numberOfColumns)
   {
      emit writeMessage("Number of files does not match gridSize");
      return(false);
   }
   // really need to check that all files are xy
   resize(gridQuery.numberOfRows, gridQuery.numberOfColumns);

   QProgressDialog progress("Reading files...", "Stop read", 0,fileNames.size());
   progress.setWindowModality(Qt::WindowModal);
   progress.show();
   QString fileName;
   int i = 0;
   for (int j = 0 ; j < gridQuery.numberOfRows ; ++j)
   {
      for (int k = 0 ; k < gridQuery.numberOfColumns ; ++k)
      {
         fileName = fileNames[i];
         contentVoxel[j][k]->readXY(fileName);
         progress.setValue(i);
         if (progress.wasCanceled())
            break;
         ++i;
      }
   }
   progress.setValue(fileNames.size());
   progress.update();
   voxelDataLen = contentVoxel[0][0]->contentYData.size();
   //    emit writeMessage("Number of bins : " + QString::number(voxelDataLen));
   zeroStats();
   xType = UNIQUE;
   return(true);
}

bool Slice::writeXY(QString fileStem)
{
   if (xType == COMMON)
   {
      for (int i = 0 ; i < gridSizeX ; ++i)
      {
         for (int j = 0 ; j < gridSizeY ; ++j)
         {
            QString fileName;
            fileName.sprintf("_%3.3d_%3.3d.xy",i,j);
            fileName = fileStem + fileName;
            QFile file(fileName);
            if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
               return(false);
            QTextStream out(&file);
            for (int k = 0 ; k < contentVoxel[i][j]->contentYData.size(); ++k)
               out << commonX[k] << " " << contentVoxel[i][j]->contentYData[k] << endl;
            file.close();
         }
      }
   }
   return(true);
}

bool Slice::writeXMY(QString  fileStem, QString suffix)
{
   if (!xType == COMMON)
      return(false);

   QProgressDialog progress("Writing files...", "Stop write", 0,gridSizeX);
   progress.setWindowModality(Qt::WindowModal);
   progress.show();

   for (int i = 0 ; i < gridSizeX ; ++i)
   {
      QString fileName;
      fileName.sprintf("_%3.3d",i);
      fileName = fileStem + fileName + "." + suffix;
      QFile file(fileName);
      if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
         return(false);
      QTextStream out(&file);
      for (int k = 0 ; k < commonX.size(); ++k)
      {
         out << commonX[k] ;
         for (int j = 0 ; j < gridSizeY ; ++j)
            out << "\t" << contentVoxel[i][j]->contentYData[k];
         out << endl;
      }
      file.close();
      progress.setValue(i);
      if (progress.wasCanceled())
         return(false);

   }
   progress.setValue(gridSizeX);
   progress.update();

   return(true);
}

bool Slice::readXMY(QStringList fileNames)
{

   gridSizeQuery gridQuery;
   // Guess the sizes - rows = number of files
   //                 - cols = (should be) number of lines in first file
   gridQuery.setSize(fileNames.size(), 1);
   gridQuery.exec();
   if (fileNames.size() != gridQuery.numberOfRows)
   {
      emit writeMessage("Number of files does not match suggested gridSize");
      return(false);
   }
   resize(gridQuery.numberOfRows, gridQuery.numberOfColumns);

   // could do something here to check the first file

   QProgressDialog progress("Reading files...", "Stop read", 0,fileNames.size());
   progress.setWindowModality(Qt::WindowModal);
   progress.show();
   QString fileName;
   int i = 0;
   double x, y;
   for (int j = 0 ; j < gridQuery.numberOfRows ; ++j)
   {
      fileName = fileNames[j];
      QFile file(fileName);
      if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
      {
         // emit writeMessage("Cannot open file " + fileName + "/nSkipping this file");
         return(false);
      }
      QTextStream in(&file);
      while (1)
      {
         in >> x;
         if (in.atEnd())
            break;
         if (j == 0)
            commonX.push_back(x);
         for (int k = 0 ; k < gridQuery.numberOfColumns ; ++k)
         {
            in >> y;
            contentVoxel[j][k]->contentYData.push_back(y);
         }
      }
      progress.setValue(i);
      if (progress.wasCanceled())
         break;
      ++i;
   }
   progress.setValue(fileNames.size());
   progress.update();
   voxelDataLen = contentVoxel[0][0]->contentYData.size();
   zeroStats();
   xType = COMMON;
   // temporary message
   emit writeMessage("size of commonX: " + QString::number(commonX.size()));
   return(true);
}

bool Slice::readHXT(unsigned short *buffer)
{
    qDebug() << "Slice::readHXT(unsigned short *buffer) start";
    struct HxtBuffer hxtBuffer;
    hxtBuffer.allData = (double*) malloc (MAX_SPECTRUM_SIZE * sizeof(double));
    double *allDataPointer;
    allDataPointer = hxtBuffer.allData;
    Voxel *voxelPointer;
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

    contentVoxel.resize(gridSizeX);
    voxels = new Voxel[gridSizeX * gridSizeY];
    summedImageY = (double*) calloc (numberOfBins, sizeof(double));

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
           voxelPointer = &voxels[currentVoxel];
           voxelPointer->contentXData.resize(numberOfBins);
           voxelPointer->contentYData.resize(numberOfBins);
           contentVoxel[iRow][iCol] = voxelPointer;
           memcpy((void *) &(voxelPointer->contentYData[0]), (void *) (allDataPointer), numberOfBins * sizeof(double));
           for (iBin = 0; iBin < numberOfBins; iBin++)
           {
              summedImageY[iBin] += voxelPointer->contentYData[iBin];
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
    qDebug() << "Slice::readHXT(Qstr)  !";
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
   summedImageY = (double*) calloc (numberOfBins, sizeof(double));
   for (iRow = 0; iRow < nRows; ++iRow)
   {
      for (iCol = 0; iCol < nCols; ++iCol)
      {
         //voxelDataLen = iBin;
         // Create new voxel by reading nBins doubles from the file
         Voxel *v = new Voxel;
         contentVoxel[iRow].push_back(v);
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

bool Slice::readHIF(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
       emit writeMessage("Cannot open file " + fileName + ": Skipping this file");
       return(false);
    }
    char myChar;
    quint64 hifVersion;
    quint32 nRows, nCols, nBins, nComps;

    bool jj;
    QString hxtLabel;
    for (int i = 0; i < 26; ++i)
    {
       jj   = file.getChar(&myChar);
       hxtLabel += myChar;
    }

    if (hxtLabel.contains("HYPERSPECTRAL IMAGE FORMAT"))
    {
       emit writeMessage("Type: HYPERSPECTRAL IMAGE FORMAT");
    }
    else
    {
       emit writeMessage("<B>Format not recognised !</B>");
       file.close();
       return(false);
    }
    file.read((char *) &hifVersion, sizeof(hifVersion));
    file.read((char *) &nRows, sizeof(nRows));
    file.read((char *) &nCols, sizeof(nCols));
    file.read((char *) &nBins, sizeof(nBins));
    file.read((char *) &nComps, sizeof(nComps));

    emit writeMessage("Version: " + QString::number(hifVersion));
    emit writeMessage("Number of rows: " + QString::number(nRows));
    emit writeMessage("Number of columns: " + QString::number(nCols));
    emit writeMessage("Number of bins: " + QString::number(nBins));
    emit writeMessage("Number of components: " + QString::number(nComps));

    resize(nRows,nCols);

    quint32 iRow, iCol, iBin, iComp, nRC, iPix;
    double d;

    // Read Channels
    QVector <double> channels(nBins);
    for (iBin = 0 ; iBin < nBins ; ++iBin)
    {
       file.read((char *) &d, sizeof(d));
       commonX.push_back(d);
    }

    // Read Data
    nRC = nRows*nCols;
    eigenWeights.resize(nComps, nRC);
    eigenSpectra.resize(nBins, nComps);
    for (iComp = 0 ; iComp < nComps; ++iComp)
    {
        // Read Eigen image
        for (iPix = 0; iPix < nRC ; ++iPix)
        {
            file.read((char *) &d, sizeof(d));
            eigenWeights(iComp,iPix) = d;
        }
        // Read Eigen spectra
        for (iBin = 0 ; iBin < nBins; ++iBin)
        {
            file.read((char *) &d, sizeof(d));
            eigenSpectra(iBin,iComp) = d;
        }
    }

    file.close();

    // Multiply out
    MatrixXd MAT;
    MAT.resize(nBins,nRC);
    MAT = eigenSpectra * eigenWeights;
    // Unpack the result
    for (iCol = 0; iCol < nCols; ++iCol)
    {
       for (iRow = 0; iRow < nRows; ++iRow)
       {
          iPix = iCol*nRows+iRow;
          // Create new voxel
          Voxel *v = new Voxel;
          contentVoxel[iRow].push_back(v);
          for (iBin = 0 ; iBin < nBins ; ++iBin)
          {
              contentVoxel[iRow][iCol]->contentYData.push_back(MAT(iBin,iPix));
          }
       }
    }
    // Finish up
    voxelDataLen = nBins;
    zeroStats();
    xType = COMMON;
    return(true);
 }

void Slice::writeHIF(QString fileName)
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

   quint32 nComps = eigenWeights.rows();
   if (nComps == 0)
   {
       // will need to calculate the stuff
   }

   QFile ofile(fileName);
   if (!ofile.open(QIODevice::WriteOnly))
   {
      emit writeMessage("Can not write file : " + fileName);
      return;
   }

   QDataStream bout(&ofile);
   char myChar;
   quint64 hifVersion;
   quint32 nRows, nCols, nBins, nRC;
   hifVersion = 1;
   nRows = (quint32) gridSizeX;
   nCols = (quint32) gridSizeY;
   nBins = (quint32) voxelDataLen;
   QString hifLabel = "HYPERSPECTRAL IMAGE FORMAT";
   for (int i = 0; i < 26; ++i)
   {
      myChar = hifLabel.at(i).toLatin1();
      bout.writeRawData((char *) &myChar, sizeof(myChar));
      hifLabel += myChar;
   }
   bout.writeRawData((char *) &hifVersion, sizeof(hifVersion));
   bout.writeRawData((char *) &nRows, sizeof(nRows));
   bout.writeRawData((char *) &nCols, sizeof(nRows));
   bout.writeRawData((char *) &nBins, sizeof(nRows));
   bout.writeRawData((char *) &nComps, sizeof(nComps));

   quint32 iBin, iComp, iPix;
   // Read Channels
   QVector <double> channels(nBins);
   for (iBin = 0 ; iBin < nBins ; ++iBin)
   {
      bout.writeRawData((char *) &commonX[iBin], sizeof(commonX[iBin]));
   }
   // Read data
   nRC = nRows*nCols;
   eigenWeights.resize(nComps, nRC);
   eigenSpectra.resize(nBins, nComps);
   for (iComp = 0 ; iComp < nComps; ++iComp)
   {
       // Read Eigen image
       for (iPix = 0; iPix < nRC ; ++iPix)
       {
           bout.writeRawData((char *) &eigenWeights(iComp,iPix), sizeof(double));
       }
       // Read Eigen spectra
       for (iBin = 0 ; iBin < nBins; ++iBin)
       {
           bout.writeRawData((char *) &eigenSpectra(iBin,iComp), sizeof(double));
       }
   }

   ofile.close();
   emit writeMessage("data successfully writen");  // should comment this out
}

bool Slice::readEZD(QString fileName)
{
   QFile file(fileName);
   if (!file.open(QIODevice::ReadOnly))
   {
      emit writeMessage("Cannot open file " + fileName + " Skipping this file");
      return(false);
   }

   int nBins = 0;  // this info should be read from the header
   int nRows = 0;  // ditto
   int nCols = 0;  // ditto
   int numberOfXdata = 0; // not really used
   int numberOfYdata = 0; // not really used
   int numberOfXYdata = 0; // not really used

   QString headerLine;
   QStringList words;
   bool foundEOH = false;
   bool isHIST = false;
   bool validTYPE = false;
   while (!foundEOH & !file.atEnd())
   {
      headerLine = file.readLine();
      writeMessage(headerLine);
      if (headerLine.contains("#EOH"))
      {
         foundEOH = true;
         continue;
      }
      if (headerLine.contains("#HIST"))
      {
         words = headerLine.split(" ");
         nRows = words[1].toInt();
         nCols = words[3].toInt();
         emit writeMessage(QString::number(nRows) + " " + QString::number(nCols));
         isHIST = true;
      }
      if (headerLine.contains("#TYPE"))
      {
         validTYPE = true;
         if (headerLine.contains("MY"))
            xType = NONE;
         if (headerLine.contains("SXMY"))
            xType = COMMON;
         if (headerLine.contains("MXY"))
            xType = UNIQUE;
      }
      if (headerLine.contains("#XDATA"))
      {
         words = headerLine.split(" ");
         numberOfXdata = words[1].toInt();
         nBins = words[3].toInt();
         isHIST = true;
      }
      if (headerLine.contains("#YDATA"))
      {
         words = headerLine.split(" ");
         numberOfYdata = words[1].toInt();
         nBins = words[3].toInt();
         //format = words[4];
         isHIST = true;
      }
      if (headerLine.contains("#XYDATA"))
      {
         words = headerLine.split(" ");
         numberOfXYdata = words[1].toInt();
         nBins = words[3].toInt();
         //format = words[4];
         isHIST = true;
      }
   }

   if (!foundEOH | !validTYPE )
   {
      file.close();
      emit writeMessage("header not good");
      return(false);
   }
   emit writeMessage(QString::number(nBins));

   // need to cheek if size of Y or XY data is equal to nRows * nCols
   // next lot of code shouild be in:  if (isHIST) { // histogram stuff}
   resize(nRows,nCols);
   QDataStream bin(&file);
   if (xType == COMMON)
   {
      double x;
      for (int iBin = 0 ; iBin < nBins ; ++iBin)
      {
         bin >> x;
         commonX.push_back(x);
      }
   }

   if ((xType == NONE) | (xType == COMMON))
   {
      double y;
      for (int iRow = 0; iRow < nRows; ++iRow)
      {
         for (int iCol = 0; iCol < nCols; ++iCol)
         {
            for (int iBin = 0 ; iBin < nBins ; ++iBin)
            {
               bin >> y;
               contentVoxel[iRow][iCol]->contentYData.push_back(y);
            }
         }
      }
   }

   if (xType == UNIQUE)
   {
      double x, y;
      for (int iRow = 0; iRow < nRows; ++iRow)
      {
         for (int iCol = 0; iCol < nCols; ++iCol)
         {
            for (int iBin = 0 ; iBin < nBins ; ++iBin)
            {
               bin >> x >> y;
               contentVoxel[iRow][iCol]->contentXData.push_back(x);
               contentVoxel[iRow][iCol]->contentYData.push_back(y);
            }
         }
      }
   }
   file.close();
   voxelDataLen = nBins;
   zeroStats();
   return(true);
}

bool Slice::readDAT(QString fileName)
{
   QFile file(fileName);
   if (!file.open(QIODevice::ReadOnly))
   {
      emit writeMessage("Cannot open file " + fileName + ": Skipping this file");
      return(false);
   }

   int iBin, iRow, iCol;
   if (voxelDataLen == 0) // probably a new slice
   {
      // use default values
      resize(80,80);
      voxelDataLen = 400;
      zeroStats();

      double d = 0.0;
      for (iRow = 0; iRow < gridSizeX; ++iRow)
      {
         for (iCol = 0; iCol < gridSizeY; ++iCol)
         {
            for (iBin = 0 ; iBin < voxelDataLen ; ++iBin)
               contentVoxel[iRow][iCol]->contentYData.push_back(d);
         }
      }

      double voltageStep;
      voltageStep = (double) pow(2.0,16) / (double) voxelDataLen / 10.0;

      for (iBin = 1 ; iBin < voxelDataLen + 1 ; ++iBin)
      {
         binVoltage.push_back(voltageStep * iBin);
      }
   }

   quint8 n1,n2,n3,rowNumber,colNumber;
   quint32 frameNumber;
   quint16 frameNumber2;
   double value;
   quint64 amountRead = 0;
   quint64 block = sizeof(n1);
   QFileInfo fi(file);
   emit writeMessage(QString::number(fi.size()));
   QString progressLabel("Reading ..." + fileName.right(50));
   QProgressDialog progress(progressLabel, "Stop read", 0,fi.size());
   progress.setWindowModality(Qt::WindowModal);
   progress.show();
   while (!file.atEnd())
   {
      file.read((char *) &n1, sizeof(n1));
      file.read((char *) &n2, sizeof(n2));
      file.read((char *) &n3, sizeof(n3));
      amountRead += 3 * block;

      if (n1 == 255 && n2 == 255 && n3 == 255)
      {
         // read frame number
         file.read((char *) &frameNumber, sizeof(frameNumber));
         // emit writeMessage("<B>Frame Number: " + QString::number(frameNumber) + "</B>");
         file.read((char *) &frameNumber2, sizeof(frameNumber2));
         progress.setValue(amountRead);
         progress.update();
         if (progress.wasCanceled())
            break;
         continue;
      }
      if (n1 == 0 && n2 == 192)
         rowNumber = n3;
      else  // event data
      {
         value = (double) (n2 * 256 + n1);
         colNumber = n3;
         for (int kk = 0 ; kk < voxelDataLen ; ++kk)
         {
            if (value < binVoltage[kk])
            {
               contentVoxel[rowNumber][colNumber]->contentYData[kk] += 1.0;
               break;
            }
         }
      }
   }
   file.close();
   progress.update();
   progress.deleteLater();
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

//bool Slice::makeCommonX(double step)
//{
//   // should overload this so that it takes a vector argument as well for non-discrete X
//   // also should do something abou the uniqueX case i.e. delel
//   if (xType == UNIQUE)
//   {
//      for (int i = 0; i < gridSizeX; ++i)
//      {
//         for (int j = 0; j < gridSizeY; ++j)
//         {
//            contentVoxel[i][j]->contentXData.resize(0);;
//         }
//      }
//   }
//   commonX.resize(voxelDataLen);
//   for (int k = 0; k < voxelDataLen ; ++k)
//      commonX[k] = (double)(k) * step;

//   xType = COMMON;
//   return(true);
//}

///* Creates a C-style array out of the data.
//  */
//double *Slice::getData()
//{
//   double *data = (double *)malloc(voxelDataLen * gridSizeY * gridSizeX * sizeof(double));
//   double *ptr = data;
//   for (int i = 0; i < gridSizeX; i++)
//   {
//      for (int j = 0; j < gridSizeY; j++)
//      {
//         for (int k = 0; k < voxelDataLen; k++)
//         {
//            *ptr = contentVoxel[i][j]->contentYData[k];
//            ptr++;
//         }
//      }
//   }
//   return data;
//}

//void Slice::setData(double *data)
//{
//   //this->voxelDataLen = voxelDataLen;
//  // this->gridSizeY = gridSizeY;
//  // this->gridSizeX = gridSizeX;

//   double *ptr = data;
//   for (int i = 0; i < gridSizeX; i++)
//   {
//      for (int j = 0; j < gridSizeY; j++)
//      {
//         contentVoxel[i][j]->contentYData.resize(voxelDataLen);
//         for (int k = 0; k < voxelDataLen; k++)
//         {
//            contentVoxel[i][j]->contentYData[k] = *ptr;
//            ptr++;
//         }
//      }
//   }
//}

//void Slice::sendToMatlab()
//{
//   sendToMatlab(objectName());
//}

//void Slice::sendToMatlab(QString varName)
//{
//   matlab *matlab = matlab::instance();
//   int dimensions[3] = {voxelDataLen, gridSizeY, gridSizeX};

//   double *data = getData();
//   matlab->sendArrayToMatlab(dimensions, data, varName);
//   free(data);

//   if (this->xType == UNIQUE)
//   {
//      emit writeWarning("X scale has not been sent to Matlab (Not available in this version)");
//   }
//   else if (this->xType == COMMON)
//   {
//      matlab->sendVectorToMatlab(this->commonX, varName + "XScale");
//   }
//}

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

///* Moved here from MainWindow because it's only used by Slice::backProject. Since it is a general
//       purpose mathematical thing it should probably be in some kind of Math or Util class.
//       */
//void Slice::myFFT(int flag, int N, QVector <double> &x1 , QVector  <double> &y1)
//{
//   N = x1.size();
//   // WARNING LOOP VARIABLES ARE NOT LONG
//   QVector <double> x2, y2;
//   x2.resize(N);
//   y2.resize(N);
//   double arg;
//   double cosarg, sinarg;

//   for (int i = 0; i < N; i++)
//   {
//      x2[i] = 0.0;
//      y2[i] = 0.0;
//      arg = - flag * 2.0 * 3.141592654 * (double)i / (double)N;
//      for (int k = 0; k < N; k++)
//      {
//         cosarg = cos(k * arg);
//         sinarg = sin(k * arg);
//         x2[i] += (x1[k] * cosarg - y1[k] * sinarg);
//         y2[i] += (x1[k] * sinarg + y1[k] * cosarg);
//      }
//   }
//   if (flag == -1)
//   {
//      for (int i = 0 ; i < N ; i++)
//      {
//         x1[i] = x2[i] / (double)N;
//         y1[i] = y2[i] / (double)N;
//      }
//   }
//   else
//   {
//      for (int i = 0 ; i < N ; i++)
//      {
//         x1[i] = x2[i];
//         y1[i] = y2[i];
//      }
//   }
//}

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



