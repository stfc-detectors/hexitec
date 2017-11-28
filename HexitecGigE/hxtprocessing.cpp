/*
 * appMain.cpp - main application code for Hexitech raw data processing
 *
 *  Created on: 10 Dec 2010
 *      Author: Tim Nicholls
 */

#include "HxtProcessing.h"

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

    mFilePrefix    = "(blank)             ";
    mDataTimeStamp = string("000000_000000");
    mutexTimeout   = 50;  // Milliseconds duration to attempt acquiring a mutex lock

    bPrefixEnabled      = false;
//    bMotorEnabled       = false;
    bTimeStampEnabled   = false;
    bUseRawFileEnabled  = false;

    bThreadRunning      = true;

    // Initialise mPositions members to positionUninitialised (2222222)
//    clearMotorPositions();

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
    mAppName = aAppName;
    mFirstBufferInCollection = false;

    pixelThreshold = 0;
    // Create new raw data processor instance
    dataProcessor = 0;
    // 1. Induced Noise Corrector
    inCorrector = 0;
    // 2. Calibration
    // Create pointer to Gradients file
    gradientsContents = 0;
    // Create pointer to Intercepts file
    interceptsContents = 0;
    cabCorrector = 0;
    // 3. CS Addition / O R / CS Discriminator
    // 3.1 subpixel
    // Create subpixel corrector
    subCorrector = 0;
    // 3.2 CS Addition
    csdCorrector = 0;
    // 4. Incomplete Data
    idCorrector = 0;
    // 5. Momentum
    // Create pointer to Momentum file
    momentumContents = 0;
    momCorrector = 0;
    // x. Development purposes only, check for pixels read out more than once
    dbpxlCorrector = 0;

    gLogConfig = 0;

    /// Determine size pool containing buffers (communicate with Visualisation tab, for displaying)
    numHxtBuffers = 10;
    // Ensure memory etc only setup when prepSettings() called the first time
    bFirstTime = true;
    // How often (in seconds) should data be written to disc?
    mDiscWritingInterval = 1.0;
    // Processing Logging disabled by default (matching the GUI upon initialisation)
    bProcessingLoggingDisabled = false;//true;
    // Assume not using custom filename upon start
    bCustomFile = false;
    // Track when hxt file name changes
    mPreviousOutputFileName = "";
    bLastDataOfCurrentHxtFile = false;
}

HxtProcessing::~HxtProcessing()
{
    delete mProcessingTimer;
    // Signal to run() to shut down
    bThreadRunning = false;
    sleep(1);
    if (gLogConfig!= 0)
    {
        // Close log configuration and any associated files
        gLogConfig->close();
        delete gLogConfig;
    }
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
    vector<HxtBuffer*>::iterator bufferIterator;
    for (bufferIterator = mHxtBuffers.begin(); bufferIterator != mHxtBuffers.end(); bufferIterator++)
    {
        /*delete [] bufferIterator*/;   /// Revisit this..
    }

}

