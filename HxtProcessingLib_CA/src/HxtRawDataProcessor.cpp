/*
 * HxtRawDataProcessor.cpp - implementation of the HxtRawDatProcessor class, which is
 * responsible for parsing and processing Hexitech raw data files.
 *
 *  Created on: 13 Dec 2010
 *      Author: Tim Nicholls
 */

#include "HxtRawDataProcessor.h"
#include <string.h>
/// Debugging purposes: checking frame by frame
#include  <DateStamp.h>
/// FOR TEMPORARY DEBUGGING PURPOSES ONLY:
#include <QDebug>

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
                                         string filePrefix, string dataTimeStamp, bool enableCallback) :
                            mPixels(aRows * aCols),
                            mHistoStart(aHistoStart),
                            mHistoEnd(aHistoEnd),
                            mPixelThreshold(new double[aRows * aCols]),
                            mCallbackAvailable(enableCallback)
{
    ///Populate mHxtBuffer struct
    strncpy(mHxtBuffer.hxtLabel, "HEXITECH", 8);
    mHxtBuffer.hxtVersion = formatVersion;
//    mHxtBuffer.motorPositions[0] = ssx;
//    mHxtBuffer.motorPositions[1] = ssy;
//    mHxtBuffer.motorPositions[2] = ssz;
//    mHxtBuffer.motorPositions[3] = ssrot;
//    mHxtBuffer.motorPositions[4] = timer;
//    mHxtBuffer.motorPositions[5] = galx;
//    mHxtBuffer.motorPositions[6] = galy;
//    mHxtBuffer.motorPositions[7] = galz;
//    mHxtBuffer.motorPositions[8] = galrot;
    strncpy(mHxtBuffer.filePrefix, filePrefix.c_str(), filePrefix.size());
    mHxtBuffer.filePrefixLength = filePrefix.size();
    strncpy(mHxtBuffer.dataTimeStamp, dataTimeStamp.c_str(), dataTimeStamp.size());
    mHxtBuffer.nRows = aRows;
    mHxtBuffer.nCols = aCols;
    mHxtBuffer.nBins = aHistoBins;
    /* Using string copy above is unwieldy, but accessing struct e.g.:
     * char hxtLabel[8] = "HEXITECH";
     *  Results in:
     * cannot convert from 'const char [8]' to 'char [8]'.. (And the Internet is broken :-p)
     */

    if (mDebug) LOG(gLogConfig, logDEBUG2) << "HxtRawDataProcessor constructor";
    // Debugging, verbose output disabled by default
    mDebug = false;

    // Initialise frame and event counters
    mFramesDetected = 0;
    mEventsDetected = 0;
    mCorrectedFramesWritten  = 0;
//    mSubPixelFramesWritten = 0;
    mEventsAboveThreshold = 0;

    // Initialise parser state so it is preserved across files. Assumes
    // that first line of first file will be a frame preamble
    mRowIdx = -1;

    // Create file processing timer
    mFileTimer = new Timer();

    // Create global raw and corrected histograms  - ONLY needed for debugging
    mGlobalRawHisto = new Histogram(mHistoStart, mHistoEnd, mHxtBuffer.nBins);
    mGlobalDecodedHisto = new Histogram(mHistoStart, mHistoEnd, mHxtBuffer.nBins);
//    mGlobalSubPixelHisto = new Histogram(mHistoStart, mHistoEnd, mHxtBuffer.nBins);

    // Create pair of decoded frame stores. These are defined to be bigger than the real
    // size in each dimension by two pixels to allow for pixel addressing starting from
    // one and to give an empty guard band around which we can do searches for charge sharing etc
    mDecodedFrame[0] = new HxtDecodedFrame(mHxtBuffer.nRows, mHxtBuffer.nBins);
    mDecodedFrame[1] = new HxtDecodedFrame(mHxtBuffer.nRows, mHxtBuffer.nBins);

    // Create subpixel frame here
//    mSubPixelFrame = new HxtFrame(mSubPixelRows, mSubPixelCols);

    // Decoded Frame: Create histogram for each pixel and initialize pixel thresholds to zero
    for (unsigned int iPixel = 0; iPixel < mPixels; iPixel++) {
        Histogram* pixelHisto = new Histogram(mHistoStart, mHistoEnd, mHxtBuffer.nBins);
        mPixelHistogram.push_back(pixelHisto);
        mPixelThreshold[iPixel] = 0.0;
    }

//    // SubPixels Frame: Create histogram for each pixel and initialize pixel thresholds to zero
//    for (unsigned int iPixel = 0; iPixel < mSubPixelPixels; iPixel++) {
//        Histogram* pixelHisto = new Histogram(mHistoStart, mHistoEnd, mHxtBuffer.nBins);
//        mSubPixelHistogram.push_back(pixelHisto);
//    }

    // Charge Sharing Addition Sub Pixel Corrector disabled by default
    mCaCorrector = false;
    // Vector disabled by default
    mEnableVector = false;

    /// Cannot assign a memory here, depends whether file(s)
    ///     (u8 bytes[3] * 2) or buffer(s) (u16) processing occurring
    mRawData = new u16;      /* New raw mode uses 16 bit data */

//    mCsvFileName = string("");

//    string mRawCsvFileName = string("");
//    string mCorCsvFileName = string("");

    /// Setup lookup table for pixel reordering
    mPixelTable = 0;

    /// The pixels come in the order:
    ///  (0, 0) (0, 20) (0, 40) (0, 60) (0, 1) (0, 21) (0, 41) etc
    int index = 0;
    mPixelTable = new int[6400];
    int row=0, col=-20;
    for (index=0; index <6400; index++)
    {
        col += 20;
        if (col == 99)
        {
            row += 1;
            col = 0;
        }
        if (col >79)
            col -= 79;
        // Add value to lookup table
        mPixelTable[index] = (col + row*80);
    }
    /// For debugging purposes:
    mFrameNumber = 0;    mDebugFrames = false;    mDebugFrameDir = "C:/temp/";
}

