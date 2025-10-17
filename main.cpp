#include <QApplication>
#include "mainwindow.h"
#include <QStyleFactory>
#include <QPalette>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    QPalette pal = app.palette();
    pal.setColor(QPalette::Window, QColor(246,248,250));
    pal.setColor(QPalette::WindowText, QColor(27,31,35));
    pal.setColor(QPalette::Button, QColor(255,255,255));
    pal.setColor(QPalette::ButtonText, QColor(27,31,35));
    app.setPalette(pal);

    MainWindow window;
    window.show();

    return app.exec();
}
