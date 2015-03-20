/*
 * HxtRawDataProcessor.cpp - implementation of the HxtRawDatProcessor class, which is
 * responsible for parsing and processing Hexitech raw data files.
 *
 *  Created on: 13 Dec 2010
 *      Author: Tim Nicholls
 */

#include "HxtRawDataProcessor.h"
#include <string.h>

using namespace std;

namespace hexitech {

/// Constructor for HxtRawDataProcessor - initialises internal state of the processor (which can
/// persist across invocations to allow for parsing of frames straddling file boundaries) and
/// books histograms etc needed for decoded data
HxtRawDataProcessor::HxtRawDataProcessor(unsigned int aRows, unsigned int aCols,
                                         double aHistoStart, double aHistoEnd,
                                         unsigned int aHistoBins,
                                         u64 formatVersion,
                                         int ssx, int ssy, int ssz, int ssrot,
                                         int timer,
                                         int galx, int galy, int galz, int galrot,
                                         string filePrefix, string dataTimeStamp) :
                            mRows(aRows),
                            mCols(aCols),
                            mPixels(aRows * aCols),
                            mSubPixelRows(aRows*3),
                            mSubPixelCols(aCols*3),
                            mSubPixelPixels(mSubPixelRows*mSubPixelCols),
                            mHistoStart(aHistoStart),
                            mHistoEnd(aHistoEnd),
                            mHistoBins(aHistoBins),
                            mPixelThreshold(new double[aRows * aCols]),
                            mFormatVersion(formatVersion),
                            mSSX(ssx),
                            mSSY(ssy),
                            mSSZ(ssz),
                            mSSROT(ssrot),
                            mTimer(timer),
                            mGALX(galx),
                            mGALY(galy),
                            mGALZ(galz),
                            mGALROT(galrot),
                            mFilePrefix(filePrefix),
                            mDataTimeStamp(dataTimeStamp)
{
    if (mDebug) LOG(gLogConfig, logDEBUG2) << "HxtRawDataProcessor constructor";
	// Debugging, verbose output disabled by default
	mDebug = false;

	// Initialise frame and event counters
	mFramesDetected = 0;
	mEventsDetected = 0;
	mCorrectedFramesWritten  = 0;
	mSubPixelFramesWritten = 0;
	mEventsAboveThreshold = 0;

	// Initialise parser state so it is preserved across files. Assumes
	// that first line of first file will be a frame preamble
	mReadNextLine = true;
	mAtStartOfFiles = true;
	mRowIdx = -1;
	mNextParserState = framePreamble;
	mPartialFrameHeader.byte[0] = mPartialFrameHeader.byte[1] = mPartialFrameHeader.byte[2] = 0;

	// Create file processing timer
	mFileTimer = new Timer();

	// Create global raw and corrected histograms  - ONLY needed for debugging
	mGlobalRawHisto = new Histogram(mHistoStart, mHistoEnd, mHistoBins);
	mGlobalDecodedHisto = new Histogram(mHistoStart, mHistoEnd, mHistoBins);
	mGlobalSubPixelHisto = new Histogram(mHistoStart, mHistoEnd, mHistoBins);

	// Create pair of decoded frame stores. These are defined to be bigger than the real
	// size in each dimension by two pixels to allow for pixel addressing starting from
	// one and to give an empty guard band around which we can do searches for charge sharing etc
	mDecodedFrame[0] = new HxtDecodedFrame(mRows, mCols);
	mDecodedFrame[1] = new HxtDecodedFrame(mRows, mCols);

	// Create subpixel frame here
	mSubPixelFrame = new HxtFrame(mSubPixelRows, mSubPixelCols);
	
	// Decoded Frame: Create histogram for each pixel and initialize pixel thresholds to zero
	for (unsigned int iPixel = 0; iPixel < mPixels; iPixel++) {
		Histogram* pixelHisto = new Histogram(mHistoStart, mHistoEnd, mHistoBins);
		mPixelHistogram.push_back(pixelHisto);
		mPixelThreshold[iPixel] = 0.0;
	}
	
	// SubPixels Frame: Create histogram for each pixel and initialize pixel thresholds to zero
	for (unsigned int iPixel = 0; iPixel < mSubPixelPixels; iPixel++) {
		Histogram* pixelHisto = new Histogram(mHistoStart, mHistoEnd, mHistoBins);		
		mSubPixelHistogram.push_back(pixelHisto);
	}

	// Charge Sharing Addition Sub Pixel Corrector disabled by default
	mCaCorrector = false;
	// Vector disabled by default
	mEnableVector = false;

	// Allocate memory to read at most two lines of data
	const size_t rawLineSize = sizeof(hxtRawLine);
    mRawData = new u8[rawLineSize * 2];

    mDateTimeString = currentDateTime();

    string mRawCsvFileName = string("");
    string mCorCsvFileName = string("");
}

/// Destructor for HxtRawDataProcessor - deletes histogram objects etc
HxtRawDataProcessor::~HxtRawDataProcessor() {

	delete mGlobalRawHisto;
	delete mGlobalDecodedHisto;
	delete mGlobalSubPixelHisto;

	delete mFileTimer;

	// Delete decoded frames
	delete mDecodedFrame[0];
	delete mDecodedFrame[1];

	// Delete SubPixel frames
	delete mSubPixelFrame;
	
	// Delete histogram objects
	for (unsigned int iPixel = 0; iPixel < mPixels; iPixel++) {
		delete mPixelHistogram[iPixel];
	}
	mPixelHistogram.clear();

	for (unsigned int iPixel = 0; iPixel < mSubPixelPixels; iPixel++) {
		delete mSubPixelHistogram[iPixel];
	}
	mSubPixelHistogram.clear();

	// Delete pixel thresholds
	delete [] mPixelThreshold;


	// Delete raw line storage
	delete [] mRawData;
}

/// currentDateTime - Construct current date/time into string
/// Get current date/time, format is YYYYMMDD_HHMMSS
const std::string HxtRawDataProcessor::currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    localtime_s(&tstruct, &now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", &tstruct);

    return buf;
}

/// setDebug - enable debug output
/// @param aDebug set to true to enable debug output
void HxtRawDataProcessor::setDebug(bool aDebug) {
	mDebug = aDebug;
}

/// setCaCorrector - enable Charge Sharing Addition Sub Pixel Corrector
/// @param aCaCorrector set to true to enable Charge Sharing Addition Sub Pixel Corrector
void HxtRawDataProcessor::setCsaCorrector(bool aCsaCorrector) {
	mCaCorrector = aCsaCorrector;
}

/// setVector - enable Vector
/// @param aEnableVector set to true to enable Vector (instead of looping every pixel in a frame)
void HxtRawDataProcessor::setVector(bool aEnableVector) {
	mEnableVector = aEnableVector;

	// If Vector Enabled, notify HxtDecodedFrame objects
	if (aEnableVector)
	{
		mDecodedFrame[0]->setVector(aEnableVector);
		mDecodedFrame[1]->setVector(aEnableVector);
	}
}

/// registerCorrector - register a frame corrector object (a class derived from
/// HxtFrameCorrector) with the raw data processor to be applied to decoded
/// frames.
/// NB The order correctors are registered in IS IMPORTANT, as they are called
/// in the order they are registered
/// @param apCorrector pointer to HxtFrameCorrector derived corrector object
void HxtRawDataProcessor::registerCorrector(HxtFrameCorrector* apCorrector)
{
	// Push the corrector object onto the vector
	mFrameCorrector.push_back(apCorrector);
}

/// applyPixelThresholds - apply a set of pixel thresholds to be used during
/// processing of raw data.
/// @arg apThreshold pointer to HxtPixelThreshold object containing the thresholds
void HxtRawDataProcessor::applyPixelThresholds(HxtPixelThreshold* apThreshold)
{
	for (unsigned int iPixel = 0; iPixel < mPixels; iPixel++) {
		mPixelThreshold[iPixel] = apThreshold->getPixelThreshold(iPixel);
	}
}

/// parseFile - this is the heart of HxtRawDataProcessor, which parses a
/// raw data file using a state-driven idiom that detects the different
/// types of raw lines in the file and moves from one state to another. Once
/// complete frames have been parsed, they are passed on for further correction,
/// output etc
/// @param aFileName string of file name (fully qualified to path)
/// @return boolean flag indicating if processing completed OK
bool HxtRawDataProcessor::parseFile(string aFileName) {

	// Start file timer
	mFileTimer->start();

	// Open data file, binary mode, reading
	ifstream datFile(aFileName.c_str(), ios::in | ios::binary);
	if (!datFile.is_open()) {
		LOG(gLogConfig, logERROR) << "Failed to open raw data file " << aFileName;
		return false;
	}
    LOG(gLogConfig, logINFO) << "Opened data file " << aFileName << " OK";

	// Frame, row and column indices

    u64 frameIdx = -1;
	int colIdx = -1;

	// Allocate memory to read at most two lines of data
	const size_t rawLineSize = sizeof(hxtRawLine);

	// Initialise internal parser state and counters
	bool parseOK = true;
	bool lineReadOK = true;
	int framesDetected = 0;
	int eventsDetected = 0;

	// Set current frame index modulo number of current frames
	unsigned int currentFrameIdx = mFramesDetected % 2;
	unsigned int lastFrameIdx    = 1 - currentFrameIdx;

	LOG(gLogConfig, logDEBUG2) << "Parser start: mFramesDetected = " << mFramesDetected << " currentFrameIdx = " << currentFrameIdx << " lastFrameIdx = " << lastFrameIdx;

	// Loop over data file while the parser is still running OK
	while (!datFile.eof() && parseOK) {

		// Read in a raw line if required
		if (mReadNextLine) {

			// Read in a line of data
			datFile.read(reinterpret_cast<char*>(mRawData), rawLineSize);

			// Check if EOF was reached during last read, in which case we don't evaluate the parser state
			if (datFile.eof()) {
					LOG(gLogConfig, logDEBUG2) << "End of file detected during line read, parser state = " << mNextParserState;
					lineReadOK = false;
			}

			if (mDebug) dumpRawLine(reinterpret_cast<hxtRawLine*>(mRawData), logDEBUG3);
		}

		if (lineReadOK) {

			// Decode raw lines according to current state of parser
			switch( mNextParserState ) {

			case framePreamble:

				{
					// Have read a single raw line
					hxtRawLine* theLine = reinterpret_cast<hxtRawLine*>(mRawData);

					// Detect frame preamble, which is all bytes = 255
					if (isFramePreamble(theLine)) {
						if (mDebug) LOG(gLogConfig, logDEBUG2) << "Frame preamble detected";

						mNextParserState = frameHeaderStart;
						mReadNextLine = true;
						mAtStartOfFiles = false;

						// Have a new frame now, so can perform corrections on on any previously processed frames
						this->processFrame(currentFrameIdx, lastFrameIdx);

						// Clear values from last frame, ready for use in next
						mDecodedFrame[lastFrameIdx]->clear();

						// Toggle current and last frame index
						currentFrameIdx = 1 - currentFrameIdx;
						lastFrameIdx    = 1 - currentFrameIdx;

						// Increment total frames detected count
						mFramesDetected++;


					} else {
						if (mAtStartOfFiles) {
							if (mDebug) LOG(gLogConfig, logDEBUG2) << "Waiting for frame preamble at start of file";
						}
						else {
							LOG(gLogConfig, logERROR) << "Expecting frame preamble";
							parseOK = false;
						}
					}
				}
				break;

			case frameHeaderStart:

				{
					// Have read a the first line (bottom half) of frame header, so store it
					memcpy(&mPartialFrameHeader, mRawData, sizeof(hxtRawLine));

					mNextParserState = frameHeaderEnd;
					mReadNextLine = true;

				}
				break;

			case frameHeaderEnd:

				{
					//Reset frame counter to zero first though so we don't mix old bits in
					frameIdx = 0;

					// Have read a second line (top half) of frame header, so build frame number from both halves
					memcpy(&frameIdx, (u8*)&mPartialFrameHeader, sizeof(hxtRawLine));
					memcpy((((u8*)&frameIdx) + sizeof(hxtRawLine)), mRawData, sizeof(hxtRawLine));

					if (mDebug) LOG(gLogConfig, logDEBUG2) << "Frame number " << frameIdx;

					// Set frame index in current decoded frame
					mDecodedFrame[currentFrameIdx]->setFrameIndex(frameIdx);

					framesDetected++;

					mNextParserState = rowMarker;
					mReadNextLine = true;
				}
				break;

			case rowMarker:

				{
					// Have read a single line of data - cast to row marker
					hxtRowMarker* theRow = reinterpret_cast<hxtRowMarker*>(mRawData);

					// Detect row marker, which is byte0 = 0, byte1 = 192, byte2 = row
					if (isRowMarker(theRow)) {
						mRowIdx = theRow->rowIdx;
						if (mDebug) LOG(gLogConfig, logDEBUG2) << "Row number " << mRowIdx;

						mNextParserState = colData;
						mReadNextLine = true;
					} else {
						LOG(gLogConfig, logERROR) << "Expecting row marker" << endl;
						parseOK = false;
					}
				}
				break;

			case colData:

				{
					// Have read a single raw line - could be frame preamble, row marker or column data
					hxtRawLine* theLine = reinterpret_cast<hxtRawLine*>(mRawData);

					if (isFramePreamble(theLine)) {
						mNextParserState = framePreamble;
						mReadNextLine = false;
					}
					else if (isRowMarker(reinterpret_cast<hxtRowMarker*>(theLine))) {
						mNextParserState = rowMarker;
						mReadNextLine = false;
					}
					else {
						hxtColumnData* theData = reinterpret_cast<hxtColumnData*>(mRawData);
						colIdx = theData->colIdx;
						if (mDebug) LOG(gLogConfig, logDEBUG3) << "Row " << setw(2) << mRowIdx << " col " << setw(2) << colIdx << " pixel value " << setw(6) << theData->pixel;

						// Sanity check row and column indices
						if (((unsigned int)mRowIdx > mRows) || ((unsigned int)colIdx > mCols)) {

							if (mDebug) LOG(gLogConfig, logERROR) << "Illegal row (" << mRowIdx << ") or column (" << colIdx << ") index on decoded column data line";
							if (mDebug) dumpRawLine(theLine, logERROR);

						} else {
							// If pixel value is above threshold,  fill value into global raw histogram and decoded frame
							if ((double)theData->pixel > this->getPixelThreshold(mRowIdx, colIdx)) {
								mGlobalRawHisto->Fill((double)(theData->pixel));
								mDecodedFrame[currentFrameIdx]->setPixel(mRowIdx, colIdx, (double)(theData->pixel));
								mEventsAboveThreshold++;
							}
							eventsDetected++;
							mEventsDetected++;

						}
						mNextParserState = colData;
						mReadNextLine = true;
					}
				}

				break;

			default:
				// Should never happen!
				LOG(gLogConfig, logERROR) << "Unknown parser state " << mNextParserState;
				parseOK = false;
				break;

			} // switch( mNextParserState )

		} // if (lineReadOK)

	} // while (!datFile.eof() && parseOK)

	LOG(gLogConfig, logDEBUG2) << "Parser finish: mFramesDetected = " << mFramesDetected << " currentFrameIdx = " << currentFrameIdx << " lastFrameIdx = " << lastFrameIdx;

	// Close file
	datFile.close();

	// Stop timer
	mFileTimer->stop();

	// Print processing summary
	if (parseOK) {
		LOG(gLogConfig, logINFO) << "Processed file " << aFileName << " in " << mFileTimer->elapsed() << " secs, frames read: " << framesDetected << " events read: " << eventsDetected;
	} else {
		LOG(gLogConfig, logERROR) << "Processing file " << aFileName << " failed after frames read: " << framesDetected << " events read: " << eventsDetected;
	}

	return parseOK;
}

/// parseFile - overloaded version of parseFile method, which takes a vector
/// of files and iterates the parser over them
/// @param aFileList vector of string of file names to process
/// @return boolean flag indicating if parsing succeeded
bool HxtRawDataProcessor::parseFile(vector<string> aFileList) {

	bool processOK = true;
	unsigned int numFilesProcessed = 0;

	// Loop over vector of file names, processing each in turn
	vector<string>::iterator fileIterator;
    for (fileIterator = aFileList.begin(); fileIterator != aFileList.end(); fileIterator++)
    {
        // Process the file
		processOK = this->parseFile(*fileIterator);
		if (!processOK) break;
		numFilesProcessed++;
	}

	LOG(gLogConfig, logINFO) << "Completed processing " << numFilesProcessed << " files";

	return processOK;
}


/// processFrame - apply corrections and output decoded frames. This method takes pairs of
/// decoded frames and applies any registered corrections to them, before filling the first
/// of the pair into the output histograms. This allows n/n+1 corrections to be applied before committing
/// the frames to the output
/// @param currentFrameIndex index of current frame in decoded frame buffers
/// @param lastFrameIndex index of previous frame in decoded frame buffers
/// @return boolean flag indicating if processing succeeded or not (not currently implemented)
bool HxtRawDataProcessor::processFrame(unsigned int currentFrameIdx, unsigned int lastFrameIdx)
{
	// If debugging is enabled, write frame numbers and dump frame to output
	if (mFramesDetected) {
		if (mDebug) LOG(gLogConfig, logDEBUG1) << "Completed processing of frame " << mFramesDetected << " (current idx=" << currentFrameIdx << ")";
		if (mDebug) LOG(gLogConfig, logDEBUG1) << "This frame index: " << mDecodedFrame[currentFrameIdx]->getFrameIndex();
		if (mDebug) LOG(gLogConfig, logDEBUG1) << "Last frame index: " << mDecodedFrame[lastFrameIdx]->getFrameIndex();
		if (mDebug) mDecodedFrame[currentFrameIdx]->dumpFrame();
	}
	
	// If we have processed more than two frames (so that we have a last and current frame), apply corrections to the frames
	if (mFramesDetected >= 2) {
		vector<HxtFrameCorrector*>::iterator correctorIterator;
		for (correctorIterator = mFrameCorrector.begin(); correctorIterator != mFrameCorrector.end(); correctorIterator++) {
			(*correctorIterator)->apply(mDecodedFrame[lastFrameIdx], mDecodedFrame[currentFrameIdx], mSubPixelFrame);
		}

		this->outputFrame(lastFrameIdx);
	}

	// Output subpixel frame if CSA Enable
	if (mCaCorrector)
	{
		this->outputFrame(mSubPixelFrame);
		mSubPixelFrame->clear();
	}		

	return true;
}

/// outputFrame - outputs decoded and corrected frames to histograms
/// @param aFrameIdx index of frame to write
/// @return boolean flag indicating if output succeeded (not currently implemented)
bool HxtRawDataProcessor::outputFrame(unsigned int aFrameIdx)
{
	if (mDebug) LOG(gLogConfig, logDEBUG1) << "Outputting frame " << mDecodedFrame[aFrameIdx]->getFrameIndex() << " to histograms";

	for (unsigned int iRow = 0; iRow < mRows; iRow++) {
		for (unsigned int iCol = 0; iCol < mCols; iCol++) {			
			double correctedPixelValue = mDecodedFrame[aFrameIdx]->getPixel(iRow, iCol);			
			if (correctedPixelValue != 0.0) {
				mGlobalDecodedHisto->Fill(correctedPixelValue);
				unsigned int pixelAddress = (iRow * mCols) + iCol;
				mPixelHistogram[pixelAddress]->Fill(correctedPixelValue);
			}
		}
	}
	
	mCorrectedFramesWritten++;
	
	return true;
}

/// outputFrame - outputs decoded and corrected frames to histograms
/// @param aFrameIdx index of frame to write
/// @return boolean flag indicating if output succeeded (not currently implemented)
bool HxtRawDataProcessor::outputFrame(HxtFrame* apSubPixelFrame)
{
	if (mDebug) LOG(gLogConfig, logDEBUG1) << "Outputting SubPixel frame " << apSubPixelFrame->getFrameIndex() << " to histograms";

	for (unsigned int iRow = 0; iRow < mSubPixelRows; iRow++) {
		for (unsigned int iCol = 0; iCol < mSubPixelCols; iCol++) {
			double subPixelValue = apSubPixelFrame->getPixel(iRow, iCol);
			if (subPixelValue != 0.0) {
				mGlobalSubPixelHisto->Fill(subPixelValue);
				unsigned int pixelAddress = (iRow * mSubPixelCols) + iCol;
				mSubPixelHistogram[pixelAddress]->Fill(subPixelValue);
			}
		}
	}
	
	mSubPixelFramesWritten++;

	return true;
}

/// getSubPixelFrame - get Sub Pixel Frame
/// @return pointer to SubPixel Frame HxtFrame object
HxtFrame* HxtRawDataProcessor::getSubPixelFrame()
{
	return mSubPixelFrame;
}

/// flushFrames - flush pending frames through processor, correctors and output at end
/// of parsing. This method MUST be called after processing all files to ensure that
/// the frames and events at the end of the parse sequence are handled

bool HxtRawDataProcessor::flushFrames(void) {

	// Set current frame index modulo number of current frames
	unsigned int currentFrameIdx = mFramesDetected % 2;
	unsigned int lastFrameIdx    = 1 - currentFrameIdx;

	vector<HxtFrameCorrector*>::iterator correctorIterator;
	// Loop over corrections and apply them to the last two frames (i.e. current and previous)
	for (correctorIterator = mFrameCorrector.begin(); correctorIterator != mFrameCorrector.end(); correctorIterator++) {
		(*correctorIterator)->apply(mDecodedFrame[lastFrameIdx], mDecodedFrame[currentFrameIdx], mSubPixelFrame);
	}
	
	// Loop over corrections and apply to last frame (current) - other ptr is null which allows
	// corrector to handle this edge case correctly
	for (correctorIterator = mFrameCorrector.begin(); correctorIterator != mFrameCorrector.end(); correctorIterator++) {
		(*correctorIterator)->apply(mDecodedFrame[currentFrameIdx], NULL, mSubPixelFrame);
	}

	// Write last two frames
	this->outputFrame(lastFrameIdx);
	this->outputFrame(currentFrameIdx);

    LOG(gLogConfig, logINFO) << "Frames detected: " << mFramesDetected << " Frames written: " << mCorrectedFramesWritten;
    LOG(gLogConfig, logINFO) << "Events detected: " << mEventsDetected << " Events above threshold: " << mEventsAboveThreshold;
	LOG(gLogConfig, logINFO) << "Total counts in global raw spectrum = \t\t" << mGlobalRawHisto->GetTotalCount();
    LOG(gLogConfig, logINFO) << "Total counts in global corrected spectrum = \t" << mGlobalDecodedHisto->GetTotalCount();
	
	// If Charge Sharing Addition Sub Pixel enabled, display how many of each case of subpixel hit distribution
	if (mCaCorrector)
	{
		for (correctorIterator = mFrameCorrector.begin(); correctorIterator != mFrameCorrector.end(); correctorIterator++) {
			// Hack to output subpixel frame if that corrector chosen
			if (strcmp( ((*correctorIterator)->getName()).c_str(), "ChargeSharingAdditionSubPixel") == 0)
			{
				LOG(gLogConfig, logINFO) << "Total counts in global  subpixel spectrum = \t" << mGlobalSubPixelHisto->GetTotalCount();
				LOG(gLogConfig, logINFO) << "Case A: " << setw(11) << (*correctorIterator)->getCaseA();
				LOG(gLogConfig, logINFO) << "Case B: " << setw(11) << (*correctorIterator)->getCaseB();
				LOG(gLogConfig, logINFO) << "Case C: " << setw(11) << (*correctorIterator)->getCaseC();
				LOG(gLogConfig, logINFO) << "Case D: " << setw(11) << (*correctorIterator)->getCaseD();
				LOG(gLogConfig, logINFO) << "Total : " << setw(11) << (*correctorIterator)->getCaseA() + (*correctorIterator)->getCaseB() + 
					(*correctorIterator)->getCaseC() + (*correctorIterator)->getCaseD();
			}
		}
	}

	for (correctorIterator = mFrameCorrector.begin(); correctorIterator != mFrameCorrector.end(); correctorIterator++) {
		const unsigned int nCorrected = (*correctorIterator)->getNumEventsCorrected();
		LOG(gLogConfig, logINFO) << "Correction " << (*correctorIterator)->getName() << " corrected " << nCorrected << " events";
	}

	return true;
}

/// writeOutput - write the output of a raw data processing run to output files
bool HxtRawDataProcessor::writePixelOutput(string aOutputPixelFileName) {

    // Prepend current date/time onto filename
    string tempDateTime = string(mDateTimeString);
    tempDateTime.append("_" + aOutputPixelFileName);
    aOutputPixelFileName = tempDateTime;

	// Write pixel histograms out to binary file
	ofstream pixelFile;
	pixelFile.open(aOutputPixelFileName.c_str(), ios::binary | ios::out | ios::trunc);
	if (!pixelFile.is_open()) {
		LOG(gLogConfig, logERROR) << "Failed to open output file " << aOutputPixelFileName;
		return false;
	}

	// Write binary file header

	string label("HEXITECH");
	pixelFile.write(label.c_str(), label.length());

    /// Debugging purposes:
//    unsigned int headerSize = calculateCurrentHeaderSize(label);
//    unsigned int bodySize = calculateBodySize();
//    LOG(gLogConfig, logINFO) << "FORMAT VERSION: " << mFormatVersion << " HeaderSize: " << headerSize << " BodySize: " << bodySize << " Totalsize: " << headerSize+bodySize;
//    LOG(gLogConfig, logINFO) << "mSSX:             " << mSSX << "   mSSY:             " << mSSY << "   mRot1:          " << mRot1 << "   mRot2:          " << mRot2 << "   mTime:          " << mTime;
//    LOG(gLogConfig, logINFO) << "mFormatVersion " << mFormatVersion << "\tmFilePrefix " << mFilePrefix << "\tmData: " << mDataTimeStamp;
//    LOG(gLogConfig, logINFO) << "mRows:          " << mRows << "   mCols:          " << mCols << "   mHistoBins:     " << mHistoBins;

    pixelFile.write((const char*)&mFormatVersion, sizeof(u64));

    // Include File Prefix/Motor Positions/Data Time Stamp - if format version 2 selected
    if (mFormatVersion == 2)
    {
        /// motor order: mSSX, mSSY, mSSZ, mSSROT, mTimer, mGALX, mGALY, mGALZ, mGALROT
        pixelFile.write((const char*)&mSSX, sizeof(mSSX));
        pixelFile.write((const char*)&mSSY, sizeof(mSSY));
        pixelFile.write((const char*)&mSSZ, sizeof(mSSZ));
        pixelFile.write((const char*)&mSSROT, sizeof(mSSROT));
        pixelFile.write((const char*)&mTimer, sizeof(mTimer));
        pixelFile.write((const char*)&mGALX, sizeof(mGALX));
        pixelFile.write((const char*)&mGALY, sizeof(mGALY));
        pixelFile.write((const char*)&mGALZ, sizeof(mGALZ));
        pixelFile.write((const char*)&mGALROT, sizeof(mGALROT));

        // Determine length of mFilePrefix string
        int filePrefixSize = (int)mFilePrefix.size();

        // Write prefix length, followed by prefix itself
        pixelFile.write((const char*)&filePrefixSize, sizeof(filePrefixSize));
        pixelFile.write(mFilePrefix.c_str(), mFilePrefix.size());

        pixelFile.write(mDataTimeStamp.c_str(), mDataTimeStamp.size());
    }
    // Continue writing header information that is common to both format versions
	pixelFile.write((const char*)&mRows, sizeof(mRows));
	pixelFile.write((const char*)&mCols, sizeof(mCols));
	pixelFile.write((const char*)&mHistoBins, sizeof(mHistoBins));

	// Write histogram bins to file
	mPixelHistogram[0]->BinaryWriteBins(pixelFile);

	// Write pixel histograms to file
	for (unsigned int iPixel = 0; iPixel < mPixels; iPixel++) {
		mPixelHistogram[iPixel]->BinaryWriteContent(pixelFile);
	}
	pixelFile.close();

    LOG(gLogConfig, logINFO) << "Written output histogram binary file " << aOutputPixelFileName;

	return true;
}

unsigned int HxtRawDataProcessor::calculateCurrentHeaderSize(string label)
{
    /// Calculate the size of the header
    unsigned int headerSize = 0;
    headerSize += (int)label.length();
    headerSize += sizeof(mFormatVersion);
    headerSize += sizeof(mRows);
    headerSize += sizeof(mCols);
    headerSize += sizeof(mHistoBins);
    return headerSize;
}


unsigned int HxtRawDataProcessor::calculateBodySize()
{
    /// Calculate the size of the written file without the header portion
    return (sizeof(double) * mHistoBins + sizeof(double) * mHistoBins * mPixels);
}

void HxtRawDataProcessor::updateFilePrefix(string filePrefix)
{
    mFilePrefix = filePrefix;
}

void HxtRawDataProcessor::updateMotorPositions(int ssx, int ssy, int ssz, int ssrot, int timer, int galx, int galy, int galz, int galrot)
{
    mSSX    = ssx;
    mSSY    = ssy;
    mSSZ    = ssz;
    mSSROT  = ssrot;
    mTimer  = timer;
    mGALX   = galx;
    mGALY   = galy;
    mGALZ   = galz;
    mGALROT = galrot;
}

void HxtRawDataProcessor::updateTimeStamp(string timeStamp)
{
    mDataTimeStamp = timeStamp;
}

/// InterpolateDeadPixels - Workout the average of all nonzero pixel histogram surrounding a dead pixel
void HxtRawDataProcessor::InterpolateDeadPixels(const unsigned int aThreshold)
{	
	// Construct frame containing 1 = dead pixel, 0 = fine pixel
	HxtFrame* deadPixelFrame = new HxtFrame(mRows, mCols);

	// Create vector to track dead pixels
	vector<hxtPixel> deadPixels;

	// Initialise array of flags
	for (unsigned int iRow = 0; iRow < mRows; iRow++)
	{
		// Iterate over columns
		for (unsigned int iCol = 0; iCol < mCols; iCol++)
		{
			// Is this pixel dead?
			if ( mPixelHistogram[pixelAddress(iRow, iCol)]->GetTotalAboveThreshold(aThreshold) == 0 ) {
				// Create hxtPixel and add to vector
				hxtPixel thisPixel;
				thisPixel.nRow = iRow;
				thisPixel.nCol = iCol;
				deadPixels.push_back(thisPixel);
				
				// Flag pixel as dead in deadPixelFrame
				deadPixelFrame->setPixel(iRow, iCol, 1.0);
			}
		}
	}
	

	// Check whether index sitting in the first/last row/column
	int rowStart, rowEnd;
	int colStart, colEnd;

	unsigned int binTotalValue = 0;
	unsigned int iPixel = 0;
	unsigned int neighbourCount = 0;

	unsigned int iRow = 0;
	unsigned int iCol = 0;

	// Iterate over vector containing dead pixels
	vector<hxtPixel>::iterator pixelListIterator;	
	for (pixelListIterator = deadPixels.begin(); pixelListIterator != deadPixels.end();  pixelListIterator++)
	{
		iRow = pixelListIterator->nRow;
		iCol = pixelListIterator->nCol;

		if (mDebug) LOG(gLogConfig, logDEBUG1) << "Interpolate found dead pixel at (" << iRow << "," << iCol << ")"; 

		// get index for (dead)pixel's histogram
		iPixel  = pixelAddress(iRow, iCol);

		// Decide row range:	-1 = previous row, 0 = current, 1 = next	
		if (iRow == 0) {
			// pixel first in row; Only need next row
			rowStart = 0;
			rowEnd   = 1;
		}
		else if (iRow == (mRows-1)) {
			// pixel is last in row; Only need previous row
			rowStart = -1;
			rowEnd   = 0;
		}
		else {
			// pixel in neither first nor last row
			rowStart = -1;
			rowEnd   = 1;
		}

		// Decide column range
		if (iCol == 0) {
			// pixel in first column
			colStart = 0;
			colEnd   = 1;
		}
		else if (iCol == (mCols-1)) {
			// pixel in last column
			colStart = -1;
			colEnd   = 0;
		}
		else {
			// pixel in neither first nor last column
			colStart = -1;
			colEnd	 = 1;
		}

		unsigned int neighbourPixelAddress = 0;

		// Loop over rows and columns immediately surrounding dead pixel				
		for (unsigned int iBin = 0; iBin < mHistoBins; iBin++)
		{			
			// Iterate over rows
			for (int iMiniRow = rowStart; iMiniRow <= rowEnd; iMiniRow++)
			{
				// Iterate over columns
				for (int iMiniCol = colStart; iMiniCol <= colEnd; iMiniCol++)
				{
					// Do not include dead pixel itself
					if ( (iMiniRow == 0) && (iMiniCol == 0) )
						continue;

					// Check if neighbouring pixel dead
					if ( deadPixelFrame->getPixel(iRow+iMiniRow, iCol+iMiniCol) == 1.0 )
					{
						if (mDebug)
						{
							LOG(gLogConfig, logDEBUG2) << "Dead pixel at (" << iRow << "," << iCol << ") has dead neighbour at (" 
									<< iRow+iMiniRow << "," << iCol+iMiniCol << ")";
						}
					}
					else
					{
						neighbourPixelAddress = pixelAddress( (iRow+iMiniRow), (iCol+iMiniCol) );
						binTotalValue += mPixelHistogram[neighbourPixelAddress]->GetBinContent(iBin);
						neighbourCount++;
					}

				}	// iMiniCol
			}	// iMiniRow

			// Only update dead pixel if at least one neighbour not zero
			if (neighbourCount != 0)
			{
				unsigned int averageBin = binTotalValue / neighbourCount;
				if (mDebug)
				{
					LOG(gLogConfig, logDEBUG2) << "Bin " << iBin << ", surrounding neighbours averages " << averageBin;
				}

				mPixelHistogram[iPixel]->SetValue(iBin, averageBin);			
				// Reset binTotalValue and neighbourCount before next dead pixel
				binTotalValue = 0;
				neighbourCount = 0;				
			}
		}	// for(iBin)
	}	// for(pixelListIterator)

	LOG(gLogConfig, logINFO) << "Interpolate masked " << deadPixels.size() << " dead pixels";

	// Free allocated memory
	delete [] deadPixelFrame;
}

/// writeSubPixelOutput - write the output of a raw data processing run to output files
bool HxtRawDataProcessor::writeSubPixelOutput(string aOutputSubPixelFileName) {

    // Prepend current date/time onto filename
    string tempDateTime = string(mDateTimeString);
    tempDateTime.append("_" + aOutputSubPixelFileName);
    aOutputSubPixelFileName = tempDateTime;

	// Write subpixel histograms out to binary file
	ofstream subPixelFile;
	subPixelFile.open(aOutputSubPixelFileName.c_str(), ios::binary | ios::out | ios::trunc);
	if (!subPixelFile.is_open()) {
		LOG(gLogConfig, logERROR) << "Failed to open output file " << aOutputSubPixelFileName;
		return false;
	}

	// Write binary file header
	string label("HEXITECH");
	subPixelFile.write(label.c_str(), label.length());


//    u64 formatVersion = 1;
    subPixelFile.write((const char*)&mFormatVersion, sizeof(u64));

	subPixelFile.write((const char*)&mSubPixelRows, sizeof(mSubPixelRows));
	subPixelFile.write((const char*)&mSubPixelCols, sizeof(mSubPixelCols));
	subPixelFile.write((const char*)&mHistoBins, sizeof(mHistoBins));

	// Write histogram bins to file
	mSubPixelHistogram[0]->BinaryWriteBins(subPixelFile);

	// Write subpixel histograms to file
	for (unsigned int iPixel = 0; iPixel < mSubPixelPixels; iPixel++) {
		mSubPixelHistogram[iPixel]->BinaryWriteContent(subPixelFile);
	}
	subPixelFile.close();

	LOG(gLogConfig, logINFO) << "Written output histogram binary file " << aOutputSubPixelFileName;

	return true;
}

bool HxtRawDataProcessor::writeCsvFiles(void) {

	// Write CSV histogram file out
	ofstream rawCsvFile;
	string rawCsvFileName("histoRaw.csv");

    // Prepend current date/time onto filename
    string tempDateTime = string(mDateTimeString);
    tempDateTime.append("_" + rawCsvFileName);
    rawCsvFileName = tempDateTime;

    rawCsvFile.open(rawCsvFileName.c_str(), ios::out | ios::trunc);
	if (!rawCsvFile.is_open()) {
		LOG(gLogConfig, logERROR) << "Failed to open CSV file" << rawCsvFileName;
		return false;
	}

	mGlobalRawHisto->WriteCsv(rawCsvFile);
	rawCsvFile.close();

	LOG(gLogConfig, logINFO) << "Written global raw histogram to CSV file " << rawCsvFileName;

	// Write CSV histogram file out
	ofstream corCsvFile;
	string corCsvFileName("histoCorrected.csv");

    // Prepend current date/time onto filename
    tempDateTime = string(mDateTimeString);
    tempDateTime.append("_" + corCsvFileName);
    corCsvFileName = tempDateTime;

    corCsvFile.open(corCsvFileName.c_str(), ios::out | ios::trunc);
	if (!corCsvFile.is_open()) {
		LOG(gLogConfig, logERROR) <<  "Failed to open CSV file" << corCsvFileName;
		return false;
	}

	mGlobalDecodedHisto->WriteCsv(corCsvFile);
	
	corCsvFile.close();

	LOG(gLogConfig, logINFO) << "Written global corrected histogram to CSV file " << corCsvFileName;

	// Write subpixel CSV histogram file if charge sharing addition corrector enabled
	if (mCaCorrector)
	{
		ofstream corSbPxlCsvFile;
		string corSbPxlCsvFileName("histoSubPixelCorrected.csv");

		corSbPxlCsvFile.open(corSbPxlCsvFileName.c_str(), ios::out | ios::trunc);
		if (!corSbPxlCsvFile.is_open()) {
			LOG(gLogConfig, logERROR) <<  "Failed to open CSV file" << corSbPxlCsvFileName;
			return false;
		}

		mGlobalSubPixelHisto->WriteCsv(corSbPxlCsvFile);
	
		corSbPxlCsvFile.close();

		LOG(gLogConfig, logINFO) << "Written subpixel corrected histogram to CSV file " << corSbPxlCsvFileName;

	}
    // Processing CSV files successful, note filenames before returning
    mRawCsvFileName = rawCsvFileName;
    mCorCsvFileName = corCsvFileName;

    return true;
}

} // namespace
