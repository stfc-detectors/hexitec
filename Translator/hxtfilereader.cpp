#include "hxtfilereader.h"

HxtFileReader::HxtFileReader(): FileReader()
{
}

void *HxtFileReader::read(string fileName)
{
   unsigned int bufferSize;

   inFile.open(fileName, std::ifstream::binary);

   bufferSize = sizeof(hxtBuffer) - sizeof(double *);
   inFile.read((char *)&hxtBuffer, bufferSize);

   bufferSize = ((hxtBuffer.nBins * hxtBuffer.nRows * hxtBuffer.nCols) + hxtBuffer.nBins) * sizeof(double);
   hxtBuffer.allData = (double*) malloc (bufferSize);
   inFile.read((char *)hxtBuffer.allData, bufferSize);

   inFile.close();
   return (void *)&hxtBuffer;
}
