#include "hxtchargedsharinggenerator.h"
#include <Windows.h>
//
#include <QDebug>
#include <QTime>
#include <QThread>
HxtChargedSharingGenerator::HxtChargedSharingGenerator(int nRows, int nCols, ProcessingDefinition *processingDefinition) :
   HxtGenerator(nRows, nCols, processingDefinition)
{
   this->nRows = nRows;
   this->nCols = nCols;

   pixelRow = (int *) calloc(nRows * nCols, sizeof(int));
   pixelCol = (int *) calloc(nCols * nCols, sizeof(int));
   pixelValue = (double*) calloc(nRows * nCols, sizeof(double));
   chargedSharingMode = processingDefinition->getChargedSharingMode();
   setPixelGridSize(processingDefinition->getPixelGridSize());
}

void HxtChargedSharingGenerator::processEnergies(unordered_map <int, double> *pixelEnergyMap)
{
   calculateChargedSharing(pixelEnergyMap);
   hxtItem->addToHistogram(*pixelEnergyMap);
   incrementProcessedEnergyCount();
   delete pixelEnergyMap;
}

void HxtChargedSharingGenerator::setPixelGridSize(int pixelGridSize)
{
   this->pixelGridSize = pixelGridSize;
   directionalDistance = (int)pixelGridSize/2;
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
   QTime qtTime;
   int /*sortTime = 0,*/ callTime = 0;
//   qtTime.restart();
   while (it != itend)
   {
      pixel = it->first;
      pixelRow[index] = (int) (pixel / nRows);
      pixelCol[index] = (int) (pixel - (pixelRow[index] * nCols));
      pixelValue[index] = it->second;
      it++;
      index++;
   }
//   sortTime = qtTime.elapsed();
   switch (chargedSharingMode)
   {
      case ADDITION:
         processAdditionChargedSharing(pixelEnergyMap, length);
         break;
      case DISCRIMINATION:
         qtTime.restart();
         processDiscriminationChargedSharing(pixelEnergyMap, length);
         callTime = qtTime.elapsed();
         break;
      default:
         break;
   }
//   qDebug() << "CSD sortTime: " << (sortTime) << " ms.";
//   qDebug() << "CSD callTime: " << (callTime) << " ms.";
}

void HxtChargedSharingGenerator::calibrateAndApplyChargedAlgorithm(uint16_t *frame, uint16_t thresholdValue, double *gradients, double *intercepts)
{
   qDebug() << "HxtChargedSharingGenerator::calibrateAndApplyChargedAlgorithm(" << frame << thresholdValue << gradients << intercepts;
}

