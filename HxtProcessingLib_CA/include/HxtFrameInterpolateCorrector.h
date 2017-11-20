/*
 * HxtFrameInterpolateCorrector.h
 *
 *  Created on: 1 Mar 12
 *      Author: ckd
 */

#ifndef HXTFRAMEINTERPOLATECORRECTOR_H_
#define HXTFRAMEINTERPOLATECORRECTOR_H_

#include "HxtFrameCorrector.h"
#include "HxtFrame.h"
#include <vector>


namespace hexitech {

class HxtFrameInterpolateCorrector: public hexitech::HxtFrameCorrector {
public:
	HxtFrameInterpolateCorrector();
	virtual ~HxtFrameInterpolateCorrector();

    bool HxtFrameInterpolateCorrector::apply(HxtDecodedFrame* apLastDecodedFrame, HxtDecodedFrame* apCurrentDecodedFrame);

	double calculateNeighboursAverage(HxtDecodedFrame* apLastDecodedFrame, unsigned int aRow, unsigned int aCol);
private:
	unsigned long long	mLastFrameProcessed;

};

}

#endif /* HXTFRAMEINTERPOLATECORRECTOR_H_ */