/// Destructor for HxtRawDataProcessor - deletes histogram objects etc
HxtRawDataProcessor::~HxtRawDataProcessor() {

    delete mGlobalRawHisto;
    delete mGlobalDecodedHisto;
//    delete mGlobalSubPixelHisto;

    delete mFileTimer;

    // Delete decoded frames
    delete mDecodedFrame[0];
    delete mDecodedFrame[1];

//    // Delete SubPixel frames
//    delete mSubPixelFrame;

    // Delete histogram objects
    for (unsigned int iPixel = 0; iPixel < mPixels; iPixel++) {
        delete mPixelHistogram[iPixel];
    }
    mPixelHistogram.clear();

//    for (unsigned int iPixel = 0; iPixel < mSubPixelPixels; iPixel++) {
//        delete mSubPixelHistogram[iPixel];
//    }
//    mSubPixelHistogram.clear();

    // Delete pixel thresholds
    delete [] mPixelThreshold;

//    /// Delete raw storage - But not if parseBuffer() utilised
//    if (!mCallbackAvailable)
//        delete mRawData;

    // Delete lookup table's array
    delete [] mPixelTable;
}

void HxtRawDataProcessor::resetHistograms() {

    bool bDebug = false;
    if (bDebug)
    {
        LOG(gLogConfig, logINFO) << "PRE-RESET: Frames detected: " << mFramesDetected << " Frames written: " << mCorrectedFramesWritten;
        LOG(gLogConfig, logINFO) << "PRE-RESET: Events detected: " << mEventsDetected << " Events above threshold: " << mEventsAboveThreshold;
        LOG(gLogConfig, logINFO) << "PRE-RESET: Total counts in global raw spectrum = \t\t" << mGlobalRawHisto->GetTotalCount();
        LOG(gLogConfig, logINFO) << "PRE-RESET: Total counts in global corrected spectrum = \t" << mGlobalDecodedHisto->GetTotalCount();
    }

    vector<HxtFrameCorrector*>::iterator correctorIterator;
    for (correctorIterator = mFrameCorrector.begin(); correctorIterator != mFrameCorrector.end(); correctorIterator++) {
        if (bDebug) {
            const unsigned int nCorrected = (*correctorIterator)->getNumEventsCorrected();
            LOG(gLogConfig, logINFO) << "PRE-RESET: Correction " << (*correctorIterator)->getName() << " corrected " << nCorrected << " events";
        }
        /// Reset number of events as we go along in this loop..
        (*correctorIterator)->resetNumEventsCorrected();
    }

    /// Begin by freeing histograms' memory
    delete mGlobalRawHisto;
    delete mGlobalDecodedHisto;
//    delete mGlobalSubPixelHisto;

    // Delete histogram objects
    for (unsigned int iPixel = 0; iPixel < mPixels; iPixel++) {
        delete mPixelHistogram[iPixel];
    }
    mPixelHistogram.clear();

//    for (unsigned int iPixel = 0; iPixel < mSubPixelPixels; iPixel++) {
//        delete mSubPixelHistogram[iPixel];
//    }
//    mSubPixelHistogram.clear();

    /// Recreate histograms

    // Create global raw and corrected histograms  - ONLY needed for debugging
    mGlobalRawHisto = new Histogram(mHistoStart, mHistoEnd, mHxtBuffer.nBins);
    mGlobalDecodedHisto = new Histogram(mHistoStart, mHistoEnd, mHxtBuffer.nBins);
//    mGlobalSubPixelHisto = new Histogram(mHistoStart, mHistoEnd, mHxtBuffer.nBins);

    // Decoded Frame: Create histogram for each pixel
    for (unsigned int iPixel = 0; iPixel < mPixels; iPixel++) {
        Histogram* pixelHisto = new Histogram(mHistoStart, mHistoEnd, mHxtBuffer.nBins);
        mPixelHistogram.push_back(pixelHisto);
    }

//    // SubPixels Frame: Create histogram for each pixel
//    for (unsigned int iPixel = 0; iPixel < mSubPixelPixels; iPixel++) {
//        Histogram* pixelHisto = new Histogram(mHistoStart, mHistoEnd, mHxtBuffer.nBins);
//        mSubPixelHistogram.push_back(pixelHisto);
//    }

    /// Reset variables associated with a histogram
    mFramesDetected = 0;
    mCorrectedFramesWritten = 0;
    mEventsDetected = 0;
    mEventsAboveThreshold = 0;

    if (bDebug)
    {
        LOG(gLogConfig, logINFO) << "POST-RESET: Frames detected: " << mFramesDetected << " Frames written: " << mCorrectedFramesWritten;
        LOG(gLogConfig, logINFO) << "POST-RESET: Events detected: " << mEventsDetected << " Events above threshold: " << mEventsAboveThreshold;
        LOG(gLogConfig, logINFO) << "POST-RESET: Total counts in global raw spectrum = \t\t" << mGlobalRawHisto->GetTotalCount();
        LOG(gLogConfig, logINFO) << "POST-RESET: Total counts in global corrected spectrum = \t" << mGlobalDecodedHisto->GetTotalCount();

        for (correctorIterator = mFrameCorrector.begin(); correctorIterator != mFrameCorrector.end(); correctorIterator++) {
            const unsigned int nCorrected = (*correctorIterator)->getNumEventsCorrected();
            LOG(gLogConfig, logINFO) << "POST-RESET: Correction " << (*correctorIterator)->getName() << " corrected " << nCorrected << " events";
        }
    }
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
    /// Check corrector not already registered
    bool correctorDetected = false;
    vector<HxtFrameCorrector*>::iterator correctorIterator; int i = 0;
    for (correctorIterator = mFrameCorrector.begin(); correctorIterator != mFrameCorrector.end(); correctorIterator++)
    {
        if ( (*correctorIterator)->getName() == apCorrector->getName())
        {
            correctorDetected = true;
            break;
        }
        i++;
    }

    if (correctorDetected)
        LOG(gLogConfig, logWARNING) << "Corrector " << apCorrector->getName() << " already registered";
    else
    {
        // Push the corrector object onto the vector
        mFrameCorrector.push_back(apCorrector);
    }
}

