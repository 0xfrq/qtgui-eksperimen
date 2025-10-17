#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QScrollArea>
#include <QFont>
#include <QStyle>
#include <QIcon>
#include <QSpacerItem>
#include <QSizePolicy>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Robot Commands");
    resize(900, 640);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    applyAppStyle();

    buttonGroup = new QGroupBox(this);
    QGridLayout *buttonLayout = new QGridLayout(buttonGroup);

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
    for (QPushButton *button : buttonProcessMap.keys()) {
        buttonLayout->addWidget(button, row, col);
        col++;
        if (col > 3) {
            col = 0;
            row++;
        }
    }

    scrollArea = new QScrollArea(this);
    scrollArea->setWidget(buttonGroup);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFixedHeight(220);

    QHBoxLayout *topBar = new QHBoxLayout();
    QLabel *title = new QLabel("Quick Launch", this);
    QFont titleFont = title->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    title->setFont(titleFont);
    topBar->addWidget(title);
    topBar->addStretch();

    clearButton = new QToolButton(this);
    clearButton->setText("Clear Output");
    clearButton->setIcon(style()->standardIcon(QStyle::SP_DialogResetButton));
    connect(clearButton, &QToolButton::clicked, this, &MainWindow::onClearClicked);

    stopAllButton = new QToolButton(this);
    stopAllButton->setText("Stop All");
    stopAllButton->setIcon(style()->standardIcon(QStyle::SP_BrowserStop));
    connect(stopAllButton, &QToolButton::clicked, this, &MainWindow::onStopAllClicked);

    topBar->addWidget(clearButton);
    topBar->addWidget(stopAllButton);

    QLabel *outputLabel = new QLabel("Output", this);

    outputDisplay = new QTextEdit(this);
    QFont mono("Consolas");
    if (!mono.exactMatch()) mono = QFont("Courier New");
    mono.setPointSize(10);
    outputDisplay->setFont(mono);
    outputDisplay->setReadOnly(true);
    outputDisplay->setPlaceholderText("Click a button to start/stop a command...");

    mainLayout->addLayout(topBar);
    mainLayout->addWidget(scrollArea);
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
    button->setMinimumHeight(36);
    button->setCursor(Qt::PointingHandCursor);
    QFont f = button->font();
    f.setPointSize(10);
    button->setFont(f);

    QProcess *process = new QProcess(this);

    buttonProcessMap.insert(button, process);
    buttonCommandMap.insert(button, command);
    buttonArgsMap.insert(button, args);

    connect(button, &QPushButton::clicked, this, &MainWindow::onButtonClicked);
    connect(process, &QProcess::readyReadStandardOutput, this, &MainWindow::onProcessOutput);
    connect(process, &QProcess::readyReadStandardError, this, &MainWindow::onProcessOutput);
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
        button->setStyleSheet("background-color: #dff0d8; color: #2b7a2b; border: 1px solid #c5e0c5; border-radius:4px;");
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

void MainWindow::onClearClicked()
{
    outputDisplay->clear();
}

void MainWindow::onStopAllClicked()
{
    stopAllProcesses();
}

void MainWindow::stopAllProcesses()
{
    for (QProcess *proc : buttonProcessMap.values()) {
        if (proc && proc->state() == QProcess::Running) {
            proc->kill();
            proc->waitForFinished();
        }
    }
    for (QPushButton *btn : buttonProcessMap.keys()) {
        btn->setStyleSheet("");
    }
    outputDisplay->append("\n[All] Stopped\n");
}

void MainWindow::applyAppStyle()
{
    QString s = R"(
        QMainWindow { background: #f6f8fa; }
        QGroupBox { border: none; }
        QPushButton { background: #ffffff; border: 1px solid #d0d7de; padding: 6px 10px; border-radius: 6px; }
        QPushButton:hover { background: #f1f3f5; }
        QToolButton { background: #2b6cb0; color: white; padding: 6px 8px; border-radius: 6px; }
    )";
    setStyleSheet(s);
}

