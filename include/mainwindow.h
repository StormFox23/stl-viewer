#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStatusBar>
#include <QProgressBar>

class STLViewer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openFile();
    void resetView();
    void showAbout();
    void onModelLoaded(const QString& filename, int triangleCount);
    void onLoadError(const QString& error);

private:
    void setupUI();
    void setupMenuBar();
    void setupStatusBar();
    
    STLViewer* m_viewer;
    QLabel* m_statusLabel;
    QProgressBar* m_progressBar;
    QPushButton* m_resetButton;
};

#endif // MAINWINDOW_H