/// deregisterCorrector - deregister a frame corrector from the raw data processor object
bool HxtRawDataProcessor::deregisterCorrector(HxtFrameCorrector *apCorrector)
{
    bool errorOK = false;
    vector<HxtFrameCorrector*>::iterator correctorIterator; int i = 0;
    for (correctorIterator = mFrameCorrector.begin(); correctorIterator != mFrameCorrector.end(); correctorIterator++)
    {
        if ( (*correctorIterator)->getName() == apCorrector->getName())
        {
            //cout << " De(Reg) " << ((*correctorIterator)->getName()) << " has a vector position: " << i << endl;
            errorOK = true;
            break;
        }
        i++;
    }

    //cout << "  Time to remove that pesky corrector @Pos'n: " << i << "\n"    ; Sleep(1000);
    if (errorOK)
        mFrameCorrector.erase( mFrameCorrector.begin()+i);
    else
        LOG(gLogConfig, logWARNING) << "No " << apCorrector->getName() << " Corrector to Deregister";
    return errorOK;
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

//    cout << "::parseFile(string)\n";
    mDebug = true;  /// DEBUGGING
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

    int colIdx = -1;

    // Initialise internal parser state and counters
    bool parseOK = true;
    bool lineReadOK = true;
    int framesDetected = 0;
    int eventsDetected = 0;

    // Set current frame index modulo number of current frames
    unsigned int currentFrameIdx = mFramesDetected % 2;
    unsigned int lastFrameIdx    = 1 - currentFrameIdx;

    LOG(gLogConfig, logDEBUG2) << "Parser start: mFramesDetected = " << mFramesDetected << " currentFrameIdx = " << currentFrameIdx << " lastFrameIdx = " << lastFrameIdx;
//    qDebug() << "Parser start: mFramesDetected = " << mFramesDetected << " currentFrameIdx = " << currentFrameIdx << " lastFrameIdx = " << lastFrameIdx;

    // Track pixel number (across frames)
    u64 index=0;
    // Track pixel number within current frame
    u16 pixelNumber = 0, reorderedPixel = 0;
    // Loop over data file while the parser is still running OK
    while (!datFile.eof() ) {

        // Read 16 bits of data
        datFile.read(reinterpret_cast<char*>(mRawData), sizeof(u16));

        // Check if EOF was reached during last read
        if (datFile.eof()) {
                LOG(gLogConfig, logDEBUG2) << "End of file detected   ";
                lineReadOK = false;
        }

        if (lineReadOK)
        {
            pixelNumber = index % 6400;
            /// HexitecGigE Addition: Descramble pixel reordering
            reorderedPixel = mPixelTable[pixelNumber];
            colIdx = reorderedPixel % 80;
            mRowIdx = reorderedPixel / 80;

            // Increment frames detected if first pixel read
            if (pixelNumber == 0)
                framesDetected++;

            if (mDebug) LOG(gLogConfig, logDEBUG3) << "Row " << setw(2) << mRowIdx << " col " << setw(2) << colIdx << " pixel value " << setw(6) << (double)(*mRawData);
//            qDebug() << "Row " << setw(2) << mRowIdx << " col " << setw(2) << colIdx << " pixel value " << setw(6) << (double)(*mRawData);
            //if (mDebug) LOG(gLogConfig, logNOTICE) << "Row " << setw(2) << mRowIdx << " col " << setw(2) << colIdx << " pixel value " << setw(6) << (double)(*mRawData);

            //if (mRowIdx >0) break; /// DEBUGGING

            // Sanity check row and column indices
            if (((unsigned int)mRowIdx > mHxtBuffer.nRows) || ((unsigned int)colIdx > mHxtBuffer.nCols)) {

                if (mDebug) LOG(gLogConfig, logERROR) << "Illegal row (" << mRowIdx << ") or column (" << colIdx << ") index on decoded column data line";

            } else {
                // If pixel value is above threshold,  fill value into global raw histogram and decoded frame
                if ((double)(*mRawData) > this->getPixelThreshold(mRowIdx, colIdx)) {
                    mGlobalRawHisto->Fill((double)(*mRawData));
                    mDecodedFrame[currentFrameIdx]->setPixel(mRowIdx, colIdx, (double)(*mRawData));
                    mEventsAboveThreshold++;
                }
 //               .
                eventsDetected++;
                mEventsDetected++;

            }

            // Process frame if last pixel of the frame detected?
            if ((index != 0)  && (index % 6399 == 0))
            {
                if (mDebug) LOG(gLogConfig, logDEBUG2) << "Last pixel of frame detected";

                // Set frame index in current decoded frame
                mDecodedFrame[currentFrameIdx]->setFrameIndex(mFramesDetected /*frameIdx*/);

                // Have a new frame now, so can perform corrections on on any previously processed frames
                this->processFrame(currentFrameIdx, lastFrameIdx);

                // Clear values from last frame, ready for use in next
                mDecodedFrame[lastFrameIdx]->clear();

                // Toggle current and last frame index
                currentFrameIdx = 1 - currentFrameIdx;
                lastFrameIdx    = 1 - currentFrameIdx;

                // Increment total frames detected count
                mFramesDetected++;
            }
            // Increment index before next loop iteration
            index++;

        } // if (lineReadOK)

    } // while (!datFile.eof())

    LOG(gLogConfig, logDEBUG2) << "Parser finish: mFramesDetected = " << mFramesDetected << " currentFrameIdx = " << currentFrameIdx << " lastFrameIdx = " << lastFrameIdx;

    // Close file
    datFile.close();

    // Stop timer
    mFileTimer->stop();

    // Check whether file stopped mid-frame
    if (index % 6400 == 0)
        parseOK = true;
    else
        parseOK = false;

    // Print processing summary
    if (parseOK) {
        LOG(gLogConfig, logINFO) << "Processed file " << aFileName << " in " << mFileTimer->elapsed() << " secs, frames read: " << framesDetected << " events read: " << eventsDetected;
    } else {
        LOG(gLogConfig, logERROR) << "Processing file " << aFileName << " unexpected stopped inside frame: " << framesDetected << " events read: " << eventsDetected;
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

/// parseBuffer - Cloned & modified of parseBuffer(string aBufferName)
/// to handle buffers instead of filenames
///
/// @param aBufferName pointer to buffer
/// @param frameLength number of frames within buffer
/// @return boolean flag indicating if processing completed OK
bool HxtRawDataProcessor::parseBuffer(unsigned short *aBufferName, unsigned long frameLength) {

    ///cout << "::parseBuffer(aBufferName=" << aBufferName << ", unsigned long frameLength=" << frameLength << "\n";

    // Start file timer
    mFileTimer->start();
    //mDebug = true;      /// DEBUGGING
    ///LOG(gLogConfig, logINFO) << "Accessing buffer address " << aBufferName /*<< " OK"*/;

    // Frame, row and column indices

    int colIdx = -1;

    // Initialise internal parser state and counters
    bool parseOK = true;
    int framesDetected = 0;
    int eventsDetected = 0;

    // Set current frame index modulo number of current frames
    unsigned int currentFrameIdx = mFramesDetected % 2;
    unsigned int lastFrameIdx    = 1 - currentFrameIdx;

    LOG(gLogConfig, logDEBUG2) << "Parser start: mFramesDetected = " << mFramesDetected << " currentFrameIdx = " << currentFrameIdx << " lastFrameIdx = " << lastFrameIdx;

    // Track pixel number (across frames)
    u64 index=0;
    // Track pixel number within current frame
    u16 pixelNumber = 0, reorderedPixel = 0;
    // Calculate amount of data accessible within buffer
    int bufferSize = (frameLength * mHxtBuffer.nRows * mHxtBuffer.nCols);     /// frameLength = number of frames in buffer; 2 bytes to each pixel
    unsigned short * pBuffer = 0;
    pBuffer = aBufferName;
    // Loop over buffer until frameLength pixels read
    while ( index < bufferSize) {

        // Read pixel from pointer location
        mRawData = reinterpret_cast<u16*>(pBuffer);

        pixelNumber = index % 6400;
        /// HexitecGigE Addition: Descramble pixel reordering
        reorderedPixel = mPixelTable[pixelNumber];
        colIdx = reorderedPixel % 80;
        mRowIdx = reorderedPixel / 80;

        // Increment frames detected if first pixel read
        if (pixelNumber == 0)
            framesDetected++;

        if (mDebug) LOG(gLogConfig, logDEBUG3) << "Row " << setw(2) << mRowIdx << " col " << setw(2) << colIdx << " pixel value " << setw(6) << (double)(*mRawData);
//        qDebug() << "Row " << setw(2) << mRowIdx << " col " << setw(2) << colIdx << " pixel value " << setw(6) << (double)(*mRawData);

        //if (mRowIdx >0) break; /// DEBUGGING

        // If pixel value is above threshold,  fill value into global raw histogram and decoded frame
        if ((double)(*mRawData) > this->getPixelThreshold(mRowIdx, colIdx)) {
            mGlobalRawHisto->Fill((double)(*mRawData));
            mDecodedFrame[currentFrameIdx]->setPixel(mRowIdx, colIdx, (double)(*mRawData));
            mEventsAboveThreshold++;
        }
        eventsDetected++;
        mEventsDetected++;


        // Process frame if last pixel of the frame detected?
        if ((index != 0)  && (index % 6399 == 0))
        {
            if (mDebug) LOG(gLogConfig, logDEBUG2) << "Last pixel of frame detected";

            // Set frame index in current decoded frame
            mDecodedFrame[currentFrameIdx]->setFrameIndex(mFramesDetected /*frameIdx*/);

            // Have a new frame now, so can perform corrections on any previously processed frames
            this->processFrame(currentFrameIdx, lastFrameIdx);

            // Clear values from last frame, ready for use in next
            mDecodedFrame[lastFrameIdx]->clear();

            // Toggle current and last frame index
            currentFrameIdx = 1 - currentFrameIdx;
            lastFrameIdx    = 1 - currentFrameIdx;

            // Increment total frames detected count
            mFramesDetected++;
        }
        // Increment index before next loop iteration
        index++;
        pBuffer++;  // Ditto increment pointer

    } // while (!datFile.eof())

    LOG(gLogConfig, logDEBUG2) << "Parser finish: mFramesDetected = " << mFramesDetected << " currentFrameIdx = " << currentFrameIdx << " lastFrameIdx = " << lastFrameIdx;

    // Stop timer
    mFileTimer->stop();

    // Check whether file stopped mid-frame
    if (index % 6400 == 0)
        parseOK = true;
    else
    {
        int remainder = 6400 - (index % 6400);
        LOG(gLogConfig, logERROR) << "Last frame only contained " << remainder << " pixels";
        parseOK = false;
    }

    // Print processing summary
    if (parseOK) {
        /*LOG(gLogConfig, logINFO) << "Processed buffer " << aBufferName << " in " << mFileTimer->elapsed() << " secs, frames read: " << framesDetected << " events read: " << eventsDetected*/;
    } else {
        LOG(gLogConfig, logERROR) << "Processed buffer " << aBufferName << " unexpected stopped inside frame: " << framesDetected << " events read: " << eventsDetected;
    }

//    delete mRawData;
//    mRawData = 0;
    ///Sleep(1000);
    return parseOK;
}

/// parseBuffer - overloaded version of parseFile method, which takes a vector
/// of buffers and iterates the parser over them
/// @param aBufferNames vector of pointers of buffers to process
/// @param aValidFrames vector of number of frames in each buffer
/// @return boolean flag indicating if parsing succeeded
bool HxtRawDataProcessor::parseBuffer(vector<unsigned short*> &aBufferNames, vector<unsigned long>  &aValidFrames)
{

    ///LOG(gLogConfig, logNOTICE) << "::parseBuffer(vector<" << aBufferNames.size() << ">, vector<" << aValidFrames.size() << ">)";
    bool processOK = true;
    unsigned int numBufferProcessed = 0;

    /// Loop over vector of buffers& frame sizes, processing each pair in turn
    vector<unsigned short*>::iterator bufferIterator;
    vector<unsigned long>::iterator frameIterator;
    frameIterator = aValidFrames.begin();

    for (bufferIterator = aBufferNames.begin(); bufferIterator != aBufferNames.end(); bufferIterator++)
    {
       // Process the buffer
        processOK = this->parseBuffer(*bufferIterator, *frameIterator);
        // Callback callBack function here
        /// Don't use fallback function if undefined!
        if (mCallbackAvailable)
            ;// Call the callback function here when implemented

        if (!processOK) break;
        numBufferProcessed++;
        frameIterator++;
    }

    LOG(gLogConfig, logINFO) << "Completed processing " << numBufferProcessed << " buffers";

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

    /// --------                                                       -------      ///

    // If we have processed more than two frames (so that we have a last and current frame), apply corrections to the frames
    if (mFramesDetected >= 2)
    {
        /// DEBUGGING purposes: Write frame(s) to disk (if enabled)
        if (mDebugFrames)
        {
            this->debugWriteFrame(currentFrameIdx, "A_PreProcessing"); // Need only process previous frame; Next call will cover "current" frame
            this->debugWriteFrame(lastFrameIdx, "B_PreProcessing");
        }
        ///
        vector<HxtFrameCorrector*>::iterator correctorIterator;
        for (correctorIterator = mFrameCorrector.begin(); correctorIterator != mFrameCorrector.end(); correctorIterator++) {
            (*correctorIterator)->apply(mDecodedFrame[lastFrameIdx], mDecodedFrame[currentFrameIdx]/*, mSubPixelFrame*/);
        }
        /// DEBUGGING purposes: Write frame(s) to disk (if enabled)
        if (mDebugFrames)
        {
            this->debugWriteFrame(currentFrameIdx, "A_PostProcessing"); // Need only process previous frame; Next call will cover "current" frame
            this->debugWriteFrame(lastFrameIdx, "B_PostProcessing");
            mFrameNumber++;
        }
        ///


        this->outputFrame(lastFrameIdx);
    }

    /// Don't write 250x240 subpixel frame histogram info
    // Output subpixel frame if CSA Enable
//    if (mCaCorrector)
//    {
//        this->outputFrame(mSubPixelFrame);
//        mSubPixelFrame->clear();
//    }

    return true;
}

/// Debugging function - Write frame number 'aFrame'
bool HxtRawDataProcessor::debugWriteFrame(unsigned int aFrameIdx, string fileDescription)
{
    /// Return early without any action if we were already done 10 frames..
    if (mFrameNumber > 10) return false;

    // Create file name from frame number and current timestamp
    DateStamp* now = new DateStamp();
    string debugFile(mDebugFrameDir + now->GetDateStamp() + fileDescription + "_Frame_" + to_string(mFrameNumber) + ".bin");
    delete(now);


    // Write pixel histograms out to binary file
    ofstream debugStream;
    debugStream.open(debugFile.c_str(), ios::binary | ios::out | ios::trunc);
    if (!debugStream.is_open()) {
        LOG(gLogConfig, logERROR) << "DEBUG: Failed to open output file " << debugFile;
        return false;
    }

    unsigned int counting = 0;
    // Write Each pixel to file
    for (unsigned int iRow = 0; iRow < mHxtBuffer.nRows; iRow++) {
        for (unsigned int iCol = 0; iCol < mHxtBuffer.nCols; iCol++) {
            double pixelValue = mDecodedFrame[aFrameIdx]->getPixel(iRow, iCol);
            if (pixelValue > 0.0)
                counting++;

            debugStream.write(reinterpret_cast<const char *>(&pixelValue), sizeof(pixelValue));
        }
    }
    debugStream.close();

    LOG(gLogConfig, logINFO) << "DEBUG: Written t_o file " << debugFile << " & where non-zero: " << counting;

    return true;
}

/// outputFrame - outputs decoded and corrected frames to histograms
/// @param aFrameIdx index of frame to write
/// @return boolean flag indicating if output succeeded (not currently implemented)
bool HxtRawDataProcessor::outputFrame(unsigned int aFrameIdx)
{
    if (mDebug) LOG(gLogConfig, logDEBUG1) << "Outputting frame " << mDecodedFrame[aFrameIdx]->getFrameIndex() << " to histograms";

    for (unsigned int iRow = 0; iRow < mHxtBuffer.nRows; iRow++) {
        for (unsigned int iCol = 0; iCol < mHxtBuffer.nCols; iCol++) {
            double correctedPixelValue = mDecodedFrame[aFrameIdx]->getPixel(iRow, iCol);
            if (correctedPixelValue != 0.0) {
                mGlobalDecodedHisto->Fill(correctedPixelValue);
                unsigned int pixelAddress = (iRow * mHxtBuffer.nCols) + iCol;
                mPixelHistogram[pixelAddress]->Fill(correctedPixelValue);
            }
        }
    }

    mCorrectedFramesWritten++;

    return true;
}

///// outputFrame - outputs decoded and corrected frames to histograms
///// @param aFrameIdx index of frame to write
///// @return boolean flag indicating if output succeeded (not currently implemented)
//bool HxtRawDataProcessor::outputFrame(HxtFrame* apSubPixelFrame)
//{
//    if (mDebug) LOG(gLogConfig, logDEBUG1) << "Outputting SubPixel frame " << apSubPixelFrame->getFrameIndex() << " to histograms";

//    for (unsigned int iRow = 0; iRow < mSubPixelRows; iRow++) {
//        for (unsigned int iCol = 0; iCol < mSubPixelCols; iCol++) {
//            double subPixelValue = apSubPixelFrame->getPixel(iRow, iCol);
//            if (subPixelValue != 0.0) {
//                mGlobalSubPixelHisto->Fill(subPixelValue);
//                unsigned int pixelAddress = (iRow * mSubPixelCols) + iCol;
//                mSubPixelHistogram[pixelAddress]->Fill(subPixelValue);
//            }
//        }
//    }

//    mSubPixelFramesWritten++;

//    return true;
//}

///// getSubPixelFrame - get Sub Pixel Frame
///// @return pointer to SubPixel Frame HxtFrame object
//HxtFrame* HxtRawDataProcessor::getSubPixelFrame()
//{
//    return mSubPixelFrame;
//}

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
        (*correctorIterator)->apply(mDecodedFrame[lastFrameIdx], mDecodedFrame[currentFrameIdx]/*, mSubPixelFrame*/);
    }

    // Loop over corrections and apply to last frame (current) - other ptr is null which allows
    // corrector to handle this edge case correctly
    for (correctorIterator = mFrameCorrector.begin(); correctorIterator != mFrameCorrector.end(); correctorIterator++) {
        (*correctorIterator)->apply(mDecodedFrame[currentFrameIdx], NULL/*, mSubPixelFrame*/);
    }

    // Write last two frames
    this->outputFrame(lastFrameIdx);
    this->outputFrame(currentFrameIdx);

    LOG(gLogConfig, logINFO) << "Frames detected: " << mFramesDetected << " Frames written: " << mCorrectedFramesWritten;
    LOG(gLogConfig, logINFO) << "Events detected: " << mEventsDetected << " Events above threshold: " << mEventsAboveThreshold;

    // If Charge Sharing Addition Sub Pixel enabled, display how many of each case of subpixel hit distribution
    if (mCaCorrector)
    {
        for (correctorIterator = mFrameCorrector.begin(); correctorIterator != mFrameCorrector.end(); correctorIterator++) {
            // Hack to output subpixel frame if that corrector chosen
            if (strcmp( ((*correctorIterator)->getName()).c_str(), "ChargeSharingAdditionSubPixel") == 0)
            {
                /// Don't report 240x240 frame (now redundant) but do report hits/case type
                //LOG(gLogConfig, logINFO) << "Total counts in global  subpixel spectrum = \t" << mGlobalSubPixelHisto->GetTotalCount();
                LOG(gLogConfig, logINFO) << "Case A: " << setw(11) << (*correctorIterator)->getCaseA();
                LOG(gLogConfig, logINFO) << "Case B: " << setw(11) << (*correctorIterator)->getCaseB();
                LOG(gLogConfig, logINFO) << "Case C: " << setw(11) << (*correctorIterator)->getCaseC();
                LOG(gLogConfig, logINFO) << "Case T: " << setw(11) << (*correctorIterator)->getCaseT();
                LOG(gLogConfig, logINFO) << "Case D: " << setw(11) << (*correctorIterator)->getCaseD();
                LOG(gLogConfig, logINFO) << "Total : " << setw(11) << (*correctorIterator)->getCaseA() + (*correctorIterator)->getCaseB() +
                    (*correctorIterator)->getCaseC() + (*correctorIterator)->getCaseD() + (*correctorIterator)->getCaseT();
            }
        }
    }

    for (correctorIterator = mFrameCorrector.begin(); correctorIterator != mFrameCorrector.end(); correctorIterator++) {
        const unsigned int nCorrected = (*correctorIterator)->getNumEventsCorrected();
        LOG(gLogConfig, logINFO) << "Correction " << (*correctorIterator)->getName() << " corrected " << nCorrected << " events";
    }

    return true;
}