void HxtProcessing::prepSettings()
{
    if (bFirstTime)
    {
        // Setup pool of buffers - To send HXT file contents by RAM rather than file
        for (unsigned int i=0; i < numHxtBuffers; i++)
        {
            HxtBuffer* hxtBuffer = new HxtBuffer;
            mHxtBuffers.push_back(hxtBuffer);
        }
        bFirstTime = false;
    }

    if (gLogConfig != 0)
    {
        gLogConfig->close();
        delete gLogConfig;
    }

    // Obtain date stamp, save logging to the same folder as processed file
    DateStamp* now = new DateStamp();
    logFileStream.clear();
    logFileStream.str("");
    logFileStream << mFilePath;
    logFileStream << "/hexitech_log_";
    logFileStream << now->GetDateStamp();
    logFileStream << ".txt";
    delete(now);

    // Create new log configuration - Required whether logging disabled or not
    gLogConfig = new LogConfig( mAppName);

    /// 07/04/2016 Addition: User may disable Processing Logging
    if (bProcessingLoggingDisabled)
    {
        // Logging is disabled
        gLogConfig->setLogStdout(false);
    }
    else
    {
        gLogConfig->setLogStdout(true);
        gLogConfig->setLogFile(true, logFileStream.str());
    }

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
    if (pixelThreshold != 0) delete pixelThreshold;
    pixelThreshold = new HxtPixelThreshold(kHxtSensorRows, kHxtSensorCols);
    if (mGlobalThreshold != -1.0) {
        pixelThreshold->setGlobalThreshold(mGlobalThreshold);
    }
    if (!mThresholdFileName.empty()) {
        pixelThreshold->loadThresholds(mThresholdFileName);
    }

    // Create new raw data processor instance
    if (dataProcessor != 0) delete dataProcessor;
    dataProcessor = new HxtRawDataProcessor(kHxtSensorRows, kHxtSensorCols, mHistoStartVal, mHistoEndVal, mHistoBins,
                                                                 mFormatVersion,
                                                                 mPositions.mSSX, mPositions.mSSY, mPositions.mSSZ, mPositions.mSSROT,
                                                                 mPositions.mTimer,
                                                                 mPositions.mGALX, mPositions.mGALY, mPositions.mGALZ, mPositions.mGALROT,
                                                                 mFilePrefix, mDataTimeStamp, mEnableCallback);

    /// DEBUGGING frame by frame [redundant later on]
    //dataProcessor->setDebugFrameDir("U:/BInMe/debug_frames/");
    //// Disable this for now..
    //dataProcessor->setEnableDebugFrame(true);   /// ENABLED, for now - Testing time

//    // If Motor Positions not selected, pass dummy values as Motor Positions
//    if (!bMotorEnabled)
//    {
//        int dummyPos = positionUninitialised;
//        dataProcessor->updateMotorPositions(dummyPos, dummyPos, dummyPos, dummyPos, dummyPos, dummyPos, dummyPos, dummyPos, dummyPos);
//    }
//    else
//    {
//        dataProcessor->updateMotorPositions(mPositions.mSSX, mPositions.mSSY, mPositions.mSSZ, mPositions.mSSROT, mPositions.mTimer,
//                                            mPositions.mGALX, mPositions.mGALY, mPositions.mGALZ, mPositions.mGALROT);
//    }
    dataProcessor->updateTimeStamp(mDataTimeStamp);

    // Set up vector implementation
    dataProcessor->setVector(mEnableVector);

    //  Enable debug frames - each frame written to file before/after being processed
//    dataProcessor->setDebugFrameDir(mDebugFrameDir);
//    dataProcessor->setEnableDebugFrame(mEnableDebugFrame);

    // Set debug flags if debug level non-zero
    if (mDebugLevel) dataProcessor->setDebug(true);

    // Apply thresholds to raw data processor
    dataProcessor->applyPixelThresholds(pixelThreshold);

    // Create and register frame correctors with raw data processor if enabled

    // 1. Induced Noise Corrector
    if (inCorrector != 0) delete inCorrector;
    inCorrector = new HxtFrameInducedNoiseCorrector(mInducedNoiseThreshold);
    // Register it, enable debug if needed
    if (mDebugLevel) inCorrector->setDebug(true);
    if (mEnableInCorrector) {
        dataProcessor->registerCorrector(dynamic_cast<HxtFrameCorrector*>(inCorrector));
        LOG(gLogConfig, logINFO) << "Applying " << inCorrector->getName() << " corrector to data";
    }

    // 2. Calibration

    // Create pointer to Gradients file
    if (gradientsContents != 0) delete gradientsContents;
    gradientsContents = new HxtPixelThreshold(kHxtSensorRows, kHxtSensorCols);
    // Create pointer to Intercepts file
    if (interceptsContents != 0) delete interceptsContents;
    interceptsContents = new HxtPixelThreshold(kHxtSensorRows, kHxtSensorCols);

    if (cabCorrector != 0)   delete cabCorrector;
    cabCorrector = 0;

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
    if (subCorrector != 0) delete subCorrector;
    subCorrector = new HxtFrameChargeSharingSubPixelCorrector();
    // Register it, enable debug if needed
    if (mDebugLevel) subCorrector->setDebug(true);
    if (mEnableCsaspCorrector) {
        dataProcessor->registerCorrector(dynamic_cast<HxtFrameCorrector*>(subCorrector));
        LOG(gLogConfig, logINFO) << "Applying " << subCorrector->getName() << " corrector to data";
    }

    // 3.2 CS Addition
    if (csdCorrector!= 0) delete csdCorrector;
    csdCorrector = new HxtFrameChargeSharingDiscCorrector();
    if (mDebugLevel) csdCorrector->setDebug(true);
    if (mEnableCsdCorrector) {
        dataProcessor->registerCorrector(dynamic_cast<HxtFrameCorrector*>(csdCorrector));
        LOG(gLogConfig, logINFO) << "Applying " << csdCorrector->getName() << " corrector to data";
    }

    // 4. Incomplete Data
    if (idCorrector != 0) delete idCorrector;
    idCorrector = new HxtFrameIncompleteDataCorrector();
    if (mDebugLevel) idCorrector->setDebug(true);
    if (mEnableIdCorrector) {
        dataProcessor->registerCorrector(dynamic_cast<HxtFrameCorrector*>(idCorrector));
        LOG(gLogConfig, logINFO) << "Applying " << idCorrector->getName() << " corrector to data";
    }

    // 5. Momentum

    // Create pointer to Momentum file
    if (momentumContents != 0) delete momentumContents;
    momentumContents = new HxtPixelThreshold(kHxtSensorRows, kHxtSensorCols);
    if (momCorrector != 0) momCorrector = 0;

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
    if (dbpxlCorrector != 0) delete dbpxlCorrector;
    dbpxlCorrector = new HxtFrameDoublePixelsCorrector();
    if (mEnableDbPxlsCorrector)  {
        if (mDebugLevel) dbpxlCorrector->setDebug(true);
        dataProcessor->registerCorrector(dynamic_cast<HxtFrameCorrector*>(dbpxlCorrector));
        LOG(gLogConfig, logINFO) << "Applying " << dbpxlCorrector->getName() << " corrector to data";
    }
}

