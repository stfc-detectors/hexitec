#include "hxtchargedsharingsumgenerator.h"
//
//#include <QDebug>
//#include <QTime>
HxtChargedSharingSumGenerator::HxtChargedSharingSumGenerator(int nRows, int nCols, ProcessingDefinition *processingDefinition) :
   HxtChargedSharingGenerator(nRows, nCols, processingDefinition)
{

}

void HxtChargedSharingSumGenerator::processEnergies(unordered_map <int, double>*pixelEnergyMap)
{
//    QTime qtTime;
//    int calcTime = 0, histoTime = 0;
//    qtTime.restart();
   calculateChargedSharing(pixelEnergyMap);
//   calcTime = qtTime.elapsed();
//   qtTime.restart();
   hxtItem->addToHistogramWithSum(*pixelEnergyMap);
//   histoTime = qtTime.elapsed();
//   qDebug() << "CSD calculat: " << (calcTime) << " ms.";
//   qDebug() << "CSD histogra: " << (histoTime) << " ms.";
   incrementProcessedEnergyCount();
   delete pixelEnergyMap;
}
