#ifndef PROCESSINGFORM_H
#define PROCESSINGFORM_H

#include <QMainWindow>
#include <QWidget>
#include <QFileDialog>
#include <regex>
#include <string>
#include <cstdint>

namespace Ui {
class ProcessingForm;
}

using namespace std;

class ProcessingForm : public QWidget
{
   Q_OBJECT

public:
   explicit ProcessingForm(QWidget *parent = 0);
   ~ProcessingForm();
   QMainWindow *getMainWindow();

   void Qt2CppListHandler();
   
private:
   Ui::ProcessingForm *ui;
   QMainWindow *mainWindow;
   void processImage(const char *imageFilename, const char *filename);
   void readThresholdFile(char *thresholdFile);

   int thresholdOption;
   uint16_t thresholdPerPixel[6400];
   bool nextFrame;
   bool energyCalibration;
   bool hxtGeneration;
   int chargedSharingOption;

signals:
   /*
   void configureProcessing(bool re_order,
                            const char *gradientFilename,
                            const char *interceptFilename,
                            const char *processedFilename);
                            */
   void configureProcessing(bool re_order,
                            bool nextFrame,
                            int threshholdMode,
                            int thresholdValue,
                            const char *thresholdFile,
                            const char *gradientFilename,
                            const char *interceptFilename,
                            const char *processedFilename);
   void configureProcessing(bool energyCalibration,
                            long long binStart,
                            long long binEnd,
                            double binWidth,
                            bool totalSpectrum,
                            const char *gradientFilename,
                            const char *interceptFilename,
                            const char *processedFilename);
   void configureProcessing(int ChargedSharingMode,
                            int pixelGridOption,
                            const char *gradientFilename,
                            const char *interceptFilename,
                            const char *processedFilename);

   void imageStarted(const char *path, int nRows, int nCols);
   void transferBufferReady(char *transferBuffer, unsigned long validFrames);
   void imageComplete(long long totalFramesAcquired);

private slots:
   void initialise();
   void processClicked();
   void setThresholdOptions(int thresholdOption);
   void setThresholdParameters();
   void setThresholdFile();
   void setEnergyCalibration(bool energyCalibration);
   void setEnergyCalibrationParameters();
   void setHxtGeneration(bool hxtGeneration);
   void setStartSpinBoxLimit(int upperLimit);
   void setEndSpinBoxLimit(int lowerLimit);
   void NextFrameCorrectionOption(bool nextFrameCorrection);
   void setChargedSharingOptions(int chargedSharingOption);
   void setChargedSharingParameters();
};

#endif // PROCESSINGFORM_H
