#ifndef H5FILEWRITER_H
#define H5FILEWRITER_H

#include <stdint.h>
#include <iostream>
#include <fstream>
#include <H5Cpp.h>
#include <H5public.h>
#include "filewriter.h"

using namespace std;
using namespace H5;

class H5FileWriter : public FileWriter
{
public:
    H5FileWriter();
    void write(void *buffer, string fileName);
private:
    std::ifstream inFile;
    struct HxtBuffer *hxtBuffer;
    H5File file;

    void setFileId(H5File file);
    void buildDataSet(H5File file, string dsName, PredType dsType, int rank, hsize_t dsDimensions[], void *dsValues);

};


#endif // FILEWRITER_H