void HxtProcessing::setTargetDirectory(string aFileName)
{
    QFileInfo fileInfo(aFileName.c_str());
    mFilePath = fileInfo.absolutePath().toStdString();
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
//   qDebug() << " -=-=- pushRawFileName() " << aFileName.c_str() << "frameSize (bytes) = " << frameSize;

   // Setup file path before calling prepSettings() (which instantiates the logger)
   this->setTargetDirectory(aFileName);
   this->prepSettings();

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
   qDebug() << "The image is complete and has " << totalFramesAcquired << "total frames";
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

//   qDebug() << "    ::pushTransferBuffer() transferBuffer: " << (long*)transferBuffer << " containing: " << validFrames << " frames.";
//   LOG(gLogConfig, logNOTICE) <<  "transferBuffer: " << (long*)transferBuffer << " containing: " << validFrames << " frames.";

   int iDebug = 0;
   if (bufferMutex.tryLock(mutexTimeout))
   {
       bufferQueue.enqueue(newBuffer);
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
}

//void HxtProcessing::pushMotorPositions(QHash<QString, int> *qHashPositions)
//{
//    /// convert QHash into motorPositions struct object and place that object on motorQueue queue
//    motorPositions *newPositions = copyQHashToMotorPositions(qHashPositions);
//    if (motorMutex.tryLock(mutexTimeout))
//    {
//        motorQueue.enqueue(*newPositions);
//        motorMutex.unlock();
//    }
//    else
//        emit hexitechSignalError("HxtProcessing::pushMotorPositions() - Unable to acquire mutex lock!");

//    delete newPositions;
//    newPositions = 0;
//}

//motorPositions* HxtProcessing::copyQHashToMotorPositions(QHash<QString, int> *qHashPositions)
//{
//    /// Convert QHash into motorPositions object
//    motorPositions *newPositions = new motorPositions;
//    newPositions->mSSX     = -1;
//    newPositions->mSSY     = -1;
//    newPositions->mSSZ     = -1;
//    newPositions->mSSROT   = -1;
//    newPositions->mGALX    = -1;
//    newPositions->mGALY    = -1;
//    newPositions->mGALZ    = -1;
//    newPositions->mGALROT  = -1;
//    newPositions->mTimer   = -1;

//    QHashIterator<QString, int> mpi(*qHashPositions);
//    while (mpi.hasNext())
//    {
//        mpi.next();
//        //qDebug() << mpi.key() << ":" << mpi.value();

//        if (mpi.key() == "ssx")             newPositions->mSSX      = mpi.value();
//        else if (mpi.key() == "ssy")        newPositions->mSSY      = mpi.value();
//        else if (mpi.key() == "ssz")        newPositions->mSSZ      = mpi.value();
//        else if (mpi.key() == "ssrot")      newPositions->mSSROT    = mpi.value();
//        else if (mpi.key() == "galx")       newPositions->mGALX     = mpi.value();
//        else if (mpi.key() == "galy")       newPositions->mGALY     = mpi.value();
//        else if (mpi.key() == "galz")       newPositions->mGALZ     = mpi.value();
//        else if (mpi.key() == "galrot")     newPositions->mGALROT   = mpi.value();
//        else if (mpi.key() == "timer")      newPositions->mTimer    = mpi.value();
//        else
//            emit hexitechSignalError(QString("Received unrecognised motor! Named: %1, Value: %2").arg(mpi.key()).arg(mpi.value()) );
//    }

//    return newPositions;
//}

void HxtProcessing::run()
{
    /// Thread's main function - Periodically check queues, processing them when specified condition met
    emit hexitechSignalError("HxtProcessing Thread up and running.");

    // Local variable to track status of fileQueue, motorQueue, bProcessQueueContents
    bool bFileQueueEmpty = true, /*bMotorQueueEmpty = true,*/ bProcessTheQueue = false;
    bool bBufferQueueEmpty  = false;
    int currentCondition = -1;

    // Start and stop timer to give it a sane (~0.0) value
    mDiscWritingTimer->start();
    mDiscWritingTimer->stop();

    bDebug = false; /// Debugging purposes only

    bool bLastHexitechRunning = false;    // Remember last hexitechRunning signal sent

    while (bThreadRunning)
    {
        /// Setup infinite loop - break condition later on (dependent upon Queue(s), Etc)
        do
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
                emit hexitechSignalError("HxtProcessing Unable to acquire queue contents mutex lock!");

            if (bRemoveUnprocessedFiles)                // If User pressed Clear Unprocessed button while queue empty, reset the Boolean variable
                bRemoveUnprocessedFiles = false;

            if (!bThreadRunning)                        // Exit function when GUI is shut down
                return;

            Sleep(1);                                   // Sleep then recheck whether queues still empty

            if (fileMutex.tryLock(mutexTimeout))        // Check if fileQueue's empty?
            {
                bFileQueueEmpty = fileQueue.isEmpty();
                fileMutex.unlock();
            }
            else
                emit hexitechSignalError("HxtProcessing Unable to acquire File mutex lock!");

//            if (motorMutex.tryLock(mutexTimeout))       // Check if motorQueue's empty?
//            {
//                bMotorQueueEmpty = motorQueue.isEmpty();
//                motorMutex.unlock();
//            }
//            else
//                emit hexitechSignalError("HxtProcessing Unable to acquire Motor mutex lock!");

            if (bufferMutex.tryLock(mutexTimeout))      /// Check if bufferQueue's empty?
            {
                bBufferQueueEmpty = bufferQueue.isEmpty();
                bufferMutex.unlock();
            }
            else
                emit hexitechSignalError("HxtProcessing Unable to acquire Buffer mutex lock!");

            if ( /*!bMotorQueueEmpty && */(!bFileQueueEmpty || !bBufferQueueEmpty) )   // Break out and process the Queue (if Boolean set)
            {
                break;
            }
            else if (bProcessTheQueue)
            {
                break;              ///  Break if Motor Queue not empty, provided that either of File or Buffer Queue  also not empty
            }
        }
        while( 1);

        // Tell processing tab we are no longer idling - But don't repeat
        if (mTimeSinceLastDiscOp > 0.0)
        {
            if (!bLastHexitechRunning)
            {
                emit hexitechRunning(true);
                bLastHexitechRunning = true;
            }
        }

        if (bProcessTheQueue)
        {
            // Whether Queue(s) empty or not - Ensure unwritten buffer(s) "flushed" to disk and Visualisation tab
            mTimeSinceLastDiscOp += mDiscWritingInterval;
            // Tell processing tab we (very soon will be) idle
            emit hexitechRunning(false);
            bLastHexitechRunning = false;
        }

        /// First off, catch choices where no processing to be performed

        if (bRemoveUnprocessedFiles)    // User clicked clearedUnprocessedButton in processingwindow's tab
        {
            clearAllQueues();    /// bRemoveUnprocessedFiles cleared within this function
            continue;
        }
        if (bDontDisplayProcessedData && (!bManualProcessingEnabled))   // User selected manual (i.e. no) processing
        {
            if (bProcessTheQueue)
                bProcessTheQueue = false;

            clearAllQueues();
            continue;
        }

        /// Ok, processing selected - decide what kind  (of processing), then if it's been fulfilled

        if (processingMutex.tryLock(mutexTimeout))        // Obtain targetCondition value
        {
            currentCondition = targetCondition;
            processingMutex.unlock();
        }
        else
            emit hexitechSignalError("HxtProcessing Error: Unable to acquire processing mutex lock!");

        // Did the user select Raw file(s) to be processed manually?
        if (bManualProcessingEnabled)
        {
            // Ensure custom filename (if user selected it)
            bCustomFile = bCustomFileNameSelected;
            performManualProcessing();
        }
        else
        {
            // Some type of Automatic Processing - Start the Timer
            mDiscWritingTimer->start();

            if (currentCondition == conditionEveryNewFile)         /// HexitecGigE: filenames replaced by buffers
            {
                prepareSingleProcessing(bProcessTheQueue);
            }
            else    /// Condition is a change of motor position {conditionMotorAnyStep, conditionMotorPositionStep, conditionMotorTimeStep}
            {
                if (bProcessTheQueue)   /// Set when image collection finished - So process the lot, no more data arriving before next user-GUI interaction
                {
                    // Call with argument = false, to grab everything of the queues
                    mTimeSinceLastDiscOp += mDiscWritingInterval;
                    bProcessTheQueue = false;
                    prepareWholeQueueProcessing(bProcessTheQueue);
                }
                else
                {
                    int numberOfBuffersToProcess = -1;

                    // Has motorPositions changed?
                    if (this->checkQueueForMotorChanges(currentCondition, &numberOfBuffersToProcess))
                    {
                        /* Desired motorPosition change DID happen */

                        prepareMotorPositionProcessing(&numberOfBuffersToProcess);
                        dataProcessor->resetHistograms();
                    }
                    else
                    {
                        /* motorPosition change DIDN'T occur  - HOWEVER, we cannot empty the queue every time
                         *      or we will never see a motor step changing. SOLUTION: Always leave last item in the queue alone */

                        bool bProcessContentsSoFar = true;  /// Need not change bool, because the processing of the entire queue's handled by bProcessTheQueue (set when image collection finished)

                        prepareWholeQueueProcessing(bProcessContentsSoFar);    // bProcessTheQueue = bProcessContentsSoFar here
                    }
                }

            } // End of else (i.e. not conditionEveryNewFile)

        } // End of else (if bManualProcessingEnabled)

    } // End of while (bThreadRunning)
}

