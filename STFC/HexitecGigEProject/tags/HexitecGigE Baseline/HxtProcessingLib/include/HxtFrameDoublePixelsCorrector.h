/*
 * HxtFrameDoublePixelsCorrector.h
 *
 *  Created on: 19 Mar 2012
 *      Author: ckd
 */

#ifndef HXTFRAMEDOUBLEPIXELSCORRECTOR_H_
#define HXTFRAMEDOUBLEPIXELSCORRECTOR_H_

#include "HxtFrameCorrector.h"
#include "HxtFrame.h"

namespace hexitech {

class HxtFrameDoublePixelsCorrector: public hexitech::HxtFrameCorrector {
public:
	HxtFrameDoublePixelsCorrector();
	virtual ~HxtFrameDoublePixelsCorrector();
	
	bool HxtFrameDoublePixelsCorrector::apply(HxtDecodedFrame* apLastDecodedFrame, HxtDecodedFrame* apCurrentDecodedFrame,
													HxtFrame* apSubPixelFrame = NULL);
};

}

#endif /* HXTFRAMEDOUBLEPIXELSCORRECTOR_H_ */
