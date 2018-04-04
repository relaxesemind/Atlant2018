#include "mainwindow.h"
#include "ui_mainwindow.h"

quint32 static logStrPos = 0;
constexpr qreal zoomMultiple_inc = 1.05;
constexpr qreal zoomMultple_dec = 1/zoomMultiple_inc;

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
    int steps = ui->spinBox->value();
    msgToLog(core.moveToRight(steps));
}

void MainWindow::on_action_triggered()//connection port
{
   core.makeConnetionWithPort();
   connect(&core,&Core::feedBackFromPort,this,&MainWindow::printPortFeedBack);
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
    ui->graphicsView->scale(zoomMultiple_inc,zoomMultiple_inc);
}

void MainWindow::on_actionZoomOut_triggered()
{
    ui->graphicsView->scale(zoomMultple_dec,zoomMultple_dec);
}

void MainWindow::on_action_save_triggered()
{
    msgToLog(QString("image save status = ") + QString(core.printScreen()));
}

void MainWindow::on_Button_LeftCamera_clicked()
{
    int steps = ui->spinBox->value();
    msgToLog(core.moveToLeft(steps));
}

void MainWindow::on_Button_UpCamera_clicked()
{
    int steps = ui->spinBox->value();
    msgToLog(core.moveToUp(steps));
}

void MainWindow::printPortFeedBack(const QString &message)
{
    msgToLog(message);
}
