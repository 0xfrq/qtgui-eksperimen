#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QProcess>
#include <QMap>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onButtonClicked();
    void onProcessOutput();

private:
    QMap<QPushButton*, QProcess*> buttonProcessMap;
    QMap<QPushButton*, QString> buttonCommandMap;
    QMap<QPushButton*, QStringList> buttonArgsMap;
    QTextEdit *outputDisplay;
    
    void createButton(const QString &label, const QString &command, const QStringList &args);
    void toggleProcess(QPushButton *button);
};

#endif