/// writeBufferToFile - Write HxtBuffer contents to file
bool HxtRawDataProcessor::writeHxtBufferToFile(string aOutputFileName)
{
    // Write pixel histograms out to binary file
    ofstream pixelFile;
    pixelFile.open(aOutputFileName.c_str(), ios::binary | ios::out | ios::trunc);
    if (!pixelFile.is_open()) {
        LOG(gLogConfig, logERROR) << "(1) Failed to open output file " << aOutputFileName << ", cannot write HxtBuffer";
        return false;
    }

    unsigned int headerSize = ((8*sizeof(char)) + sizeof(u64) + (9*sizeof(int)) + sizeof(int) + 100 + 16 + (3*sizeof(u32)) );
    unsigned int bufferSize = ((mHxtBuffer.nBins * mHxtBuffer.nRows * mHxtBuffer.nCols) + mHxtBuffer.nBins) * sizeof(double);

    unsigned int totalSize  = headerSize + bufferSize;

    ///  Calculate size of Buffer sections: Header, Bins
    int hxtBufferHeaderSize = (char*)&(mHxtBuffer.allData[0]) - (char*)(mHxtBuffer.hxtLabel);
    int sizeBins = mHxtBuffer.nBins *sizeof(double);

    /// Calculate pointers for Binary Bins, Binary Contents
    char* pCharBufferAddress = (char*)(&mHxtBuffer);
    void* pBinsAddress       = (pCharBufferAddress + hxtBufferHeaderSize);
    void* pContentsAddress   = (pCharBufferAddress + hxtBufferHeaderSize  + sizeBins);

    // Write histogram bins to file
    int binsBytesCopied = mPixelHistogram[0]->BinaryCopyBins((char*)pBinsAddress);

    /// Original code for copying  Bin Contents
    char* pBuffer = (char*)(pContentsAddress);
    int bytesCopied = -1;
    // Copy pixel histograms into mHxtBuffer structure
    for (unsigned int iPixel = 0; iPixel < mPixels; iPixel++) {

        bytesCopied = mPixelHistogram[iPixel]->BinaryCopyContent(pBuffer);

        // Increment by number of histograms/pixel
        pBuffer = pBuffer + bytesCopied;
    }

    // Write Header & Contents, all in one go
    pixelFile.write((const char*)mHxtBuffer.hxtLabel, totalSize);
    pixelFile.close();

    LOG(gLogConfig, logINFO) << "Written HxtBuffer Header & Contents to binary file " << aOutputFileName;

    return true;
}

