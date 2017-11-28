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
///// For callback functionality:
//#include <functional>

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

///// HexitecGigE Edition; Defined class and create callback function
///// Source: http://stackoverflow.com/a/2298291/2903608
//class HxtRawDataProcessor;
//int dataProcessorCallback(const HxtRawDataProcessor& dp);

class HxtRawDataProcessor {
public:
    HxtRawDataProcessor(unsigned int aRows, unsigned int aCols, double aHistoStart = 0.0, double aHistoEnd = 10000.0, unsigned int aHistoBins = 1000,
                        u64 formatVersion=1,
                        int ssx=-1, int ssy=-1, int ssz=-1, int ssrot=-1, int timer=-1, int galx=-1, int galy=-1, int galz=-1, int galrot=-1,
                        string filePrefix="", string dataTimeStamp=0, bool enableCallback=false);
	virtual ~HxtRawDataProcessor();
//    /// ---------------------------------------------
//    /// typedef to support callback functionality:

//    typedef std::/*tr1::*/function<int (const HxtRawDataProcessor&)> DataProcessorFunc;
////    typedef std::tr1::function<int (const GameCharacter&)> HealthCalcFunc;

//    /// Explicit CTOR for callback functionality
//    explicit HxtRawDataProcessor(DataProcessorFunc dp = dataProcessorCallback/*, bool bEnableCallback = true*/) :
//        callbackFunc(dp)/*, mCallbackAvailable(bEnableCallback)*/ {}

//    int getCallbackFunc() const { return callbackFunc(*this); }
//    /// ---------------------------------------------

	void setDebug(bool aDebug);

	void setCsaCorrector(bool aCaCorrector);
	void setVector(bool aEnableVector);
	void InterpolateDeadPixels(const unsigned int aThreshold);

	void registerCorrector(HxtFrameCorrector* apCorrector);
    bool deregisterCorrector(HxtFrameCorrector* apCorrector);

	void applyPixelThresholds(HxtPixelThreshold* apThreshold);

	bool parseFile(string aFileName);
	bool parseFile(vector<string> aFileList);
    /// HexitecGigE Added: (2 functions)
    bool parseBuffer(unsigned short *aBufferName, unsigned long frameLength);
    bool parseBuffer(vector<unsigned short*> &aBufferNames, vector<unsigned long> &aValidFrames);
    bool flushFrames(void);

//	bool outputFrame(HxtFrame* pSubPixelFrame);
	bool outputFrame(unsigned int aFrameIdx);
    bool writeHxtBufferToFile(string aOutputFileName);  /// HexitecGigE: To replace writePixelOutput
    bool writeStructOutput(string aOutputPixelFileName);
    bool writePixelOutput(string aOutputFileName);
//	bool writeSubPixelOutput(string aOutputFileName);
    bool writeCsvFiles(void);
    /// HexitecGigE Added: (Support copying .HXT contents to buffer, not file)
    bool copyPixelOutput(unsigned short* aHxtBuffer);
    void resetHistograms();

//	HxtFrame* getSubPixelFrame();

    // Debugging functions - may be useful in the future?
    unsigned int calculateCurrentHeaderSize();
    unsigned int calculateBodySize();

    /// Set and control header entries introduced by Format Version 2
    // Configure prefix, motorPositions, etc according to selections made by functions (that control bools) below
    void updateFilePrefix(string filePrefix);
//    void updateMotorPositions(int ssx, int ssy, int ssz, int ssrot, int timer, int galx, int galy, int galz, int galrot);
    void updateTimeStamp(string timeStamp);
    void updateFormatVersion(u64 formatVersion) { mHxtBuffer.hxtVersion = formatVersion; }
    ///

    // Access CSV file names
    string getRawCsvFileName() { return mRawCsvFileName; }
    string getCorCsvFileName() { return mCorCsvFileName; }

    /// Debugging frame by frame (access the function)
    void setDebugFrameDir(string aDebugFrameDir){ mDebugFrameDir = aDebugFrameDir; }
    void setEnableDebugFrame(bool aDebugFrames) { mDebugFrames = aDebugFrames; }
private:
	bool processFrame(unsigned int currentFrameIdx, unsigned int lastFrameIdx);

