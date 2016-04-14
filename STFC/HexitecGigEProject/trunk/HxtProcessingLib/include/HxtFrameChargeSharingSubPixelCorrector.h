/*
 * HxtFrameChargeSharingSubPixelCorrector.h
 *
 *  Created on: 13 Feb 2012
 *      Author: ckd
 */

#ifndef HXTFRAMECHARGESHARINGSUBPIXELCORRECTOR_H_
#define HXTFRAMECHARGESHARINGSUBPIXELCORRECTOR_H_

#include "HxtFrameCorrector.h"
#include "HxtFrame.h"

namespace hexitech {

class HxtFrameChargeSharingSubPixelCorrector: public hexitech::HxtFrameCorrector {
public:

    HxtFrameChargeSharingSubPixelCorrector();
	virtual ~HxtFrameChargeSharingSubPixelCorrector();

    bool HxtFrameChargeSharingSubPixelCorrector::apply(HxtDecodedFrame* apLastDecodedFrame, HxtDecodedFrame* apCurrentDecodedFrame);
    double sumEightPixels(double aaPixels[]);
	bool	updateDecodedFrame(unsigned int currentRow, unsigned int currentCol, double totalCharge,
										int const &aWinnerIdx, HxtDecodedFrame* apLastDecodedFrame);
    void	winnerPixel(double aaPixels[], int &aWinnerIdx);

private:

	unsigned long long mLastFrameProcessed;

};

}

#endif /* HXTFRAMECHARGESHARINGSUBPIXELCORRECTOR_H_ */
