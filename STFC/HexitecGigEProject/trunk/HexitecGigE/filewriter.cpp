#include "filewriter.h"
#include <QTextStream>
#include <QDebug>
#include <QDateTime>

FileWriter::FileWriter(const QString directory, const QString file)
{
   initFilename(directory, file, false, false);
}

FileWriter::FileWriter(const QString directory, const QString file, bool timestamp, bool islogfile)
{
   initFilename(directory, file, timestamp, islogfile);
}

void FileWriter::initFilename(const QString directory, const QString file, bool timestamp, bool islogfile)
{
   QString filename = directory;

   filename.append("/");
   filename.append(file);
   if (timestamp)
   {
      filename.append(QDateTime::currentDateTime().toString("yyMMdd_hhmmss"));
   }
   if (islogfile)
   {
      filename.append(".log");
   }

   this->file = new QFile(filename);
}

int FileWriter::write(int number)
{
   int error = -1;

   if (error = file->open(QIODevice::WriteOnly | QIODevice::Text))
   {
      QTextStream out(file);
      out << number << "\n";
   }
   file->close();

   return error;
}

int FileWriter::append(QString text)
{
   int error = -1;

   if (error = file->open(QIODevice::Append | QIODevice::Text))
   {
      QTextStream out(file);
      out << text << "\n";
      file->close();
   }

   return error;
}