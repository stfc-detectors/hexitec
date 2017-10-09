#include "hxtchargedsharinggenerator.h"
#include <Windows.h>

HxtChargedSharingGenerator::HxtChargedSharingGenerator(int nRows, int nCols, ProcessingDefinition *processingDefinition) :
   HxtGenerator(nRows, nCols, processingDefinition)
{
   pixelRow = (int *) calloc(nRows, sizeof(int));
   pixelCol = (int *) calloc(nCols, sizeof(int));
   pixelValue = (double*) calloc(nRows * nCols, sizeof(double));
   chargedSharingMode = processingDefinition->getChargedSharingMode();
   setPixelGridSize(processingDefinition->getPixelGridSize());
   qDebug() << "HxtChargedSharingGenerator: chargedSharingMode, pixelGridSize"
            << chargedSharingMode << pixelGridSize;
}

void HxtChargedSharingGenerator::handleProcess()
{
   unordered_map <int, double> *pixelEnergyMap;
   int temp = 0;

  while (getFrameProcessingInProgress() || (hxtItem->getPixelEnergyMapQueueSize() > 0) || processedEnergyCount < (hxtItem->getTotalEnergiesToProcess()))
   {
      while (getFrameProcessingInProgress() &&(((temp = hxtItem->getPixelEnergyMapQueueSize())) == 0))
      {
         Sleep(10);
      }
      qDebug() << "NUMBER OF ENERGIES TO PROCESS: " << temp;
      while ((hxtItem->getPixelEnergyMapQueueSize()) > 0)
      {
         pixelEnergyMap = hxtItem->getNextPixelEnergyMap();
         if (!pixelEnergyMap->empty())
         {
//               result = processEnergies(pixelEnergy);

                 processEnergies(pixelEnergyMap);
//                 free(pixelEnergy);

               // MUST USE RESULT IN FURTHER CALCULATIONS
//               free(result);
//            writeFile(bufferStart, (validFrames * frameSize), filename);
//            free(bufferStart);
         }
      }
   }
  //   emit energyProcessingComplete(processedEnergyCount);
}

void HxtChargedSharingGenerator::handleProcess(bool totalSpectrum)
{
   unordered_map <int, double> *pixelEnergyMap;
   int temp = 0;

  while (getFrameProcessingInProgress() || (hxtItem->getPixelEnergyMapQueueSize() > 0) || processedEnergyCount < (hxtItem->getTotalEnergiesToProcess()))
   {
      while (getFrameProcessingInProgress() &&(((temp = hxtItem->getPixelEnergyMapQueueSize())) == 0))
      {
         Sleep(10);
      }
      qDebug() << "NUMBER OF ENERGIES TO PROCESS: " << temp;
      while ((hxtItem->getPixelEnergyMapQueueSize()) > 0)
      {
         pixelEnergyMap = hxtItem->getNextPixelEnergyMap();
         if (!pixelEnergyMap->empty())
         {
//               result = processEnergies(pixelEnergy);

                 processEnergiesWithSum(pixelEnergyMap);
//                 free(pixelEnergy);

               // MUST USE RESULT IN FURTHER CALCULATIONS
//               free(result);
//            writeFile(bufferStart, (validFrames * frameSize), filename);
//            free(bufferStart);
         }
      }
   }
  //   emit energyProcessingComplete(processedEnergyCount);
}

void HxtChargedSharingGenerator::processEnergies(unordered_map <int, double> *pixelEnergyMap)
{
   qDebug() << "HxtChargedSharingGenerator::processEnergies()";
   calculateChargedSharing(pixelEnergyMap);
   hxtItem->addToHistogram(*pixelEnergyMap);
   incrementProcessedEnergyCount();
   delete pixelEnergyMap;
}

void HxtChargedSharingGenerator::processEnergiesWithSum(unordered_map <int, double> *pixelEnergyMap)
{
   qDebug() << "HxtChargedSharingGenerator::processEnergiesWithSum()";
   calculateChargedSharing(pixelEnergyMap);
   hxtItem->addToHistogramWithSum(*pixelEnergyMap);
   incrementProcessedEnergyCount();
   delete pixelEnergyMap;
}

void HxtChargedSharingGenerator::setPixelGridSize(int pixelGridSize)
{
   this->pixelGridSize = pixelGridSize;
   directionalDistance = (int)pixelGridSize/2;
   qDebug() << "directionalDistance: " << directionalDistance;
}

