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

    void on_Button_LeftCamera_clicked();

    void on_Button_UpCamera_clicked();

    void printPortFeedBack(const QString& message);

    void on_spinBox_valueChanged(int arg1);

    void on_Button_DownCamera_clicked();

    void on_pushButton_clicked();

    void updateUIfocusBar(int value);

    void on_pushButton_2_clicked();

    void on_commandLinkButton_2_clicked();

    void on_commandLinkButton_clicked();

    void on_action_4_triggered();

    void on_pushButton_3_clicked();

signals:
    void startWalking();
    void stopWalking();

private:
    Ui::MainWindow *ui;
    Core core;
    void msgToLog(const QString&);
    bool buttonMoveIsPressed;
    int steps;
    int steps_up_down;
};

#endif // MAINWINDOW_H
