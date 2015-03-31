/*
 * HxtRawDataProcessor.h - header file for HxtRawDataProcessor class
 *
 *  Created on: 13 Dec 2010
 *      Author: Tim Nicholls
 */

#ifndef HXTFILEPARSER_H_
#define HXTFILEPARSER_H_

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>

#include "HxtLogger.h"
#include "HxtRawDataTypes.h"
#include "HxtFrame.h"
#include "HxtDecodedFrame.h"
#include "HxtFrameCorrector.h"
#include "HxtFrameIncompleteDataCorrector.h"
#include "HxtPixelThreshold.h"
#include "HxtOutputFormat.h"
#include "Histogram.h"
#include "Timer.h"

using namespace std;

namespace hexitech {

class HxtRawDataProcessor {
public:
    HxtRawDataProcessor(unsigned int aRows, unsigned int aCols, double aHistoStart = 0.0, double aHistoEnd = 10000.0, unsigned int aHistoBins = 1000,
                        u64 formatVersion=1,
                        int ssx=-1, int ssy=-1, int ssz=-1, int ssrot=-1, int timer=-1, int galx=-1, int galy=-1, int galz=-1, int galrot=-1,
                        string filePrefix="", string dataTimeStamp=0);
	virtual ~HxtRawDataProcessor();

	void setDebug(bool aDebug);

	void setCsaCorrector(bool aCaCorrector);
	void setVector(bool aEnableVector);
	void InterpolateDeadPixels(const unsigned int aThreshold);

	void registerCorrector(HxtFrameCorrector* apCorrector);

	void applyPixelThresholds(HxtPixelThreshold* apThreshold);

	bool parseFile(string aFileName);
	bool parseFile(vector<string> aFileList);
	bool flushFrames(void);

	bool outputFrame(HxtFrame* pSubPixelFrame);
	bool outputFrame(unsigned int aFrameIdx);
	bool writePixelOutput(string aOutputFileName);
	bool writeSubPixelOutput(string aOutputFileName);
	bool writeCsvFiles(void);

	HxtFrame* getSubPixelFrame();

    // Debugging functions - may be useful in the future?
    unsigned int calculateCurrentHeaderSize(string label);
    unsigned int calculateBodySize();

    /// Set and control header entries introduced by Format Version 2
    // Configure prefix, motorPositions, etc according to selections made by functions (that control bools) below
    void updateFilePrefix(string filePrefix);
    void updateMotorPositions(int ssx, int ssy, int ssz, int ssrot, int timer, int galx, int galy, int galz, int galrot);
    void updateTimeStamp(string timeStamp);
    ///

    // Access CSV file names
    string getRawCsvFileName() { return mRawCsvFileName; }
    string getCorCsvFileName() { return mCorCsvFileName; }


private:
	bool processFrame(unsigned int currentFrameIdx, unsigned int lastFrameIdx);

	/// dumpRawLine - inline function to print raw Hxt data line
	inline void dumpRawLine(hxtRawLine* apLine, LogLevel aLevel) {
		LOG(gLogConfig, aLevel) << setw(3) << (int)apLine->byte[0] << " " << setw(3) << (int)apLine->byte[1] << " " << setw(3) << (int)apLine->byte[2] << " ";
	}

	/// isFramePreamble - inline function to detect if raw line is a frame preamble
	inline bool isFramePreamble(hxtRawLine* apLine) {
		return ((apLine->byte[0] == kHxtFramePreambleLabel) &&
				(apLine->byte[1] == kHxtFramePreambleLabel) &&
				(apLine->byte[2] == kHxtFramePreambleLabel));
	}

	/// isRowMarker - inline function to detect if raw line is a row marker
	inline bool isRowMarker(hxtRowMarker* apRow) {
		return ((apRow->pad == 0) && (apRow->rowLabel == kHxtRowMarkerLabel));
	}

	// getPixelThreshold - inline function to retreive pixel threshold indexed by row and column
	inline double getPixelThreshold(unsigned int aRow, unsigned int aCol) {
		return mPixelThreshold[(aRow * mCols) + aCol];
	}

	virtual inline unsigned int pixelAddress(int aRow, int aCol) {
		return ((aRow * mCols) + aCol);
	}

    string mCsvFileName;

    unsigned int mRows;     // number of decoded pixel rows
	unsigned int mCols;     // number of decoded pixel columns
	unsigned int mPixels;   // number of decoded pixels in sensor

	unsigned int mSubPixelRows;     // number of subpixel rows
	unsigned int mSubPixelCols;     // number of subpixel columns
	unsigned int mSubPixelPixels;   // number of subpixel in sensor

	double       mHistoStart; // histogram start value
	double       mHistoEnd;   // histogram end value
	unsigned int mHistoBins;  // number of bins in histogram

	unsigned int   mFramesDetected;       // number of frames detected in parser
	unsigned int   mEventsDetected;       // number of events detected in parser
	unsigned int   mEventsAboveThreshold; // number of events above threshold

	bool           mReadNextLine ;        // flag if parser should read next line at next pass
	bool           mAtStartOfFiles;       // flag if parser as at start of file list
	int            mRowIdx;               // current row index in parser state
	hxtParserState mNextParserState;      // next state of parser, i.e. what sort of line to detect
	hxtRawLine     mPartialFrameHeader;   // raw line to hold partial frame header that spans file boundary

	unsigned int   mCorrectedFramesWritten;   // number of frames written to output histograms
	unsigned int   mSubPixelFramesWritten;   // number of frames written to output histograms

	bool mDebug;   // debug flag

	bool mCaCorrector;	// Is Charge Sharing Addition SubPixel enabled?
	bool mEnableVector;	// Is Vector enabled?

	// Temporary histograms for raw, corrected and subpixel data from all pixels
	Histogram*   mGlobalRawHisto;
	Histogram*   mGlobalDecodedHisto;
	Histogram*   mGlobalSubPixelHisto;

	// Pair of decoded frame stores for current and last frames
	HxtDecodedFrame* mDecodedFrame[2];

	// Create subpixel frame [240x240] where [3x3] for each pixel of decoded frame [82x82]
	HxtFrame* mSubPixelFrame;

	// Vector of per-pixel histograms for output of corrected data
	vector<Histogram*> mPixelHistogram;

	// Vector of per-pixel histograms for output of subpixel data
	vector<Histogram*> mSubPixelHistogram;

	// Vector of frame correctors to be applied to the data
	vector<HxtFrameCorrector*> mFrameCorrector;

	// Array of pixel thresholds to be applied to the data
	double* mPixelThreshold;

	// Timer for timing file processing operations
	Timer*       mFileTimer;

	// Memory to and store raw lines of data
	u8* mRawData;

    // File format version
    u64 mFormatVersion;

    // Variables to save filename prefix, motor position and timestamp
    int mSSX;
    int mSSY;
    int mSSZ;
    int mSSROT;
    int mTimer;
    int mGALX;
    int mGALY;
    int mGALZ;
    int mGALROT;
    string mFilePrefix;
    string mDataTimeStamp;

    // Record CSV file names to enables external access
    string mRawCsvFileName;
    string mCorCsvFileName;

};

}

#endif /* HXTFILEPARSER_H_ */
