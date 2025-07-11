#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    app.setApplicationName("STL Viewer");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("STL Viewer");
    
    MainWindow window;
    window.show();
    
    return app.exec();
}
