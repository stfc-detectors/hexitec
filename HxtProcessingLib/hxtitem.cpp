#include "hxtitem.h"
#include <iostream>
#include <math.h>
#include <qdebug.h>

using namespace std;


HxtItem::HxtItem(int nRows, int nCols, long long binStart, long long binEnd, double binWidth)
{
   this->frameSize = nRows * nCols;
   setBinStart(binStart);
   setBinEnd(binEnd);
   setBinWidth(binWidth);
   nBins = (int)(((binEnd - binStart) / binWidth) + 0.5);
   hxtBin = NULL;
   initialiseHxtBuffer(nRows, nCols);

   summedHistogram = NULL;
   hxtsProcessed = 0;
}

HxtItem::~HxtItem()
{
   qDebug() << "~HxtItem free dat mem man!";
   if (summedHistogram != NULL)
   {
      free(summedHistogram);
      summedHistogram = NULL;
   }

   if (hxtBin != NULL)
   {
      free(hxtBin);
      hxtBin = NULL;
   }
}

void HxtItem::initialiseHxtBuffer(int nRows, int nCols)
{
   double currentBin;

   strncpy(hxtV3Buffer.hxtLabel, "HEXITECH", sizeof(hxtV3Buffer.hxtLabel));
   hxtV3Buffer.hxtVersion = 3;
   hxtV3Buffer.filePrefixLength = 0;
   strncpy(hxtV3Buffer.filePrefix, "(blank)", 7);
   hxtV3Buffer.nRows = nRows;
   hxtV3Buffer.nCols = nCols;
   hxtV3Buffer.nBins = nBins;
   hxtV3Buffer.allData = (double *) calloc((nBins * frameSize) + nBins, sizeof(double));
   hxtBin = hxtV3Buffer.allData;
   histogramPerPixel = hxtV3Buffer.allData + nBins;

   currentBin = binStart;
   for (long long i = binStart; i < nBins; i++, currentBin += binWidth)
   {
      *hxtBin = currentBin;
      hxtBin++;
   }
   hxtBin = hxtV3Buffer.allData;
}

void HxtItem::initialiseTotalSpectrum()
{
   summedHistogram = (long long *) calloc(nBins, sizeof(long long));    // free()'d in DTOR
}

void HxtItem::setTotalEnergiesToProcess(long long totalEnergiesToProcess)
{
   this->totalEnergiesToProcess = totalEnergiesToProcess;
}

void HxtItem::incrementTotalEnergiesToProcess()
{
   this->totalEnergiesToProcess = totalEnergiesToProcess++;
}

long long HxtItem::getTotalEnergiesToProcess()
{
   return totalEnergiesToProcess;
}

long long HxtItem::getBinStart() const
{
   return binStart;
}

void HxtItem::setBinStart(const long long value)
{
   binStart = value;
}

long long HxtItem::getBinEnd() const
{
   return binEnd;
}

void HxtItem::setBinEnd(const long long value)
{
   binEnd = value;
}

double HxtItem::getBinWidth() const
{
   return binWidth;
}

void HxtItem::setBinWidth(double value)
{
   binWidth = value;
}

// Called when the user NOT selected spectrum option
void HxtItem::addFrameDataToHistogram(double *frame)
{
    double *currentHistogram = &histogramPerPixel[0];
    double thisEnergy;
    int bin;
    int pixel;

    int frameSize = hxtV3Buffer.nRows * hxtV3Buffer.nCols;
    for (int i = 0; i < frameSize; i++)
    {
       pixel = i;
       thisEnergy = frame[i];
       if (thisEnergy == 0)
           continue;
       bin = (int)((thisEnergy / binWidth));
       if (bin <= nBins)
       {
          (*(currentHistogram + (pixel * nBins) + bin))++;
       }
       else
       {
 /*         qDebug() << "BAD BIN = " << bin << " in pixel " << pixel << " ("
                   << (int)(pixel/400) << "," << (pixel % 400) <<")"*/;
       }
    }

    hxtsProcessed++;
}

// Called when the user HAS selected spectrum option
void HxtItem::addFrameDataToHistogramWithSum(double *frame)
{
   double *currentHistogram = &histogramPerPixel[0];
   long long *summed = &summedHistogram[0];
   double thisEnergy;
   uint64_t bin;
   uint32_t pixel;

   uint32_t frameSize = hxtV3Buffer.nRows * hxtV3Buffer.nCols;
   for (uint32_t i = 0; i < frameSize; i++)
   {
      pixel = i;
      thisEnergy = frame[i];

      if (thisEnergy == 0.0)
          continue;
      bin = (uint64_t)((thisEnergy / binWidth));
      if (bin <= nBins)
      {
         (*(currentHistogram + (pixel * nBins) + bin))++;
         (*(summed + bin)) ++;
      }
      else
      {
         /*qDebug() << "BAD BIN = " << bin << " in pixel " << pixel << " ("
                  << (int)(pixel/400) << "," << (pixel % 400) <<")"*/;
      }
   }

   hxtsProcessed++;
}

HxtItem::HxtV3Buffer *HxtItem::getHxtV3Buffer()
{
   return &hxtV3Buffer;
}

double *HxtItem::getHxtV3AllData()
{
   return hxtV3Buffer.allData;
}

double *HxtItem::getEnergyBin()
{
   return hxtBin;
}

long long *HxtItem::getSummedHistogram()
{
   return summedHistogram;
}