void HxtChargedSharingGenerator::calculateChargedSharing(unordered_map <int, double>*pixelEnergyMap)
{
   int pixel;
   int index = 0;
   int length;

   unordered_map<int, double>::iterator it = pixelEnergyMap->begin();
   unordered_map<int, double>::iterator itend = pixelEnergyMap->end();

   length = pixelEnergyMap->size();
   fill(pixelRow, pixelRow + sizeof(pixelRow), 0);
   fill(pixelCol, pixelCol + sizeof(pixelCol), 0);

   while (it != itend)
   {
      pixel = it->first;
      pixelRow[index] = (int) (pixel / nRows);
      pixelCol[index] = (int) (pixel - (pixelRow[index] * nCols));
      pixelValue[index] = it->second;
//      qDebug() << "pixel: " << pixel << " index " << index
//               << " row " << pixelRow[index] << " col " << pixelCol[index]
//               << " value: " << pixelValue[index];
      it++;
      index++;
   }
   switch (chargedSharingMode)
   {
      case ADDITION:
         processAdditionChargedSharing(pixelEnergyMap, length);
         break;
      case DISCRIMINATION:
         processDiscriminationChargedSharing(pixelEnergyMap, length);
         break;
      default:
         break;
   }

}
void HxtChargedSharingGenerator::processAdditionChargedSharing(unordered_map <int, double>*pixelEnergyMap, int length)
{
   int rowIndexBegin;
   int rowIndexEnd;
   int colIndexBegin;
   int colIndexEnd;
   int row;
   int column;
   int rowShare;
   int columnShare;

   unordered_map<int, double> map = *pixelEnergyMap;

   for (int i = 0; i < length; i++)
   {
      row = pixelRow[i];
      column = pixelCol[i];
      maxValue = pixelValue[i];
//      qDebug() << "EXAMINE PIXEL: "<< (row*80)+column << " row, column " << row << column;

      rowIndexBegin = row - directionalDistance;
      rowIndexEnd = row + directionalDistance;
      colIndexBegin = column - directionalDistance;
      colIndexEnd = column + directionalDistance;

      for (int j = i + 1; j < length; j++)
      {
         if ((pixelRow[j] >= rowIndexBegin) && (pixelRow[j] <= rowIndexEnd))
         {
            if ((pixelCol[j] >= colIndexBegin) && (pixelCol[j] <= colIndexEnd))
            {
               rowShare = pixelRow[j];
               columnShare = pixelCol[j];
 //              qDebug() << "=================FOUND CHARGE SHARING AT: row, col: " << row << ", " << column
//                        << " pixel: " << (row * nCols + column) << " value: " << pixelValue[i];
//               qDebug() << "=================SHARED WITH            : row, col: " << rowShare << ", " << columnShare
//                        << " pixel: " << (rowShare * nCols + columnShare) << " value: " << pixelValue[j];

               if (pixelValue[j] > maxValue)
               {
                  maxValue = pixelValue[j];
                  pixelValue[j] += pixelValue[i];
                  pixelValue[i] = 0.0;
                  pixelEnergyMap->erase(row * nCols + column);
                  map[rowShare * nCols + columnShare] = pixelValue[j];

               }
               else
               {
                  pixelValue[i] += pixelValue[j];
                  pixelValue[j] = 0.0;
                  pixelEnergyMap->erase(rowShare * nCols + columnShare);
                  map[row * nCols + column] = pixelValue[i];
               }
            }
         }
      }

   }
//   qDebug() << "END OF CHARGED SHARING: number of pixels at start = "  << length
//            << " number of pixels remaining = " << pixelEnergyMap->size();
}


void HxtChargedSharingGenerator::processDiscriminationChargedSharing(unordered_map <int, double>*pixelEnergyMap, int length)
{
   int rowIndexBegin;
   int rowIndexEnd;
   int colIndexBegin;
   int colIndexEnd;
   int row;
   int column;
   int rowShare;
   int columnShare;

   unordered_map<int, double> map = *pixelEnergyMap;

   for (int i = 0; i < length; i++)
   {
      row = pixelRow[i];
      column = pixelCol[i];
      maxValue = pixelValue[i];

      rowIndexBegin = row - directionalDistance;
      rowIndexEnd = row + directionalDistance;
      colIndexBegin = column - directionalDistance;
      colIndexEnd = column + directionalDistance;

      for (int j = i + 1; j < length; j++)
      {
         if ((pixelRow[j] >= rowIndexBegin) && (pixelRow[j] <= rowIndexEnd))
         {
            if ((pixelCol[j] >= colIndexBegin) && (pixelCol[j] <= colIndexEnd))
            {
               rowShare = pixelRow[j];
               columnShare = pixelCol[j];

               pixelEnergyMap->erase(row * nCols + column);
               pixelEnergyMap->erase(rowShare * nCols + columnShare);
            }
         }
      }

   }
//   qDebug() << "END OF CHARGED SHARING: number of pixels at start = "  << length
//            << " number of pixels remaining = " << pixelEnergyMap->size();
}

void HxtChargedSharingGenerator::setChargedSharingMode(ChargedSharingMode chargedSharingMode)
{
   this->chargedSharingMode = chargedSharingMode;
}
