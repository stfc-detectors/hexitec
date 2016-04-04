/*
 * HxtFrameIncompleteDataCorrector.cpp
 *
 * This frame corrector implements the incomplete data corrector, where
 * events which don't return to baseline in the following frame are
 * corrected for.
 *
 *  Created on: 14 Dec 2010
 *      Author: tcn
 */

#include <iostream>

#include "HxtFrameIncompleteDataCorrector.h"

using namespace std;

namespace hexitech {

HxtFrameIncompleteDataCorrector::HxtFrameIncompleteDataCorrector() : HxtFrameCorrector(string("IncompleteData")) {

	// Nothing to do
}

HxtFrameIncompleteDataCorrector::~HxtFrameIncompleteDataCorrector() {

	// Nothing to do
}

/// apply - applies incomplete data corrector to the specified frames
/// @param apLastDecodedFrame ptr to previous decoded frame
/// @param apCurrentDecodedFrame ptr to current decoded frame
/// @return bool value indicating success of correction
bool HxtFrameIncompleteDataCorrector::apply(HxtDecodedFrame* apLastDecodedFrame, HxtDecodedFrame* apCurrentDecodedFrame) {

	// Do not apply this correction if currentDecodedFrame ptr is NULL; this occurs when last frame is processed
	// and is not relevant for the incomplete data correction
	if (apCurrentDecodedFrame == NULL) {
		if (mDebug) LOG(gLogConfig, logDEBUG2) << "Current frame ptr is NULL, no correction to apply";
		return true;
	}

	// Determine last and current frame indices from objects
	unsigned long long currentFrameIdx, lastFrameIdx;
	currentFrameIdx = apCurrentDecodedFrame->getFrameIndex();
	lastFrameIdx    = apLastDecodedFrame->getFrameIndex();

	if (mDebug) LOG(gLogConfig, logDEBUG1) << "Applying incomplete data corrector to current frame index " << currentFrameIdx << " last frame index " << lastFrameIdx;

	// If the two frames are not consecutive, do not apply the correction as there were no
	// incomplete events spanning those frames
	if (currentFrameIdx != lastFrameIdx + 1) {
		if (mDebug) LOG(gLogConfig, logDEBUG2) << "Current frame is not last frame + 1, no correction to apply";
		return true;
	}

	// Execute vector containing only hit pixels?
	if ( apLastDecodedFrame->getVectorStatus() )
	{
		unsigned int iRow;
		unsigned int iCol;

		// Loop over vector of hit pixel pairs, processing each in turn
		vector<hxtPixel>::iterator pixelListIterator;
		vector<hxtPixel> currentPixelList = apLastDecodedFrame->getPixelList();
		for (pixelListIterator = (currentPixelList.begin()); pixelListIterator != currentPixelList.end(); pixelListIterator++) {
			
			iRow = pixelListIterator->nRow;
			iCol = pixelListIterator->nCol;

			// Zero out event in current frame if same pixel had event in previous frame
			double lastValue = apLastDecodedFrame->getPixel(iRow, iCol);
			double currentValue = apCurrentDecodedFrame->getPixel(iRow, iCol);

			if ((lastValue != 0.0) && (currentValue != 0.0)) {
				if (mDebug) LOG(gLogConfig, logDEBUG2) << "Row " << iRow << " col " << iCol
														<< " incomplete data, last=" << lastValue
														<< " current=" << currentValue;
				apCurrentDecodedFrame->setPixel(iRow, iCol, 0.0);
				// Increment running total of number of corrections applied
				mEventsCorrected++;
			}	// if (lastValue)
		}	// for (..)
	}
	else	// Loop each pixel in frame
	{		
		const unsigned int nRows = apLastDecodedFrame->getRows();
		const unsigned int nCols = apLastDecodedFrame->getCols();

		// Iterate over rows and columns in frames
		for (unsigned int iRow = 0; iRow < nRows; iRow++) {
			for (unsigned int iCol = 0; iCol < nCols; iCol++) {

				// Zero out event in current frame if same pixel had event in previous frame
				double lastValue = apLastDecodedFrame->getPixel(iRow, iCol);
				double currentValue = apCurrentDecodedFrame->getPixel(iRow, iCol);

				if ((lastValue != 0.0) && (currentValue != 0.0)) {
					if (mDebug) LOG(gLogConfig, logDEBUG2) << "Row " << iRow << " col " << iCol
														   << " incomplete data, last=" << lastValue
														   << " current=" << currentValue;
					apCurrentDecodedFrame->setPixel(iRow, iCol, 0.0);
					// Increment running total of number of corrections applied
					mEventsCorrected++;
				}
			}	// for (iCol)
		}	// for (iRow)
	}	// End of non-vector code
	
	return true;
}

}
