/*
 * HxtPixelArray.cpp
 *
 *  Created on: 15 Dec 2010
 *      Author: tcn
 */

#include "HxtPixelArray.h"

namespace hexitech {

HxtPixelArray::HxtPixelArray(unsigned int aRows, unsigned int aCols) :
		       mRows(aRows), mCols(aCols), mPixels(aRows * aCols)
{

}
HxtPixelArray::~HxtPixelArray() {

}

}
