/*
 * HxtFrameInducedNoiseCorrector.cpp
 *
 * This frame corrector implements induced noise correction, where
 * a hit pixel may generate noise in the column immediately following it
 * and possibly also in the same column in the following frame.
 *
 *		current				next frame
 *		---------- 			----------
 *		|00000000|  		|00200000|
 *		|00000000| 			|00200000|
 *		|05200000|			|00000000|
 *		|00200000|	 		|00000000|
 *		|00200000| 			|00000000|
 *		|00200000| 			|00000000|
 *		----------			----------
 *		
 * 5 = The pixel that's hit
 * 2 = Induced noise
 * 0 = empty pixel
 *
 *  Created on: 29 Feb 12
 *      Author: ckd
 */

#include "HxtFrameInducedNoiseCorrector.h"
#include <iostream>
#include <iomanip>

namespace hexitech {

HxtFrameInducedNoiseCorrector::HxtFrameInducedNoiseCorrector(double aInducedNoiseThreshold) : 
															HxtFrameCorrector(string("InducedNoise")),
															mLastFrameProcessed(0),
															mInducedNoiseThreshold(aInducedNoiseThreshold),
															mNoiseCurrentFrame(0),
															mNoiseNextFrame(0)
{
	// Nothing to do
}

HxtFrameInducedNoiseCorrector::~HxtFrameInducedNoiseCorrector() {

	// Nothing to do
}

/// apply - applies induced noise correction to specified frame
/// @param apLastDecodedFrame ptr to the previous decoded frame
/// @param apCurrentDecoded frame ptr to current decoded frame
/// @return bool value indicating success of correction
bool HxtFrameInducedNoiseCorrector::apply(HxtDecodedFrame* apLastDecodedFrame, HxtDecodedFrame* apCurrentDecodedFrame) {

	// Check if each hit pixel in current frame induces noise in the following column, starting at the same row
	// until the end of that column, then check same column in the next frame from the first row until the same 
	// row as hit pixel.

	// Check whether we encountered last frame
	bool bCurrentFrame = (apCurrentDecodedFrame != NULL);
	
    unsigned long long lastFrameIdx;
	lastFrameIdx = apLastDecodedFrame->getFrameIndex();

	// Current pixel to be interrogated
	double currentPixelValue = 0.0;
	double inducedPixelValue = 0.0;

	if (mDebug) LOG(gLogConfig, logDEBUG1) << "Applying Induced Noise corrector to (lastFrameIdx) frame index " << lastFrameIdx;
	
	mLastFrameProcessed = lastFrameIdx;

	// Get rows and columns lengths
	const unsigned int nRows = apLastDecodedFrame->getRows();
	const unsigned int nCols = apLastDecodedFrame->getCols();

	if ( apLastDecodedFrame->getVectorStatus() )
	{
		unsigned int iRow;
		unsigned int iCol;

		vector<hxtPixel>::iterator pixelListIterator;
		vector<hxtPixel> currentPixelList = apLastDecodedFrame->getPixelList();
		for (pixelListIterator = currentPixelList.begin(); pixelListIterator != currentPixelList.end(); pixelListIterator++) {

			iRow = pixelListIterator->nRow;
			iCol = pixelListIterator->nCol;

			currentPixelValue = apLastDecodedFrame->getPixel(iRow, iCol);
			
			// Is current pixel hit?
			if (currentPixelValue > 0.0) {
				
				// Check THIS frame's pixels in next column, starting with same row until last row
				for ( unsigned int i = 0; i < (nRows-iRow); i++)
				{
					// Grab pixel in next column
					inducedPixelValue = apLastDecodedFrame->getPixel(iRow+i, iCol+1);

					// Is inducedPixelValue above zero but below threshold of currentPixelValue?
					if ( (inducedPixelValue > 0.0) && ( inducedPixelValue < (mInducedNoiseThreshold * currentPixelValue) ) )
					{
						// This pixel contains induced noise, clear it
						apLastDecodedFrame->setPixel(iRow+i, iCol+1, 0.0);
						mNoiseCurrentFrame++;
					}
				}

				if (bCurrentFrame)
				{
					// Check NEXT frame's pixels in next column, from the first row to penultimate row 
					// before currentPixelValue's row
					for ( unsigned int i = 0; i < iRow; i++)
					{
						// Grab pixel in next column
						inducedPixelValue = apCurrentDecodedFrame->getPixel(i, iCol+1);

						// Is inducedPixelValue above zero but below threshold of currentPixelValue?
						if ( (inducedPixelValue > 0.0) && ( inducedPixelValue < (mInducedNoiseThreshold * currentPixelValue) ) )
						{
							// This pixel contains induced noise, clear it
							apCurrentDecodedFrame->setPixel(i, iCol+1, 0.0);
							mNoiseNextFrame++;
						}
					}
				}								

				if (mDebug) {
					if ( (mNoiseCurrentFrame > 0) || (mNoiseNextFrame > 0) )
					{
						LOG(gLogConfig, logDEBUG2)  << "Frame " << apLastDecodedFrame->getFrameIndex() << " Pixel(" << setw(2) << iRow << 
							"," << setw(2) << iCol << ") = " << currentPixelValue << " had induced " << setw(3) << 
							mNoiseCurrentFrame + mNoiseNextFrame  << " noisy pixels";
					}
				}

				// Increment running total of number of corrections applied
				mEventsCorrected++;

				// Reset both counters before checking the next hit pixel
				mNoiseCurrentFrame = 0;
				mNoiseNextFrame = 0;

			} // if (currentPixelValue)
		} // for (pixelListIterator)
	}
	else
	{
		// Iterate over rows and columns in frames
		for (unsigned int iRow = 0; iRow < nRows; iRow++) {
			for (unsigned int iCol = 0; iCol < nCols; iCol++) {			
			
				currentPixelValue = apLastDecodedFrame->getPixel(iRow, iCol);

				if (currentPixelValue > 0.0) {

					// Check THIS frame's pixels in next column, starting with same row until last row
					for ( unsigned int i = 0; i < (nRows-iRow); i++)
					{
						// Grab pixel in next column
						inducedPixelValue = apLastDecodedFrame->getPixel(iRow+i, iCol+1);

						// Is inducedPixelValue above zero but below threshold of currentPixelValue?
						if ( (inducedPixelValue > 0.0) && ( inducedPixelValue < (mInducedNoiseThreshold * currentPixelValue) ) )
						{
							// This pixel contains induced noise, clear it
							apLastDecodedFrame->setPixel(iRow+i, iCol+1, 0.0);
							mNoiseCurrentFrame++;
						}
					}

					if (bCurrentFrame)
					{
						// Check NEXT frame's pixels in next column, from the first row to penultimate row 
						// before currentPixelValue's row
						for ( unsigned int i = 0; i < iRow; i++)
						{
							// Grab pixel in next column
							inducedPixelValue = apCurrentDecodedFrame->getPixel(i, iCol+1);

							// Is inducedPixelValue above zero but below threshold of currentPixelValue?
							if ( (inducedPixelValue > 0.0) && ( inducedPixelValue < (mInducedNoiseThreshold * currentPixelValue) ) )
							{
								// This pixel contains induced noise, clear it
								apCurrentDecodedFrame->setPixel(i, iCol+1, 0.0);
								mNoiseNextFrame++;
							}
						}
					}								

					if (mDebug) {
						if ( (mNoiseCurrentFrame > 0) || (mNoiseNextFrame > 0) )
						{
							LOG(gLogConfig, logDEBUG2)  << "Frame " << apLastDecodedFrame->getFrameIndex() << " Pixel(" << setw(2) << iRow << 
								"," << setw(2) << iCol << ") = " << currentPixelValue << " had induced " << setw(3) << 
								mNoiseCurrentFrame + mNoiseNextFrame  << " noisy pixels";
						}
					}

					// Increment running total of number of corrections applied
					mEventsCorrected++;

					// Reset both counters before checking the next hit pixel
					mNoiseCurrentFrame = 0;
					mNoiseNextFrame = 0;

				}	// End of currentPixelValue > 0.0
			}	// End of iCol loop
		}	// End of iRow loop

	} // End of if/else (vector)

	return true;
}

}
