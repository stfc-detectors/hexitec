#include "hxtchargedsharingsumgenerator.h"
//
#include <QDebug>
#include <QThread>
//#include <QTime>
HxtChargedSharingSumGenerator::HxtChargedSharingSumGenerator(int nRows, int nCols, ProcessingDefinition *processingDefinition) :
   HxtChargedSharingGenerator(nRows, nCols, processingDefinition)
{

}

void HxtChargedSharingSumGenerator::processEnergies(uint16_t *frame)
{
   calculateChargedSharing(frame);
   hxtItem->addFrameDataToHistogramWithSum(frame, thresholdValue);
   incrementProcessedEnergyCount();
}

