/*
 * HxtPixelArray.h
 *
 *  Created on: 15 Dec 2010
 *      Author: tcn
 */

#ifndef HXTPIXELARRAY_H_
#define HXTPIXELARRAY_H_

namespace hexitech {

class HxtPixelArray {
public:
	HxtPixelArray(unsigned int aRows, unsigned int aCols);;
	virtual ~HxtPixelArray();
    unsigned int getCols() const
    {
        return mCols;
    }

    unsigned int getPixels() const
    {
        return mPixels;
    }

    unsigned int getRows() const
    {
        return mRows;
    }

protected:
	unsigned int mRows;
	unsigned int mCols;
	unsigned int mPixels;
};

}

#endif /* HXTPIXELARRAY_H_ */
