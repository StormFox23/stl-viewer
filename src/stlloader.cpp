#include "stlloader.h"
#include "stlviewer.h"
#include <QTextStream>
#include <QDebug>
#include <QRegularExpression>

QVector<Triangle> STLLoader::loadSTL(const QString& filename, QString& error)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        error = QString("Cannot open file: %1").arg(file.errorString());
        return QVector<Triangle>();
    }
    
    QVector<Triangle> triangles;
    
    if (isBinarySTL(file)) {
        triangles = loadBinarySTL(file, error);
    } else {
        triangles = loadAsciiSTL(file, error);
    }
    
    file.close();
    return triangles;
}

bool STLLoader::isBinarySTL(QFile& file)
{
    // Check if file starts with "solid" (ASCII format)
    file.seek(0);
    QByteArray header = file.read(5);
    
    if (header.startsWith("solid")) {
        // Could be ASCII, but let's check file size
        file.seek(0);
        QByteArray firstLine = file.readLine();
        
        // If it's a simple "solid" line, it's likely ASCII
        if (firstLine.trimmed() == "solid" || firstLine.contains("solid ")) {
            return false; // ASCII
        }
    }
    
    // Check binary format: 80-byte header + 4-byte triangle count
    file.seek(0);
    if (file.size() < 84) {
        return false; // Too small for binary format
    }
    
    file.seek(80); // Skip header
    QDataStream stream(&file);
    stream.setByteOrder(QDataStream::LittleEndian);
    
    quint32 triangleCount;
    stream >> triangleCount;
    
    // Calculate expected file size for binary format
    qint64 expectedSize = 80 + 4 + (triangleCount * 50); // 50 bytes per triangle
    
    return (file.size() == expectedSize);
}

QVector<Triangle> STLLoader::loadBinarySTL(QFile& file, QString& error)
{
    QVector<Triangle> triangles;
    
    file.seek(80); // Skip 80-byte header
    QDataStream stream(&file);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
    
    quint32 triangleCount;
    stream >> triangleCount;
    
    if (triangleCount == 0) {
        error = "No triangles found in binary STL file";
        return triangles;
    }
    
    if (triangleCount > 10000000) { // Sanity check
        error = "Triangle count seems too large, possibly corrupted file";
        return triangles;
    }
    
    triangles.reserve(triangleCount);
    
    for (quint32 i = 0; i < triangleCount; ++i) {
        Triangle triangle;
        
        // Read normal vector
        float nx, ny, nz;
        stream >> nx >> ny >> nz;
        triangle.normal = QVector3D(nx, ny, nz);
        
        // Read vertices
        float x1, y1, z1, x2, y2, z2, x3, y3, z3;
        stream >> x1 >> y1 >> z1;
        stream >> x2 >> y2 >> z2;
        stream >> x3 >> y3 >> z3;
        
        triangle.vertex1 = QVector3D(x1, y1, z1);
        triangle.vertex2 = QVector3D(x2, y2, z2);
        triangle.vertex3 = QVector3D(x3, y3, z3);
        
        // Skip attribute byte count (2 bytes)
        quint16 attributeByteCount;
        stream >> attributeByteCount;
        
        // If normal is zero, calculate it from vertices
        if (triangle.normal.lengthSquared() == 0) {
            QVector3D v1 = triangle.vertex2 - triangle.vertex1;
            QVector3D v2 = triangle.vertex3 - triangle.vertex1;
            triangle.normal = QVector3D::crossProduct(v1, v2).normalized();
        }
        
        triangles.append(triangle);
        
        if (stream.status() != QDataStream::Ok) {
            error = QString("Error reading binary STL file at triangle %1").arg(i);
            return QVector<Triangle>();
        }
    }
    
    return triangles;
}

