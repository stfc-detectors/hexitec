#include <stdlib.h>
#include <stdio.h>

#include "tiffio.h"
#include "readpstiffP.h"

void read_ps_tiff_(char *filnam, int *order_ref, unsigned short *img_full,
		   int *nxrasts_ref, int *nyrasts_ref,
		   int *minval, int *maxval, int *ierr)
{
  int i;
  int swap;
  int irec;
  int nrecs, nrasts;
  int nxrasts = *nxrasts_ref;
  int nyrasts = *nyrasts_ref;
  unsigned short *pixels;
  int ir;
  int order = *order_ref;

  /* Initialisations */

  *minval = 1000000;
  *maxval = 0;
  swap = 0;
  if (order < 0)
  {
    swap = 1;
    order = -order;
  }
  if (order < 1 || order > 8)
    order=1;

  nxrasts = *nxrasts_ref = getNx(filnam);
  nyrasts = *nyrasts_ref = getNy(filnam);

  nrecs = nxrasts;
  nrasts = nyrasts;
  if (order > 4)
  {
    nrecs = nyrasts;
    nrasts = nxrasts;
  }
  
  pixels = (unsigned short *)
    malloc(nxrasts * nyrasts * sizeof(unsigned short));
  read_pfbyte(filnam, pixels, nxrasts_ref, nyrasts_ref, ierr);

  i = 0;
  for (ir = 0;ir < nrecs;++ir)
  {
    reorder(order, swap, ir, nrasts, nrecs, minval, maxval, &pixels[i],
	    img_full);
    i += nrasts;
  }

  free(pixels);
  pixels = NULL;

  return;
}

void read_pfbyte(char *filnam,
		 unsigned short *pixels, int *nxrast, int *nyrast,
		 int *ierr)
{
  int i, dataPoint = 0, id, even;
  uint32 bytesRead;
  tdata_t buf, bufp, bufps;
  tstrip_t strip;
  uint16 bits;

  fflush(stderr);

  *ierr = 0;

  TIFFSetWarningHandler(NULL);
  TIFF* tif = TIFFOpen(filnam, "r");

  if (tif)
  {
    TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bits);

    buf = _TIFFmalloc(TIFFStripSize(tif));
    bytesRead = 0;
    for (strip = 0; strip < TIFFNumberOfStrips(tif); strip++)
    {
      bytesRead = TIFFReadEncodedStrip(tif, strip, buf, (tsize_t) -1);
      bufps = bufp = buf;

      for (i = 0; i < bytesRead;)
      {
	if (bits == 12)
	{
	  id = bufp - bufps;
	  if ((id % 3) == 0)
	  {
	    pixels[dataPoint++] = createEven12BitPixel(bufp);
	    bufp += 1; i += 1;
	  }
	  else
	  {
	    pixels[dataPoint++] = createOdd12BitPixel(bufp);
	    bufp += 2; i += 2;
	  }
	}
	else if (bits == 16)
        {
	  pixels[dataPoint++] = create16BitPixel(bufp);
	  bufp+=2; i += 2;
	}
      }
    }

    _TIFFfree(buf);
    TIFFClose(tif);

  }
  else
  {
    *ierr = 1;
  }

  return;
}

void reorder(int order, int swap, int ir, int nrasts, int nrecs,
	     int *minval, int *maxval,
	     unsigned short *buf, unsigned short *img_full)
{
  int j, k, l;
  int step;
  unsigned char *cbuf;
  unsigned char tempc;
 
  switch (order)
  {
    case 1:
      j = ir*nrasts;
      step = 1;
      break;
    case 2:
      j = (ir+1)*nrasts - 1;
      step = -1;
      break;
    case 3:
      j = (nrecs-ir-1)*nrasts;
      step = 1;
      break;
    case 4:
      j = (nrecs-ir)*nrasts - 1;
      step = -1;
      break;
    case 5:
      j = ir;
      step = nrecs;
      break;
    case 6:
      j = nrecs - 1 - ir;
      step = nrecs;
      break;
    case 7:
      j = (nrasts-1)*nrecs + ir;
      step = -nrecs;
      break;
    case 8:
      j = nrasts*nrecs - 1 - ir;
      step = -nrecs;
      break;
  }

  if (swap)
  {
    l=0;
    for (k = 0;k < nrasts;++k)
    {
      tempc = cbuf[l];
      cbuf[l] = cbuf[l+1];
      cbuf[l+1] = tempc;
      l += 2;
    }
  }

  for (k = 0;k < nrasts;++k)
  {
    img_full[j] = buf[k];
    if (buf[k] < *minval)
      *minval = buf[k];
    if (buf[k] > *maxval)
      *maxval = buf[k];
    j += step;
  }
}

unsigned short createEven12BitPixel(tdata_t *buf)
{
  unsigned short pixel = 0;

  pixel = (unsigned short)
    (((((char *)buf)[0]&0xff)<<4) | ((((char *)buf)[1]&0xf0)>>4));

  return pixel;
}

unsigned short createOdd12BitPixel(tdata_t *buf)
{
  unsigned short pixel = 0;

  pixel = (unsigned short)
    (((((char *)buf)[0]&0x0f)<<8) | (((char *)buf)[1]&0xff));

  return pixel;
}

unsigned short create16BitPixel(tdata_t *buf)
{
  unsigned short pixel = 0;

  pixel = (unsigned short)
    (((((char *)buf)[1]&0xff)<<8) | (((char *)buf)[0]&0xff));

  return pixel;
}

void printPixels(unsigned short *pixels)
{
  int i;

  printf("Pixels");
  for (i = 0; i < 5; i++)
  {
    printf(" %d", pixels[i]);
  }
  printf("\n");
}

int getNx(char *filnam)
{
  int nxrast = -1;

  TIFFSetWarningHandler(NULL);
  TIFF* tif = TIFFOpen(filnam, "r");
  TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &nxrast);
  TIFFClose(tif);

  return nxrast;
}

int getNy(char *filnam)
{
  int nyrast = -1;

  TIFFSetWarningHandler(NULL);
  TIFF* tif = TIFFOpen(filnam, "r");
  TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &nyrast);
  TIFFClose(tif);

  return nyrast;
}

uint16 getBits(char *filnam)
{
  uint16 bits = -1;

  TIFFSetWarningHandler(NULL);
  TIFF* tif = TIFFOpen(filnam, "r");
  TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bits);
  TIFFClose(tif);

  return bits;
}
