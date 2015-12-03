/*
 * HxtDecodedFrame.h
 *
 *  Created on: 09 February 2012
 *      Author: ckd
 */

#ifndef HXTDECODEDFRAME_H
#define HXTDECODEDFRAME_H

#include "HxtLogger.h"
#include "HxtFrame.h"

namespace hexitech {

class HxtDecodedFrame: public hexitech::HxtFrame {

public:
	HxtDecodedFrame(unsigned int aRows, unsigned int aCols);
	virtual ~HxtDecodedFrame();

	double	getAdjacent(unsigned int aRow, unsigned int aCol);
	void	clearAdjacent(unsigned int aRow, unsigned int aCol);

private:
	unsigned int mInternalRows;
	unsigned int mInternalCols;

	inline unsigned int pixelAddress(int aRow, int aCol) {
		return ((aRow+1) * mInternalCols) + (aCol+1);
	}

};


}

#endif /* HXTDECODEDFRAME_H */
