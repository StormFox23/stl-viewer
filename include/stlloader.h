#ifndef STLLOADER_H
#define STLLOADER_H

#include <QString>
#include <QVector>
#include <QVector3D>
#include <QFile>
#include <QDataStream>

// Forward declaration - Triangle is defined in stlviewer.h
struct Triangle;

class STLLoader
{
public:
    static QVector<Triangle> loadSTL(const QString& filename, QString& error);
    
private:
    static QVector<Triangle> loadBinarySTL(QFile& file, QString& error);
    static QVector<Triangle> loadAsciiSTL(QFile& file, QString& error);
    static bool isBinarySTL(QFile& file);
    static QVector3D parseVertex(const QString& line);
    static QVector3D parseNormal(const QString& line);
};

#endif // STLLOADER_H
