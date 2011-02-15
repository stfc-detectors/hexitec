#ifndef _readpstiff_h
#define _readpstiff_h

void read_ps_tiff(char *, int *, unsigned short *,
                  int *, int *, int *, int *, int *);
void printPixels(unsigned short *);
int getNx(char *);
int getNy(char *);
uint16 getBits(char *);

#endif
