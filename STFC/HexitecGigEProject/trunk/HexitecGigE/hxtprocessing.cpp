/*
 * appMain.cpp - main application code for Hexitech raw data processing
 *
 *  Created on: 10 Dec 2010
 *      Author: Tim Nicholls
 */

#include "HxtProcessing.h"

// This makes QT a dependency for this project:
#include <QDebug>
// Regular expressions:
#include <regex>

const unsigned int kHxtSensorCols = 80;
const unsigned int kHxtSensorRows = 80;

using namespace std;

// Declaration of global log configuration used throughout
LogConfig* gLogConfig;

namespace hexitech {

HxtProcessing::HxtProcessing(string aAppName, unsigned int aDebugLevel) :
    mDebugLevel(aDebugLevel)
{
    // Default values
    mHistoStartVal               = 0;
    mHistoEndVal                 = 10000;
    mHistoBins                   = 1000;
    mInterpolationThreshold      = 0;
    mInducedNoiseThreshold       = 0.0;
    mGlobalThreshold             = -1.0;
    mOutputFileNameDecodedFrame  = "pixelHisto.hxt";
    mOutputFileNameSubPixelFrame = "pixelSubResolutionHisto.hxt";
    mEnableInCorrector           = false;
    mEnableCabCorrector          = false;
    mEnableMomCorrector          = false;
    mEnableCsaspCorrector        = false;
    mEnableCsdCorrector          = true;
    mEnableIdCorrector           = true;
    mEnableIpCorrector           = false;
    mEnableDbPxlsCorrector       = false;
    mWriteCsvFiles               = false;
    mEnableVector                = false;
    mEnableDebugFrame            = false;
    mDebugFrameDir               = "C:/temp/";

    mFormatVersion = 3;

    mFilePrefix    = "Def_prefix_";
    mDataTimeStamp = string("000000_000000");
    mutexTimeout   = 50;  // Milliseconds duration to attempt acquiring a mutex lock

    bPrefixEnabled      = false;
    bMotorEnabled       = false;
    bTimeStampEnabled   = false;
    bUseRawFileEnabled  = false;

    bThreadRunning      = true;

    // Initialise mPositions members to positionUninitialised (2222222)
    clearMotorPositions();

    // Condition to be met for file(s) be processed
    targetCondition = conditionNoneMet;

    bDontDisplayProcessedData   = false;    // Assume data to be displayed initially (i.e. display not be updated)
    bManualProcessingEnabled    = false;    // Don't assume ProcessNowButton action pressed during initialisation
    bRemoveUnprocessedFiles     = false;    // No raw files to be removed from queue initially
    bProcessQueueContents       = false;    // Don't process contents in Queues before DataAcquisitionForm signal

    mMomentumFile = string("");

    qRegisterMetaType<vector<unsigned short*> >("vector<unsigned short*>");
    /// DEBUGGING (temporary) PURPOSES:
    bReordering = true;
    ///     ---------
    mProcessingTimer  = new Timer();    // Time how long to process a received buffer/file from the Q
    mDiscWritingTimer = new Timer();    // Used to time  when to write histograms to disc/pass to visualisation tab
    mIdleTimer        = new Timer();    // Time how long idling on empty buffers (Allow processing of  unprocessed remnants)
    mAppName = aAppName;
    mFirstBufferInCollection = false;

    pixelThreshold = new HxtPixelThreshold(kHxtSensorRows, kHxtSensorCols);
    // Create new raw data processor instance
    dataProcessor = new HxtRawDataProcessor(kHxtSensorRows, kHxtSensorCols, mHistoStartVal, mHistoEndVal, mHistoBins,
                                                                 mFormatVersion,
                                                                 mPositions.mSSX, mPositions.mSSY, mPositions.mSSZ, mPositions.mSSROT,
                                                                 mPositions.mTimer,
                                                                 mPositions.mGALX, mPositions.mGALY, mPositions.mGALZ, mPositions.mGALROT,
                                                                 mFilePrefix, mDataTimeStamp, mEnableCallback);
    // 1. Induced Noise Corrector
    inCorrector = new HxtFrameInducedNoiseCorrector(mInducedNoiseThreshold);
    // 2. Calibration
    // Create pointer to Gradients file
    gradientsContents = new HxtPixelThreshold(kHxtSensorRows, kHxtSensorCols);
    // Create pointer to Intercepts file
    interceptsContents = new HxtPixelThreshold(kHxtSensorRows, kHxtSensorCols);
    cabCorrector = 0;
    // 3. CS Addition / O R / CS Discriminator
    // 3.1 subpixel
    // Create subpixel corrector
    subCorrector = new HxtFrameChargeSharingSubPixelCorrector();
    // 3.2 CS Addition
    csdCorrector = new HxtFrameChargeSharingDiscCorrector();
    // 4. Incomplete Data
    idCorrector = new HxtFrameIncompleteDataCorrector();
    // 5. Momentum
    // Create pointer to Momentum file
    momentumContents = new HxtPixelThreshold(kHxtSensorRows, kHxtSensorCols);
    momCorrector = 0;
    // x. Development purposes only, check for pixels read out more than once
    dbpxlCorrector = new HxtFrameDoublePixelsCorrector();

    /// Determine size pool containing buffers (communicate with Visualisation tab, for displaying)
    numHxtBuffers = 10;
    // Ensure memory etc only setup when prepSettings() called the first time
    bFirstTime = true;
}

HxtProcessing::~HxtProcessing()
{
    delete mProcessingTimer;
    // Signal to run() to shut down
    bThreadRunning = false;
    sleep(1);
    // Close log configuration and any associated files
    gLogConfig->close();
    delete gLogConfig;

    //// Delete objects
    delete dataProcessor;
    delete idCorrector;
    delete csdCorrector;
    delete subCorrector;
    delete inCorrector;
    delete pixelThreshold;
    delete gradientsContents;
    delete interceptsContents;
    delete dbpxlCorrector;
    if (cabCorrector != 0) delete cabCorrector;
    if (momCorrector != 0) delete momCorrector;
    delete momentumContents;
    // Free pool of buffers
    vector<unsigned short*>::iterator bufferIterator;
    for (bufferIterator = mHxtBuffers.begin(); bufferIterator != mHxtBuffers.end(); bufferIterator++)
    {
        /*delete [] bufferIterator*/;   /// Revisit this..
    }

}

void HxtProcessing::prepSettings()
{
    qDebug() << "HxtProcessing::prepSettings() called ";

    if (bFirstTime)
    {
        // Setup pool of buffers - To send HXT file contents by RAM rather than file
        for (int i=0; i < numHxtBuffers; i++)
        {
            unsigned short* hxtBuffer = new unsigned short[59381];
            mHxtBuffers.push_back(hxtBuffer);
        }
    }
    // Extract file path of hexitech file
    QFileInfo fileInfo(mOutputFileNameDecodedFrame.c_str());
    string filePath = fileInfo.absolutePath().toStdString();

    // Obtain date stamp, save logging to the same folder as processed file
    DateStamp* now = new DateStamp();
    logFileStream.clear();
    logFileStream.str("");
    logFileStream << filePath;
    logFileStream << "/hexitech_log_";
    logFileStream << now->GetDateStamp();
    logFileStream << ".txt";
    delete(now);
    qDebug() << "logFileStream.str: " << logFileStream.width();

    // Create new log configuration
    gLogConfig = new LogConfig( mAppName);
    gLogConfig->setLogStdout(true);
    gLogConfig->setLogFile(true, logFileStream.str());
    gLogConfig->setDebugLevel(mDebugLevel);

    LOG(gLogConfig, logNOTICE) << "Starting up";

    // Display user settings
    LOG(gLogConfig, logINFO) << "Histogram start: " << mHistoStartVal << " End: " << mHistoEndVal << " Bins: " << mHistoBins;
    if (mGlobalThreshold != -1.0)
        LOG(gLogConfig, logINFO) << "Global threshold set as: " << mGlobalThreshold;
    else
        LOG(gLogConfig, logINFO) << "Global threshold not set";


    if (!mThresholdFileName.empty())
        LOG(gLogConfig, logINFO) << "Global threshold file selected: " << mThresholdFileName;

    if (mEnableCabCorrector)
        LOG(gLogConfig, logINFO) << "Calibration selected, Gradients file: " << mGradientsFile << " Intercepts file: " << mInterceptsFile;
    else
        LOG(gLogConfig, logINFO) << "Calibration not selected";

    if (mEnableMomCorrector)
        LOG(gLogConfig, logINFO) << "Momentum selected, Momentum file: " << mMomentumFile;
    else
        LOG(gLogConfig, logINFO) << "Momentum not selected";

    if (mWriteCsvFiles)
        LOG(gLogConfig, logINFO) << "Selected to write csv files [To be disabled - Visualisation thread to calculate this]";

    LOG(gLogConfig, logINFO) << "Selected correctors:" <<
        ((mEnableInCorrector) ? (" InducedNoise") : "") <<
        ((mEnableCabCorrector) ? (" Calibration") : "") <<
        ((mEnableCsaspCorrector) ? (" ChargeSharingAddition") : "") <<
        ((mEnableCsdCorrector) ? (" ChargeSharingDiscrimination") : "") <<
        ((mEnableIdCorrector) ? (" IncompleteData") : "") <<
        ((mEnableIpCorrector) ? (" Interpolate") : "") <<
        ((mEnableMomCorrector) ? (" Momentum") : "");


    LOG(gLogConfig, logINFO) << "Writing to log file " << logFileStream.str();

    // Load a threshold file
    HxtPixelThreshold* pixelThreshold = new HxtPixelThreshold(kHxtSensorRows, kHxtSensorCols);
    if (mGlobalThreshold != -1.0) {
        pixelThreshold->setGlobalThreshold(mGlobalThreshold);
    }
    if (!mThresholdFileName.empty()) {
        pixelThreshold->loadThresholds(mThresholdFileName);
    }

    /// More staff copied in from ::executeProcessing() ... ///

    /// DEBUGGING frame by frame [redundant later on]
    dataProcessor->setDebugFrameDir("U:/BInMe/debug_frames/");
    // Disable this for now..
    dataProcessor->setEnableDebugFrame(false);

    // If Motor Positions not selected, pass dummy values as Motor Positions
    if (!bMotorEnabled)
    {
        int dummyPos = positionUninitialised;
        dataProcessor->updateMotorPositions(dummyPos, dummyPos, dummyPos, dummyPos, dummyPos, dummyPos, dummyPos, dummyPos, dummyPos);
    }
    else
    {
        dataProcessor->updateMotorPositions(mPositions.mSSX, mPositions.mSSY, mPositions.mSSZ, mPositions.mSSROT, mPositions.mTimer,
                                            mPositions.mGALX, mPositions.mGALY, mPositions.mGALZ, mPositions.mGALROT);
    }
    dataProcessor->updateTimeStamp(mDataTimeStamp);

    // Set up vector implementation
    dataProcessor->setVector(mEnableVector);

    //  Enable debug frames - each frame written to file before/after being processed
    dataProcessor->setDebugFrameDir(mDebugFrameDir);
    dataProcessor->setEnableDebugFrame(mEnableDebugFrame);

    // Set debug flags if debug level non-zero
    if (mDebugLevel) dataProcessor->setDebug(true);

    // Apply thresholds to raw data processor
    dataProcessor->applyPixelThresholds(pixelThreshold);

    // Create and register frame correctors with raw data processor if enabled

    // 1. Induced Noise Corrector
    HxtFrameInducedNoiseCorrector* inCorrector = new HxtFrameInducedNoiseCorrector(mInducedNoiseThreshold);
    // Register it, enable debug if needed
    if (mDebugLevel) inCorrector->setDebug(true);
    if (mEnableInCorrector) {
        dataProcessor->registerCorrector(dynamic_cast<HxtFrameCorrector*>(inCorrector));
        LOG(gLogConfig, logINFO) << "Applying " << inCorrector->getName() << " corrector to data";
    }

    // 2. Calibration

    // Create pointer to Gradients file
    HxtPixelThreshold* gradientsContents = new HxtPixelThreshold(kHxtSensorRows, kHxtSensorCols);
    // Create pointer to Intercepts file
    HxtPixelThreshold* interceptsContents = new HxtPixelThreshold(kHxtSensorRows, kHxtSensorCols);
    HxtFrameCalibrationCorrector* cabCorrector = 0;

    // Load Gradients file and Intercepts file contents to memory
    if (mEnableCabCorrector) {
        // Load files' contents into memory
        gradientsContents->loadThresholds(mGradientsFile);
        interceptsContents->loadThresholds(mInterceptsFile);

        // Create Calibration Corrector
        cabCorrector = new HxtFrameCalibrationCorrector(gradientsContents, interceptsContents, kHxtSensorRows, kHxtSensorCols);
        // Register it, enable debug if needed
        if (mDebugLevel) cabCorrector->setDebug(true);

        dataProcessor->registerCorrector(dynamic_cast<HxtFrameCorrector*>(cabCorrector));
        LOG(gLogConfig, logINFO) << "Applying " << cabCorrector->getName() << " corrector to data";
    }

    // 3. CS Addition / O R / CS Discriminator
    // 3.1 subpixel

    // Set Charge Sharing Sub Pixel flag if enabled
    dataProcessor->setCsaCorrector(mEnableCsaspCorrector);

    // Create subpixel corrector
    HxtFrameChargeSharingSubPixelCorrector* subCorrector = new HxtFrameChargeSharingSubPixelCorrector();
    // Register it, enable debug if needed
    if (mDebugLevel) subCorrector->setDebug(true);
    if (mEnableCsaspCorrector) {
        dataProcessor->registerCorrector(dynamic_cast<HxtFrameCorrector*>(subCorrector));
        LOG(gLogConfig, logINFO) << "Applying " << subCorrector->getName() << " corrector to data";
    }

    // 3.2 CS Addition
    HxtFrameChargeSharingDiscCorrector* csdCorrector = new HxtFrameChargeSharingDiscCorrector();
    if (mDebugLevel) csdCorrector->setDebug(true);
    if (mEnableCsdCorrector) {
        dataProcessor->registerCorrector(dynamic_cast<HxtFrameCorrector*>(csdCorrector));
        LOG(gLogConfig, logINFO) << "Applying " << csdCorrector->getName() << " corrector to data";
    }

    // 4. Incomplete Data
    HxtFrameIncompleteDataCorrector* idCorrector = new HxtFrameIncompleteDataCorrector();
    if (mDebugLevel) idCorrector->setDebug(true);
    if (mEnableIdCorrector) {
        dataProcessor->registerCorrector(dynamic_cast<HxtFrameCorrector*>(idCorrector));
        LOG(gLogConfig, logINFO) << "Applying " << idCorrector->getName() << " corrector to data";
    }

    // 5. Momentum

    // Create pointer to Momentum file
    HxtPixelThreshold* momentumContents = new HxtPixelThreshold(kHxtSensorRows, kHxtSensorCols);
    HxtFrameMomentumCorrector* momCorrector = 0;

    // Load Momentum file contents to memory
    if (mEnableMomCorrector) {
        // Load file's contents into memory
        momentumContents->loadThresholds(mMomentumFile);

        // Create Momentum Corrector
        momCorrector = new HxtFrameMomentumCorrector(momentumContents, kHxtSensorRows, kHxtSensorCols);
        // Register it, enable debug if needed
        if (mDebugLevel) momCorrector->setDebug(true);

        dataProcessor->registerCorrector(dynamic_cast<HxtFrameCorrector*>(momCorrector));
        LOG(gLogConfig, logINFO) << "Applying " << momCorrector->getName() << " corrector to data";
    }


    // x. Development purposes only, check for pixels read out more than once
    HxtFrameDoublePixelsCorrector* dbpxlCorrector = new HxtFrameDoublePixelsCorrector();
    if (mEnableDbPxlsCorrector)  {
        if (mDebugLevel) dbpxlCorrector->setDebug(true);
        dataProcessor->registerCorrector(dynamic_cast<HxtFrameCorrector*>(dbpxlCorrector));
        LOG(gLogConfig, logINFO) << "Applying " << dbpxlCorrector->getName() << " corrector to data";
    }

    bFirstTime = false;
}

void HxtProcessing::pushRawFileName(string aFileName, int frameSize)
{
   /* This method is being used to indicate a new image collection is starting.
    * aFileName contains the name (directory and prefix, but no extension) to use
    * for the new image. frameSize is the size (in bytes) of each frame within
    * the transfer buffer. This filename with .bin extension is used to store the
    * raw image. New filenames for processed data can be created by adding any
    * extension (except .bin) to this filename.
    */
   qDebug() << "hxtProcessing::pushRawFileName() with " << aFileName.c_str()
               << "frameSize (bytes) = " << frameSize;

   /// HexitecGigE Addition, Check file has .hxt file ending - It always should have
   if ( aFileName.find(".") > 250)
       aFileName = aFileName + ".hxt";

   /// Only one filename per  acquisition,  use it as name of processed file
   mOutputFileNameDecodedFrame = aFileName;
    /// Modification to handle buffers (to replace files, except where raw file(s) picked manually by user)
    mFrameSize = frameSize;
}
void HxtProcessing::pushImageComplete(unsigned long long totalFramesAcquired)
{
   /* totalFramesAcquired should tally with how many acquired before this function is called */
   qDebug() <<"The image is complete and has " << totalFramesAcquired << "total frames";
}

void hexitech::HxtProcessing::pushTransferBuffer(unsigned char *transferBuffer, unsigned long validFrames)
{
   /// This function needs to queue the transfer buffer for processing.
   /// Once the processing is complete and the buffer no-longer required
   /// the following signal should be sent to say the buffer can be released
   /// back to the GigEDetector thread for saving the raw data and for re-use.
   /// Its currently done here for testing.

   // Recast unsigned char pointer to unsigned short pointer (2 bytes, = 1 pixel)
   unsigned short* newBuffer = reinterpret_cast<unsigned short*>(transferBuffer);

//   /// DEBUGGING  PURPOSES: Is pixel  reordering correct?
//   if (bReordering)
//   {
//       unsigned short *pBuffer = newBuffer;
//       for (unsigned int i=0; i < 80*5; i++)
//       {
//           cout << "   [" << setw(2) << i << "]=" << setw(3) <<  *pBuffer;
//           pBuffer++;
//           if ((i!= 0) && (i  % 79 == 0))
//               cout << endl << "=========================== i:" << i << " ===========================" << endl;
//       }
////       bReordering = false;s
////       Sleep(1000);
//       qDebug() << "- - - - DEBUGGING PURPOSES: finished";
//   }
//   /// - - - - - - - - - - - - -    - - - - - - - - - - - - -///
   int iDebug = 0;
   if (bufferMutex.tryLock(mutexTimeout))
   {
       bufferQueue.enqueue(newBuffer); //transferBuffer);
       bufferMutex.unlock();
       iDebug++;
   }
   else
       emit hexitechSignalError("HxtProcessing::pushTransferBuffer - Unable to acquire buffer lock!");

   if (framesMutex.tryLock(mutexTimeout))
   {
       framesQueue.enqueue(validFrames);
       framesMutex.unlock();
       iDebug++;
   }
   else
       emit hexitechSignalError("HxtProcessing::pushTransferBuffer - Unable to acquire frames lock!");

   if (iDebug == 2)
       /*qDebug() << "::pushTr'rBuffer() Added: " << transferBuffer << " to bufferQ and " << validFrames << " onto framesQ."*/;
   else
       qDebug() << "pushTr'rBuffer() failed to add 1/several of buffer & frames  onto respective Queues";

   /// For testing purposes only, this will be handed using a callback function later on:
   /*TODO: Move: */emit returnBufferReady(transferBuffer, validFrames)/**/;

}

void HxtProcessing::pushMotorPositions(QHash<QString, int> *qHashPositions)
{
    /// convert QHash into motorPositions struct object and place that object on motorQueue queue
    motorPositions *newPositions = copyQHashToMotorPositions(qHashPositions);
    if (motorMutex.tryLock(mutexTimeout))
    {
        motorQueue.enqueue(*newPositions);
        motorMutex.unlock();
    }
    else
        emit hexitechSignalError("HxtProcessing::pushMotorPositions() - Unable to acquire mutex lock!");

    delete newPositions;
    newPositions = 0;
}

motorPositions* HxtProcessing::copyQHashToMotorPositions(QHash<QString, int> *qHashPositions)
{
    /// Convert QHash into motorPositions object
    motorPositions *newPositions = new motorPositions;
    newPositions->mSSX     = -1;
    newPositions->mSSY     = -1;
    newPositions->mSSZ     = -1;
    newPositions->mSSROT   = -1;
    newPositions->mGALX    = -1;
    newPositions->mGALY    = -1;
    newPositions->mGALZ    = -1;
    newPositions->mGALROT  = -1;
    newPositions->mTimer   = -1;

    QHashIterator<QString, int> mpi(*qHashPositions);
    while (mpi.hasNext())
    {
        mpi.next();
        //qDebug() << mpi.key() << ":" << mpi.value();

        if (mpi.key() == "ssx")             newPositions->mSSX      = mpi.value();
        else if (mpi.key() == "ssy")        newPositions->mSSY      = mpi.value();
        else if (mpi.key() == "ssz")        newPositions->mSSZ      = mpi.value();
        else if (mpi.key() == "ssrot")      newPositions->mSSROT    = mpi.value();
        else if (mpi.key() == "galx")       newPositions->mGALX     = mpi.value();
        else if (mpi.key() == "galy")       newPositions->mGALY     = mpi.value();
        else if (mpi.key() == "galz")       newPositions->mGALZ     = mpi.value();
        else if (mpi.key() == "galrot")     newPositions->mGALROT   = mpi.value();
        else if (mpi.key() == "timer")      newPositions->mTimer    = mpi.value();
        else
        {
            emit hexitechSignalError(QString("Received unrecognised motor! Named: %1, Value: %2").arg(mpi.key(), mpi.value()) );
            qDebug() << "    HxtProcessing::copyQHashToMotorPositions() error! key: " << mpi.key() << " not recognised!";
        }
    }

    return newPositions;
}

void HxtProcessing::run()
{
    /// Thread's main function - Periodically check queues, processing them when specified condition met
    emit hexitechSignalError("HxtProcessing Thread up and running.");

    /// Call prepSettings function before may begin
    prepSettings();

    // Local variable to track status of fileQueue, motorQueue, bProcessQueueContents
    bool bFileQueueEmpty = true, bMotorQueueEmpty = true, bProcessTheQueue = false;
    bool bBufferQueueEmpty  = false, bFramesQueueEmpty = false, bProcessFiles = false;
    int currentCondition = -1;
    unsigned long newFrameSize = -1;

    /// Make decision to write to disk, tied to a Timer (e.g. once/second etc)
    bool bWriteToDisk = false;//true;
    // Start and stop timer to give it a sane (~0) value
    mDiscWritingTimer->start();//        qDebug() << " ! ! mdiscWritingTimer-->start() line: " << 515;
    mDiscWritingTimer->stop();//        qDebug() << " ! ! mdiscWritingTimer-->stop() line: " << 516;
    float discWritingInterval = 1.0, timeSinceLastDiscOp = 0.0;

    // Initialise timer tracking whether we are idling
    mIdleTimer->start();
    mIdleTimer->stop();

    float idleThreshold = 3.0, timeSinceLastCollection = 0.0;

    while (bThreadRunning)
    {
        // Check if bProcessQueueContents set
        if (qContentsMutex.tryLock(mutexTimeout))
        {
            // If bProcessQueueContents set, update local variable bProcessTheQueue then clear bProcessQueueContents
            if (bProcessQueueContents)
            {
                bProcessTheQueue = bProcessQueueContents;
                bProcessQueueContents = false;
            }
            qContentsMutex.unlock();
        }
        else
        {
            emit hexitechSignalError("HxtProcessing Unable to acquire queue contents mutex lock!");
            qDebug() << "HxtProcessing::run() I am unable to acquire queue contents mutex lock!";
        }

//        int fileNumbers = 0, motorNumbers = 0, bufferNumbers = 0;        /// DEBUGGING information:

        // Wait while either fileQueue, motorQueue, bufferQueue empty; Or break if bProcessTheQueue (bProcessQueueContents) is set
        do
        {
            // If User pressed Clear Unprocessed button while queue empty, reset the Boolean variable
            if (bRemoveUnprocessedFiles)
                bRemoveUnprocessedFiles = false;

            // Sleep then recheck whether both queues still empty
            Sleep(1);   /// Reduced from 500 to 1 ms for improved performance

            // Check if fileQueue's empty
            if (fileMutex.tryLock(mutexTimeout))
            {
                bFileQueueEmpty = fileQueue.isEmpty();
//                fileNumbers = fileQueue.size();         /// DEBUGGING info
                fileMutex.unlock();
            }
            else
                emit hexitechSignalError("HxtProcessing Unable to acquire File mutex lock!");

            // Check if motorQueue's empty
            if (motorMutex.tryLock(mutexTimeout))
            {
                bMotorQueueEmpty = motorQueue.isEmpty();
//                motorNumbers = motorQueue.size();         /// DEBUGGING info
                motorMutex.unlock();
            }
            else
                emit hexitechSignalError("HxtProcessing Unable to acquire Motor mutex lock!");

            /// Check if bufferQueue's empty    -    HexitecGigE Addition
            if (bufferMutex.tryLock(mutexTimeout))
            {
                bBufferQueueEmpty = bufferQueue.isEmpty();
//                bufferNumbers = bufferQueue.size();         /// DEBUGGING info
                bufferMutex.unlock();
            }
            else
                emit hexitechSignalError("HxtProcessing Unable to acquire Buffer mutex lock!");

            // Exit function when GUI is shut down
            if (!bThreadRunning)
                return;
        }
        while ( ((bFileQueueEmpty || bMotorQueueEmpty) && (!bProcessTheQueue)) && ((bBufferQueueEmpty || bMotorQueueEmpty) && (!bProcessTheQueue)) );
                //((bFileQueueEmpty || bMotorQueueEmpty || bBufferQueueEmpty) && (!bProcessTheQueue));   /// HexitecGigE  Addition

        /// Received something on the Queue(s) - Start the timer
//        mProcessingTimer->start();

//        qDebug() << " *1* 292 fileNumbers: " << fileNumbers << " motorNumbers: " <<motorNumbers << " bufferNumbers:" << bufferNumbers
//                 << "bProcessTheQ: " << bProcessTheQueue<< "(!bProcessTheQ=" << bProcessTheQueue << ") LOGIC: " <<
//                    ( ((bFileQueueEmpty || bMotorQueueEmpty) && (!bProcessTheQueue)) && ((bBufferQueueEmpty || bMotorQueueEmpty) && (!bProcessTheQueue)) );	/// DEBUGGING Info

         /// Remove this block?? Or Add bufferQueue to play safe??

        // If bProcessTheQueue is true but queue(s) empty then the daq has been re-initialised
        /// This block still needed... (For now?)
        //   Clear bProcessTheQueue without processing the queue
        if (bProcessTheQueue)
        {
            if(bFileQueueEmpty || bMotorQueueEmpty)
            {
                // bProcessTheQueue set prematurely, reset and re-run inner loop
                bProcessTheQueue = false;
                continue;
            }
        }

        // Obtain targetCondition value
        if (processingMutex.tryLock(mutexTimeout))
        {
            currentCondition = targetCondition;
            processingMutex.unlock();
        }
        else
        {
            emit hexitechSignalError("HxtProcessing Error: Unable to acquire processing mutex lock!");
        }

        // Signal to ProcessingWindow that queue contains unprocessed raw file(s),
        //   UNLESS: It's manual processing, or It's conditionEveryNewFile, or It's bDontDisplayProcessedData (Update Display: Manual)
        //    (either of these scenarios mean these raw file(s) will imminently be processed)
        if ((!bManualProcessingEnabled) && (currentCondition != conditionEveryNewFile) && (!bDontDisplayProcessedData))
        {
            emit hexitechUnprocessedFiles(true);
        }

        // If User selected Manual Processing but didn't press ProcessNow button,
        //  or, pressed the clearUnprocessed button,
        //  then clear the queues without processing them

        if ((bDontDisplayProcessedData && (!bManualProcessingEnabled))  // User selected Update Display: Manual
                || bRemoveUnprocessedFiles)                             // User clicked Clear Unprocessed Button
        {

            /// Clearing bufferQueue added; Clearing fileQueue no longer needed?

            // Clear bufferQueue
            if (bufferMutex.tryLock(mutexTimeout))
            {
                // Report Buffers before discarding them
                QQueue<unsigned short*>::iterator bufferIterator = bufferQueue.begin();
                for ( ; bufferIterator != bufferQueue.end(); bufferIterator++)
                    emit hexitechSignalError(QString("Ignoring Buffer: %1").arg( (unsigned long)*bufferIterator ));
                    ///qDebug() << "Qsize: " << bufferQueue.size()  << " clear bufferQueue: emit signal buffer  " << (unsigned long)*bufferIterator << " released?";
                // Discard file(s), unlock mutex
                bufferQueue.clear();
                bufferMutex.unlock();
            }
            else
            {
                hexitechSignalError("HxtProcessing Skipping Buffers Error: Unable to acquire buffer mutex lock!");
                break;
            }

            // Clear framesQueue
            if (framesMutex.tryLock(mutexTimeout))
            {
                // Report frames (per buffer) before discarding them
                QQueue<unsigned long>::iterator framesIterator = framesQueue.begin();
                for ( ; framesIterator != framesQueue.end(); framesIterator++)
                    emit hexitechSignalError(QString("Ignoring Frames: %1").arg( (unsigned long)*framesIterator ));
                    ////qDebug() << "Qsize:" << framesQueue.size() << " clear framesQueue: emit signal frames  " << (unsigned long)*framesIterator << " released?";
                // Discard frames, unlock mutex
                framesQueue.clear();
                framesMutex.unlock();
            }
            else
            {
                hexitechSignalError("HxtProcessing Skipping Framess Error: Unable to acquire frames mutex lock!");
                break;
            }

            // Clear fileQueue
            if (fileMutex.tryLock(mutexTimeout))
            {
                // Report filenames before discarding them
                QQueue<string>::iterator fileIterator = fileQueue.begin();
                for ( ; fileIterator != fileQueue.end(); fileIterator++)
                    emit hexitechSignalError(QString("Ignoring: %1").arg(string(*fileIterator).c_str()));
                // Discard file(s), unlock mutex
                fileQueue.clear();
                fileMutex.unlock();
            }
            else
            {
                hexitechSignalError("HxtProcessing Skipping Raw Files Error: Unable to acquire file mutex lock!");
                break;
            }

            // Clear motorQueue
            if (motorMutex.tryLock(mutexTimeout))
            {
                motorQueue.clear();
                motorMutex.unlock();
            }
            else
            {
                hexitechSignalError("HxtProcessing Skipping Motor Pos'n Error: Unable to acquire motor mutex lock!");
                break;
            }

            //  User pressed Clear Unprocessed button? Reset bool value, Signal raw files discarded
            if (bRemoveUnprocessedFiles)
            {
                bRemoveUnprocessedFiles = false;
                emit hexitechUnprocessedFiles(false);
            }

            // Reset queues' boolean variables and wait for new files
            bFileQueueEmpty = true;
            bMotorQueueEmpty = true;
            bBufferQueueEmpty = true;   /// HexitecGigE Addition
            continue;
        }

        // Was ProcessNowButton pressed?
        if (bManualProcessingEnabled)                   /// Remains unchanged (?)
        {
            // Confirm all files in queue should be manually processed
            if (this->confirmOnlyManualFilesInQueue())
            {
                // Process all files manually
                if (fileMutex.tryLock(mutexTimeout))
                {
                    while (!fileQueue.empty())
                    {
                        string newFile = fileQueue.dequeue();
                        //qDebug() << "Going to process filename: " <<  QString(newFile.c_str());
                        mRawFileNames.push_back(newFile);
                        qDebug() << "HxtProcessing Manual adding file: " << newFile.substr(0, 10).c_str() <<
                                    ".." << newFile.substr( newFile.length() -31, newFile.length()).c_str();
                    }
                    fileMutex.unlock();
                    // All Files to be manually processed dequeued: process them, signal gui when completed
                    bProcessFiles = true;
                    bWriteToDisk = true;
                    //qDebug() << "3"; Sleep(1000);
                    executeProcessing(bProcessFiles, bWriteToDisk);    /// process (Manual) Files
                    emit hexitechSignalManualProcessingFinished();
                    // Reset manual processing until user presses ProcessNow button again
                    bManualProcessingEnabled = false;
                }
                else
                {
                    emit hexitechSignalError("Manual Processing Error: Cannot acquire file mutex lock!");
                    break;
                }
                // Clear motor positions Queue
                if (motorMutex.tryLock(mutexTimeout))
                {
                    motorQueue.clear();
                    motorMutex.unlock();
                }
                else
                {
                    hexitechSignalError("HxtProcessing Skipping Motor Pos'n Error: Unable to acquire motor mutex lock!");
                    break;
                }
            }
            else
            {
                emit hexitechSignalError("Manual Processing Error: Detected file(s) that should be Automatically processed!");
                qDebug() << "*!*      Manual Processing Error: Detected file(s) that should be automatically processed!         *";
                break;
            }
        }
        else if (currentCondition == conditionEveryNewFile)         /// HexitecGigE: filenames replaced by buffers
        {
            // Process 1 buffer at a time
            if (bufferMutex.tryLock(mutexTimeout))
            {
                unsigned short* newBuffer = bufferQueue.dequeue();
                bufferMutex.unlock();
                ///qDebug() << "HxtProcessing 475 EveryNewFile adding buffer: " << (unsigned long*)*newBuffer;
                mBufferNames.push_back(newBuffer);

                if (motorMutex.tryLock(mutexTimeout))
                {
                    mPositions = motorQueue.dequeue();
                    motorMutex.unlock();
                }
                else
                {
                    emit hexitechSignalError("EveryNewFile Processing Error: Cannot acquire motor mutex lock!");
                    break;
                }

                if (framesMutex.tryLock(mutexTimeout))
                {
                    newFrameSize = framesQueue.dequeue();
                    mValidFrames.push_back(newFrameSize);
                    framesMutex.unlock();
                    bProcessFiles = false;  // Process this buffer
                    // Time to write data to disk?
                    mDiscWritingTimer->stop();//        qDebug() << " ! ! mdiscWritingTimer-->stop() line: " << 801;
                    timeSinceLastDiscOp += mDiscWritingTimer->elapsed();        if (timeSinceLastDiscOp > 5) timeSinceLastDiscOp = 0.0; // Avoid spurious timings        if (timeSinceLastDiscOp > 5) timeSinceLastDiscOp = 0.0; // Avoid spurious timings
                    qDebug() << " ** [Condition:EveryNewFile] timeSinceLastDiscOp: " << timeSinceLastDiscOp << " timer elapsed: " <<  mDiscWritingTimer->elapsed() << " logic: "
                             << (timeSinceLastDiscOp > discWritingInterval) << "Boolean: " << bWriteToDisk;
                    if (timeSinceLastDiscOp > discWritingInterval)
                    {
                        bWriteToDisk = true;
                        timeSinceLastDiscOp = 0;
                    }
                    mDiscWritingTimer->start();//        qDebug() << " ! ! mdiscWritingTimer-->start() line: " << 810;
                    executeProcessing(bProcessFiles, bWriteToDisk);    /// process 1 Buffer
                }
                else
                {
                    emit hexitechSignalError("EveryNewFile Processing Error: Cannot acquire frames mutex lock!");
                    break;
                }
            }
            else
            {
                emit hexitechSignalError("EveryNewFile Error: Cannot acquire buffer mutex lock!");
                break;
            }
        }
        else
        {
            /// Has Motor Positions changed?
            if (bufferMutex.tryLock(mutexTimeout))
            {
                int numberBuffersInQueue = bufferQueue.size();
                bufferMutex.unlock();
                int numberOfBuffersToProcess = -1, buffersDequeuedCount = 0;
                unsigned short* newBuffer;
                unsigned long newFrameSize;

                if (this->checkQueueForMotorChanges(currentCondition, &numberOfBuffersToProcess))         /// Only bufferQueue
                {
                    // Signal if this processing will leave any unprocessed buffers the queue
                    if (numberBuffersInQueue > numberOfBuffersToProcess)
                        emit hexitechUnprocessedFiles(true);            // Yes buffers will remain
                    else
                        emit hexitechUnprocessedFiles(false);           // No buffers will remain

                    // Depending upon numberOfBuffersToProcess, add buffer(s) onto mBufferNames
                    if (bufferMutex.tryLock(mutexTimeout))
                    {
                        if (motorMutex.tryLock(mutexTimeout))
                        {
                            if (framesMutex.tryLock(mutexTimeout))
                            {
                                while(buffersDequeuedCount < numberOfBuffersToProcess)
                                {
                                    newBuffer = bufferQueue.dequeue();
                                    mBufferNames.push_back(newBuffer);
                                    mPositions = motorQueue.dequeue();
                                    newFrameSize = framesQueue.dequeue();
                                    mValidFrames.push_back(newFrameSize);
                                    qDebug() << "Motor Changes: Adding buffer: " << (unsigned long)*newBuffer << " with: " << newFrameSize << "frames";
                                    buffersDequeuedCount++;
                                }
                                // Check whether framesQueue empty, then release mutex
                                bFramesQueueEmpty = framesQueue.isEmpty();
                                framesMutex.unlock();
                            }
                            else
                            {
                                emit hexitechSignalError("Motor Changes Error: Unable to acquire frames mutex lock!");
                            }
                            // Check whether motorQueue empty, then release mutex
                            bMotorQueueEmpty = motorQueue.isEmpty();
                            motorMutex.unlock();
                        }
                        else
                        {
                            emit hexitechSignalError("Motor Changes Error: Unable to acquire motor mutex lock!");
                            break;
                        }
                        // Check whether bufferQueue empty, then release mutex
                        bBufferQueueEmpty = bufferQueue.isEmpty();
                        bufferMutex.unlock();
                    }
                    else
                    {
                        emit hexitechSignalError("Motor Changes Error: (2) Unable to acquire buffer mutex lock!");
                        break;
                    }
                    // Process buffer(s)
                    bProcessFiles = false;  // Process this buffer
                    // Time to write data to disk?
                    mDiscWritingTimer->stop();//        qDebug() << " ! ! mdiscWritingTimer-->stop() line: " << 890;
                    timeSinceLastDiscOp += mDiscWritingTimer->elapsed();        if (timeSinceLastDiscOp > 5) timeSinceLastDiscOp = 0.0; // Avoid spurious timings        if (timeSinceLastDiscOp > 5) timeSinceLastDiscOp = 0.0; // Avoid spurious timings
                    qDebug() << " ** [Condition:MotorPos'n] timeSinceLastDiscOp: " << timeSinceLastDiscOp << " timer elapsed: " <<  mDiscWritingTimer->elapsed() << " logic: "
                             << (timeSinceLastDiscOp > discWritingInterval) << "Boolean: " << bWriteToDisk;
                    if (timeSinceLastDiscOp > discWritingInterval)
                    {
                        bWriteToDisk = true;
                        timeSinceLastDiscOp = 0;
                    }
                    mDiscWritingTimer->start();//        qDebug() << " ! ! mdiscWritingTimer-->start() line: " << 899;
                    executeProcessing(bProcessFiles, bWriteToDisk);        /// Process Buffer(s) [MotorPos'n]
                    continue;
                }
                else    // MotorPositions unchanged
                {
                    // No motor position changes found matching currentCondition - has GUI signalled
                    //      that all buffers in the queue should be processed?
                    if (bProcessTheQueue)        /// HexitecGigE Modification
                    {
                        // Process the queue
                        if (bufferMutex.tryLock(mutexTimeout))
                        {
                            if (framesMutex.tryLock(mutexTimeout))
                            {
                                numberOfBuffersToProcess = bufferQueue.size();
                                while (buffersDequeuedCount < numberOfBuffersToProcess)
                                {
                                    newBuffer = bufferQueue.dequeue();
                                    mBufferNames.push_back(newBuffer);
                                    newFrameSize = framesQueue.dequeue();
                                    mValidFrames.push_back(newFrameSize);
                                    emit hexitechSignalError(QString("Processing buffer: %1 (%2)").arg((unsigned long)*newBuffer, newFrameSize));
                                    qDebug() << "Processing the Queue: Adding buffer: " << newBuffer << "containing:" << newFrameSize;
                                    buffersDequeuedCount++;
                                }
                                framesMutex.unlock();
                            }
                            else
                            {
                                hexitechSignalError("HxtProcessing Processing the Queue Error: Unable to acquire frames mutex lock!");
                            }
                            bufferMutex.unlock();
                        }
                        else
                        {
                            hexitechSignalError("HxtProcessing Processing the Queue Error: Unable to acquire buffer mutex lock!");
                            break;
                        }

                        // Obtain first set of motor positions, then clear remainder of motorQueue
                        if (motorMutex.tryLock(mutexTimeout))
                        {
                            mPositions = motorQueue.dequeue();
                            motorQueue.clear();
                            motorMutex.unlock();
                        }
                        else
                        {
                            hexitechSignalError("HxtProcessing Processing the Queue Error: Unable to acquire motor mutex lock!");
                            break;
                        }
                        // Process buffer(s)
                        bProcessFiles = false;  // Process this buffer
                        // Time to write data to disk?
                        mDiscWritingTimer->stop();//        qDebug() << " ! ! mdiscWritingTimer-->stop() line: " << 954;
                        timeSinceLastDiscOp += mDiscWritingTimer->elapsed();        if (timeSinceLastDiscOp > 5) timeSinceLastDiscOp = 0.0; // Avoid spurious timings        if (timeSinceLastDiscOp > 5) timeSinceLastDiscOp = 0.0; // Avoid spurious timings
                        qDebug() << " ** [Condition:WholeQ?] timeSinceLastDiscOp: " << timeSinceLastDiscOp << " timer elapsed: " <<  mDiscWritingTimer->elapsed() << " logic: "
                                 << (timeSinceLastDiscOp > discWritingInterval) << "Boolean: " << bWriteToDisk;
                        if (timeSinceLastDiscOp > discWritingInterval)
                        {
                            bWriteToDisk = true;
                            timeSinceLastDiscOp = 0;
                        }
                        mDiscWritingTimer->start();//        qDebug() << " ! ! mdiscWritingTimer-->start() line: " << 963;
                        executeProcessing(bProcessFiles, bWriteToDisk);    /// Process Buffer(s)  [WholeQ?]

                        // Signal that no unprocessed buffers remains in bufferQueue
                        emit hexitechUnprocessedFiles(false);

                        // Clear bProcessTheQueue bool before next loop iteration
                        bProcessTheQueue = false;
                    }
                }
            }
            else
                emit hexitechSignalError("Motor Changes Error: (1) Unable to acquire buffer mutex lock!");
        }
        // Assume both queues empty until nested while loop performs checks again
        bFileQueueEmpty = true;
        bMotorQueueEmpty = true;
        bBufferQueueEmpty = true;
        bFramesQueueEmpty = true;
//        /// stop the timer and wait for next Q item
//        mProcessingTimer->stop();
//        qDebug() << "That Took " << mProcessingTimer->elapsed() << " secs overall to do "  << newFrameSize << " frames. Data rate: "
//                 << (6400 * 2 * newFrameSize) / mProcessingTimer->elapsed();
    }   // End of while (bThreadRunning)
}

bool HxtProcessing::checkQueueForMotorChanges(int currentCondition, int *numberOfFilesToProcess)
{
    /// Compare the first entry of motorQueue against that of each motor position
    /// in the queue until currentCondition's change found (or none found)
    ///     Return true if found, false otherwise

    motorPositions firstPositions = motorQueue.at(0);

    int conditionMatches = 0;
    bool bConditionFound = false;
    if (motorMutex.tryLock(mutexTimeout))
    {
        // Determine length of motorQueue
        int queueLength = motorQueue.size();
        for (int index = 1; index < queueLength; index++)
        {
            // Motor Any Step: (any one differ: mSSX, mSSY, mSSZ, mSSROT, mTimer, mGALX, mGALY, mGALZ, mGALROT)
            if ((firstPositions.mSSX != motorQueue.at(index).mSSX) || (firstPositions.mSSY != motorQueue.at(index).mSSY) ||
                    (firstPositions.mSSZ != motorQueue.at(index).mSSZ) || (firstPositions.mSSROT != motorQueue.at(index).mSSROT) ||
                    (firstPositions.mTimer != motorQueue.at(index).mTimer) ||
                    (firstPositions.mGALX != motorQueue.at(index).mGALX) || (firstPositions.mGALY != motorQueue.at(index).mGALY) ||
                    (firstPositions.mGALZ != motorQueue.at(index).mGALZ) || (firstPositions.mGALROT != motorQueue.at(index).mGALROT))
            {
                conditionMatches |= conditionMotorAnyStep;
                // Break if this is currentCondition
                if (currentCondition & conditionMatches)
                {
                    *numberOfFilesToProcess = index;
                    break;
                }
            }

            // Motor Position Step:
            //  1(+) of these differ: mSSX, mSSY, mSSZ, mSSROT, mGALX, mGALY, mGALZ, mGALROT.
            if ((firstPositions.mSSX != motorQueue.at(index).mSSX) || (firstPositions.mSSY != motorQueue.at(index).mSSY) ||
                    (firstPositions.mSSZ != motorQueue.at(index).mSSZ) || (firstPositions.mSSROT != motorQueue.at(index).mSSROT) ||
                    (firstPositions.mGALX != motorQueue.at(index).mGALX) || (firstPositions.mGALY != motorQueue.at(index).mGALY) ||
                    (firstPositions.mGALZ != motorQueue.at(index).mGALZ) || (firstPositions.mGALROT != motorQueue.at(index).mGALROT))
            {
                conditionMatches |= conditionMotorPositionStep;
                // Break if this is currentCondition
                if (currentCondition & conditionMatches)
                {
                    *numberOfFilesToProcess = index;
                    break;
                }
            }

            // Motor Time Step
            if (firstPositions.mTimer != motorQueue.at(index).mTimer)
            {
                conditionMatches |= conditionMotorTimeStep;
                // Break if this is currentCondition
                if (currentCondition & conditionMatches)
                {
                    *numberOfFilesToProcess = index;
                    break;
                }
            }
        }
        motorMutex.unlock();
    }
    else
    {
        emit hexitechSignalError("checkQueueForMotorChanges() Error: Couldn't obtain motor mutex lock!");
        qDebug() << "*!*       checkQueueForMotorChanges() Error: Couldn't obtain motor mutex lock!     * ";
    }

    // Check if processing condition found
    if (currentCondition & conditionMatches)
        bConditionFound = true;
    else
    {
        // processing condition not found, check if any condition present
        if (conditionMatches == 0)
            conditionMatches |= conditionNoneMet;       /// This line is debugging information only
    }

    return bConditionFound;
}

bool HxtProcessing::confirmOnlyManualFilesInQueue()
{
    /// Check motorQueue for file(s) that should not be manually processed.
    /// The motor positions belonging to a manual file should all be 1111111
    bool bOnlyManualFilesDetected = true;

    // Check motorQueue to confirm fileQueue contain file(s) to be manually processed
    if (motorMutex.tryLock(mutexTimeout))
    {
        int motorLength = motorQueue.size();

        int i = 0;
        for ( ; i < motorLength; i++)   // Is this the last file to be manual processed?
        {
            if ((motorQueue.at(i).mSSX != positionUnavailable) || (motorQueue.at(i).mSSY != positionUnavailable) ||
                    (motorQueue.at(i).mSSZ != positionUnavailable) || (motorQueue.at(i).mSSROT != positionUnavailable) ||
                    (motorQueue.at(i).mTimer != positionUnavailable) || (motorQueue.at(i).mGALX != positionUnavailable) ||
                    (motorQueue.at(i).mGALY != positionUnavailable) || (motorQueue.at(i).mGALZ != positionUnavailable) ||
                    (motorQueue.at(i).mGALROT != positionUnavailable))
            {
                bOnlyManualFilesDetected = false;
                break;
            }
        }
        motorMutex.unlock();
    }
    else
        emit hexitechSignalError("confirmOnlyManualFilesInQueue() Unable to acquire motor mutex!");

    return bOnlyManualFilesDetected;
}

void HxtProcessing::setRawFilesToProcess(QStringList rawFilesList)
{
    /// MainWindow uses this function to pass list of file(s) user selected
    /// to process manually when processNowButton pressed in ProcessingWindow

    // These are file(s) to be manually processed so there are no motor positions available
    //  Therefore we create motor positions with the value 1111111 to notify HxtProcessing
    rawFilesToProcess = rawFilesList;

    motorPositions *dummyPositions = new motorPositions;
    dummyPositions->mSSX     = positionUnavailable;
    dummyPositions->mSSY     = positionUnavailable;
    dummyPositions->mSSZ     = positionUnavailable;
    dummyPositions->mSSROT   = positionUnavailable;
    dummyPositions->mGALX    = positionUnavailable;
    dummyPositions->mGALY    = positionUnavailable;
    dummyPositions->mGALZ    = positionUnavailable;
    dummyPositions->mGALROT  = positionUnavailable;
    dummyPositions->mTimer   = positionUnavailable;

    qDebug() << "HxtProcessing files to manually process:\n----------------------------------------------";
    // Display files selected to be processed
    if (!rawFilesToProcess.empty())
    {
        QStringList::iterator listIterator;
        for (listIterator = rawFilesToProcess.begin(); listIterator != rawFilesToProcess.end(); listIterator++)
        {
            qDebug() << "** Adding file: " << (*listIterator).toStdString().c_str();

            // Add unprocessed file, dummy motor positions, onto respective queue
            if (fileMutex.tryLock(mutexTimeout))
            {
                if (motorMutex.tryLock(mutexTimeout))
                {
                    fileQueue.enqueue((*listIterator).toStdString());
                    motorQueue.enqueue((*dummyPositions));
                    motorMutex.unlock();
                }
                else
                    emit hexitechSignalError("Manual Processing Error: Unable to acquire motor mutex!");
                fileMutex.unlock();
            }
            else
                emit hexitechSignalError("Manual Processing Error: Unable to acquire file mutex!");
        }
    }
    else
        emit hexitechSignalError("Manual Processing Error: No raw files to process!");
    qDebug() << "reached the end of HxtProcessing::setBroughtFilesTwoProcess()" << endl;
    delete dummyPositions;
    dummyPositions = 0;
}

int HxtProcessing::checkConfigValid()
{
    /// Check for illegal configuration settings (returns nonzero value if so)

    // Cannot run Charge Sharing Addition and Charge Sharing Discriminator together
    if (mEnableCsaspCorrector && mEnableCsdCorrector)
    {
        emit hexitechSignalError("HxtProcessor Cannot combine Charge Sharing Addition and Charge Sharing Discriminator");
        return 1;
    }
    // Is one calibration file specified, but not the other?
    else if ( (mGradientsFile.empty() && !mInterceptsFile.empty()) || (!mGradientsFile.empty() && mInterceptsFile.empty()) )
    {
        emit hexitechSignalError("Must specify both Gradients and Intercepts files for calibration");
        return 2;
    }

    // Is momentum file specified?
    /// ToDo: Warn the user if file specified but corrector disabled ? (Vice versa?)
    if (!mMomentumFile.empty())
    {
        qDebug() << "DEBUG: mMomentumFile specified.";
        mEnableMomCorrector = true;
    }
    else
    {
        qDebug() << "DEBUG: mMomentumFile NOT specified.";
        mEnableMomCorrector = false;
    }

    // Cannot specify both global threshold and threshold file
    if ( (mGlobalThreshold != -1.0) && (!mThresholdFileName.empty()))
    {
        emit hexitechSignalError("Cannot specify both a global threshold and a threshold file");
        return 3;
    }
    return 0;
}


void HxtProcessing::dumpSettings()
{
    /// Debug function - call it to check hexitech settings in memory
    qDebug() << "Start ADU                       " << mHistoStartVal;
    qDebug() << "End ADU                         " << mHistoEndVal;
    qDebug() << "Bin Width ADU                   " << mHistoBins;
    qDebug() << "Interpolate Threshold           " << mInterpolationThreshold;
    qDebug() << "Induced Noise Fraction:         " << mInducedNoiseThreshold;
    qDebug() << "Pixel Threshold File:           " << mThresholdFileName.c_str();
    qDebug() << "Calibration:                    " << mEnableCabCorrector;
    qDebug() << "Momentum:                       " << mEnableMomCorrector;
    qDebug() << "Global Threshold                " << mGlobalThreshold;
    qDebug() << "Decoded Frame Filename:         " << mOutputFileNameDecodedFrame.c_str();
    qDebug() << "Subpixel Frame Filename:        " << mOutputFileNameSubPixelFrame.c_str();
    qDebug() << "CalibrationFile";
    qDebug() << "mGradientsFile:                 " << mGradientsFile.c_str();
    qDebug() << "mInterceptsFile:                " << mInterceptsFile.c_str();
    qDebug() << "mMomentumFile:                  " << mMomentumFile.c_str();
    qDebug() << "Charge Sharing";
    qDebug() << "Addition:                       " << mEnableCsaspCorrector;
    qDebug() << "Discrimination:                 " << mEnableCsdCorrector;
    qDebug() << "Incomplete Data:                " << mEnableIdCorrector;
    qDebug() << "Interpolate Correction:         " << mEnableIpCorrector;
    qDebug() << "Induced Noise Correction:       " << mEnableInCorrector;
    qDebug() << "Diagnostic Histogram CSV files: " << mWriteCsvFiles;
    qDebug() << "Vector Indexing:                " << mEnableVector;
    qDebug() << "Debug Frame                     " << mEnableDebugFrame;
}

void HxtProcessing::configHeaderEntries(string fileName)
{
    /// Prepare Format Version 2 header entries
    /// (file prefix, motor positions, timestamp)

    // Create typedef to store results from regex operations
    typedef match_results<const char*> cmatch;
    bool bOk = false;
    cmatch dateResults;

    string dateString  = string("");
    string filePrefix  = string("");

    // Check for date of format "YYMMDD_HHMMSS" within raw file name
    // e.g.:
    //     "Hexitec_140317_113831_reduced_0000.dat" -> dateString: "140317_113831"
    // Or: "20110315_173830_reduced_0000.dat"       -> dateString: "110315_173830"

    regex dateRegexFirst("([0-9]{6}[-_][0-9]{6})[a-zA-Z0-9_]+\\.dat");
    bOk = regex_search(fileName.c_str(), dateResults, dateRegexFirst);
    // If regular expression search successful, obtain date and file prefix
    if (bOk)
        dateString = dateResults[1];
    // Silently fail if no dateString in name:
//    else
//        emit hexitechSignalError("HxtProcessing: Raw filename's missing timestamp in format \"YYMMDD_HHMMSS\" or \"YYYYMMDD_HHHMMSS\" !");

    // Capture entire prefix (e.g. discard .dat from "Hexitec_140317_113831_reduced_0000.dat")
    QFileInfo fileInfo(fileName.c_str());
    QString fileCompleteBaseName = fileInfo.completeBaseName();
    filePrefix = fileCompleteBaseName.toStdString();

    if (bPrefixEnabled)
        mFilePrefix = filePrefix;
    else
        mFilePrefix = "(blank)";

    // If data timestamp enabled use dateStrings
    if (bTimeStampEnabled)
        mDataTimeStamp = string(dateString).substr(0, 13);  // Only accept 13 chars to avoid hosing the GUI..
    else
        mDataTimeStamp = string("000000_000000");

    /// Debugging purposes:
//    qDebug() << "*3* configHeaderEntries() dateString: " << dateString.c_str() << "(" << dateString.length() << "). filePrefix: "
//             << filePrefix.c_str() << "(" << filePrefix.length()  << ").";
//    qDebug() << "*4* configHeaderEntries() mFilePrefix: " << mFilePrefix.c_str();
//    qDebug() << "*5* configHeaderEntries() mDataTimeStamp: " << mDataTimeStamp.c_str() << "(" << mDataTimeStamp.length() << ").";
//    qDebug() << "*6* configHeaderEntries() mOutputFileD..Frame: " << mOutputFileNameDecodedFrame.c_str();
}

void HxtProcessing::obtainRawFilePathAndPrefix(string fileName)
{
    /// If selected, obtain raw file's path and prefix
    ///     and construct Decoded and SubPixel filenames from these
    string emptyString = "";
    string beforeString = mOutputFileNameDecodedFrame;       /// Debugging..
    // Check and remove a previous dateString from mOutputFileNameDecodedFrame
    //  e.g. does it already end with in "_YYMMDD_HHMMSS"?
    regex preventRx("_[0-9]{6}_[0-9]{6}");
    mOutputFileNameDecodedFrame = regex_replace(mOutputFileNameDecodedFrame, preventRx, emptyString);

    // Create typedef to store results from regex operations
    typedef match_results<const char*> cmatch;
    bool bOk = false;
    cmatch dateResults;

    string dateString  = string("");

    // Check for date ("YYMMDD_HHMMSS_") within raw file name
    // e.g.:
    //  "Hexitec_140317_113831_reduced_0000.dat" ->
    //  dateString: "140317_113831"

    regex dateStringRx("([0-9]{6}[-_][0-9]{6})[a-zA-Z0-9_]+\\.dat");
    bOk = regex_search(fileName.c_str(), dateResults, dateStringRx);
    // If regular expression search successful, obtain date and file prefix
    if (bOk)
        dateString = dateResults[1];
    else
    {
        regex dateStringSecond("([0-9]{8}[-_][0-9]{6})[a-zA-Z0-9_]+\\.dat");
        bOk = regex_search(fileName.c_str(), dateResults, dateStringSecond);
        if (bOk)
        {
            dateString = dateResults[1];
            // Reduce "_YYYYMMDD_HHMMSS" down to "_YYMMDD_HHMMSS_"
            //  e.g.  "_20140317_113831"   ->   "_140317_113831"
            dateString =   "_" + dateString.substr(3, dateString.length());
        }
        else
        {
            emit hexitechSignalError("obtainRawFilePathAndPrefix() Raw filename's missing timestamp in format \"YYMMDD_HHMMSS\" or \"YYYYMMDD_HHHMMSS\" !");
            qDebug() << "HxtProcessing::obtainRawFilePathAndPrefix() - Cannot find timestamp of format \"_YYMMDD_HHMMSS_\", nor \"_YYYYMMDD_HHHMMSS_\" within raw file!" << endl;
            return;
        }
    }
    // First timestamp is the path:
    size_t pathTimestamp = fileName.find(dateString);
    // Second timestamp is found by looking beyond the first timestamp:
    size_t fileTimestamp = fileName.find(dateString, pathTimestamp+1);

    //qDebug() << "__________________________________________________________";
    // Obtain path and file prefix but without the "_" character at the end (hence -1)
    string pathAndPrefix = fileName.substr(0, fileTimestamp-1);
    //qDebug() << "pathAndPrefix: " << pathAndPrefix.c_str() << "\nfileName: " << fileName.c_str();

    // Construct hexitech filename, using file path and prefix inserting pre-file extension characters and timestamp before file's extension:
    mOutputFileNameDecodedFrame = pathAndPrefix + "_" + string(dateString) + ".hxt";

    // Repeat operation upon mOutputFileNameSubPixelFrame if charge sharing enabled
    if (mEnableCsaspCorrector)
    {
        // Construct filename,  using file path and prefix inserting "_SubResolution_", pre-file extension characters and timestamp before file's extension:
        mOutputFileNameSubPixelFrame = pathAndPrefix + "_SubResolution" + "_" + string(dateString) + ".hxt";
    }
    qDebug() << "::obtainRaw..() Before: " << beforeString.c_str() << " after: " << mOutputFileNameDecodedFrame.c_str();

//    qDebug() << "dateString: " << dateString.c_str() << "\nmOutputFileNameDecodedFrame: " << mOutputFileNameDecodedFrame.c_str() <<
//            "\nmOutputFileNameSubPixelFrame: " << mOutputFileNameSubPixelFrame.c_str();
}

void HxtProcessing::clearMotorPositions()
{
    /// Clear motor positions by setting them to positionUninitialised
    mPositions.mSSX     = positionUninitialised;
    mPositions.mSSY     = positionUninitialised;
    mPositions.mSSZ     = positionUninitialised;
    mPositions.mSSROT   = positionUninitialised;
    mPositions.mGALX    = positionUninitialised;
    mPositions.mGALY    = positionUninitialised;
    mPositions.mGALZ    = positionUninitialised;
    mPositions.mGALROT  = positionUninitialised;
    mPositions.mTimer   = positionUninitialised;
}

void HxtProcessing::setManualProcessing(bool bManualEnabled)
{
    /// Enable MainWindow (via ProcessingWindow) to indicate ProcessNow button pressed
    bManualProcessingEnabled = bManualEnabled;
}

void HxtProcessing::handleReturnHxtBuffer(unsigned short *buffer)
{
    qDebug() << "HxtProcessing Returning buffer from the Visualisation tab";
    mHxtBuffers.push_back(buffer);
}

void HxtProcessing::savePrefix(bool bPrefixChecked)
{
    /// Should file prefix be saved to header
    bPrefixEnabled = bPrefixChecked;
}

void HxtProcessing::saveMotor(bool bMotorChecked)
{
    /// Should motor position be saved to header
    bMotorEnabled = bMotorChecked;
}

void HxtProcessing::saveTimeStamp(bool bTimeStampChecked)
{
    /// Should data time stamp be saved to header
    bTimeStampEnabled = bTimeStampChecked;
    // If disabled; clear mDataTimeStamp
    if (!bTimeStampEnabled)
        mDataTimeStamp = string("000000_000000");
}

void HxtProcessing::saveSameAsRawFile(bool bSameAsRawFileChecked)
{
    /// Is the same filename as the raw file name to be used?
    bUseRawFileEnabled = bSameAsRawFileChecked;
}

void HxtProcessing::changeProcessingCondition(processingCondition newCondition)
{
    if (processingMutex.tryLock(mutexTimeout))
    {
        targetCondition = newCondition;
        processingMutex.unlock();
    }
    else
        emit hexitechSignalError("changeProcessingCondition Error: Unable to acquire processingMutex lock!");
}

void HxtProcessing::processingVetoed(bool bProcessingOverruled)
{
    /// Invoked when raw data is not to be processed
    bDontDisplayProcessedData = bProcessingOverruled;
}

void HxtProcessing::removeFiles(bool bRemoveFiles)
{
    bRemoveUnprocessedFiles = bRemoveFiles;
}

void HxtProcessing::handleDataAcquisitionStatusChanged(DataAcquisitionStatus status)
{
   switch (status.getMajorStatus())
   {
   case DataAcquisitionStatus::ACQUIRING_DATA:
      switch (status.getMinorStatus())
      {
      case DataAcquisitionStatus::TRIGGERING_STOPPED:
         dataCollectionFinished();
         break;
      case DataAcquisitionStatus::DONE:
         dataCollectionFinished();
         break;
      }
      break;
   }
}

void HxtProcessing::dataCollectionFinished()
{
    /// Data collection finished, process queue contents
    if (qContentsMutex.tryLock(mutexTimeout))
    {
        bProcessQueueContents = true;
        qContentsMutex.unlock();
    }
    else
    {
        emit hexitechSignalError("HxtProcessing dataCollectionFinished() error: Unable to acquire queue context mutex lock!");
        qDebug() << "HxtProcessing dataCollectionFinished() error: Unable to acquire queue context mutex lock!";
    }

}

int HxtProcessing::executeProcessing(bool bProcessFiles, bool & bWriteFiles)
{
        string fileName;
    /// Execute processing of file(s) / buffer(s), depending upon setup
    //qDebug() << "HxtProcessing: ---=- 1156 -=--- BEGINNING buffer: " << mBufferNames.size() << " with: " << mValidFrames.size() << ". Manual file(s): " << mRawFileNames.size();

    if (mFirstBufferInCollection)
    {
        // Signal to GUI processing has begun
        emit hexitechRunning(true);
        mFirstBufferInCollection = false;
    }

    /// ----- Question-Mark: Should remove this block away from here? ----- ///

    /// Setup mDataTimeStamp, mFilePrefix, processed filename and file's path
    ///     This is a stupid duplication, courtesy to the irate compiler..
    if (bProcessFiles)
    {
        /// Using the name of file, obtain mFilePrefix & timestamp (remove & re-add timestamp from file name???)
        /// Obtain variables used for Format Version 2 header entries
        vector<string>::iterator fileIterator;
        fileIterator = mRawFileNames.begin();
        configHeaderEntries(*fileIterator);

        // Use raw file path and prefix if user made corresponding selection
        if (bUseRawFileEnabled)     /// Important: Ignore filename provided by pushTransfer
            obtainRawFilePathAndPrefix(*fileIterator);
    }
    else
    {
        /// Using the name of file, obtain mFilePrefix & timestamp (remove & re-add timestamp from file name???)
        /// Obtain variables used for Format Version 2 header entries
        fileName = mOutputFileNameDecodedFrame;
        qDebug() <<"PROCESSING FILE: " << QString::fromStdString(fileName);
        configHeaderEntries(fileName);

        // Use raw file path and prefix if user made corresponding selection
        if (bUseRawFileEnabled)
            obtainRawFilePathAndPrefix(fileName);
    }
    // Update with (possibly changed) values for  prefix, Motor position, timestamp
    dataProcessor->updateFilePrefix(mFilePrefix);
    dataProcessor->updateTimeStamp(mDataTimeStamp);

    /// ----- Question-Mark: Ending ----- ///

    // A new hexitech file will be produced before this function is finished
    //  Signal to mainwindow to remove any excess image Slice(s) in preparation
    emit hexitechRemoveAnyExcessSlices();

    /// Signal which file(s)/buffer(s) about to be processed
    if (bProcessFiles)
        emit hexitechConsumedFiles(mRawFileNames);
    else
    {
        //qDebug() << "no complaints from Q connect now.. ??";
        emit hexitechConsumedBuffers( mBufferNames);
    }

    /// Parse the specified raw file(s)/buffer(s)
    if (bProcessFiles)
    {
        dataProcessor->parseFile(mRawFileNames);
        // Clear filenames
        mRawFileNames.clear();
    }
    else
    {
        dataProcessor->parseBuffer(mBufferNames, mValidFrames);
        // Release all the buffers
        ///  TODO:  Implement Callback & Move into HxtRawDataProcessor
        vector<unsigned short*>::iterator bufferIterator;
        vector<unsigned long>::iterator   frameIterator;
        frameIterator = mValidFrames.begin();
        for (bufferIterator = mBufferNames.begin(); bufferIterator != mBufferNames.end(); bufferIterator++)
        {
            emit returnBufferReady(reinterpret_cast<unsigned char*>(*bufferIterator), *frameIterator);
            //emit returnBufferReady(transferBuffer, validFrames);
            ////qDebug() << "HxtProcessing: 1391 releasing  buffer: " << (unsigned long*)(*bufferIterator) << " with: " << *frameIterator;
            frameIterator++;
        }
        /// Clear both buffer and frames vectors
        mBufferNames.clear();
        mValidFrames.clear();
    }

//    qDebug() << "hxtProc:execProc() bwriteFiles: " << bWriteFiles << " bProcessFiles: " << bProcessFiles;
    // Time to write files/send file or a buffer to visualisation thread?
    if (bWriteFiles)
    {
        // Flush last frames through processor to output
        dataProcessor->flushFrames();       /// Independent of file/buffer selection

        // Interpolate pixel histograms if enabled
        if (mEnableIpCorrector) dataProcessor->InterpolateDeadPixels(mInterpolationThreshold);

        // Processing of file(s)?
//        if (bProcessFiles)
        {   qDebug() << "Time to process file(s)..";
            // Write output files
            dataProcessor->writePixelOutput(mOutputFileNameDecodedFrame);

            // Signal produced Hxt filename
            emit hexitechProducedFile(mOutputFileNameDecodedFrame);

            // Write subpixel file(s)? - To become redundant?
            if (mEnableCsaspCorrector)
            {
                dataProcessor->writeSubPixelOutput(mOutputFileNameSubPixelFrame);
                emit hexitechProducedFile(mOutputFileNameSubPixelFrame);
            }

            // Write CSV diagnostic histograms(s) - Visualisation thread to do this
            if (mWriteCsvFiles)
            {
                dataProcessor->writeCsvFiles();
                // Obtain filename of summed spectrum; signal it to processingwindow
                string sSpectrumFile = dataProcessor->getCorCsvFileName();
                QString spectrumFile = QString::fromStdString(sSpectrumFile);
                emit hexitechSpectrumFile(spectrumFile);
            }

//            // Signal that processed data [inside FILE] is ready to be displayed in the GUI - Redundant
//            //emit hexitechFilesToDisplay(  (QStringList() << QString::fromStdString(mOutputFileNameDecodedFrame)) );
//        }
//        else    //Processing of buffer(s)
//        {
            qDebug() << "Time to process the buffers ";
            /// Checkat least 1 buffer available before proceeding
            if (mHxtBuffers.empty())
            {
                emit hexitechSignalError("HxtProcessing Error: Pool of Buffers! (Visualisation thread too slow?)");
                qDebug() << "HxtProcessing Error: Pool of Buffers! (Visualisation thread too slow?)";
                return -1;
            }
            // Copy "output files" into buffer
            unsigned short* hxtBuffer = 0;
            hxtBuffer = mHxtBuffers.at(0);
            dataProcessor->copyPixelOutput(hxtBuffer);

            // Erase element 0 from vector
            mHxtBuffers.erase(mHxtBuffers.begin());

            // Signal that processed data [inside RAM] is ready to be displayed in the GUI
            // DSoFt: added filename to indicate when a new image/slice begins as this will change.
            // This is a quick fix and should be reviewed.
            qDebug() <<"#############emit hexitechBufferToDisplay: hxtBuffer contains: " << *hxtBuffer;
            emit hexitechBufferToDisplay(hxtBuffer, QString::fromStdString(fileName));
        }
        bWriteFiles = false;
    }

    LOG(gLogConfig, logNOTICE) << "Finished";

    /// Better place to signal processing completed than here?
    ///     (Will be signalling this A LOT during longer image collection..)
    // Signal to GUI processing completed
    emit hexitechRunning(false);

    return 0;
}

void HxtProcessing::commitConfigChanges()
{
    //dataProcessor->updateFormatVersion(mFormatVersion);
    dataProcessor->updateMotorPositions(mPositions.mSSX, mPositions.mSSY, mPositions.mSSZ,
                                        mPositions.mSSROT, mPositions.mTimer, mPositions.mGALX,
                                        mPositions.mGALY, mPositions.mGALZ, mPositions.mGALROT);
    dataProcessor->updateFilePrefix(mFilePrefix);
    dataProcessor->updateTimeStamp(mDataTimeStamp);
}


char* baseName(char* aPath) {
#if defined(OS_WIN)
    char delimiter = '\\';
#else
    char delimiter = '/';
#endif
    char *ptr = strrchr(aPath, delimiter);
    return ptr ? ptr + 1 : (char*)aPath;
}

} // End of namespace

