#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "deserializer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();

    void on_actionDisplay_triggered();

    void on_actionSave_triggered();

    void on_actionMembers_triggered();

    void on_actionLog_triggered();

private:
    Ui::MainWindow *ui;
    TDeserializer* deserial;
};

#endif // MAINWINDOW_H
