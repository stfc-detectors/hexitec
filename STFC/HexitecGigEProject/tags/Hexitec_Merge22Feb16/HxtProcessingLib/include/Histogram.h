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
    /// HexitecGigE Overload operators:
    Histogram operator+ (const Histogram& aOther);
    Histogram & operator= (const Histogram& aOther);
    /// Overloading operators require access to aOther's private members
//    double & getStart() { return  *mStart; }
//    double & getInterval() { return *mBinsInterval; }
//    unsigned int & GetBinContent
    /// -----------------
    ~Histogram();

	void Fill(const double aValue);
	const unsigned int GetTotalCount(void);
    const unsigned int GetBinContent(const unsigned int aIdx)  const;
    const double GetBinStart(const unsigned int aIdx) const;
	void Clear(void);
	// Set a specific bin to a specific value (for Interpolate only)
	void SetValue(const unsigned int aIdx, const unsigned int aValue);
	const unsigned int GetTotalAboveThreshold(const unsigned int aValue);

	void WriteCsv(std::ofstream& aOutFile);
	void BinaryWriteBins(std::ofstream& aOutFile);
	void BinaryWriteContent(std::ofstream& aOutFile);
    /// HexitecGigE Added:
    int BinaryCopyBins(char* aBuffer);
    int BinaryCopyContent(char* aBuffer);
private:

	double mStart, mBinsInterval;
	unsigned int mBins;
	unsigned int* mFrequency;
};

#endif /* HISTOGRAM_H_ */
