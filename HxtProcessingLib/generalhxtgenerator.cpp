#include "generalhxtgenerator.h"

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
}

GeneralHxtGenerator::~GeneralHxtGenerator()
{

   if (pixelValue != NULL)
   {
      free(pixelRow);
      free(pixelCol);
      free(pixelValue);
   }
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

