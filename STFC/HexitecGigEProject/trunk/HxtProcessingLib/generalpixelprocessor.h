#ifndef GENREALPIXELPROCESSOR_H
#define GENREALPIXELPROCESSOR_H

#include <QObject>
#include <cstdint>
#include <string>
#include <unordered_map>

using namespace std;

class GeneralPixelProcessor : public QObject
{
   Q_OBJECT

public:
   static uint16_t pixelMap[6400];
   static bool pixelMapInitialised;
   static uint16_t frameSize;

public:
   GeneralPixelProcessor();
   void initialiseEnergyCalibration(double *gradientValue, double *interceptValue);
   void setEnergyCalibration(bool energyCalibration);
   void setLastFrameCorrection(bool lastFrameCorrection);
   void setGradientValue(double *gradientValue);
   void setInterceptValue(double *gradientValue);
   double *getGradientValue();
   double *getInterceptValue();

   uint16_t *processFrame(uint16_t *frame);
   virtual uint16_t *processFrame(uint16_t *frame, uint16_t thresholdValue);
   virtual uint16_t *processFrame(uint16_t *frame, uint16_t *thresholdPerPixel);

   virtual uint16_t *processFrame(uint16_t *frame,
              unordered_map<int, double>**pixelEnergyMapPtr);
   virtual uint16_t *processFrame(uint16_t *frame, uint16_t thresholdValue,
				  unordered_map<int, double>**pixelEnergyMapPtr);
   virtual uint16_t *processFrame(uint16_t *frame, uint16_t *thresholdPerPixel,
				  unordered_map<int, double>**pixelEnergyMapPtr);

   uint16_t *processRe_orderFrame(uint16_t *frame);
   virtual uint16_t *processRe_orderFrame(uint16_t *frame, uint16_t thresholdValue);
   virtual uint16_t *processRe_orderFrame(uint16_t *frame, uint16_t *thresholdPerPixel);

   uint16_t *processRe_orderFrame(uint16_t *frame, 
				  unordered_map<int, double>**pixelEnergyMapPtr);
   virtual uint16_t *processRe_orderFrame(uint16_t *frame, uint16_t thresholdValue,
                                                  unordered_map<int, double>**pixelEnergyMapPtr);
   virtual uint16_t *processRe_orderFrame(uint16_t *frame, uint16_t *thresholdPerPixel,
              unordered_map<int, double>**pixelEnergyMapPtr);

protected:
   void initialisePixelMap();
   bool energyCalibration;
   bool lastFrameCorrection;
   bool chargedSharingCorrection;
   double *gradientValue;
   double *interceptValue;
   uint16_t *lastFrame;

signals:
   void enqueuePixelEnergy(double *pixelEnergy);

};

#endif // GENREALPIXELPROCESSOR_H
