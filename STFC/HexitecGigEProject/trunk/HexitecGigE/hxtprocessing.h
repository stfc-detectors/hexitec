#ifndef HXTPROCESSING_H
#define HXTPROCESSING_H

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

#include <QThread>
#include <QMutex>
#include <QQueue>
// QFileInfo needed by configHeaderEntries()
#include <QFileInfo>
// QHash needed by pushMotorPositions()
#include <QHash>

#include "inifile.h"
#include "dataacquisitionstatus.h"

using namespace std;

namespace hexitech {

class HxtProcessing : public QThread {

   Q_OBJECT

public:
    HxtProcessing(string aAppName, unsigned int aDebugLevel);
    ~HxtProcessing();
    void run();

    void pushRawFileName(string aFileName); // Add raw filename onto fileQueue queue
    void pushTransferBuffer(unsigned char *transferBuffer, unsigned long validFrames); // Add transfer buffer onto queue
    int executeProcessing();
    int checkConfigValid();

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
    void pushMotorPositions(QHash<QString, int> *qHashPositions);
    motorPositions* copyQHashToMotorPositions(QHash<QString, int> *qHashPositions);
    void setDataTimeStamp(string dataTimeStamp) { mDataTimeStamp = dataTimeStamp; }
    void setFilePrefix(string filePrefix) { mFilePrefix = filePrefix; }
    void dumpSettings();
    // Configure prefix, motorPositions, etc ..
    void configHeaderEntries(string fileName);
    void clearMotorPositions();
    // .. according to selections performed by functions listed underneath private slots

    // Provide access to set bManualProcessingEnabled:
    void setManualProcessing(bool bManualEnabled);

    // Access to selectedFilesToProcess member
    void setRawFilesToProcess(QStringList rawFilesList);
    void obtainRawFilePathAndPrefix(string fileName);

private:
    void dataCollectionFinished();

signals:
    void hexitechRunning(bool isRunning);
    void hexitechFilesToDisplay(QStringList filesList);
    void hexitechSpectrumFile(QString spectrumFile);
    void hexitechConsumedFiles(vector<string> fileNames);
    void hexitechProducedFile(string fileName);
    void hexitechSignalError(QString errorString);
    void hexitechSignalManualProcessingFinished();
    void hexitechRemoveAnyExcessSlices();
    void hexitechUnprocessedFiles(bool bFilesExists);
    void returnBufferReady(unsigned char *transferBuffer);

private slots:
    void savePrefix(bool bPrefixChecked);
    void saveMotor(bool bMotorChecked);
    void saveTimeStamp(bool bTimeStampChecked);
    void saveSameAsRawFile(bool bSameAsRawFileChecked);
    void changeProcessingCondition(processingCondition newCondition);
    void processingVetoed(bool bProcessingOverruled);
    void removeFiles(bool bRemoveFiles);
    // DataAcquisitionForm signalling (via MainWindow) that data collection finished
    void handleDataAcquisitionStatusChanged(DataAcquisitionStatus);

protected:
    // Go through fileQueue looking for any motor position changes
    bool checkQueueForMotorChanges(int currentCondition, int *numberOfFilesToProcess);
    // Check that fileQueue only contain files to be manually processed
    bool confirmOnlyManualFilesInQueue();

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

    bool bPrefixEnabled;
    bool bMotorEnabled;
    bool bTimeStampEnabled;
    bool bUseRawFileEnabled;

    bool bThreadRunning;
    bool bDontDisplayProcessedData;
    bool bManualProcessingEnabled;

    // Raw files selected by User to be manually processed
    QStringList rawFilesToProcess;

    // Variables supporting file format version 2
    u64 mFormatVersion;
    motorPositions mPositions;
    string mFilePrefix;
    string mDataTimeStamp;

    int targetCondition;            // The type of condition to trigger processing of file(s)
    bool bRemoveUnprocessedFiles;   // Remove unprocessed raw files if enabled
    bool bProcessQueueContents;     // Process remainder of Queues (if DataAcquisitionForm signals data collection completed)

    // Provide lock access mechanism to fileQueue, motorQueue, targetCondition, bProcessQueueContents
    QMutex fileMutex;
    QMutex motorMutex;
    QMutex processingMutex;
    QMutex qContentsMutex;
    int mutexTimeout;

    ostringstream logFileStream;

    // Queues for raw filename and motor positions
    QQueue<string> fileQueue;
    QQueue<motorPositions> motorQueue;
};


}

#endif // HXTPROCESSING_H
