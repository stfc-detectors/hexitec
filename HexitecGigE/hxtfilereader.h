#ifndef HXTFILEREADER_H
#define HXTFILEREADER_H

#include <QObject>
#include <QFile>
#include <sarray.h>


class HXTFileReader : public QObject
{
    Q_OBJECT
public:
    explicit HXTFileReader(QObject *parent = 0);
    HXTFileReader(QString);

    QFile file;
    QString HXTFileName;

    bool readHeader();
    bool openFile(QString);
    bool readXData();
    bool readSingleSpectrum(int, int);

    quint64 hxtVersion;
    quint32 nRows, nCols, nBins;

    QVector <double> X;
    QVector <double> Y;
    SArray <double> line;

signals:
    void writeMessage(QString);
    void writeWarning(QString);
    void writeError(QString);

};

#endif // HXTFILEREADER_H
