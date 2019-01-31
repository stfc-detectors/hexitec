#include "pixelProcessor.h"

#include <iostream>
#include <fstream>
#include <intrin.h>
#include <bitset>
#include <vector>
#include <sys/stat.h>
///
#include <QDebug>
PixelProcessor::PixelProcessor(int occupancyThreshold) :
    GeneralPixelProcessor(occupancyThreshold)
{
}

