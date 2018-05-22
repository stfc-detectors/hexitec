#include "hxtgenerator.h"

#include <Windows.h>
///
#include <QDebug>

HxtGenerator::HxtGenerator(int nRows, int nCols, ProcessingDefinition *processingDefinition) :
   GeneralHxtGenerator(nRows, nCols, processingDefinition)
{
//    qDebug() << Q_FUNC_INFO << "CTOR called";
   if (processingDefinition->getTotalSpectrum())
   {
      hxtItem->initialiseTotalSpectrum();
   }

//   switch (processingDefinition->getThreshholdMode())
//   {
//      case ThresholdMode::NONE:
//         processThresholdNone(fp, result, filenameBin, filenameHxt, filenameCsv);
//         break;
//      case ThresholdMode::SINGLE_VALUE:
//         thresholdValue = processingDefinition->getThresholdValue();
//         processThresholdValue(fp, thresholdValue, result, filenameBin, filenameHxt, filenameCsv);
//         break;
//      case ThresholdMode::THRESHOLD_FILE:
//         processThresholdFile(fp, thresholdPerPixel, result, filenameBin, filenameHxt, filenameCsv);
//         break;
//      default:
//         break;
//   }

}

void HxtGenerator::processEnergies(unordered_map <int, double>*pixelEnergyMap)
{
   hxtItem->addToHistogram(*pixelEnergyMap);
   incrementProcessedEnergyCount();
   delete pixelEnergyMap;
}

void HxtGenerator::processEnergies(uint16_t *frame)
{
    qDebug() << Q_FUNC_INFO << " Feeding hardcoded thresholdvalue to hxt->addFrameDataToHistogram() - Figure out how do you get down this path later on? (And fix)";
    hxtItem->addFrameDataToHistogram(frame, 10);
    incrementProcessedEnergyCount();
}
