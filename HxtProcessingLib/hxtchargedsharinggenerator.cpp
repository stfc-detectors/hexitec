#include "hxtchargedsharinggenerator.h"
#include <Windows.h>
// Used by debugging functions:
#include <iostream>

HxtChargedSharingGenerator::HxtChargedSharingGenerator(int nRows, int nCols, ProcessingDefinition *processingDefinition) :
   HxtGenerator(nRows, nCols, processingDefinition)
{
   this->nRows = nRows;
   this->nCols = nCols;

   chargedSharingMode = processingDefinition->getChargedSharingMode();
   setPixelGridSize(processingDefinition->getPixelGridSize());
}

void HxtChargedSharingGenerator::processEnergies(double *frame)
{
   calculateChargedSharing(frame);
   hxtItem->addFrameDataToHistogram(frame);
   incrementProcessedEnergyCount();
}

void HxtChargedSharingGenerator::setPixelGridSize(int pixelGridSize)
{
   this->pixelGridSize = pixelGridSize;
   directionalDistance = (int)pixelGridSize/2;
}

void HxtChargedSharingGenerator::calculateChargedSharing(double *frame)
{
    /// extendedFrame contains empty (1-2) pixel(s) on all 4 sides to enable charge sharing algorithm execution
    int sidePadding           = 2 *  directionalDistance;
    int extendedFrameRows    = (nRows + sidePadding);
    int extendedFrameColumns = (nCols + sidePadding);
    int extendedFrameSize    = extendedFrameRows * extendedFrameColumns;

    double  *extendedFrame;
    extendedFrame = (double*) malloc(extendedFrameSize * sizeof(double));
    memset(extendedFrame, 0, extendedFrameSize * sizeof(double));

    // Copy frame's each row into extendedFrame leaving (directionalDistance pixel(s)) padding on each side
    int startPosn = extendedFrameColumns * directionalDistance + directionalDistance;
    int endPosn   = extendedFrameSize - extendedFrameColumns;
    int increment = extendedFrameColumns;
    double *rowPtr = frame;

    for (int i = startPosn; i < endPosn; )
    {
       memcpy(&(extendedFrame[i]), rowPtr, nCols * sizeof(double));
       rowPtr = rowPtr + nCols;
       i = i + increment;
    }

    //// CSD example frame, with directionalDistance = 1
    ///
    ///      0    1    2    3  ...  399  400  401
    ///    402  403  404  405  ...  801  802  803
    ///    804  805  806  807  ... 1203 1204 1205
    ///   1206
    ///   1608 1609 1610 1611  ... 2007 2008 2009
    ///
    ///   Where frame's first row is 400 pixels from position 403 - 802,
    ///      second row is 805 - 1204, etc

    // This change to end position brings it just one element sooner than the Soleil fix on line 43
    //   but leaving this as is (path of least resistance)
    endPosn = extendedFrameSize - (extendedFrameColumns * directionalDistance) - directionalDistance;

    switch (chargedSharingMode)
    {
       case ADDITION:
          processAdditionRewritten(extendedFrame, extendedFrameRows, startPosn, endPosn);
          break;
       case DISCRIMINATION:
          processDiscriminationRewritten(extendedFrame, extendedFrameRows, startPosn, endPosn);
          break;
       default:
          break;
    }

    /// Copy CSD frame (i.e. 402x402) back into originally sized frame (400x400)
    rowPtr = frame;
    for (int i = startPosn; i < endPosn; )
    {
       memcpy(rowPtr, &(extendedFrame[i]), nCols * sizeof(double));
       rowPtr = rowPtr + nCols;
       i = i + increment;
    }

    free(extendedFrame);
    extendedFrame = NULL;
}

void HxtChargedSharingGenerator::processDiscriminationRewritten(double *extendedFrame, int extendedFrameRows, int startPosn, int endPosn)
{
    double *neighbourPixel = NULL, *currentPixel = extendedFrame;
    int rowIndexBegin = (-1*directionalDistance);
    int rowIndexEnd   = (directionalDistance+1);
    int colIndexBegin = rowIndexBegin;
    int colIndexEnd   = rowIndexEnd;
    bool bWipePixel = false;

    for (int i = startPosn; i < endPosn;  i++)
    {
       if (extendedFrame[i] != 0)
       {
          currentPixel = (&(extendedFrame[i]));       // Point at current (non-Zero) pixel

          for (int row = rowIndexBegin; row < rowIndexEnd; row++)
          {
             for (int column = colIndexBegin; column < colIndexEnd; column++)
             {

                if ((row == 0) && (column == 0)) // Don't compare pixel with itself
                   continue;

                neighbourPixel = (currentPixel + (extendedFrameRows*row)  + column);

                // Wipe this pixel if another neighbour was non-Zero
                if (bWipePixel)
                {
                    *neighbourPixel = 0;
                }
                else
                {
                   // Is this the first neighbouring, non-Zero pixel?
                   if (*neighbourPixel != 0)
                   {
                      // Yes; Wipe neighbour and current (non-zero) pixel
                      *neighbourPixel = 0;
                      *currentPixel = 0;
                      bWipePixel = true;
                   }
                }
             }
          }
          bWipePixel = false;
       }
    }
}