bool HxtProcessing::prepareSingleProcessing(bool &bProcessTheQueue)
{
    if (bProcessTheQueue)
    {
        // Set argument to false and feed into prepareWholeQueueProcessing
        //  to signal that (all remaining) buffer(s) to be processed
        bProcessTheQueue = false;
        prepareWholeQueueProcessing(bProcessTheQueue);
    }
    else
    {
        if (bufferMutex.tryLock(mutexTimeout))
        {
            unsigned short* newBuffer = bufferQueue.dequeue();
            bufferMutex.unlock();

            mBufferNames.push_back(newBuffer);

            if (motorMutex.tryLock(mutexTimeout))
            {
                mPositions = motorQueue.dequeue();
                motorMutex.unlock();
            }
            else
            {
                emit hexitechSignalError("EveryNewFile Processing Error: Cannot acquire motor mutex lock!");
                return false;
            }

            if (framesMutex.tryLock(mutexTimeout))
            {
                long newFrameSize = framesQueue.dequeue();
                mValidFrames.push_back(newFrameSize);
                framesMutex.unlock();
            }
            else
            {
                emit hexitechSignalError("EveryNewFile Processing Error: Cannot acquire frames mutex lock!");
                return false;
            }
        }
        else
        {
            emit hexitechSignalError("EveryNewFile Error: Cannot acquire buffer mutex lock!");
            return false;
        }
        bool bProcessFiles = false;  // true = file, false = buffer
        executeProcessing(bProcessFiles);    /// process 1 Buffer
    }
    return true;
}

bool HxtProcessing::prepareMotorPositionProcessing(int *numberOfBuffersToProcess)
{
    // Depending upon numberOfBuffersToProcess, add buffer(s) onto mBufferNames
    if (bufferMutex.tryLock(mutexTimeout))
    {
        int numberBuffersInQueue = bufferQueue.size();
        int buffersDequeuedCount = 0;
        unsigned short* newBuffer;
        unsigned long newFrameSize;

        // Signal if this processing will leave any unprocessed buffers the queue
        if (numberBuffersInQueue > *numberOfBuffersToProcess)
            emit hexitechUnprocessedFiles(true);            // Yes buffers will remain
        else
            emit hexitechUnprocessedFiles(false);           // No buffers will remain

        if (motorMutex.tryLock(mutexTimeout))
        {
            if (framesMutex.tryLock(mutexTimeout))
            {
                if (bDebug) qDebug() << "HxtProcessing::prepareMotorPositionProcessing() Adding " << *numberOfBuffersToProcess << " buffer(s).";
                while(buffersDequeuedCount < *numberOfBuffersToProcess)
                {
                    newBuffer = bufferQueue.dequeue();
                    mBufferNames.push_back(newBuffer);
                    mPositions = motorQueue.dequeue();
                    newFrameSize = framesQueue.dequeue();
                    mValidFrames.push_back(newFrameSize);
                    //qDebug() << "Motor Changes: Adding buffer: " << (unsigned long)*newBuffer << " with: " << newFrameSize << "frames";
                    buffersDequeuedCount++;
                }
                framesMutex.unlock();
                /// Because motor position change to occurred, must ensure data written to file now (and Visualisation tab updated)
                mTimeSinceLastDiscOp += mDiscWritingInterval;
            }
            else
            {
                emit hexitechSignalError("HxtProcessing::prepareMotorPositionProcessing() Error: Unable to acquire frames mutex lock!");
                return false;
            }
            motorMutex.unlock();
        }
        else
        {
            emit hexitechSignalError("HxtProcessing::prepareMotorPositionProcessing() Error: Unable to acquire motor mutex lock!");
            return false;
        }
        bufferMutex.unlock();
    }
    else
    {
        emit hexitechSignalError("HxtProcessing::prepareMotorPositionProcessing() Error: Unable to acquire buffer mutex lock!");
        return false;
    }
//    }
    bool bProcessFiles = false;  // Process buffer
    executeProcessing(bProcessFiles);        /// Process Buffer(s) [MotorPos'n Changed]
//    qDebug() << "   ::PerformMotorPositionProcessing() Having called executeProcessing, time to change the .HXT file name.";
//    qDebug() << "Before:       mOutputFileNameDecodedFrame = " << mOutputFileNameDecodedFrame.c_str();
    mOutputFileNameDecodedFrame = deduceNewHxtFileName(mOutputFileNameDecodedFrame);
//    qDebug() << "After:        mOutputFileNameDecodedFrame = " << mOutputFileNameDecodedFrame.c_str();
    return true;
}

