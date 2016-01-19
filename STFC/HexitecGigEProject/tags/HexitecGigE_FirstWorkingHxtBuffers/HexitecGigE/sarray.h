/*
Generic 2D array
example usage :
SArray <Voxel> myVoxels(20, 20);
S D M Jacques 24th February 2011
*/


#ifndef SARRAY_H
#define SARRAY_H

#include <QVector>

template <typename T>

class SArray
{
public:
    SArray(){
        rowSize = 0;
        colSize = 0;
        m_data.resize(rowSize);
          }
    SArray(int rows, int cols):m_data(rows, QVector <T> (cols)){}
    inline QVector<T> & operator[](int i) { return m_data[i];}
    inline const QVector<T> & operator[] (int i) const { return m_data[i];}
    void resize(int rows, int cols){
        rowSize = rows;
        colSize = cols;
        m_data.resize(rows);
        for(int i = 0;i < rows;++i) m_data[i].resize(cols);
    }
    void zeros(){ //ok for numbers but not good for anything else - need to think about this
        for (int i = 0; i < m_data.size(); ++i)
        {
            for (int j = 0; j < m_data[i].size(); ++j)
            {
               m_data[i][j] = 0;
            }
        }
    }
    int width() {return rowSize;}
    int height() {return colSize;}


private:
    QVector < QVector <T> > m_data;
    int rowSize;
    int colSize;
};

#endif // SARRAY_H