uint16_t *HxtChargedSharingGenerator::calibrateAndChargedSharing(uint16_t *frame, uint16_t thresholdValue, double *gradients, double *intercepts)
{
   /// 1. Calibration
   unsigned int frameSize  = nRows * nCols;
   double *gradientValue = gradients, *interceptValue = intercepts;
   double value = 0.0;
   int applyTime = 0, copyTime = 0, callTime = 0, recpTime = 0;   // Debug
   QTime qtTime;        // Debug
   qtTime.restart();    // Debug
   int nonZeroCount = 0;
   for (unsigned int i = 0; i < frameSize; i++)
   {
      if (frame[i] < thresholdValue)
      {
         frame[i] = 0;
      }
      else
      {
         nonZeroCount++;
         value = (frame[i] * gradientValue[i] + interceptValue[i]);
//         if (nonZeroCount < 75)
//            qDebug() << "frame[" << i << "] = " << frame[i] << ". Calibrated value: " << value;
         frame[i] = value;
      }
   }
   applyTime = qtTime.elapsed();
//   qDebug() << "Detected " << nonZeroCount << " pixel(s) in frame";

   /// processFrame contains empty (1-2) pixel(s) on each side to enable charge sharing algorithm execution
   int sidePadding           = 2 *  directionalDistance;
   int processedFrameRows    = (nRows + sidePadding);
   int processedFrameColumns = (nCols + sidePadding);
   int processedFrameSize    = processedFrameRows * processedFrameColumns;

   uint16_t  *processedFrame;
   processedFrame = (uint16_t *) calloc(processedFrameSize, sizeof(uint16_t));

   // Copy frame's each row into processedFrame with (directionalDistance pixel(s)) padding on each side
   int startPosn = processedFrameColumns * directionalDistance + directionalDistance;
   int endPosn   = processedFrameSize;
   int increment = processedFrameColumns;
   uint16_t *rowPtr = frame;

   qtTime.restart();
//   qDebug() << "memory copy," << startPosn << endPosn;
   for (int i = startPosn; i < endPosn; )
   {
      memcpy(&(processedFrame[i]), rowPtr, nCols * sizeof(uint16_t));   // Swapped nRows for nCols
      rowPtr = rowPtr + nCols;                                          // Ditto
      i = i + increment;
   }
   copyTime = qtTime.elapsed();

   // Apply CSD algorithm onto processedFrame
   endPosn = processedFrameSize - (processedFrameColumns * directionalDistance) - directionalDistance;

   uint16_t *neighbourPixelPtr = NULL, *pixelPtr = processedFrame;
   int rowIndexBegin = (-1*directionalDistance);
   int rowIndexEnd   = (directionalDistance+1);
   int colIndexBegin = rowIndexBegin;
   int colIndexEnd   = rowIndexEnd;
   bool bWipePixel = false;
   unsigned int countSharedEvents = 0;
   qtTime.restart();
//   qDebug() << "CS algorithm" << startPosn << endPosn << " processedFrameRows:" << processedFrameRows;
//   int iDebug = 420;
   for (int i = startPosn; i < endPosn;  i++)
   {

      if (processedFrame[i] != 0)
      {
//         if (i < iDebug)
//            qDebug() << "processedFrame[" << i << "] = " << processedFrame[i] << " = " << processedFrame << " rowIdx/colIdx Begin&End: "
//                        << rowIndexBegin << rowIndexEnd << colIndexBegin << colIndexEnd << "dD: " << directionalDistance;

         pixelPtr = (&(processedFrame[i]));       // Point at current (non-Zero) pixel
         for (int row = rowIndexBegin; row < rowIndexEnd; row++)
         {
            for (int column = colIndexBegin; column < colIndexEnd; column++)
            {
               if ((row == 0) && (column == 0)) // Don't compare pixel with itself
                  continue;

//               qDebug() << "processedFrame[" << i << "] = " << processedFrame[i] << " = " << processedFrame;
//               qDebug() << "processedFrame: " << (&(processedFrame));
//               qDebug() << "startPosn: " << startPosn;
////               qDebug() << "neighbourPixelPtr " << neighbourPixelPtr;
//               qDebug() << "pixelPtr  " << pixelPtr;
//               qDebug() << "row, col " << row << column;
//               qDebug() << "processedFrameRows " << processedFrameRows;
//               qDebug() << "processedFrameRows*row " << processedFrameRows*row;
//               qDebug() << "neighbourPixelPtr " << (pixelPtr + (processedFrameRows*row)  + column);
               neighbourPixelPtr = (pixelPtr + (processedFrameRows*row)  + column);
               /// Debug
//               if (i < iDebug)
//                  qDebug() <<  "pFrm[" << i << "] = " << processedFrame[i] <<  (&(processedFrame[i])) << "nPxlPtr: " << *neighbourPixelPtr << neighbourPixelPtr << "row, column: " << row << column
//                            << " CmpAddresses: " << ((&(processedFrame[i])) - neighbourPixelPtr);
               // Wipe this pixel if another neighbour was non-Zero
               if (bWipePixel)
               {
//                  if (i < iDebug)
//                      qDebug() << "Prev neigh hit, wiping addr: " << neighbourPixelPtr;
                   *neighbourPixelPtr = 0;
               }
               else
               {
//                  if (i < 500)
//                     qDebug() << "First time neighb hit! *nPlPtr:" << *neighbourPixelPtr << neighbourPixelPtr;
                  // Is this the first neighbouring, non-Zero pixel?
                  if (*neighbourPixelPtr != 0)
                  {
                     *neighbourPixelPtr = 0;
                     bWipePixel = true;
                     countSharedEvents++;
                  }
               }
            }
         }
         bWipePixel = false;
      }
   }
   callTime = qtTime.elapsed();
   /// Copy CSD frame (i.e. 402x402) back into originally sized frame (400x400)
   ///              DOUBLE CHECK THAT THIS IS CORRECT AND THEN REMOVE IT AFTER THE CSD FRAME HAS BEEN  processed
   qtTime.restart();
//   qDebug() << "memory copy," << startPosn << endPosn;
   rowPtr = frame;
   for (int i = startPosn; i < endPosn; )
   {
      memcpy(rowPtr, &(processedFrame[i]), nCols * sizeof(uint16_t));
      rowPtr = rowPtr + nCols;
      i = i + increment;
   }
   recpTime = qtTime.elapsed();

//   qDebug() << "Counted " << countSharedEvents << " cases of CSD.";

///   switch (chargedSharingMode)
//   {
//      case ADDITION:
//         processAdditionChargedSharing();
//         break;
//      case DISCRIMINATION:
//         qtTime.restart();
//         processDCS(&processedFrame);
//         callTime = qtTime.elapsed();
//         break;
//      default:
//         break;
//   }
//   qDebug() << "CSG Calibrat: " << (applyTime) << " ms.";
//   qDebug() << "CSG copyTime: " << (copyTime) << " ms.";
//   qDebug() << "CSG CSalTime: " << (callTime) << " ms.";

//   qDebug() << "CSG recpTime: " << (recpTime) << " ms.";
   return frame /*processedFrame*/; /// Return frame, not processedFrame (CSD frame)
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

   for (int i = 0; i < length; i++)
   {
      row = pixelRow[i];
      column = pixelCol[i];
      maxValue = pixelValue[i];

      rowIndexBegin = row - directionalDistance;
      rowIndexEnd = row + directionalDistance;
      colIndexBegin = column - directionalDistance;
      colIndexEnd = column + directionalDistance;

      if (rowIndexBegin < 0)
      {
         rowIndexBegin = 0;
      }
      if (colIndexBegin < 0)
      {
         colIndexBegin = 0;
      }
      if (rowIndexEnd >= nRows)
      {
         rowIndexEnd = nRows - 1;
      }
      if (colIndexEnd >= nCols)
      {
         colIndexEnd = nCols - 1;
      }

      for (int j = 0; j < length; j++)
      {
         if (i != j && (pixelValue[i] !=0) && (pixelValue[j] !=0))
         {
         if ((pixelRow[j] >= rowIndexBegin) && (pixelRow[j] <= rowIndexEnd))
         {
            if ((pixelCol[j] >= colIndexBegin) && (pixelCol[j] <= colIndexEnd))
            {
               rowShare = pixelRow[j];
               columnShare = pixelCol[j];

               if (pixelValue[j] > maxValue)
               {
                  pixelValue[j] += pixelValue[i];
                  maxValue = pixelValue[j];
                  pixelValue[i] = 0.0;
                  pixelEnergyMap->erase(row * nCols + column);
                  (*pixelEnergyMap)[rowShare * nCols + columnShare] = pixelValue[j];
               }
               else
               {
                  pixelValue[i] += pixelValue[j];
                  maxValue = pixelValue[i];
                  pixelValue[j] = 0.0;
                  pixelEnergyMap->erase(rowShare * nCols + columnShare);
                  (*pixelEnergyMap)[row * nCols + column] = pixelValue[i];
               }
            }
         }
         }
      }
   }
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

   for (int i = 0; i < length; i++)
   {
      row = pixelRow[i];
      column = pixelCol[i];
      maxValue = pixelValue[i];
      rowIndexBegin = row - directionalDistance;
      rowIndexEnd = row + directionalDistance;
      colIndexBegin = column - directionalDistance;
      colIndexEnd = column + directionalDistance;

      if (rowIndexBegin < 0)
//      if ((rowIndexBegin ^ 1) < 0)
      {
         rowIndexBegin = 0;
      }
      if (colIndexBegin < 0)
//      if ((colIndexBegin ^ 1) < 0)
      {
         colIndexBegin = 0;
      }
      if (rowIndexEnd >= nRows)
      {
         rowIndexEnd = nRows - 1;
      }
      if (colIndexEnd >= nCols)
      {
         colIndexEnd = nCols - 1;
      }

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
}

void HxtChargedSharingGenerator::setChargedSharingMode(ChargedSharingMode chargedSharingMode)
{
   this->chargedSharingMode = chargedSharingMode;
}
