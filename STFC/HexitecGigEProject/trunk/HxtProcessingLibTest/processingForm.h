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
   uint16_t thresholdPerPix[6400];

signals:
   void configureProcessing(int threshholdMode,
                            int thresholdValue,
                            uint16_t *thresholdPerPixel,
                            const char *gradientFilename,
                            const char *interceptFilename,
                            const char *processedFilename);
   void imageStarted(const char *path, int frameSize);
   void transferBufferReady(char *transferBuffer, unsigned long validFrames);
   void imageComplete(unsigned long long totalFramesAcquired);

private slots:
   void processClicked();
   void initialise();
   void setThresholdOptions(int thresholdOption);
   void setThresholdParameters();
   void setThresholdFile();
};

#endif // PROCESSINGFORM_H
