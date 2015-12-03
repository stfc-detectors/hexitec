/*
 * HxtPixelThreshold.cpp
 *
 *  Created on: 15 Dec 2010
 *      Author: tcn
 */


#include "HxtPixelThreshold.h"

using namespace std;

namespace hexitech {

HxtPixelThreshold::HxtPixelThreshold(unsigned int aRows, unsigned int aCols)
                   : HxtPixelArray(aRows, aCols),
                     mPixelThreshold( new double[aRows * aCols] )
{
	// Initialise threshold array to zero
	for (unsigned int iPixel = 0; iPixel < mPixels; iPixel++) {
		mPixelThreshold[iPixel] = 0.0;
	}

}

HxtPixelThreshold::~HxtPixelThreshold() {
	delete [] mPixelThreshold;
}

bool HxtPixelThreshold::loadThresholds(string aFileName) {

	bool loadOK = true;

	unsigned int numLoaded = 0;
	
	ifstream threshFile;
	threshFile.open(aFileName.c_str(), ios::in);

	if (!threshFile.is_open()) {
		LOG(gLogConfig, logERROR) << "Failed to open threshold file " << aFileName;
		loadOK = false;

	} else {

		while ((numLoaded < mPixels) && !threshFile.eof()) {			
			threshFile >> mPixelThreshold[numLoaded];
			numLoaded++;			
		}

		if (numLoaded != mPixels) {
			LOG(gLogConfig, logERROR) << "Failed to read " << mPixels << " threshold values, loaded " << numLoaded << " from file " << aFileName;
			loadOK = false;
		}		
		threshFile.close();
	}

	return loadOK;
}

void HxtPixelThreshold::setGlobalThreshold(double aThreshold) {

	for (unsigned int iPixel = 0; iPixel < mPixels; iPixel++) {
		mPixelThreshold[iPixel] = aThreshold;
	}
}

void HxtPixelThreshold::dumpThresholds(void) {

	LOG(gLogConfig, logDEBUG2) << "Dumping thresholds ...";

	for (unsigned int iRow = 0; iRow < mRows; iRow++) {

		ostringstream rowStream;
		for (unsigned int iCol = 0; iCol < mCols; iCol++) {
			rowStream << mPixelThreshold[(iRow * mCols) + iCol] << " ";
		}
		LOG(gLogConfig, logDEBUG2) << "Row " << iRow << ": " << rowStream.str();
		rowStream.str("");
	}
}

double HxtPixelThreshold::getPixelThreshold(unsigned int aPixel) {

	double threshVal = -1.0;
	if (aPixel < mPixels) {
		threshVal = mPixelThreshold[aPixel];
	}
	return threshVal;
}

double HxtPixelThreshold::getPixelThreshold(unsigned int aRow, unsigned int aCol) {

	double threshVal = -1.0;
	if ((aRow < mRows) && (aCol < mCols)) {
		threshVal = mPixelThreshold[(aRow * mCols) + aCol];
	}
	return threshVal;
}

} // namespace
