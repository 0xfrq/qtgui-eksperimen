#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), currentButton(0), isContinuousRunning(false)
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
    button4 = new QPushButton("Launch OP3 Manager", this);
    
    buttonLayout->addWidget(button1);
    buttonLayout->addWidget(button2);
    buttonLayout->addWidget(button3);
    buttonLayout->addWidget(button4);
    
    QLabel *outputLabel = new QLabel("Command Output:", this);
    
    outputDisplay = new QTextEdit(this);
    outputDisplay->setReadOnly(true);
    outputDisplay->setPlaceholderText("Click a button to execute a command...");
    
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(outputLabel);
    mainLayout->addWidget(outputDisplay);
    
    process = new QProcess(this);
    continuousProcess = new QProcess(this);
    
    connect(button1, SIGNAL(clicked()), this, SLOT(onButton1Clicked()));
    connect(button2, SIGNAL(clicked()), this, SLOT(onButton2Clicked()));
    connect(button3, SIGNAL(clicked()), this, SLOT(onButton3Clicked()));
    connect(button4, SIGNAL(clicked()), this, SLOT(onButton4Clicked()));
    
    connect(process, SIGNAL(finished(int)), this, SLOT(onProcessFinished(int)));
    connect(process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(onProcessError(QProcess::ProcessError)));
    
    connect(continuousProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(onContinuousProcessOutput()));
}

MainWindow::~MainWindow()
{
    if (process) {
        process->kill();
        process->waitForFinished();
    }
    if (continuousProcess) {
        continuousProcess->kill();
        continuousProcess->waitForFinished();
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

void MainWindow::onButton4Clicked()
{
    if (isContinuousRunning) {
        stopContinuousCommand();
    } else {
        startContinuousCommand();
    }
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

void MainWindow::startContinuousCommand()
{
    outputDisplay->clear();
    outputDisplay->append("Starting ROS launch: roslaunch op3_manager op3_manager.launch\n");
    outputDisplay->append("----------------------------------------\n");
    
    QStringList args;
    args << "op3_manager" << "op3_manager.launch";
    
    continuousProcess->start("roslaunch", args);
    isContinuousRunning = true;
    button4->setStyleSheet("color: green;");
    button4->setText("Stop OP3 Manager");
}

void MainWindow::stopContinuousCommand()
{
    if (continuousProcess->state() == QProcess::Running) {
        continuousProcess->kill();
        continuousProcess->waitForFinished();
    }
    
    outputDisplay->append("\n----------------------------------------\n");
    outputDisplay->append("ROS launch stopped.\n");
    
    isContinuousRunning = false;
    button4->setStyleSheet("");
    button4->setText("Launch OP3 Manager");
}

void MainWindow::onContinuousProcessOutput()
{
    QString output = continuousProcess->readAllStandardOutput();
    if (!output.isEmpty()) {
        outputDisplay->append(output);
        QTextCursor cursor = outputDisplay->textCursor();
        cursor.movePosition(QTextCursor::End);
        outputDisplay->setTextCursor(cursor);
    }
}

