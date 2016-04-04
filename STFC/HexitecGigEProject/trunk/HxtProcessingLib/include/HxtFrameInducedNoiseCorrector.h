/*
 * HxtFrameInducedNoiseCorrector.h
 *
 *  Created on: 29 Feb 2012
 *      Author: ckd
 */

#ifndef HXTFRAMEINDUCEDNOISECORRECTOR_H_
#define HXTFRAMEINDUCEDNOISECORRECTOR_H_

#include "HxtFrameCorrector.h"
#include "HxtFrame.h"

namespace hexitech {

class HxtFrameInducedNoiseCorrector: public hexitech::HxtFrameCorrector {
public:
	HxtFrameInducedNoiseCorrector(double aInducedNoiseThreshold);
	virtual ~HxtFrameInducedNoiseCorrector();

    bool HxtFrameInducedNoiseCorrector::apply(HxtDecodedFrame* apLastDecodedFrame, HxtDecodedFrame* apCurrentDecodedFrame);

private:	
	unsigned long long	mLastFrameProcessed;
	double				mInducedNoiseThreshold;
	unsigned int		mNoiseCurrentFrame;
	unsigned int		mNoiseNextFrame;
};

}

#endif /* HXTFRAMEINDUCEDNOISECORRECTOR_H_ */
