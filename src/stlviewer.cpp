#include "stlviewer.h"
#include "stlloader.h"
#include <QOpenGLShader>
#include <QOpenGLTexture>
#include <QDebug>
#include <QtMath>

STLViewer::STLViewer(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_shaderProgram(nullptr)
    , m_rotationX(0.0f)
    , m_rotationY(0.0f)
    , m_zoom(1.0f)
    , m_mousePressed(false)
    , m_modelScale(1.0f)
    , m_animationTimer(nullptr)
    , m_animationEnabled(false)
    , m_modelLoaded(false)
{
    setFocusPolicy(Qt::StrongFocus);
    
    // Enable multisampling for smoother rendering
    QSurfaceFormat format;
    format.setSamples(4);
    setFormat(format);
    
    // Setup animation timer
    m_animationTimer = new QTimer(this);
    connect(m_animationTimer, &QTimer::timeout, this, &STLViewer::animate);
}

STLViewer::~STLViewer()
{
    makeCurrent();
    m_vao.destroy();
    m_vertexBuffer.destroy();
    m_normalBuffer.destroy();
    delete m_shaderProgram;
    doneCurrent();
}

void STLViewer::initializeGL()
{
    initializeOpenGLFunctions();
    
    // Set clear color to light gray
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    
    // Enable face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    // Enable polygon offset to avoid z-fighting
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);
    
    setupShaders();
    setupBuffers();
}

void STLViewer::setupShaders()
{
    m_shaderProgram = new QOpenGLShaderProgram(this);
    
    // Vertex shader
    const char* vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        
        uniform mat4 model;
        uniform mat4 view;
        uniform mat4 projection;
        
        out vec3 FragPos;
        out vec3 Normal;
        
        void main()
        {
            FragPos = vec3(model * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(model))) * aNormal;
            
            gl_Position = projection * view * vec4(FragPos, 1.0);
        }
    )";
    
    // Fragment shader
    const char* fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        
        in vec3 FragPos;
        in vec3 Normal;
        
        uniform vec3 lightPos;
        uniform vec3 lightColor;
        uniform vec3 objectColor;
        uniform vec3 viewPos;
        
        void main()
        {
            // Ambient
            float ambientStrength = 0.3;
            vec3 ambient = ambientStrength * lightColor;
            
            // Diffuse
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * lightColor;
            
            // Specular
            float specularStrength = 0.5;
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
            vec3 specular = specularStrength * spec * lightColor;
            
            vec3 result = (ambient + diffuse + specular) * objectColor;
            FragColor = vec4(result, 1.0);
        }
    )";
    
    m_shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    
    if (!m_shaderProgram->link()) {
        qDebug() << "Shader program linking failed:" << m_shaderProgram->log();
    }
}

void STLViewer::setupBuffers()
{
    m_vao.create();
    m_vao.bind();
    
    m_vertexBuffer.create();
    m_vertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    
    m_normalBuffer.create();
    m_normalBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    
    m_vao.release();
}

void STLViewer::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (!m_modelLoaded || m_vertices.isEmpty()) {
        return;
    }
    
    m_shaderProgram->bind();
    m_vao.bind();
    
    // Set up matrices
    m_model.setToIdentity();
    m_model.translate(0.0f, 0.0f, 0.0f);
    m_model.rotate(m_rotationX, 1.0f, 0.0f, 0.0f);
    m_model.rotate(m_rotationY, 0.0f, 1.0f, 0.0f);
    m_model.scale(m_modelScale * m_zoom);
    
    m_view.setToIdentity();
    m_view.translate(0.0f, 0.0f, -3.0f);
    
    // Set uniforms
    m_shaderProgram->setUniformValue("model", m_model);
    m_shaderProgram->setUniformValue("view", m_view);
    m_shaderProgram->setUniformValue("projection", m_projection);
    
    // Lighting uniforms
    m_shaderProgram->setUniformValue("lightPos", QVector3D(2.0f, 2.0f, 2.0f));
    m_shaderProgram->setUniformValue("lightColor", QVector3D(1.0f, 1.0f, 1.0f));
    m_shaderProgram->setUniformValue("objectColor", QVector3D(0.3f, 0.6f, 0.9f));
    m_shaderProgram->setUniformValue("viewPos", QVector3D(0.0f, 0.0f, 3.0f));
    
    // Draw the model
    glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
    
    m_vao.release();
    m_shaderProgram->release();
}

void STLViewer::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    
    m_projection.setToIdentity();
    m_projection.perspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);
}

void STLViewer::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_mousePressed = true;
        m_lastMousePos = event->position().toPoint();
    }
}

