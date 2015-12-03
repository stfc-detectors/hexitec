#ifndef HXTFRAME_H
#define HXTFRAME_H

#include <sarray.h>

class HXTFrame
{
public:
    HXTFrame();
    HXTFrame(int, int);

public:
    SArray <double> sliceEvent;

};

#endif // HXTFRAME_H