string HxtProcessing::deduceNewHxtFileName(string fileName)
{
    string emptyString = "";

    // Create typedef to store results from regex operations
    typedef match_results<const char*> cmatch;
    bool bOk = false;
    cmatch dateResults;

    string dateString  = string("");

    // Check for date ("YYMMDD_HHMMSS") within raw file name
    // e.g.:
    //  "Hexitec_140317_11383.hxt" ->
    //  dateString: "140317_113831"

    regex dateStringRx("([0-9]{6}[-_][0-9]{6})");
    bOk = regex_search(fileName.c_str(), dateResults, dateStringRx);
    // If regular expression search successful, obtain date and file prefix
    if (bOk)
    {
        dateString = dateResults[1];
    }
    else
    {
        regex dateStringSecond("([0-9]{8}[-_][0-9]{6})[a-zA-Z0-9_]+");
        //regex dateStringSecond("([0-9]{8}[-_][0-9]{6})[a-zA-Z0-9_]+\\.hxt");
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
            qDebug() << "   Cannot find timestamp of format \"_YYMMDD_HHMMSS_\", nor "
                     << "\"_YYYYMMDD_HHHMMSS_\" within raw file!" << endl;
            // Locate  where file extension begins
            size_t fileExtension = fileName.find(".hxt");

            /// Repeating some of the code below..

            // Obtain date stamp, save logging to the same folder as processed file
            DateStamp* now = new DateStamp();
            string dString = now->GetDateStamp();
            delete(now);

            // Date format: "2016-02-10_10-36-27", Remove -'s characters
            regex removeUnderscoreRx("[-]");
            dString = regex_replace(dString, removeUnderscoreRx, emptyString);

            // Reduce year from YYYY -> YY; e.g. 2016 -> 16
            string dFinishedString = dString.substr(2,dString.size());

            string absolutePathNoTimestamp = fileName.substr(0, fileExtension);

            string newCompleteFileName = absolutePathNoTimestamp + dFinishedString + ".hxt";
            //qDebug() << "   Our candidates new file name: " << newCompleteFileName.c_str() << " (original file had no timestamp, but this one does)";
            return newCompleteFileName;
            /// -----
        }
    }


    // First timestamp is the path:
    size_t pathTimestamp = fileName.find(dateString);

    // Obtain path and file prefix without timestamp
    string absolutePathWithoutTimestamp = fileName.substr(0, pathTimestamp);
//    qDebug() << "What you want == absolutePathWithoutTimestamp: " << absolutePathWithoutTimestamp.c_str() << " pathTimestamp: " << pathTimestamp <<
//                " Sanity check, emptyString: " << emptyString.c_str()  << " Containing " << emptyString.size() << " character(s)";

    // Obtain date stamp, save logging to the same folder as processed file
    DateStamp* now = new DateStamp();
    string dString = now->GetDateStamp();
    delete(now);

    // Date format: "2016-02-10_10-36-27", Remove -'s characters
    regex removeUnderscoreRx("[-]");
    dString = regex_replace(dString, removeUnderscoreRx, emptyString);
    // Reduce year from YYYY -> YY; e.g. 2016 -> 16
    string dFinishedString = dString.substr(2,dString.size());

    string newCompleteFileName = absolutePathWithoutTimestamp + dFinishedString + ".hxt";
//    qDebug() << "   Our candidates new file name: " << newCompleteFileName.c_str();
    return newCompleteFileName;
}

bool HxtProcessing::performManualProcessing()
{
    //qDebug() << "HxtProcessing::performManualProcessing() called by run() [Manual processing to be done]";

    bool bUpdateFileName = true;
    // Setup debugging purposes
    int motorSize = -1, bufferSize = -1, fileSize = -1, framesSize = -1;
    checkQueueLengths(motorSize, bufferSize, fileSize, framesSize);

    // DEBUGGING stuff:
//    qDebug() << " DEBUGGING:  motorSize (" << motorSize << ") bufferSize (" << bufferSize
//             << ") framesSize: (" << framesSize << ") fileSize: (" << fileSize << ")";

    if (this->confirmOnlyManualFilesInQueue())
    {
        // Process all files manually
        if (fileMutex.tryLock(mutexTimeout))
        {
            while (!fileQueue.empty())
            {
                string newFile = fileQueue.dequeue();
//                qDebug() << "Going to process filename: " <<  QString(newFile.c_str());
                mRawFileNames.push_back(newFile);
                /// Grab file name of first file & use as .HXT filename - BUT ONLY if custom filename NOT selected
                if (!bCustomFile)
                {
                    if (bUpdateFileName)
                    {
                        size_t fileExtensionPosn = newFile.find(".bin");
                        string fileNameWithoutExtension = newFile.substr( 0, fileExtensionPosn);
                        string fileNameWithHxt          = fileNameWithoutExtension + ".hxt";
                        //qDebug() << ":: performManualProcessing() hanging mOutputFileNameDecodedFrame: " << fileNameWithHxt.c_str();
                        mOutputFileNameDecodedFrame     = fileNameWithHxt;
                        bUpdateFileName = false;
                    }
                }
            }
            fileMutex.unlock();
            // All Files to be manually processed dequeued: process them, signal gui when completed
            bool bProcessFiles = true;

            executeProcessing(bProcessFiles);    /// process (Manual) Files
            emit hexitechSignalManualProcessingFinished();
            // Reset manual processing until user presses ProcessNow button again
            bManualProcessingEnabled = false;
        }
        else
        {
            emit hexitechSignalError("HxtProcessing::performManualProcessing() Error: Cannot acquire file mutex lock!");
        }
        // Clear motor positions Queue
        if (motorMutex.tryLock(mutexTimeout))
        {
            motorQueue.clear();
            motorMutex.unlock();
        }
        else
        {
            hexitechSignalError("HxtProcessing::performManualProcessing() Error: Unable to acquire motor mutex lock!");
        }
    }
    else
    {
        emit hexitechSignalError("HxtProcessing::performedManualProcessing() Error - Detected file(s) that should have been Automatically processed!");
    }

    bManualProcessingEnabled = false;
    return true;
}

