/*
 Voxel class for 2Easy code

 S D M Jacques 24th February 2011
*/
#include "voxel.h"
#include <QFile>
#include <QTextStream>

Voxel::Voxel()
{
   maxY = 0.0;
   meanY = 0.0;
   minY = 0.0;
   sumY = 0.0;
   contentYData.resize(0);
   contentXData.resize(0);
}

Voxel::Voxel(int size, double value)
{
   maxY = value;
   minY = value;
   sumY = size * value;
   contentYData.resize(size);
   contentXData.resize(size);

   // Should contentXData also be intialized (to (double)i for example)?
   if (size > 0)
   {
      for (int i = 0; i < size; i++)
      {
         contentXData[i] = 0.0;
         contentYData[i] = value;
      }
      meanY = sumY / size;
   }
}

Voxel::~Voxel()
{
}

double Voxel::getMax()
{
   return maxY;
}

double Voxel::getMean()
{
   return meanY;
}

double Voxel::getMin()
{
   return minY;
}

double Voxel::getSum()
{
   return sumY;
}


void Voxel::stats()
{
   sumY = 0.0;
   maxY = -DBL_MAX;
   minY = DBL_MAX;
   for (int i = 0 ; i < contentYData.size(); ++i)
   {
      minY = std::min(minY, contentYData[i]);
      maxY = std::max(maxY, contentYData[i]);
      sumY += contentYData[i];
   }
   if (contentYData.size() > 0)
   {
      meanY =  maxY / (1.0 * contentYData.size());
   }
   return;
}


bool Voxel::readXY(QString fileName)
{
   QFile file(fileName);
   if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
   {
      // writeMessage("Cannot open file " + fileName + "/nSkipping this file");
      return(false);
   }

   QTextStream in(&file);
   double x,y;
   int nBins = 0;
   in >> x >> y;
   while (!in.atEnd())
   {
      this->contentXData.push_back(x);
      this->contentYData.push_back(y);
      nBins++;
      in >> x >> y;
   }
   file.close();
   return(true);
}

/* Creates a new Voxel by adding together 'this' and 'anOtherVoxel'
  */
Voxel *Voxel::add(Voxel *anOtherVoxel)
{
   int i;
   Voxel *newVoxel = new Voxel();

   // X values are done separately because xData size may be zero
   for (i = 0; i < contentXData.size(); i++)
   {
      newVoxel->contentXData.append(contentXData.at(i));
   }

   for (i = 0; i < contentYData.size(); i++)
   {
      newVoxel->contentYData.append(contentYData.at(i) + anOtherVoxel->contentYData.at(i));
   }

   newVoxel->stats();

   return newVoxel;
}

Voxel *Voxel::add(double value)
{
   int i;
   Voxel *newVoxel = new Voxel();

   // X values are done separately because xdata size may be zero
   for (i = 0; i < contentXData.size(); i++)
   {
      newVoxel->contentXData.append(contentXData.at(i));
   }

   for (i = 0; i < contentYData.size(); i++)
   {
      newVoxel->contentYData.append(contentYData.at(i) + value);
   }

   newVoxel->stats();

   return newVoxel;
}

Voxel *Voxel::multiply(double value)
{
   int i;
   Voxel *newVoxel = new Voxel();

   // X values are done separately because xdata size may be zero
   for (i = 0; i < contentXData.size(); i++)
   {
      newVoxel->contentXData.append(contentXData.at(i));
   }

   for (i = 0; i < contentYData.size(); i++)
   {
      newVoxel->contentYData.append(contentYData.at(i) * value);
   }

   newVoxel->stats();

   return newVoxel;

}


