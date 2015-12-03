/*
 * Histogram.h
 *
 *  Created on: 10 Dec 2010
 *      Author: tcn
 */

#ifndef HISTOGRAM_H_
#define HISTOGRAM_H_

#include <iostream>
#include <iomanip>
#include <fstream>

class Histogram {
public:

	Histogram(const double aStart, const double aEnd, const unsigned int aBins);
	Histogram(const Histogram& aOther);
	~Histogram();

	void Fill(const double aValue);
	const unsigned int GetTotalCount(void);
	const unsigned int GetBinContent(const unsigned int aIdx);
	const double GetBinStart(const unsigned int aIdx);
	void Clear(void);
	// Set a specific bin to a specific value (for Interpolate only)
	void SetValue(const unsigned int aIdx, const unsigned int aValue);
	const unsigned int GetTotalAboveThreshold(const unsigned int aValue);

	void WriteCsv(std::ofstream& aOutFile);
	void BinaryWriteBins(std::ofstream& aOutFile);
	void BinaryWriteContent(std::ofstream& aOutFile);

private:

	double mStart, mBinsInterval;
	unsigned int mBins;
	unsigned int* mFrequency;
};

#endif /* HISTOGRAM_H_ */
