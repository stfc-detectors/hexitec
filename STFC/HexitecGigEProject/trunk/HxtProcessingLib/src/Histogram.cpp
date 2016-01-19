/*
 * Histogram - a histogram class, creates and manages a simple
 * histogram between start and end (lower and upper) limits with
 * a specified number of bins.
 *
 *  Created on: 10 Dec 2010
 *      Author: Tim Nicholls
 */
#include <iostream>
#include "Histogram.h"

//#include <QDebug>
#include "HxtRawDataTypes.h"

/// Histogram constructor - creates and initialises to zero
/// a histogram with a specified number of bins between
/// start and end values
/// @param aStart start (low) limit of histogram
/// @param aEnd   end (high) limit of histogram
/// @param aBins  number of bins
Histogram::Histogram(const double aStart,
		             const double aEnd,
		             const unsigned int aBins):
		             mStart(aStart),
		             mBinsInterval(aBins/(aEnd - aStart)),
		             mBins(aBins),
		             mFrequency( new unsigned int[aBins])
{
	for (unsigned int i = 0; i < aBins; i++) {
		mFrequency[i] = 0;
	}
}

/// Copy constructor - copies one histogram to another
/// @param other reference to existing histogram
Histogram::Histogram(const Histogram& aOther):
		             mStart(aOther.mStart),
                     mBinsInterval(aOther.mBinsInterval),
                     mBins(aOther.mBins),
                     mFrequency( new unsigned int[mBins] )
{
   for(unsigned int i = 0; i < mBins; i++)
      mFrequency[i] = aOther.mFrequency[i];
}

/// ----    HexitecGigE Addition    ----
/// Plus operator - adds histogram onto current histogram
/// @param
Histogram Histogram::operator+ (const Histogram& aOther)
{
    unsigned int aValue = -1;
    // Check both histograms compatible before addition
    for (unsigned int iBin=0; iBin < mBins; iBin++)
    {
        if (GetBinStart(iBin) != aOther.GetBinStart(iBin))
            std::cout << " *** Op+   Error: Histograms not compatible; Bin start values differ: "
                 << GetBinStart(iBin) << " vs " << aOther.GetBinStart(iBin) << std::endl;

        aValue = this->GetBinContent(iBin) + aOther.GetBinContent(iBin);
        this->SetValue(iBin, aValue);
    }

    return *this;
}

/// Equal operator - copies histogram over current histogram
/// @param
Histogram & Histogram::operator= (const Histogram& aOther)
{
    unsigned int aValue = -1;
    // Check both histograms compatible before addition
    for (unsigned int iBin=0; iBin < mBins; iBin++)
    {
        if (this->GetBinStart(iBin) != aOther.GetBinStart(iBin))
            std::cout << " *** Equal = Op Error: Histograms not compatible; Bin start values differ: "
                 << GetBinStart(iBin) << " vs " << aOther.GetBinStart(iBin) << std::endl;

        aValue = aOther.GetBinContent(iBin);
        this->SetValue(iBin, aValue);
    }

    return *this;
}



/// Destructor - deletes frequency array
Histogram::~Histogram() {
	// Delete frequency array
	delete [] mFrequency;
}

/// Fills value into histogram. This method will ignore any values
/// outside the range of the histogram
/// @param aVal  value to fill
void Histogram::Fill(const double aVal) {

	const unsigned int idx = static_cast<unsigned int>((aVal-mStart) * mBinsInterval);

	if( idx < mBins ) mFrequency[idx]++;

	return;
}

/// Clears the contents of the histogram
void Histogram::Clear(void) {

	for (unsigned int i = 0; i < mBins; i++) {
		mFrequency[i] = 0;
	}
}

/// Set a specific bin to a specific value (for Interpolate only)
void Histogram::SetValue(const unsigned int aIdx, const unsigned int aValue) {

	if (aIdx < mBins) mFrequency[aIdx] = aValue;

}
/// Gets the sum of all counts in the histogram
/// @return total counts in histogram
const unsigned int Histogram::GetTotalCount(void)
{
   unsigned int count = 0;
   for( unsigned int i = 0; i < mBins; i++ ) {
      count += mFrequency[i];
   }
   return count;
}

/// Gets the start (low) limit of a given histogram bin
/// @param aIdx index of bin to return
/// @return start (lower) limit of histogram
const double Histogram::GetBinStart(const unsigned int aIdx) const {

	double start = 0.0;
	if (aIdx < mBins) start = ((double)aIdx / mBinsInterval) + mStart;

	return start;
}

/// Gets the content of a given histogram bin
/// @param aIdx index of bin to return
/// @return number of counts in specified bin
const unsigned int Histogram::GetBinContent(const unsigned int aIdx) const {

	unsigned int value = 0;
	if (aIdx < mBins) value = mFrequency[aIdx];

	return value;
}

/// Count total number of hits above threshold aValue
/// @param aValue Add together everything above aValue
/// @return number of count above threshold aValue
const unsigned int Histogram::GetTotalAboveThreshold(const unsigned int aValue) {

	const unsigned int idx = static_cast<unsigned int>((aValue-mStart) * mBinsInterval);
	unsigned int total = 0;
	if( idx < mBins )
	{
		for (unsigned int i = idx; i < mBins; i++)		
			total += mFrequency[i];
	}
	else
        std::cout << "Error: Interpolate Threshold exceeding maximum bin (" << mBins << ") !\n";

	return total;
}

/// Writes a CSV (comma-separated values) formatted output of the
/// histogram to the specified output stream
/// @param aOutFile output file stream to write to
void Histogram::WriteCsv(std::ofstream& aOutFile) {

	for (unsigned int iBin = 0; iBin < mBins; iBin++) {
		aOutFile << this->GetBinStart(iBin) << "," << this->GetBinContent(iBin) << std::endl;
	}

}

void Histogram::BinaryWriteBins(std::ofstream& aOutFile) {

	for (unsigned int iBin = 0; iBin < mBins; iBin++) {
		double binStart = this->GetBinStart(iBin);
		aOutFile.write((char*)&binStart, sizeof(binStart));
	}

}

void Histogram::BinaryWriteContent(std::ofstream& aOutFile) {

	for (unsigned int iBin = 0; iBin < mBins; iBin++) {
		double content = (double)this->GetBinContent(iBin);
		aOutFile.write((char*)&content, sizeof(content));
	}

}

int Histogram::BinaryCopyBins(char* aBuffer) {

    double* pBuffer = (double*) aBuffer;

    int numberBytesCopied = -1;
    numberBytesCopied = mBins * sizeof(double);

    for (unsigned int iBin = 0; iBin < mBins; iBin++) {
        double binStart = this->GetBinStart(iBin);
        memcpy((void*)pBuffer, (void*)&binStart, sizeof(binStart));
        pBuffer++;
    }
    return numberBytesCopied;
}

int Histogram::BinaryCopyContent(char* aBuffer) {

    double* pBuffer = (double*) aBuffer;

    int numberBytesCopied = 0;

    for (unsigned int iBin = 0; iBin < mBins; iBin++) {

        double content = (double)this->GetBinContent(iBin);
        memcpy(pBuffer, (void*)&content, sizeof(content));
        pBuffer++;
        numberBytesCopied += 8;
    }

    return numberBytesCopied;
}
