#include "core.h"

bool static portisWorking = false;

Core::Core(QObject *parent) : QObject(parent)
{
  //  qDebug() << scene.width() << scene.height();
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
    QString result;
    if (COM1port)
    {
         result = COM1port->MoveXBy(steps);
    }else
    {
         result = QString("port error");
    }
    return result;
}

QString Core::checkPortStatus()
{
    if (COM1port == nullptr)
    {
        return QString("port is closed!");
    }
    if (COM1port->isConnected())
    {
        return QString("port is open and it can write and read data!");
    }else{
        return QString("port is open but it can't write and read data!");
    }
}

void Core::makeCameraCapture()
{
    camPtr = new CamStreamTask;
    m_pool.start(camPtr);
    connect(camPtr,&CamStreamTask::frameAvailable,this,&Core::updateVideoFrame);
}

void Core::stopCameraCapture()
{
    camPtr->onStop();
}

void Core::updateVideoFrame(const QImage &frame_img)
{
   framePointer = std::make_unique<Core::PXitem>
           (QPixmap::fromImage(frame_img));

   scene.addItem(framePointer.get());
}







