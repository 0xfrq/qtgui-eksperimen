#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QGridLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("ROS Command Runner");
    resize(800, 600);
    
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    QGridLayout *buttonLayout = new QGridLayout();
    
    createButton("OP3 Manager", "roslaunch", QStringList() << "op3_manager" << "op3_manager.launch");
    createButton("Vision 2024", "rosrun", QStringList() << "v1_detection" << "vision_2024ssh.py");
    createButton("Driver Baru", "rosrun", QStringList() << "kri2024" << "driver_baru.py");
    createButton("Process Com2", "rosrun", QStringList() << "kri2024" << "process_com2.py");
    createButton("Motion Control", "rostopic", QStringList() << "pub" << "/DEWO/MotionControl/MotionModule" << "std_msgs/String" << "data: 'action'");
    createButton("Odometry", "rosrun", QStringList() << "kri2024" << "odometry.py");
    createButton("Receiver8", "rosrun", QStringList() << "protocolgc" << "receiver8.py");
    createButton("Com Receiver Shooter", "rosrun", QStringList() << "communication" << "com_receivershooter.py");
    createButton("Com Receiver Nasional", "rosrun", QStringList() << "communication" << "com_receivernasional.py");
    createButton("Com Sender Kickoff", "rosrun", QStringList() << "communication" << "com_senderkickoff.py");
    createButton("Com Sender Nasional", "rosrun", QStringList() << "communication" << "com_sendernasional.py");
    
    int row = 0, col = 0;
    foreach(QPushButton *button, buttonProcessMap.keys()) {
        buttonLayout->addWidget(button, row, col);
        col++;
        if (col > 3) {
            col = 0;
            row++;
        }
    }
    
    QLabel *outputLabel = new QLabel("Command Output:", this);
    
    outputDisplay = new QTextEdit(this);
    outputDisplay->setReadOnly(true);
    outputDisplay->setPlaceholderText("Click a button to start/stop a command...");
    
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(outputLabel);
    mainLayout->addWidget(outputDisplay);
}

MainWindow::~MainWindow()
{
    foreach(QProcess *proc, buttonProcessMap.values()) {
        if (proc && proc->state() == QProcess::Running) {
            proc->kill();
            proc->waitForFinished();
        }
        delete proc;
    }
}

void MainWindow::createButton(const QString &label, const QString &command, const QStringList &args)
{
    QPushButton *button = new QPushButton(label, this);
    QProcess *process = new QProcess(this);
    
    buttonProcessMap[button] = process;
    buttonCommandMap[button] = command;
    buttonArgsMap[button] = args;
    
    connect(button, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(onProcessOutput()));
    connect(process, SIGNAL(readyReadStandardError()), this, SLOT(onProcessOutput()));
}

void MainWindow::onButtonClicked()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (button) {
        toggleProcess(button);
    }
}

void MainWindow::toggleProcess(QPushButton *button)
{
    QProcess *process = buttonProcessMap[button];
    
    if (process->state() == QProcess::Running) {
        process->kill();
        process->waitForFinished();
        button->setStyleSheet("");
        outputDisplay->append(QString("\n[%1] Stopped\n").arg(button->text()));
    } else {
        QString command = buttonCommandMap[button];
        QStringList args = buttonArgsMap[button];
        
        outputDisplay->append(QString("\n[%1] Starting: %2 %3\n").arg(button->text()).arg(command).arg(args.join(" ")));
        process->start(command, args);
        button->setStyleSheet("color: green;");
    }
}

void MainWindow::onProcessOutput()
{
    QProcess *process = qobject_cast<QProcess*>(sender());
    if (process) {
        QString output = process->readAllStandardOutput();
        QString error = process->readAllStandardError();
        
        if (!output.isEmpty()) {
            outputDisplay->append(output);
        }
        if (!error.isEmpty()) {
            outputDisplay->append(error);
        }
        
        QTextCursor cursor = outputDisplay->textCursor();
        cursor.movePosition(QTextCursor::End);
        outputDisplay->setTextCursor(cursor);
    }
}

