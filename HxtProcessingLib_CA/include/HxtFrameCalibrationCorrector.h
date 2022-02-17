/*
 * HxtFrameCalibrationCorrector.h
 *
 *  Created on: 6 Mar 12
 *      Author: ckd
 */

#ifndef HXTFRAMECALIBRATIONCORRECTOR_H_
#define HXTFRAMECALIBRATIONCORRECTOR_H_

#include "HxtFrameCorrector.h"
#include "HxtFrame.h"
#include "HxtPixelThreshold.h"

namespace hexitech {

class HxtFrameCalibrationCorrector: public hexitech::HxtFrameCorrector {
public:
	HxtFrameCalibrationCorrector(HxtPixelThreshold* apGradients, HxtPixelThreshold* apIntercepts, unsigned int aRows, unsigned int aCols);
	virtual ~HxtFrameCalibrationCorrector();

    bool HxtFrameCalibrationCorrector::apply(HxtDecodedFrame* apLastDecodedFrame, HxtDecodedFrame* apCurrentDecodedFrame);

private:
	unsigned long long	mLastFrameProcessed;
	unsigned int mRows;
	unsigned int mCols;
	unsigned int mPixels;
	double*		mGradientValue;
	double*		mInterceptValue;

	inline unsigned int pixelAddress(int aRow, int aCol) {
		return ((aRow * mCols) + aCol);
	}

};

}

#endif /* HXTFRAMECALIBRATIONCORRECTOR_H_ */
