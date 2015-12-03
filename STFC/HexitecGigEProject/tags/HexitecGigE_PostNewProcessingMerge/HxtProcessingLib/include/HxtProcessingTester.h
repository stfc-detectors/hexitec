#ifndef HXTPROCESSINGTESTER_H
#define HXTPROCESSINGTESTER_H

#include <iostream>

#include "HxtLogger.h"
#include "HxtRawDataProcessor.h"
#include "HxtFrameIncompleteDataCorrector.h"
#include "HxtFrameChargeSharingDiscCorrector.h"
#include "HxtFrameChargeSharingSubPixelCorrector.h"
#include "HxtFrameInducedNoiseCorrector.h"
#include "HxtFrameCalibrationCorrector.h"
#include "HxtFrameMomentumCorrector.h"
#include "HxtPixelThreshold.h"
#include "DateStamp.h"
#include "HxtFrameDoublePixelsCorrector.h"

using namespace std;

namespace hexitech {

class HxtProcessing {

public:
    HxtProcessing(string aAppName, unsigned int aDebugLevel);
    ~HxtProcessing();

    void pushRawFileName(string aFileName); // Add raw file name onto vector of files to be processed
    void pushBufferNameAndFrames(unsigned short *aBufferName, unsigned long aValidFrames); // Add transfer buffer onto queue

    int executeProcessing(bool bProcessFiles, bool bWriteFiles);

    /// HexitecGigE Addition:
    void setCallback(bool aCallback) { mEnableCallback = aCallback; }
    bool getCallback() { return mEnableCallback; }
    // Need a function to apply settings (just once), then the user  may call executeProcessing() to their hearts content..
    void prepSettings();

    /// Accessor functions - get functions redundant?
    unsigned int getDebugLevel() { return mDebugLevel; }
    unsigned int getHistoStartVal() { return mHistoStartVal; }
    unsigned int getHistoEndVal() { return mHistoEndVal; }
    unsigned int getHistoBins() { return mHistoBins; }
    unsigned int getInterpolationThreshold() { return mInterpolationThreshold; }

    void setDebugLevel(unsigned int debugLevel) { mDebugLevel = debugLevel; }
    void setHistoStartVal(unsigned int histoStartVal) { mHistoStartVal = histoStartVal; }
    void setHistoEndVal(unsigned int histoEndVal) { mHistoEndVal = histoEndVal; }
    void setHistoBins(unsigned int histBins) { mHistoBins = histBins; }
    void setInterpolationThreshold(unsigned int interpolationThreshold) { mInterpolationThreshold = interpolationThreshold; }

    void setInducedNoiseThreshold(double aInducedNoiseThreshold) { mInducedNoiseThreshold = aInducedNoiseThreshold; }
    void setGlobalThreshold(double aGlobalThreshold) { mGlobalThreshold = aGlobalThreshold; }

    void setThresholdFileName(string aThresholdFileName) { mThresholdFileName = aThresholdFileName; }
    void setOutputFileNameDecodedFrame(string aOutputFileNameDecodedFrame) { mOutputFileNameDecodedFrame = aOutputFileNameDecodedFrame; }
    void setOutputFileNameSubPixelFrame(string aOutputFileNameSubPixelFrame) { mOutputFileNameSubPixelFrame = aOutputFileNameSubPixelFrame; }
    string getOutputFileNameDecodedFrame() { return mOutputFileNameDecodedFrame; }
    string getOutputFileNameSubPixelFrame() { return mOutputFileNameSubPixelFrame; }
    void setGradientsFile(string aGradientsFile) { mGradientsFile = aGradientsFile; }
    void setInterceptsFile(string aInterceptsFile) { mInterceptsFile = aInterceptsFile; }
    void setMomentumFile(string aMomentumFile) { mMomentumFile = aMomentumFile; }

