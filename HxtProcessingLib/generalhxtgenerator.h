#ifndef GENERALHXTGENERATOR_H
#define GENERALHXTGENERATOR_H

#include "hxtitem.h"
#include "processingdefinition.h"
#include <cstdint>
#include <unordered_map>

class GeneralHxtGenerator
{

public:
   GeneralHxtGenerator(int nRows, int nCols, ProcessingDefinition *processingDefinition);
   ~GeneralHxtGenerator();
   void enqueuePixelRawVals(double *pixelRawVals);
   void enqueuePixelEnergyMap(unordered_map<int, double> *pixelEnergyMap);
   void setFrameProcessingInProgress(bool inProgress);
   void incrementProcessedEnergyCount();
   long long getProcessedEnergyCount();
   HxtItem::HxtV3Buffer *getHxtV3Buffer();
   double *getHxtV3AllData();
   double *getEnergyBin();
   long long *getSummedHistogram();

   virtual void processEnergies(uint16_t *frame) = 0;

protected:
   bool getFrameProcessingInProgress();
   HxtItem *hxtItem;
   long long *histogram;
   int nRows;
   int nCols;
   int frameSize;
   long long processedEnergyCount;
   bool inProgress;
   int *pixelRow;
   int *pixelCol;
   double *pixelValue;
   /// Assist histogramming requiring pixel threshold
   int thresholdValue;

};

#endif // GENERALHXTGENERATOR_H
