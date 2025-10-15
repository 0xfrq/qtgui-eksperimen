#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QProcess>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onButton1Clicked();
    void onButton2Clicked();
    void onButton3Clicked();
    void onButton4Clicked();
    void onProcessFinished(int exitCode);
    void onProcessError(QProcess::ProcessError error);
    void onContinuousProcessOutput();

private:
    QPushButton *button1;
    QPushButton *button2;
    QPushButton *button3;
    QPushButton *button4;
    QTextEdit *outputDisplay;
    QProcess *process;
    QProcess *continuousProcess;
    QPushButton *currentButton;
    bool isContinuousRunning;
    
    void executeCommand(const QString &command, QPushButton *button);
    void setButtonState(QPushButton *button, const QString &state);
    void startContinuousCommand();
    void stopContinuousCommand();
};

#endif
