#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Set window properties
    setWindowTitle("Linux Command Runner");
    resize(600, 400);
    
    // Create central widget and main layout
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // Create horizontal layout for buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    // Create three buttons with descriptive labels
    button1 = new QPushButton("List Files (ls -la)", this);
    button2 = new QPushButton("Show Date/Time (date)", this);
    button3 = new QPushButton("Disk Usage (df -h)", this);
    
    buttonLayout->addWidget(button1);
    buttonLayout->addWidget(button2);
    buttonLayout->addWidget(button3);
    
    // Create label for output section
    QLabel *outputLabel = new QLabel("Command Output:", this);
    
    // Create text display for command output
    outputDisplay = new QTextEdit(this);
    outputDisplay->setReadOnly(true);
    outputDisplay->setPlaceholderText("Click a button to execute a command...");
    
    // Add widgets to main layout
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(outputLabel);
    mainLayout->addWidget(outputDisplay);
    
    // Initialize process
    process = new QProcess(this);
    
    // Connect button signals
    connect(button1, &QPushButton::clicked, this, &MainWindow::onButton1Clicked);
    connect(button2, &QPushButton::clicked, this, &MainWindow::onButton2Clicked);
    connect(button3, &QPushButton::clicked, this, &MainWindow::onButton3Clicked);
    
    // Connect process signals
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::onProcessFinished);
    connect(process, &QProcess::errorOccurred, this, &MainWindow::onProcessError);
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
    executeCommand("ls -la");
}

void MainWindow::onButton2Clicked()
{
    executeCommand("date");
}

void MainWindow::onButton3Clicked()
{
    executeCommand("df -h");
}

void MainWindow::executeCommand(const QString &command)
{
    if (process->state() == QProcess::Running) {
        outputDisplay->append("\n[ERROR] A command is already running. Please wait...\n");
        return;
    }
    
    outputDisplay->clear();
    outputDisplay->append(QString("Executing: %1\n").arg(command));
    outputDisplay->append("----------------------------------------\n");
    
    // Split command into program and arguments
    QStringList parts = command.split(" ");
    QString program = parts.takeFirst();
    
    process->start(program, parts);
}

void MainWindow::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
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
}
