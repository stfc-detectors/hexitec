#include "frameProcessor.h"
#include <iostream>
#include <fstream>
#include <QDebug>

FrameProcessor::FrameProcessor()
{
   pixelProcessor = new PixelProcessor();
}

FrameProcessor::~FrameProcessor()
{
   //   free((void *)re_ordered);
}

void FrameProcessor::setGradients(const char *filename)
{
   double *gradientValue;

   gradientValue = getData(filename);
   pixelProcessor->initialiseEnergyCalibration(gradientValue, pixelProcessor->getInterceptValue());
}

void FrameProcessor::setIntercepts(const char *filename)
{
   double *interceptValue;

   interceptValue = getData(filename);
   pixelProcessor->initialiseEnergyCalibration(pixelProcessor->getGradientValue(), interceptValue);
}

double *FrameProcessor::getData(const char *filename)
{
   int i = 0;
   double *dataValue;
   std::ifstream inFile;

   dataValue = (double *) calloc(6400, sizeof(double));
   inFile.open(filename);

   if (!inFile)
     qDebug() << "error opening c_gradients.txt";
   while (inFile >> dataValue[i])
   {
       i++;
   }

   if (i < 6400)
     qDebug() << "error: only " << i << " could be read";
   else
     qDebug() << "gradients read OK ";
   inFile.close();

   return dataValue;
}

uint16_t *FrameProcessor::process(uint16_t *frame)
{
   uint16_t *result;
   const char* filename = "C://karen//STFC//Technical//PLTest//re_order.bin";

   qDebug() << "START PROCESSING 1" << endl;
   result = pixelProcessor->re_orderFrame(frame);
   writeFile(result, filename);

   return result;
}

uint16_t *FrameProcessor::process(uint16_t *frame, uint16_t thresholdValue)
{
   uint16_t *result;
   const char* filename = "C://karen//STFC//Technical//PLTest//re_orderThreshVal.bin";

   qDebug() << "START PROCESSING 2" << endl;
   result = pixelProcessor->re_orderFrame(frame, thresholdValue);
   writeFile(result, filename);

   return result;
}

uint16_t *FrameProcessor::process(uint16_t *frame, uint16_t *thresholdPerPix)
{
   uint16_t *result;
   const char* filename = "C://karen//STFC//Technical//PLTest//re_orderThreshPerPix.bin";

   qDebug() << "START PROCESSING 3";
   result = pixelProcessor->re_orderFrame(frame, thresholdPerPix, NULL);
   writeFile(result, filename);

   return result;
}
/*
uint16_t *FrameProcessor::process(uint16_t *frame, uint16_t thresholdValue,
                                  double *gradientValue, double *interceptValue)
{
   uint16_t *result;
   double *pixelEnergy;
   const char* filename = "C://karen//STFC//Technical//PLTest//re_orderThreshPerPix.bin";

   qDebug() << "START PROCESSING 4" << endl;
   result = pixelProcessor->re_orderFrame(frame, thresholdValue, gradientValue, interceptValue, pixelEnergy);
   writeFile(result, filename);

   return result;
}
uint16_t *FrameProcessor::process(uint16_t *frame, uint16_t *thresholdPerPix,
                                  double *gradientValue, double *interceptValue)
{
   uint16_t *result;
   double *pixelEnergy;
   const char* filename = "C://karen//STFC//Technical//PLTest//re_orderThreshPerPix.bin";

   qDebug() << "START PROCESSING 4" << endl;
   result = pixelProcessor->re_orderFrame(frame, thresholdPerPix, gradientValue, interceptValue, pixelEnergy);
   writeFile(result, filename);

   return result;
}
*/
void FrameProcessor::writeFile(uint16_t *result, const char* filename)
{
   std::ofstream outFile;

   qDebug() <<"Write re-ordered IMAGE: address = " << result << " contents " << *result << endl;

   outFile.open(filename, std::ofstream::binary);
   outFile.write((const char *)result, 6400 * sizeof(uint16_t));
   outFile.close();
}


