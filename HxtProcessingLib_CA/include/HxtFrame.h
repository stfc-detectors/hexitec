/*
 * HxtFrame.h
 *
 *  Created on: 13 Dec 2010
 *      Author: tcn
 */

#ifndef HXTFRAME_H_
#define HXTFRAME_H_

#include "HxtLogger.h"
#include "HxtRawDataTypes.h"

#include <vector>

namespace hexitech {

class HxtFrame {

public:
	HxtFrame(unsigned int aRows, unsigned int aCols);
	virtual ~HxtFrame();

	void setFrameIndex(unsigned long long aFrameNumber);
	unsigned long long getFrameIndex(void);

	void setPixel(unsigned int aRow, unsigned int aCol, double aValue);
	double getPixel(unsigned int aRow, unsigned int aCol);

    unsigned int getCols() const;
    unsigned int getRows() const;

	void dumpFrame(void);
	void clear(void);

	vector<hxtPixel> getPixelList();
	void setVector(bool aEnableVector);
	bool getVectorStatus();

	// Make copy of existing pixel frame - needed by CSA algorithm
	void copyPixels();
	void setPixelCopy(unsigned int aRow, unsigned int aCol, double aValue);
	double getPixelCopy(unsigned int aRow, unsigned int aCol);

protected:
	unsigned int mRows;
	unsigned int mCols;
	unsigned long long mFrameIndex;
	unsigned int mPixels;
	double*      mPixelValue;
	double*		 mPixelCopy;

	virtual inline unsigned int pixelAddress(int aRow, int aCol) {
		return ((aRow * mCols) + aCol);
	}

	vector<hxtPixel> mPixelList;
	bool mEnableVector;
};


}

#endif /* HXTFRAME_H_ */
