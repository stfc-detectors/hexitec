#include "hxtsumgenerator.h"
///
#include <QDebug>
HxtSumGenerator::HxtSumGenerator(int nRows, int nCols, ProcessingDefinition *processingDefinition) :
   HxtGenerator(nRows, nCols, processingDefinition)
{

}

void HxtSumGenerator::processEnergies(unordered_map <int, double>*pixelEnergyMap)
{
   hxtItem->addToHistogramWithSum(*pixelEnergyMap);
   incrementProcessedEnergyCount();
   delete pixelEnergyMap;
}

void HxtSumGenerator::processEnergies(uint16_t *frame)
{
   qDebug() << Q_FUNC_INFO << " ::processEnergies(frame), hardcoded threshold into histogram (fix)";
   hxtItem->addFrameDataToHistogramWithSum(frame, 10);
   incrementProcessedEnergyCount();
}


//void HxtSumGenerator::calibrateAndApplyChargedAlgorithm(uint16_t *frame, uint16_t thresholdValue, double *gradients, double *intercepts)
//{
//   qDebug() << "HxtSumGenerator::calibrateAndApplyChargedAlgorithm(" << frame << thresholdValue << gradients << intercepts;
//   qDebug() << " User selected no CS algorithm;  However ThresholdValue, gradients, intercepts are  needed here";

//    /// 1. Calibration
//    int frameSize  = nRows * nCols;
//    double *gradientValue = gradients, *interceptValue = intercepts;
//    double value = 0.0;
//    for (int i = 0; i < frameSize; i++)
//    {
//       if (frame[i] < thresholdValue)
//       {
//          frame[i] = 0;
//       }
//       else
//       {
//          value = (frame[i] * gradientValue[i] + interceptValue[i]);
//          frame[i] = value;
//       }
//    }
//   hxtItem->addFrameDataToHistogramWithSum(frame, thresholdValue);
//   incrementProcessedEnergyCount();
//}
