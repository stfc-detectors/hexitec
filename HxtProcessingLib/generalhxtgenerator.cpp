#include "generalhxtgenerator.h"
///
#include <QDebug>

GeneralHxtGenerator::GeneralHxtGenerator(int nRows, int nCols, ProcessingDefinition *processingDefinition)
{
    qDebug() << Q_FUNC_INFO << " CTOR";
   this->nRows = nRows;
   this->nCols = nCols;
   frameSize = nRows * nCols;
   hxtItem = new HxtItem(nRows, nCols, processingDefinition->getBinStart(), processingDefinition->getBinEnd(), processingDefinition->getBinWidth());

   pixelValue = NULL;
   processedEnergyCount = 0;
   hxtItem->setTotalEnergiesToProcess(0);

   setFrameProcessingInProgress(true);

   thresholdValue = 0;
   thresholdPerPixel = NULL;

   switch (processingDefinition->getThreshholdMode())
   {
      case ThresholdMode::NONE:
         // Nothing to do here really
         break;
      case ThresholdMode::SINGLE_VALUE:
         thresholdValue = processingDefinition->getThresholdValue();
         break;
      case ThresholdMode::THRESHOLD_FILE:
         thresholdPerPixel = processingDefinition->getThresholdPerPixel();
         break;
      default:
         break;
   }

}

GeneralHxtGenerator::~GeneralHxtGenerator()
{

   if (pixelValue != NULL)
   {
      free(pixelRow);
      free(pixelCol);
      free(pixelValue);
   }
   if (thresholdPerPixel != NULL)
      free(thresholdPerPixel);
   delete hxtItem;

}

void GeneralHxtGenerator::freeAllocedMemory()
{
   hxtItem->freeAllocedMemory();
}

bool GeneralHxtGenerator::getFrameProcessingInProgress()
{
   return inProgress;
}

void GeneralHxtGenerator::setFrameProcessingInProgress(bool inProgress)
{
   this->inProgress = inProgress;
}

void GeneralHxtGenerator::incrementProcessedEnergyCount()
{
   this->processedEnergyCount = processedEnergyCount++;
}

long long GeneralHxtGenerator::getProcessedEnergyCount()
{
   return processedEnergyCount;
}

HxtItem::HxtV3Buffer *GeneralHxtGenerator::getHxtV3Buffer()
{
   return hxtItem->getHxtV3Buffer();
}

double *GeneralHxtGenerator::getHxtV3AllData()
{
   return hxtItem->getHxtV3AllData();
}

double *GeneralHxtGenerator::getEnergyBin()
{
   return hxtItem->getEnergyBin();
}

long long *GeneralHxtGenerator::getSummedHistogram()
{
   return hxtItem->getSummedHistogram();
}


