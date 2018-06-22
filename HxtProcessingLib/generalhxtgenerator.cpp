#include "generalhxtgenerator.h"
///
#include <QDebug>

GeneralHxtGenerator::GeneralHxtGenerator(int nRows, int nCols, ProcessingDefinition *processingDefinition)
{
   this->nRows = nRows;
   this->nCols = nCols;
   frameSize = nRows * nCols;
   hxtItem = new HxtItem(nRows, nCols, processingDefinition->getBinStart(), processingDefinition->getBinEnd(), processingDefinition->getBinWidth());

   pixelValue = NULL;
   processedEnergyCount = 0;
   hxtItem->setTotalEnergiesToProcess(0);

   setFrameProcessingInProgress(true);

   thresholdValue = 0;

   switch (processingDefinition->getThreshholdMode())
   {
      case ThresholdMode::NONE:
         // Nothing to do here really
         break;
      case ThresholdMode::SINGLE_VALUE:
         thresholdValue = processingDefinition->getThresholdValue();
         break;
      case ThresholdMode::THRESHOLD_FILE:
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
      pixelRow = NULL;
      free(pixelCol);
      pixelCol = NULL;
      free(pixelValue);
      pixelValue = NULL;
   }
   delete hxtItem;
   hxtItem = NULL;
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


