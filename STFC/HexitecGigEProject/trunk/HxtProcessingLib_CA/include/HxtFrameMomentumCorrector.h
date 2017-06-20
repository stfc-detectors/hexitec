/*
 * HxtFrameMomentumCorrector.h
 *
 *  Created on: 25 Feb 15
 *      Author: ckd
 */

#ifndef HXTFRAMEMOMENTUMCORRECTOR_H_
#define HXTFRAMEMOMENTUMCORRECTOR_H_

#include "HxtFrameCorrector.h"
#include "HxtFrame.h"
#include "HxtPixelThreshold.h"

namespace hexitech {

class HxtFrameMomentumCorrector: public hexitech::HxtFrameCorrector {
public:
    HxtFrameMomentumCorrector(HxtPixelThreshold* apMomentum, unsigned int aRows, unsigned int aCols);
    virtual ~HxtFrameMomentumCorrector();

    bool HxtFrameMomentumCorrector::apply(HxtDecodedFrame* apLastDecodedFrame, HxtDecodedFrame* apCurrentDecodedFrame);

private:	
	unsigned long long	mLastFrameProcessed;
	unsigned int mRows;
	unsigned int mCols;
	unsigned int mPixels;
    double*		mMomentumValue;

	inline unsigned int pixelAddress(int aRow, int aCol) {
		return ((aRow * mCols) + aCol);
	}

};

}

#endif /* HXTFRAMEMOMENTUMCORRECTOR_H_ */
