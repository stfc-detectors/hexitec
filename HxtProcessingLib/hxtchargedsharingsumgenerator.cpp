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
//   qDebug() << Q_FUNC_INFO;
   calculateChargedSharing(frame);
   hxtItem->addFrameDataToHistogramWithSum(frame, 1);
   incrementProcessedEnergyCount();
}

void HxtChargedSharingSumGenerator::processEnergies(double *frame)
{
//   qDebug() << Q_FUNC_INFO;
   calculateChargedSharing(frame);
   hxtItem->addFrameDataToHistogramWithSum(frame);
   incrementProcessedEnergyCount();
//   QThread::msleep(1200*2);
}

