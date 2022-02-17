/*
 * HxtFrameIncompleteDataCorrector.h
 *
 *  Created on: 14 Dec 2010
 *      Author: tcn
 */

#ifndef HXTFRAMEINCOMPLETEDATACORRECTOR_H_
#define HXTFRAMEINCOMPLETEDATACORRECTOR_H_

#include "HxtFrameCorrector.h"
#include "HxtFrame.h"

namespace hexitech {

class HxtFrameIncompleteDataCorrector: public hexitech::HxtFrameCorrector {
public:
	HxtFrameIncompleteDataCorrector();
	virtual ~HxtFrameIncompleteDataCorrector();

    bool apply(HxtDecodedFrame* apLastDecodedFrame, HxtDecodedFrame* apCurrentDecodedFrame);
};

}

#endif /* HXTFRAMEINCOMPLETEDATACORRECTOR_H_ */
