#include "mainwindow.h"
#include "stlviewer.h"
#include <QApplication>
#include <QStyle>
#include <QScreen>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_viewer(nullptr)
    , m_statusLabel(nullptr)
    , m_progressBar(nullptr)
    , m_resetButton(nullptr)
{
    setupUI();
    setupMenuBar();
    setupStatusBar();
    
    setWindowTitle("STL Viewer");
    resize(1000, 800);
    
    // Center the window
    QScreen* screen = QApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->availableGeometry();
        int x = (screenGeometry.width() - width()) / 2;
        int y = (screenGeometry.height() - height()) / 2;
        move(x, y);
    }
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    // Create the STL viewer
    m_viewer = new STLViewer(this);
    mainLayout->addWidget(m_viewer);
    
    // Create control panel
    QHBoxLayout* controlLayout = new QHBoxLayout();
    
    m_resetButton = new QPushButton("Reset View", this);
    m_resetButton->setEnabled(false);
    controlLayout->addWidget(m_resetButton);
    
    controlLayout->addStretch();
    
    QPushButton* openButton = new QPushButton("Open STL File", this);
    controlLayout->addWidget(openButton);
    
    mainLayout->addLayout(controlLayout);
    
    // Connect signals
    connect(openButton, &QPushButton::clicked, this, &MainWindow::openFile);
    connect(m_resetButton, &QPushButton::clicked, this, &MainWindow::resetView);
    connect(m_viewer, &STLViewer::modelLoaded, this, &MainWindow::onModelLoaded);
    connect(m_viewer, &STLViewer::loadError, this, &MainWindow::onLoadError);
}

void MainWindow::setupMenuBar()
{
    QMenuBar* menuBar = this->menuBar();
    
    // File menu
    QMenu* fileMenu = menuBar->addMenu("&File");
    
    QAction* openAction = fileMenu->addAction("&Open STL...");
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
    
    fileMenu->addSeparator();
    
    QAction* exitAction = fileMenu->addAction("E&xit");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    
    // View menu
    QMenu* viewMenu = menuBar->addMenu("&View");
    
    QAction* resetAction = viewMenu->addAction("&Reset View");
    resetAction->setShortcut(QKeySequence("Ctrl+R"));
    connect(resetAction, &QAction::triggered, this, &MainWindow::resetView);
    
    // Help menu
    QMenu* helpMenu = menuBar->addMenu("&Help");
    
    QAction* aboutAction = helpMenu->addAction("&About");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
}

void MainWindow::setupStatusBar()
{
    m_statusLabel = new QLabel("Ready");
    statusBar()->addWidget(m_statusLabel);
    
    m_progressBar = new QProgressBar();
    m_progressBar->setVisible(false);
    statusBar()->addPermanentWidget(m_progressBar);
}

void MainWindow::openFile()
{
    QString filename = QFileDialog::getOpenFileName(
        this,
        "Open STL File",
        "",
        "STL Files (*.stl);;All Files (*)"
    );
    
    if (!filename.isEmpty()) {
        m_statusLabel->setText("Loading STL file...");
        m_progressBar->setVisible(true);
        m_progressBar->setRange(0, 0); // Indeterminate progress
        
        m_viewer->loadSTL(filename);
    }
}

void MainWindow::resetView()
{
    if (m_viewer) {
        m_viewer->resetView();
    }
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, "About STL Viewer",
        "STL Viewer v1.0\n\n"
        "A simple STL file viewer built with Qt and OpenGL.\n\n"
        "Features:\n"
        "• Load and display STL files\n"
        "• Mouse controls for rotation and zoom\n"
        "• Automatic model centering and scaling\n\n"
        "Controls:\n"
        "• Left click + drag: Rotate model\n"
        "• Mouse wheel: Zoom in/out\n"
        "• Ctrl+R: Reset view");
}

void MainWindow::onModelLoaded(const QString& filename, int triangleCount)
{
    m_progressBar->setVisible(false);
    m_statusLabel->setText(QString("Loaded: %1 (%2 triangles)")
                          .arg(QFileInfo(filename).fileName())
                          .arg(triangleCount));
    m_resetButton->setEnabled(true);
}

void MainWindow::onLoadError(const QString& error)
{
    m_progressBar->setVisible(false);
    m_statusLabel->setText("Ready");
    
    QMessageBox::critical(this, "Load Error", 
        QString("Failed to load STL file:\n%1").arg(error));
}
