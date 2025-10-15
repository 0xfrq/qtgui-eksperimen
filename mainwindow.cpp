#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), currentButton(0)
{
    setWindowTitle("Linux Command Runner");
    resize(600, 400);
    
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    button1 = new QPushButton("List Files (ls -la)", this);
    button2 = new QPushButton("Show Date/Time (date)", this);
    button3 = new QPushButton("Disk Usage (df -h)", this);
    
    buttonLayout->addWidget(button1);
    buttonLayout->addWidget(button2);
    buttonLayout->addWidget(button3);
    
    QLabel *outputLabel = new QLabel("Command Output:", this);
    
    outputDisplay = new QTextEdit(this);
    outputDisplay->setReadOnly(true);
    outputDisplay->setPlaceholderText("Click a button to execute a command...");
    
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(outputLabel);
    mainLayout->addWidget(outputDisplay);
    
    process = new QProcess(this);
    
    connect(button1, SIGNAL(clicked()), this, SLOT(onButton1Clicked()));
    connect(button2, SIGNAL(clicked()), this, SLOT(onButton2Clicked()));
    connect(button3, SIGNAL(clicked()), this, SLOT(onButton3Clicked()));
    
    connect(process, SIGNAL(finished(int)), this, SLOT(onProcessFinished(int)));
    connect(process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(onProcessError(QProcess::ProcessError)));
}

MainWindow::~MainWindow()
{
    if (process) {
        process->kill();
        process->waitForFinished();
    }
}

void MainWindow::onButton1Clicked()
{
    executeCommand("ls -la", button1);
}

void MainWindow::onButton2Clicked()
{
    executeCommand("date", button2);
}

void MainWindow::onButton3Clicked()
{
    executeCommand("df -h", button3);
}

void MainWindow::executeCommand(const QString &command, QPushButton *button)
{
    if (process->state() == QProcess::Running) {
        outputDisplay->append("\n[ERROR] A command is already running. Please wait...\n");
        return;
    }
    
    currentButton = button;
    setButtonState(button, "running");
    
    outputDisplay->clear();
    outputDisplay->append(QString("Executing: %1\n").arg(command));
    outputDisplay->append("----------------------------------------\n");
    
    QStringList parts = command.split(" ");
    QString program = parts.takeFirst();
    
    process->start(program, parts);
}

void MainWindow::onProcessFinished(int exitCode)
{
    QString output = process->readAllStandardOutput();
    QString error = process->readAllStandardError();
    
    if (!output.isEmpty()) {
        outputDisplay->append(output);
    }
    
    if (!error.isEmpty()) {
        outputDisplay->append("\n[STDERR]:\n" + error);
    }
    
    outputDisplay->append("\n----------------------------------------");
    outputDisplay->append(QString("\nProcess finished with exit code: %1").arg(exitCode));
    
    if (currentButton) {
        if (exitCode == 0) {
            setButtonState(currentButton, "idle");
        } else {
            setButtonState(currentButton, "error");
        }
    }
}

void MainWindow::onProcessError(QProcess::ProcessError error)
{
    QString errorString;
    switch (error) {
        case QProcess::FailedToStart:
            errorString = "Failed to start. The command may not exist or you may lack permissions.";
            break;
        case QProcess::Crashed:
            errorString = "Process crashed.";
            break;
        case QProcess::Timedout:
            errorString = "Process timed out.";
            break;
        case QProcess::WriteError:
            errorString = "Write error occurred.";
            break;
        case QProcess::ReadError:
            errorString = "Read error occurred.";
            break;
        case QProcess::UnknownError:
            errorString = "Unknown error occurred.";
            break;
    }
    
    outputDisplay->append(QString("\n[ERROR] %1\n").arg(errorString));
    
    if (currentButton) {
        setButtonState(currentButton, "error");
    }
}

void MainWindow::setButtonState(QPushButton *button, const QString &state)
{
    if (state == "running") {
        button->setStyleSheet("color: green;");
    } else if (state == "error") {
        button->setStyleSheet("color: red;");
    } else {
        button->setStyleSheet("");
    }
}