/*
void FrameProcessor::initialise(string configDirectory)
{
   std::ifstream inFile;

   inFile.open(configDirectory + "//" + reorderFile, ifstream::binary);
   inFile.read((char *)&order[0], 2400 * sizeof(int32_t));
   if (!inFile)
     std::cout << "error: only " << inFile.gcount() << " could be read" << endl;
   else
     std::cout << "reorderFile read OK " << endl;
   inFile.close();

   int index = 0;

   cout << "Initializing Processing... " << endl;



}

void FrameProcessor::setImageFileList(list<string> imageNameList)
{
   this->imageNameList = imageNameList;

   cout << "FrameProcessor::setImageFileList" << endl;
}

void FrameProcessor::setDarkFileList(list<string> darkNameList)
{
   this->darkNameList = darkNameList;

   cout << "FrameProcessor::setDarkFileList" << endl;
}

void FrameProcessor::setDarkRegExp(string darkRegExp)
{
   this->darkRegExp = darkRegExp;

   cout << "FrameProcessor::setDarkRegExp" << endl;
}

uint8_t *FrameProcessor::readImage(string fileToProcess)
{
   TIFF* tifFile = TIFFOpen(fileToProcess.c_str(), "r");
   int scanLength;
   uint8_t *image;
   int pixelBytes;
   uint8_t *imageRowAddress;
   uint32 byteCount;

   cout << "fileToProcess: " << fileToProcess << endl;
   if (TIFFGetField(tifFile, TIFFTAG_IMAGEWIDTH, &width) != 1)
   {
      string msg = "Failed to read width of TIFF";
      cout << msg << endl;
   }
   if (TIFFGetField(tifFile, TIFFTAG_IMAGELENGTH, &height) != 1)
   {
      string msg = "Failed to read height of TIFF";
      cout << msg << endl;
   }
   if (TIFFGetField(tifFile, TIFFTAG_STRIPBYTECOUNTS, &byteCount) != 1)
   {
      string msg = "Failed to read height of TIFF";
      cout << msg << endl;
   }
   scanLength = TIFFScanlineSize(tifFile);

   cout << fileToProcess << " read, image width: "
        << width << " image height: " << height
        << " scan length: " << scanLength
        << " strip bytes: " << byteCount
        << " strip count: " << TIFFNumberOfStrips(tifFile)
        << " directoy count: " << TIFFNumberOfDirectories(tifFile) <<endl;

   pixelBytes = width * height;
   image = (uint8_t *) _TIFFmalloc(pixelBytes * sizeof(uint8_t));

   if (image != NULL)
   {
      imageRowAddress = image;
      for (int row = 0; row < height; row++)
      {
         TIFFReadScanline(tifFile, &dataRaw[row][0] , row);
         imageRowAddress += width;
      }
   }

   TIFFClose(tifFile);

   return image;
}

uint16_t *FrameProcessor::process(uint8 *image)
{
   uint16_t *result;
   std::ofstream outFile;

   cout << "START PROCESSING" << endl;
   result = re_order(image);

   cout <<"Write re-ordered IMAGE: address = " << result << " contents " << *result << endl;

   outFile.open("C:\\karen\\STFC\\Hxt\\TestFiles\\Output\\re_ordered.txt", std::ofstream::binary);
   outFile.write((const char *)re_ordered, 2400 * height * sizeof(uint16_t));
   outFile.close();

   correct(image);

   cout <<"Write rotated IMAGE!!!" << endl;

   outFile.open("C:\\karen\\STFC\\Hxt\\TestFiles\\Output\\rotated.txt", std::ofstream::binary);
   outFile.write((const char *)rotated, 2400 * height * sizeof(uint16_t));
   outFile.close();

   return re_ordered;
}

uint16_t *FrameProcessor::re_order(uint8 *image)
{
   uint16_t *iterator;

   cout << "===START RE-ORDER" << endl;

   for (int row = 0; row < height; row++)
   {
      for (int col = 0; col < width; col++)
      {
         byteOrderedRaw[row][col] = (uint16_t) (16384 -(((dataRaw[row][lsb[col]]) | (dataRaw[row][msb[col]] ) << 8)));
      }
//      cout << "processing image, row: " << row << " value: " << hex << byteOrderedRaw[row][0] << endl;
   }

   pixels = 2400 * height;
   cout << "Bytes re-ordered! "<< endl;
   re_ordered = (uint16_t *) calloc(pixels, sizeof(uint16_t));
   iterator = re_ordered;
   cout << hex << "re_ordered " << re_ordered << " contents " << *re_ordered <<endl;
   cout << hex << "iterator " << iterator << " contents " << *iterator <<endl;

   for (int row = 0; row < height; row++)
   {
      for (int col = 0; col < 2400; col++)
      {
         memcpy ((void *)iterator, &byteOrderedRaw[row][order[col]], sizeof(uint16_t));
         iterator++;
      }
   }

   return re_ordered;
}

uint16_t *FrameProcessor::correct(uint8 *image)
{
   cout << "---START CORRECT" << endl;

   rotated = rotate(image);

   return rotated;
}

uint16_t *FrameProcessor::rotate(uint8_t *image)
{
   uint16_t arrOut[2400][2800];

   rotated = &arrOut[0][0];
   memcpy ((void *)&arr[0][0], re_ordered, 2800 * 2400 * sizeof(uint16_t));

   cout << dec << "------START ROTATE" << endl;
   for(int r=0; r < rows; r++)
   {
//      cout << "------START LOOP, r =  " << r<< endl;
      for(int c=0; c < cols; c++)
      {
 //        cout << "---------c loop c =  " << c << " r =  " << r
//              << " rows - r - 1 =  " << rows - r - 1
              //<< " cols - c - 1" << cols - c - 1
//              << endl;
         arrOut[c][r] = arr[rows - r - 1][c];
      }

   }

   return rotated;
}

uint16_t *FrameProcessor::getDarks()
{
   uint8_t *d;
   uint16_t *dark;

   readImage("C:\\karen\\STFC\\Hxt\\TestFiles\\Input\\Unit0-Dark2.tif");
   dark = re_order(d);

   return (uint16_t *)dark;
}

void FrameProcessor::setReorderCheck(int reorder)
{
   this->doRe_order = reorder;
}

void FrameProcessor::setDarkReorderCheck(int darkReorder)
{
   this->doDarkRe_order = darkReorder;
}

void FrameProcessor::setCorrectCheck(int correct)
{
   this->doCorrect = correct;
}

void FrameProcessor::setCombineCheck(int combine)
{
   this->doCombine = combine;
}

void FrameProcessor::cyclic_roll(uint16_t &a, uint16_t &b, uint16_t &c, uint16_t &d)
{
   int temp = a;
   a = b;
   b = c;
   c = d;
   d = temp;
   cout << "ROLLING: " <<temp <<endl;
}
*/