void HxtChargedSharingGenerator::processAdditionRewritten(double *extendedFrame, int extendedFrameRows, int startPosn, int endPosn)
{
    double *neighbourPixel = NULL, *currentPixel = extendedFrame;
    int rowIndexBegin = (-1*directionalDistance);
    int rowIndexEnd   = (directionalDistance+1);
    int colIndexBegin = rowIndexBegin;
    int colIndexEnd   = rowIndexEnd;
    double maxValue;

    for (int i = startPosn; i < endPosn;  i++)
    {
       if (extendedFrame[i] != 0)
       {
          maxValue = extendedFrame[i];
          currentPixel = (&(extendedFrame[i]));
          for (int row = rowIndexBegin; row < rowIndexEnd; row++)
          {
             for (int column = colIndexBegin; column < colIndexEnd; column++)
             {
                if ((row == 0) && (column == 0)) // Don't compare pixel with itself
                   continue;

                neighbourPixel = (currentPixel + (extendedFrameRows*row)  + column);
                if (*neighbourPixel != 0)
                {
                   if (*neighbourPixel > maxValue)
                   {
                      *neighbourPixel += extendedFrame[i];
                      maxValue = *neighbourPixel;
                      extendedFrame[i] = 0;
                   }
                   else
                   {
                       extendedFrame[i] += *neighbourPixel;
                       maxValue = extendedFrame[i];
                       *neighbourPixel = 0;
                   }
                }
             }
          }
       }
    }
}

void HxtChargedSharingGenerator::setChargedSharingMode(ChargedSharingMode chargedSharingMode)
{
   this->chargedSharingMode = chargedSharingMode;
}

/// DEBUGGING functions:
void HxtChargedSharingGenerator::showFrameSubset(uint16_t *frame, int offset)
{
    std::cout << "showFrameSubset() - Displaying pixels surrounding [" << offset << "]" << endl;
    offset -= 800;
    for (int i = offset; i < (offset+(8*400)); )
    {
        for (int j = 0; j < 5; j++)
        {
            std::cout << "\t!Frm[" << i+j << "]: \t" << frame[i+j] << " ";
        }
        std::cout << endl;
        i = i + 400;
    }
    std::cout << endl;
}

void HxtChargedSharingGenerator::showCsdFrameSubset(uint16_t  *extendedFrame, int offset)
{
    std::cout << "showCsdFrameSubset() - Displaying pixels surrounding [" << offset << "]" << endl;
    offset -= 804;
    for (int i = offset; i < (offset+(8*402)); )
    {
        for (int j = 0; j < 5; j++)
        {
            std::cout << "\t!CSD[" << i+j << "]: \t" << extendedFrame[i+j] << " ";
        }
        std::cout << endl;
        i = i + 402;
    }
}

void HxtChargedSharingGenerator::showCsdFrameBinContents(int extendedFrameSize, uint16_t *extendedFrame, int bin)
{
   int nonZeroCount = 0, value = 0;
   int binMin = bin - 1;
   int binMax = bin + 10;
   std::cout << "showCsdFrameBinContents() for bin " << bin << " (i.e. " << binMin << "-" << binMax << ")." << endl;

   for (int i = 0; i < extendedFrameSize; i++)
   {
       value = extendedFrame[i];
       if ( (value > binMin) && (value < (binMax)) )
       {
           std::cout << "\tCSD[" << i << "]: \t" << extendedFrame[i] << " ";
           if (nonZeroCount%5 == 0)
               std::cout << endl;
           nonZeroCount++;
        }
   }
   std::cout << "\n That's " << nonZeroCount << " hits between 110-120." << endl;
}

void HxtChargedSharingGenerator::showFrameBinContents(int frameSize, uint16_t *frame, int bin)
{
   int nonZeroCount = 0;
   int binMin = bin - 1;
   int binMax = bin + 10;
   std::cout << "showFrameBinContents() for bin " << bin << " (i.e. " << binMin << "-" << binMax << ")." << endl;

   for (int i = 0; i < frameSize; i++)
   {
       if ((frame[i] > binMin) && (frame[i] < binMax))
       {
           std::cout << "C[" << i << "] = " << frame[i] << endl;
           nonZeroCount++;
       }
   }
   std::cout << "\n That's " << nonZeroCount << " hits between 110-120." << endl;
}