void HxtProcessing::clearAllQueues()
{
//    qDebug() << "HxtProcessing::clearAllQueues() called by run() [Clearing all the queues]   !!!";
    // Return buffer(s), frames from bufferQueue, framesQueue respectively
    if (bufferMutex.tryLock(mutexTimeout))
    {
        // Clear framesQueue
        if (framesMutex.tryLock(mutexTimeout))
        {
            // Report Buffers before discarding them
            QQueue<unsigned short*>::iterator bufferIterator = bufferQueue.begin();
            QQueue<unsigned long>::iterator framesIterator = framesQueue.begin();
            for ( ; bufferIterator != bufferQueue.end(); bufferIterator++)
            {
                emit hexitechSignalError(QString("Ignoring Buffer: %1").arg( (unsigned long)*bufferIterator ));
//                qDebug() << "Qsize: " << bufferQueue.size()  << " clear bufferQueue: emit signal buffer  " << (unsigned long)*bufferIterator << " released?";
                emit returnBufferReady(reinterpret_cast<unsigned char*>(*bufferIterator), *framesIterator);
                framesIterator++;
            }
            // Discard frames, unlock mutex
            framesQueue.clear();
            framesMutex.unlock();
        }
        else
            hexitechSignalError("HxtProcessing::clearAllQueues() Framess Error: Unable to acquire frames mutex lock!");

        // Discard buffer(s), unlock mutex
        bufferQueue.clear();
        bufferMutex.unlock();
    }
    else
        hexitechSignalError("HxtProcessing::clearAllQueues() Buffers Error: Unable to acquire buffer mutex lock!");

    // Clear fileQueue
    if (fileMutex.tryLock(mutexTimeout))
    {
        // Report filenames before discarding them
        QQueue<string>::iterator fileIterator = fileQueue.begin();
        for ( ; fileIterator != fileQueue.end(); fileIterator++)
            emit hexitechSignalError(QString("Ignoring File: %1").arg(string(*fileIterator).c_str()));
        // Discard file(s), unlock mutex
        fileQueue.clear();
        fileMutex.unlock();
    }
    else
        hexitechSignalError("HxtProcessing::clearAllQueues() Files Error: Unable to acquire file mutex lock!");

    // Clear motorQueue
    if (motorMutex.tryLock(mutexTimeout))
    {
        motorQueue.clear();
        motorMutex.unlock();
    }
    else
        hexitechSignalError("HxtProcessing::clearAllQueues() Motor Pos'n Error: Unable to acquire motor mutex lock!");

    //  User pressed Clear Unprocessed button? Reset bool value, Signal raw files discarded
    if (bRemoveUnprocessedFiles)
    {
        bRemoveUnprocessedFiles = false;
        emit hexitechUnprocessedFiles(false);
    }

}

bool HxtProcessing::prepareWholeQueueProcessing(bool bProcessTheQueue)
{
    int queueOffset = 0;
    if (bProcessTheQueue)
        queueOffset = 1;    /// How many items left on the Queue? (Either leave the last/grab all)

    int buffersDequeuedCount = 0, numberOfBuffersToProcess = 0;
    unsigned short* newBuffer;
    unsigned long newFrameSize;

    // Process the queue
    if (bufferMutex.tryLock(mutexTimeout))
    {
        if (framesMutex.tryLock(mutexTimeout))
        {
            numberOfBuffersToProcess = bufferQueue.size();

            if (motorMutex.tryLock(mutexTimeout))
            {
                while (buffersDequeuedCount < (numberOfBuffersToProcess - queueOffset))
                {
                    newBuffer = bufferQueue.dequeue();
                    mBufferNames.push_back(newBuffer);
                    newFrameSize = framesQueue.dequeue();
                    mValidFrames.push_back(newFrameSize);
                    mPositions = motorQueue.dequeue();
                    //emit hexitechSignalError(QString("Processing buffer: %1 (%2)").arg((unsigned long)*newBuffer, newFrameSize));
                    buffersDequeuedCount++;
                }
                motorMutex.unlock();
            }
            else
            {
                hexitechSignalError("HxtProcessing::prepareWholeQueueProcessing() Error: Unable to acquire motor mutex lock!");
                return false;
            }
            framesMutex.unlock();
        }
        else
        {
            hexitechSignalError("HxtProcessing::prepareWholeQueueProcessing() Error: Unable to acquire frames mutex lock!");
        }
        bufferMutex.unlock();
    }
    else
    {
        hexitechSignalError("HxtProcessing::prepareWholeQueueProcessing() Error: Unable to acquire buffer mutex lock!");
        return false;
    }

    // Process buffer(s)
    bool bProcessFiles = false;  // Process this buffer
    executeProcessing(bProcessFiles);    /// Process (all) Buffer(s)  [WholeQ?]

    // Signal that no unprocessed buffers remains in bufferQueue
    emit hexitechUnprocessedFiles(false);

    return true;
}

