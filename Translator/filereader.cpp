#include "filereader.h"

FileReader::FileReader()
{
}

void *FileReader::read(string fileName)
{
   return NULL;
}
/*
void FileReader::buildH5(string fileName)
{
   double *allDataPointer;
   allDataPointer = hxtBuffer.allData;

   H5File file(fileName, H5F_ACC_TRUNC);
   setFileId(file);

   hsize_t channelDimensions[1];               // dataset dimensions
   hsize_t spectrumDimensions[3];              // dataset dimensions
   hsize_t singleDimensions[1];

   channelDimensions[0] = hxtBuffer.nBins;
   spectrumDimensions[0] = hxtBuffer.nRows;
   spectrumDimensions[1] = hxtBuffer.nCols;
   spectrumDimensions[2] = hxtBuffer.nBins;

   singleDimensions[0] = 8;
   buildDataSet(file, "hxtLabel", PredType::NATIVE_CHAR, 1,
                singleDimensions, &hxtBuffer.hxtLabel);

   singleDimensions[0] = 9;
   buildDataSet(file, "motorPositions", PredType::NATIVE_INT, 1,
                singleDimensions, &hxtBuffer.motorPositions);
   singleDimensions[0] = 1;

   buildDataSet(file, "hxtVersion", PredType::NATIVE_INT, 1,
                singleDimensions, &hxtBuffer.hxtVersion);
   buildDataSet(file, "filePrefixLength", PredType::NATIVE_INT, 1,
                singleDimensions, &hxtBuffer.filePrefixLength);


   singleDimensions[0] = hxtBuffer.filePrefixLength;
   buildDataSet(file, "filePrefix", PredType::NATIVE_CHAR, 1,
                singleDimensions, &hxtBuffer.filePrefix);

   singleDimensions[0] = 16;
   buildDataSet(file, "dataTimeStamp", PredType::NATIVE_CHAR, 1,
                singleDimensions, &hxtBuffer.dataTimeStamp);

   singleDimensions[0] = 1;
   buildDataSet(file, "numBins", PredType::NATIVE_INT, 1,
                singleDimensions, &hxtBuffer.nBins);
   buildDataSet(file, "numRows", PredType::NATIVE_INT, 1,
                singleDimensions, &hxtBuffer.nRows);
   buildDataSet(file, "numCols", PredType::NATIVE_INT, 1,
                singleDimensions, &hxtBuffer.nCols);

   buildDataSet(file, "channelData", PredType::NATIVE_DOUBLE, 1,
                channelDimensions, allDataPointer);

   allDataPointer += hxtBuffer.nBins;
   buildDataSet(file, "spectrumData", PredType::NATIVE_DOUBLE, 3,
                spectrumDimensions, allDataPointer);

   this->file.close();

}

void FileReader::setFileId(H5File file)
{
   this->file = file;
}

void FileReader::buildDataSet(H5File file, string dsName, PredType dsType, int rank, hsize_t dsDimensions[], void *dsValues)
{
   DataSpace dataSpace(rank, dsDimensions);
   DataSet dataSet(file.createDataSet(dsName, dsType, dataSpace));

   dataSet.write(dsValues, dsType);

   dataSpace.close();
   dataSet.close();
}
*/