/// writeStructOutput - Restored for now..
bool HxtRawDataProcessor::writeStructOutput(string aOutputPixelFileName) {

    // Write pixel histograms out to binary file
    ofstream pixelFile;
    pixelFile.open(aOutputPixelFileName.c_str(), ios::binary | ios::out | ios::trunc);
    if (!pixelFile.is_open()) {
        LOG(gLogConfig, logERROR) << "(1) Failed to open output file " << aOutputPixelFileName;
        return false;
    }

    // Calculate size of the size in bytes:
    int dataSize = (char*)&(mHxtBuffer.allData[0]) - (char*)(mHxtBuffer.hxtLabel);
    // Write HxBuffer's hxtLabel through nBins in one fell sweep
    pixelFile.write((const char*)&(mHxtBuffer.hxtLabel), dataSize);

    // Write histogram bins to file -   Remains unchanged (?)
    mPixelHistogram[0]->BinaryWriteBins(pixelFile);

    // Write pixel histograms to file   - Remains unchanged (?)
    for (unsigned int iPixel = 0; iPixel < mPixels; iPixel++) {
        mPixelHistogram[iPixel]->BinaryWriteContent(pixelFile);
    }
    pixelFile.close();

    LOG(gLogConfig, logINFO) << "(modded) Written output histogram binary file " << aOutputPixelFileName;

    return true;
}

