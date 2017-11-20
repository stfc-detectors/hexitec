/*
 Eigen component class for 2Easy code

 S D M Jacques 31st October 2012
*/
#include "eigencomponent.h"

EigenComponent::EigenComponent(QObject *parent) :
    QObject(parent)
{
    resize();
}

EigenComponent::EigenComponent(int rows, int cols, int spectralLength)
{
    gridSizeX = rows;
    gridSizeY = cols;
    spectrumDataLen = spectralLength;
    contentImage.resize(gridSizeX,gridSizeY);
    contentSpectra.resize(spectrumDataLen);
}

void EigenComponent::resize()
{
    gridSizeX = 0;
    gridSizeY = 0;
    spectrumDataLen = 0;
    contentImage.resize(0,0);
    contentSpectra.resize(0);
}
