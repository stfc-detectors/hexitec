#include "hxtchargedsharinggenerator.h"
#include <Windows.h>
//
#include <QDebug>
#include <iostream>
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
//    qDebug() << "TESTING..";
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
   ///
//    std::cout << endl;

//    it = pixelEnergyMap->begin();
//    itend = pixelEnergyMap->end();
//    int value = 0;

//    int nonZeroCount = 0;
////    while (it != itend)
////    {
////        if (((nonZeroCount % 4) == 0) && (nonZeroCount != 0))
////            std::cout << endl;
////        pixel = it->first;
////        std::cout << "C[" << pixel << "] = " << it->second << " \t\t";
////        nonZeroCount++;
////        it++;
////    }
//    std::map <int, double> orderedMap(pixelEnergyMap->begin(), pixelEnergyMap->end());

//    for(auto it =  orderedMap.begin(); it != orderedMap.end(); ++it)
//    {
////        if (nonZeroCount > 100)
////            continue;
////        if (((nonZeroCount % 4) == 0) && (nonZeroCount != 0))
////            std::cout << endl;
//        pixel = it->first;
//        value = it->second;
//        if ((value > 639) && (value < 650))
//            std::cout << "C[" << pixel << "] = " << value << endl;
//        nonZeroCount++;
//    }
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

//   std::cout << "SORT AND DISPLAY\n";
//   int pixel = 0, value = 0, i  = 0, j = 0, offset = 0;
//   offset = 2820-800;
//   for (i = offset; i < (offset+(8*400)); )
//   {
//       for (j = 0; j < 5; j++)
//       {
//           ;
//       }
//       i = i + 400;
//   }

//   std::map<int, double> ordered(pixelEnergyMap->begin(), pixelEnergyMap->end());
//   for(auto it = ordered.begin(); it != ordered.end(); ++it)
//   {
//       pixel = it->first;
//       if ( (pixel > offset) && (pixel < (i+j)) )
//       {
//           std::cout << "[" << pixel << "] = " << it->second << endl;
//       }
//   }

//   for (int i = 0; i < 750; i++)
//   {
//       std::cout << "i" << i  << "\t row: " << pixelRow[i] << " \tcolumn: " << pixelCol[i] << " \tvalue: " << pixelValue[i] << endl;
//   }

   ///  DEBUGGING - Sort unordered map into a sorted map; Ditto associated arrays
//   int counting = 0/*, pixel1 = 0, pixel2 = 0, value1 = 0, value2 = 0*/;
//   int lastRow = -1, lastCol = -1;
//   int pixel, pixel2;
//   std::map<int, double> ordered(pixelEnergyMap->begin(), pixelEnergyMap->end());
//   int index = 0;
//   int target = 11749;
//   for(auto it = ordered.begin(); it != ordered.end(); ++it)
//   {
//      pixel = it->first;

//      if(isPixelInRange(pixel, target))
//      {
//         std::cout << "PF[" << pixel << "]  = \t" << it->second << "\t\t";
//      }
//      index++;
//   }
//   std::cout << endl << "CSD algorithm next" << endl;
//   int value = -1;
   ///

   for (int i = 0; i < length; i++)
   {
      row = pixelRow[i];
      column = pixelCol[i];
      maxValue = pixelValue[i];
//      std::cout << "i:" << i << " " << pixelRow[i] << "/" << pixelCol[i] << " = " << pixelValue[i] << endl;

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

      for (int j = i + 1; j < length; j++)
      {
         if ((pixelRow[j] >= rowIndexBegin) && (pixelRow[j] <= rowIndexEnd))
         {
            if ((pixelCol[j] >= colIndexBegin) && (pixelCol[j] <= colIndexEnd))
            {
               rowShare = pixelRow[j];
               columnShare = pixelCol[j];

//               /// Debug
//               pixel = row * nCols + column;
//               value = maxValue;
//               if (this->isPixelInRange( value, target))
//                  std::cout << "Pre-fix This[" << pixel << "]\t";
//               pixel2 = columnShare + (rowShare * nCols);
//               if (this->isPixelInRange( pixel2, target))
//                  std::cout << "Pre-fix T[" << pixel << "] has neighbour@ [" << pixel2 << "]\t\t";


//               if (( maxValue > 109) && (maxValue < 120))
//               {
//                   std::cout << "Pixel[" << (column + (400*row)) << "]:\t(" << row << "\t," << column << ")\t = " << maxValue << "\tdetected: \t(" << rowShare  << "\t," << columnShare << ").\n";
//                   if ( (lastRow != row) && (lastCol != column) )
//                   {
//                       lastRow = row;
//                       lastCol = column;
//                       counting++;
//                   }
//               }

//               std::cout << "i:" << i << "\tj:" << j << "\tCSD not havin' row/col: " << pixelRow[j] << "/" << pixelCol[j] << "\t = " << pixelValue[j] <<
//                                "\terasing: " << row * nCols + column << "\t " << rowShare * nCols + columnShare << " " << pixelEnergyMap->at(row * nCols + column)  << endl;
//               QThread::msleep(800);
               ///
               pixelEnergyMap->erase(row * nCols + column);
               pixelEnergyMap->erase(rowShare * nCols + columnShare);
            }
         }
      }

   }
//   std::cout << "Counted " << counting << " CSD corrections." << endl;
//   int nonZeroCount = 0, pxlRow = 0, pxlCol = 0;


//   std::map<int, double> corrected(pixelEnergyMap->begin(), pixelEnergyMap->end());
//   for(auto it = corrected.begin(); it != corrected.end(); ++it)
//   {
//       pixel = it->first;
//       value = it->second;
//       if ( (value > 109) && (value < (120)) )
//       {
//           pxlRow = (int) (pixel / 400);
//           pxlCol = (int) (pixel - (pxlRow * 400));
//           nonZeroCount++;
//           std::cout << "C[" << pixel << "] = \t" << it->second <<  "\ti.e. row: " << pxlRow << "\tcolumn: " << pxlCol << endl;
//       }
//   }
//   std::cout << "Counted " << nonZeroCount << " pixels after CSD correction." << endl;

}

/// DEBUGGING function:
bool HxtChargedSharingGenerator::isPixelInRange(int pixel, int target)
{
    bool bInRange = false;
    if ( (pixel > (target-1606)) && (pixel < (target-1594)) ||
         (pixel > (target-1206)) && (pixel < (target-1194)) ||
         (pixel > (target-806))  && (pixel < (target-794)) ||
         (pixel > (target-406))  && (pixel < (target-394)) ||
         (pixel > (target-6))    && (pixel < (target+6))   ||
         (pixel > (target+394))  && (pixel < (target+406)) ||
         (pixel > (target+794))  && (pixel < (target+806)) ||
         (pixel > (target+1194)) && (pixel < (target+1206))||
         (pixel > (target+1594)) && (pixel < (target+1406))   )
    {
        bInRange = true;
    }
    return bInRange;
}

void HxtChargedSharingGenerator::setChargedSharingMode(ChargedSharingMode chargedSharingMode)
{
   this->chargedSharingMode = chargedSharingMode;
}