    /// ----- HexitecGigE Addition: Callback functionality ---- Not Yet Implemented
//    DataProcessorFunc callbackFunc;
    bool mCallbackAvailable;
    /// ---------------------------------------------------------------------
    /// Setup lookup table for pixel reordering:
    int* mPixelTable;

    /// Debugging frame by frame
    string mDebugFrameDir;
    unsigned int mFrameNumber;
    bool mDebugFrames;
    bool debugWriteFrame(unsigned int aFrameIdx, string fileDescription);
    ///
    // getPixelThreshold - inline function to retreive pixel threshold indexed by row and column
	inline double getPixelThreshold(unsigned int aRow, unsigned int aCol) {
        return mPixelThreshold[(aRow * mHxtBuffer.nCols) + aCol];
	}

	virtual inline unsigned int pixelAddress(int aRow, int aCol) {
        return ((aRow * mHxtBuffer.nCols) + aCol);
	}

    string mCsvFileName;

    HxtBuffer mHxtBuffer;   /// HexitecGigE: to Replace contents going into file/buffer?

    /// Rows and Columns moved into mHxtBuffer
//    unsigned int mRows;     // number of decoded pixel rows
//	unsigned int mHxtBuffer.nCols;     // number of decoded pixel columns
	unsigned int mPixels;   // number of decoded pixels in sensor

//	unsigned int mSubPixelRows;     // number of subpixel rows
//	unsigned int mSubPixelCols;     // number of subpixel columns
//	unsigned int mSubPixelPixels;   // number of subpixel in sensor

	double       mHistoStart; // histogram start value
	double       mHistoEnd;   // histogram end value
    //unsigned int mHistoBins;  // number of bins in histogram - Replaced by mHxtBuffer.nBins

	unsigned int   mFramesDetected;       // number of frames detected in parser
	unsigned int   mEventsDetected;       // number of events detected in parser
	unsigned int   mEventsAboveThreshold; // number of events above threshold

    int            mRowIdx;               // current pixel row index within current frame

	unsigned int   mCorrectedFramesWritten;   // number of frames written to output histograms
//	unsigned int   mSubPixelFramesWritten;   // number of frames written to output histograms

	bool mDebug;   // debug flag

	bool mCaCorrector;	// Is Charge Sharing Addition SubPixel enabled?
	bool mEnableVector;	// Is Vector enabled?

	// Temporary histograms for raw, corrected and subpixel data from all pixels
	Histogram*   mGlobalRawHisto;
	Histogram*   mGlobalDecodedHisto;
//	Histogram*   mGlobalSubPixelHisto;

	// Pair of decoded frame stores for current and last frames
	HxtDecodedFrame* mDecodedFrame[2];

	// Create subpixel frame [240x240] where [3x3] for each pixel of decoded frame [82x82]
//	HxtFrame* mSubPixelFrame;

	// Vector of per-pixel histograms for output of corrected data
	vector<Histogram*> mPixelHistogram;

	// Vector of per-pixel histograms for output of subpixel data
//	vector<Histogram*> mSubPixelHistogram;

	// Vector of frame correctors to be applied to the data
	vector<HxtFrameCorrector*> mFrameCorrector;

	// Array of pixel thresholds to be applied to the data
	double* mPixelThreshold;

	// Timer for timing file processing operations
	Timer*       mFileTimer;

    // Memory to and store raw lines of data - Now just one 16-bit raw pixel value
    u16* mRawData;

    // File format version
//    u64 mFormatVersion;

    // Variables to save filename prefix, motor position and timestamp
//    int mSSX;
//    int mSSY;
//    int mSSZ;
//    int mSSROT;
//    int mTimer;
//    int mGALX;
//    int mGALY;
//    int mGALZ;
//    int mGALROT;
//    string mFilePrefix;
//    string mDataTimeStamp;

    // Record CSV file names to enables external access
    string mRawCsvFileName;
    string mCorCsvFileName;

};

}

#endif /* HXTFILEPARSER_H_ */
