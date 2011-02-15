#ifndef _readpstiffP_H
#define _readpstiffP_H

#include "readpstiff.h"

unsigned short create16BitPixel(tdata_t *);
unsigned short createEven12BitPixel(tdata_t *);
unsigned short createOdd12BitPixel(tdata_t *);
void printPixels(unsigned short *);
void read_pfbyte(char *, unsigned short *, int *, int *, int *);
void reorder(int, int, int, int, int, int *, int *,
             unsigned short *, unsigned short *);

#endif