/// writeOutput - write the output of a raw data processing run to output files
bool HxtRawDataProcessor::writePixelOutput(string aOutputPixelFileName) {

    // Write pixel histograms out to binary file
    ofstream pixelFile;
    pixelFile.open(aOutputPixelFileName.c_str(), ios::binary | ios::out | ios::trunc);
    if (!pixelFile.is_open()) {
        LOG(gLogConfig, logERROR) << "(2) Failed to open output file " << aOutputPixelFileName;
        return false;
    }

    // Write binary file header

    pixelFile.write(mHxtBuffer.hxtLabel, sizeof(mHxtBuffer.hxtLabel));

    pixelFile.write((const char*)&mHxtBuffer.hxtVersion, sizeof(u64));

    // Include File Prefix/Motor Positions/Data Time Stamp - if format version > 1
    if (mHxtBuffer.hxtVersion > 1)
    {
//        /// motor order: mSSX, mSSY, mSSZ, mSSROT, mTimer, mGALX, mGALY, mGALZ, mGALROT
//        pixelFile.write((const char*)&mHxtBuffer.motorPositions[0], sizeof(mHxtBuffer.motorPositions[0]));
//        pixelFile.write((const char*)&mHxtBuffer.motorPositions[1], sizeof(mHxtBuffer.motorPositions[1]));
//        pixelFile.write((const char*)&mHxtBuffer.motorPositions[2], sizeof(mHxtBuffer.motorPositions[2]));
//        pixelFile.write((const char*)&mHxtBuffer.motorPositions[3], sizeof(mHxtBuffer.motorPositions[3]));
//        pixelFile.write((const char*)&mHxtBuffer.motorPositions[4], sizeof(mHxtBuffer.motorPositions[4]));
//        pixelFile.write((const char*)&mHxtBuffer.motorPositions[5], sizeof(mHxtBuffer.motorPositions[5]));
//        pixelFile.write((const char*)&mHxtBuffer.motorPositions[6], sizeof(mHxtBuffer.motorPositions[6]));
//        pixelFile.write((const char*)&mHxtBuffer.motorPositions[7], sizeof(mHxtBuffer.motorPositions[7]));
//        pixelFile.write((const char*)&mHxtBuffer.motorPositions[8], sizeof(mHxtBuffer.motorPositions[8]));

        // Determine length of File Prefix
        int filePrefixSize = mHxtBuffer.filePrefixLength;

        // Write prefix length, followed by prefix itself
        pixelFile.write((const char*)&filePrefixSize, sizeof(filePrefixSize));

        size_t prefixLen = 0; size_t stampLen = 0;
        if (mHxtBuffer.hxtVersion == 2)
        {
            prefixLen = filePrefixSize;
            stampLen  = sizeof(mHxtBuffer.dataTimeStamp);
        }
        else    // Version 3; fixed lengths: prefix=100,timeStamp=16
        {
            prefixLen = 100;
            stampLen  = 16;
        }

        pixelFile.write(mHxtBuffer.filePrefix, prefixLen);
        pixelFile.write(mHxtBuffer.dataTimeStamp, stampLen);
    }
    // Continue writing header information that is common to both format versions
    pixelFile.write((const char*)&mHxtBuffer.nRows, sizeof(mHxtBuffer.nRows));
    pixelFile.write((const char*)&mHxtBuffer.nCols, sizeof(mHxtBuffer.nCols));
    pixelFile.write((const char*)&mHxtBuffer.nBins, sizeof(mHxtBuffer.nBins));

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

/// copyOutput - copy the output of a raw data processing run to buffer
bool HxtRawDataProcessor::copyPixelOutput(unsigned short* aHxtBuffer) {

    /// Copy HxBuffer's hxtLabel through nBins in one fell sweep
    // Manually Calculate Header size: (184 Bytes)
//    int headerSize = ((8*sizeof(char)) + sizeof(u64) + (9*sizeof(int)) +
//                    sizeof(int) + 100 + 16 + (3*sizeof(u32)) );


    ///  Calculate size of Buffer sections: Header, Bins
    int hxtBufferHeaderSize = (char*)&(mHxtBuffer.allData[0]) - (char*)(mHxtBuffer.hxtLabel);
    int sizeBins = mHxtBuffer.nBins *sizeof(double);

    /// Calculate pointers for Binary Bins, Binary Contents
    char* pCharBufferAddress = (char*)(aHxtBuffer);
    void* pBinsAddress       = (pCharBufferAddress + hxtBufferHeaderSize);                // Old implementation
    void* pContentsAddress   = (pCharBufferAddress + hxtBufferHeaderSize  + sizeBins);    // Old implementation
//    HxtBuffer* tmpBufferPtr  = (HxtBuffer*)(aHxtBuffer);                       // Re-cast argument pointer
//    void* pBinsAddress       = (char*)(tmpBufferPtr->allData);
//    void* pContentsAddress   = (pBinsAddress + sizeBins);

    memcpy((void*)aHxtBuffer, &mHxtBuffer, hxtBufferHeaderSize);  /// Copying 184 Bytes in one go

    // Write histogram bins to file
    int binsBytesCopied = mPixelHistogram[0]->BinaryCopyBins((char*)pBinsAddress);

    /// Original code for copying  Bin Contents
    char* pBuffer = (char*)(pContentsAddress);
    int bytesCopied = -1;
    // Write pixel histograms to file
    for (unsigned int iPixel = 0; iPixel < mPixels; iPixel++) {

        bytesCopied = mPixelHistogram[iPixel]->BinaryCopyContent(pBuffer);

        // Increment by number of histograms/pixel
        pBuffer = pBuffer + bytesCopied;
    }

    LOG(gLogConfig, logINFO) << "Copied processed HXT contents to buffer";

    return true;
}


unsigned int HxtRawDataProcessor::calculateCurrentHeaderSize()
{
    /// Calculate the size of the header - currently untested !
    unsigned int headerSize = 0;
    headerSize= (char*) &mHxtBuffer.allData[0] - (char*) mHxtBuffer.hxtLabel;
//    headerSize += (int)label.length();
//    headerSize += sizeof(mFormatVersion);
//    headerSize += sizeof(mHxtBuffer.nRows);
//    headerSize += sizeof(mHxtBuffer.nCols);
//    headerSize += sizeof(mHxtBuffer.nBins);
    return headerSize;
}


unsigned int HxtRawDataProcessor::calculateBodySize()
{
    /// Calculate the size of the written file without the header portion
    return (sizeof(double) * mHxtBuffer.nBins + sizeof(double) * mHxtBuffer.nBins * mPixels);
}

void HxtRawDataProcessor::updateFilePrefix(string filePrefix)
{
    strncpy(mHxtBuffer.filePrefix, filePrefix.c_str(), filePrefix.size());
}

//void HxtRawDataProcessor::updateMotorPositions(int ssx, int ssy, int ssz, int ssrot, int timer, int galx, int galy, int galz, int galrot)
//{
//    mHxtBuffer.motorPositions[0] = ssx;
//    mHxtBuffer.motorPositions[1] = ssy;
//    mHxtBuffer.motorPositions[2] = ssz;
//    mHxtBuffer.motorPositions[3] = ssrot;
//    mHxtBuffer.motorPositions[4] = timer;
//    mHxtBuffer.motorPositions[5] = galx;
//    mHxtBuffer.motorPositions[6] = galy;
//    mHxtBuffer.motorPositions[7] = galz;
//    mHxtBuffer.motorPositions[8] = galrot;
//}

void HxtRawDataProcessor::updateTimeStamp(string timeStamp)
{
    strncpy(mHxtBuffer.dataTimeStamp, timeStamp.c_str(), timeStamp.size());
}

/// InterpolateDeadPixels - Workout the average of all nonzero pixel histogram surrounding a dead pixel
void HxtRawDataProcessor::InterpolateDeadPixels(const unsigned int aThreshold)
{
    // Construct frame containing 1 = dead pixel, 0 = fine pixel
    HxtFrame* deadPixelFrame = new HxtFrame(mHxtBuffer.nRows, mHxtBuffer.nCols);

    // Create vector to track dead pixels
    vector<hxtPixel> deadPixels;

    // Initialise array of flags
    for (unsigned int iRow = 0; iRow < mHxtBuffer.nRows; iRow++)
    {
        // Iterate over columns
        for (unsigned int iCol = 0; iCol < mHxtBuffer.nCols; iCol++)
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
        else if (iRow == (mHxtBuffer.nRows-1)) {
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
        else if (iCol == (mHxtBuffer.nCols-1)) {
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
        for (unsigned int iBin = 0; iBin < mHxtBuffer.nBins; iBin++)
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

///// writeSubPixelOutput - write the output of a raw data processing run to output files
//bool HxtRawDataProcessor::writeSubPixelOutput(string aOutputSubPixelFileName) {

//    // Write subpixel histograms out to binary file
//    ofstream subPixelFile;
//    subPixelFile.open(aOutputSubPixelFileName.c_str(), ios::binary | ios::out | ios::trunc);
//    if (!subPixelFile.is_open()) {
//        LOG(gLogConfig, logERROR) << "(3) Failed to open output file " << aOutputSubPixelFileName;
//        return false;
//    }

//    // Write binary file header
//    subPixelFile.write(mHxtBuffer.hxtLabel, sizeof(mHxtBuffer.hxtLabel));

//    subPixelFile.write((const char*)&mHxtBuffer.hxtVersion, sizeof(u64));

//    subPixelFile.write((const char*)&mSubPixelRows, sizeof(mSubPixelRows));
//    subPixelFile.write((const char*)&mSubPixelCols, sizeof(mSubPixelCols));
//    subPixelFile.write((const char*)&mHxtBuffer.nBins, sizeof(mHxtBuffer.nBins));

//    // Write histogram bins to file
//    mSubPixelHistogram[0]->BinaryWriteBins(subPixelFile);

//    // Write subpixel histograms to file
//    for (unsigned int iPixel = 0; iPixel < mSubPixelPixels; iPixel++) {
//        mSubPixelHistogram[iPixel]->BinaryWriteContent(subPixelFile);
//    }
//    subPixelFile.close();

//    LOG(gLogConfig, logINFO) << "Written output histogram binary file " << aOutputSubPixelFileName;

//    return true;
//}

bool HxtRawDataProcessor::writeCsvFiles(void) {

    // Write CSV histogram file out
    ofstream rawCsvFile;
    string rawCsvFileName("histoRaw.csv");

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
    string corCsvFileName(mCsvFileName);    //"histoCorrected.csv");

    corCsvFile.open(corCsvFileName.c_str(), ios::out | ios::trunc);
    if (!corCsvFile.is_open()) {
        LOG(gLogConfig, logERROR) <<  "Failed to open CSV file" << corCsvFileName;
        return false;
    }

    mGlobalDecodedHisto->WriteCsv(corCsvFile);

    corCsvFile.close();

    LOG(gLogConfig, logINFO) << "Written global corrected histogram to CSV file " << corCsvFileName;

//    /// DO NOT Write subpixel CSV histogram file EVEN if charge sharing addition corrector enabled
//    if (mCaCorrector)
//    {
//        ofstream corSbPxlCsvFile;
//        string corSbPxlCsvFileName("histoSubPixelCorrected.csv");

//        corSbPxlCsvFile.open(corSbPxlCsvFileName.c_str(), ios::out | ios::trunc);
//        if (!corSbPxlCsvFile.is_open()) {
//            LOG(gLogConfig, logERROR) <<  "Failed to open CSV file" << corSbPxlCsvFileName;
//            return false;
//        }

//        mGlobalSubPixelHisto->WriteCsv(corSbPxlCsvFile);

//        corSbPxlCsvFile.close();

//        LOG(gLogConfig, logINFO) << "Written subpixel corrected histogram to CSV file " << corSbPxlCsvFileName;

//    }
    // Processing CSV files successful, note filenames before returning
    mRawCsvFileName = rawCsvFileName;
    mCorCsvFileName = corCsvFileName;

    return true;
}

} // namespace
