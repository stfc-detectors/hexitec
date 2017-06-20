/*
 * HxtFrameChargeSharingDiscCorrector.h
 *
 *  Created on: 6 Jan 2011
 *      Author: tcn
 */

#ifndef HXTFRAMECHARGESHARINGDISCCORRECTOR_H_
#define HXTFRAMECHARGESHARINGDISCCORRECTOR_H_

#include "HxtFrameCorrector.h"
#include "HxtFrame.h"

namespace hexitech {

class HxtFrameChargeSharingDiscCorrector: public hexitech::HxtFrameCorrector {
public:
	HxtFrameChargeSharingDiscCorrector();
	virtual ~HxtFrameChargeSharingDiscCorrector();
	
    bool HxtFrameChargeSharingDiscCorrector::apply(HxtDecodedFrame* apLastDecodedFrame, HxtDecodedFrame* apCurrentDecodedFrame);
};

}

#endif /* HXTFRAMECHARGESHARINGDISCCORRECTOR_H_ */
