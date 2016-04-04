/*
 * HxtFrameChargeSharingSubPixelCorrector.cpp
 *
 * This frame corrector implements charge sharing addition, where pixels
 * which have non-zero values in surrounding pixels (2x2 matrix) are summed
 * and moved to the "winner pixel" while neighbours are zeroed.
 * e.g. 2x2: becomes 2x2:
 *   50 20			100 0
 *   20 10			  0 0
 *
 *  Created on: 13 Feb 12
 *      Author: ckd
 */

#include "HxtFrameChargeSharingSubPixelCorrector.h"
#include <algorithm>
/// DEBUGGING purposes:
#include <iostream>
#include <iomanip>
///

namespace hexitech {

HxtFrameChargeSharingSubPixelCorrector::HxtFrameChargeSharingSubPixelCorrector() :
																			HxtFrameCorrector(string("ChargeSharingAdditionSubPixel")),
																			mLastFrameProcessed(0)																			
{
	// Nothing to do
    /// DEBUGGING:
    bDebug = false;
}

HxtFrameChargeSharingSubPixelCorrector::~HxtFrameChargeSharingSubPixelCorrector() {

	// Nothing to do
}

/// apply - applies charge sharing addition correction (CSA) algorithm to specified frame
/// @param apLastDecodedFrame ptr to the previous decoded frame
/// @param apCurrentDecoded frame ptr to current decoded frame

/// @return bool value indicating success of correction
bool HxtFrameChargeSharingSubPixelCorrector::apply(HxtDecodedFrame* apLastDecodedFrame, HxtDecodedFrame* apCurrentDecodedFrame) {

    // Examine apLastDecodedFrame for hits; Any hit examines local 3x3 matrix for charge sharing and were found,
    // move all charges on to pixel containing highest initial charge.
    /// HexitecGigE Modification, the 240x240 matrix (apSubPixelFrame) of the subpixel algorithm has been removed
	// (apCurrentDecodedFrame pointer never used)
    apCurrentDecodedFrame = apCurrentDecodedFrame;

    unsigned long long lastFrameIdx;
	lastFrameIdx = apLastDecodedFrame->getFrameIndex();
	

	// Get rows and columns lengths
	const unsigned int nRows = apLastDecodedFrame->getRows();
	const unsigned int nCols = apLastDecodedFrame->getCols();

	// Current pixel to be interrogated
	double currentPixelValue = 0.0;
    // Sum of pixel values in 3x3 pixel matrix
    double sumOfEightPixels = 0.0;

    // Create array of nine indices, one for each pixel in 3x3 matrix
    double arPixelValues[9];
    for ( int i = 0; i < 9; i++ )
	{
		arPixelValues[i] = 0;
		if (mDebug) LOG(gLogConfig, logDEBUG2) << "CSSPxl::apply() arPixelValues[" << i << "] = " << arPixelValues[i];
	}

    // Looking for the eight neighbouring pixels,
    // forming a 3x3 matrix:
    //		----------
    //		| 0 1| 2|
    //		----------
    //		| 3|*4| 5|
    //		----------
    //		| 6| 7| 8|
    //		----------
	// * = The pixel that's hit
	
	int winnerIdx = -1;

	if (mDebug) LOG(gLogConfig, logDEBUG1) << "Applying charge sharing addition data corrector to (lastFrameIdx) frame index " << lastFrameIdx;

	// Note frame number of the frame being processed
	mLastFrameProcessed = lastFrameIdx;

    /// DEBUGGING:
    if (bDebug) cout << " mLastFrameProcessed: " << mLastFrameProcessed  << endl;
//    if (mLastFrameProcessed > 7) bDebug = true;
//    if (mLastFrameProcessed > 8) bDebug = false;

    /// Debugging purposes:
    int iStart=23*80, iStop=27*80, iWidth=4;
    bool bWinnerDebug  = false; //true;
    if (bDebug)
    {
        cout << "iStart: " << iStart << " iStop: " << iStop << " iWidth: " << iWidth << endl;
        cout << " --------- BEFOREHAND ----------";
        for (int i = iStart; i < iStop; i++ )
        {
            if ( i % 40 == 0) cout << endl << "[" << setw(2) << i/80 << "][" << setw(2) << i%80 << "]: ";
            cout  << setw(iWidth) << (int)(apLastDecodedFrame->getPixel(i/80, i% 80)) << ", ";
        }
        cout << endl;
    }
    ///

	// Make copy of pixels in frame
//	apLastDecodedFrame->copyPixels();

	// Normal mode only - vector mode slower because
	// algorithm requires ordering of vector elements

    // Iterate over rows and columns in frames
    for (unsigned int iRow = 0; iRow < nRows; iRow++) {

        for (unsigned int iCol = 0; iCol < nCols; iCol++) {
			
			// Get pixel values from copy of frame
            currentPixelValue = apLastDecodedFrame->getPixel(iRow, iCol);
			
			// Is current pixel hit?
            if (currentPixelValue > 0.0) {

                arPixelValues[0] = apLastDecodedFrame->getPixel(iRow-1, iCol-1);
                arPixelValues[1] = apLastDecodedFrame->getPixel(iRow-1, iCol);
                arPixelValues[2] = apLastDecodedFrame->getPixel(iRow-1, iCol+1);

                arPixelValues[3] = apLastDecodedFrame->getPixel(iRow, iCol-1);
                arPixelValues[4] = currentPixelValue;
                arPixelValues[5] = apLastDecodedFrame->getPixel(iRow, iCol+1);

                arPixelValues[6] = apLastDecodedFrame->getPixel(iRow+1, iCol-1);
                arPixelValues[7] = apLastDecodedFrame->getPixel(iRow+1, iCol);
                arPixelValues[8] = apLastDecodedFrame->getPixel(iRow+1, iCol+1);

                // Count number of hit pixels
                unsigned int numberOfPixelsHit = 0;

                for (unsigned int i= 0; i < 9; i++)
                    if (arPixelValues[i] > 0) numberOfPixelsHit++;  // Only count non-zero pixels..

                sumOfEightPixels = this->sumEightPixels(arPixelValues);

                /// DEBUGGING:
                if (bDebug && (iRow*80+iCol > iStart) && (iRow*80+iCol < iStop))
                {
                    cout << " Pxl hit! coord: [" << iRow << "][" << iCol << "] = " << currentPixelValue << ", sumOfEightPixels: " << sumOfEightPixels << ",  arPixelValues array vals:";
                    for (unsigned int i= 0; i < 9; i++)
                    {
                        if (i % 3 == 0)
                            cout << endl;
                        cout << setw(3) << i << ": " << setw(iWidth) << arPixelValues[i] << ", ";
                    }
                    cout << endl;
                    bWinnerDebug = true;
                }
                else
                    bWinnerDebug = false;



				// Pixel hit; case A, B, C or D?
                if ( numberOfPixelsHit > 3) {

                    if (bDebug && (iRow*80+iCol > iStart) && (iRow*80+iCol < iStop)) cout << "CASE Alpha!\n";   /// DEBUGGING purposes

                    // Case A, at least four pixels hit
                    this->winnerPixel(arPixelValues, winnerIdx, bWinnerDebug);
										
                    // 80x80 frame: Set winning pixel to total charge and clear the other eight pixels
                    this->updateDecodedFrame(iRow, iCol, sumOfEightPixels, winnerIdx, apLastDecodedFrame);
					mCaseA++;
				}
                else if ( numberOfPixelsHit > 1 ) {

                    // Case B, if same column pixels hit
                    // Case C, if same row pixels hit
                    this->winnerPixel(arPixelValues, winnerIdx, bWinnerDebug);

                    /// How determine each case?
                    // Lets see if pixel "1", "7" hit: Case B
                    if ( winnerIdx == 1 || winnerIdx == 7)
                    {
                        if (bDebug && (iRow*80+iCol > iStart) && (iRow*80+iCol < iStop)) cout << "CASE bravo!\n";   /// DEBUGGING purposes
                        // 80x80 frame: Set winning pixel to total charge and clear the other eight pixels
                        this->updateDecodedFrame(iRow, iCol, sumOfEightPixels, winnerIdx, apLastDecodedFrame);
                        mCaseB++;
                    }
                    else if (winnerIdx == 3 || winnerIdx == 5)   // Lets see if pixel "3", "5" hit: Case C
					{
                        if (bDebug && (iRow*80+iCol > iStart) && (iRow*80+iCol < iStop)) cout << "CASE Charlie!\n";   /// DEBUGGING purposes
                        // 80x80 frame: Set winning pixel to total charge and clear the other eight pixels
                        this->updateDecodedFrame(iRow, iCol, sumOfEightPixels, winnerIdx, apLastDecodedFrame);
                        mCaseC++;
					}
                    else
                    {
                        if (bDebug && (iRow*80+iCol > iStart) && (iRow*80+iCol < iStop)) cout << "CASE is Triplett..!\n";   /// DEBUGGING purposes;   // It DOES happen !
                        updateDecodedFrame(iRow, iCol, sumOfEightPixels, winnerIdx, apLastDecodedFrame);
                        mCaseTriplett++;
                    }
				}
				else {
					// Case D, no other pixels hit
					mCaseD++;
                    if (bDebug && (iRow*80+iCol > iStart) && (iRow*80+iCol < iStop)) cout << "CASE Delta!\n";   /// DEBUGGING purposes
				}				

				// Increment running total of number of corrections applied
				mEventsCorrected++;

			}	// End of currentPixelValue > 0.0
		}	// End of iCol loop		
    }
    // End of iRow loop

    /// Debugging purposes:
    if (bDebug)
    {
        cout << " --------- AFTERWARDS ----------";
        for (int i = iStart; i < iStop; i++ )
        {
            if ( i % 40 == 0) cout << endl << "[" << setw(2) << i/80 << "][" << setw(2) << i%80 << "]: ";
            cout  << setw(iWidth) << (int)(apLastDecodedFrame->getPixel(i/80, i% 80)) << ", ";
        }
        cout << endl;
    }
    ///

    return true;
}

/// sumEightPixels - Sum the pixel values in array aaPixels[]
/// @param aaPixels[] array containing four pixel values
/// @return double value containing sum of the four pixels
double HxtFrameChargeSharingSubPixelCorrector::sumEightPixels(double aaPixels[]) {

    double sumOfEightPixels = 0.0;
    // Sum up to charge for the eight neighbouring pixels (i.e. 9 including the pixel in the middle)
    for ( int i = 0; i < 9; i++ )
    {
        sumOfEightPixels += aaPixels[i];
	}
    return sumOfEightPixels;
}

/// updateDecodedFrame - update decoded frame at pointer apLastDecodedFrame based upon the other arguments
/// @param currentRow - the row index of the current 3x3 miniature targeted frame
/// @param currentCol - the col index of the current 3x3 miniature targeted frame
/// @param totalCharge - the total charge of the four pixels in the 3x3 frame
/// @param aWinnerIdx - the pixel in the 3x3 frame containing the highest value
/// @param apLastDecodedFrame pointer pointing at decoded frame to be modified
/// @return bool value  signalling true for success and false for failure
bool HxtFrameChargeSharingSubPixelCorrector::updateDecodedFrame(unsigned int currentRow, unsigned int currentCol, double totalCharge,
										int const &aWinnerIdx, HxtDecodedFrame* apLastDecodedFrame) {

    /// DEBUGGING:
    int iStart=23*80, iStop=                     27*80                    , iWidth=4;

    if (bDebug && (currentRow*80+currentCol > iStart) && (currentRow*80+currentCol < iStop))
    {
        cout << " ::updateDecodedFrame() arguments, currentRow: " << currentRow << ", currentCol: "
             << currentCol << ", totalCharge: " <<  totalCharge << ", aWinnerIdx: " << aWinnerIdx << endl;

        cout << "---- Content of Origin before clearing ----\n";
        cout << " " << setw(iWidth) << apLastDecodedFrame->getPixel(currentRow-1, currentCol-1);
        cout << " " << setw(iWidth) << apLastDecodedFrame->getPixel(currentRow-1, currentCol);
        cout << " " << setw(iWidth) << apLastDecodedFrame->getPixel(currentRow-1, currentCol+1) << endl;

        cout << " " << setw(iWidth) << apLastDecodedFrame->getPixel(currentRow, currentCol-1);
        cout << " " << setw(iWidth) << apLastDecodedFrame->getPixel(currentRow, currentCol);
        cout << " " << setw(iWidth) << apLastDecodedFrame->getPixel(currentRow, currentCol+1) << endl;

        cout << " " << setw(iWidth) << apLastDecodedFrame->getPixel(currentRow+1, currentCol-1);
        cout << " " << setw(iWidth) << apLastDecodedFrame->getPixel(currentRow+1, currentCol);
        cout << " " << setw(iWidth) << apLastDecodedFrame->getPixel(currentRow+1, currentCol+1) << endl;
    }
    ///

    /// Regardless of which pixel "won",  we can clear all pixels - winning pixel is then assigned value of totalCharge
    // Clear all 9 pixels in the original and copy of frame
    for (unsigned int iRow = (currentRow-1); iRow < (currentRow+2); iRow++)
    {
        for (unsigned int iCol = (currentCol-1); iCol< (currentCol+2); iCol++)
        {
            apLastDecodedFrame->setPixel(iRow, iCol, 0.0);
        }
    }

	switch(aWinnerIdx)
	{
		case 0:
            {
                apLastDecodedFrame->setPixel(currentRow-1, currentCol-1, totalCharge);
            }
        break;

        case 1:
            {
                apLastDecodedFrame->setPixel(currentRow-1, currentCol, totalCharge);
            }
        break;

        case 2:
            {
                apLastDecodedFrame->setPixel(currentRow-1, currentCol+1, totalCharge);
            }
        break;

        case 3:
            {
                apLastDecodedFrame->setPixel(currentRow, currentCol-1, totalCharge);
            }
        break;

        case 4:
            {
                apLastDecodedFrame->setPixel(currentRow, currentCol, totalCharge);
            }
        break;

        case 5:
			{
				apLastDecodedFrame->setPixel(currentRow, currentCol+1, totalCharge);
			}
			break;

        case 6:
            {
                apLastDecodedFrame->setPixel(currentRow+1, currentCol-1, totalCharge);
            }
        break;

        case 7:
            {
                apLastDecodedFrame->setPixel(currentRow+1, currentCol, totalCharge);
            }
        break;

        case 8:
            {
                apLastDecodedFrame->setPixel(currentRow+1, currentCol+1, totalCharge);
            }
        break;

		default:
			{
				// Print error Message!
				LOG(gLogConfig, logERROR) << "Unable to clear neighbour pixels, aWinnerIdx = " << aWinnerIdx;
				return false;
			}
	}
    if (bDebug && (currentRow*80+currentCol > iStart) && (currentRow*80+currentCol < iStop))
    {
        cout << "---- Content of Origin AFTER SETTING winning pixel ----\n";
        cout << " " << setw(iWidth) << apLastDecodedFrame->getPixel(currentRow-1, currentCol-1);
        cout << " " << setw(iWidth) << apLastDecodedFrame->getPixel(currentRow-1, currentCol);
        cout << " " << setw(iWidth) << apLastDecodedFrame->getPixel(currentRow-1, currentCol+1) << endl;

        cout << " " << setw(iWidth) << apLastDecodedFrame->getPixel(currentRow, currentCol-1);
        cout << " " << setw(iWidth) << apLastDecodedFrame->getPixel(currentRow, currentCol);
        cout << " " << setw(iWidth) << apLastDecodedFrame->getPixel(currentRow, currentCol+1) << endl;

        cout << " " << setw(iWidth) << apLastDecodedFrame->getPixel(currentRow+1, currentCol-1);
        cout << " " << setw(iWidth) << apLastDecodedFrame->getPixel(currentRow+1, currentCol);
        cout << " " << setw(iWidth) << apLastDecodedFrame->getPixel(currentRow+1, currentCol+1) << endl;
    }
    return true;
}

/// winnerPixel - determine which pixel contains the highest value in the aaPixels[] array
/// @param aaPixels[] - array containing nine pixel values
/// @param aWinnerIdx - this reference will contain index of pixel with highest value
void HxtFrameChargeSharingSubPixelCorrector::winnerPixel(double aaPixels[], int &aWinnerIdx, bool bDebug) {

	// Assume first pixel is winner by default
	aWinnerIdx = 0;
	double maxVal = -1;

	// Look for biggest hit in array
    for ( int i = 0; i < 9; i++ )
    {
		if (aaPixels[i] > maxVal)
		{
			maxVal = aaPixels[i];
			aWinnerIdx = i;
		}
	}
    /// DEBUGGING:
    if (bDebug) cout << " winnerPixel() aWinnerIdx: " << aWinnerIdx << " MaxVal: " << maxVal << endl;
}

}
