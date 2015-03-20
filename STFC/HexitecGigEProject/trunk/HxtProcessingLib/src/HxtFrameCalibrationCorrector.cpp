/*
 * HxtFrameCalibrationCorrector.cpp
 *
 * This frame "corrector" implements calibration using the equation
 *		 y = m*x + c
 * where y = keV
 *		 m = Gradients value taken from gGradientsFile (see appMain.cpp)
 *		 x = ADu (adc value) taken from .dat raw file
 *		 c = Intercepts value taken from gInterceptsFile (see appMain.cpp)
 *
 *  Created on: 6 Mar 12
 *      Author: ckd
 */

#include "HxtFrameCalibrationCorrector.h"
#include <iostream>
#include <iomanip>

namespace hexitech {

HxtFrameCalibrationCorrector::HxtFrameCalibrationCorrector(HxtPixelThreshold* apGradients, HxtPixelThreshold* apIntercepts,
															unsigned int aRows, unsigned int aCols) : 
															HxtFrameCorrector(string("Calibration")),
															mLastFrameProcessed(0),
															mRows(aRows),
															mCols(aCols),
															mPixels(aRows * aCols),
															mGradientValue(new double[mPixels]),
															mInterceptValue(new double[mPixels])
{
	// Copy Gradients and Intercepts values into corresponding class' member variables
	for (unsigned int iPixel = 0; iPixel < mPixels; iPixel++) {
		
		mGradientValue[iPixel] = apGradients->getPixelThreshold(iPixel);
		mInterceptValue[iPixel] = apIntercepts->getPixelThreshold(iPixel);
	}
}

HxtFrameCalibrationCorrector::~HxtFrameCalibrationCorrector() {

	delete [] mGradientValue;
	delete [] mInterceptValue;
}


/// apply - applies calibration "correction" to apLastDecodedFrame's frame
/// @param apLastDecodedFrame ptr to the previous decoded frame
/// @param apCurrentDecoded frame ptr to current decoded frame
/// @param apSubPixelFrame frame ptr to previous subpixel frame
/// @return bool value indicating success of calibration
bool HxtFrameCalibrationCorrector::apply(HxtDecodedFrame* apLastDecodedFrame, HxtDecodedFrame* apCurrentDecodedFrame, 
													HxtFrame* apSubPixelFrame) {

	// Calibrate according to equation y = m*x + c
	// where m = mGradientValue
	//		 x = apLastDecodedFrame->getPixel();
	// 		 c = mInterceptValue
	//		 y = measured in keV
	// (apCurrentDecodedFrame and apSubPixelFrame never used)
    apCurrentDecodedFrame = 0;
    apSubPixelFrame = 0;

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
				// y = m*x + c
				newPixelValue = (mGradientValue[pixelAddress(iRow, iCol)]) * currentPixelValue + (mInterceptValue[pixelAddress(iRow, iCol)]);

				// Update pixel with new value in [keV] unit
				apLastDecodedFrame->setPixel(iRow, iCol, newPixelValue);

				if (mDebug)
				{
					LOG(gLogConfig, logDEBUG2)  << "Frame " << apLastDecodedFrame->getFrameIndex() << " Pixel(" << setw(2) << iRow << 
						"," << setw(2) << iCol << ") was: " << setw(4) << currentPixelValue << ". y = " << setw(9) << 
						mGradientValue[pixelAddress(iRow, iCol)] << " * " <<
						setw(4) << currentPixelValue << " + " << setw(9) <<	
						mInterceptValue[pixelAddress(iRow, iCol)] << " = " <<  setw(9) <<
						apLastDecodedFrame->getPixel(iRow, iCol);
				}

				// Increment running total of number of corrections applied
				mEventsCorrected++;
			}			
		}	// End of iCol loop		
	}	// End of iRow loop
	
	return true;
}

}
