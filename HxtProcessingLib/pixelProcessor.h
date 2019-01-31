#ifndef PIXELPROCESSOR_H
#define PIXELPROCESSOR_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include "generalpixelProcessor.h"

using namespace std;

class PixelProcessor : public GeneralPixelProcessor
{

public:
   PixelProcessor(int occupancyThreshold);

   int getOccupancyCorrections() { return occupancyCorrections; }
};

#endif // PIXELPROCESSOR_H
