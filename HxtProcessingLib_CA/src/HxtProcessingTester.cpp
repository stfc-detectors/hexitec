/*
 * appMain.cpp - main application code for Hexitech raw data processing
 *
 *  Created on: 10 Dec 2010
 *      Author: Tim Nicholls
 */

#include "HxtProcessingTester.h"
#include <QDebug>

const unsigned int kHxtSensorCols = 80;
const unsigned int kHxtSensorRows = 80;

using namespace std;

namespace hexitech {

/// Main entry point for application - parses arguments, creates raw data processor and corrector objects,
/// invokes processor on specified files and writes output data files
HxtProcessing::HxtProcessing(string aAppName, unsigned int aDebugLevel) :
    mDebugLevel(aDebugLevel)
{
    // Default values
    mHistoStartVal = 0;
    mHistoEndVal   = 10000;
    mHistoBins     = 1000;
    mInterpolationThreshold = 0;
    mInducedNoiseThreshold = 0.0;
    mGlobalThreshold     = -1.0;

    // Make these user changeable
    mOutputFileNameDecodedFrame = "pixelHisto.hx3";

    mEnableInCorrector = false;
    mEnableCabCorrector = false;
    mEnableMomCorrector = false;
    mEnableCsaspCorrector = false;
    mEnableCsdCorrector = true;
    mEnableIdCorrector = true;
    mEnableIpCorrector = false;
    mEnableDbPxlsCorrector = false;

    mWriteCsvFiles = false;
    mEnableVector = false;


    // Create new log file name
    DateStamp* now = new DateStamp();
    logFileStream << "hexitech_log_";
    logFileStream << now->GetDateStamp();
    logFileStream << ".txt";
    delete(now);

    // Create new log configuration
    gLogConfig = new LogConfig( aAppName);
    bool bLoggingDisabled = false;//true;
    if (bLoggingDisabled)
    {
        gLogConfig->setLogStdout(false/*true*/);
    //    gLogConfig->setLogFile(true, logFileStream.str());
        cout << "Logging o neither file nor standard out!\n";
    }
    else
    {
        gLogConfig->setLogStdout(true);
        gLogConfig->setLogFile(true, logFileStream.str());
    }
    gLogConfig->setDebugLevel(mDebugLevel);

    mFormatVersion = 3;

    // Initialise file format 2 related variables
    mX             = -65535;
    mY             = -65535;
    mZ             = -65535;
    mRot           = -65535;
    mTimer         = -65535;
    mGalx          = -65535;
    mGaly          = -65535;
    mGalz          = -65535;
    mGalRot        = -65535;
    mFilePrefix    = "prefix_";
    mDataTimeStamp = string("000000_000000");

    /// HexitecGigE Addition:
    mEnableCallback = false;

    /// --------    Moving stuff from executeProcessing that need not be repeated.. ------- ///

    pixelThreshold = new HxtPixelThreshold(kHxtSensorRows, kHxtSensorCols);
    // Create new raw data processor instance
    dataProcessor = new HxtRawDataProcessor(kHxtSensorRows, kHxtSensorCols, mHistoStartVal, mHistoEndVal, mHistoBins,
                                                                 mFormatVersion, mX, mY, mZ, mRot,
                                                                 mTimer, mGalx, mGaly, mGalz, mGalRot,
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


}

HxtProcessing::~HxtProcessing()
{
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
}

void HxtProcessing::pushRawFileName(string aFileName)
{
    mRawFileNames.push_back(aFileName);
}

void HxtProcessing::pushBufferNameAndFrames(unsigned short *aBufferName, unsigned long aValidFrame)
{
    mBufferNames.push_back(aBufferName);
    mValidFrames.push_back(aValidFrame);
}

void HxtProcessing::prepSettings()
{
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
        LOG(gLogConfig, logINFO) << "Selected to write csv files";

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
   if (mGlobalThreshold != -1.0) {
       pixelThreshold->setGlobalThreshold(mGlobalThreshold);
   }
   if (!mThresholdFileName.empty()) {
       pixelThreshold->loadThresholds(mThresholdFileName);
   }

   // Test setting up vector implementation
   dataProcessor->setVector(mEnableVector);

   // Set debug flags if debug level non-zero
   if (mDebugLevel) dataProcessor->setDebug(true);

   // Apply thresholds to raw data processor
   dataProcessor->applyPixelThresholds(pixelThreshold);

   // Create and register frame correctors with raw data processor if enabled

   // 1. Induced Noise Corrector

   // Register it, enable debug if needed
   if (mDebugLevel) inCorrector->setDebug(true);
   if (mEnableInCorrector) {
       dataProcessor->registerCorrector(dynamic_cast<HxtFrameCorrector*>(inCorrector));
       LOG(gLogConfig, logINFO) << "Applying " << inCorrector->getName() << " corrector to data";
   }

   // 2. Calibration

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

   // Register it, enable debug if needed
   if (mDebugLevel) subCorrector->setDebug(true);
   if (mEnableCsaspCorrector) {
       dataProcessor->registerCorrector(dynamic_cast<HxtFrameCorrector*>(subCorrector));
       LOG(gLogConfig, logINFO) << "Applying " << subCorrector->getName() << " corrector to data";
   }

   // 3.2 CS Addition

   if (mDebugLevel) csdCorrector->setDebug(true);
   if (mEnableCsdCorrector) {
       dataProcessor->registerCorrector(dynamic_cast<HxtFrameCorrector*>(csdCorrector));
       LOG(gLogConfig, logINFO) << "Applying " << csdCorrector->getName() << " corrector to data";
   }

   // 4. Incomplete Data

   if (mDebugLevel) idCorrector->setDebug(true);
   if (mEnableIdCorrector) {
       dataProcessor->registerCorrector(dynamic_cast<HxtFrameCorrector*>(idCorrector));
       LOG(gLogConfig, logINFO) << "Applying " << idCorrector->getName() << " corrector to data";
   }

   // 5. Momentum

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

   if (mEnableDbPxlsCorrector)  {
       if (mDebugLevel) dbpxlCorrector->setDebug(true);
       dataProcessor->registerCorrector(dynamic_cast<HxtFrameCorrector*>(dbpxlCorrector));
       LOG(gLogConfig, logINFO) << "Applying " << dbpxlCorrector->getName() << " corrector to data";
   }

}

int HxtProcessing::executeProcessing(bool bProcessFiles, bool bWriteFiles)
{
    /// Move away from this function:
    gLogConfig->setDebugLevel(mDebugLevel); // Less important..

    /* Check for illegal configuration settings */  /// Move or stay?

    // Cannot run Charge Sharing Addition and Charge Sharing Discriminator together
    if (mEnableCsaspCorrector && mEnableCsdCorrector)
    {
        LOG(gLogConfig, logERROR) << "Cannot combine Charge Sharing Addition and Charge Sharing Discriminator";
        return 1;
    }

    // If Calibration (Energy) enabled, check specified files exists
    if (mEnableCabCorrector)
    {
        // Is of the calibration files not specified?
        if (mGradientsFile.empty() || mInterceptsFile.empty())
        {
            LOG(gLogConfig, logERROR) << "Calibration requires both Gradients and Intercepts files" << endl;
            return 2;
        }
     }

    // If Momentum enabled, check specified file exists
    if (mEnableMomCorrector)
    {
        if (mMomentumFile.empty())
        {
            LOG(gLogConfig, logERROR) << "Momentum requires Momentum file to be specified" << endl;
            return 4;
        }
    }

    // Cannot specify both global threshold and threshold file
    if ( (mGlobalThreshold != -1.0) && (!mThresholdFileName.empty()))
    {
        LOG(gLogConfig, logERROR) << "Cannot specify both a global threshold and a threshold file" << endl;
        return 3;
    }

    /// If files to be processed, i.e. bProcessFiles = true (files) [false (buffers)]
    ///  then no need for callback:
    mEnableCallback = (!bProcessFiles);

    /// Parse the specified raw file(s)/buffer(s)
    if (bProcessFiles)
    {
        dataProcessor->parseFile(mRawFileNames);
        mRawFileNames.clear();
    }
    else
    {
        dataProcessor->parseBuffer(mBufferNames, mValidFrames);
        /// Clear/release buffer(s) parsed above - No?

    }

    /// Testing dataProcessor->copyPixelOutput() ...

    vector<HxtBuffer*> mHxtBuffers;    /// Pool of buffers;signalled to Visualisation tab to be displayed
    int numHxtBuffers = 10;
    // Setup pool of buffers - To send HXT file contents by RAM rather than file
    for (int i=0; i < numHxtBuffers; i++)
    {
        HxtBuffer* hxtBuffer = new HxtBuffer;
        //hxtBuffer.allData = (double*) malloc (hxtBuffer->nBins * sizeof(double)); // Borrowed from slice.cpp
        mHxtBuffers.push_back(hxtBuffer);
    }

    // Copy "output files" into buffer
    HxtBuffer* hxtBuffer = 0;

    hxtBuffer = *mHxtBuffers.begin();// *buffersIterator;
    //qDebug() << " * " << (void*)(hxtBuffer) << " <- HxtProcessingTester.cpp:396 (hxtBuffer)";
    dataProcessor->copyPixelOutput((unsigned short*)hxtBuffer);

    if (bWriteFiles)
    {
        // Flush last frames through processor to output
        dataProcessor->flushFrames();

        // Interpolate pixel histograms if enabled
        if (mEnableIpCorrector) dataProcessor->InterpolateDeadPixels(mInterpolationThreshold);

        // Write output files - use the hexitech buffer structure instead:
        dataProcessor->writeHxtBufferToFile( string("_New_") + mOutputFileNameDecodedFrame);  // cout << endl << "\t New Implementation.. Write just 0's???\n";
        //dataProcessor->writeStructOutput( string("_Old_") + mOutputFileNameDecodedFrame);  cout << endl << "\t Old Implementation.. Write pxl values\n";

        // Write CSV diagnostic histograms if selected
        if (mWriteCsvFiles)
        {
            dataProcessor->writeCsvFiles();
            string rawSpectrumFile = dataProcessor->getRawCsvFileName();
            string corSpectrumFile = dataProcessor->getCorCsvFileName();
            //cout << "raw spectrum:" << rawSpectrumFile.c_str() << endl;
            //cout << "cor spectrum:" << corSpectrumFile.c_str() << endl;
        }
    }
    LOG(gLogConfig, logNOTICE) << "Finished";

    return 0;
}

void HxtProcessing::commitConfigChanges() {

    dataProcessor->updateFormatVersion(mFormatVersion);
//    dataProcessor->updateMotorPositions(mX, mY, mZ, mRot, mTimer, mGalx, mGaly, mGalz, mGalRot);
    dataProcessor->updateFilePrefix(mFilePrefix);
    dataProcessor->updateTimeStamp(mDataTimeStamp);
}

/// printUsage - print usage string
/// @arg aAppName string of application name
void HxtProcessing::printUsage(string aAppName) {

    cout << endl
         << aAppName
         << " is a standalone application to parse Hexitech raw data files, apply" << endl
         << "corrections to decoded frames, fill and output per-pixel histograms"  << endl
         << "in a convenient format." << endl
         << endl
         << "Command syntax: " << aAppName << " {option option ...} <list of files>" << endl
         << endl
         << "where option is one of the following:" << endl
         << endl
         << "   --help|-h                        Print this help" << endl
         << "   --debug|-d <value>               Set debug output level" << endl
         << "   --start|-s <value>               Start value (lower limit) of output histograms" << endl
         << "   --end|-e <value>                 End value (upper limit) of output histograms" << endl
         << "   --nbins|-n <value>               Number of bins in output histograms" << endl
         << "   --gthreshold|-gt <value>         Global threshold to apply to all pixels" << endl
         << "   --thresholdfile|-tf <file>       Pixel threshold file to load" << endl
         << "   --incor|-in <fraction>           Enable and set induced noise corrector threshold (fraction) level" << endl
         << "   --gradfile|-gf <file>            Gradients file to load for calibration" << endl
         << "   --interfile|-if <file>           Intercepts file to load for calibration" << endl
         << "   --cacor|-ca                      Enable charge sharing addition corrector" << endl
         << "   --nocsdcor|-nc                   Disable charge sharing discrimination corrector" << endl
         << "   --noidcor|-ni                    Disable incomplete data corrector" << endl
         << "   --ipcor|-ip <threshold>          Enable and set interpolate corrector threshold level" << endl
         << "   --outputpixelfile|-opf <name>    Output pixel file name (defaults to " << mOutputFileNameDecodedFrame << ")" << endl

         << "   --csvfiles|-c                    Enable output of diagnostic histogram CSV files" << endl
         << "   --vector|-v                      Enable vector indexing (boosts ID, CSD and IN performance)" << endl
         << endl
         << "and <list of files> is a space delimited list of files (including paths) to process."
         << endl;
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

}
