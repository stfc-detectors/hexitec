/*
 * HxtFrameInterpolateCorrector.cpp
 *
 * This frame corrector implements Interpolation where a dead pixel's
 * value is replaced by the average of its surrounding neighbours' values
 *
 *  Created on: 1 Mar 12
 *      Author: ckd
 */

#include "HxtFrameInterpolateCorrector.h"
#include <iostream>
#include <iomanip>

namespace hexitech {

HxtFrameInterpolateCorrector::HxtFrameInterpolateCorrector() : 
															HxtFrameCorrector(string("Interpolate")),
															mLastFrameProcessed(0)
{
	// Nothing to do
}

HxtFrameInterpolateCorrector::~HxtFrameInterpolateCorrector() {

	// Nothing to do
}

/// apply - applies interpolate correction to specified frame
/// @param apLastDecodedFrame ptr to the previous decoded frame
/// @param apCurrentDecoded frame ptr to current decoded frame
/// @return bool value indicating success of correction
bool HxtFrameInterpolateCorrector::apply(HxtDecodedFrame* apLastDecodedFrame, HxtDecodedFrame* apCurrentDecodedFrame) {

	// Check if each hit pixel in current frame induces noise in the following column, starting at the same row
	// until the end of that column, then check same column in the next frame from the first row until the same 
	// row as hit pixel.
    // (apCurrentDecodedFrame never used)
    apCurrentDecodedFrame = apCurrentDecodedFrame;

    unsigned long long lastFrameIdx;
	lastFrameIdx = apLastDecodedFrame->getFrameIndex();
	
	// Current pixel to be interrogated
	double currentPixelValue = 0.0;
    double neighbourAverage;

	if (mDebug) LOG(gLogConfig, logDEBUG1) << "Applying Induced Noise corrector to (lastFrameIdx) frame index " << lastFrameIdx;
	
	mLastFrameProcessed = lastFrameIdx;

	// Testing purposes only
	//unsigned int frame0blanks = 0;

	// Make copy of frame content
	apLastDecodedFrame->copyPixels();

	// Get rows and columns lengths
	const unsigned int nRows = apLastDecodedFrame->getRows();
	const unsigned int nCols = apLastDecodedFrame->getCols();

	// Iterate over rows and columns in frames
	for (unsigned int iRow = 0; iRow < nRows; iRow++)
	{
		for (unsigned int iCol = 0; iCol < nCols; iCol++)
		{
			/*currentPixelValue = apLastDecodedFrame->getPixel(iCol, iRow);*/
			currentPixelValue = apLastDecodedFrame->getPixelCopy(iCol, iRow);

			// Is current pixel zero?
			if (currentPixelValue == 0)
			{								
				// Calculate average of all nonzero neighbours
				neighbourAverage = this->calculateNeighboursAverage(apLastDecodedFrame, iRow, iCol);

				// Only update pixel if sum of its' neighbours aren't zero
				if ( neighbourAverage > 0.0 )
				{
					// Set pixel to (copy's) neighbours' average
					apLastDecodedFrame->setPixel(iCol, iRow, neighbourAverage);

					if (mDebug)
					{
						LOG(gLogConfig, logDEBUG2)  << "Frame " << apLastDecodedFrame->getFrameIndex() << " Pixel(" << setw(2) << iCol << 
							"," << setw(2) << iRow << ")'s neighbours values: " << setw(4) << apLastDecodedFrame->getPixelCopy(iCol-1, iRow-1) << ", " << 
							setw(4) << apLastDecodedFrame->getPixelCopy(iCol-1, iRow) << ", " << 
							setw(4) << apLastDecodedFrame->getPixelCopy(iCol-1, iRow+1) << ", " << 				
							setw(4) << apLastDecodedFrame->getPixelCopy(iCol, iRow-1) << ", " << 
							setw(4) << apLastDecodedFrame->getPixelCopy(iCol, iRow+1) << ", " << 
							setw(4) << apLastDecodedFrame->getPixelCopy(iCol+1, iRow-1) << ", " << 
							setw(4) << apLastDecodedFrame->getPixelCopy(iCol+1, iRow) << ", " << 
							setw(4) << apLastDecodedFrame->getPixelCopy(iCol+1, iRow+1) << " and average: " << neighbourAverage;
					}

					// Increment running total of number of corrections applied
					mEventsCorrected++;
				}

            }	// End of currentPixelValue > 0
		}	// End of iCol loop
	}	// End of iRow loop

	return true;
}

double HxtFrameInterpolateCorrector::calculateNeighboursAverage(HxtDecodedFrame* apLastDecodedFrame, unsigned int aRow, unsigned int aCol)
{
	double neighbourValue = 0.0;
	double neighbourTotal = 0.0;			// Sum of non-zero neighbours
	unsigned int nonzeroNeighbourCount = 0;	// Counts how many of neighbours are not zero

	neighbourValue = apLastDecodedFrame->getPixelCopy(aCol-1, aRow-1);
	if ( neighbourValue > 0.0 ) {
		neighbourTotal += neighbourValue;
		nonzeroNeighbourCount++;
	}

	neighbourValue = apLastDecodedFrame->getPixelCopy(aCol-1, aRow);
	if ( neighbourValue > 0.0 ) {
		neighbourTotal += neighbourValue;
		nonzeroNeighbourCount++;
	}

	neighbourValue = apLastDecodedFrame->getPixelCopy(aCol-1, aRow+1);
	if ( neighbourValue > 0.0 ) {
		neighbourTotal += neighbourValue;
		nonzeroNeighbourCount++;
	}
	
	neighbourValue = apLastDecodedFrame->getPixelCopy(aCol, aRow-1);
	if ( neighbourValue > 0.0 ) {
		neighbourTotal += neighbourValue;
		nonzeroNeighbourCount++;
	}

	neighbourValue = apLastDecodedFrame->getPixelCopy(aCol, aRow+1);
	if ( neighbourValue > 0.0 ) {
		neighbourTotal += neighbourValue;
		nonzeroNeighbourCount++;
	}

	neighbourValue = apLastDecodedFrame->getPixelCopy(aCol+1, aRow-1);
	if ( neighbourValue > 0.0 ) {
		neighbourTotal += neighbourValue;
		nonzeroNeighbourCount++;
	}

	neighbourValue = apLastDecodedFrame->getPixelCopy(aCol+1, aRow);
	if ( neighbourValue > 0.0 ) {
		neighbourTotal += neighbourValue;
		nonzeroNeighbourCount++;
	}

	neighbourValue = apLastDecodedFrame->getPixelCopy(aCol+1, aRow+1);
	if ( neighbourValue >0.0 ) {
		neighbourTotal += neighbourValue;
		nonzeroNeighbourCount++;
	}

	// Avoid division with zero
	if ( nonzeroNeighbourCount == 0 ) return 0;
	// Only go ahead if four or more neighbours are actually populated
	if ( 1)//nonzeroNeighbourCount >= 4 )
		return (neighbourTotal / nonzeroNeighbourCount);
	else
		return 0;
}

}
