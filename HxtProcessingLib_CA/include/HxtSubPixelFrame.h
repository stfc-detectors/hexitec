/*
 * HxtSubPixelFrame.h
 *
 *  Created on: 10 February 2012
 *      Author: ckd
 */

#ifndef HXTSUBPIXELFRAME_H
#define HXTSUBPIXELFRAME_H

#include "HxtLogger.h"
#include "HxtFrame.h"

namespace hexitech {

class HxtSubPixelFrame: public hexitech::HxtFrame {

public:
	HxtSubPixelFrame(unsigned int aRows, unsigned int aCols);
	virtual ~HxtSubPixelFrame();

private:
	unsigned int mInternalRows;
	unsigned int mInternalCols;

	inline unsigned int pixelAddress(int aRow, int aCol) {
        return ((aRow) * mInternalCols) + (aCol);
	}
};


}

#endif /* HXTSUBPIXELFRAME_H */
