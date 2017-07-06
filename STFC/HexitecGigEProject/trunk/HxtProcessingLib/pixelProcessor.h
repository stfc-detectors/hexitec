#ifndef PIXELPROCESSOR_H
#define PIXELPROCESSOR_H

#include <QObject>
#include <cstdint>
#include <string>

using namespace std;

class PixelProcessor : public QObject
{
   Q_OBJECT

public:
   PixelProcessor();
   void initialiseEnergyCalibration(double *gradientValue, double *interceptValue);
   void setEnergyCalibration(bool energyCalibration);
   void setGradientValue(double *gradientValue);
   void setInterceptValue(double *gradientValue);
   double *getGradientValue();
   double *getInterceptValue();
   uint16_t *processFrame(uint16_t *frame);
   uint16_t *processFrame(uint16_t *frame, uint16_t thresholdValue);
   uint16_t *processFrame(uint16_t *frame, uint16_t *thresholdPerPixel);
   uint16_t *processRe_orderFrame(uint16_t *frame);
   uint16_t *processRe_orderFrame(uint16_t *frame, uint16_t thresholdValue);
   uint16_t *processRe_orderFrame(uint16_t *frame, uint16_t *thresholdPerPixel);


private:
   void initialisePixelMap();
   bool energyCalibration;
   double *pixelEnergy;
   double *gradientValue;
   double *interceptValue;

signals:
   void enqueuePixelEnergy(double *pixelEnergy);

};

#endif // PIXELPROCESSOR_H
