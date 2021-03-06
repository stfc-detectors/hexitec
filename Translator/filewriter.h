#ifndef FILEWRITER_H
#define FILEWRITER_H

#include <stdint.h>
#include <iostream>
#include <fstream>
#include <H5Cpp.h>
#include <H5public.h>

using namespace std;
using namespace H5;

class FileWriter
{
public:
    FileWriter();
    virtual void write(void * buffer, string fileName);
protected:
    struct HxtBuffer
    {
        char hxtLabel[8];
        uint64_t hxtVersion;
        int motorPositions[9];
        int filePrefixLength;
        char filePrefix[100];
        char dataTimeStamp[16];
        uint32_t nRows;
        uint32_t nCols;
        uint32_t nBins;
        double *allData;
 //       double channel[MAX_BINS];
 //       double *spectrum;
 //       double spectrum[64000];
 //       double allData[6401000];
 /* This is calculated from the maximum possible bins, rows and columns as follows:
  * max bins = 1000, max rows = 80, max cols = 80 therefore:
  * max channel data = 1000
  * max spectrum = max bins * max rows * max cols
  *     double channel[1000];
  *     double spectrum[6400000];
  * where these arrays start and end will be determined on reading the
  * values from the buffer (proobably - tbc!!!)
  * */
    };

    struct HxtBuffer hxtBuffer;
    H5File file;

//    void setFileId(H5File file);
//    void buildDataSet(H5File file, string dsName, PredType dsType, int rank, hsize_t dsDimensions[], void *dsValues);

};


#endif // FILEWRITER_H
