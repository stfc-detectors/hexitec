#ifndef FRAMEPROCESSOR_H
#define FRAMEPROCESSOR_H

#include "pixelProcessor.h"

#include <cstdint>
#include <string>
#include <list>
#include <regex>

using namespace std;

class FrameProcessor
{

public:
   FrameProcessor();
   ~FrameProcessor();
   void setGradients(const   char* filename);
   void setIntercepts(const   char* filename);
   uint16_t *process(uint16_t *frame);
   uint16_t *process(uint16_t *frame, uint16_t thresholdValue);
   uint16_t *process(uint16_t *frame, uint16_t *thresholdPerPixel);
//   uint16_t *process(uint16_t *frame, uint16_t thresholdValue, double *gradientValue, double *interceptValue);
//   uint16_t *process(uint16_t *frame, uint16_t *thresholdPerPixel, double *gradientValue, double *interceptValue);

private:
   PixelProcessor *pixelProcessor;
   void writeFile(uint16_t *result, const char *filename);
   double *getData(const char *filename);
   /*
   void initialise(string configDirectory);

   void setImageFileList(list<string> imageNameList);
   void setDarkFileList(list<string> darkNameList);
   void setDarkRegExp(string darkRegExp);
   void setFilePrefix();
   void setConfigDirectory();

   uint8_t *readImage(string fileToProcess);
   uint16_t *process(uint8_t *image);

   void setReorderCheck(int doRe_order);
   void setDarkReorderCheck(int doDarkRe_order);
   void setCorrectCheck(int doCorrect);
   void setCombineCheck(int doCombine);

private:
//   ArrayRaw dataRaw;
//   Array16Bit byteOrderedRaw;
   uint16_t *re_order(uint8_t *image);
   uint16_t *correct(uint8_t *image);
   uint16_t *rotate(uint8_t *image);
   uint16_t *getDarks();
   void cyclic_roll(uint16_t &a, uint16_t &b, uint16_t &c, uint16_t &d);

   string reorderFile;
   list<uint16_t *> imageList;
   list<uint16_t *> darkList;
   list<string> imageNameList;
   list<string> darkNameList;
   string darkRegExp;
   uint8_t dataRaw[2800][4992];
   uint16_t byteOrderedRaw[2800][4992];
   uint16_t *re_ordered;
   uint16_t *rotated;
   uint16_t arr[2800][2400];
   int32_t order[2400];
   uint16_t msb[2496];
   uint16_t lsb[2496];
   int width;
   int height;
   int pixels;
   int rows = 2800;
   int cols = 2400;
   bool doRe_order;
   bool doDarkRe_order;
   bool doCorrect;
   bool doCombine;
//   int rows = 2400;
//   int cols = 2800;
*/
};

#endif // FRAMEPROCESSOR_H
