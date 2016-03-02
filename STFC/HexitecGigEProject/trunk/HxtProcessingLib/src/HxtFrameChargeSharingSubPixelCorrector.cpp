/*
 * HxtFrameChargeSharingSubPixelCorrector.cpp
 *
 * This frame corrector implements charge sharing addition, where pixels
 * which have non-zero values in surrounding pixels (2x2 matrix) are summed
 * and moved to the "winner pixel" while neighbours are zeroed.
 * e.g. 2x2: becomes 2x2:
 *   50 20			100 0
 *   20 10			  0 0
 * It then creates a 3x3 matrix for each  original pixel, i.e. a
 * 240x240 matrix
 *
 *  Created on: 13 Feb 12
 *      Author: ckd
 */

#include "HxtFrameChargeSharingSubPixelCorrector.h"
#include <algorithm>

namespace hexitech {

HxtFrameChargeSharingSubPixelCorrector::HxtFrameChargeSharingSubPixelCorrector() :
																			HxtFrameCorrector(string("ChargeSharingAdditionSubPixel")),
																			mLastFrameProcessed(0)																			
{
	// Nothing to do
}

HxtFrameChargeSharingSubPixelCorrector::~HxtFrameChargeSharingSubPixelCorrector() {

	// Nothing to do
}

/// apply - applies charge sharing addition correction (CSA) algorithm to specified frame
/// @param apLastDecodedFrame ptr to the previous decoded frame
/// @param apCurrentDecoded frame ptr to current decoded frame
/// @param apSubPixelFrame frame ptr to previous subpixel frame
/// @return bool value indicating success of correction
bool HxtFrameChargeSharingSubPixelCorrector::apply(HxtDecodedFrame* apLastDecodedFrame, HxtDecodedFrame* apCurrentDecodedFrame, 
													HxtFrame* apSubPixelFrame) {

	// Examine apLastDecodedFrame for hits; Any hit examines local 2x2 matrix for charge sharing and were found,
	// move all charges on to pixel containing highest initial charge. This charge is also copied into
	// the 240x240 matrix (apSubPixelFrame) according to subpixel algorithm
	// (apCurrentDecodedFrame pointer never used)
    apCurrentDecodedFrame = apCurrentDecodedFrame;

	if ( apSubPixelFrame == NULL )
	{
		LOG(gLogConfig, logERROR) << "Charge Sharing Addition Error: apSubPixelFrame argument not defined!";
		return false;
	}

    unsigned long long lastFrameIdx;
	lastFrameIdx = apLastDecodedFrame->getFrameIndex();
	
	if ( (lastFrameIdx <= apSubPixelFrame->getFrameIndex()) && (lastFrameIdx !=0))
	{
//		LOG(gLogConfig, logINFO) << "Already processed frame " << apSubPixelFrame->getFrameIndex();
		return false;
	}

	// Set SubPixel frame number to apLastDecodedFrame's
	apSubPixelFrame->setFrameIndex((lastFrameIdx));

	// Get rows and columns lengths
	const unsigned int nRows = apLastDecodedFrame->getRows();
	const unsigned int nCols = apLastDecodedFrame->getCols();

	// Current pixel to be interrogated
	double currentPixelValue = 0.0;
	// Sum of pixel values in 2x2 pixel matrix
	double sumOfFourPixels = 0.0;

	// Create array of four indexes, one for each pixel in 2x2 matrix
	double arPixelValues[4];
	for ( int i = 0; i < 4; i++ )
	{
		arPixelValues[i] = 0;
		if (mDebug) LOG(gLogConfig, logDEBUG2) << "CSSPxl::apply() arPixelValues[" << i << "] = " << arPixelValues[i];
	}

	// Looking for the three neighbouring pixels, 
	// forming a 2x2 matrix:
	//		-------
	//		|0*| 1|
	//		-------
	//		|2 | 3|
	//		-------
	// * = The pixel that's hit
	
	int winnerIdx = -1;

	if (mDebug) LOG(gLogConfig, logDEBUG1) << "Applying charge sharing addition data corrector to (lastFrameIdx) frame index " << lastFrameIdx;

	// Note frame number of the frame being processed
	mLastFrameProcessed = lastFrameIdx;

	// Make copy of pixels in frame
	apLastDecodedFrame->copyPixels();

	// Normal mode only - vector mode slower because
	// algorithm requires ordering of vector elements
	
	// Iterate over rows and columns in frames
	for (unsigned int iCol = 0; iCol < nCols; iCol++) {

		for (unsigned int iRow = 0; iRow < nRows; iRow++) {
			
			// Get pixel values from copy of frame
			currentPixelValue = apLastDecodedFrame->getPixelCopy(iRow, iCol);
			
			// Is current pixel hit?
			if (currentPixelValue > 0.0) {				

				arPixelValues[0] = currentPixelValue;
				// Current pixel hit, grab its three neighbours..
				arPixelValues[1] = apLastDecodedFrame->getPixelCopy(iRow, iCol+1);
				arPixelValues[2] = apLastDecodedFrame->getPixelCopy(iRow+1, iCol);
				arPixelValues[3] = apLastDecodedFrame->getPixelCopy(iRow+1, iCol+1);

				sumOfFourPixels = this->sumFourPixels(arPixelValues);

				// Pixel hit; case A, B, C or D?
				if ( arPixelValues[3] > 0.0) {

					// Case A, all four pixels hit
					this->winnerPixel(arPixelValues, winnerIdx);

					// 240x240 frame: Update SubPixel frame with total charge
					this->updateSubPixelFrameCaseA(apSubPixelFrame, iRow, iCol, winnerIdx, sumOfFourPixels);
										
					// 80x80 frame: Set winning pixel to total charge and clear the other three pixels
					this->updateDecodedFrame(iRow, iCol, sumOfFourPixels, winnerIdx, apLastDecodedFrame);		
					mCaseA++;
				}
				else if ( arPixelValues[2] > 0.0 ) {

					// Case B, same column pixels hit
					// (or possible case A but only three hit pixels)
					this->winnerPixel(arPixelValues, winnerIdx);

					// If index 1 is the winner of then we actually have a case A scenario
					//	but the pixel at index 3's charge fell below that pixel's threshold
					if ( winnerIdx == 1 )
					{
						// Case A (three hits pixels), 240x240 frame: Update SubPixel frame with total charge
						this->updateSubPixelFrameCaseA(apSubPixelFrame, iRow, iCol, winnerIdx, sumOfFourPixels);
										
						// 80x80 frame: Set winning pixel to total charge and clear the other three pixels
						this->updateDecodedFrame(iRow, iCol, sumOfFourPixels, winnerIdx, apLastDecodedFrame);		
						mCaseA++;
					}
					else
					{
						// Case B; 240x240 frame: Update SubPixel frame with total charge
						this->updateSubPixelFrameCaseB(apSubPixelFrame, iRow, iCol, winnerIdx, sumOfFourPixels);

						// 80x80 frame: Set winning pixel to total charge and clear the other three pixels
						this->updateDecodedFrame(iRow, iCol, sumOfFourPixels, winnerIdx, apLastDecodedFrame);
						mCaseB++;
					}
				}
				else if ( arPixelValues[1] > 0.0 ) {
		
					// Case C, same row pixels hit
					this->winnerPixel(arPixelValues, winnerIdx);

					// 240x240 frame: Update SubPixel frame with total charge
					this->updateSubPixelFrameCaseC(apSubPixelFrame, iRow, iCol, winnerIdx, sumOfFourPixels);
					
					// 80x80 frame: Set winning pixel to total charge and clear the other three pixels
					this->updateDecodedFrame(iRow, iCol, sumOfFourPixels, winnerIdx, apLastDecodedFrame);
					mCaseC++;
				}
				else {

					// Case D, no other pixels hit

					// 240x240 frame: Update SubPixel frame with total charge
					apSubPixelFrame->setPixel((iRow*3+1), (iCol*3+1), currentPixelValue);
					
					// Only current pixel hit, therefore need not call updateDecodedFrame(), updateSubPixelFrameCase..()
					mCaseD++;
				}				

				// Increment running total of number of corrections applied
				mEventsCorrected++;

			}	// End of currentPixelValue > 0.0
		}	// End of iCol loop		
	}	// End of iRow loop

	return true;
}

/// sumFourPixels - Sum the pixel values in array aaPixels[]
/// @param aaPixels[] array containing four pixel values
/// @return double value containing sum of the four pixels
double HxtFrameChargeSharingSubPixelCorrector::sumFourPixels(double aaPixels[]) {

	double sumOfFourPixels = 0.0;
	// Sum up to charge for the four pixels
	for ( int i = 0; i < 4; i++ )
	{		
		sumOfFourPixels += aaPixels[i];
	}

	return sumOfFourPixels;
}

/// updateDecodedFrame - update decoded frame at pointer apLastDecodedFrame based upon the other arguments
/// @param currentRow - the row index of the current 2x2 miniature targeted frame
/// @param currentCol - the col index of the current 2x2 miniature targeted frame
/// @param totalCharge - the total charge of the four pixels in the 2x2 frame
/// @param aWinnerIdx - the pixel in the 2x2 frame containing the highest value
/// @param apLastDecodedFrame pointer pointing at decoded frame to be modified
/// @return bool value  signalling true for success and false for failure
bool HxtFrameChargeSharingSubPixelCorrector::updateDecodedFrame(unsigned int currentRow, unsigned int currentCol, double totalCharge,
										int const &aWinnerIdx, HxtDecodedFrame* apLastDecodedFrame) {
	
	switch(aWinnerIdx)
	{
		case 0:
			{
				// Update winner (current pixel)
				apLastDecodedFrame->setPixel(currentRow, currentCol, totalCharge);
				// The hit pixel was the winner, clear its' three neighbours
				apLastDecodedFrame->setPixel(currentRow+1, currentCol, 0.0);
				apLastDecodedFrame->setPixel(currentRow, currentCol+1, 0.0);
				apLastDecodedFrame->setPixel(currentRow+1, currentCol+1, 0.0);
				// Clear all four pixels in copy of frame to prevent reprocessing same pixel(s) later on
				apLastDecodedFrame->setPixelCopy(currentRow, currentCol, 0.0);
				apLastDecodedFrame->setPixelCopy(currentRow+1, currentCol, 0.0);
				apLastDecodedFrame->setPixelCopy(currentRow, currentCol+1, 0.0);
				apLastDecodedFrame->setPixelCopy(currentRow+1, currentCol+1, 0.0);				
			}
			break;

		case 1:
			{
				// Update winner (currentRow, currentCol+1)
				apLastDecodedFrame->setPixel(currentRow, currentCol+1, totalCharge);
				// Clear 0, 2, 3
				apLastDecodedFrame->setPixel(currentRow, currentCol, 0.0);
				apLastDecodedFrame->setPixel(currentRow+1, currentCol, 0.0);
				apLastDecodedFrame->setPixel(currentRow+1, currentCol+1, 0.0);
				// Clear all four pixels in copy of frame to prevent reprocessing same pixel(s) later on
				apLastDecodedFrame->setPixelCopy(currentRow, currentCol, 0.0);
				apLastDecodedFrame->setPixelCopy(currentRow+1, currentCol, 0.0);
				apLastDecodedFrame->setPixelCopy(currentRow, currentCol+1, 0.0);
				apLastDecodedFrame->setPixelCopy(currentRow+1, currentCol+1, 0.0);
			}
			break;

		case 2:
			{
				// Update winner (currentRow+1, currentCol)
				apLastDecodedFrame->setPixel(currentRow+1, currentCol, totalCharge);
				// Clear 0, 1, 3
				apLastDecodedFrame->setPixel(currentRow, currentCol, 0.0);
				apLastDecodedFrame->setPixel(currentRow, currentCol+1, 0.0);
				apLastDecodedFrame->setPixel(currentRow+1, currentCol+1, 0.0);
				// Clear all four pixels in copy of frame to prevent reprocessing same pixel(s) later on
				apLastDecodedFrame->setPixelCopy(currentRow, currentCol, 0.0);
				apLastDecodedFrame->setPixelCopy(currentRow+1, currentCol, 0.0);
				apLastDecodedFrame->setPixelCopy(currentRow, currentCol+1, 0.0);
				apLastDecodedFrame->setPixelCopy(currentRow+1, currentCol+1, 0.0);
			}
			break;

		case 3:
			{
				// Update winner (currentRow+1, currentCol+1)
				apLastDecodedFrame->setPixel(currentRow+1, currentCol+1, totalCharge);
				// Clear 0, 1, 2
				apLastDecodedFrame->setPixel(currentRow, currentCol, 0.0);
				apLastDecodedFrame->setPixel(currentRow+1, currentCol, 0.0);
				apLastDecodedFrame->setPixel(currentRow, currentCol+1, 0.0);
				// Clear all four pixels in copy of frame to prevent reprocessing same pixel(s) later on
				apLastDecodedFrame->setPixelCopy(currentRow, currentCol, 0.0);
				apLastDecodedFrame->setPixelCopy(currentRow+1, currentCol, 0.0);
				apLastDecodedFrame->setPixelCopy(currentRow, currentCol+1, 0.0);
				apLastDecodedFrame->setPixelCopy(currentRow+1, currentCol+1, 0.0);
			}
			break;

		default:
			{
				// Print error Message!
				LOG(gLogConfig, logERROR) << "Unable to clear neighbour pixels, aWinnerIdx = " << aWinnerIdx;
				return false;
			}
	}
	return true;
}

/// winnerPixel - determine which pixel contains the highest value in the aaPixels[] array
/// @param aaPixels[] - array containing four pixel values
/// @param aWinnerIdx - this reference will contain index of pixel with highest value
void HxtFrameChargeSharingSubPixelCorrector::winnerPixel(double aaPixels[], int &aWinnerIdx) {

	// Assume first pixel is winner by default
	aWinnerIdx = 0;
	double maxVal = -1;

	// Look for biggest hit in array
	for ( int i = 0; i < 4; i++ )
	{	
		if (aaPixels[i] > maxVal)
		{
			maxVal = aaPixels[i];
			aWinnerIdx = i;
		}
	}
}

/// updateSubPixelFrameCaseA - update subpixelframe at apSubPixelFrame according to CSA algorithm Case A
/// @param aRow - the row index of "winning" pixel 
/// @param aCol - the column index of "winning" pixel
/// @param aSumOfFourPixels - the total charge of pixel (aRow, aCol) + its three neighbours
/// @param aWinnerIdx - the location of the "winning" pixel in relation to the 2x2 matrix
/// @param apSubPixelFrame pointer pointing at subpixel frame to be modified
void HxtFrameChargeSharingSubPixelCorrector::updateSubPixelFrameCaseA(HxtFrame* apSubPixelFrame, unsigned int aRow, unsigned int aCol, 
																	int aWinnerIdx, double aSumOfFourPixels) {
	// 240x240 frame: Update SubPixel frame with total charge
	switch(aWinnerIdx)
	{
		case 0:
			{				
				apSubPixelFrame->setPixel((aRow*3+2), (aCol*3+2), aSumOfFourPixels);
			}
			break;

		case 1:
			{				
				apSubPixelFrame->setPixel((aRow*3+2), (aCol*3), aSumOfFourPixels);
			}
			break;

		case 2:
			{				
				apSubPixelFrame->setPixel((aRow*3), (aCol*3+2), aSumOfFourPixels);
			}
			break;

		case 3:
			{				
				apSubPixelFrame->setPixel((aRow*3), (aCol*3), aSumOfFourPixels);
			}
			break;

		default:
			{
				// Print error Message!
				LOG(gLogConfig, logERROR) << "Unable to Update Case A Subpixel, aWinnerIdx = " << aWinnerIdx;
				return;
			}
	}
}

/// updateSubPixelFrameCaseA - update subpixelframe at apSubPixelFrame according to CSA algorithm Case B
/// @param aRow - the row index of "winning" pixel 
/// @param aCol - the column index of "winning" pixel
/// @param aSumOfFourPixels - the total charge of pixel (aRow, aCol) + its three neighbours
/// @param aWinnerIdx - the location of the "winning" pixel in relation to the 2x2 matrix
/// @param apSubPixelFrame pointer pointing at subpixel frame to be modified
void HxtFrameChargeSharingSubPixelCorrector::updateSubPixelFrameCaseB(HxtFrame* apSubPixelFrame, unsigned int aRow, unsigned int aCol, 
																	int aWinnerIdx, double aSumOfFourPixels) {
	
	switch(aWinnerIdx)
	{
		case 0:
			{				
				apSubPixelFrame->setPixel((aRow*3+2), (aCol*3+1), aSumOfFourPixels);
			}
			break;

		case 2:
			{				
				apSubPixelFrame->setPixel((aRow*3), (aCol*3+1), aSumOfFourPixels);
			}
			break;

		default:
			{
				// Print error Message!
				LOG(gLogConfig, logERROR) << "Unable to Update Case B Subpixel, aWinnerIdx = " << aWinnerIdx;
				return;
			}
	}
}

/// updateSubPixelFrameCaseA - update subpixelframe at apSubPixelFrame according to CSA algorithm Case C
/// @param aRow - the row index of "winning" pixel 
/// @param aCol - the column index of "winning" pixel
/// @param aSumOfFourPixels - the total charge of pixel (aRow, aCol) + its three neighbours
/// @param aWinnerIdx - the location of the "winning" pixel in relation to the 2x2 matrix
/// @param apSubPixelFrame pointer pointing at subpixel frame to be modified
void HxtFrameChargeSharingSubPixelCorrector::updateSubPixelFrameCaseC(HxtFrame* apSubPixelFrame, unsigned int aRow, unsigned int aCol, 
																	int aWinnerIdx, double aSumOfFourPixels) {
	switch(aWinnerIdx)
	{
		case 0:
			{				
				apSubPixelFrame->setPixel((aRow*3+1), (aCol*3+2), aSumOfFourPixels);
			}
			break;

		case 1:
			{				
				apSubPixelFrame->setPixel((aRow*3+1), (aCol*3), aSumOfFourPixels);
			}
			break;

		default:
			{
				// Print error Message!
				LOG(gLogConfig, logERROR) << "Unable to Update Case C Subpixel, aWinnerIdx = " << aWinnerIdx;
				return;
			}
	}
}

}