void HxtProcessing::checkQueueLengths(int & motorSize, int &bufferSize, int &fileSize, int &framesSize)
{
    if (motorMutex.tryLock(mutexTimeout))
    {
        motorSize = motorQueue.size();
        motorMutex.unlock();
    }
    if (bufferMutex.tryLock(mutexTimeout))
    {
        bufferSize = bufferQueue.size();
        bufferMutex.unlock();
    }
    if (framesMutex.tryLock(mutexTimeout))
    {
        framesSize = framesQueue.size();
        framesMutex.unlock();
    }
    if (fileMutex.tryLock(mutexTimeout))
    {
        fileSize = fileQueue.size();
        fileMutex.unlock();
    }

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

//    qDebug() << "HxtProcessing files to manually process:\n----------------------------------------------";
    // Display files selected to be processed
    if (!rawFilesToProcess.empty())
    {
        QStringList::iterator listIterator;
        for (listIterator = rawFilesToProcess.begin(); listIterator != rawFilesToProcess.end(); listIterator++)
        {
//            qDebug() << "** Adding file: " << (*listIterator).toStdString().c_str();

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
//        qDebug() << "DEBUG: mMomentumFile specified.";
    }
    else
    {
//        qDebug() << "DEBUG: mMomentumFile NOT specified.";
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
    /// Prepare Format Version 3 header entries
    /// (Only file prefix set up here the space)

    string filePrefix  = string("");

    // Capture entire prefix (e.g. discard .dat from "Hexitec_140317_113831_reduced_0000.dat")
    QFileInfo fileInfo(fileName.c_str());
    QString fileCompleteBaseName = fileInfo.completeBaseName();
    filePrefix = fileCompleteBaseName.toStdString();

    if (bPrefixEnabled)
        mFilePrefix = filePrefix;
    else
        mFilePrefix = "(blank)             ";

    /// Debugging purposes:
//    qDebug() << "*1* configHeaderEntries() fileName: " << fileName.c_str ();
//    qDebug() << "*2* configHeaderEntries() bPrefixEnabled: " << bPrefixEnabled << " bTimeStampEnabled: " << bTimeStampEnabled;
//    qDebug() << "*3* configHeaderEntries() filePrefix: " << filePrefix.c_str() << "(" << filePrefix.length()  << ").";
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
            return;
        }
    }
    // First timestamp is the path:
    size_t pathTimestamp = fileName.find(dateString);
    // Second timestamp is found by looking beyond the first timestamp:
    size_t fileTimestamp = fileName.find(dateString, pathTimestamp+1);

    // Obtain path and file prefix but without the "_" character at the end (hence -1)
    string pathAndPrefix = fileName.substr(0, fileTimestamp-1);
    //qDebug() << "pathAndPrefix: " << pathAndPrefix.c_str() << "\nfileName: " << fileName.c_str();

    // Construct hexitech filename, using file path and prefix inserting pre-file extension characters and timestamp before file's extension:
    mOutputFileNameDecodedFrame = pathAndPrefix + "_" + string(dateString) + ".hxt";

    qDebug() << "::obtainRaw..() Before: " << beforeString.c_str() << " after: " << mOutputFileNameDecodedFrame.c_str();
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

void HxtProcessing::handleReturnHxtBuffer(unsigned short* buffer)
{
    //qDebug() << "HxtProcessing Returning buffer from the Visualisation tab, address: " << (void*)buffer;
    mHxtBuffers.push_back((HxtBuffer*)buffer);
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

void HxtProcessing::handleHxtProcessingPrepSettings()
{
    prepSettings();
}

void HxtProcessing::customFileSelected(bool bCustom)
{
    bCustomFileNameSelected = bCustom;
}

void HxtProcessing::toggleProcessingLogging(bool bBool)
{
    bProcessingLoggingDisabled = !bBool;
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
    //qDebug() << "HxtProcessing::dataCollectionFinish() executing..";
    /// Data collection finished, process queue contents
    if (qContentsMutex.tryLock(mutexTimeout))
    {
        bProcessQueueContents = true;
        qContentsMutex.unlock();
        //qDebug() << "HxtProcessing::dataCollectionFinish() Time to note this is the last data belonging to current HXT file..";
        bLastDataOfCurrentHxtFile = true;
    }
    else
    {
        emit hexitechSignalError("HxtProcessing dataCollectionFinished() error: Unable to acquire queue context mutex lock!");
    }

}

int HxtProcessing::executeProcessing(bool bProcessFiles)
{
    string fileName;
    /// Execute processing of file(s) / buffer(s), depending upon bProcessFiles selection (true=file(s), otherwise buffer(s))

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
//        qDebug() <<"PROCESSING FILE: " << QString::fromStdString(fileName);
        configHeaderEntries(fileName);

        // Use raw file path and prefix if user made corresponding selection
        if (bUseRawFileEnabled)
            obtainRawFilePathAndPrefix(fileName);
    }

    // A new hexitech file will be produced before this function is finished
    //  Signal to mainwindow to remove any excess image Slice(s) in preparation
    emit hexitechRemoveAnyExcessSlices();

    /// Signal which file(s)/buffer(s) about to be processed
    if (bProcessFiles)
        emit hexitechConsumedFiles(mRawFileNames);
    else
        emit hexitechConsumedBuffers( mBufferNames);

    /// Parse the specified raw file(s)/buffer(s)
    if (bProcessFiles)
    {
//        qDebug() << "HxtProcessing::executeProcessing() - Time to parse raw file(s) ";
        dataProcessor->parseFile(mRawFileNames);
        // Clear filenames
        mRawFileNames.clear();
        updateVisualisationTabAndHxtFile();
    }
    else
    {
        if (!mBufferNames.empty())
        {
            dataProcessor->parseBuffer(mBufferNames, mValidFrames);
            // Release all the buffers
            ///  TODO:  Implement Callback & Move into HxtRawDataProcessor - No longer necessary (?)
            vector<unsigned short*>::iterator bufferIterator;
            vector<unsigned long>::iterator   frameIterator;
            frameIterator = mValidFrames.begin();
            for (bufferIterator = mBufferNames.begin(); bufferIterator != mBufferNames.end(); bufferIterator++)
            {
                emit returnBufferReady(reinterpret_cast<unsigned char*>(*bufferIterator), *frameIterator);
                //emit returnBufferReady(transferBuffer, validFrames);
//                qDebug() << "HxtProcessing: 1685 releasing  buffer: " << (unsigned long*)(*bufferIterator) << " with: " << *frameIterator;
                frameIterator++;
            }
            /// Clear both buffer and frames vectors
            mBufferNames.clear();
            mValidFrames.clear();
        }

        mDiscWritingTimer->stop();
        mTimeSinceLastDiscOp += mDiscWritingTimer->elapsed();
//        qDebug() << "  ~~~~~  HxtProcessing::executeProcessing() mTimeSinceLastDiscOp: " << mTimeSinceLastDiscOp << "s (Just added: " << mDiscWritingTimer->elapsed() <<
//                    "s) [mDiscWritingInterval: " << mDiscWritingInterval << "s] ~~~~~";

        // Time to write files/send file or a buffer to visualisation thread?
        if ( mTimeSinceLastDiscOp > mDiscWritingInterval)
        {
            mTimeSinceLastDiscOp = 0.0;
            /// Call function to write to disc and update Visualise tab
            updateVisualisationTabAndHxtFile();
            LOG(gLogConfig, logNOTICE) << "Process data pushed to Visualisation Tab, HXT file";
        }
    }

    return 0;
}

int HxtProcessing::updateVisualisationTabAndHxtFile()
{
    //qDebug() << "HxtProcessing::updateVisualise...File() ! We will be writing the hexitech filename called: " << mOutputFileNameDecodedFrame.c_str();

    // Update with (possibly changed) values for  prefix, Motor position, timestamp
    commitConfigChanges();

    // Flush last frames through processor to output
    dataProcessor->flushFrames();       /// Independent of file/buffer selection

    // Interpolate pixel histograms if enabled
    if (mEnableIpCorrector) dataProcessor->InterpolateDeadPixels(mInterpolationThreshold);

    /// Checkat least 1 buffer available before proceeding
    if (mHxtBuffers.empty())
    {
        emit hexitechSignalError("HxtProcessing Error: Pool of Buffers! (Visualisation thread too slow?)");
//        qDebug() << "HxtProcessing Error: Pool of Buffers! (Visualisation thread too slow?)";
        return -1;
    }
    // Copy "output files" into buffer
    HxtBuffer* hxtBuffer = 0;

    hxtBuffer = *mHxtBuffers.begin();
    //qDebug() << " * " << (void*)(hxtBuffer)  << " <- HxtProcessing.cpp:1607 (hxtBuffer)";
    dataProcessor->copyPixelOutput((unsigned short*)hxtBuffer);


    // Signal that processed data [inside RAM] is ready to be displayed in the GUI
    // DSoFt: added filename to indicate when a new image/slice begins as this will change.
    // This is a quick fix and should be reviewed.

    // Write output files
    dataProcessor->writeHxtBufferToFile(mOutputFileNameDecodedFrame);

    // Signal produced Hxt filename
    emit hexitechProducedFile(mOutputFileNameDecodedFrame);
    ///  Only now, we can safely emit:
//        emit hexitechBufferToDisplay( (HxtBuffer*)hxtBuffer, QString::fromStdString(fileName));
    emit hexitechBufferToDisplay( (unsigned short*)hxtBuffer, QString::fromStdString(mOutputFileNameDecodedFrame));

    /// Determine whether file name changed; If yes, signal that
    //qDebug() << "mPreviousOutputFileName: " << mPreviousOutputFileName.c_str() << " mOutputFileNameDecodedFrame: " << mOutputFileNameDecodedFrame.c_str();
    if (mPreviousOutputFileName == "")
    {
        //qDebug() << "Previous file name is blank - Meaning no real file name change to be signalled";
        mPreviousOutputFileName = mOutputFileNameDecodedFrame;
    }
    else
    {
        if (mPreviousOutputFileName != mOutputFileNameDecodedFrame)
        {
            //qDebug() << "previous file name differs from current;  signal file name changed & update previous file name";
            emit hxtProcessedFileNameChanged(QString::fromStdString(mPreviousOutputFileName));
            mPreviousOutputFileName = mOutputFileNameDecodedFrame;
        }
        else
            ;//qDebug() << "previous file name defined but no file name change occurred";
    }

    if (bLastDataOfCurrentHxtFile)
    {
        //qDebug() << " *** This is the last data belonging to the current HXT file !";
        emit hxtProcessedFileNameChanged(QString::fromStdString(mOutputFileNameDecodedFrame));
        mPreviousOutputFileName = "";
        bLastDataOfCurrentHxtFile = false;
    }
    //qDebug() << "Erasing element 0 from mHxtBuffer vector, address: " << (void*)hxtBuffer;
    mHxtBuffers.erase(mHxtBuffers.begin());

    return 0;
}

void HxtProcessing::commitConfigChanges()
{
    /// Communicate motor position, file prefix and timestamp according to user selection(s)
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

    dataProcessor->updateFilePrefix(mFilePrefix);   // ConfigHeaderEntries() sets mFilePrefix; So update regardless if bFilePrefix = true/false
    if (bTimeStampEnabled)
        mDataTimeStamp = createDataTimeStampString();
    // saveTimeStamp() clear mDataTimeStamp if unselected by user
    dataProcessor->updateTimeStamp(mDataTimeStamp);
}

float hexitech::HxtProcessing::getDiscWritingInterval()
{
   return mDiscWritingInterval;
}

string HxtProcessing::createDataTimeStampString()
{
    // Obtain date stamp, save logging to the same folder as processed file
    DateStamp* now = new DateStamp();
    string dString = now->GetDateStamp();
    delete(now);

    // Date format: "2016-02-10_10-36-27", Remove -'s characters
    regex removeUnderscoreRx("[-]");
    string emptyString = "";
    dString = regex_replace(dString, removeUnderscoreRx, emptyString);

    // Reduce year from YYYY -> YY; e.g. 2016 -> 16
    string dateTimeString = dString.substr(2,dString.size());
//    qDebug() << " ! dateTimeString = " << dateTimeString.c_str();
    return dateTimeString;
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

