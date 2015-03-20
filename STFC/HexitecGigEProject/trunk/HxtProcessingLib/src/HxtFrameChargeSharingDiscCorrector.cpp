/*
 * HxtFrameChargeSharingDiscCorrector.cpp
 *
 * This frame corrector implements charge sharing discrimination, where pixels
 * which have non-zero values in surrounding pixels are zeroed, and the same
 * pixels are zeroed in the current and following frame
 *
 *  Created on: 6 Jan 2011
 *      Author: tcn
 */

#include "HxtFrameChargeSharingDiscCorrector.h"

namespace hexitech {

HxtFrameChargeSharingDiscCorrector::HxtFrameChargeSharingDiscCorrector() : HxtFrameCorrector(string("ChargeSharingDiscrimination")){

	// Nothing to do
}

HxtFrameChargeSharingDiscCorrector::~HxtFrameChargeSharingDiscCorrector() {

	// Nothing to do
}

/// apply - applies charge sharing discrimination data corrected to specified frame
/// @param apLastDecodedFrame ptr to the previous decoded frame
/// @param apCurrentDecoded frame ptr to current decoded frame
/// @param apSubPixelFrame Disregard, Not used in this class
/// @return bool value indicating success of correction
bool HxtFrameChargeSharingDiscCorrector::apply(HxtDecodedFrame* apLastDecodedFrame, HxtDecodedFrame* apCurrentDecodedFrame, 
												HxtFrame* apSubPixelFrame) {
	
	// Determine last and current frame indices from objects - current decoded frame ptr can be null at end
	// of processing (i.e. on last frame), in which case we still apply the correction to the last frame but
	// there is no forward rejection of pixel into current frame
    apSubPixelFrame = 0;

	unsigned long long currentFrameIdx = 0, lastFrameIdx = 0;
	lastFrameIdx = apLastDecodedFrame->getFrameIndex();
	if (apCurrentDecodedFrame != NULL) {
		currentFrameIdx = apCurrentDecodedFrame->getFrameIndex();
	}

	if (mDebug) LOG(gLogConfig, logDEBUG1) << "Applying charge sharing discrimination data corrector to current frame index " << currentFrameIdx <<
			                                  " last frame index" << lastFrameIdx;

	bool consecutiveFrames = (currentFrameIdx == lastFrameIdx + 1);

	// Vector mode? (Only examine hit pixels)
	if (apLastDecodedFrame->getVectorStatus())
	{
		unsigned int iRow;
		unsigned int iCol;

		// Loop over a vector of hit pixels
		vector<hxtPixel>::iterator pixelListIterator;
		vector<hxtPixel> lastPixelList = apLastDecodedFrame->getPixelList();
		for (pixelListIterator = lastPixelList.begin(); pixelListIterator != lastPixelList.end();  pixelListIterator++)
		{
			iRow = pixelListIterator->nRow;
			iCol = pixelListIterator->nCol;

			// Get pixel value and sum of adjacent pixels
			double pixelValue = apLastDecodedFrame->getPixel(iRow, iCol);
			double adjacentSum = apLastDecodedFrame->getAdjacent(iRow, iCol);

			// If pixel and adjacent sums are non-zero, clear them to reject event
			if ((pixelValue != 0.0) && (adjacentSum != 0.0)) {
				if (mDebug) LOG(gLogConfig, logDEBUG2) << "Row " << iRow << " col " << iCol
														<< " charge sharing, pixel value = " << pixelValue
														<< " adjacent sum = " << adjacentSum;
				apLastDecodedFrame->setPixel(iRow, iCol, 0.0);
				apLastDecodedFrame->clearAdjacent(iRow, iCol);
	
				// If current frame is consecutive to last frame, also clear pixels in current
				if (consecutiveFrames) {
					apCurrentDecodedFrame->setPixel(iRow, iCol, 0.0);
					apCurrentDecodedFrame->clearAdjacent(iRow, iCol);
				}
				
				// Increment running total of number of corrections applied
				mEventsCorrected++;
			}	// if (pixelValue)
		}	// for(..)
	}
	else	// Normal mode, examine every pixel in each frame
	{
		const unsigned int nRows = apLastDecodedFrame->getRows();
		const unsigned int nCols = apLastDecodedFrame->getCols();

		// Iterate over rows and columns in frames
		for (unsigned int iRow = 0; iRow < nRows; iRow++) {
			for (unsigned int iCol = 0; iCol < nCols; iCol++) {

				// Get pixel value and sum of adjacent pixels
				double pixelValue = apLastDecodedFrame->getPixel(iRow, iCol);
				double adjacentSum = apLastDecodedFrame->getAdjacent(iRow, iCol);

				// If pixel and adjacent sums are non-zero, clear them to reject event
				if ((pixelValue != 0.0) && (adjacentSum != 0.0)) {
					if (mDebug) LOG(gLogConfig, logDEBUG2) << "Row " << iRow << " col " << iCol
														   << " charge sharing, pixel value = " << pixelValue
														   << " adjacent sum = " << adjacentSum;
					
					apLastDecodedFrame->setPixel(iRow, iCol, 0.0);
					apLastDecodedFrame->clearAdjacent(iRow, iCol);					

					// If current frame is consecutive to last frame, also clear pixels in current
					if (consecutiveFrames) {
						apCurrentDecodedFrame->setPixel(iRow, iCol, 0.0);
						apCurrentDecodedFrame->clearAdjacent(iRow, iCol);
					}

					// Increment running total of number of corrections applied
					mEventsCorrected++;
				}	// if (pixelValue)
			}	// for (iCol)
		}	// for (iRow)
	}
	return true;
}

}
