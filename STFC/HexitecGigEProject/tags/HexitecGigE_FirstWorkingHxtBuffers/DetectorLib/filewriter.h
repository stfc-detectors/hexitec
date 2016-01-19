#ifndef FILEWRITER_H
#define FILEWRITER_H

#include <QFile>
#include <QString>

class FileWriter : public QFile
{
public:
    FileWriter(const QString directory, const QString file);
    FileWriter(const QString directory, const QString file, bool timestamp, bool islogfile);
    int append(QString text);
private:
    QFile *file;
    void initFilename(const QString directory, const QString file, bool timestamp, bool islogfile);
};

#endif // FILEWRITER_H
