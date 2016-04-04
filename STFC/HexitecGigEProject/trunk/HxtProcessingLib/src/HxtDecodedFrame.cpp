/*
 * HxtDecodedFrame.cpp - subclass inheriting from HxtFrame
 *
 *  Created on: 09 February 2012
 *      Author: ckd
 */

#include <iostream>
#include <iomanip>

#include "HxtDecodedFrame.h"

using namespace std;

namespace hexitech {

HxtDecodedFrame::HxtDecodedFrame(unsigned int aRows, unsigned int aCols) :
	                            HxtFrame(aRows, aCols),
	                            mInternalRows(aRows + 2),
	                            mInternalCols(aCols + 2)
{
	mPixels = mInternalRows * mInternalCols;
	mPixelValue = new double[mPixels];

	for (unsigned int iPixel = 0; iPixel < mPixels; iPixel++) {
		mPixelValue[iPixel] = 0.0;
	}
	
}

HxtDecodedFrame::~HxtDecodedFrame() {

	// Delete pixel values
	//delete [] mPixelValue;	// Don't delete here, let parent free common memory
}


/// getAdjacent - gets sum of values of adjacent pixels
/// @param aRow row number
/// @param aCol column number
/// @return double sum of adjacent pixel value
double HxtDecodedFrame::getAdjacent(unsigned int aRow, unsigned int aCol) {

	double adjacentSum = 0.0;

	// Sum up pixel values for pixels(row+-1,col+-1) around specified pixel - here we exploit
	// the fact that the pixel array is offset in row and col by one to allow this to work
	// generally even up to the edges of the pixel array
    /// HexitecGigE Addition: Expanded to incorporate all 8 neighbours
    adjacentSum += mPixelValue[pixelAddress((aRow - 1), (aCol - 1))];
    adjacentSum += mPixelValue[pixelAddress((aRow - 1),  aCol)];
    adjacentSum += mPixelValue[pixelAddress((aRow - 1), (aCol + 1))];
    adjacentSum += mPixelValue[pixelAddress(aRow,       (aCol - 1))];
    adjacentSum += mPixelValue[pixelAddress(aRow,       (aCol + 1))];
    adjacentSum += mPixelValue[pixelAddress((aRow + 1), (aCol - 1))];
    adjacentSum += mPixelValue[pixelAddress((aRow + 1),  aCol)];
    adjacentSum += mPixelValue[pixelAddress((aRow + 1), (aCol + 1))];

	return adjacentSum;
}

/// clearAdjacent - clears values of adjacent pixels
/// @param aRow row number
/// @param aCol column number
void HxtDecodedFrame::clearAdjacent(unsigned int aRow, unsigned int aCol) {

    /// HexitecGigE Addition: Expanded to incorporate all 8 neighbours
    mPixelValue[pixelAddress((aRow - 1), (aCol - 1))] = 0.0;
    mPixelValue[pixelAddress((aRow - 1), aCol)]       = 0.0;
    mPixelValue[pixelAddress((aRow - 1), (aCol + 1))] = 0.0;
    mPixelValue[pixelAddress(aRow, (aCol - 1))]       = 0.0;
    mPixelValue[pixelAddress(aRow, (aCol + 1))]       = 0.0;
    mPixelValue[pixelAddress((aRow + 1), (aCol - 1))] = 0.0;
    mPixelValue[pixelAddress((aRow + 1), aCol)]       = 0.0;
    mPixelValue[pixelAddress((aRow + 1), (aCol + 1))] = 0.0;
	
}

}
