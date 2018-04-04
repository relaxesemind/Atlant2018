#include "core.h"

bool portisWorking = false;

Core::Core(QObject *parent) : QObject(parent)
{

}

Core::~Core()
{

    if (camPtr)
    {
         camPtr->onStop();
    }
    m_pool.waitForDone();

}

void Core::makeConnetionWithPort()
{
    if (portisWorking == true)
        return;
    portisWorking = true;

    COM1port = std::make_unique<PortController> ();
    QThread* thread = new QThread();
    COM1port.get() -> moveToThread(thread);

    connect(thread,&QThread::started,COM1port.get(),&PortController::process_Port,Qt::DirectConnection);
    connect(COM1port.get(),&PortController::finished_Port,thread,&QThread::quit,Qt::DirectConnection);
    connect(thread,&QThread::finished,thread,&QThread::deleteLater,Qt::DirectConnection);
    connect(COM1port.get(),&PortController::outPort,this,&Core::feedBackFromPort,Qt::DirectConnection);
    //
    thread->start();
}

void Core::disconnectPort()
{
    if (portisWorking == false)
        return;
    portisWorking = false;
    COM1port.reset(nullptr);
}

QString Core::moveToRight(int steps)
{
    if (COM1port and COM1port->isWritable())
    {
         return COM1port->moveXBy(steps);
    }else
    {
         return QString("port error");
    }
}

QString Core::moveToLeft(int steps)
{
    if (COM1port and COM1port->isWritable())
    {
         return COM1port->moveXBy(-steps);
    }else
    {
         return QString("port error");
    }
}

QString Core::moveToUp(int steps)
{
    if (COM1port and COM1port->isWritable())
    {
         return COM1port->moveYBy(steps);
    }else
    {
         return QString("port error");
    }
}

QString Core::moveToDown(int steps)
{

}

QString Core::checkPortStatus()
{
    if (COM1port == nullptr)
    {
        return QString("port is null!");
    }
    QString status;
    if (COM1port->isConnected())
    {
        status = "port is connected!";
    }else{
        status = "port is disconnected!";
    }

    if (COM1port->isWritable())
        status += " isWritable";
    else status += " is not Writable";
    if (COM1port->isReadable())
        status += " isReadable";
    else status += " is not Readable";

    qDebug() << COM1port.get();
    return status;
}

void Core::makeCameraCapture()
{
    camPtr = new CamStreamTask;
    m_pool.start(camPtr);
    connect(camPtr,&CamStreamTask::frameAvailable,
            this,  &Core::updateVideoFrame);
}

void Core::stopCameraCapture()
{
    camPtr->onStop();
}

bool Core::printScreen()
{
  const QString Data_dir = QString("DATA/DATA ")
            + QDate::currentDate().toString(QString("dd_MM_yyyy")) + QString(" ")
                + QTime::currentTime().toString(QString("hh_mm_ss"));

  QString path = QFileDialog::getSaveFileName
                    (nullptr,"SAVE IMAGE",Data_dir,"*.png");
  return lastFrame->save(path,"PNG");
}

void Core::updateVideoFrame(const QImage& frame_img)
{
   lastFrame = std::make_unique<QImage> (frame_img);
   framePointer = std::make_unique<Core::PXitem>
           (QPixmap::fromImage(frame_img));
   scene.addItem(framePointer.get());
}

void VScene::wheelEvent(QGraphicsSceneWheelEvent * e)
{

}