    void setEnableInCorrector(bool aEnableInCorrector) { mEnableInCorrector = aEnableInCorrector; }
    void setEnableCabCorrector(bool aEnableCabCorrector) { mEnableCabCorrector = aEnableCabCorrector; }
    void setEnableMomCorrector(bool aEnableMomCorrector) { mEnableMomCorrector = aEnableMomCorrector; }
    void setEnableCsaspCorrector(bool aEnableCsaspCorrector) { mEnableCsaspCorrector = aEnableCsaspCorrector; }
    void setEnableCsdCorrector(bool aEnableCsdCorrector) { mEnableCsdCorrector = aEnableCsdCorrector; }
    void setEnableIdCorrector(bool aEnableIdCorrector) { mEnableIdCorrector = aEnableIdCorrector; }
    void setEnableIpCorrector(bool aEnableIpCorrector) { mEnableIpCorrector = aEnableIpCorrector; }
    void setEnableDbPxlsCorrector(bool aEnableDbPxlsCorrector) { mEnableDbPxlsCorrector = aEnableDbPxlsCorrector; }

    void setWriteCsvFiles(bool aWriteCsvFiles) { mWriteCsvFiles = aWriteCsvFiles; }
    void setEnableVector(bool aEnableVector) { mEnableVector = aEnableVector; }

    // Function supporting file format version 2
    void setFormatVersion(u64 formatVersion) { mFormatVersion = formatVersion; }

    void setMotorPosition(double xPos, double yPos, double zPos, double rotPos, double aTimer, double xGal, double yGal, double zGal, double rotGal)
    {
        mX = xPos; mY = yPos; mZ = zPos;
        mRot = rotPos; mTimer = aTimer; mGalx = xGal;
        mGaly = yGal; mGalz = zGal; mGalRot = rotGal;
    }
    void setDataTimeStamp(string dataTimeStamp) { mDataTimeStamp = dataTimeStamp; }
    void setFilePrefix(string filePrefix) { mFilePrefix = filePrefix; }

    // Forward declarations
    void printUsage(string aAppName);

protected:
    /// HexitecGigE Addition:
    bool mEnableCallback;
    vector<unsigned short*> mBufferNames;
    vector<unsigned long>  mValidFrames;
    ///     ------  Moving stuff away from executeProcessing that need not be repeated ----- ///
    HxtPixelThreshold* pixelThreshold;
    HxtRawDataProcessor* dataProcessor;
    HxtFrameInducedNoiseCorrector* inCorrector;
    HxtPixelThreshold* gradientsContents;
    HxtPixelThreshold* interceptsContents;
    HxtFrameCalibrationCorrector* cabCorrector;
    HxtFrameChargeSharingSubPixelCorrector* subCorrector;
    HxtFrameChargeSharingDiscCorrector* csdCorrector;
    HxtFrameIncompleteDataCorrector* idCorrector;
    HxtPixelThreshold* momentumContents;
    HxtFrameMomentumCorrector* momCorrector;
    HxtFrameDoublePixelsCorrector* dbpxlCorrector;

    ///
    vector<string> mRawFileNames;
    unsigned int mDebugLevel;
    unsigned int mHistoStartVal;
    unsigned int mHistoEndVal;
    unsigned int mHistoBins;
    unsigned int mInterpolationThreshold;
    double mInducedNoiseThreshold;
    double mGlobalThreshold;
    string mThresholdFileName;
    string mOutputFileNameDecodedFrame;
    string mOutputFileNameSubPixelFrame;
    string mGradientsFile;
    string mInterceptsFile;
    string mMomentumFile;
    bool mEnableInCorrector;
    bool mEnableCabCorrector;
    bool mEnableMomCorrector;
    bool mEnableCsaspCorrector;
    bool mEnableCsdCorrector;
    bool mEnableIdCorrector;
    bool mEnableIpCorrector;
    bool mEnableDbPxlsCorrector;

    bool mWriteCsvFiles;
    bool mEnableVector;

    // Variables supporting file format version 2
    u64 mFormatVersion;
    double mX;
    double mY;
    double mZ;
    double mRot;
    double mTimer;
    double mGalx;
    double mGaly;
    double mGalz;
    double mGalRot;
    string mFilePrefix;
    string mDataTimeStamp;

    ostringstream logFileStream;
};


}

#endif // HXTPROCESSINGTESTER_H
