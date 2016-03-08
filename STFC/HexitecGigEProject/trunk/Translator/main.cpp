#include <iostream>
#include <string>
#include <fstream>
#include "filereader.h"

using namespace std;
/*
using namespace H5;
std::ofstream outFile1;
std::ofstream outFile2;
std::ofstream outFile3;
*/

int main(int argc, char *argv[])
{

//   char *fileName = argv[1];
   string fileName = argv[1];
   char *fileType = argv[2];

   cout << "Converting " << fileName << " to file type " << fileType <<endl;
   FileReader fileReader;

   fileReader.hxt(fileName);

//   std::string fileNameString(charString);
   int found = fileName.find_last_of(".");
   int replaceLength = fileName.length() - found;
   fileName.replace(found, replaceLength, ".h5");
   fileReader.buildH5(fileName);
/*
   float m1[1000];
   float m2[20][1000];

   for(int i = 0; i < 1000; i++)
   {
       m1[i] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
       for (int j = 0; j < 20; j++)
       {
          m2[j][i] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) + (10 * j);
       }
   }

// hdf5write('BatteryCellTest6mins.h5','dSpaceXaxis',d,'NumRows',80,'NumCols',80,'SpectrumPerPixel',TotData);

//   with h5py.File('data.h5', 'w') as hf:
//       hf.create_dataset('dataset_1', data=m1)
//       hf.create_dataset('dataset_2', data=m2)
//   H5File h5File("karenHDF5Test.h5", H5F_ACC_TRUNC);
//   h5File.createDataSet('CPPdataset_1', m1);
//   h5File.createDataSet('CPPdataset_2', m2);

   // Create a new file using the default property lists.
   H5File file("C:\\karen\\karenHDF5Test.h5", H5F_ACC_TRUNC);


   // Create the data space for the dataset.
   hsize_t m1Dimensions[1];               // dataset dimensions
   hsize_t m2Dimensions[2];               // dataset dimensions
   m1Dimensions[0] = 1000;
   m2Dimensions[0] = 20;
   m2Dimensions[1] = 1000;


   DataSpace dataspace1(1, m1Dimensions);
   DataSpace dataspace2(2, m2Dimensions);

   // Create the dataset.
   DataSet dataset1(file.createDataSet("dSet1", PredType::NATIVE_FLOAT, dataspace1));
   DataSet dataset2(file.createDataSet("dSet2", PredType::NATIVE_FLOAT, dataspace2));

   dataset1.write(m1, PredType::NATIVE_FLOAT);
   dataset2.write(m2, PredType::NATIVE_FLOAT);

   dataspace1.close();
   dataspace2.close();
   dataset1.close();
   dataset2.close();
   file.close();
*/
   return 0;
}

