#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "core.h"

#include <QMainWindow>
#include <QThread>
#include <QString>
#include <QDebug>
#include <QGraphicsView>

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

    void on_Button_RightCamera_clicked();

    void on_action_triggered();

    void on_action_2_triggered();

    void on_action_3_triggered();

    void on_actionCamera_triggered();

    void on_actionStopCamera_triggered();

    void on_actionZoomIn_triggered();

    void on_actionZoomOut_triggered();

    void on_action_save_triggered();

private:
    Ui::MainWindow *ui;
    Core core;
    void msgToLog(const QString&);
};

#endif // MAINWINDOW_H
