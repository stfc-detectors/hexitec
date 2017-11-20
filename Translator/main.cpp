#include <iostream>
#include <string>
#include <fstream>
#include "hxtfilereader.h"
#include "h5filewriter.h"

using namespace std;

int main(int argc, char *argv[])
{
   string fileName = argv[1];
   char *fileType = argv[2];

   HxtFileReader fileReader;
   H5FileWriter fileWriter;
   void *buffer;

   buffer = fileReader.read(fileName);

   int found = fileName.find_last_of(".");
   int replaceLength = fileName.length() - found;
   fileName.replace(found, replaceLength, ".h5");
   fileWriter.write(buffer, fileName);

   return 0;
}

