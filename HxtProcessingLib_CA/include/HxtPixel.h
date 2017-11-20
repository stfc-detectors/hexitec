/*
 * HxtPixel.h
 *
 *  Created on: 9 Mar 2012
 *      Author: ckd
 */

#ifndef HXTPIXELH_
#define HXTPIXEL_H_

#include <string>
//#include "HxtLogger.h"
//#include "HxtFrame.h"
//#include "HxtDecodedFrame.h"

//using namespace std;

namespace hexitech {

class HxtPixel {
public:
	HxtPixel(unsigned int aRow, unsigned int aCol) : mRow(aRow), mCol(aCol)
									{ };
	virtual ~HxtPixel() { };

    unsigned int getCol() const
    {
        return mCol;
    }

    unsigned int getRow() const
    {
        return mRow;
    }

protected:
	unsigned int mRow;
	unsigned int mCol;
	
};

}

#endif /* HXTPIXEL_H_ */
