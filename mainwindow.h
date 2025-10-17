#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTextEdit>
#include <QProcess>
#include <QMap>
#include <QScrollArea>
#include <QGroupBox>
#include <QLabel>
#include <QToolButton>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onButtonClicked();
    void onProcessOutput();
    void onClearClicked();
    void onStopAllClicked();

private:
    QMap<QPushButton*, QProcess*> buttonProcessMap;
    QMap<QPushButton*, QString> buttonCommandMap;
    QMap<QPushButton*, QStringList> buttonArgsMap;
    QTextEdit *outputDisplay;
    QScrollArea *scrollArea;
    QGroupBox *buttonGroup;
    QToolButton *clearButton;
    QToolButton *stopAllButton;
    
    void createButton(const QString &label, const QString &command, const QStringList &args);
    void toggleProcess(QPushButton *button);
    void stopAllProcesses();
    void applyAppStyle();
};

#endif
