/*
 * HxtFrameDoublePixelsCorrector.cpp
 *
 * This is NOT a real frame corrector; It is a tool for checking for frames that
 * contain any pixel that is read out twice within the very same frame.
 * It is not intended for general use and as such is not listed in the help file.
 * To use this "debug" info, supply the --DoublePixels flag
 *
 *  Created on: 19 Mar 2012
 *      Author: ckd
 */

#include "HxtFrameDoublePixelsCorrector.h"
#include <iomanip>

namespace hexitech {

HxtFrameDoublePixelsCorrector::HxtFrameDoublePixelsCorrector() : HxtFrameCorrector(string("DoublePixelsChecker..")){

	// Nothing to do
}

HxtFrameDoublePixelsCorrector::~HxtFrameDoublePixelsCorrector() {

	// Nothing to do
}

/// apply - applies charge sharing discrimination data corrected to specified frame
/// @param apLastDecodedFrame ptr to the previous decoded frame
/// @param apCurrentDecoded frame ptr to current decoded frame
/// @return bool value indicating success of correction
bool HxtFrameDoublePixelsCorrector::apply(HxtDecodedFrame* apLastDecodedFrame, HxtDecodedFrame* apCurrentDecodedFrame) {

    apCurrentDecodedFrame = apCurrentDecodedFrame;

	// Check each pixel in frame apLastDecodedFrame against vector of all pixels
	// read from file and flag any pixel that occurs more than once
    unsigned long long lastFrameIdx = 0;
	lastFrameIdx = apLastDecodedFrame->getFrameIndex();

	if (mDebug) LOG(gLogConfig, logDEBUG1) << "Applying double pixels checker corrector to frame index " << lastFrameIdx;

	const unsigned int nRows = apLastDecodedFrame->getRows();
	const unsigned int nCols = apLastDecodedFrame->getCols();

	// How many times same pixel hit
	unsigned int pixelHit = 0;

	// Vector variables
	vector<hxtPixel>::iterator pixelListIterator;
	vector<hxtPixel> lastPixelList = apLastDecodedFrame->getPixelList();

	// Iterate over rows and columns in frames
	for (unsigned int iRow = 0; iRow < nRows; iRow++) {
        for (unsigned int iCol = 0; iCol < nCols; iCol++) {

			double pixelValue = apLastDecodedFrame->getPixel(iRow, iCol);

			if ( pixelValue > 0.0 )
			{
				// Loop over the vector of hit pixels
				for (pixelListIterator = lastPixelList.begin(); pixelListIterator != lastPixelList.end();  pixelListIterator++)
				{
					// Check whether this row and column exists in vector
					// (In most cases each pixel should exist exactly once)
					if ( pixelListIterator->nRow == iRow ) {
						if ( pixelListIterator->nCol == iCol ) {
							pixelHit++;
						}
					}

				}	// for( pixelListIterator )
			}	// if ( pixelValue )

			// Was this pixel read out more than once?
			if ( pixelHit > 1 )
			{
				LOG(gLogConfig, logINFO) << "Frame " << lastFrameIdx << " Row " << iRow << " col " << iCol << " hit " << pixelHit << " times!";
				// Increment running total of number of "corrections" applied
				mEventsCorrected++;
			}

			// Clear pixelHit before next iteration
			pixelHit = 0;

		}	// for (iCol)
	}	// for (iRow)

	return true;
}

}
