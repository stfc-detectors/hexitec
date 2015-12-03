/*
 * HxtFrame.cpp
 *
 *  Created on: 13 Dec 2010
 *      Author: tcn
 */

#include <iostream>
#include <iomanip>

#include "HxtFrame.h"

using namespace std;

namespace hexitech {

HxtFrame::HxtFrame(unsigned int aRows, unsigned int aCols) :
	                            mRows(aRows),
	                            mCols(aCols)
{
	mFrameIndex = 0;

	mPixels = mRows * mCols;
	mPixelValue = new double[mPixels];
	mPixelCopy  = 0;

	for (unsigned int iPixel = 0; iPixel < mPixels; iPixel++) {
		mPixelValue[iPixel] = 0.0;
	}
	mEnableVector = false;
}

HxtFrame::~HxtFrame() {

	delete [] mPixelValue;
	if ( mPixelCopy != 0 ) delete [] mPixelCopy;
}

void HxtFrame::copyPixels() {

	// Only allocate memory once!
	if (mPixelCopy == 0)
		mPixelCopy = new double[mPixels];
	for (unsigned int iPixel = 0; iPixel < mPixels; iPixel++) {
		mPixelCopy[iPixel] = mPixelValue[iPixel];
	}
}

void HxtFrame::setPixel(unsigned int aRow, unsigned int aCol, double aValue) {

	if ((aRow < mRows) && (aCol < mCols)) {
		mPixelValue[pixelAddress(aRow, aCol)] = aValue;
		
		if (mEnableVector){
			if (aValue != 0.0)
			{
				hxtPixel thisPixel;
				thisPixel.nRow = aRow;
				thisPixel.nCol = aCol;
				mPixelList.push_back(thisPixel);
			}
		}
	}	
}

double HxtFrame::getPixel(unsigned int aRow, unsigned int aCol) {

	double pixelValue = 0.0;
	if ((aRow < mRows) && (aCol < mCols)) {
		pixelValue = mPixelValue[pixelAddress(aRow, aCol)];
	}
	return pixelValue;
}

void HxtFrame::setPixelCopy(unsigned int aRow, unsigned int aCol, double aValue) {

	if ((aRow < mRows) && (aCol < mCols))
		mPixelCopy[pixelAddress(aRow, aCol)] = aValue;
}

double HxtFrame::getPixelCopy(unsigned int aRow, unsigned int aCol) {

	double pixelValue = 0.0;
	if ((aRow < mRows) && (aCol < mCols)) {
		pixelValue = mPixelCopy[pixelAddress(aRow, aCol)];
	}
	return pixelValue;
}

void HxtFrame::setFrameIndex(unsigned long long aFrameNumber) {
	mFrameIndex = aFrameNumber;
}

unsigned long long HxtFrame::getFrameIndex(void) {
	return mFrameIndex;
}


unsigned int HxtFrame::getCols() const
 {
     return mCols;
 }

 unsigned int HxtFrame::getRows() const
 {
     return mRows;
 }
 
void HxtFrame::dumpFrame(void) {

	LOG(gLogConfig, logDEBUG2) << "Dumping frame content ...";

	for (unsigned int iRow = 0; iRow < mRows; iRow++) {

		ostringstream rowStream;
		for (unsigned int iCol = 0; iCol < mCols; iCol++) {
			rowStream << setw(6) << mPixelValue[pixelAddress(iRow, iCol)] << " ";
		}
		LOG(gLogConfig, logDEBUG2) << "Row " << iRow << ": " << rowStream.str();
		rowStream.str("");
	}

}

void HxtFrame::clear(void) {

	for (unsigned int iPixel = 0; iPixel < mPixels; iPixel++) {
		mPixelValue[iPixel] = 0.0;
	}
	// Also clear mPixelList if vector enabled
	if (mEnableVector)
		mPixelList.clear();
}

vector<hxtPixel> HxtFrame::getPixelList() {

	return mPixelList;
}


void HxtFrame::setVector(bool aEnableVector) {

	mEnableVector = aEnableVector;
}

bool HxtFrame::getVectorStatus() {

	return mEnableVector;
}

}