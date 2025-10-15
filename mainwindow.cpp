#include "mainwindow.h"
#include <QVBoxLayout>
#include <QWidget>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    label = new QLabel("Hello World", this);
    button = new QPushButton("Click me", this);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    QVBoxLayout *layout2 = new QVBoxLayout(centralWidget);
    layout->addWidget(label);
    layout2->addWidget(label);
    layout->addWidget(button);

    connect(button, &QPushButton::clicked, this, &MainWindow::onButtonClicked);

}

void MainWindow::onButtonClicked() {
    label->setText("Button clicked");
}
