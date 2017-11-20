/*
 * HxtFrameMomentumCorrector.cpp
 *
 * This frame "corrector" implements the Momentum Transfer Function
 *  by multiplying each pixel value with corresponding value contained in the momentum file
 *
 *  Created on: 25 Feb 15
 *      Author: ckd
 */

#include "HxtFrameMomentumCorrector.h"
#include <iostream>
#include <iomanip>

namespace hexitech {

HxtFrameMomentumCorrector::HxtFrameMomentumCorrector(HxtPixelThreshold* apMomentum, unsigned int aRows, unsigned int aCols) :
                                                            HxtFrameCorrector(string("Momentum")),
															mLastFrameProcessed(0),
															mRows(aRows),
															mCols(aCols),
															mPixels(aRows * aCols),
                                                            mMomentumValue(new double[mPixels])
{
	// Copy Gradients and Intercepts values into corresponding class' member variables
    for (unsigned int iPixel = 0; iPixel < mPixels; iPixel++)
    {
       mMomentumValue[iPixel] = apMomentum->getPixelThreshold(iPixel);
	}
}

HxtFrameMomentumCorrector::~HxtFrameMomentumCorrector() {

    delete [] mMomentumValue;
}


/// apply - applies calibration "correction" to apLastDecodedFrame's frame
/// @param apLastDecodedFrame ptr to the previous decoded frame
/// @param apCurrentDecoded frame ptr to current decoded frame
/// @return bool value indicating success of calibration
bool HxtFrameMomentumCorrector::apply(HxtDecodedFrame* apLastDecodedFrame, HxtDecodedFrame* apCurrentDecodedFrame) {

    // Apply Momentum after Calibration and Charge XX (CSA, CSD?) applied
    // (apCurrentDecodedFrame never used)
    apCurrentDecodedFrame = apCurrentDecodedFrame;

    unsigned long long lastFrameIdx;
	lastFrameIdx = apLastDecodedFrame->getFrameIndex();
	
	// Get rows and columns lengths
	const unsigned int nRows = apLastDecodedFrame->getRows();
	const unsigned int nCols = apLastDecodedFrame->getCols();

	// Current pixel to be interrogated
	double currentPixelValue = 0.0;
	double newPixelValue = 0.0;	

	if (mDebug) LOG(gLogConfig, logDEBUG1) << "Applying Calibration to frame index " << lastFrameIdx;
	
	mLastFrameProcessed = lastFrameIdx;

	// Iterate over rows and columns in frames
	for (unsigned int iRow = 0; iRow < nRows; iRow++) {

		for (unsigned int iCol = 0; iCol < nCols; iCol++) {
			
			currentPixelValue = apLastDecodedFrame->getPixel(iRow, iCol);
			
			if (currentPixelValue > 0.0)
			{
                // y = m*x
                newPixelValue = (mMomentumValue[pixelAddress(iRow, iCol)]) * currentPixelValue;

                // Update pixel with new value in [nnm^-1] unit
				apLastDecodedFrame->setPixel(iRow, iCol, newPixelValue);

				if (mDebug)
				{
					LOG(gLogConfig, logDEBUG2)  << "Frame " << apLastDecodedFrame->getFrameIndex() << " Pixel(" << setw(2) << iRow << 
						"," << setw(2) << iCol << ") was: " << setw(4) << currentPixelValue << ". y = " << setw(9) << 
                        mMomentumValue[pixelAddress(iRow, iCol)] << " * " << setw(4) << currentPixelValue;
				}

				// Increment running total of number of corrections applied
				mEventsCorrected++;
			}			
		}	// End of iCol loop		
	}	// End of iRow loop
	
	return true;
}

}
