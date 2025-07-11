#ifndef STLVIEWER_H
#define STLVIEWER_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QVector3D>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QTimer>

struct Triangle {
    QVector3D normal;
    QVector3D vertex1;
    QVector3D vertex2;
    QVector3D vertex3;
};

class STLViewer : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit STLViewer(QWidget *parent = nullptr);
    ~STLViewer();

    void loadSTL(const QString& filename);
    void resetView();

signals:
    void modelLoaded(const QString& filename, int triangleCount);
    void loadError(const QString& error);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private slots:
    void animate();

private:
    void setupShaders();
    void setupBuffers();
    void calculateBoundingBox();
    void centerModel();
    
    QOpenGLShaderProgram* m_shaderProgram;
    QOpenGLBuffer m_vertexBuffer;
    QOpenGLBuffer m_normalBuffer;
    QOpenGLVertexArrayObject m_vao;
    
    QMatrix4x4 m_projection;
    QMatrix4x4 m_view;
    QMatrix4x4 m_model;
    
    QVector<Triangle> m_triangles;
    QVector<QVector3D> m_vertices;
    QVector<QVector3D> m_normals;
    
    // Camera controls
    float m_rotationX;
    float m_rotationY;
    float m_zoom;
    QPoint m_lastMousePos;
    bool m_mousePressed;
    
    // Model bounds
    QVector3D m_minBounds;
    QVector3D m_maxBounds;
    QVector3D m_center;
    float m_modelScale;
    
    // Animation
    QTimer* m_animationTimer;
    bool m_animationEnabled;
    
    // Rendering
    bool m_modelLoaded;
    QString m_currentFile;
};

#endif // STLVIEWER_H