QVector<Triangle> STLLoader::loadAsciiSTL(QFile& file, QString& error)
{
    QVector<Triangle> triangles;
    
    file.seek(0);
    QTextStream stream(&file);
    
    QString line;
    Triangle currentTriangle;
    int vertexCount = 0;
    bool inFacet = false;
    bool inLoop = false;
    
    QRegularExpression normalRegex(R"(^\s*facet\s+normal\s+([+-]?\d*\.?\d+([eE][+-]?\d+)?)\s+([+-]?\d*\.?\d+([eE][+-]?\d+)?)\s+([+-]?\d*\.?\d+([eE][+-]?\d+)?)\s*$)");
    QRegularExpression vertexRegex(R"(^\s*vertex\s+([+-]?\d*\.?\d+([eE][+-]?\d+)?)\s+([+-]?\d*\.?\d+([eE][+-]?\d+)?)\s+([+-]?\d*\.?\d+([eE][+-]?\d+)?)\s*$)");
    
    while (!stream.atEnd()) {
        line = stream.readLine().trimmed();
        
        if (line.startsWith("solid")) {
            // Beginning of solid
            continue;
        } else if (line.startsWith("endsolid")) {
            // End of solid
            break;
        } else if (line.startsWith("facet normal")) {
            // Parse normal
            auto match = normalRegex.match(line);
            if (match.hasMatch()) {
                float nx = match.captured(1).toFloat();
                float ny = match.captured(3).toFloat();
                float nz = match.captured(5).toFloat();
                currentTriangle.normal = QVector3D(nx, ny, nz);
                inFacet = true;
                vertexCount = 0;
            } else {
                error = QString("Invalid normal format: %1").arg(line);
                return QVector<Triangle>();
            }
        } else if (line.startsWith("outer loop")) {
            if (!inFacet) {
                error = "Found 'outer loop' outside facet";
                return QVector<Triangle>();
            }
            inLoop = true;
        } else if (line.startsWith("vertex")) {
            if (!inLoop) {
                error = "Found vertex outside loop";
                return QVector<Triangle>();
            }
            
            auto match = vertexRegex.match(line);
            if (match.hasMatch()) {
                float x = match.captured(1).toFloat();
                float y = match.captured(3).toFloat();
                float z = match.captured(5).toFloat();
                QVector3D vertex(x, y, z);
                
                switch (vertexCount) {
                    case 0: currentTriangle.vertex1 = vertex; break;
                    case 1: currentTriangle.vertex2 = vertex; break;
                    case 2: currentTriangle.vertex3 = vertex; break;
                    default:
                        error = "Too many vertices in facet";
                        return QVector<Triangle>();
                }
                vertexCount++;
            } else {
                error = QString("Invalid vertex format: %1").arg(line);
                return QVector<Triangle>();
            }
        } else if (line.startsWith("endloop")) {
            if (!inLoop) {
                error = "Found 'endloop' without matching 'outer loop'";
                return QVector<Triangle>();
            }
            if (vertexCount != 3) {
                error = QString("Facet has %1 vertices, expected 3").arg(vertexCount);
                return QVector<Triangle>();
            }
            inLoop = false;
        } else if (line.startsWith("endfacet")) {
            if (!inFacet) {
                error = "Found 'endfacet' without matching 'facet'";
                return QVector<Triangle>();
            }
            if (inLoop) {
                error = "Found 'endfacet' with unclosed loop";
                return QVector<Triangle>();
            }
            
            // If normal is zero, calculate it from vertices
            if (currentTriangle.normal.lengthSquared() == 0) {
                QVector3D v1 = currentTriangle.vertex2 - currentTriangle.vertex1;
                QVector3D v2 = currentTriangle.vertex3 - currentTriangle.vertex1;
                currentTriangle.normal = QVector3D::crossProduct(v1, v2).normalized();
            }
            
            triangles.append(currentTriangle);
            inFacet = false;
        }
    }
    
    if (triangles.isEmpty()) {
        error = "No triangles found in ASCII STL file";
    }
    
    return triangles;
}
