/*
 * HxtPixelThreshold.h
 *
 *  Created on: 15 Dec 2010
 *      Author: tcn
 */

#ifndef HXTTHRESHOLDMANAGER_H_
#define HXTTHRESHOLDMANAGER_H_

#include <iostream>
#include <fstream>
#include <string>

#include "HxtLogger.h"
#include "HxtPixelArray.h"

using namespace std;

namespace hexitech {

class HxtPixelThreshold : public HxtPixelArray {
public:
	HxtPixelThreshold(unsigned int aRows, unsigned int aCols);
	virtual ~HxtPixelThreshold();

	bool loadThresholds(string aFileName);
	void setGlobalThreshold(double aThreshold);

	void dumpThresholds(void);

	double getPixelThreshold(unsigned int aPixel);
	double getPixelThreshold(unsigned int aRow, unsigned int aCol);

private:

	double* mPixelThreshold;
};

}

#endif /* HXTTHRESHOLDMANAGER_H_ */
