/*
 * appMain.cpp - main application code for Hexitech raw data processing
 *
 *  Created on: 10 Dec 2010
 *      Author: Tim Nicholls
 */

#include "HxtProcessing.h"

// This makes QT a dependency for this project:
#include <QDebug>


const unsigned int kHxtSensorCols = 80;
const unsigned int kHxtSensorRows = 80;

using namespace std;

// Declaration of global log configuration used throughout - must be initialised
//LogConfig* gLogConfig;

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
    mOutputFileNameDecodedFrame = "pixelHisto.hxt";
    mOutputFileNameSubPixelFrame = "pixelSubResolutionHisto.hxt";

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
    gLogConfig->setLogStdout(true);
    gLogConfig->setLogFile(true, logFileStream.str());
    gLogConfig->setDebugLevel(mDebugLevel);

    mFormatVersion = 1;

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
    mFilePrefix    = "-1";
    mDataTimeStamp = string("000000_000000");

    mMomentumFile = string("");
}

HxtProcessing::~HxtProcessing()
{
    // Close log configuration and any associated files
    gLogConfig->close();
    delete gLogConfig;
}

void HxtProcessing::pushRawFileName(string aFileName)
{
    mRawFileNames.push_back(aFileName);
}

int HxtProcessing::executeProcessing()
{
    gLogConfig->setDebugLevel(mDebugLevel); // Less important..

    /* Check for illegal configuration settings */

    // Cannot run Charge Sharing Addition and Charge Sharing Discriminator together
    if (mEnableCsaspCorrector && mEnableCsdCorrector)
    {
        LOG(gLogConfig, logERROR) << "Cannot combine Charge Sharing Addition and Charge Sharing Discriminator";
        return 1;
    }

    // Are both calibration file specified?
    if ( (!mGradientsFile.empty()) && (!mInterceptsFile.empty()) )
    {
        // Both calibration files specified, enable calibration
        mEnableCabCorrector = true;
    }
    else
    {
        // Is one calibration file specified, but not the other?
        if ( (mGradientsFile.empty() && !mInterceptsFile.empty())
             || (!mGradientsFile.empty() && mInterceptsFile.empty()) )
        {
            LOG(gLogConfig, logERROR) << "Calibration requires both Gradients and Intercepts files" << endl;
            return 2;
        }
        else
            mEnableCabCorrector = false;
    }


    // Is momentum file specified?
    if (!mMomentumFile.empty())
        mEnableMomCorrector = true;
    else
        mEnableMomCorrector = false;

    // Cannot specify both global threshold and threshold file
    if ( (mGlobalThreshold != -1.0) && (!mThresholdFileName.empty()))
    {
        LOG(gLogConfig, logERROR) << "Cannot specify both a global threshold and a threshold file" << endl;
        return 3;
    }



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
    HxtPixelThreshold* pixelThreshold = new HxtPixelThreshold(kHxtSensorRows, kHxtSensorCols);
    if (mGlobalThreshold != -1.0) {
        pixelThreshold->setGlobalThreshold(mGlobalThreshold);
    }
    if (!mThresholdFileName.empty()) {
        pixelThreshold->loadThresholds(mThresholdFileName);
    }

    // Create new raw data processor instance
    HxtRawDataProcessor* dataProcessor = new HxtRawDataProcessor(kHxtSensorRows, kHxtSensorCols, mHistoStartVal, mHistoEndVal, mHistoBins,
                                                                 mFormatVersion, mX, mY, mZ, mRot,
                                                                 mTimer, mGalx, mGaly, mGalz, mGalRot,
                                                                 string("prefix"), mDataTimeStamp);

    // Test setting up vector implementation
    dataProcessor->setVector(mEnableVector);

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

    // Parse the specified raw files
    dataProcessor->parseFile(mRawFileNames);

    // Flush last frames through processor to output
    dataProcessor->flushFrames();

    // Interpolate pixel histograms if enabled
    if (mEnableIpCorrector) dataProcessor->InterpolateDeadPixels(mInterpolationThreshold);

    // Write output files
    dataProcessor->writePixelOutput(mOutputFileNameDecodedFrame);

    // Write subpixel files if subpixel corrector enabled
    if (mEnableCsaspCorrector)	dataProcessor->writeSubPixelOutput(mOutputFileNameSubPixelFrame);

    // Write CSV diagnostic histograms if selected
    if (mWriteCsvFiles)
    {
        dataProcessor->writeCsvFiles();
        string rawSpectrumFile = dataProcessor->getRawCsvFileName();
        string corSpectrumFile = dataProcessor->getCorCsvFileName();
        qDebug() << "raw spectrum:" << rawSpectrumFile.c_str();
        qDebug() << "cor spectrum:" << corSpectrumFile.c_str();

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

    LOG(gLogConfig, logNOTICE) << "Finished";

    return 0;
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
         << "   --outputsubpixelfile|-osf <name> Output subpixel file name (defaults to " << mOutputFileNameSubPixelFrame << ", if enabled)" << endl
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
