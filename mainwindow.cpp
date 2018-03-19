#include "mainwindow.h"
#include "ui_mainwindow.h"

quint32 static logStrPos = 0;
const qreal zoomMultiple = 1.05;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->graphicsView->setScene(&core.scene);
    connect(&core.scene,&VScene::zoomIn,
            this,&MainWindow::on_actionZoomIn_triggered);

    connect(&core.scene,&VScene::zoomOut,
            this,&MainWindow::on_actionZoomOut_triggered);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_Button_RightCamera_clicked()
{
    msgToLog(core.moveToRight());
}

void MainWindow::on_action_triggered()//connection port
{
   core.makeConnetionWithPort();
   msgToLog(QString("Port was connected!"));
}

void MainWindow::on_action_2_triggered()//disconnect port
{
    core.disconnectPort();
    msgToLog(QString("Port was disconnected"));
}

void MainWindow::msgToLog(const QString &msg)
{
    ui->textEdit->append(QString(QString::number(++logStrPos)+"# " + msg));
}

void MainWindow::on_action_3_triggered()//check port status
{
   msgToLog(core.checkPortStatus());
}

void MainWindow::on_actionCamera_triggered()
{
    core.makeCameraCapture();
    msgToLog(QString("Camera was captured!"));
}

void MainWindow::on_actionStopCamera_triggered()
{
    core.stopCameraCapture();
    msgToLog(QString("Camera was stopped!"));
}

void MainWindow::on_actionZoomIn_triggered()
{
    ui->graphicsView->scale(zoomMultiple,zoomMultiple);
}

void MainWindow::on_actionZoomOut_triggered()
{
    ui->graphicsView->scale(1/zoomMultiple,1/zoomMultiple);
}

void MainWindow::on_action_save_triggered()
{
    msgToLog(QString("image save status = ") + QString(core.printScreen()));
}
