#ifndef HXTITEM_H
#define HXTITEM_H

#include <cstdint>
#include <unordered_map>
#include <queue>

#define MIN_ENERGY 1.0e-100

using namespace std;

class HxtItem
{
public:
   struct HxtV3Buffer {
       char hxtLabel[8];
       uint64_t hxtVersion;
       int motorPositions[9];
       int filePrefixLength;
       char filePrefix[100];
       char dataTimeStamp[16];
       uint32_t nRows;
       uint32_t nCols;
       uint32_t nBins;
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

   HxtItem(int nRows, int nCols, long long binStart, long long binEnd, double binWidth);
   ~HxtItem();
   void initialiseTotalSpectrum();
   void setTotalEnergiesToProcess(long long totalEnergiesToProcess);
   void incrementTotalEnergiesToProcess();
   long long getTotalEnergiesToProcess();
   ///
   void addFrameDataToHistogram(double *frame);
   void addFrameDataToHistogramWithSum(double *frame);
   ///
   HxtV3Buffer *getHxtV3Buffer();
   double *getHxtV3AllData();
   double *getEnergyBin();
   long long *getSummedHistogram();


private:
   HxtV3Buffer hxtV3Buffer;
   long long getBinStart() const;
   void setBinStart(const long long value);
   long long getBinEnd() const;
   void setBinEnd(const long long value);
   double getBinWidth() const;
   void setBinWidth(double value);
   int frameSize;
   long long binStart;
   long long binEnd;
   double binWidth;
   long long nBins;
   double *hxtBin;
   double *histogramPerPixel;
   long long *summedHistogram;
   long long totalEnergiesToProcess;
   long long hxtsProcessed;
   void initialiseHxtBuffer(int nRows, int nCols);
};

#endif // HXTITEM_H
