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
    void onProcessFinished(int exitCode);
    void onProcessError(QProcess::ProcessError error);

private:
    QPushButton *button1;
    QPushButton *button2;
    QPushButton *button3;
    QTextEdit *outputDisplay;
    QProcess *process;
    
    void executeCommand(const QString &command);
};

#endif
