#include "hxtitem.h"
#include <QMutexLocker>
#include <iostream>
#include <math.h>

using namespace std;


HxtItem::HxtItem(int nRows, int nCols, long long binStart, long long binEnd, double binWidth)
{
   this->frameSize = nRows * nCols;
   setBinStart(binStart);
   setBinEnd(binEnd);
   setBinWidth(binWidth);
   nBins = (int)(((binEnd - binStart) / binWidth) + 0.5);
   initialiseHxtBuffer(nRows, nCols);

   pixelEnergy = NULL;
   pixelEnergyMap = NULL;
   hxtsProcessed = 0;
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
   summedHistogram = (long long *) calloc(nBins, sizeof(long long));
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

void HxtItem::addToHistogram(unordered_map<int, double> pixelEnergyMap)
{
   double *currentHistogram = &histogramPerPixel[0];
   double thisEnergy;
   int bin;
   int pixel;

   unordered_map<int, double>::iterator it = pixelEnergyMap.begin();
   unordered_map<int, double>::iterator itend = pixelEnergyMap.end();

   while (it != itend)
   {
      pixel = it->first;
      thisEnergy = it->second;
      bin = (int)((thisEnergy / binWidth));
      if (bin <= nBins)
      {
         (*(currentHistogram + (pixel * nBins) + bin))++;
//         qDebug() << "HxtItem::addToHistogram pixel = " << pixel << " bin " << bin
//                  << " offset into histograms = " << (pixel * nBins) + bin;
      }
      else
      {
//         qDebug() << "BAD BIN = " << bin;
      }
      it++;
   }

   hxtsProcessed++;
}


void HxtItem::addToHistogramWithSum(unordered_map<int, double> pixelEnergyMap)
{

   double *currentHistogram = &histogramPerPixel[0];
   long long *summed = &summedHistogram[0];
   double thisEnergy;
   int bin;
   int pixel;

   unordered_map<int, double>::iterator it = pixelEnergyMap.begin();
   unordered_map<int, double>::iterator itend = pixelEnergyMap.end();

   while (it != itend)
   {
      pixel = it->first;
      thisEnergy = it->second;
      bin = (int)((thisEnergy / binWidth));
      if (bin <= nBins)
      {
         (*(currentHistogram + (pixel * nBins) + bin))++;
         (*(summed + bin)) ++;
//         qDebug() << "HxtItem::addToHistogram pixel = " << pixel << " bin " << bin
//                  << " offset into histograms = " << (pixel * nBins) + bin;
      }
      else
      {
//         qDebug() << "BAD BIN = " << bin;
      }
      it++;
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