void STLViewer::mouseMoveEvent(QMouseEvent *event)
{
    if (m_mousePressed) {
        QPoint currentPos = event->position().toPoint();
        int deltaX = currentPos.x() - m_lastMousePos.x();
        int deltaY = currentPos.y() - m_lastMousePos.y();
        
        m_rotationY += deltaX * 0.5f;
        m_rotationX += deltaY * 0.5f;
        
        // Clamp rotation
        if (m_rotationX > 90.0f) m_rotationX = 90.0f;
        if (m_rotationX < -90.0f) m_rotationX = -90.0f;
        
        m_lastMousePos = currentPos;
        update();
    }
}

void STLViewer::wheelEvent(QWheelEvent *event)
{
    float delta = event->angleDelta().y() / 120.0f;
    m_zoom += delta * 0.1f;
    
    if (m_zoom < 0.1f) m_zoom = 0.1f;
    if (m_zoom > 10.0f) m_zoom = 10.0f;
    
    update();
}

void STLViewer::loadSTL(const QString& filename)
{
    QString error;
    QVector<Triangle> triangles = STLLoader::loadSTL(filename, error);
    
    if (!error.isEmpty()) {
        emit loadError(error);
        return;
    }
    
    if (triangles.isEmpty()) {
        emit loadError("No triangles found in STL file");
        return;
    }
    
    m_triangles = triangles;
    m_vertices.clear();
    m_normals.clear();
    
    // Convert triangles to vertex arrays
    for (const Triangle& triangle : triangles) {
        m_vertices.append(triangle.vertex1);
        m_vertices.append(triangle.vertex2);
        m_vertices.append(triangle.vertex3);
        
        m_normals.append(triangle.normal);
        m_normals.append(triangle.normal);
        m_normals.append(triangle.normal);
    }
    
    makeCurrent();
    
    // Update vertex buffer
    m_vao.bind();
    
    m_vertexBuffer.bind();
    m_vertexBuffer.allocate(m_vertices.constData(), m_vertices.size() * sizeof(QVector3D));
    m_shaderProgram->enableAttributeArray(0);
    m_shaderProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3);
    
    // Update normal buffer
    m_normalBuffer.bind();
    m_normalBuffer.allocate(m_normals.constData(), m_normals.size() * sizeof(QVector3D));
    m_shaderProgram->enableAttributeArray(1);
    m_shaderProgram->setAttributeBuffer(1, GL_FLOAT, 0, 3);
    
    m_vao.release();
    
    calculateBoundingBox();
    centerModel();
    
    m_modelLoaded = true;
    m_currentFile = filename;
    
    doneCurrent();
    
    emit modelLoaded(filename, triangles.size());
    update();
}

void STLViewer::calculateBoundingBox()
{
    if (m_vertices.isEmpty()) return;
    
    m_minBounds = m_vertices[0];
    m_maxBounds = m_vertices[0];
    
    for (const QVector3D& vertex : m_vertices) {
        m_minBounds.setX(qMin(m_minBounds.x(), vertex.x()));
        m_minBounds.setY(qMin(m_minBounds.y(), vertex.y()));
        m_minBounds.setZ(qMin(m_minBounds.z(), vertex.z()));
        
        m_maxBounds.setX(qMax(m_maxBounds.x(), vertex.x()));
        m_maxBounds.setY(qMax(m_maxBounds.y(), vertex.y()));
        m_maxBounds.setZ(qMax(m_maxBounds.z(), vertex.z()));
    }
    
    m_center = (m_minBounds + m_maxBounds) * 0.5f;
    
    QVector3D size = m_maxBounds - m_minBounds;
    float maxSize = qMax(qMax(size.x(), size.y()), size.z());
    m_modelScale = maxSize > 0 ? 2.0f / maxSize : 1.0f;
}

void STLViewer::centerModel()
{
    if (m_vertices.isEmpty()) return;
    
    makeCurrent();
    
    // Center the model by translating all vertices
    for (QVector3D& vertex : m_vertices) {
        vertex -= m_center;
    }
    
    // Update the vertex buffer
    m_vao.bind();
    m_vertexBuffer.bind();
    m_vertexBuffer.allocate(m_vertices.constData(), m_vertices.size() * sizeof(QVector3D));
    m_vao.release();
    
    doneCurrent();
}

void STLViewer::resetView()
{
    m_rotationX = 0.0f;
    m_rotationY = 0.0f;
    m_zoom = 1.0f;
    update();
}

void STLViewer::animate()
{
    if (m_animationEnabled) {
        m_rotationY += 1.0f;
        if (m_rotationY >= 360.0f) {
            m_rotationY = 0.0f;
        }
        update();
    }
}
