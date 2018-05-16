#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "autofocusprocessmanager.h"
#include <QMessageBox>

quint32 static logStrPos = 0;
constexpr qreal zoomMultiple_inc = 1.05;
constexpr qreal zoomMultple_dec = 1/zoomMultiple_inc;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->graphicsView->setScene(&core.scene);
   // ui->gridLayout->addWidget(mgraphics);

    AutoFocusProcessManager::sharedManager().setCore(&core);

    connect(&core.scene,&VScene::zoomIn,
            this,&MainWindow::on_actionZoomIn_triggered);

    connect(&core.scene,&VScene::zoomOut,
            this,&MainWindow::on_actionZoomOut_triggered);

    connect(&core,&Core::updateFocusQualityBar,this,&MainWindow::updateUIfocusBar);

    connect(this,&MainWindow::startWalking,&core,&Core::traverseWalkOnImage);


    steps = ui->spinBox->value();
    steps_up_down = ui->spinBox_2->value();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_Button_RightCamera_clicked()
{
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
 //   mgraphics->scale(zoomMultiple_inc,zoomMultiple_inc);
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
    msgToLog(core.moveToLeft(steps));
}

void MainWindow::on_Button_UpCamera_clicked()
{
    msgToLog(core.moveToForward(steps));
}

void MainWindow::printPortFeedBack(const QString &message)
{
    msgToLog(message);
}


void MainWindow::on_spinBox_valueChanged(int arg1)
{
    steps = arg1;
}


void MainWindow::on_Button_DownCamera_clicked()
{
    msgToLog(core.moveToBackward(steps));
}

void MainWindow::on_pushButton_clicked()//autoFucus button
{
    core.setAutoFocusSemaphore(true);
    msgToLog("autofocus is running");
}

void MainWindow::updateUIfocusBar(int value)
{
    int maxValue = static_cast<int> (AutoFocusMath::getInstance().currentMaxFocusValue);
    qDebug() << "maxValue == " << maxValue;
    ui->focusQuality->setValue(ui->focusQuality->maximum()*value/maxValue);
}


void MainWindow::on_pushButton_2_clicked()// stop auto focus
{
    core.setAutoFocusSemaphore(false);
    msgToLog("autofocus is stopped");
}

void MainWindow::on_commandLinkButton_2_clicked()
{
    msgToLog(core.moveToUp(ui->spinBox_2->value()));
}

void MainWindow::on_commandLinkButton_clicked()
{
    msgToLog(core.moveToDown(ui->spinBox_2->value()));
}

void MainWindow::on_action_4_triggered()//show log
{
    ui->dockWidget->show();
}

void MainWindow::on_pushButton_3_clicked()//main algo
{
    QMessageBox::StandardButton reply;

    reply = QMessageBox::question(this, "Start walking", "Are you sure that the objects are focused?",
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    if (reply == QMessageBox::Yes) {
        qDebug() << "Yes was clicked";
        emit startWalking();
    }
    if (reply == QMessageBox::No)
    {
        // toDo
    }
    if(reply == QMessageBox::Cancel)
    {
        //toDo
    }
}














